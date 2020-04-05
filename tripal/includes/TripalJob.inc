<?php

class TripalJob {

  /**
   * The ID of the job.
   */
  protected $job_id = NULL;

  /**
   * Contains the job record for this job.
   */
  protected $job = NULL;


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
   * progress incurrs a database write which takes time and if it occurs too
   * frequently can slow down the loader.  This should be a value between
   * 0 and 100 to indicate a percent interval (e.g. 1 means update the
   * progress every time the num_handled increases by 1%).
   */
  private $interval;

  /**
   * The time stamp when the job begins.
   *
   * @var integer
   */
  private $start_time;

  /**
   * The time from when the setTotalItems is called to the present time.
   *
   * @var
   */
  private $progress_start_time;

  /**
   * Stores the last percentage that progress was reported.
   *
   * @var integer
   */
  private $reported = 0;

  /**
   * Instantiates a new TripalJob object.
   *
   * By default the job object is "empty". It must be associated with
   * job details either by calling the load() function or the
   * create() function.
   */
  public function __construct() {
  }

  /**
   * Loads a job for this object.
   *
   * @param $job_id
   *   The ID of the job.
   */
  public function load($job_id) {

    // Make sure we have a numeric job_id.
    if (!$job_id or !is_numeric($job_id)) {
      // If we don't then do a quick double check in case this is a
      // TripalJob object in which case, I still have the job_id.
      if (is_object($job_id) AND is_a($job_id, 'TripalJob')) {
        $job_id = $job_id->job->job_id;
      }
      // Finally just throw an exception.
      // I can't load a job if I don't know which one.
      else {
        throw new Exception("You must provide the job_id to load the job.");
      }
    }

    $sql = 'SELECT j.* FROM {tripal_jobs} j WHERE j.job_id = :job_id';
    $args = [':job_id' => $job_id];
    $this->job = db_query($sql, $args)->fetchObject();
    if (!$this->job) {
      throw new Exception("Cannot find a job with this ID provided.");
    }

    // Fix the date/time fields.
    $this->job->submit_date_string = $this->job->submit_date ? format_date($this->job->submit_date) : '';
    $this->job->start_time_string = $this->job->start_time ? format_date($this->job->start_time) : '';
    $this->job->end_time_string = $this->job->end_time ? format_date($this->job->end_time) : '';

    // Unserialize the includes.
    $this->job->includes = unserialize($this->job->includes);

    // Arguments for jobs used to be stored as plain string with a double colon
    // separating them.  But as of Tripal v2.0 the arguments are stored as
    // a serialized array.  To be backwards compatible, we should check for
    // serialization and if not then we will use the old style
    $this->job->arguments = unserialize($this->job->arguments);
    if (!is_array($this->job->arguments)) {
      $this->job->arguments = explode("::", $this->job->arguments);
    }

  }

  /**
   * Creates a new job.
   *
   * @param $details
   *   An associative array of the job details or a single job_id.  If the
   *   details are provided then the job is created and added to the database
   *   otherwise if a job_id is provided then the object is loaded from the
   *   database.  The following keys are allowed:
   *   - job_name: The human readable name for the job.
   *   - modulename: The name of the module adding the job.
   *   - callback: The name of a function to be called when the job is executed.
   *   - arguments:  An array of arguments to be passed on to the callback.
   *   - uid: The uid of the user adding the job
   *   - priority: The priority at which to run the job where the highest
   *     priority is 10 and the lowest priority is 1. The default
   *     priority is 10.
   *   - includes: An array of paths to files that should be included in order
   *     to execute the job. Use the module_load_include function to get a path
   *     for a given file.
   *   - ignore_duplicate: (Optional). Set to TRUE to ignore a job if it has
   *     the same name as another job which has not yet run. If TRUE and a job
   *     already exists then this object will reference the job already in the
   *     queue rather than a new submission.  The default is TRUE.
   *
   * @throws Exception
   *   On failure an exception is thrown.
   *
   * @return
   *   Returns TRUE if the job was succesfully created. Returns FALSE otherwise.
   *   A return of FALSE does not mean the job creation failed. If the
   *   ignore_duplicate is set to false and the job already is present in the
   *   queue then the return value will be FALSE.
   */
  public function create($details) {

    // Set some defaults
    if (!array_key_exists('prority', $details)) {
      $details['priority'] = 10;
    }
    if (!array_key_exists('includes', $details)) {
      $details['includes'] = [];
    }
    if (!array_key_exists('ignore_duplicate', $details)) {
      $details['ignore_duplicate'] = FALSE;
    }

    // Make sure the arguments are correct.
    if (!$details['job_name']) {
      throw new Exception("Must provide a 'job_name' to create a job.");
    }
    if (!$details['modulename']) {
      throw new Exception("Must provide a 'modulename' to create a job.");
    }
    if (!$details['callback']) {
      throw new Exception("Must provide a 'callback' to create a job.");
    }
    if ($details['ignore_duplicate'] !== FALSE and $details['ignore_duplicate'] !== TRUE) {
      throw new Exception("Must provide either TRUE or FALSE for the ignore_duplicate option when creating a job.");
    }

    $includes = $details['includes'];
    if ($includes and is_array($includes)) {
      foreach ($includes as $path) {
        $full_path = $_SERVER['DOCUMENT_ROOT'] . base_path() . $path;
        if (!empty($path)) {
          if (file_exists($path)) {
            require_once($path);
          }
          elseif (file_exists($full_path)) {
            require_once($path);
          }
          elseif (!empty($path)) {
            throw new Exception("Included files for Tripal Job must exist. This path ($full_path) doesn't exist.");
          }
        }
      }
    }
    if (!function_exists($details['callback'])) {
      throw new Exception("Must provide a valid callback function to the tripal_add_job() function.");
    }
    if (!is_numeric($details['uid'])) {
      throw new Exception("Must provide a numeric \$uid argument to the tripal_add_job() function.");
    }
    $priority = $details['priority'];
    if (!$priority or !is_numeric($priority) or $priority < 1 or $priority > 10) {
      throw new Exception("Must provide a numeric \$priority argument between 1 and 10 to the tripal_add_job() function.");
    }
    $arguments = $details['arguments'];
    if (!is_array($arguments)) {
      throw new Exception("Must provide an array as the \$arguments argument to the tripal_add_job() function.");
    }

    // convert the arguments into a string for storage in the database
    $args = [];
    if (is_array($arguments)) {
      $args = serialize($arguments);
    }

    try {
      // Before inserting a new record, and if ignore_duplicate is TRUE then
      // check to see if the job already exists.
      if ($details['ignore_duplicate'] === TRUE) {
        $query = db_select('tripal_jobs', 'tj');
        $query->fields('tj', ['job_id']);
        $query->condition('job_name', $details['job_name']);
        $query->isNull('start_time');
        $job_id = $query->execute()->fetchField();
        if ($job_id) {
          $this->load($job_id);
          return FALSE;
        }
      }

      $job_id = db_insert('tripal_jobs')
        ->fields([
          'job_name' => $details['job_name'],
          'modulename' => $details['modulename'],
          'callback' => $details['callback'],
          'status' => 'Waiting',
          'submit_date' => time(),
          'uid' => $details['uid'],
          'priority' => $priority,
          'arguments' => $args,
          'includes' => serialize($includes),
        ])
        ->execute();
      // Now load the job into this object.
      $this->load($job_id);

      return TRUE;
    } catch (Exception $e) {
      throw new Exception('Cannot create job: ' . $e->getMessage());
    }
  }

  /**
   * Cancels the job and prevents it from running.
   */
  public function cancel() {

    if (!$this->job) {
      throw new Exception("There is no job associated with this object. Cannot cancel");
    }

    if ($this->job->status == 'Running') {
      throw new Exception("Job Cannot be cancelled it is currently running.");

    }
    if ($this->job->status == 'Completed') {
      throw new Exception("Job Cannot be cancelled it has already finished.");
    }
    if ($this->job->status == 'Error') {
      throw new Exception("Job Cannot be cancelled it is in an error state.");
    }
    if ($this->job->status == 'Cancelled') {
      throw new Exception("Job Cannot be cancelled it is already cancelled.");
    }

    // Set the end time for this job.
    try {
      if ($this->job->start_time == 0) {
        $record = new stdClass();
        $record->job_id = $this->job->job_id;
        $record->status = 'Cancelled';
        $record->progress = '0';
        drupal_write_record('tripal_jobs', $record, 'job_id');
      }
    } catch (Exception $e) {
      throw new Exception('Cannot cancel job: ' . $e->getMessage());
    }
  }

  /**
   * Executes the job.
   */
  public function run() {

    $this->start_time = time();
    $this->progress_start_time = time();

    if (!$this->job) {
      throw new Exception('Cannot launch job as no job is associated with this object.');
    }

    try {

      // Include the necessary files needed to run the job.
      if (is_array($this->job->includes)) {
        foreach ($this->job->includes as $path) {
          if ($path) {
            require_once $path;
          }
        }
      }

      // Set the start time for this job.
      $record = new stdClass();
      $record->job_id = $this->job->job_id;
      $record->start_time = $this->start_time;
      $record->status = 'Running';
      $record->pid = getmypid();
      drupal_write_record('tripal_jobs', $record, 'job_id');


      // Callback functions need the job in order to update
      // progress.  But prior to Tripal v3 the job callback functions
      // only accepted a $job_id as the final argument.  So, we need
      // to see if the callback is Tv3 compatible or older.  If older
      // we want to still support it and pass the job_id.
      $arguments = $this->job->arguments;
      $callback = $this->job->callback;
      $ref = new ReflectionFunction($callback);
      $refparams = $ref->getParameters();
      if (count($refparams) > 0) {
        $lastparam = $refparams[count($refparams) - 1];
        if ($lastparam->getName() == 'job_id') {
          $arguments[] = $this->job->job_id;
        }
        else {
          $arguments[] = $this;
        }
      }

      // Launch the job.
      call_user_func_array($callback, $arguments);

      // Set the end time for this job.
      $record = new stdClass();
      $record->job_id = $this->job->job_id;
      $record->end_time = time();
      $record->error_msg = $this->job->error_msg;
      $record->progress = 100;
      $record->status = 'Completed';
      $record->pid = '';

      drupal_write_record('tripal_jobs', $record, 'job_id');
      $this->load($this->job->job_id);
    } catch (Exception $e) {
      $record->end_time = time();
      $record->error_msg = $this->job->error_msg;
      $record->progress = $this->job->progress;
      $record->status = 'Error';
      $record->pid = '';
      drupal_write_record('tripal_jobs', $record, 'job_id');
      drupal_set_message('Job execution failed: ' . $e->getMessage(), 'error');
    }
  }

  /**
   * Inidcates if the job is running.
   *
   * @return
   *   TRUE if the job is running, FALSE otherwise.
   */
  public function isRunning() {
    if (!$this->job) {
      throw new Exception('Cannot check running status as no job is associated with this object.');
    }

    $status = shell_exec('ps -p ' . escapeshellarg($this->job->pid) . ' -o pid=');
    if ($this->job->pid && $status) {
      // The job is still running.
      return TRUE;
    }
    // return FALSE to indicate that no jobs are currently running.
    return FALSE;
  }

  /**
   * Retrieve the job object as if from a database query.
   */
  public function getJob() {
    return $this->job;
  }

  /**
   * Retrieves the job ID.
   */
  public function getJobID() {
    return $this->job->job_id;
  }

  /**
   * Retrieves the user ID of the user that submitted the job.
   */
  public function getUID() {
    return $this->job->uid;
  }

  /**
   * Retrieves the job name.
   */
  public function getJobName() {
    return $this->job->job_name;
  }

  /**
   * Retrieves the name of the module that submitted the job.
   */
  public function getModuleName() {
    return $this->job->modulename;
  }

  /**
   * Retrieves the callback function for the job.
   */
  public function getCallback() {
    return $this->job->callback;
  }

  /**
   * Retrieves the array of arguments for the job.
   */
  public function getArguments() {
    return $this->job->arguments;
  }

  /**
   * Retrieves the current percent complete (i.e. progress) of the job.
   */
  public function getProgress() {
    return $this->job->progress;
  }

  /**
   * Sets the current percent complete of a job.
   *
   * @param $percent_done
   *   A value between 0 and 100 indicating the percentage complete of the job.
   */
  public function setProgress($percent_done) {
    if (!$this->job) {
      throw new Exception('Cannot set progress as no job is associated with this object.');
    }

    $this->job->progress = $percent_done;

    $progress = sprintf("%d", $percent_done);
    db_update('tripal_jobs')
      ->fields([
        'progress' => $progress,
      ])
      ->condition('job_id', $this->job->job_id)
      ->execute();
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
  public function setTotalItems($total_items) {
    $this->progress_start_time = time();

    $this->total_items = $total_items;
  }

  /**
   * Adds to the count of the total number of items that have been handled.
   *
   * @param $num_handled
   */
  public function addItemsHandled($num_handled) {
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
  public function setItemsHandled($total_handled) {

    // First set the number of items handled.
    $this->num_handled = $total_handled;

    if ($total_handled == 0) {
      $memory = number_format(memory_get_usage());
      print "Percent complete: 0%. Memory: " . $memory . " bytes.\r";
      return;
    }

    // Now see if we need to report to the user the percent done.  A message
    // will be printed on the command-line if the job is run there.
    $percent = ($this->num_handled / $this->total_items) * 100;
    $ipercent = (int) $percent;

    // If we've reached our interval then print update info.
    if ($ipercent > 0 and $ipercent != $this->reported and ($ipercent % $this->interval) == 0) {
      $duration = (time() - $this->progress_start_time) / 60;
      $duration = sprintf("%.2f", $duration);
      $memory = memory_get_usage();
      $fmemory = number_format($memory);
      $spercent = sprintf("%d", $percent);
      print "Percent complete: " . $spercent . "%. Memory: " . $fmemory . " bytes. Duration: " . $duration . " mins\r";
      $this->setProgress($percent);
      $this->reported = $ipercent;
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
  public function setInterval($interval) {
    $this->interval = $interval;
  }

  /**
   * Retrieves the status of the job.
   */
  public function getStatus() {
    return $this->job->status;
  }

  /**
   * Retrieves the time the job was submitted.
   */
  public function getSubmitTime() {
    return $this->job->submit_date;
  }

  /**
   * Retieves the time the job began execution (i.e. the start time).
   */
  public function getStartTime() {
    return $this->job->start_time;
  }

  /**
   * Retieves the time the job completed execution (i.e. the end time).
   */
  public function getEndTime() {
    return $this->job->end_time;
  }

  /**
   * Retieves the log for the job.
   *
   * @return
   *   A large string containing the text of the job log.  It contains both
   *   status upates and errors.
   */
  public function getLog() {
    return $this->job->error_msg;
  }

  /**
   * Retrieves the process ID of the job.
   */
  public function getPID() {
    return $this->job->pid;
  }

  /**
   * Retreieves the priority that is currently set for the job.
   */
  public function getPriority() {
    return $this->job->priority;
  }

  /**
   * Get the MLock value of the job.
   *
   * The MLock value indicates if no other jobs from a give module
   * should be executed while this job is running.
   */
  public function getMLock() {
    return $this->job->mlock;
  }

  /**
   * Get the lock value of the job.
   *
   * The lock value indicates if no other jobs from any module
   * should be executed while this job is running.
   */
  public function getLock() {
    return $this->job->lock;
  }

  /**
   * Get the list of files that must be included prior to job execution.
   */
  public function getIncludes() {
    return $this->job->includes;
  }

  /**
   * Logs a message for the job.
   *
   * There is no distinction between status messages and error logs.  Any
   * message that is intended for the user to review the status of the job
   * can be provided here.
   *
   * Messages that are are of severity TRIPAL_CRITICAL or TRIPAL_ERROR
   * are also logged to the watchdog.
   *
   * Logging works regardless if the job uses a transaction. If the
   * transaction must be rolled back to to an error the error messages will
   * persist.
   *
   * If a function can be executed by the Tripal job system (and hence the
   * job object is passed in) then you can directly use this function to
   * log messages. However, if the function can be run via drush on the
   * command-line, consider using the tripal_report_error() function which can
   * accept a job object as an $option and will print to both the terminal
   * and to the job object.  If you use the tripal_report_error() be sure
   * to set the 'watchdog' option only if you need log messages also going
   * to the watchdog.
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

    // For the sake of the command-line user, print the message to the
    // terminal.
    print $tmessage . "\n";

    // Add this message to the job's log.
    $this->job->error_msg .= "\n" . $tmessage;

    // Report this message to watchdog or set a message.
    if ($severity == TRIPAL_CRITICAL or $severity == TRIPAL_ERROR) {
      tripal_report_error('tripal_job', $severity, $message, $variables);
      $this->job->status = 'Error';
    }
  }
}
