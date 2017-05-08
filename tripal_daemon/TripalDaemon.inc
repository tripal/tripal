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

    // When sorting the job list we want to use version specific SQL and thus
    // need to know the postgreSQL version to determine what SQL to execute.
    $version_string = db_query('SELECT version()')->fetchField();
    if (preg_match('/PostgreSQL (\d+)\.(\d+)/', $version_string, $matches)) {
      $version = array('major' => $matches[1], 'minor' => $matches[2]);
    }
    // If we can't determine the version then use the deprecated method.
    else {
      $version = array('major' => 8, 'minor' => 4);
    }

    // First check to see if there are any tripal jobs to be run.
    if ($version['major'] >= 9 ) {
      $waiting_jobs = db_query(
        "SELECT
          count(*) as count,
          array_to_string(array_agg(j.job_id ORDER BY j.priority ASC, j.job_id ASC),'|') as jobs
        FROM {tripal_jobs} j
        WHERE j.pid IS NULL AND j.end_time IS NULL"
      )->fetchObject();
    }
    else {
     $waiting_jobs = db_query(
        "SELECT
          count(*) as count,
          array_to_string(array_agg(j.job_id),'|') as jobs
        FROM (SELECT * FROM {tripal_jobs} WHERE pid IS NULL AND end_time IS NULL ORDER BY priority ASC, job_id ASC) as j"
      )->fetchObject();
    }

    $num_waiting_jobs = $waiting_jobs->count;
    $job_ids = explode('|', $waiting_jobs->jobs);

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

        // Launch Tripal Job.
        tripal_launch_job(FALSE, $id);

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
    else {
      $this->log('There are no Tripal Jobs to run');
    }

  }
}
