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
  $items = [];

  $items['trp-refresh-mview'] = [
    'description' => dt('Refreshes the contents of the specified materialized view.'),
    'arguments' => [],
    'examples' => [
      'By Materialized View ID' => 'drush trp-refresh-mview --mview=5',
      'By Table Name' => 'drush trp-refresh-mview --table=organism_feature_count',
    ],
    'options' => [
      'mview' => dt('The ID of the materialized view to update'),
      'table' => dt('The name of the materialized view table to update.'),
    ],
  ];

  $items['trp-get-cversion'] = [
    'description' => dt('Returns the current installed version of Chado.'),
    'arguments' => [],
    'examples' => [
      'Standard Example' => 'drush trp-get-cversion',
    ],
  ];
  $items['trp-get-table'] = [
    'description' => dt('Returns a table description in Drupal Schema API format.'),
    'arguments' => [],
    'examples' => [
      'By Table Name' => 'drush trp-get-table --table=feature',
      'By Section' => 'drush trp-get-table --table=feature --section=fields',
    ],
    'options' => [
      'table' => [
        'description' => dt('The name of the table. The table can be a true Chado table or a custom Chado table.'),
        'required' => TRUE,
      ],
      'section' => dt('Only return the specified section of the schema array. Possible sections include: description, fields, primary key, unique keys, foreign keys, indexes, referring_tables.'),
    ],
  ];

  //
  // Drush commands for publications
  //
  $items['trp-import-pubs'] = [
    'description' => dt('Imports publications from remote databases using saved configuration settings.'),
    'options' => [
      'create_contacts' => dt('Create or update contacts for authors. This option is only available with the --dxref option because creation of contacts is otherwise set for each importer.'),
      'dbxref' => dt('An accession number for a publication from a remote database (e.g. PMID:23582642).'),
      'report' => dt("Set to the email address of the recipient who should receive an HTML report of the publications that have been added. Not applicable with the --dbxref option."),
      'update' => dt("Set to 'Y' to update existing pubs.  By default only new pubs are inserted."),
      'publish' => dt("Set to 'Y' to publish publications after import into Chado. (default: Y)."),
      'sync' => dt("For Tripal v3 legacy mode only. Set to 'Y' to sync publications after import into Chado. This will create legacy \"nodes\" rather than the newer Tripal v3 entities."),
      'username' => [
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
      ],
    ],
    'examples' => [
      'Standard example' => 'drush trp-import-pubs --username=[username] --report=[email]. Where [site url] is the URL of the website and [email] is the email address of the recipient to receive the HTML report',
      'Import single publication' => 'drush tripal-pub-import --username=[username] --dbxref=PMID:23582642',
    ],
  ];
  $items['trp-update-pubs'] = [
    'description' => dt('Updates publication information for publications with a supported database cross-reference.'),
    'options' => [
      'dbxref' => dt('An accession number for a publication from a remote database (e.g. PMID:23582642)'),
      'db' => dt('The database name (e.g. PMID or AGL)'),
      'publish' => dt("Set to 'Y' to publish publications after import into Chado. (default: Y)."),
      'sync' => dt("For Tripal v3 legacy mode only. Set to 'Y' to sync publications after import into Chado. This will create legacy \"nodes\" rather than the newer Tripal v3 entities."),
      'username' => [
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
      ],
    ],
    'examples' => [
      'Standard example' => 'drush trp-update-pubs --username=[username]',
      'Update a single record' => 'drush trp-update-pubs --username=[username] --dbxref=PMID:23582642',
      'Update all records for a single database' => 'drush trp-update-pubs --username=[username] --db=PMID',
    ],
  ];

  return $items;
}

/**
 * Imports publications into Chado
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_trp_import_pubs() {
  // The create contacts only goes with the dbxref argument
  $create_contacts = drush_get_option('create_contacts');
  $dbxref = drush_get_option('dbxref');

  // The do_report, publish, sync only go with the normal importer.
  $do_report = drush_get_option('report');
  $publish = drush_get_option('publish');
  $sync = drush_get_option('sync');
  $update = drush_get_option('update');
  $uname = drush_get_option('username');

  drush_tripal_set_user($uname);

  if ($update == 'Y') {
    $update = TRUE;
  }
  else {
    $update = FALSE;
  }


  if ($publish == 'Y' and $sync == 'Y') {
    $publish = 'both';
  }
  elseif ($publish != 'Y' and $sync == 'Y') {
    $publish = 'sync';
  }
  elseif ($publish == 'Y' and $sync != 'Y') {
    $publish = TRUE;
  }
  else {
    $publish = FALSE;
  }

  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.pub_importers');
  if ($dbxref) {
    chado_import_pub_by_dbxref($dbxref, $create_contacts, $publish, $update);
  }
  else {
    chado_execute_active_pub_importers($do_report, $publish, $update);
  }
}

/**
 * Imports publications into Chado
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_trp_update_pubs() {
  $create_contacts = drush_get_option('create_contacts');
  $dbxref = drush_get_option('dbxref');
  $db = drush_get_option('db');
  $publish = drush_get_option('publish');
  $sync = drush_get_option('sync');

  $uname = drush_get_option('username');
  drush_tripal_set_user($uname);

  if ($publish == 'Y' and $sync == 'Y') {
    $publish = 'both';
  }
  elseif ($publish != 'Y' and $sync == 'Y') {
    $publish = 'sync';
  }
  elseif ($publish == 'Y' and $sync != 'Y') {
    $publish = TRUE;
  }
  else {
    $publish = FALSE;
  }

  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.pub_importers');
  chado_reimport_publications($create_contacts, $dbxref, $db, $publish);
}

/**
 * Set the user to run a drush job.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_chado_set_user($username) {
  if ($username) {
    $sql = "SELECT uid FROM {users} WHERE name = :name";
    $results = db_query($sql, [':name' => $username]);
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
  $mview_id = drush_get_option('mview');
  $table_name = drush_get_option('table');

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
      drush_set_error('Plese provide one option of --mview or --table.');
    }
  }

  drush_print('Updating the Materialized View with ID=' . $mview_id);
  $status = chado_populate_mview($mview_id);
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



