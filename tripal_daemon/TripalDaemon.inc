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
  protected $tripal_jobs = array();

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
 
    $do_parallel = FALSE;
    $max_jobs = 1;

    // First check if any jobs are currently running if they are, don't continue,
    // we don't want to have more than one job script running at a time.
    if (!$do_parallel and tripal_is_job_running()) {
      $this->log("Jobs are still running. Use the --parallel=1 option with the Drush command to run jobs in parallel.\n");
    }
    elseif ($do_parallel && tripal_max_jobs_exceeded($max_jobs)) {
      $this->log("At least $max_jobs jobs are still running. At least one of these jobs much complete before a new job can start.\n");
    }
    else {

      // First check to see if there are any tripal jobs to be run.
      $sql =  "
        SELECT TJ.job_id
        FROM {tripal_jobs} TJ
        WHERE
          TJ.start_time IS NULL AND
          TJ.end_time IS NULL AND
          NOT TJ.status = 'Cancelled'
        ORDER BY priority ASC,job_id ASC";
      $job_ids = db_query($sql)->fetchCol();
      $num_waiting_jobs = sizeof($job_ids);

      // If there are then run them and log the output.
      if ($num_waiting_jobs > 0) {
        $this->log($num_waiting_jobs . ' Waiting Tripal Jobs... '
          . 'Running waiting job(s) now.');

        // Launch all tripal jobs :) Yay for bootstrapping!!
        foreach ($job_ids as $id) {
          $this->log('Starting Job (ID=' . $id . ')', '', 1);

          // We would like to log the output from the job.
          // However, most tripal jobs simply print to the screen :-(
          // Thus we have to use output buffering to capture the output.
          // Start Buffering.
          ob_start();

          // Tell admins we are running a job.
          $this->tripal_jobs[$id] = $id;
          $this->setStatus();

          // Launch Tripal Job.
          $job = new TripalJob();
          $job->load($id);
          try {
            $job->run();
          }
          catch (Exception $e) {
            $job->logMessage($e->getMessage(), array(), TRIPAL_ERROR);
          } 

          // Tell admins that we're done :-).
          unset($this->tripal_jobs[$id]);
          $this->setStatus();

          // Save the buffer to the log and stop buffering.
          $this->log(str_repeat('=', 80));
          $this->log(ob_get_clean());
          $this->log(str_repeat('=', 80));

          // Report job details.
          $job = db_query(
            "SELECT j.*
            FROM {tripal_jobs} j
            WHERE j.job_id = :jid",
            array(':jid' => $id)
          )->fetchObject();
          $this->log("Job completed at "
          . date('d M Y H:i:s', $job->end_time) . " with a status of '"
          . $job->status . "'", "", 1);
        }
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
    $status_details['Current Jobs'] = $this->tripal_jobs;

    return $status_details;
  }
}
