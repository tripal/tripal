<?php

class TripalImporter {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Tripal Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'tripal_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'A base loader for all Tripal loaders';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = [];


  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = '';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'File Upload';

  /**
   * If the loader should require an analysis record.  To maintain provenance
   * we should always indicate where the data we are uploading comes from.
   * The method that Tripal attempts to use for this by associating upload files
   * with an analysis record.  The analysis record provides the details for
   * how the file was created or obtained. Set this to FALSE if the loader
   * should not require an analysis when loading. if $use_analysis is set to
   * true then the form values will have an 'analysis_id' key in the $form_state
   * array on submitted forms.
   */
  public static $use_analysis = TRUE;


  /**
   * If the $use_analysis value is set above then this value indicates if the
   * analysis should be required.
   */
  public static $require_analysis = TRUE;

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import File';

  /**
   * If the form submit button that is provided by the Importer is not
   * needed (i.e. the child class wants to do something different). Then
   * set this to FALSE.
   */
  public static $use_button = TRUE;

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  ];

  /**
   * Indicates if the file must be provided.  An example when it may not be
   * necessary to require that the user provide a file for uploading if the
   * loader keeps track of previous files and makes those available for
   * selection.
   */
  public static $file_required = TRUE;


  /**
   * The array of arguments used for this loader.  Each argument should
   * be a separate array containing a machine_name, name, and description
   * keys.  This information is used to build the help text for the loader.
   */
  public static $argument_list = [];


  /**
   * Indicates how many files are allowed to be uploaded.  By default this is
   * set to allow only one file.  Change to any positive number. A value of
   * zero indicates an unlimited number of uploaded files are allowed.
   */
  public static $cardinality = 1;


  /**
   * Be default, all loaders are automaticlly added to the Admin >
   * Tripal > Data Loaders menu.  However, if this loader should be
   * made available via a different menu path, then set it here.  If the
   * value is empty then the path will be the default.
   */
  public static $menu_path = '';


  /**
   * If your importer requires more flexibility and advanced features than
   * the TripalImporter provides, you can indicate a callback function. If set,
   * the callback will be used to provide the importer interface to the
   * end-user.  However, because this bypasses the class infrastructure the
   * run() function will also not be available and your importer must be
   * fully self-sufficient outside of this class.  The benefit for using a
   * TripalImporter despite your loader being self-sufficient is that Tripal
   * will treat your loader like all others providing a consistent location
   * in the menu and set of permissions.
   *
   * Note: use of a callback is discouraged as the importer provides a
   * consistent workflow for all importers.  Try your best to fit your importer
   * within the class.  Use this if you absolutely cannot fit your importer
   * into  TripalImporter implementation.
   *
   */
  public static $callback = '';

  /**
   * The name of the module that provides the callback function.
   */
  public static $callback_module = '';

  /**
   * An include path for the callback function.  Use a relative path within
   * this scope of this module
   * (e.g. includes/loaders/tripal_chado_pub_importers).
   */
  public static $callback_path = '';

  // --------------------------------------------------------------------------
  //                  PRIVATE MEMBERS -- DO NOT EDIT or OVERRIDE
  // --------------------------------------------------------------------------

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
   * funning.
   */
  protected $is_prepared;


  /**
   * Stores the last percentage that progress was reported.
   *
   * @var integer
   */
  protected $reported = 0;

  // --------------------------------------------------------------------------
  //                          CONSTRUCTORS
  // --------------------------------------------------------------------------
  /**
   * Instantiates a new TripalImporter object.
   *
   * @param TripalJob $job
   *   An optional TripalJob object that this loader is associated with.
   */
  public function __construct(TripalJob $job = NULL) {

    // Intialize the private member variables.
    $this->is_prepared = FALSE;
    $this->import_id = NULL;
    $this->arguments = [];
    $this->job = NULL;
    $this->total_items = 0;
    $this->interval = 1;
    $this->num_handled = 0;
    $this->prev_update = 0;
  }

  /**
   * Instantiates a new TripalImporter object using the import record ID.
   *
   * This function will automatically instantiate the correct TripalImporter
   * child class that is appropriate for the provided ID.
   *
   * @param $import_id
   *   The ID of the import recrod.
   *
   * @return
   *   An TripalImporter object of the appropriate child class.
   */
  static public function byID($import_id) {
    // Get the importer.
    $import = db_select('tripal_import', 'ti')
      ->fields('ti')
      ->condition('ti.import_id', $import_id)
      ->execute()
      ->fetchObject();

    if (!$import) {
      throw new Exception('Cannot find an importer that matches the given import ID.');
    }

    $class = $import->class;
    tripal_load_include_importer_class($class);
    if (class_exists($class)) {
      $loader = new $class();
      $loader->load($import_id);
      return $loader;
    }
    else {
      throw new Exception('Cannot find the matching class for this import record.');
    }
  }

  /**
   * Associate this importer with the Tripal job that is running it.
   *
   * Associating an import with a job will allow the importer to log messages
   * to the job log.
   *
   * @param TripalJob $job
   *   An instance of a TripalJob.
   */
  public function setJob(TripalJob $job) {
    $this->job = $job;
  }

  /**
   * Creates a new importer record.
   *
   * @param $run_args
   *   An associative array of the arguments needed to run the importer. Each
   *   importer will have its own defined set of arguments.
   *
   * @param $file_details
   *   An associative array with one of the following keys:
   *   -fid: provides the Drupal managed File ID for the file.
   *   -file_local: provides the full path to the file on the server.
   *   -file_remote: provides the remote URL for the file.
   *   This argument is optional if the loader does not use the built-in
   *   file loader.
   *
   * @throws Exception
   */
  public function create($run_args, $file_details = []) {
    global $user;
    $class = get_called_class();

    try {
      // Build the values for the tripal_importer table insert.
      $values = [
        'uid' => $user->uid,
        'class' => $class,
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
            $file = file_load($fid);
            $arguments['files'][] = [
              'file_path' => drupal_realpath($file->uri),
              'fid' => $fid,
            ];
            $has_file++;
          }
        }
        // Handle a single file.
        else {
          $fid = $file_details['fid'];
          $file = file_load($fid);
          $arguments['files'][] = [
            'file_path' => drupal_realpath($file->uri),
            'fid' => $fid,
          ];
          $has_file++;

          // For backwards compatibility add the old 'file' element.
          $arguments['file'] = [
            'file_path' => drupal_realpath($file->uri),
            'fid' => $fid,
          ];
        }
      }

      // Validate the $file_details argument.
      if ($has_file == 0 and $class::$file_required == TRUE) {
        throw new Exception("Must provide a proper file identifier for the \$file_details argument.");
      }

      // Store the arguments in the class and serialize for table insertion.
      $this->arguments = $arguments;
      $values['arguments'] = serialize($arguments);

      // Insert the importer record.
      $import_id = db_insert('tripal_import')
        ->fields($values)
        ->execute();

      $this->import_id = $import_id;
    } catch (Exception $e) {
      throw new Exception('Cannot create importer: ' . $e->getMessage());
    }
  }

  /**
   * Loads an existing import record into this object.
   *
   * @param $import_id
   *   The ID of the import record.
   */
  public function load($import_id) {
    $class = get_called_class();

    // Get the importer.
    $import = db_select('tripal_import', 'ti')
      ->fields('ti')
      ->condition('ti.import_id', $import_id)
      ->execute()
      ->fetchObject();

    if (!$import) {
      throw new Exception('Cannot find an importer that matches the given import ID.');
    }

    if ($import->class != $class) {
      throw new Exception('The importer specified by the given ID does not match this importer class.');
    }

    $this->arguments = unserialize($import->arguments);
    $this->import_id = $import_id;

  }


  /**
   * Submits the importer for execution as a job.
   *
   * @return
   *   The ID of the newly submitted job.
   */
  public function submitJob() {
    global $user;

    $class = get_called_class();

    if (!$this->import_id) {
      throw new Exception('Cannot submit an importer job without an import record. Please run create() first.');
    }

    // Add a job to run the importer.
    try {
      $args = [$this->import_id];
      $includes = [];
      $job_id = tripal_add_job($class::$button_text, 'tripal',
        'tripal_run_importer', $args, $user->uid, 10, $includes);

      return $job_id;
    } catch (Exception $e) {
      throw new Exception('Cannot create importer job: ' . $e->getMessage());
    }
  }

  /**
   * Prepares the importer files for execution.
   *
   * This function must be run prior to the run() function to ensure that
   * the import file is ready to go.
   */
  public function prepareFiles() {
    $class = get_called_class();

    // If no file is required then just indicate that all is good to go.
    if ($class::$file_required == FALSE) {
      $this->is_prepared = TRUE;
      return;
    }

    try {
      for ($i = 0; $i < count($this->arguments['files']); $i++) {
        if (!empty($this->arguments['files'][$i]['file_remote'])) {
          $file_remote = $this->arguments['files'][$i]['file_remote'];
          $this->logMessage('Download file: !file_remote...', ['!file_remote' => $file_remote]);

          // If this file is compressed then keepthe .gz extension so we can
          // uncompress it.
          $ext = '';
          if (preg_match('/^(.*?)\.gz$/', $file_remote)) {
            $ext = '.gz';
          }
          // Create a temporary file.
          $temp = tempnam("temporary://", 'import_') . $ext;
          $this->logMessage("Saving as: !file", ['!file' => $temp]);

          $url_fh = fopen($file_remote, "r");
          $tmp_fh = fopen($temp, "w");
          if (!$url_fh) {
            throw new Exception(t("Unable to download the remote file at %url. Could a firewall be blocking outgoing connections?",
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
          $this->logMessage("Uncompressing: !file", ['!file' => $this->arguments['files'][$i]['file_path']]);
          $buffer_size = 4096;
          $new_file_path = $matches[1];
          $gzfile = gzopen($this->arguments['files'][$i]['file_path'], 'rb');
          $out_file = fopen($new_file_path, 'wb');
          if (!$out_file) {
            throw new Exception("Cannot uncompress file: new temporary file, '$new_file_path', cannot be created.");
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
    } catch (Exception $e) {
      throw new Exception('Cannot prepare the importer: ' . $e->getMessage());
    }
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
          $this->logMessage('Removing downloaded file...');
          unlink($this->arguments['files'][$i]['file_path']);
          $this->is_prepared = FALSE;
        }
      }
    } catch (Exception $e) {
      throw new Exception('Cannot prepare the importer: ' . $e->getMessage());
    }
  }

  /**
   * Logs a message for the importer.
   *
   * There is no distinction between status messages and error logs.  Any
   * message that is intended for the user to review the status of the loading
   * can be provided here.  If this importer is associated with a job then
   * the logging is passed on to the job for storage.
   *
   * Messages that are are of severity TRIPAL_CRITICAL or TRIPAL_ERROR
   * are also logged to the watchdog.
   *
   * @param $message
   *   The message to store in the log. Keep $message translatable by not
   *   concatenating dynamic values into it! Variables in the message should
   *   be added by using placeholder strings alongside the variables argument
   *   to declare the value of the placeholders. See t() for documentation on
   *   how $message and $variables interact.
   * @param $variables
   *   Array of variables to replace in the message on display or NULL if
   *   message is already translated or not possible to translate.
   * @param $severity
   *   The severity of the message; one of the following values:
   *     - TRIPAL_CRITICAL: Critical conditions.
   *     - TRIPAL_ERROR: Error conditions.
   *     - TRIPAL_WARNING: Warning conditions.
   *     - TRIPAL_NOTICE: Normal but significant conditions.
   *     - TRIPAL_INFO: (default) Informational messages.
   *     - TRIPAL_DEBUG: Debug-level messages.
   */
  public function logMessage($message, $variables = [], $severity = TRIPAL_INFO) {
    // Generate a translated message.
    $tmessage = t($message, $variables);

    // If we have a job then pass along the messaging to the job.
    if ($this->job) {
      $this->job->logMessage($message, $variables, $severity);
    }
    // If we don't have a job then just use the drpual_set_message.
    else {
      // Report this message to watchdog or set a message.
      if ($severity == TRIPAL_CRITICAL or $severity == TRIPAL_ERROR) {
        drupal_set_message($tmessage, 'error');
      }
      if ($severity == TRIPAL_WARNING) {
        drupal_set_message($tmessage, 'warning');
      }
    }
  }

  /**
   * Sets the total number if items to be processed.
   *
   * This should typically be called near the beginning of the loading process
   * to indicate the number of items that must be processed.
   *
   * @param $total_items
   *   The total number of items to process.
   */
  protected function setTotalItems($total_items) {
    $this->total_items = $total_items;
  }

  /**
   * Adds to the count of the total number of items that have been handled.
   *
   * @param $num_handled
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
   * @param $total_handled
   *   The total number of items that have been processed.
   */
  protected function setItemsHandled($total_handled) {
    // First set the number of items handled.
    $this->num_handled = $total_handled;

    if ($total_handled == 0) {
      $memory = number_format(memory_get_usage());
      print t("Percent complete: 0%. Memory: !memory  bytes.", ['!memory' => $memory]) . "\r";
      return;
    }

    // Now see if we need to report to the user the percent done.  A message
    // will be printed on the command-line if the job is run there.
    $percent = ($this->num_handled / $this->total_items) * 100;
    $ipercent = (int) $percent;

    // If we've reached our interval then print update info.
    if ($ipercent > 0 and $ipercent != $this->reported and $ipercent % $this->interval == 0) {
      $memory = number_format(memory_get_usage());
      $spercent = sprintf("%.2f", $percent);
      print t("Percent complete: !percent %. Memory: !memory bytes.",
          ['!percent' => $spercent, '!memory' => $memory]) . "\r";

      // If we have a job the update the job progress too.
      if ($this->job) {
        $this->job->setProgress($percent);
      }
      $this->reported = $ipercent;
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
   * @param $interval
   *   A number between 0 and 100.
   */
  protected function setInterval($interval) {
    $this->interval = $interval;
  }

  // --------------------------------------------------------------------------
  //                     OVERRIDEABLE FUNCTIONS
  // --------------------------------------------------------------------------

  /**
   * Provides form elements to be added to the loader form.
   *
   * These form elements are added after the file uploader section that
   * is automaticaly provided by the TripalImporter.
   *
   * @return
   *   A $form array.
   */
  public function form($form, &$form_state) {
    return $form;
  }

  /**
   * Handles submission of the form elements.
   *
   * The form elements provided in the implementation of the form() function
   * can be used for special submit if needed.
   */
  public function formSubmit($form, &$form_state) {

  }

  /**
   * Handles validation of the form elements.
   *
   * The form elements provided in the implementation of the form() function
   * should be validated using this function.
   */
  public function formValidate($form, &$form_state) {

  }

  /**
   * Performs the import.
   */
  public function run() {
  }

  /**
   * Performs the import.
   */
  public function postRun() {
  }

  /**
   * Retrieves the list of arguments that were provided to the importer.
   *
   * @return
   *   The array of arguments as passed to create function.
   */
  public function getArguments() {
    return $this->arguments;
  }

}
