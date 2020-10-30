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
    case 'trp-version':
      return dt('Returns the current version of Tripal that is installed.');
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
    case 'trp-prepare-chado':
      break;
    case 'trp-set-permissions':
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
  $items = [];

  $items['trp-get-currjob'] = [
    'description' => dt('Returns details about the currently running job including percent complete.'),
    'arguments' => [],
    'examples' => [
      'Standard example' => 'drush trp-get-currjob',
    ],
  ];
  $items['trp-run-jobs'] = [
    'description' => dt('Launches jobs waiting in the queue. Only one job can execute at a time unless the --parallel=1 option is provided.'),
    'examples' => [
      'Single Job' => 'drush trp-run-jobs --username=administrator',
      'Parallel Job' => 'drush trp-run-jobs --username=administrator --parallel=1',
      'Max-jobs Job' => 'drush trp-run-jobs --username=administrator --parallel=1 --max_jobs=10',
    ],
    'arguments' => [],
    'options' => [
      'user' => [
        'description' => dt('DEPRECATED. Conflicts with Drush 7.x --user argument. Please use the --username argument.'),
      ],
      'username' => [
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
      ],
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'job_id' => dt('Provide a job_id to run a specific job. Only jobs that have not been run already can be used'),
      'max_jobs' => dt('Indicate the maximum number of concurrent jobs. Default is -1 (unlimited). Ignore if not running parallel jobs'),
      'single' => dt('Execute only one queued job'),
    ],
  ];
  $items['trp-rerun-job'] = [
    'description' => dt('Re-run a specific job from the queue.'),
    'examples' => [
      'Single Job' => 'drush trp-rerun-job --username=administrator --job_id=2',
      'Parallel Job' => 'drush trp-rerun-job --username=administrator  --job_id=2 --parallel=1',
      'Max-jobs Job' => 'drush trp-run-jobs --username=administrator --parallel=1 --max_jobs=10',
    ],
    'arguments' => [],
    'options' => [
      'user' => [
        'description' => dt('DEPRECATED. Conflicts with Drush 7.x --user argument. Please use the --username argument.'),
      ],
      'username' => [
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
      ],
      'job_id' => [
        'description' => dt('The job ID to run.'),
        'required' => TRUE,
      ],
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'max_jobs' => dt('Indicate the maximum number of concurrent jobs. Default is -1 (unlimited). Ignore if not running parallel jobs'),
      'single' => dt('Execute only one queued job'),
    ],
  ];
  $items['trp-prepare-chado'] = [
    'description' => dt('Prepares a new Tripal installation with content types, requires the username of an administrator to run.'),
    'arguments' => [],
    'examples' => [
      'Standard example' => 'drush trp-prepare-chado --user=administrator',
    ],
  ];
  $items['trp-set-permissions'] = [
    'description' => dt('Gives view, edit, delete, create priveleges to administrators for all tripal content types.'),
    'arguments' => [],
    'examples' => [
      'Standard example' => 'drush trp-set-permissions --user=administrator',
    ],
  ];
  $items['trp-version'] = [
    'description' => dt('Returns the current version of Tripal that is installed.'),
    'arguments' => [],
    'examples' => [
      'Standard usage' => 'drush trp-version',
    ],
  ];
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
    $results = db_query($sql, [':name' => $username]);
    $u = $results->fetchObject();
    if (!$u) {
      drush_print(date('Y-m-d H:i:s'));
      drush_print('ERROR: Please provide a valid username (--username argument) for running this job.');
      exit;
    }
    global $user;
    $user = user_load($u->uid);
    return $u->uid;
  }
  else {
    drush_print(date('Y-m-d H:i:s'));
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
function drush_tripal_trp_run_jobs_install($username) {
  $parallel = drush_get_option('parallel');
  $job_id = drush_get_option('job_id');
  $max_jobs = drush_get_option('max_jobs', -1);
  $single = drush_get_option('single', 0);

  drush_tripal_set_user($username);

  drush_print("\n" . date('Y-m-d H:i:s'));
  if ($parallel) {
    drush_print("Tripal Job Launcher (in parallel)");
    if ($max_jobs !== -1) {
      drush_print("Maximum number of jobs is " . $max_jobs);
    }
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job($parallel, $job_id, $max_jobs, $single);
  }
  else {
    drush_print("Tripal Job Launcher");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job(0, $job_id, $max_jobs, $single);
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
  $job_id = drush_get_option('job_id');
  $max_jobs = drush_get_option('max_jobs', -1);
  $single = drush_get_option('single', 0);

  // Unfortunately later versions of Drush use the '--user' argument which
  // makes it incompatible with how Tripal was using it.  For backwards
  // compatibility we will accept --user with a non numeric value only. The
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

  drush_print("\n" . date('Y-m-d H:i:s'));
  if ($parallel) {
    drush_print("Tripal Job Launcher (in parallel)");
    if ($max_jobs !== -1) {
      drush_print("Maximum number of jobs is " . $max_jobs);
    }
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job($parallel, $job_id, $max_jobs, $single);
  }
  else {
    drush_print("Tripal Job Launcher");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job(0, $job_id, $max_jobs, $single);
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
  print date('Y-m-d H:i:s') . ": USER: '$user', UNAME: '$uname'\n";
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
  $job_id = drush_get_option('job_id');
  $max_jobs = drush_get_option('max_jobs', -1);
  $single = drush_get_option('single', 0);

  drush_tripal_set_user($username);
  $new_job_id = tripal_rerun_job($job_id, FALSE);

  drush_print("\n" . date('Y-m-d H:i:s'));
  if ($parallel) {
    drush_print("Tripal Job Launcher (in parallel)");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job($parallel, $new_job_id, $max_jobs, $single);
  }
  else {
    drush_print("Tripal Job Launcher");
    drush_print("Running as user '$username'");
    drush_print("-------------------");
    tripal_launch_job(0, $new_job_id, $max_jobs, $single);
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
  $sql = "
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
      "Progress: " . $job->progress . "%\n" .
      "Current Date: " . date('Y-m-d H:i:s') . "\n";
    drush_print(date('Y-m-d H:i:s'));
    drush_print($output);
  }
  if (!$job_pid) {
    drush_print(date('Y-m-d H:i:s'));
    drush_print('There are currently no running jobs.');
  }
  //log to the command line with an OK status
  drush_log('Running tripal-current-job', 'ok');
}

/**
 * Prepares content types on the site after chado installation.
 *
 * Executed when 'drush trp-prepare-chado' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_trp_prepare_chado() {
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

  print_r("Now preparing the site by creating content types.\n");
  $prepare = drush_invoke_process('@self', 'php-eval', ["module_load_include('inc', 'tripal_chado', 'includes/setup/tripal_chado.setup'); tripal_chado_prepare_drush_submit();"], []);
  drush_invoke_process('@self', 'php-eval', ["module_load_include('inc', 'tripal', 'tripal.drush'); drush_tripal_trp_run_jobs_install(" . $username . ");"], []);
  if (!$prepare) {
    echo "An error occurred when attempting to install Chado. Please navigate to your new site and finish the installation process from the 'Install Tripal' section as described in the online help, found here http://tripal.info/tutorials/v3.x/installation/tripal \n";
    exit;
  }
}

/**
 * Sets permissions for the content types on the site.
 *
 * Executed when 'drush trp-set-permissions' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_trp_set_permissions() {
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
  $permissions = [];

  $bundles = tripal_get_content_types();

  foreach ($bundles as $bundles => $bundle) {
    array_push($permissions, ' view ' . $bundle->name, ' create ' . $bundle->name,
      ' edit ' . $bundle->name, ' delete ' . $bundle->name);
  }
  $string_permissions = implode(",", $permissions);
  $args4 = ['administrator', $string_permissions];
  $options4 = [];
  drush_invoke_process('@self', 'role-add-perm', $args4, $options4);

  drush_print(dt(""));
  drush_print(dt("Permissions is now complete."));
}

/**
 * Returns the current version of Tripal
 * 
 * Executed when 'drush trp-version' is called.
 * 
 * @ingroup tripal_drush
 *
 * @return string
 */
function drush_tripal_trp_version() {
  $version = system_get_info('module', 'tripal');
  drush_print("Tripal version: " . $version['version']);
}
