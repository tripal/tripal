<?php

namespace Drupal\tripal\TripalImporter;

use Drupal\Component\Plugin\PluginBase;
use Drupal\file\Entity\File;
use Drupal\user\Entity\User;
use Drupal\Core\DependencyInjection\DependencySerializationTrait;
use Drupal\Core\Form\FormState;
use Drupal\Core\Messenger\Messenger;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\tripal\Services\TripalFileRetriever;
use Drupal\tripal\Services\TripalLogger;
use Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager;
use Drupal\tripal\TripalImporter\Interfaces\TripalImporterInterface;

/**
 * Defines an interface for tripal impoerter plugins.
 */
abstract class TripalImporterBase extends PluginBase implements TripalImporterInterface {

  use StringTranslationTrait;

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
   * in the Drupal forums or code that this trait will be removed at any point.
   */
  use DependencySerializationTrait;

  /**
   * An instance of the Drupal messenger.
   *
   * @var Drupal\Core\Messenger\Messenger
   */
  protected $messenger = NULL;

  /**
   * The drupal logger for tripal, allowing importers to log messages.
   *
   * @var Drupal\tripal\Services\TripalLogger
   */
  protected $logger = NULL;

  /**
   * An instance of the Tripal file retriever service
   *
   * @var Drupal\tripal\Services\TripalFileRetriever
   */
  protected $fileretriever = NULL;

  /**
   * An instance of the Tripal publish service.
   *
   * @var Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager
   */
  protected $publish_manager = NULL;

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
   * running.
   */
  protected $is_prepared;

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
   * Stores temporary files so we can clean them up after a run.
   *
   * @var array
   */
  protected array $temporary_files = [];

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * @param Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The container.
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin ID for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   *
   * @return static
   */
  public static function create(
    ContainerInterface $container,
    array $configuration,
    $plugin_id,
    $plugin_definition,
  ) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('messenger'),
      $container->get('tripal.logger'),
      $container->get('tripal.fileretriever'),
      $container->get('tripal.backend_publish')
    );
  }

  /**
   * Constructs a TripalImporterBase object.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin ID for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param ?Drupal\Core\Messenger\Messenger
   *   The Drupal messenger service.
   * @param ?Drupal\tripal\Services\TripalLogger
   *   The Tripal logger service.
   * @param ?Drupal\tripal\Services\TripalFileRetriever
   *   The Tripal file retrieval service.
   * @param ?Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager
   *   The Tripal publish manager service.
   */
  public function __construct(
    array $configuration,
    $plugin_id,
    $plugin_definition,
    ?Messenger $messenger = NULL,
    ?TripalLogger $logger = NULL,
    ?TripalFileRetriever $fileretriever = NULL,
    ?TripalBackendPublishManager $publish_manager = NULL,
  ) {
    parent::__construct(
      $configuration,
      $plugin_id,
      $plugin_definition
    );

    // Initialize the private member variables.
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

    // Initialize the Drupal messenger
    if ($messenger === NULL) {
      @trigger_error('Calling ' . __METHOD__ . '() without the $messenger argument is deprecated in tripal 4.0.0-alpha3 and it will be required in tripal:4.1.0. To resolve this, make sure the create() method in your importer grabs the Drupal\Core\Messenger\Messenger from the container and supplies it to the parent in the constructor. See https://tripaldoc.readthedocs.io/en/latest/dev_guide/deprecations/tripalimporter_dev_inj_constructor.html', E_USER_DEPRECATED);
      $messenger = \Drupal::messenger();
    }
    $this->messenger = $messenger;

    // Initialize the Tripal logger.
    if ($logger === NULL) {
      @trigger_error('Calling ' . __METHOD__ . '() without the $logger argument is deprecated in tripal 4.0.0-alpha3 and it will be required in tripal:4.1.0. To resolve this, make sure the create() method in your importer grabs the Drupal\tripal\Services\TripalLogger from the container and supplies it to the parent in the constructor. See https://tripaldoc.readthedocs.io/en/latest/dev_guide/deprecations/tripalimporter_dev_inj_constructor.html', E_USER_DEPRECATED);
      $logger = \Drupal::service('tripal.logger');
    }
    $this->logger = $logger;

    // Initialize file retrieval service.
    if ($fileretriever === NULL) {
      @trigger_error('Calling ' . __METHOD__ . '() without the $fileretriever argument is deprecated in tripal 4.0.0-alpha3 and it will be required in tripal:4.1.0. To resolve this, make sure the create() method in your importer grabs the Drupal\tripal\Services\TripalFileRetriever from the container and supplies it to the parent in the constructor. See https://tripaldoc.readthedocs.io/en/latest/dev_guide/deprecations/tripalimporter_dev_inj_constructor.html', E_USER_DEPRECATED);
      $fileretriever = \Drupal::service('tripal.fileretriever');
    }
    $this->fileretriever = $fileretriever;

    // Initialize the publish manager.
    if ($publish_manager === NULL) {
      @trigger_error('Calling ' . __METHOD__ . '() without the $publish_manager argument is deprecated in tripal 4.0.0-alpha3 and it will be required in tripal:4.1.0. To resolve this, make sure the create() method in your importer grabs the Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager from the container and supplies it to the parent in the constructor. See https://tripaldoc.readthedocs.io/en/latest/dev_guide/deprecations/tripalimporter_dev_inj_constructor.html', E_USER_DEPRECATED);
      $publish_manager = \Drupal::service('tripal.backend_publish');
    }
    $this->publish_manager = $publish_manager;
  }

  /**
   * Returns a list of supported file extensions.
   *
   * If the PHP zlib extension is installed, we transparently support
   * appending ".gz" to supported file extensions.
   * Drupal does not support compound extensions, so we use just
   * "gz" for those and validate later if $drupal_compatible is set.
   *
   * @param bool $drupal_compatible
   *   Default FALSE, if TRUE, compound extensions are reduced to the
   *   last part, e.g. 'gff3.gz' becomes 'gz'.
   *
   * @return array
   *   The list of supported file extensions.
   */
  public function getFileTypes(bool $drupal_compatible = FALSE) {
    $file_types = $this->plugin_definition['file_types'];
    $file_seekable = $this->plugin_definition['file_seekable'] ?? FALSE;
    if (extension_loaded('zlib') && !$file_seekable) {
      if ($drupal_compatible) {
        $file_types[] = 'gz';
      }
      else {
        $updated_types = [];
        foreach ($file_types as $file_type) {
          $updated_types[] = $file_type;
          $updated_types[] = $file_type . '.gz';
        }
        $file_types = $updated_types;
      }
    }
    return $file_types;
  }

  /**
   * Validates that a file is of an allowed type by its file extension.
   *
   * @param string $file_name
   *   The file name, can be local or remote.
   * @param string $form_element
   *   The name of the form field supplying the file name.
   * @param Drupal\Core\Form\FormState|null $form_state
   *   The state of the importer form.
   *
   * @return bool
   *   TRUE if valid, FALSE in non-supported file extension.
   *   Also sets form error if not valid.
   *   An empty $file_name also returns TRUE.
   */
  public function validateFile(string $file_name, string $form_element = '', ?FormState $form_state = NULL): bool {
    $valid = TRUE;
    if ($file_name) {
      $valid = FALSE;
      $file_types = $this->getFileTypes();
      foreach ($file_types as $file_type) {
        if (preg_match('/' . $file_type . '$/i', $file_name)) {
          $valid = TRUE;
          break;
        }
      }
    }
    if (!$valid && $form_state) {
      $form_state->setErrorByName($form_element, $this->t('The file "@file_name" is not a valid type for this importer.',
        ['@file_name' => $file_name]));
    }
    return $valid;
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
    $file_types = $this->getFileTypes();
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
    $this->logger->setGlobalOption('logger', FALSE);
  }

  /**
   * Creates a database transaction in the specific schema(s) this importer will
   * be importing data into.
   *
   * @return array
   *   An array of Drupal DatabaseTransaction objects. These are usually
   *   obtained by calling the startTransaction() method on the database
   *   connection object.
   */
  public function startTransactions() {
    $transactions = [];

    // By default the Tripal importer returns a single transaction
    // focused on the Drupal schema. This is not usually what we want
    // as when a transaction is rolled back on the Drupal schema during import
    // we lose the Tripal job status updates and logs.
    $public = \Drupal::database();
    $transactions[] = $public->startTransaction();

    return $transactions;
  }

  /**
   * Clean-up anything related to this import in case of error.
   *
   * Called when an exception is caught during run() or postRun().
   * NOTE: This is called after the transaction on the current database
   * is rolled back. If you want to rollback all changes in multiple Drupal-managed
   * connections then add each one via startTransaction(). This should only be
   * needed to perform partial clean-up or when importing into non Drupal-managed
   * connections.
   *
   * @param string $stage
   *   A string indicating where this method was called from.
   *   Expected to be one of 'run' or 'postRun'.
   */
  public function rollbackTransaction(string $stage) { }

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

    // Currently user is non-default only if run by drush.
    $uid = $run_args['uid'] ?? \Drupal::currentUser()->id();

    try {
      // Build the values for the tripal_importer table insert.
      $values = [
        'uid' => $uid,
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
      $this->setArguments($arguments);
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
    $this->setArguments(unserialize(base64_decode($import->arguments)));
    $this->import_id = $import_id;

  }


  /**
   * Submits the importer for execution as a job.
   *
   * @param int|null $uid
   *   The ID of the user submitting the job.
   *
   * @return int
   *   The ID of the newly submitted job.
   */
  public function submitJob(?int $uid = NULL) {
    if (!$uid) {
      $uid = \Drupal::currentUser()->id();
    }

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
   * the import file is ready to go, i.e. it is downloaded and if necessary
   * it has been uncompressed.
   */
  public function prepareFiles() {
    try {
      for ($i = 0; $i < count($this->arguments['files']); $i++) {

        // Retrieve a remote file.
        if (!empty($this->arguments['files'][$i]['file_remote'])) {
          $file_remote = $this->arguments['files'][$i]['file_remote'];
          $this->logger->notice('Download file: %file_remote...', ['%file_remote' => $file_remote]);

          // If this file is compressed then keep the .gz extension so we
          // know to uncompress it later.
          $ext = '';
          if (preg_match('/^(.*?)\.gz$/', $file_remote)) {
            $ext = '.gz';
          }
          // Create a temporary file.
          $fs_service = \Drupal::service('file_system');
          $temp_uri = $fs_service->tempnam('temporary://', 'import_');
          if ($ext) {
            $fs_service->move($temp_uri, $temp_uri . $ext);
            $temp_uri .= $ext;
          }
          $temp_file_path = $fs_service->realpath($temp_uri);
          $this->logger->notice('Saving as: %file', ['%file' => $temp_file_path]);

          // Store in the list of files to clean up when finished.
          $this->temporary_files[] = $temp_file_path;

          // Download the remote file contents to a local temporary file.
          $status = $this->fileretriever->downloadFile($file_remote, $temp_file_path);
          if (!$status) {
            $this->is_prepared = FALSE;
            return;
          }

          // Set the path to the local temporary file for the importer to use.
          $this->arguments['files'][$i]['file_path'] = $temp_file_path;
        }

        // Uncompress a downloaded or uploaded file if the importer requests it.
        if (!empty($this->arguments['files'][$i]['file_path'])) {
          $file_path = $this->arguments['files'][$i]['file_path'];
          $this->arguments['files'][$i]['file_path'] = $this->uncompressFile($file_path);
        }

        // Uncompress a local file if the importer requests it.
        if (!empty($this->arguments['files'][$i]['file_local'])) {
          $file_local = $this->arguments['files'][$i]['file_local'];
          $this->arguments['files'][$i]['file_local'] = $this->uncompressFile($file_local);
        }

      }
    }
    catch (\Exception $e) {
      throw new \Exception('Cannot prepare the importer: ' . $e->getMessage());
    }

    // If we get here and no exception has been thrown then either:
    // 1) files were added but none needed to be prepared.
    // 2) files were not added (check for files being required happens
    //    elsewhere).
    $this->is_prepared = TRUE;

  }

  /**
   * Creates a temporary uncompressed version of a compressed file.
   *
   * If the supplied file is not compressed, that file path is
   * returned unmodified. The flag that indicates if the importer only
   * supports uncompressed files is stored as the importer's 'file_seekable'
   * definition value. This will be set when the importer uses fseek(),
   * because fseek() cannot be used on compressed files.
   *
   * @param string $compressed_file_path
   *   A file that may or may not be compressed.
   *
   * @return string
   *   If the file was compressed, a path to a temporary uncompressed
   *   copy of that file. It the file was not compressed, the input
   *   file that will be returned.
   *
   * @throws \Exception
   *   Exception if temporary output file cannot be created.
   */
  protected function uncompressFile(string $compressed_file_path): string {
    $uncompressed_file_path = $compressed_file_path;

    // This specifies if compressed files need to be uncompressed.
    $need_to_uncompress = $this->plugin_definition['file_seekable'] ?? FALSE;

    // Determine if file is compressed, and if so, uncompress.
    if ($need_to_uncompress && preg_match('/^(.*?)\.gz$/', $compressed_file_path)) {
      $fs_service = \Drupal::service('file_system');
      $temp = $fs_service->tempnam('temporary://', 'import_');
      $uncompressed_file_path = $fs_service->realpath($temp);

      // Store in the list of files to clean up when finished.
      $this->temporary_files[] = $uncompressed_file_path;

      $this->logger->notice('Uncompressing: %file', ['%file' => $compressed_file_path]);
      $buffer_size = 4096;
      $gzfile = gzopen($compressed_file_path, 'rb');
      $out_file = fopen($uncompressed_file_path, 'wb');
      if (!$out_file) {
        throw new \Exception("Cannot uncompress file '$compressed_file_path': new temporary file, '$uncompressed_file_path', cannot be created.");
      }

      // Keep repeating until the end of the input file.
      while (!gzeof($gzfile)) {
        // Read buffer-size bytes.
        // Both fwrite and gzread are binary-safe.
        fwrite($out_file, gzread($gzfile, $buffer_size));
      }

      // Files are done, close files.
      fclose($out_file);
      gzclose($gzfile);
    }
    return $uncompressed_file_path;
  }

  /**
   * Cleans up any temporary files that were created by the prepareFile().
   *
   * This function should be called after a run() to remove any temporary
   * files and keep them from building up on the server.
   */
  public function cleanFile() {
    try {
      foreach ($this->temporary_files as $temporary_file) {
        if (file_exists($temporary_file)) {
          $this->logger->notice('Removing temporary file "' . $temporary_file . '"');
          unlink($temporary_file);
        }
      }
    }
    catch (\Exception $e) {
      throw new \Exception('Cannot remove importer temporary file "' . $temporary_file . '": ' . $e->getMessage());
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

      // If we have a job then update the job progress too.
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

      // If we have a job then update the job progress too.
      if ($this->job) {
        $this->job->setProgress(100);
      }
      $this->reported = 100;
    }
  }

  /**
   * Performs tasks after the importer has completed.
   *
   * @return void
   *   No return value.
   */
  public function postRun() {
    $this->cleanFile();
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
   * Stores a set of arguments for the importer
   *
   * @param array $arguments
   *   Associative array
   */
  public function setArguments(array $arguments) {
    $this->arguments = $arguments;
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

  /**
   * Validates whether XML is valid or not.
   *
   * @param string $xml
   *   The XML to be checked.
   *
   * @return bool
   *   Return TRUE if valid, FALSE if not valid.
   *
   * @see Drupal\tripal\TripalPubLibrary\TripalPubLibraryBase::xmlIsValid().
   */
  protected function xmlIsValid(string $xml): bool {
    $valid = TRUE;

    // Enable user handling of errors so that exceptions are not
    // thrown when invalid XML is read.
    libxml_use_internal_errors(TRUE);
    // Attempt to load the XML.
    $doc = simplexml_load_string($xml);
    // If SimpleXML fails to parse the XML string then it will return FALSE.
    if ($doc === FALSE) {
      $valid = FALSE;
    }
    // If not, we will check for any errors logged during parsing.
    else {
      $errors = libxml_get_errors();
      if (!empty($errors)) {
        $valid = FALSE;
      }
    }
    return $valid;
  }

  /**
   * Transparently allows reading from gz compressed files.
   *
   * If the file name ends with a ".gz" extension, then include the
   * zlib stream wrapper. All later file operation function calls
   * can then seamlessly work with this compressed file.
   *
   * Note that if reading a compressed file, then fseek() cannot be used.
   *
   * @param string $filename
   *   The system path of the file.
   * @param string $mode
   *   The type of access to the stream, e.g. 'r' to read.
   * @param bool $use_include_path
   *   Set to true if you want to search for the file in the include_path.
   * @param resource|null $context
   *   A context stream resource.
   *
   * @return resource|bool
   *   The opened file handle, or FALSE if file was not opened.
   */
  protected function tripalFopen(
    string $filename,
    string $mode,
    bool $use_include_path = FALSE,
    $context = NULL,
  ) {
    if (preg_match('/\.gz$/', $filename) && !preg_match('/compress.zlib:\/\//', $filename)) {
      $filename = 'compress.zlib://' . $filename;
    }
    $fh = fopen($filename, $mode, $use_include_path, $context);
    return $fh;
  }

}
