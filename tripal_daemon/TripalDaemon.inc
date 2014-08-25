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

    // First check to see if there are any tripal jobs to be run.
    $waiting_jobs = db_query(
      "SELECT
        count(*) as count,
        array_to_string(array_agg(j.job_id ORDER BY j.priority ASC, j.job_id ASC),'|') as jobs
      FROM {tripal_jobs} j
      WHERE j.pid IS NULL AND j.end_time IS NULL"
    )->fetchObject();

    $num_waiting_jobs = $waiting_jobs->count;
    $job_ids = explode('|', $waiting_jobs->jobs);

    // If there are then run them and log the output.
    if ($num_waiting_jobs > 0) {
      $this->log($num_waiting_jobs . ' Waiting Tripal Jobs... '
        . 'Running waiting job(s) now.');

      // Launch all tripal jobs :) Yay for bootstrapping!!
      foreach ($job_ids as $id) {
        $this->log('Starting Job (ID=' . $id . ')', '', 1);
        tripal_launch_job(FALSE, $id);

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
    else {
      $this->log('There are no Tripal Jobs to run');
    }

  }
}
