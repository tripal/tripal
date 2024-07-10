<?php

use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\tripal\Services\TripalJob;
use Symfony\Component\HttpFoundation\RedirectResponse;


/**
 * @file
 * Tripal offers a job management subsystem for managing tasks that may require
 * an extended period of time for completion.
 */

/**
 * @defgroup tripal_jobs_api Jobs
 * @ingroup tripal_api
 * @{
 * Tripal offers a job management subsystem for managing tasks that may require
 * an extended period of time for completion. Tripal provides several
 * administrative tasks that may time out and not complete due to limitations
 * of the web server. To circumvent this, as well as provide more fine-grained
 * control and monitoring, Tripal uses a jobs management system.
 *
 * The  Tripal jobs management system allows administrators to submit tasks
 * to be performed which can then be launched through a UNIX command-line PHP
 * script or cron job.  This command-line script can be added to a cron
 * entry along-side the Drupal cron entry for automatic, regular launching of
 * Tripal jobs.  The order of execution of waiting jobs is determined first by
 * priority and second by the order the jobs were entered.
 *
 * @}
 */

/**
 * Adds a job to the Tripal Job queue
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
 *    The priority at which to run the job where the highest priority is 10 and
 *    the lowest priority is 1. The default priority is 10.
 * @param $includes
 *    An array of paths to files that should be included in order to execute
 *    the job. Use the module_load_include function to get a path for a given
 *    file.
 * @param $ignore_duplicate .
 *   Set to TRUE to not add the job if it has
 *   the same name as another job which has not yet run. The default is TRUE.
 *
 * @return
 *    The job_id of the registered job, or FALSE on failure.
 *
 * Example usage:
 *
 * @code
 *   $args = array($dfile, $organism_id, $type, $library_id, $re_name,
 *   $re_uname,
 *         $re_accession, $db_id, $rel_type, $re_subject, $parent_type,
 *   $method,
 *         $user->uid, $analysis_id, $match_type);
 *
 *   $includes = array()
 *   $includes[] = module_load_include('inc', 'tripal_chado',
 *                                'includes/loaders/tripal_chado.fasta_loader');
 *
 *   tripal_add_job("Import FASTA file: $dfile", 'tripal_feature',
 *     'tripal_feature_load_fasta', $args, $user->uid, 10, $includes);
 * @endcode
 *
 * The code above is copied from the tripal_feature/fasta_loader.php file. The
 * snipped first builds an array of arguments that will then be passed to the
 * tripal_add_job function.  The number of arguments provided in the $arguments
 * variable should match the argument set for the callback function provided
 * as the third argument.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_add_job($job_name, $modulename, $callback, $arguments, $uid,
                        $priority = 10, $includes = [], $ignore_duplicate = FALSE) {

  try {
    $job_id = \Drupal::service('tripal.job')->create([
      'job_name' => $job_name,
      'modulename' => $modulename,
      'callback' => $callback,
      'arguments' => $arguments,
      'uid' => $uid,
      'priority' => $priority,
      'includes' => $includes,
      'ignore_duplicate' => $ignore_duplicate,
    ]);
  }
  catch (Exception $e) {
    $logger = \Drupal::service('tripal.logger');
    $logger->error($e->getMessage(), [], ['drupal_set_messsage' => 'TRUE']);
    return FALSE;
  }
  return $job_id;
}


/**
 * Indicates if any jobs are running.
 *
 * This function will check the system to see if a job has a process ID
 * and if that process ID is still running. It will update the job status
 * accordingly before returning.
 *
 * @return
 *   Returns TRUE if any job is running or FALSE otherwise.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_is_job_running() {

  // iterate through each job that has not ended
  // and see if it is still running. If it is not
  // running but does not have an end_time then
  // set the end time and set the status to 'Error'
  $sql = "
    SELECT * FROM {tripal_jobs} TJ
    WHERE TJ.end_time IS NULL and NOT TJ.start_time IS NULL
  ";
  $database = \Drupal::database();
  $query = $database->query($sql);
  while($job = $query->fetchObject()) {
    $status = shell_exec('ps -p ' . escapeshellarg($job->pid) . ' -o pid=');
    if ($job->pid && $status) {
      // the job is still running so let it go
      // we return 1 to indicate that a job is running
      return TRUE;
    }
    else {
      // the job is not running so terminate it
      $database = \Drupal::database();
      $database->update('tripal_jobs')
        ->fields([
          'end_time' => time(),
          'status' => 'Error',
          'error_msg' => 'Job has terminated unexpectedly.',
        ])
        ->condition('job_id', $job->job_id)
        ->execute();
    }
  }

  // return 1 to indicate that no jobs are currently running.
  return FALSE;
}

/**
 * Check for too many concurrent jobs.
 *
 * @param $max_jobs
 *   The maximum number of concurrent jobs to allow; -1 = no limit
 *
 * @ingroup tripal_jobs_api
 */
function tripal_max_jobs_exceeded($max_jobs) {
  if ($max_jobs < 0) {
    // No limit on concurrent jobs
    return FALSE;
  }

  $num_jobs_running = 0;

  // Iterate through each job that has not ended and see if it is still running.
  // If it is not running but does not have an end_time then set the end time
  // and set the status to 'Error'
  $sql = "
    SELECT * FROM {tripal_jobs} TJ
    WHERE TJ.end_time IS NULL and NOT TJ.start_time IS NULL
  ";
  $database = \Drupal::database();
  $query = $database->query($sql);
  while ($job = $query->fetchObject()) {
    $status = shell_exec('ps -p ' . escapeshellarg($job->pid) . ' -o pid=');
    if ($job->pid && $status) {
      // the job is still running
      $num_jobs_running++;
    }
    else {
      $database = \Drupal::database();
      $database->update('tripal_jobs')
      ->fields([
        'end_time' => time(),
        'status' => 'Error',
        'error_msg' => 'Job has terminated unexpectedly.',
      ])
      ->condition('job_id', $job->job_id)
      ->execute();
    }
  }

  return ($num_jobs_running >= $max_jobs);
}

/**
 * Set a job to be re-run (ie: add it back into the job queue).
 *
 * @param $job_id
 *   The job_id of the job to be re-ran
 * @param $goto_jobs_page
 *   DEPRECATED
 *   If set to TRUE then after the re run job is added Drupal will redirect to
 *   the jobs page
 *
 * @ingroup tripal_jobs_api
 */
function tripal_rerun_job($job_id, $goto_jobs_page = NULL) {

  if (!is_null($goto_jobs_page)) {
    $logger = \Drupal::service('tripal.logger');
    $logger->warning("DEPRECATED: the '\$goto_jobs_page' argument of the ".
        "tripal_rerun_job function will be removed " .
        "from the API in a future release. Please update any calls.");
  }

  $current_user = \Drupal::currentUser();
  $user_id = $current_user->id();
  $user = \Drupal\user\Entity\User::load($current_user->id());
  $messenger = \Drupal::messenger();

  $job = new TripalJob();
  $job->load($job_id);

  $arguments = $job->getArguments();
  $includes = $job->getIncludes();

  try {
    $job->create([
      'job_name' => $job->getJobName(),
      'modulename' => $job->getModuleName(),
      'callback' => $job->getCallback(),
      'arguments' => $arguments,
      'uid' => $user_id,
      'priority' => $job->getPriority(),
      'includes' => $includes,
    ]);

    // If no exceptions were thrown then we know the creation worked.  So
    // let the user know!
    $messenger->addStatus(t("Job '%job_name' submitted.", ['%job_name' => $job->getJobName()]));

    // If this is the Tripal admin user then give a bit more information
    // about how to run the job.
    if ($user->hasPermission('administer tripal')) {
      $jobs_url = Link::fromTextAndUrl('jobs page', Url::fromUri('internal:/admin/tripal/tripal_jobs'))->toString();
      $messenger->addStatus(t("Check the @jobs_url for status.", ['@jobs_url' => $jobs_url]));
      $messenger->addStatus(t("You can execute the job queue manually on the ".
          "command line using the following Drush command: " .
          "<br>drush trp-run-jobs --job_id=%job_id --username=%uname --root=%base_path",
          [
            '%job_id' => $job->getJobID(),
            '%base_path' => DRUPAL_ROOT,
            '%uname' => $user->getAccountName()
          ]));
    }
  }
  catch (Exception $e) {
    $logger = \Drupal::service('tripal.logger');
    $logger->error($e->getMessage(), [], ['drupal_set_messsage' => 'TRUE']);
  }
}

/**
 * Cancel a Tripal Job currently waiting in the job queue.
 *
 * @param $job_id
 *   The job_id of the job to be cancelled
 *
 * @param $redirect
 *   DEPRECATED
 *
 * @return
 *   FALSE if the an error occured or the job could not be canceled, TRUE
 *   otherwise.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_cancel_job($job_id, $redirect = NULL) {

  if (!is_null($redirect)) {
    $logger = \Drupal::service('tripal.logger');
    $logger->warning("DEPRECATED: the '\$goto_jobs_page' argument of the ".
        "tripal_rerun_job function will be removed " .
        "from the API in a future release. Please update any calls.");
  }

  if (!$job_id or !is_numeric($job_id)) {
    watchdog('tripal', "Must provide a numeric \$job_id to the tripal_cancel_job() function.");
    return FALSE;
  }

  try {
    $job = new TripalJob();
    $job->load($job_id);
    $job->cancel();

    \Drupal::messenger()->addStatus('Job is now cancelled.');
    return TRUE;
  }
  catch (Exception $e) {
    $logger = \Drupal::service('tripal.logger');
    $logger->error($e->getMessage(), [], ['drupal_set_messsage' => 'TRUE']);
    return FALSE;
  }
}

/**
 * A function used to manually launch all queued tripal jobs.
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
 * @param $max_jobs
 *   The maximum number of jobs that should be run concurrently. If -1 then
 *   unlimited.
 * @param $single
 *   Ensures only a single job is run rather then the entire queue.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_launch_job($do_parallel = 0, $job_id = NULL, $max_jobs = -1, $single = 0) {

  $messenger = \Drupal::messenger();

  // First check if any jobs are currently running if they are, don't continue,
  // we don't want to have more than one job script running at a time.
  if (!$do_parallel and tripal_is_job_running()) {
    print date('Y-m-d H:i:s') . ": Jobs are still running. Use the --parallel option with the Drush command to run jobs in parallel.";
    return;
  }

  if ($do_parallel && tripal_max_jobs_exceeded($max_jobs)) {
    print date('Y-m-d H:i:s') . ": More than $max_jobs jobs are still running. At least one of these jobs much complete before a new job can start.";
    return;
  }

  // Get all jobs that have not started and order them such that they are
  // processed in a FIFO manner.
  if ($job_id) {
    $sql = "
      SELECT TJ.job_id
      FROM {tripal_jobs} TJ
      WHERE
        TJ.start_time IS NULL AND
        TJ.end_time IS NULL AND
        TJ.job_id = :job_id
      ORDER BY priority ASC, job_id ASC
    ";
    $database = \Drupal::database();
    $query = $database->query($sql, [':job_id' => $job_id]);
  }
  else {
    $sql = "
      SELECT TJ.job_id
      FROM {tripal_jobs} TJ
      WHERE
        TJ.start_time IS NULL AND
        TJ.end_time IS NULL AND
        NOT TJ.status = 'Cancelled'
      ORDER BY priority ASC,job_id ASC
    ";
    $database = \Drupal::database();
    $query = $database->query($sql);
  }

  while ($jid = $query->fetchObject()) {

    $job_id = $jid->job_id;

    // Create the Tripoaljob object.
    $job = new TripalJob();
    $job->load($job_id);

    // We need to do some additional processing for printing since the switch
    // to serialized arrays now allows nested arrays which cause errors when
    // printed using implode alone.
    $args = $job->getArguments();
    $string_args = [];
    foreach ($args as $k => $a) {
      if (is_array($a)) {
        $string_args[$k] = 'Array';
      }
      elseif (is_object($a)) {
        $string_args[$k] = 'Object';
      }
      else {
        $string_args[$k] = $a;
      }
    }

    // Run the job
    $callback = $job->getCallback();

    print date('Y-m-d H:i:s') . ": Job ID " . $job_id . ".\n";
    print date('Y-m-d H:i:s') . ": Calling: $callback(" . implode(", ", $string_args) . ")\n";
    try {
      $job->run();
    }
    catch (Exception $e) {
      $job->logMessage($e->getMessage(), [], TRIPAL_ERROR);
      $messenger->addError($e->getMessage());
    }

    if ($single) {
      // Don't start any more jobs
      break;
    }
    if (tripal_max_jobs_exceeded($max_jobs)) {
      break;
    }

    // TODO: Send an email to the user advising that the job has finished
  }
}

/**
 * An internal function for setting the progress for a current job.
 *
 * @param $job_id
 *   The job_id to set the progress for
 * @param $percentage
 *   The progress to set the job to
 *
 * @return
 *   True on success and False otherwise
 *
 * @ingroup tripal_jobs_api
 */
function tripal_set_job_progress($job_id, $percentage) {

  try {
    $job = new TripalJob();
    $job->load($job_id);
    $job->setProgress($percentage);
  }
  catch (Exception $e) {
    $logger = \Drupal::service('tripal.logger');
    $logger->error($e->getMessage(), [], ['drupal_set_messsage' => 'TRUE']);
    return FALSE;
  }
  return TRUE;
}

/**
 * Retrieves the current proress of a job.
 *
 * @param $job_id
 *   The job_id to get the progress for
 *
 * @return
 *   A value between 0 and 100 indicating the percentage complete of the job.
 *   FALSE on failure.
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_job_progress($job_id) {

  try {
    $job = new TripalJob();
    $job->load($job_id);
    $progress = $job->getProgress();
    return $progress;
  }
  catch (Exception $e) {
    $logger = \Drupal::service('tripal.logger');
    $logger->error($e->getMessage(), [], ['drupal_set_messsage' => 'TRUE']);
    return FALSE;
  }
}

/**
 * Returns a list of jobs that are active.
 *
 * @param $modulename
 *   Limit the list returned to those that were added by a specific module. If
 *   no module name is provided then all active jobs are returned.
 *
 * @return
 *    An array of objects where each object describes a tripal job. If no
 *    jobs were found then an empty array is returned.  Each object will have
 *    the following members:
 *    - job_id: The unique ID number for the job.
 *    - uid: The ID of the user that submitted the job.
 *    - job_name:  The human-readable name of the job.
 *    - modulename: The name of the module that submitted the job.
 *    - callback:  The callback function to be called when the job is run.
 *    - arguments: An array of arguments to be passed to the callback function.
 *    - progress: The percent progress of completion if the job is running.
 *    - status: The status of the job: Waiting, Completed, Running or Cancelled.
 *    - submit_date:  The UNIX timestamp when the job was submitted.
 *    - start_time: The UNIX timestamp for when the job started running.
 *    - end_time: The UNIX timestampe when the job completed running.
 *    - error_msg: Any error message that occured during execution of the job.
 *    - prirotiy: The execution priority of the job (value between 1 and 10)
 *
 * @ingroup tripal_jobs_api
 */
function tripal_get_active_jobs($modulename = NULL) {
  $query = db_select('tripal_jobs', 'TJ')
    ->fields('TJ', ['job_id', 'uid', 'job_name', 'modulename', 'callback', 'arguments',
      'progress', 'status', 'submit_date', 'start_time', 'end_time', 'error_msg','priority']);
  if ($modulename) {
    $query->where(
      "TJ.modulename = :modulename and NOT (TJ.status = 'Completed' or TJ.status = 'Cancelled')",
      [':modulename' => $modulename]
    );
  }
  $results = $query->execute();

  $jobs = [];
  while ($job = $results->fetchobject()) {
    $job->arguments = unserialize($job->arguments);
    $jobs[] = $job;
  }
  return $jobs;
}


/**
 * Execute a specific Tripal Job.
 *
 * @param $job_id
 *   The job id to be exeuted.
 * @param $redirect
 *   DEPRECATED
 *
 * @ingroup tripal_jobs_api
 */
function tripal_execute_job($job_id, $redirect = TRUE) {

  $messenger = \Drupal::messenger();

  if (!is_null($redirect)) {
    $logger = \Drupal::service('tripal.logger');
    $logger->warning("DEPRECATED: the '\$goto_jobs_page' argument of the ".
        "tripal_rerun_job function will be removed " .
        "from the API in a future release. Please update any calls.");
  }

  $job = new TripalJob();
  $job->load($job_id);

  // Run the job.
  if ($job->getStartTime() == 0 and $job->getEndTime() == 0) {
    tripal_launch_job(1, $job_id);
    $messenger->addStatus(t("Job %job_id has finished executing. See below for more information.", ['%job_id' => $job_id]));
  }
  else {
    $messenger->addError(t("Job %job_id cannot be executed. It has already finished.", ['%job_id' => $job_id]));
  }
}
