<?php
/**
 * @file
 * Contains function relating to drush-integration of this module.
 */

/**
 * @defgroup tripal_drush Tripal Drush Integration
 * @{
 * Contains function relating to drush-integration of various tripal modules.
 * @}
 */

/**
 * Describes each drush command implemented by the module
 *
 * @return
 *   The first line of description when executing the help for a given command
 *
 * @ingroup tripal_drush
 */
function tripal_drush_help($command) {
  switch ($command) {

    // TRIPAL JOBS
    case 'trp-run-jobs':
      return dt('Launches pending jobs waiting in the queue.');
      break;
    case 'trp-rerun-job':
      return dt('Rerun a job in the queue.');
      break;
    case 'trp-get-currjob':
      return dt('Returns details about the currently running tripal job including percent complete.');
      break;
    // Placeholders for unimplmeneted jobs
    case 'trp-show-job':
      break;
    case 'trp-revert-jobs':
      break;
    case 'trp-cancel-job':
      break;
    case 'trp-list-jobs':
      break;

  }
}

/**
 * Registers a drush command and constructs the full help for that command.
 *
 * @return
 *   And array of command descriptions
 *
 * @ingroup tripal_drush
 */
function tripal_drush_command() {
  $items = array();

  $items['trp-get-currjob'] = array(
    'description' => dt('Returns details about the currently running job including percent complete.'),
    'arguments'   => array(),
    'examples' => array(
      'Standard example' => 'drush trp-get-currjob',
    ),
  );
  $items['trp-run-jobs'] = array(
    'description' => dt('Launches jobs waiting in the queue. Only one job can execute at a time unless the --parllel=1 option is provided.'),
    'examples' => array(
      'Single Job' => 'drush trp-run-jobs --username=administrator',
      'Parallel Job' => 'drush trp-run-jobs --username=administrator --parallel=1'
    ),
    'arguments' => array(),
    'options' => array(
      'user' => array(
        'description' => dt('DEPRECATED. Conflicts with Drush 7.x --user argument. Please use the --username argument.'),
      ),
      'username' => array(
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
      ),
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'job_id' => dt('Provide a job_id to run a specific job. Only jobs that have not been run already can be used'),
    ),
  );
  $items['trp-rerun-job'] = array(
    'description' => dt('Re-run a specific job from the queue.'),
    'examples' => array(
      'Single Job' => 'drush trp-rerun-job --username=administrator --job_id=2',
      'Parallel Job' => 'drush trp-rerun-job --username=administrator  --job_id=2 --parallel=1'
    ),
    'arguments' => array(),
    'options' => array(
      'user' => array(
        'description' => dt('DEPRECATED. Conflicts with Drush 7.x --user argument. Please use the --username argument.'),
      ),
      'username' => array(
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
      ),
      'job_id' => array(
        'description' => dt('The job ID to run.'),
        'required' => TRUE,
      ),
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
    ),
  );

  return $items;
}

/**
 * Set the user to run a drush job.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_set_user($username) {
  if ($username) {
    $sql = "SELECT uid FROM {users} WHERE name = :name";
    $results = db_query($sql, array(':name' => $username));
    $u = $results->fetchObject();
    if (!$u) {
      drush_print('ERROR: Please provide a valid username (--username argument) for running this job.');
      exit;
    }
    global $user;
    $user = user_load($u->uid);
    return $u->uid;
  }
  else {
    drush_print('ERROR: Please provide a username (--username argument) for running this job.');
    exit;
  }
}

/**
 * Executes jobs in the Tripal Jobs Queue.
 *
 * Executed when 'drush trp-run-job' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_trp_run_jobs() {
  $parallel = drush_get_option('parallel');
  $job_id   = drush_get_option('job_id');

  // Unfortunately later versions of Drush use the '--user' argument which
  // makes it incompatible with how Tripal was using it.  For backwards
  // compatabiliy we will accept --user with a non numeric value only. The
  // numeric value should be for Drush. Tripal will instead use the
  // --username argument for the fture.
  $user = drush_get_option('user');
  $uname = drush_get_option('username');
  if ($user and is_numeric($user)) {
  }
  elseif ($user) {
    print "\nNOTE: Use of the --user argument is deprecated as it conflicts with the --user argument of Drush 7.x. Please now use --username instead.\n\n";
    $username = $user;
  }
  if ($uname) {
    $username = $uname;
  }

  drush_tripal_set_user($username);

  if ($parallel) {
    drush_print("Tripal Job Launcher (in parallel)");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job($parallel, $job_id);
  }
  else {
    drush_print("Tripal Job Launcher");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job(0, $job_id);
  }
}

/**
 * Executes jobs in the Tripal Jobs Queue.
 *
 * Executed when 'drush trp-rerun-job' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_trp_rerun_job() {
  // Unfortunately later versions of Drush use the '--user' argument which
  // makes it incompatible with how Tripal was using it.  For backwards
  // compatabiliy we will accept --user with a non numeric value only. The
  // numeric value should be for Drush. Tripal will instead use the
  // --username argument for the fture.
  $user = drush_get_option('user');
  $uname = drush_get_option('username');
  print "USER: '$user', UNAME: '$uname'\n";
  if ($user and is_numeric($user)) {
  }
  elseif ($user) {
    print "\nNOTE: Use of the --user argument is deprecated as it conflicts with the --user argument of Drush 7.x. Please now use --username instead.\n\n";
    $username = $user;
  }
  if ($uname) {
    $username = $uname;
  }

  $parallel = drush_get_option('parallel');
  $job_id   = drush_get_option('job_id');

  drush_tripal_set_user($username);
  $new_job_id = tripal_rerun_job($job_id, FALSE);

  if ($parallel) {
    drush_print("Tripal Job Launcher (in parallel)");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job($parallel, $new_job_id);
  }
  else {
    drush_print("Tripal Job Launcher");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job(0, $new_job_id);
  }

}

/**
 * Prints details about the current running job.
 *
 * Executed when 'drush trp-get-currjob' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_trp_get_currjob() {
  $sql =  "
    SELECT *
    FROM {tripal_jobs} TJ
    WHERE TJ.end_time IS NULL and NOT TJ.start_time IS NULL
  ";
  $jobs = db_query($sql);

  foreach ($jobs as $job) {
    $job_pid = $job->pid;
    $output = "Name: " . $job->job_name . "\n" .
        "Submitted: " . date(DATE_RFC822, $job->submit_date) . "\n" .
        "Started: " . date(DATE_RFC822, $job->start_time) . "\n" .
        "Module: " . $job->modulename . "\n" .
        "Callback: " . $job->callback . "\n" .
        "Process ID: " . $job->pid . "\n" .
        "Progress: " . $job->progress . "%\n";
    drush_print($output);
  }
  if (!$job_pid) {
    drush_print('There are currently no running jobs.');
  }
  //log to the command line with an OK status
  drush_log('Running tripal-current-job', 'ok');
}




