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
function tripal_chado_drush_help($command) {
  switch ($command) {
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
function tripal_chado_drush_command() {
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


  return $items;
}

/**
 * Set the user to run a drush job.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_set_user($username) {
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
 * Updates the specified materialized view
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_trp_refresh_mview() {
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
 * Returns the current version of chado.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_trp_get_cversion() {
  $version = $GLOBALS["exact_chado_version"];
  drush_print('Current Chado Version: ' . $version);
}

/**
 * Returns the Tripal Schema API Description of the given table
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_trp_get_table() {
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
 * Clean-up orphaned Drupal nodes and chado records.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_trp_clean_nodes() {
  $table = drush_get_option('table');

  chado_cleanup_orphaned_nodes($table, 0);
}

