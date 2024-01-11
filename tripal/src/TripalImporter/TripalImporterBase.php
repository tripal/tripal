<?php

namespace Drupal\tripal\TripalImporter;

use Drupal\tripal\TripalImporter\Interfaces\TripalImporterInterface;
use Drupal\Component\Plugin\PluginBase;
use Drupal\file\Entity\File;
use Drupal\user\Entity\User;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;
use Drupal\Core\DependencyInjection\DependencySerializationTrait;

/**
 * Defines an interface for tripal impoerter plugins.
 */
abstract class TripalImporterBase extends PluginBase implements TripalImporterInterface {

  /**
   * Needed to allow AJAX on TripalImporter forms once Dependency injection is used.
   *
   * The error message implies that the log exception this trait is needed to
   * solve is caused by the form serializing an object that has an indirect
   * reference to the database connection (TripalImporter) and that we should
   * adjust your code so that is not necessary.
   *
   * That said, the TripalImporterForm does not appear to save the TripalImporter
   * object in the form or form state at any point. Instead it only uses
   * the importer object to get strings and arrays that are incorporated
   * into the form.
   *
   * Anyway, using this trait solves the problem and although the error
   * mentions this should be a temporary solution, there are no mentioned plans
   * in the Drupal forumns or code that this trait will be removed at any point.
   */
  use DependencySerializationTrait;

  /**
   * The number of items that this importer needs to process. A progress
   * can be calculated by dividing the number of items process by this
   * number.
   */
  private $total_items;

  /**
   * The number of items that have been handled so far.  This must never
   * be below 0 and never exceed $total_items;
   */
  private $num_handled;

  /**
   * The interval when the job progress should be updated. Updating the job
   * progress incurrs a database write which takes time and if it occurs to
   * frequently can slow down the loader.  This should be a value between
   * 0 and 100 to indicate a percent interval (e.g. 1 means update the
   * progress every time the num_handled increases by 1%).
   */
  private $interval;

  /**
   * Each time the job progress is updated this variable gets set.  It is
   * used to calculate if the $interval has passed for the next update.
   */
  private $prev_update;

  /**
   * The job that this importer is associated with.  This is needed for
   * updating the status of the job.
   */
  protected $job;

  /**
   * The drupal logger for tripal. This allows any of the importers to
   * send log messages.
   */
  protected $logger;

  /**
   * The arguments needed for the importer. This is a list of key/value
   * pairs in an associative array.
   */
  protected $arguments;

  /**
   * The ID for this import record.
   */
  protected $import_id;

  /**
   * Prior to running an importer it must be prepared to make sure the file
   * is available.  Preparing the importer will download all the necessary
   * files.  This value is set to TRUE after the importer is prepared for
   * funning.
   */
  protected $is_prepared;

  /**
   * An instance of the Drupal messenger.
   *
   * @var object \Drupal\Core\Messenger\Messenger
   */
  protected $messenger = NULL;

  /**
   * Stores the last percentage that progress was reported.
   *
   * @var integer
   */
  protected $reported;

  /**
   * The ID of this plugin.
   *
   * @var string
   */
  protected $plugin_id;

  /**
   * The plugin definition
   *
   * @var array
   */
  protected $plugin_definition;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    // Intialize the private member variables.
    $this->plugin_id = $plugin_id;
    $this->plugin_definition = $plugin_definition;
    $this->is_prepared = FALSE;
    $this->import_id = NULL;
    $this->arguments = [];
    $this->job = NULL;
    $this->total_items = 0;
    $this->interval = 1;
    $this->num_handled = 0;
    $this->prev_update = 0;
    $this->reported = 0;


    // Initialize the logger.
    $this->logger = \Drupal::service('tripal.logger');

    // Initialize messenger
    $this->messenger = \Drupal::messenger();

  }

  /**
   * Provide more informative description than is ideal in the annotation alone.
   *
   * NOTE: Supports full HTML.
   *
   * @return
   *   A fully formatted string describing the format of the file to be uploaded
   *   and providing any additional upload file information.
   */
  public function describeUploadFileFormat() {
    $default_description = $this->plugin_definition['upload_description'];
    $file_types = $this->plugin_definition['file_types'];
    return $default_description . ' The following file extensions are supported: ' . implode(', ', $file_types) . '.';
  }

   /**
   * Associate this importer with the Tripal job that is running it.
   *
   * Associating an import with a job will allow the importer to log messages
   * to the job log.
   *
   * @param \Drupal\tripal\Services\TripalJob $job
   *   An instance of a TripalJob.
   */
  public function setJob($job) {
    $this->job = $job;
    $this->logger->setJob($job);
  }

  /**
   * Creates a new importer record.
   *
   * @param array $run_args
   *   An associative array of the arguments needed to run the importer. Each
   *   importer will have its own defined set of arguments.
   *
   * @param array $file_details
   *   An associative array with one of the following keys:
   *   -fid: provides the Drupal managed File ID for the file.
   *   -file_local: provides the full path to the file on the server.
   *   -file_remote: provides the remote URL for the file.
   *   This argument is optional if the loader does not use the built-in
   *   file loader.
   * @return int
   *   Returns the import_id.
   */
  public function createImportJob($run_args, $file_details = []) {

    // global $user;
    $user = User::load(\Drupal::currentUser()->id());

    try {
      // Build the values for the tripal_importer table insert.
      $values = [
        'uid' => $user->get('uid')->value,
        'class' => $this->plugin_id,
        'submit_date' => time(),
      ];

      // Build the arguments array, which consists of the run arguments
      // and the file.
      $arguments = [
        'run_args' => $run_args,
        'files' => [],
      ];

      // Get the file argument.
      $has_file = 0;
      if (array_key_exists('file_local', $file_details)) {
        $arguments['files'][] = [
          'file_local' => $file_details['file_local'],
          'file_path' => $file_details['file_local'],
        ];
        $has_file++;
      }
      if (array_key_exists('file_remote', $file_details)) {
        $arguments['files'][] = [
          'file_remote' => $file_details['file_remote'],
        ];
        $has_file++;
      }
      if (array_key_exists('fid', $file_details)) {
        $values['fid'] = $file_details['fid'];
        // Handle multiple file uploads.
        if (preg_match('/\|/', $file_details['fid'])) {
          $fids = explode('|', $file_details['fid']);
          foreach ($fids as $fid) {
            $file = File::load($fid);
            $arguments['files'][] = [
              'file_path' => \Drupal::service('file_system')->realpath($file->getFileUri()),
              'fid' => $fid,
            ];
            $has_file++;
          }
        }
        // Handle a single file.
        else {
          $fid = $file_details['fid'];
          $file = File::load($fid);
          $arguments['files'][] = [
            'file_path' => \Drupal::service('file_system')->realpath($file->getFileUri()),
            'fid' => $fid,
          ];
          $has_file++;

          // For backwards compatibility add the old 'file' element.
          $arguments['file'] = [
            'file_path' => \Drupal::service('file_system')->realpath($file->getFileUri()),
            'fid' => $fid,
          ];
        }
      }

      // Validate the $file_details argument.
      if ($has_file == 0 and $this->plugin_definition['file_required'] == TRUE) {
        throw new \Exception("Must provide a proper file identifier for the \$file_details argument.");
      }

      // Store the arguments in the class and serialize for table insertion.
      $this->arguments = $arguments;
      $values['arguments'] = base64_encode(serialize($arguments));

      // Insert the importer record.
      $public = \Drupal::database();
      $import_id = $public->insert('tripal_import')
        ->fields($values)
        ->execute();

      $this->import_id = $import_id;
      return $import_id;
    }
    catch (\Exception $e) {
      throw new \Exception('Cannot create importer: ' . $e->getMessage());
    }
  }

  /**
   * Loads an existing import record into this object.
   *
   * @param int $import_id
   *   The ID of the import record.
   */
  public function load($import_id) {
    $public = \Drupal::database();
    // Get the importer.
    $import = $public->select('tripal_import', 'ti')
      ->fields('ti')
      ->condition('ti.import_id', $import_id)
      ->execute()
      ->fetchObject();

    if (!$import) {
      throw new \Exception('Cannot find an importer that matches the given import ID.');
    }

    if ($import->class != $this->plugin_id) {
      throw new \Exception('The importer specified by the given ID does not match this importer class.');
    }

    //$this->arguments = unserialize($import->arguments);
    $this->arguments = unserialize(base64_decode($import->arguments));
    $this->import_id = $import_id;

  }


  /**
   * Submits the importer for execution as a job.
   *
   * @return int
   *   The ID of the newly submitted job.
   */
  public function submitJob() {
    $user = \Drupal::currentUser();
    $uid = $user->id();

    if (!$this->import_id) {
      throw new \Exception('Cannot submit an importer job without an import record. Please run createImportJob() first.');
    }

    // Add a job to run the importer.
    try {
      $args = [$this->import_id];
      $job_id = \Drupal::service('tripal.job')->create([
        'job_name' => $this->plugin_definition['button_text'],
        'modulename' => 'tripal',
        'callback' => 'tripal_run_importer',
        'arguments' => $args,
        'uid' => $uid
      ]);

      return $job_id;
    }
    catch (\Exception $e) {
      throw new \Exception('Cannot create importer job: ' . $e->getMessage());
    }
  }

  /**
   * Prepares the importer files for execution.
   *
   * This function must be run prior to the run() function to ensure that
   * the import file is ready to go.
   */
  public function prepareFiles() {

    try {
      for ($i = 0; $i < count($this->arguments['files']); $i++) {
        if (!empty($this->arguments['files'][$i]['file_remote'])) {
          $file_remote = $this->arguments['files'][$i]['file_remote'];
          $this->logger->notice('Download file: %file_remote...', ['%file_remote' => $file_remote]);

          // If this file is compressed then keep the .gz extension so we can
          // uncompress it.
          $ext = '';
          if (preg_match('/^(.*?)\.gz$/', $file_remote)) {
            $ext = '.gz';
          }
          // Create a temporary file.
          $temp = \Drupal::service('file_system')->tempnam("temporary://", 'import_') . $ext;
          $this->logger->notice('Saving as: %file', ['%file' => $temp]);

          $url_fh = fopen($file_remote, "r");
          $tmp_fh = fopen($temp, "w");
          if (!$url_fh) {
            throw new \Exception(t("Unable to download the remote file at %url. Could a firewall be blocking outgoing connections?",
              ['%url', $file_remote]));
          }

          // Write the contents of the remote file to the temp file.
          while (!feof($url_fh)) {
            fwrite($tmp_fh, fread($url_fh, 255), 255);
          }
          // Set the path to the file for the importer to use.
          $this->arguments['files'][$i]['file_path'] = $temp;
          $this->is_prepared = TRUE;
        }

        // Is this file compressed?  If so, then uncompress it
        $matches = [];
        if (preg_match('/^(.*?)\.gz$/', $this->arguments['files'][$i]['file_path'], $matches)) {
          $this->logger->notice('Uncompressing: %file', ['%file' => $this->arguments['files'][$i]['file_path']]);
          $buffer_size = 4096;
          $new_file_path = $matches[1];
          $gzfile = gzopen($this->arguments['files'][$i]['file_path'], 'rb');
          $out_file = fopen($new_file_path, 'wb');
          if (!$out_file) {
            throw new \Exception("Cannot uncompress file: new temporary file, '$new_file_path', cannot be created.");
          }

          // Keep repeating until the end of the input file
          while (!gzeof($gzfile)) {
            // Read buffer-size bytes
            // Both fwrite and gzread and binary-safe
            fwrite($out_file, gzread($gzfile, $buffer_size));
          }

          // Files are done, close files
          fclose($out_file);
          gzclose($gzfile);

          // Now remove the .gz file and reset the file_path to the new
          // uncompressed version.
          unlink($this->arguments['files'][$i]['file_path']);
          $this->arguments['files'][$i]['file_path'] = $new_file_path;
        }
      }
    }
    catch (\Exception $e) {
      throw new \Exception('Cannot prepare the importer: ' . $e->getMessage());
    }


    // If we get here and no exception has been thrown then either
    // 1) files were added but none needed to be prepared.
    // 2) files were not added (check for files being required happens elsewhere).
    $this->is_prepared = TRUE;

  }

  /**
   * Cleans up any temporary files that were created by the prepareFile().
   *
   * This function should be called after a run() to remove any temporary
   * files and keep them from building up on the server.
   */
  public function cleanFile() {
    try {
      // If a remote file was downloaded then remove it.
      for ($i = 0; $i < count($this->arguments['files']); $i++) {
        if (!empty($this->arguments['files'][$i]['file_remote']) and
          file_exists($this->arguments['files'][$i]['file_path'])) {
          $this->logger->notice('Removing downloaded file...');
          unlink($this->arguments['files'][$i]['file_path']);
          $this->is_prepared = FALSE;
        }
      }
    }
    catch (\Exception $e) {
      throw new \Exception('Cannot prepare the importer: ' . $e->getMessage());
    }
  }

  /**
   * Sets the total number if items to be processed.
   *
   * This should typically be called near the beginning of the loading process
   * to indicate the number of items that must be processed.
   *
   * @param int $total_items
   *   The total number of items to process.
   */
  protected function setTotalItems($total_items) {
    $this->total_items = $total_items;
  }

  /**
   * Adds to the count of the total number of items that have been handled.
   *
   * @param int $num_handled
   */
  protected function addItemsHandled($num_handled) {
    $items_handled = $this->num_handled = $this->num_handled + $num_handled;
    $this->setItemsHandled($items_handled);
  }

  /**
   * Sets the number of items that have been processed.
   *
   * This should be called anytime the loader wants to indicate how many
   * items have been processed.  The amount of progress will be
   * calculated using this number.  If the amount of items handled exceeds
   * the interval specified then the progress is reported to the user.  If
   * this loader is associated with a job then the job progress is also updated.
   *
   * @param int $total_handled
   *   The total number of items that have been processed.
   */
  protected function setItemsHandled($total_handled) {
    // First set the number of items handled.
    $this->num_handled = $total_handled;

    if ($total_handled == 0) {
      $memory = number_format(memory_get_usage());
      $this->logger->notice(t("Percent complete: 0%. Memory: " . $memory . " bytes.") . "\r");
      return;
    }

    // Now see if we need to report to the user the percent done.  A message
    // will be printed on the command-line if the job is run there.
    if ($this->total_items) {
      $percent = ($this->num_handled / $this->total_items) * 100;
      $ipercent = (int) $percent;
    }

    // If we've reached our interval then print update info.
    if ($ipercent > 0 and $ipercent != $this->reported and $ipercent % $this->interval == 0) {
      $memory = number_format(memory_get_usage());
      $spercent = sprintf("%.2f", $percent);
      $this->logger->notice(
        t("Percent complete: " . $spercent . " %. Memory: " . $memory . " bytes.")
         . "\r"
      );

      // If we have a job the update the job progress too.
      if ($this->job) {
        $this->job->setProgress($percent);
      }
      $this->reported = $ipercent;
    }

    // If we're done then indicate so.
    if ($this->num_handled >= $this->total_items) {
      $memory = number_format(memory_get_usage());
      $spercent = sprintf("%.2f", 100);
      $this->logger->notice(
        t("Percent complete: " . $spercent . " %. Memory: " . $memory . " bytes.")
         . "\r"
      );

      // If we have a job the update the job progress too.
      if ($this->job) {
        $this->job->setProgress(100);
      }
      $this->reported = 100;
    }
  }

  /**
   * Updates the percent interval when the job progress is updated.
   *
   * Updating the job progress incurrs a database write which takes time
   * and if it occurs to frequently can slow down the loader.  This should
   * be a value between 0 and 100 to indicate a percent interval (e.g. 1
   * means update the progress every time the num_handled increases by 1%).
   *
   * @param int $interval
   *   A number between 0 and 100.
   */
  protected function setInterval($interval) {
    $this->interval = $interval;
  }

  /**
   * Retrieves the list of arguments that were provided to the importer.
   *
   * @return array
   *   The array of arguments as passed to create function.
   */
  public function getArguments() {
    return $this->arguments;
  }

}
