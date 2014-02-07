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
function tripal_core_drush_help($command) {
  switch ($command) {
    // Tripal Materialized Views
    case 'drush:tripal-update-mview':
      return dt('Updates the specified materialized view.');

    // Tripal Jobs
    case 'drush:tripal-launch-jobs':
      return dt('Launches any Tripal jobs waiting in the queue.');
    case 'drush:tripal-current-job':
      return dt('Returns details about the currently running tripal job including percent complete.');
    case 'drush:tripal-rerun-job':
      return dt('Rerun any tripal job in the queue.');

    // Chado-specific
    case 'drush:tripal-chado-version':
      return dt('Returns the current version of chado associated with this drupal site.');
    case 'drush:tripal-chadotable-desc':
      return dt('Returns the table description as specified in the Tripal Schema API for the supplied table.');

  }
}

/**
 * Registers a drush command and constructs the full help for that command
 *
 * @return
 *   And array of command descriptions
 *
 * @ingroup tripal_drush
 */
function tripal_core_drush_command() {
  $items = array();

  // Materialized Views
  $items['tripal-mview-update'] = array(
    // used by drush help
    'description' => dt('Updates the specified materialized view.'),
    'arguments' => array(
      'mview_id' => dt('The ID of the materialized view to update'),
      'table_name' => dt('The name of the materialized view table to update.'),
    ),
    'examples' => array(
      'By Materialized View ID' => 'drush tripal-update-mview --mview_id=5',
      'By Table Name' => 'drush tripal-update-mview --table_name=organism_feature_count'
    ),
    // supply options
    'options' => array(
      'mview_id',
      'table_name'
    ),
    'aliases' => array('trpmv-up', 'tripal-update-mview')
  );

  // Jobs
  $items['tripal-jobs-current'] = array(
    'description' => dt('Returns details about the currently running tripal job including percent complete.'),
    'arguments'   => array(
    ),
    'examples' => array(
      'Standard example' => 'drush tripal-jobs-current',
    ),
    'aliases' => array('trpjob-cur', 'tripal-current-job'),
  );
  $items['tripal-jobs-launch'] = array(
    // used by drush help
    'description' => dt('Lauches any jobs waiting in the queue.'),
    'examples' => array(
      'Normal Job' => 'drush tripal-jobs-launch admin',
      'Parallel Job' => 'drush tripal-jobs-launch admin --parallel=1'
    ),
    'arguments' => array(
      'user' => dt('The Drupal username under which the job should be run.  The permissions for this user will be used.'),
    ),
    // supply options
    'options' => array(
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'job_id' => dt('Provide a job_id to run a specific job. Only jobs that have not been run already can be used'),
    ),
    'aliases' => array('trpjob-run', 'tripal-launch-jobs')
  );
  $items['tripal-jobs-rerun'] = array(
    // used by drush help
    'description' => dt('Rerun any job in the queue.'),
    'examples' => array(
      'Normal Job' => 'drush tripal-jobs-rerun admin 2',
      'Parallel Job' => 'drush tripal-jobs-rerun admin  2 --parallel=1'
    ),
    'arguments' => array(
      'user' => dt('The Drupal username under which the job should be run.  The permissions for this user will be used.'),
      'job_id' => dt('The job ID to run.'),
    ),
    // supply options
    'options' => array(
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
    ),
    'aliases' => array('trpjob-rerun', 'tripal-rerun-job')
  );

  // Chado-specific
  $items['tripal-chado-version'] = array(
    // used by drush help
    'description' => dt('Returns the current version of chado associated with this drupal site.'),
    'arguments' => array(
    ),
    'examples' => array(
      'Standard Example' => 'drush tripal-chado-version',
    ),
    'aliases' => array('trpchado-ver')
  );
  $items['tripal-chadotable-desc'] = array(
    // used by drush help
    'description' => dt('Returns the table description as specified in the Tripal Schema API for the supplied table.'),
    'arguments' => array(
      'table_name' => dt('The name of the chado table.'),
    ),
    'examples' => array(
      'By Table Name' => 'drush tripal-chadotable-desc --table_name=feature'
    ),
    // supply options
    'options' => array(
      'section' => dt('Only return the specified section of the schema table description. Possible sections include: description, fields, primary key, unique keys, foreign keys, indexes, referring_tables.'),
    ),
    'aliases' => array('trpschema-tbl')
  );

  // Tripal Node-Module Specific
  $items['tripal-node-sync'] = array(
    // used by drush help
    'description' => dt('Sync\'s chado records with drupal creating nodes for the given chado-centric module.'),
    'arguments' => array(
      'module' => dt('The name of the chado-centric module to sync.'),
    ),
    'examples' => array(
      'By Module' => 'drush tripal-node-sync feature'
    ),
    // supply options
    'options' => array(
    ),
    'aliases' => array('trpnode-sync')
  );
  $items['tripal-node-clean'] = array(
    // used by drush help
    'description' => dt('Clean-up orphaned Drupal nodes and chado records.'),
    'arguments' => array(
      'module' => dt('The name of the chado-centric module to clean-up.'),
    ),
    'examples' => array(
      'By Table Name' => 'drush tripal-node-clean feature'
    ),
    // supply options
    'options' => array(
    ),
    'aliases' => array('trpnode-cln')
  );

  return $items;
}

/**
 * Set the user to run a drush job.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_set_user($username) {
  if ($username) {
    $sql = "SELECT uid FROM {users} WHERE name = :name";
    $results = db_query($sql, array(':name' => $username));
    $u = $results->fetchObject();
    if (!$u) {
      drush_print('ERROR: Please provide a valid username for running this job.');
      exit;
    }
    global $user;
    $user = user_load($u->uid);
    return $u->uid;
  }
  else {
    drush_print('ERROR: Please provide a username for running this job.');
    exit;
  }
}

/**
 * Executes jobs in the Tripal Jobs Queue
 *
 * NOTE: The following code is executed when drush 'trpjob-run' or 'drush tripal-launch-jobs' is called
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_launch($username) {
  $parallel = drush_get_option('parallel');
  $job_id = drush_get_option('job_id');

  drush_tripal_core_set_user($username);

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
 * Executes jobs in the Tripal Jobs Queue
 *
 * NOTE: The following code is executed when drush 'trpjob-run' or 'drush tripal-launch-jobs' is called
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_rerun($username, $job_id) {

  drush_tripal_core_set_user($username);
  $new_job_id = tripal_rerun_job($job_id, FALSE);
  drush_tripal_core_tripal_jobs_launch($username, $new_job_id);

}

/**
 * Prints details about the current running job
 *
 * NOTE: The following code is executed when 'drush trpjob-curr' or 'drush tripal-current-job' is called
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_current() {
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

/**
 * Updates the specified materialized view
 *
 * @param $mview_id
 *   The ID of the materialized view (tripal_mview.mview_id)
 * @param $table_name
 *   The name of the table storing the materialized view (tripal_mview.mv_table)
 *
 * Note: Either $mview_id OR $table_name is required
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_update_mview() {
  $mview_id = drush_get_option('mview_id');
  $table_name = drush_get_option('table_name');

  // Either table_name or mview is required
  if (!$mview_id) {
    if ($table_name) {
      // if table_name supplied use that to get mview_id
      $sql = "SELECT mview_id FROM {tripal_mviews} WHERE mv_table = :mv_table";
      $results = db_query($sql, array(':mv_table' => $table_name));
      $r = $resuls->fetchObject();
      if (!$r->mview_id) {
        drush_set_error('No Materialized View associated with that table_name.');
      }
      $mview_id=$r->mview_id;
    }
    else {
      drush_set_error('Either mview_id OR table_name are required.');
    }
  }

  drush_print('Updating the Materialized View with ID=' . $mview_id);
  $status = tripal_populate_mview($mview_id);
  if ($status) {
    drush_log('Materialized View Updated', 'ok');
  }
  else {
    drush_set_error('Update failed.');
  }
}

/**
 * Returns the current version of chado
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_chado_version() {
  $version = $GLOBALS["exact_chado_version"];
  drush_print('Current Chado Version: ' . $version);
}

/**
 * Returns the Tripal Schema API Description of the given table
 *
 * @param $table_name
 *  The name of the table to return the description of
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_chadotable_desc($table_name) {
  $section = drush_get_option('section');

  drush_print("Schema API Description for $table_name:");
  $desc = chado_get_schema($table_name);

  if (!empty($section)) {
    drush_print("$section = " . print_r($desc[$section], TRUE));
  }
  else {
    drush_print(print_r($desc, TRUE));
  }
}

/**
 * Sync's chado records with drupal creating nodes for the given chado-centric module.
 *
 * @param $module
 *  The name of a module with nodes associated with it. For example, feature
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_node_sync($module) {
  switch ($module) {
    case 'analysis':
      call_user_func('tripal_analysis_sync_analyses');
      break;
    case 'feature':
      call_user_func('tripal_feature_sync_features');
      break;
    case 'featuremap':
      call_user_func('tripal_featuremap_sync_featuremaps');
      break;
    case 'library':
      call_user_func('tripal_library_sync_libraries');
      break;
    case 'organism':
      call_user_func('tripal_organism_sync_organisms');
      break;
    case 'project':
      call_user_func('tripal_project_sync_all_projects');
      break;
    case 'stock':
      call_user_func('tripal_stock_sync_stocks');
      break;
  }
}

/**
 * Clean-up orphaned Drupal nodes and chado records.
 *
 * @param $module
 *  The name of a module with nodes associated with it. For example, feature
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_node_clean($module) {
  chado_cleanup_orphaned_nodes($module, 0);
}
