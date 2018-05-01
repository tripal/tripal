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
  $items = [];

  $items['trp-clean-nodes'] = [
    'description' => dt('Removes orphaned Drupal nodes.'),
    'arguments' => [],
    'examples' => [
      'Standard Example' => 'drush trp-clean-nodes table=feature',
    ],
    'options' => [
      'table' => [
        'description' => dt('The name of the table that corresonds to the node type to ' .
          'clean up. (e.g organism, feature, stock, library, analysis, pub, etc.)'),
        'required' => TRUE,
      ],
    ],
  ];

  // DEPRECATED COMMANDS. Deprecated as of Tripal v2.0-rc
  $items['tripal-mview-update'] = [
    'description' => dt('DEPRECATED. Please see: trp-refresh-mview.'),
    'arguments' => [],
    'examples' => [
      'By Materialized View ID' => 'drush tripal-update-mview --mview_id=5',
      'By Table Name' => 'drush tripal-update-mview --table_name=organism_feature_count',
    ],
    'options' => [
      'mview_id' => dt('The ID of the materialized view to update'),
      'table_name' => dt('The name of the materialized view table to update.'),
    ],
    'aliases' => ['trpmv-up'],
  ];
  $items['tripal-jobs-current'] = [
    'description' => dt('DEPRECATED. Please see: trp-get-currjob.'),
    'arguments' => [],
    'examples' => [
      'Standard example' => 'drush tripal-jobs-current',
    ],
    'aliases' => ['trpjob-cur'],
  ];
  $items['tripal-jobs-launch'] = [
    'description' => dt('DEPRECATED. Please see: trp-run-jobs. '),
    'examples' => [
      'Normal Job' => 'drush tripal-jobs-launch admin',
      'Parallel Job' => 'drush tripal-jobs-launch admin --parallel=1',
    ],
    'arguments' => [
      'username' => dt('The Drupal username under which the job should be run.  The permissions for this user will be used.'),
    ],
    'options' => [
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
      'job_id' => dt('Provide a job_id to run a specific job. Only jobs that have not been run already can be used'),
    ],
    'aliases' => ['trpjob-run'],
  ];
  $items['tripal-jobs-rerun'] = [
    'description' => dt('DEPRECATED. Please see: trp-rerun-job. '),
    'examples' => [
      'Normal Job' => 'drush tripal-jobs-rerun admin 2',
      'Parallel Job' => 'drush tripal-jobs-rerun admin  2 --parallel=1',
    ],
    'arguments' => [
      'username' => dt('The Drupal username under which the job should be run.  The permissions for this user will be used.'),
      'job_id' => dt('The job ID to run.'),
    ],
    'options' => [
      'parallel' => dt('Normally jobs are executed one at a time. But if you are certain no conflicts will occur with other currently running jobs you may set this argument to a value of 1 to make the job run in parallel with other running jobs.'),
    ],
    'aliases' => ['trpjob-rerun'],
  ];
  $items['tripal-chado-version'] = [
    'description' => dt('DEPRECATED. Please see: trp-get-cversion. '),
    'arguments' => [],
    'examples' => [
      'Standard Example' => 'drush tripal-chado-version',
    ],
    'aliases' => ['trpchado-ver'],
  ];
  $items['tripal-chadotable-desc'] = [
    'description' => dt('DEPRECATED. Please see: trp-get-table. '),
    'arguments' => [
      'table_name' => dt('The name of the chado table.'),
    ],
    'examples' => [
      'By Table Name' => 'drush tripal-chadotable-desc --table_name=feature',
    ],
    'options' => [
      'section' => dt('Only return the specified section of the schema table description. Possible sections include: description, fields, primary key, unique keys, foreign keys, indexes, referring_tables.'),
    ],
    'aliases' => ['trpschema-tbl'],
  ];
  $items['tripal-node-clean'] = [
    'description' => dt('DEPRECATED. Please see: trp-clean-nodes.'),
    'arguments' => [
      'module' => dt('The name of the chado-centric module to clean-up.'),
    ],
    'examples' => [
      'By Table Name' => 'drush tripal-node-clean feature',
    ],
    'options' => [],
    'aliases' => ['trpnode-cln'],
  ];

  return $items;
}

/**
 * Set the user to run a drush job.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_set_user($username) {
  drush_tripal_set_user($username);
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

  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
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
 * DEPRECATED. Executes jobs in the Tripal Jobs Queue.
 *
 * Executed when 'drush tripal-jobs-rerun' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_rerun($username, $job_id) {

  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
    "continue to work but please consider using the 'trp-rerun-job' command.\n\n");

  drush_tripal_core_set_user($username);
  $new_job_id = tripal_rerun_job($job_id, FALSE);
  drush_tripal_core_tripal_jobs_launch($username, $new_job_id);

}

/**
 * DEPRECATED. Prints details about the current running job.
 *
 * Executed when 'drush trpjob-curr' or 'drush tripal-current-job' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_jobs_current() {
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
    "continue to work but please consider using the 'trp-get-currjob' command.\n\n");

  drush_tripal_core_trp_get_currjob();
}

/**
 * DEPRECATED. Updates the specified materialized view.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_core_tripal_update_mview() {

  $mview_id = drush_get_option('mview_id');
  $table_name = drush_get_option('table_name');

  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
    "continue to work but please consider using the 'trp-refresh-mview' command.\n\n");

  // Either table_name or mview is required
  if (!$mview_id) {
    if ($table_name) {
      // if table_name supplied use that to get mview_id
      $sql = "SELECT mview_id FROM {tripal_mviews} WHERE mv_table = :mv_table";
      $results = db_query($sql, [':mv_table' => $table_name]);
      $r = $resuls->fetchObject();
      if (!$r->mview_id) {
        drush_set_error('No Materialized View associated with that table_name.');
      }
      $mview_id = $r->mview_id;
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
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
    "continue to work but please consider using the 'trp-get-cversion' command.\n\n");

  drush_tripal_core_trp_get_cversion();
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

  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
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


/**
 * Clean-up orphaned Drupal nodes and chado records.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_trp_clean_nodes() {
  $table = drush_get_option('table');

  chado_cleanup_orphaned_nodes($table, 0);
}