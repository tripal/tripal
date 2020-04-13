<?php
/**
 * @file
 * Functions to install chado schema through Drupal
 *
 * @upgrade
 *
 */
use Drupal\Core\Database\Database;

/**
 * Submit Load Chado Schema Form
 *
 * @ingroup tripal_chado
 */
function tripal_chado_load_drush_submit($version) {
  $args = [$version];
  $includes = [mmodule_load_include('inc', 'tripal_chado', 'src/LegacyIncludes/tripal_chado.install')];
  // @upgrade tripal_add_job($version, 'tripal_chado',
  //  'tripal_chado_install_chado', $args, 1, 10, $includes);
}

/**
 * Install Chado Schema
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_chado($action, TripalJob $job = NULL) {

  $vsql = "
    INSERT INTO {chadoprop} (type_id, value)
      VALUES (
       (SELECT cvterm_id
        FROM {cvterm} CVT
          INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
         WHERE CV.name = 'chado_properties' AND CVT.name = 'version'),
       :version)
  ";

  $vusql = "
    UPDATE {chadoprop}
    SET value = :version
    WHERE type_id = (SELECT cvterm_id
        FROM {cvterm} CVT
          INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
         WHERE CV.name = 'chado_properties' AND CVT.name = 'version')
  ";

  $transaction = db_transaction();
  try {
    if ($action == 'Install Chado v1.3') {
      tripal_chado_install_chado_1_3();
      // @upgrade chado_query($vsql, [':version' => '1.3']);
    }
    elseif ($action == 'Upgrade Chado v1.2 to v1.3') {
      tripal_chado_upgrade_chado_1_2_to_1_3();
      // @upgrade chado_query($vusql, [':version' => '1.3']);
    }
    elseif ($action == 'Install Chado v1.2') {
      tripal_chado_install_chado_1_2();
      // @upgrade chado_query($vsql, [':version' => '1.2']);
    }
    elseif ($action == 'Upgrade Chado v1.11 to v1.2') {
      tripal_chado_upgrade_chado_1_11_to_1_2();
      // @upgrade chado_query($vsql, [':version' => '1.2']);
    }
    elseif ($action == 'Install Chado v1.11') {
      tripal_chado_install_chado_1_11();
    }
  } catch (Exception $e) {
    $transaction->rollback();
    tripal_chado_install_done();
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error($e->getMessage());
    return FALSE;
  }
  return TRUE;


}

/**
 * Installs Chado v1.3.
 */
function tripal_chado_install_chado_1_3(TripalJob $job = NULL) {
  // Get the path to the schema and init SQL files.
  $schema_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/default_schema-1.3.sql';
  $init_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/initialize-1.3.sql';

  // Erase the Chado schema if it exists and perform the install.
  if (tripal_chado_reset_chado_schema()) {
    $success = tripal_chado_install_sql($schema_file);
    if ($success) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.3 (Step 1 of 2) Successful!\n");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 1 of 2) Problems!  Please check output above for errors.");
    }
    $success = tripal_chado_install_sql($init_file);
    if ($success) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.3 (Step 2 of 2) Successful.\nInstallation Complete\n");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }
  }
  else {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error(
      "ERROR: cannot install chado.  Please check database permissions");
  }
}

/**
 * Installs Chado v1.2.
 */
function tripal_chado_install_chado_1_2() {
  // Get the path to the schema and init SQL files.
  $schema_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/default_schema-1.2.sql';
  $init_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/initialize-1.2.sql';

  // Erase the Chado schema if it exists and perform the install.
  if (tripal_chado_reset_chado_schema()) {
    $success = tripal_chado_install_sql($schema_file);
    if ($success) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.2 (Step 1 of 2) Successful!\n");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 1 of 2) Problems!  Please check output above for errors.");
    }
    $success = tripal_chado_install_sql($init_file);
    if ($success) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.2 (Step 2 of 2) Successful.\nInstallation Complete\n");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }
  }
  else {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error(
      "ERROR: cannot install chado.  Please check database permissions");
  }
}

/**
 *
 */
function tripal_chado_install_chado_1_11() {

  // Get the path to the schema and init SQL files.
  $schema_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/default_schema-1.11.sql';
  $init_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/initialize-1.11.sql';

  // Erase the Chado schema if it exists and perform the install.
  if (tripal_chado_reset_chado_schema()) {
    $success = tripal_chado_install_sql($schema_file);
    if ($success) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.11 (Step 1 of 2) Successful!\n");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 1 of 2) Problems!  Please check output above for errors.");
    }
    $success = tripal_chado_install_sql($init_file);
    if ($success) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->notice(
        "Install of Chado v1.11 (Step 2 of 2) Successful.\nInstallation Complete!\n");
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }
  }
  else {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error(
      "ERROR: cannot install chado.  Please check database permissions");
  }
}

/**
 * Upgrades Chado from v1.2 to v1.3
 */
function tripal_chado_upgrade_chado_1_2_to_1_3() {

  // Upgrade some of the custom tables that Tripal created that are now in
  // Chado v1.3.  We'll do this ahead of time because the upgrade script won't
  // upgrade tables if they already exist.
  // @upgrade tripal_report_error().
  \Drupal::logger('tripal_chado')->info(
    "Checking for existing v1.3 tables in v1.2 and fixing bigints...\n");
  tripal_chado_upgrade_chado_1_2_to_1_3_pre_alter();

  // Get the path to the diff schema and upgrade SQL files.
  // @upgrade tripal_report_error().
  \Drupal::logger('tripal_chado')->info(
    "Incorporating additional changes...\n");
  $diff_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/default_schema-1.2-1.3-diff.sql';

  $success = tripal_chado_install_sql($diff_file);
  if ($success) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->notice(
      "Upgrade from v1.2 to v1.3 Successful!\n");
  }
  else {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error(
      "Upgrade problems!  Please check output above for errors.");
  }
}

/**
 * Upgrade custom tables that may match the tables now in Chado v1.3.
 *
 * There were many new tables that were added to Chado v1.3 that were
 * suggested by the Chado user community.  Some of those were Tripal users.
 * Therefore, to help these Tripal users upgrade more seemlessly this function
 * checks if those custom tables already exists, and if so updates them as
 * best it can to match.  At a minimum it will create the table if it doesn't
 * exist and if it does it will change the primary keys and foreign keys to
 * be big ints.
 */
function tripal_chado_upgrade_chado_1_2_to_1_3_pre_alter() {

  // Include the Chado v1.3 schema definitions.
  module_load_include('inc', 'tripal_chado', '/api/tripal_chado.schema_v1.3.api');

  // The list of new tables in Chado v1.3
  $new_tables = [
    'analysis_cvterm',
    'analysis_dbxref',
    'analysis_pub',
    'analysis_relationship',
    'contactprop',
    'dbprop',
    'feature_contact',
    'featuremap_contact',
    'featuremap_dbxref',
    'featuremap_organism',
    'featuremapprop',
    'featureposprop',
    'library_contact',
    'library_expression',
    'library_expressionprop',
    'library_featureprop',
    'library_relationship',
    'library_relationship_pub',
    'nd_experiment_analysis',
    'organism_cvterm',
    'organism_cvtermprop',
    'organism_pub',
    'organism_relationship',
    'organismprop_pub',
    'phenotypeprop',
    'phylotreeprop',
    'project_analysis',
    'project_dbxref',
    'project_feature',
    'project_stock',
    'pubauthor_contact',
    'stock_feature',
    'stock_featuremap',
    'stock_library',
    'stockcollection_db',
  ];

  // Get the name of the chado schema.
  $chado_schema = chado_get_schema_name('chado');

  // Iterate through the new Chado tables and create them or if they already
  // exist then update them.
  foreach ($new_tables as $table) {

    // Get the schema for this table.
    $function = 'tripal_chado_chado_schema_v1_3_' . $table;
    $schema = $function();

    // If the table exists then fix the pkeys and fkeys.
    if (chado_table_exists($table)) {

      // Update the primary key fields to be bigints.
      $fields = $schema['fields'];
      foreach ($fields as $field_name => $field) {
        if ($field['type'] == 'serial') {
          if (chado_column_exists($table, $field_name)) {
            $sql = 'ALTER TABLE {' . $table . '} ALTER COLUMN ' . $field_name . ' TYPE bigint';
            // @upgrade chado_query($sql);
          }
          else {
            // @upgrade tripal_report_error().
            \Drupal::logger('tripal_chado')->error(
              'Could not alter primary key to bigint: ' . $table . '.' . $field_name);
          }
        }
      }

      // Update the foreign key fields to be bigints.
      $fkeys = $schema['foreign keys'];
      foreach ($fkeys as $fktable => $details) {
        foreach ($details['columns'] as $leftkey => $rightkey) {
          if (chado_column_exists($table, $leftkey)) {
            $sql = 'ALTER TABLE {' . $table . '} ALTER COLUMN ' . $leftkey . ' TYPE bigint';
            // @upgrade chado_query($sql);
          }
          else {
            // @upgrade tripal_report_error().
            \Drupal::logger('tripal_chado')->error(
              'Could not alter foreign key to bigint: ' . $table . '.' . $leftkey);
          }
        }
      }

    }
  }
  // Now create the sequences if they don't already exist.
  $sequences = [
    'analysis_cvterm_analysis_cvterm_id_seq',
    'analysis_dbxref_analysis_dbxref_id_seq',
    'analysis_pub_analysis_pub_id_seq',
    'analysis_relationship_analysis_relationship_id_seq',
    'contactprop_contactprop_id_seq',
    'dbprop_dbprop_id_seq',
    'feature_contact_feature_contact_id_seq',
    'featuremap_contact_featuremap_contact_id_seq',
    'featuremap_dbxref_featuremap_dbxref_id_seq',
    'featuremap_organism_featuremap_organism_id_seq',
    'featuremapprop_featuremapprop_id_seq',
    'featureposprop_featureposprop_id_seq',
    'library_contact_library_contact_id_seq',
    'library_expression_library_expression_id_seq',
    'library_expressionprop_library_expressionprop_id_seq',
    'library_featureprop_library_featureprop_id_seq',
    'library_relationship_library_relationship_id_seq',
    'library_relationship_pub_library_relationship_pub_id_seq',
    'nd_experiment_analysis_nd_experiment_analysis_id_seq',
    'organism_cvterm_organism_cvterm_id_seq',
    'organism_cvtermprop_organism_cvtermprop_id_seq',
    'organism_pub_organism_pub_id_seq',
    'organism_relationship_organism_relationship_id_seq',
    'organismprop_pub_organismprop_pub_id_seq',
    'phenotypeprop_phenotypeprop_id_seq',
    'phylotreeprop_phylotreeprop_id_seq',
    'project_analysis_project_analysis_id_seq',
    'project_dbxref_project_dbxref_id_seq',
    'project_feature_project_feature_id_seq',
    'project_stock_project_stock_id_seq',
    'pubauthor_contact_pubauthor_contact_id_seq',
    'stock_feature_stock_feature_id_seq',
    'stock_featuremap_stock_featuremap_id_seq',
    'stock_library_stock_library_id_seq',
    'stockcollection_db_stockcollection_db_id_seq',
  ];
  foreach ($sequences as $sequence) {

    // Now add in the sequences if they don't already exist. There is no
    // PostgreSQL 'CREATE SEQUENCE IF NOT EXIST' so we're forced to do it here
    // and these create statements were removed from the diff upgrade file.
    if (!chado_sequence_exists($sequence)) {
      $sql = "CREATE SEQUENCE {" . $sequence . "} START WITH 1 INCREMENT BY 1 NO MINVALUE NO MAXVALUE CACHE 1";
      // @upgrade chado_query($sql);
    }
  }
}

/**
 * Upgrades Chado from v1.11 to v1.2
 */
function tripal_chado_upgrade_chado_1_11_to_1_2() {

  // Get the path to the schema diff and upgarde SQL files.
  $schema_file = drupal_get_path('module', 'tripal_chado') . '/chado_schema/default_schema-1.11-1.2-diff.sql';
  $init_file = drupal_get_path('module', 'tripal_chado') . '/chado_schema/upgrade-1.11-1.2.sql';

  $success = tripal_chado_install_sql($schema_file);
  if ($success) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->notice(
      "Upgrade from v1.11 to v1.2 (Step 1 of 2) Successful!\n");
  }
  else {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error(
      "Upgrade (Step 1 of 2) problems!  Please check output above for errors.");
  }
  $success = tripal_chado_install_sql($init_file);
  if ($success) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->notice(
      "Upgrade from v1.11 to v1.2 (Step 2 of 2) Successful.\nUpgrade Complete!\n");
  }
  else {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->error(
      "Upgrade (Step 2 of 2) problems!  Please check output above for errors.");
  }
}

/**
 * Reset the Chado Schema
 * This drops the current chado and chado-related schema and re-creates it
 *
 * @ingroup tripal_chado
 */
function tripal_chado_reset_chado_schema() {
  $connection = Database::getConnection();

  // determine the schema name.
  // @upgrade $chado_schema = chado_get_schema_name('chado');
  $chado_schema = 'chado';
  $chado_dot = $chado_schema . '.';

  // drop current chado and chado-related schema
  // @upgrade if (chado_dbschema_exists('genetic_code')) {
  if (TRUE) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->info(
      "Dropping existing 'genetic_code' schema\n");
    $connection->query("drop schema genetic_code cascade");
  }
  // @upgrade if (chado_dbschema_exists('so')) {
  if (TRUE) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->info(
      "Dropping existing 'so' schema\n");
    $connection->query("drop schema so cascade");
  }
  // @upgrade if (chado_dbschema_exists('frange')) {
  if (TRUE) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->info(
      "Dropping existing 'frange' schema\n");
    $connection->query("drop schema frange cascade");
  }
  // @upgrade if (chado_dbschema_exists($chado_schema)) {
  if (TRUE) {
    // @upgrade tripal_report_error().
    \Drupal::logger('tripal_chado')->info(
      "Dropping existing Chado ('$chado_schema') schema\n");
    $connection->query("drop schema $chado_schema cascade");
  }

  // create the new chado schema
  // @upgrade tripal_report_error().
  \Drupal::logger('tripal_chado')->info(
    "Creating 'chado' schema\n");
  $connection->query("create schema $chado_schema");
  // @upgrade if (chado_dbschema_exists('chado')) {
  if (TRUE) {
    // before creating the plpgsql language let's check to make sure
    // it doesn't already exists
    $sql = "SELECT COUNT(*) FROM pg_language WHERE lanname = 'plpgsql'";
    $results = $connection->query($sql);
    $count = $results->fetchObject();
    if (!$count or $count->count == 0) {
      $connection->query("create language plpgsql");
    }
    return TRUE;
  }

  return FALSE;
}

/**
 * Execute the provided SQL
 *
 * @param $sql_file
 *   Contains SQL statements to be executed
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_sql($sql_file) {
  $connection = Database::getConnection();

  // @upgrade $chado_local = chado_dbschema_exists('chado');
  $chado_local = TRUE;

  // determine the schema name.
  // @upgrade $chado_schema = chado_get_schema_name('chado');
  $chado_schema = 'chado';
  $chado_dot = $chado_schema . '.';

  if ($chado_local) {
    $connection->query("set search_path to $chado_schema");
  }
  // @upgrade tripal_report_error().
  print "Loading $sql_file...\n";
  \Drupal::logger('tripal_chado')->info(
    "Loading $sql_file...\n");
  $lines = file($sql_file, FILE_SKIP_EMPTY_LINES);

  if (!$lines) {
    return 'Cannot open $schema_file';
  }

  $stack = [];
  $in_string = 0;
  $in_function = FALSE;
  $query = '';
  $i = 0;
  $success = 1;
  foreach ($lines as $line_num => $line) {
    $i++;
    $type = '';
    // find and remove comments except when inside of strings
    if (preg_match('/--/', $line) and !$in_string and !preg_match("/'.*?--.*?'/", $line)) {
      $line = preg_replace('/--.*$/', '', $line);  // remove comments
    }
    if (preg_match('/\/\*.*?\*\//', $line)) {
      $line = preg_replace('/\/\*.*?\*\//', '', $line);  // remove comments
    }
    // skip empty lines
    if (preg_match('/^\s*$/', $line) or strcmp($line, '') == 0) {
      continue;
    }
    // Find SQL for new objects
    if (preg_match('/^\s*CREATE\s+TABLE/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'table';
      $line = preg_replace("/public\./", $chado_dot, $line);
    }
    if (preg_match('/^\s*ALTER\s+TABLE\s+/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'alter_table';
      $line = preg_replace("/public\./", $chado_dot, $line);
    }
    if (preg_match('/^\s*SET/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'set';
    }
    if (preg_match('/^\s*CREATE\s+SCHEMA/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'schema';
    }
    if (preg_match('/^\s*CREATE\s+SEQUENCE/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'sequence';
      $line = preg_replace("/public\./", $chado_dot, $line);
    }
    if (preg_match('/^\s*CREATE\s+(?:OR\s+REPLACE\s+)*VIEW/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'view';
      $line = preg_replace("/public\./", $chado_dot, $line);
    }
    if (preg_match('/^\s*COMMENT/i', $line) and !$in_string and sizeof($stack) == 0 and !$in_function) {
      $stack[] = 'comment';
      $line = preg_replace("/public\./", $chado_dot, $line);
    }
    if (preg_match('/^\s*CREATE\s+(?:OR\s+REPLACE\s+)*FUNCTION/i', $line) and !$in_string and !$in_function) {
      $in_function = TRUE;
      $stack[] = 'function';
      $line = preg_replace("/public\./", $chado_dot, $line);
    }
    if (preg_match('/^\s*CREATE\s+INDEX/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'index';
    }
    if (preg_match('/^\s*INSERT\s+INTO/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'insert';
      $line = preg_replace("/public\./", $chado_dot, $line);
    }
    if (preg_match('/^\s*CREATE\s+TYPE/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'type';
    }
    if (preg_match('/^\s*GRANT/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'grant';
    }
    if (preg_match('/^\s*CREATE\s+AGGREGATE/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'aggregate';
    }
    if (preg_match('/^\s*DROP\s+FUNCTION/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'drop_function';
    }
    if (preg_match('/^\s*DROP\s+VIEW/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'drop_view';
    }
    if (preg_match('/^\s*DROP\s+INDEX/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'drop_index';
    }
    if (preg_match('/^\s*DROP\s+SEQUENCE/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'drop_seq';
    }
    if (preg_match('/^\s*ALTER\s+TYPE\s+/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'alter_type';
    }
    if (preg_match('/^\s*ALTER\s+SEQUENCE\s+/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'alter_seq';
    }

    // determine if we are in a string that spans a line
    $matches = preg_match_all("/[']/i", $line, $temp);
    $in_string = $in_string - ($matches % 2);
    $in_string = abs($in_string);

    // if we've reached the end of an object then pop the stack
    if (strcmp($stack[sizeof($stack) - 1], 'table') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'alter_table') == 0 and preg_match('/;\s*$/', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'set') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'schema') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'sequence') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'view') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'comment') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'function') == 0) {

      if (preg_match('/LANGUAGE.*?;\s*$/i', $line)) {
        $type = array_pop($stack);
        $in_function = FALSE;
        // @upgrade tripal_report_error().
        \Drupal::logger('tripal_chado')->debug("FUNCTION DONE ($i): $line");
      }
      else {
        if (preg_match('/\$_\$;\s*$/i', $line)) {
          $type = array_pop($stack);
          $in_function = FALSE;
          // @upgrade tripal_report_error().
          \Drupal::logger('tripal_chado')->debug("FUNCTION DONE ($i): $line");
        }
        else {
          if (preg_match('/\$\$;\s*$/i', $line)) {
            $type = array_pop($stack);
            $in_function = FALSE;
            // @upgrade tripal_report_error().
            \Drupal::logger('tripal_chado')->debug("FUNCTION DONE ($i): $line");
          }
          else {
            // @upgrade tripal_report_error().
            \Drupal::logger('tripal_chado')->info("FUNCTION ($i): $line");
          }
        }
      }
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'index') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'insert') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'type') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'grant') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'aggregate') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'drop_function') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'drop_view') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'drop_index') == 0 and preg_match("/;\s*$/i", $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'drop_seq') == 0 and preg_match("/;\s*$/i", $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'alter_type') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    elseif (strcmp($stack[sizeof($stack) - 1], 'alter_seq') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    // if we're in a recognized SQL statement then let's keep track of lines
    if ($type or sizeof($stack) > 0) {
      $query .= "$line";
    }
    else {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "UNHANDLED $i, $in_string: $line");
    }
    if (preg_match_all("/\n/", $query, $temp) > 1000) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->error(
        "SQL query is too long.  Terminating:\n$query\n");
    }
    if ($type and sizeof($stack) == 0) {
      // @upgrade tripal_report_error().
      \Drupal::logger('tripal_chado')->debug("Adding $type: line $i\n");
      // rewrite the set search_path to make 'public' be 'chado', but only if the
      // chado schema exists
      if (strcmp($type, 'set') == 0 and $chado_local) {
        $query = preg_replace("/public/m", $chado_schema, $query);
      }

      // execute the statement
      try {
        $result = $connection->query($query);
      } catch (Exception $e) {
        $error = $e->getMessage();
        // @upgrade tripal_report_error().
        \Drupal::logger('tripal_chado')->error(
          "FAILED. Line  $i, $in_string\n$error:\n$query\n\n");
      }

      if (!$result) {
        $error = pg_last_error();
        // @upgrade tripal_report_error().
        \Drupal::logger('tripal_chado')->error(
          "FAILED. Line  $i, $in_string\n$error:\n$query\n\n");
      }
      $query = '';
    }
  }
  tripal_chado_install_done();
  return $success;
}

/**
 * Finish the Chado Schema Installation
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_done() {
  $connection = Database::getConnection();

  // @upgrade $drupal_schema = chado_get_schema_name('drupal');
  $drupal_schema = 'public';
  $connection->query("set search_path to $drupal_schema");
}
