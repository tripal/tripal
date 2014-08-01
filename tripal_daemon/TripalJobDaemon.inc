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
    $waiting_jobs = db_query("SELECT count(*) as count, array_to_string(array_agg(j.job_id ORDER BY j.priority ASC, j.job_id ASC),'|') as jobs FROM {tripal_jobs} j WHERE j.pid IS NULL AND j.end_time IS NULL")->fetchObject();
    $num_waiting_jobs = $waiting_jobs->count;
    $job_ids = explode('|', $waiting_jobs->jobs);

    // If there are then run them and log the output
    if ($num_waiting_jobs > 0) {
      $this->log("$num_waiting_jobs Waiting Tripal Jobs... Running waiting job(s) now.");

      // Launch all tripal jobs :) Yay for bootstrapping!!
      foreach ($job_ids as $id) {
        $this->log("Starting Job (ID=$id)","",1);
        tripal_launch_job(FALSE, $id);

        // Report job details
        $job = db_query("SELECT j.* FROM {tripal_jobs} j WHERE j.job_id = :jid", array(':jid' => $id))->fetchObject();
        $this->log("Job completed at ". date($job->end_time, 'd M Y H:i:s') . " with a status of '" . $job->status . "'","",1);
      }
    }
    else {
      $this->log('There are no Tripal Jobs to run');
    }

	}
}