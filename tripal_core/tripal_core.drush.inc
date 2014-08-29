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
    // Deprecated functions
    case 'drush:tripal-launch-jobs':
      return dt('DEPRECATED. Please use: trp-run-jobs.');
      break;
    case 'drush:tripal-current-job':
      return dt('DEPRECATED. Please use: trp-get-currjob.');
      break;
    case 'drush:tripal-rerun-job':
      return dt('DEPRECATED. Please use: trp-rerun-job.');
      break;

    // Tripal Materialized Views
    case 'drush:tripal-update-mview':
      return dt('Updates the specified materialized view.');
      break;

    // Chado Specific
    case 'drush:tripal-chado-version':
      return dt('Returns the current version of chado associated with this drupal site.');
      break;
    case 'drush:tripal-chadotable-desc':
      return dt('Returns the table description as specified in the Tripal Schema API for the supplied table.');
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
function tripal_core_drush_command() {
  $items = array();

  $items['trp-refresh-mview'] = array(
    'description' => dt('Refreshes the contents of the specified materialized view.'),
    'arguments' => array(),
    'examples' => array(
      'By Materialized View ID' => 'drush trp-refresh-mview --mview=5',
      'By Table Name' => 'drush trp-refresh-mview --table=organism_feature_count'
    ),
    'options' => array(
      'mview' => dt('The ID of the materialized view to update'),
      'table' => dt('The name of the materialized view table to update.'),
    ),
  );
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
      'Single Job' => 'drush trp-run-jobs --user=administrator',
      'Parallel Job' => 'drush trp-run-jobs --user=administrator --parallel=1'
    ),
    'arguments' => array(),
    'options' => array(
      'user' => array(
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
        'required' => TRUE,
      ),
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'job_id' => dt('Provide a job_id to run a specific job. Only jobs that have not been run already can be used'),
    ),
  );
  $items['trp-rerun-job'] = array(
    'description' => dt('Re-run a specific job from the queue.'),
    'examples' => array(
      'Single Job' => 'drush trp-rerun-job --user=administrator --job_id=2',
      'Parallel Job' => 'drush trp-rerun-job --user=administrator  --job_id=2 --parallel=1'
    ),
    'arguments' => array(),
    'options' => array(
      'user' => array(
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
        'required' => TRUE,
      ),
      'job_id' => array(
        'description' => dt('The job ID to run.'),
        'required' => TRUE,
      ),
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
    ),
  );
  $items['trp-get-cversion'] = array(
    'description' => dt('Returns the current installed version of Chado.'),
    'arguments' => array(),
    'examples' => array(
      'Standard Example' => 'drush trp-get-cversion',
    ),
  );
  $items['trp-get-table'] = array(
    'description' => dt('Returns a table description in Drupal Schema API format.'),
    'arguments' => array(),
    'examples' => array(
      'By Table Name' => 'drush trp-get-table --table=feature',
      'By Section' => 'drush trp-get-table --table=feature --section=fields'
    ),
    'options' => array(
      'table' => array(
        'description' => dt('The name of the table. The table can be a true Chado table or a custom Chado table.'),
        'required' => TRUE,
      ),
      'section' => dt('Only return the specified section of the schema array. Possible sections include: description, fields, primary key, unique keys, foreign keys, indexes, referring_tables.'),
    ),
  );
  $items['trp-clean-nodes'] = array(
    'description' => dt('Removes orphaned Drupal nodes.'),
    'arguments' => array(),
    'examples' => array(
      'Standard Example' => 'drush trp-clean-nodes table=feature'
    ),
    'options' => array(
      'table' => array(
        'description' => dt('The name of the table that corresonds to the node type to ' .
          'clean up. (e.g organism, feature, stock, library, analysis, pub, etc.)'),
        'required' => TRUE,
      ),
    ),
  );

  // DEPRECATED COMMANDS. Deprecated as of Tripal v2.0-rc
  $items['tripal-mview-update'] = array(
    'description' => dt('DEPRECATED. Please see: trp-refresh-mview.'),
    'arguments' => array(),
    'examples' => array(
      'By Materialized View ID' => 'drush tripal-update-mview --mview_id=5',
      'By Table Name' => 'drush tripal-update-mview --table_name=organism_feature_count'
    ),
    'options' => array(
      'mview_id' => dt('The ID of the materialized view to update'),
      'table_name' => dt('The name of the materialized view table to update.'),
    ),
    'aliases' => array('trpmv-up')
  );
  $items['tripal-jobs-current'] = array(
    'description' => dt('DEPRECATED. Please see: trp-get-currjob.'),
    'arguments'   => array(),
    'examples' => array(
      'Standard example' => 'drush tripal-jobs-current',
    ),
    'aliases' => array('trpjob-cur'),
  );
  $items['tripal-jobs-launch'] = array(
    'description' => dt('DEPRECATED. Please see: trp-run-jobs. '),
    'examples' => array(
      'Normal Job' => 'drush tripal-jobs-launch admin',
      'Parallel Job' => 'drush tripal-jobs-launch admin --parallel=1'
    ),
    'arguments' => array(
      'user' => dt('The Drupal username under which the job should be run.  The permissions for this user will be used.'),
    ),
    'options' => array(
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'job_id' => dt('Provide a job_id to run a specific job. Only jobs that have not been run already can be used'),
    ),
    'aliases' => array('trpjob-run')
  );
  $items['tripal-jobs-rerun'] = array(
    'description' => dt('DEPRECATED. Please see: trp-rerun-job. '),
    'examples' => array(
      'Normal Job' => 'drush tripal-jobs-rerun admin 2',
      'Parallel Job' => 'drush tripal-jobs-rerun admin  2 --parallel=1'
    ),
    'arguments' => array(
      'user' => dt('The Drupal username under which the job should be run.  The permissions for this user will be used.'),
      'job_id' => dt('The job ID to run.'),
    ),
    'options' => array(
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
    ),
    'aliases' => array('trpjob-rerun')
  );
  $items['tripal-chado-version'] = array(
    'description' => dt('DEPRECATED. Please see: trp-get-cversion. '),
    'arguments' => array(),
    'examples' => array(
      'Standard Example' => 'drush tripal-chado-version',
    ),
    'aliases' => array('trpchado-ver')
  );
  $items['tripal-chadotable-desc'] = array(
    'description' => dt('DEPRECATED. Please see: trp-get-table. '),
    'arguments' => array(
      'table_name' => dt('The name of the chado table.'),
    ),
    'examples' => array(
      'By Table Name' => 'drush tripal-chadotable-desc --table_name=feature'
    ),
    'options' => array(
      'section' => dt('Only return the specified section of the schema table description. Possible sections include: description, fields, primary key, unique keys, foreign keys, indexes, referring_tables.'),
    ),
    'aliases' => array('trpschema-tbl')
  );
  $items['tripal-node-clean'] = array(
    'description' => dt('DEPRECATED. Please see: trp-clean-nodes.'),
    'arguments' => array(
      'module' => dt('The name of the chado-centric module to clean-up.'),
    ),
    'examples' => array(
      'By Table Name' => 'drush tripal-node-clean feature'
    ),
    'options' => array(),
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
 * Executes jobs in the Tripal Jobs Queue.
 *
 * Executed when 'drush trp-run-job' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_trp_run_jobs() {
  $username = drush_get_option('user');
  $parallel = drush_get_option('parallel');
  $job_id   = drush_get_option('job_id');

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
 * DEPRECATED. Executes jobs in the Tripal Jobs Queue.
 *
 * Executed when drush 'trpjob-run' or 'drush tripal-launch-jobs' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_launch($username) {
  $parallel = drush_get_option('parallel');
  $job_id = drush_get_option('job_id');

  drush_tripal_core_set_user($username);
  
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will ".
    "continue to work but please consider using the 'trp-run-jobs' command.\n\n");

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
function drush_tripal_core_trp_rerun_job() {
  $username = drush_get_option('user');
  $parallel = drush_get_option('parallel');
  $job_id   = drush_get_option('job_id');
  
  drush_tripal_core_set_user($username);
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
 * DEPRECATED. Executes jobs in the Tripal Jobs Queue.
 *
 * Executed when 'drush tripal-jobs-rerun' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_rerun($username, $job_id) {
  
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will ".
      "continue to work but please consider using the 'trp-rerun-job' command.\n\n");
  
  drush_tripal_core_set_user($username);
  $new_job_id = tripal_rerun_job($job_id, FALSE);
  drush_tripal_core_tripal_jobs_launch($username, $new_job_id);

}

/**
 * Prints details about the current running job.
 *
 * Executed when 'drush trp-get-currjob' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_trp_get_currjob() {
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
 * DEPRECATED. Prints details about the current running job.
 *
 * Executed when 'drush trpjob-curr' or 'drush tripal-current-job' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_current() {
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will ".
      "continue to work but please consider using the 'trp-get-currjob' command.\n\n");
  
  drush_tripal_core_trp_get_currjob();
}

/**
 * Updates the specified materialized view
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_trp_refresh_mview() {
  $mview_id   = drush_get_option('mview');
  $table_name = drush_get_option('table');

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
      drush_set_error('Plese provide one option of --mview or --table.');
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
 * DEPRECATED. Updates the specified materialized view.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_update_mview() {
  
  $mview_id = drush_get_option('mview_id');
  $table_name = drush_get_option('table_name');
  
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will ".
      "continue to work but please consider using the 'trp-refresh-mview' command.\n\n");

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
 * DEPRECATED. Returns the current version of chado.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_chado_version() {
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will ".
      "continue to work but please consider using the 'trp-get-cversion' command.\n\n");
  
  drush_tripal_core_trp_get_cversion();
}

/**
 * Returns the current version of chado.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_trp_get_cversion() {
  $version = $GLOBALS["exact_chado_version"];
  drush_print('Current Chado Version: ' . $version);
}

/**
 * Returns the Tripal Schema API Description of the given table
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_trp_get_table() {
  $section = drush_get_option('section');
  $table_name = drush_get_option('table');

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
 * DEPRECATED. Returns the Tripal Schema API Description of the given table.
 *
 * @param $table_name
 *  The name of the table to return the description of
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_chadotable_desc($table_name) {
  $section = drush_get_option('section');
  
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will ".
      "continue to work but please consider using the 'trp-get-table' command.\n\n");

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
 * Clean-up orphaned Drupal nodes and chado records.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_trp_clean_nodes() {
  $table = drush_get_option('table');
  
  chado_cleanup_orphaned_nodes($table, 0);
}

/**
 * DEPRECATED. Clean-up orphaned Drupal nodes and chado records.
 *
 * @param $module
 *  The name of a module with nodes associated with it. For example, feature
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_node_clean($module) {
  chado_cleanup_orphaned_nodes($module, 0);
}
