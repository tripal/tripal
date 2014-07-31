<?php

/**
 * This is the main class for a Job Daemon.
 *
 * A Daemon is created by running run.php where an instance of this class is created
 */
class TripalJobDaemon extends JobDaemon {

  /**
   * Implements JobDaemon::job() function
   * This gets executed once per loop iteration
   */
  protected function job() {

    // First check to see if there are any tripal jobs to be run
    $num_waiting_jobs = db_query('SELECT count(*) as count FROM {tripal_jobs} j WHERE j.pid IS NULL AND j.end_time IS NULL')->fetchField();

    // If there are then run them and log the output
    if ($num_waiting_jobs > 0) {
      $this->log("$num_waiting_jobs Waiting Tripal Jobs... Running them now.");

      // Launch all tripal jobs :) Yay for bootstrapping!!
      tripal_launch_job();
    }
    else {
      $this->log('There are no Tripal Jobs to run');
    }

	}
}