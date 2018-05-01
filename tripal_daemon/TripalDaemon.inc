<?php

/**
 * @file
 * Implements the Tripal Daemon functionality by using the Daemon API.
 */

/**
 * This is the main class for the Tripal Daemon.
 *
 * It extends the DaemonAPIDaemon class provided by the Daemon API in order
 * to implement tripal job checking and execution functionality.
 */
class TripalDaemon extends DrushDaemon {

  // OPTIONAL: Set how often in seconds your executeTask() should be called.
  // Keep in mind that this time does not include the amount of time spent
  // executing your tasks. For example, if you set this to 5 seconds and you
  // have 2 tasks in your execute_tasks() function, each of which take 15
  // seconds, then your loop will iterate (and thus your execute_task()
  // function will be called again) before your tasks finish.
  // CODING STANDARDS: Can't change this variable to lowerCamel since it
  // inherits from a library class.
  protected $loop_interval = 20;

  // Keep track of whether we are running a Tripal job or not
  // and if so, which Tripal jobs are we running.
  // If this array is empty then we are waiting for jobs ;-).
  protected $tripal_jobs = [];

  // Queue of Tripal Jobs waiting to be run.
  protected $queue = [];

  // Boolean as to whether we are aloud to process jobs in parallel.
  // @todo: Implement actually changing this setting.
  // NOTE: Can't be set via a drush option to trpjob-daemon since
  // tripal daemon calls drush daemon which then forks and can't
  // pass on the options to the child.
  protected $do_parallel = FALSE;

  // Maximum number of jobs that can be run in parallel.
  // @todo: Implement actually changing this setting (see above note).
  protected $max_num_jobs = 3;

  /**
   * Implements DaemonAPIDaemon::executeTask() function.
   *
   * This gets executed once per loop iteration & does the following:
   *   1. Checks to see if there are any Tripal Jobs waiting to be executed.
   *   2. If there are then they are run (jobs with a higher priority and higher
   *      job_id are run first.
   *
   * This function will log how many jobs have been found and when each one was
   * started/completed, as well as, it's status upon completion.
   *
   * @param int $iteration_number
   *   This is an integer stating the current iteration of the loop you are on.
   */
  protected function executeTask($iteration_number) {

    // If jobs are being run in parallel then we need to check to see if jobs
    // we think are running have actually been completed.
    if ($this->do_parallel) {
      $this->checkJobStatus();
    }

    // Start the loop by seeing if there is a job to be run :-).
    $job_id = $this->getTripalJobID();
    while ($job_id) {

      // Simply run the job :-D.
      $this->runTripalJob($job_id);

      // Get the next job (if we're aloud to run another one)...
      $job_id = $this->getTripalJobID();
    }

    // If jobs are being run in parallel then we need to check to see if jobs
    // we think are running have actually been completed.
    if ($this->do_parallel) {
      $this->checkJobStatus();
    }
  }

  /**
   * Get job_id of Tripal Job to run.
   *
   * NOTE: This function should only return a job_id if we are aloud to run it.
   */
  protected function getTripalJobID() {

    // First we need to determine if we are in sequenctial mode or parallel mode.
    // Parallel:
    if ($this->do_parallel) {

      // Check that we arn't already running the maximum number of jobs.
      if (tripal_max_jobs_exceeded($this->max_num_jobs)) {
        $this->log('Already running the maximum number of jobs.');
        return FALSE;
      }
      // Also check based on our list of running jobs just in case they haven't yet registered in the db.
      if (sizeof($this->tripal_jobs) >= $this->max_num_jobs) {
        $this->log('Already running the maximum number of jobs.');
        return FALSE;
      }
    }
    // Sequential:
    else {

      // Check that we arn't already running a job.
      if (tripal_is_job_running()) {
        $this->log('Job is still running. Waiting until it completes before starting another one.');
        return FALSE;
      }
    }

    // If we reach this point then we're aloud to run a job! :-D.
    //-----------------------------------------------------------

    // We would like to use a queue to keep track of Tripal Jobs to be run.
    // This will cut down on the number of queries and help ensure that the same job is
    // not run repeatedly with parallel processing.
    // First step, fill the queue if it's empty.
    if (empty($this->queue)) {
      $this->queue = db_query(
        "SELECT job_id FROM {tripal_jobs} TJ
         WHERE TJ.start_time IS NULL AND TJ.end_time IS NULL AND TJ.status != 'Cancelled'
         ORDER BY priority ASC, job_id ASC"
      )->fetchCol();
    }

    // If the queue is still empty then there are no jobs waiting.
    if (empty($this->queue)) {
      return FALSE;
    }

    // Return the next job in line.
    $job_id = array_shift($this->queue);

    // But only if it wasn't already run.
    if (!isset($this->tripal_jobs[$job_id])) {
      return $job_id;
    }
  }

  /**
   * Run Tripal Job.
   */
  protected function runTripalJob($job_id) {

    // Load the job we are going to run.
    $job = new TripalJob();
    $job->load($job_id);

    // Tell admins we are running a job.
    $this->tripal_jobs[$job_id] = $job;
    $this->setStatus();
    // And log the details.
    $this->log('Job (ID=' . $job_id . ') Started at ' . format_date(time(), 'small') . '.');

    // Parallel:
    if ($this->do_parallel) {
      $this->runParallelTripalJob($job_id);
    }
    // Sequential:
    else {
      $job = $this->runSequentialTripalJob($job);

      // Job needs to be re-loaded to reflect the new end time and status
      // since this does not seem to be set by run().
      $job->load($job_id);

      // If the job is sequential then we know we are done the job
      // just by the virtue of having reached this code.
      // As such, tell the admin :-).
      unset($this->tripal_jobs[$job_id]);
      $this->setStatus();
      // And log the details.
      $this->log('Job (ID=' . $job_id . ') Completed');
      $this->log('End DateTime: ' . format_date($job->getEndTime(), 'small'), '', 1);
      $this->log('Status: ' . $job->getStatus(), '', 1);

    }
  }

  /**
   * Run Parallel Tripal Job.
   */
  protected function runParallelTripalJob($job_id) {

    // Tripal job launcher needs the user... Unfortunatly we cannot pass one through the
    // drush command since that is intercepted by drushd.
    // As such we are going ot use the god user (uid=1) as a default that can be overriden
    // by passing in the drush "user" option.
    $uid = drush_get_option('user', 1);
    $user = user_load($uid);
    $username = $user->name;

    // We use drush_invoke_process() to fork the daemon safely to run
    // multiple jobs concurrently. We can't use the PHP-daemon built
    // in functionality such as workers & tasks b/c they copy the
    // entire enviro. resulting in mutliple processes using the same
    // database connection (which causes errors).
    drush_invoke_process(
      '@self',                               // Obviously run on the current site.
      'trp-run-jobs',                        // Run the tripal job launcher.
      [],                               // No arguments (only options below).
      [
        'job_id' => $job_id,                 // The job to be run.
        'username' => $username,             // The user to run it as.
        'single' => 1,                       // Only run a single job!
        'parallel' => $this->do_parallel,    // We're aloud to run in parallel.
        'max_jobs' => $this->max_num_jobs,   // But only this many jobs at once.
      ],
      ['fork' => TRUE]                  // This tells drush to spawn a new process.
    );

  }

  /**
   * Run Sequential Tripal Job.
   */
  protected function runSequentialTripalJob($job) {

    // Run the job.
    try {
      $job->run();
    } catch (Exception $e) {
      $job->logMessage($e->getMessage(), [], TRIPAL_ERROR);
    }

    return $job;
  }

  /**
   * Check the status of a given (or all running) jobs.
   *
   * @param $job_id
   *    The job_id of a specific job to check the status us. (OPTIONAL)
   */
  protected function checkJobStatus($job_id = FALSE) {

    if ($job_id) {
      $job = new TripalJob();
      $job->load($job_id);
      $jobs = [$job_id];
    }
    elseif (!empty($this->tripal_jobs)) {
      $jobs = array_keys($this->tripal_jobs);
    }
    else {
      return TRUE;
    }

    $results = db_query(
      'SELECT job_id, pid, end_time, status FROM {tripal_jobs} WHERE job_id IN (:jobs)',
      [':jobs' => $jobs]);
    foreach ($results as $job) {
      // If the system still thinks the job is running then check it's pid.
      if ($job->status == 'Running') {
        $status = shell_exec('ps -p ' . escapeshellarg($job->pid) . ' -o pid=');
        if (!$status) {

          // Update the job.
          $job->end_time = time();
          $job->error_msg = 'Unknown Error Encountered.';
          $job->status = 'Error';
          $job->pid = '';
          drupal_write_record('tripal_jobs', $job, 'job_id');
        }
      }

      // The job is finished if it's not running or waiting -tell the admin.
      if ($job->status != 'Running' AND $job->status != 'Waiting') {

        // As such, tell the admin :-).
        unset($this->tripal_jobs[$job->job_id]);
        $this->setStatus();
        // And log the details.
        $this->log('Job (ID=' . $job->job_id . ') Completed');
        $this->log('End DateTime: ' . format_date($job->end_time, 'small'), '', 1);
        $this->log('Status: ' . $job->status, '', 1);

      }
    }
  }

  /**
   * Override to include additional daemon-specific settings for use in reports.
   *
   * @return array
   *   An array of status details where the key should be a human-readable
   *   name for your detail and the value should be the current state.
   */
  protected function getStatusDetails() {
    $status_details = parent::getStatusDetails();

    $status_details['Running Job'] = (empty($this->tripal_jobs)) ? FALSE : TRUE;
    $status_details['Current Jobs'] = array_keys($this->tripal_jobs);

    return $status_details;
  }

  /**
   * Set whether we should run parallel jobs or not.
   *
   * @param $do_parallel
   *   Boolean indicating whether to allow parallel processing of jobs.
   * @param $max_num_jobs
   *   Integer indicating the maximum number of jobs to run at once.
   */
  public function setParallel($do_parallel, $max_num_jobs) {
    $this->do_parallel = $do_parallel;
    $this->max_num_jobs = $max_num_jobs;
  }
}
