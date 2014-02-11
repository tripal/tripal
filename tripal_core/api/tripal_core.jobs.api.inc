<?php
/**
 * @file
 * Tripal offers a job management subsystem for managing tasks that may require an extended period of time for
 * completion.
 */

/**
 * @defgroup tripal_jobs_api Tripal Jobs API
 * @ingroup tripal_core_api
 * @{
 * Tripal offers a job management subsystem for managing tasks that may require an extended period of time for
 * completion.  Drupal uses a UNIX-based cron job to handle tasks such as  checking  the  availability of updates,
 * indexing new nodes for searching, etc.   Drupal's cron uses the web interface for launching these tasks, however,
 * Tripal provides several administrative tasks that may time out and not complete due to limitations of the web
 * server.  Examples including syncing of a large number of features between chado and Drupal.  To circumvent this,
 * as well as provide more fine-grained control and monitoring, Tripal uses a jobs management sub-system built into
 * the Tripal Core module.   It is anticipated that this functionality will be used for managing analysis jobs provided by
 * future tools, with eventual support for distributed computing.
 *
 * The  Tripal jobs management system allows administrators to submit tasks to be performed which can then  be
 * launched through a UNIX command-line PHP script or cron job.  This command-line script can be added to a cron
 * entry along-side the Drupal cron entry for automatic, regular launching of Tripal jobs.  The order of execution of
 * waiting jobs is determined first by priority and second by the order the jobs were entered.
 *
 * The API functions described below provide a programmatic interface for adding, checking and viewing jobs.
 * @}
 */

/**
 * Adds a job to the Tripal Jbo queue
 *
 * @param $job_name
 *    The human readable name for the job
 * @param $modulename
 *    The name of the module adding the job
 * @param $callback
 *    The name of a function to be called when the job is executed
 * @param $arguments
 *    An array of arguments to be passed on to the callback
 * @param $uid
 *    The uid of the user adding the job
 * @param $priority
 *    The priority at which to run the job where the highest priority is 10 and the lowest priority
 *    is 1. The default priority is 10.
 *
 * @return
 *    The job_id of the registered job
 *
 * Example usage:
 * @code
 *  $args = array($dfile, $organism_id, $type, $library_id, $re_name, $re_uname,
 *        $re_accession, $db_id, $rel_type, $re_subject, $parent_type, $method,
 *         $user->uid, $analysis_id, $match_type);
 *
 * tripal_add_job("Import FASTA file: $dfile", 'tripal_feature',
 *   'tripal_feature_load_fasta', $args, $user->uid);
 * @endcode
 * The code above is copied from the tripal_feature/fasta_loader.php file. The
 * snipped first builds an array of arguments that will then be passed to the
 * tripal_add_job function.  The number of arguments provided in the $arguments
 * variable should match the argument set for the callback function provided
 * as the third argument.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_add_job($job_name, $modulename, $callback, $arguments, $uid, $priority = 10) {

  // convert the arguments into a string for storage in the database
  $args = array();
  if (is_array($arguments)) {
    $args = serialize($arguments);
  }

  $record = new stdClass();
  $record->job_name = $job_name;
  $record->modulename = $modulename;
  $record->callback = $callback;
  $record->status = 'Waiting';
  $record->submit_date = REQUEST_TIME;
  $record->uid = $uid;
  $record->priority = $priority;  # the lower the number the higher the priority
  $record->arguments = $args;

  if (drupal_write_record('tripal_jobs', $record)) {
    $jobs_url = url("admin/tripal/tripal_jobs");
    drupal_set_message(t("Job '%job_name' submitted.  Check the <a href='!jobs_url'>jobs page</a> for status", array('%job_name' => $job_name, '!jobs_url' => $jobs_url)));
  }
  else {
    drupal_set_message(t("Failed to add job %job_name.", array('%job_name' => $job_name)), 'error');
  }

  return $record->job_id;
}

/**
 * Returns a list of running tripal jobs
 *
 * @return
 *    and array of objects where each object describes a running job or FALSE if no jobs are running
 *
 * @ingroup tripal_jobs_api
 */
function tripal_is_job_running() {

  // iterate through each job that has not ended
  // and see if it is still running. If it is not
  // running but does not have an end_time then
  // set the end time and set the status to 'Error'
  $sql =  "SELECT * FROM {tripal_jobs} TJ " .
          "WHERE TJ.end_time IS NULL and NOT TJ.start_time IS NULL ";
  $jobs = db_query($sql);
  foreach ($jobs as $job) {
    $status = `ps -p $job->pid -o pid=`;
    if ($job->pid && $status) {
      // the job is still running so let it go
      // we return 1 to indicate that a job is running
      return TRUE;
    }
    else {
      // the job is not running so terminate it
      $record = new stdClass();
      $record->job_id = $job->job_id;
      $record->end_time = REQUEST_TIME;
      $record->status = 'Error';
      $record->error_msg = 'Job has terminated unexpectedly.';
      drupal_write_record('tripal_jobs', $record, 'job_id');
    }
  }

  // return 1 to indicate that no jobs are currently running.
  return FALSE;
}

/**
 * Returns the start time for a given job
 *
 * @param $job
 *   An object describing the job
 *
 * @return
 *   The start time of the job if it was already run and either "Cancelled" or "Not Yet Started" otherwise
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job_start($job) {

  if ($job->start_time > 0) {
    $start = format_date($job->start_time);
  }
  else {
    if (strcmp($job->job_status, 'Cancelled')==0) {
      $start = 'Cancelled';
    }
    else {
      $start = 'Not Yet Started';
    }
  }
  return $start;
}

/**
 * Returns the end time for a given job
 *
 * @param $job
 *   An object describing the job
 *
 * @return
 *   The end time of the job if it was already run and empty otherwise
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job_end($job) {

  if ($job->end_time > 0) {
    $end = format_date($job->end_time);
  }
  else {
    $end = '';
  }

  return $end;
}
/**
 * Set a job to be re-ran (ie: add it back into the job queue)
 *
 * @param $job_id
 *   The job_id of the job to be re-ran
 * @param $goto_jobs_page
 *   If set to TRUE then after the re run job is added Drupal will redirect to the jobs page
 *
 * @ingroup tripal_jobs_api
 */
function tripal_rerun_job($job_id, $goto_jobs_page = TRUE) {
  global $user;

  $user_id = $user->uid;

  $sql = "SELECT * FROM {tripal_jobs} WHERE job_id = :job_id";
  $results = db_query($sql, array(':job_id' => $job_id));
  $job = $results->fetchObject();
  // arguments for jobs used to be stored as plain string with a double colon
  // separating them.  But as of Tripal v2.0 the arguments are stored as
  // a serialized array.  To be backwards compatible, we should check for serialization
  // and if not then we will use the old style
  $args = unserialize($job->arguments);
  if (!$args) {
    $args = explode("::", $job->arguments);
  }
  $job_id = tripal_add_job($job->job_name, $job->modulename, $job->callback, $args, $user_id, $job->priority);

  if ($goto_jobs_page) {
    drupal_goto("admin/tripal/tripal_jobs");
  }
  return $job_id;
}

/**
 * Cancel a Tripal Job currently waiting in the job queue
 *
 * @param $job_id
 *   The job_id of the job to be cancelled
 *
 * @ingroup tripal_jobs_api
 */
function tripal_cancel_job($job_id, $redirect = TRUE) {
  $sql = "SELECT * FROM {tripal_jobs} WHERE job_id = :job_id";
  $results = db_query($sql, array(':job_id' => $job_id));
  $job = $results->fetchObject();

  // set the end time for this job
  if ($job->start_time == 0) {
    $record = new stdClass();
    $record->job_id = $job->job_id;
    $record->end_time = REQUEST_TIME;
    $record->status = 'Cancelled';
    $record->progress = '0';
    drupal_write_record('tripal_jobs', $record, 'job_id');
    drupal_set_message(t("Job #%job_id cancelled", array('%job_id' => $job_id)));
  }
  else {
    drupal_set_message(t("Job %job_id cannot be cancelled. It is in progress or has finished.", array('%job_id' => $job_id)));
  }
  if ($redirect) {
    drupal_goto("admin/tripal/tripal_jobs");
  }
}

/**
 * A function used to manually launch all queued tripal jobs
 *
 * @param $do_parallel
 *   A boolean indicating whether jobs should be attempted to run in parallel
 *
 * @param $job_id
 *   To launch a specific job provide the job id.  This option should be
 *   used sparingly as the jobs queue managment system should launch jobs
 *   based on order and priority.  However there are times when a specific
 *   job needs to be launched and this argument will allow it.  Only jobs
 *   which have not been run previously will run.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_launch_job($do_parallel = 0, $job_id = NULL) {

  // first check if any jobs are currently running
  // if they are, don't continue, we don't want to have
  // more than one job script running at a time
  if (!$do_parallel and tripal_is_job_running()) {
    print "Jobs are still running. Use the --parallel=1 option with the Drush command to run jobs in parallel.";
    return;
  }

  // get all jobs that have not started and order them such that
  // they are processed in a FIFO manner.
  if ($job_id) {
    $sql =  "SELECT * FROM {tripal_jobs} TJ " .
            "WHERE TJ.start_time IS NULL and TJ.end_time IS NULL and TJ.job_id = :job_id " .
            "ORDER BY priority ASC,job_id ASC";
    $job_res = db_query($sql, array(':job_id', $job_id));
  }
  else {
    $sql =  "SELECT * FROM {tripal_jobs} TJ " .
            "WHERE TJ.start_time IS NULL and TJ.end_time IS NULL " .
            "ORDER BY priority ASC,job_id ASC";
    $job_res = db_query($sql);
  }
  foreach ($job_res as $job) {
    // set the start time for this job
    $record = new stdClass();
    $record->job_id = $job->job_id;
    $record->start_time = REQUEST_TIME;
    $record->status = 'Running';
    $record->pid = getmypid();
    drupal_write_record('tripal_jobs', $record, 'job_id');

    // call the function provided in the callback column.
    // Add the job_id as the last item in the list of arguments. All
    // callback functions should support this argument.
    $callback = $job->callback;

    // arguments for jobs used to be stored as plain string with a double colon
    // separating them.  But as of Tripal v2.0 the arguments are stored as
    // a serialized array.  To be backwards compatible, we should check for serialization
    // and if not then we will use the old style
    $args = unserialize($job->arguments);
    if (!$args) {
      $args = explode("::", $job->arguments);
    }
    $args[] = $job->job_id;
    print "Calling: $callback(" . implode(", ", $args) . ")\n";
    call_user_func_array($callback, $args);
    // set the end time for this job
    $record->end_time = REQUEST_TIME;
    $record->status = 'Completed';
    $record->progress = '100';
    drupal_write_record('tripal_jobs', $record, 'job_id');

    // send an email to the user advising that the job has finished
  }
}

/**
 * An internal function for setting the progress for a current job
 *
 * @param $job_id
 *   The job_id to set the progress for
 * @param $percentage
 *   The progress to set the job to
 *
 * @return
 *   True on success and False otherwise
 *
 * @ingroup tripal_core
 */
function tripal_set_job_progress($job_id, $percentage) {

  if (preg_match("/^(\d+|100)$/", $percentage)) {
    $record = new stdClass();
    $record->job_id = $job_id;
    $record->progress = $percentage;
    if (drupal_write_record('tripal_jobs', $record, 'job_id')) {
      return TRUE;
    }
  }

  return FALSE;
}
/**
 * Returns a list of jobs associated with the given module
 *
 * @param $modulename
 *    The module to return a list of jobs for
 *
 * @return
 *    An array of objects where each object describes a tripal job
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_active_jobs($modulename) {
  $sql =  "SELECT * FROM {tripal_jobs} TJ " .
           "WHERE TJ.end_time IS NULL and TJ.modulename = :modulename ";
  $results = db_query($sql, array(':modulename' => $modulename));
  return $results->fetchObject();
}

/**
 * Returns the date the job was added to the queue
 *
 * @param $job
 *   An object describing the job
 *
 * @return
 *   The date teh job was submitted
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job_submit_date($job) {
  return format_date($job->submit_date);
}
