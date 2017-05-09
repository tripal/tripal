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
  public static $file_types = array();


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
   * we should always indiate where the data we are uploading comes from.
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
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = array(
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => TRUE,
  );

  /**
   * Indicates if the file must be provided.  An example when it may not be
   * necessary to require that the user provide a file for uploading if the
   * loader keeps track of previous files and makes those available for
   * selection.
   */
  public static $file_required = TRUE;


  // --------------------------------------------------------------------------
  //                  PRIVATE MEMBERS -- DO NOT EDIT or OVERRIDE
  // --------------------------------------------------------------------------
  /**
   * The job that this importer is associated with.  This is needed for
   * updating the status of the job.
   */
  private $job = NULL;

  /**
   * The number of items that this importer needs to process. A progress
   * can be calculated by dividing the number of items process by this
   * number.
   */
  private $total_items = 0;

  /**
   * The number of items that have been handled so far.  This must never
   * be below 0 and never exceed $total_items;
   */
  private $num_handled = 0;

  /**
   * The interval when the job progress should be updated. Updating the job
   * progress incurrs a database write which takes time and if it occurs to
   * frequently can slow down the loader.  This should be a value between
   * 0 and 100 to indicate a percent interval (e.g. 1 means update the
   * progress every time the num_handled increases by 1%).
   */
  private $interval = 1;

  /**
   * Each time the job progress is updated this variable gets set.  It is
   * used to calculate if the $interval has passed for the next update.
   */
  private $prev_update = 0;

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
    $this->job = $job;
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
   * Executes tasks that should be run prior to execution of the run() function.
   */
  public function preRun($details) {

  }
  /**
   * Performs the import.
   *
   * This function must be overrriden by a child class to perform the import.
   */
  public function run($details) {
  }
  /**
   * Executes tasks that should be run after execution of the run() function.
   */
  public function postRun($details) {

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
  public function logMessage($message, $variables = array(), $severity = TRIPAL_INFO) {
    // Generate a translated message.
    $tmessage = t($message, $variables);

    // For the sake of the command-line user, print the message to the
    // terminal.
    print $tmessage . "\n";

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
   * Adds to the count of the total number of items that have been handle.d
   * @param unknown $num_handled
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
      print "Percent complete: 0%. Memory: " . $memory . " bytes.\r";
      return;
    }

    // Now see if we need to report to the user the percent done.  A message
    // will be printed on the command-line if the job is run there.
    $percent = sprintf("%.2f", ($this->num_handled / $this->total_items) * 100);
    $diff = $percent - $this->prev_update;

    if ($diff >= $this->interval) {

      $memory = number_format(memory_get_usage());
      print "Percent complete: " . $percent . "%. Memory: " . $memory . " bytes.\r";

      // If we have a job the update the job progress too.
      if ($this->job) {
        $this->job->setProgress($percent);
      }

      $this->prev_update = $diff;
    }
  }

  /**
   * Updates the percent interval when the job progress is updated.
   *
   * Updating the job
   * progress incurrs a database write which takes time and if it occurs to
   * frequently can slow down the loader.  This should be a value between
   * 0 and 100 to indicate a percent interval (e.g. 1 means update the
   * progress every time the num_handled increases by 1%).
   *
   * @param $interval
   *   A number between 0 and 100.
   */
  protected function setInterval($interval) {
    $this->interval = $interval;
  }
}