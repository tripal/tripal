<?php
/**
 * @file
 * Functions to install chado schema through Drupal
 */

/**
 * Load Chado Schema Form
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_form() {

  // we want to force the version of Chado to be set properly
  $real_version = chado_get_version(TRUE);

  // get the effective version.  Pass true as second argument
  // to warn the user if the current version is not compatible
  $version = chado_get_version(FALSE, TRUE);

  if ($real_version == '1.2') {
    drupal_set_message('Please note: the upgrade of Chado from v1.2 to v1.3 may
        require a fix to your materialized views.  All of the primary keys
        in Chado were changed from integers to big integers to support larger
        tables.  If your materialized views uses these fields you may need to
        alter and repopulate those views.  Additionally, if you have made
        any custom PL/pgSQL functions that expect primary and foreign key fields
        to be integers, then those functions will need to be correct.
        The Tripal upgrader is not able to fix these problems automatically',
        'warning');
  }


  $form['current_version'] = array(
    '#type' => 'item',
    '#title' => t("Current installed version of Chado:"),
    '#description' => $real_version,
  );

  $form['action_to_do'] = array(
     '#type' => 'radios',
     '#title' => 'Installation/Upgrade Action',
     '#options' => array(
        'Install Chado v1.3' => t('New Install of Chado v1.3 (erases all existing Chado data if Chado already exists)'),
        'Upgrade Chado v1.2 to v1.3' => t('Upgrade existing Chado v1.2 to v1.3 (no data is lost)'),
        'Install Chado v1.2' => t('New Install of Chado v1.2 (erases all existing Chado data if Chado already exists)'),
        'Upgrade Chado v1.11 to v1.2' => t('Upgrade existing Chado v1.11 to v1.2 (no data is lost)'),
        'Install Chado v1.11' => t('New Install of Chado v1.11 (erases all existing Chado data if Chado already exists)'),
     ),
     '#description' => t('Select an action to perform. If you want to install Chado all other Tripal modules must not be installed.'),
     '#required' => TRUE,
  );

  $form['warning'] = array(
    '#markup' => "<div><font color=\"red\">WARNING:</font>" . t('A new install of
      Chado will remove and recreate the Chado database if it already exists.') . '</div>',
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Install/Upgrade Chado'),
  );

  return $form;
}

function tripal_chado_install_form_validate($form, &$form_state) {

  if ($form_state['values']['action_to_do'] == "Upgrade Chado v1.11 to v1.2") {
    // Make sure we are already not at v1.2
    $real_version = chado_get_version(TRUE);
    if ($real_version == "1.2") {
      form_set_error("action_to_do", "You are already at v1.2.  There is no need to upgrade.");
    }
  }
  if ($form_state['values']['action_to_do'] == "Upgrade Chado v1.2 to v1.3") {
    // Make sure we are already not at v1.2
    $real_version = chado_get_version(TRUE);
    if ($real_version == "1.3") {
      form_set_error("action_to_do", "You are already at v1.3.  There is no need to upgrade.");
    }
  }
}
/**
 * Submit Load Chado Schema Form
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_form_submit($form, &$form_state) {
  global $user;
  $action_to_do = trim($form_state['values']['action_to_do']);
  $args = array($action_to_do);
  $includes = array(module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.install'));
  tripal_add_job($action_to_do, 'tripal_chado',
      'tripal_chado_install_chado', $args, $user->uid, 10, $includes);
}

/**
 * Install Chado Schema
 *
 * @ingroup tripal_chado
 */
function tripal_chado_install_chado($action) {

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
      chado_query($vsql, array(':version' => '1.3'));
    }
    elseif ($action == 'Upgrade Chado v1.2 to v1.3') {
      tripal_chado_upgrade_chado_1_2_to_1_3();
      chado_query($vusql, array(':version' => '1.3'));
    }
    elseif ($action == 'Install Chado v1.2') {
      tripal_chado_install_chado_1_2();
      chado_query($vsql, array(':version' => '1.2'));
    }
    elseif ($action == 'Upgrade Chado v1.11 to v1.2') {
      tripal_chado_upgrade_chado_1_11_to_1_2();
      chado_query($vsql, array(':version' => '1.2'));
    }
    elseif ($action == 'Install Chado v1.11') {
      tripal_chado_install_chado_1_11();
    }
  }
  catch (Exception $e) {
    $transaction->rollback();
    tripal_chado_install_done();
    tripal_report_error('tripal_chado', TRIPAL_ERROR, $e->getMessage(), array('print' => TRUE));
    return FALSE;
  }
  return TRUE;

  // Set a variable to indicate the site is prepared.
  variable_set('tripal_chado_is_prepared', FALSE);

  module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.setup');

  tripal_chado_set_globals();
  tripal_chado_init();
  tripal_chado_prepare_chado();

  return TRUE;
}

/**
 * Installs Chado v1.3.
 */
function tripal_chado_install_chado_1_3() {
  // Get the path to the schema and init SQL files.
  $schema_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/default_schema-1.3.sql';
  $init_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/initialize-1.3.sql';

  // Erase the Chado schema if it exists and perform the install.
  if (tripal_chado_reset_chado_schema()) {
    $success = tripal_chado_install_sql($schema_file);
    if ($success) {
      print "Install of Chado v1.3 (Step 1 of 2) Successful!\n";
    }
    else {
      throw new Exception("Installation (Step 1 of 2) Problems!  Please check output above for errors.");
    }
    $success = tripal_chado_install_sql($init_file);
    if ($success) {
      print "Install of Chado v1.3 (Step 2 of 2) Successful.\nInstallation Complete\n";
    }
    else {
      throw new Exception("Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }
  }
  else {
    throw new Exception("ERROR: cannot install chado.  Please check database permissions");
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
      print "Install of Chado v1.2 (Step 1 of 2) Successful!\n";
    }
    else {
      throw new Exception("Installation (Step 1 of 2) Problems!  Please check output above for errors.");
    }
    $success = tripal_chado_install_sql($init_file);
    if ($success) {
      print "Install of Chado v1.2 (Step 2 of 2) Successful.\nInstallation Complete\n";
    }
    else {
      throw new Exception("Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }
  }
  else {
    throw new Exception("ERROR: cannot install chado.  Please check database permissions");
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
      print "Install of Chado v1.11 (Step 1 of 2) Successful!\n";
    }
    else {
      throw new Exception("Installation (Step 1 of 2) Problems!  Please check output above for errors.");
    }
    $success = tripal_chado_install_sql($init_file);
    if ($success) {
      print "Install of Chado v1.11 (Step 2 of 2) Successful.\nInstallation Complete!\n";
    }
    else {
      throw new Exception("Installation (Step 2 of 2) Problems!  Please check output above for errors.");
    }
  }
  else {
    throw new Exception("ERROR: cannot install chado.  Please check database permissions");
  }
}
/**
 * Upgrades Chado from v1.2 to v1.3
 */
function tripal_chado_upgrade_chado_1_2_to_1_3() {

  // Get the path to the diff schema and upgrade SQL files.
  $diff_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/default_schema-1.2-1.3-diff.sql';

  $success = tripal_chado_install_sql($diff_file);
  if ($success) {
    print "Upgrade from v1.2 to v1.3 Successful!\n";
  }
  else {
    throw new Exception("Upgrade problems!  Please check output above for errors.");
  }
}
/**
 * Upgrades Chado from v1.11 to v1.2
 */
function tripal_chado_upgrade_chado_1_11_to_1_2() {

  // Get the path to the schema diff and upgarde SQL files.
  $schema_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/default_schema-1.11-1.2-diff.sql';
  $init_file = drupal_get_path('module', 'tripal_chado') .
    '/chado_schema/upgrade-1.11-1.2.sql';

  $success = tripal_chado_install_sql($schema_file);
  if ($success) {
    print "Upgrade from v1.11 to v1.2 (Step 1 of 2) Successful!\n";
  }
  else {
    throw new Exception("Upgrade (Step 1 of 2) problems!  Please check output above for errors.");
  }
  $success = tripal_chado_install_sql($init_file);
  if ($success) {
    print "Upgrade from v1.11 to v1.2 (Step 2 of 2) Successful.\nUpgrade Complete!\n";
  }
  else {
    throw new Exception("Upgrade (Step 2 of 2) problems!  Please check output above for errors.");
  }
}

/**
 * Reset the Chado Schema
 * This drops the current chado and chado-related schema and re-creates it
 *
 * @ingroup tripal_chado
 */
function tripal_chado_reset_chado_schema() {

  // drop current chado and chado-related schema
  if (chado_dbschema_exists('genetic_code')) {
    print "Dropping existing 'genetic_code' schema\n";
    db_query("drop schema genetic_code cascade");
  }
  if (chado_dbschema_exists('so')) {
    print "Dropping existing 'so' schema\n";
    db_query("drop schema so cascade");
  }
  if (chado_dbschema_exists('frange')) {
    print "Dropping existing 'frange' schema\n";
    db_query("drop schema frange cascade");
  }
  if (chado_dbschema_exists('chado')) {
    print "Dropping existing 'chado' schema\n";
    db_query("drop schema chado cascade");
  }

  // create the new chado schema
  print "Creating 'chado' schema\n";
  db_query("create schema chado");
  if (chado_dbschema_exists('chado')) {
    // before creating the plpgsql language let's check to make sure
    // it doesn't already exists
    $sql = "SELECT COUNT(*) FROM pg_language WHERE lanname = 'plpgsql'";
    $results = db_query($sql);
    $count = $results->fetchObject();
    if (!$count or $count->count == 0) {
      db_query("create language plpgsql");
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

  $chado_local = chado_dbschema_exists('chado');

  if ($chado_local) {
    db_query("set search_path to chado");
  }
  print "Loading $sql_file...\n";
  $lines = file($sql_file, FILE_SKIP_EMPTY_LINES);

  if (!$lines) {
    return 'Cannot open $schema_file';
  }

  $stack = array();
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
    if (preg_match('/^\s*$/', $line) or strcmp($line, '')==0) {
      continue;
    }
    // Find SQL for new objects
    if (preg_match('/^\s*CREATE\s+TABLE/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'table';
      $line = preg_replace("/public\./", "chado.", $line);
    }
    if (preg_match('/^\s*ALTER\s+TABLE\s+/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'alter_table';
      $line = preg_replace("/public\./", "chado.", $line);
    }
    if (preg_match('/^\s*SET/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'set';
    }
    if (preg_match('/^\s*CREATE\s+SCHEMA/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'schema';
    }
    if (preg_match('/^\s*CREATE\s+SEQUENCE/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'sequence';
      $line = preg_replace("/public\./", "chado.", $line);
    }
    if (preg_match('/^\s*CREATE\s+(?:OR\s+REPLACE\s+)*VIEW/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'view';
      $line = preg_replace("/public\./", "chado.", $line);
    }
    if (preg_match('/^\s*COMMENT/i', $line) and !$in_string and sizeof($stack)==0 and !$in_function) {
      $stack[] = 'comment';
      $line = preg_replace("/public\./", "chado.", $line);
    }
    if (preg_match('/^\s*CREATE\s+(?:OR\s+REPLACE\s+)*FUNCTION/i', $line) and !$in_string and !$in_function) {
      $in_function = TRUE;
      $stack[] = 'function';
      $line = preg_replace("/public\./", "chado.", $line);
    }
    if (preg_match('/^\s*CREATE\s+INDEX/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'index';
    }
    if (preg_match('/^\s*INSERT\s+INTO/i', $line) and !$in_string and !$in_function) {
      $stack[] = 'insert';
      $line = preg_replace("/public\./", "chado.", $line);
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
    if (strcmp($stack[sizeof($stack)-1], 'table') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'alter_table') == 0 and preg_match('/;\s*$/', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'set') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'schema') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'sequence') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'view') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'comment') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'function') == 0) {

      if(preg_match('/LANGUAGE.*?;\s*$/i', $line)) {
        $type = array_pop($stack);
        $in_function = FALSE;
        //print "FUNCTION DONE ($i): $line";
      }
      else if(preg_match('/\$_\$;\s*$/i', $line)) {
        $type = array_pop($stack);
        $in_function = FALSE;
        //print "FUNCTION DONE ($i): $line";
      }
      else if(preg_match('/\$\$;\s*$/i', $line)) {
        $type = array_pop($stack);
        $in_function = FALSE;
        // print "FUNCTION DONE ($i): $line";
      }
      else {
        // print "FUNCTION ($i): $line";
      }
    }
    if (strcmp($stack[sizeof($stack)-1], 'index') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'insert') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'type') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'grant') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'aggregate') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'drop_function') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'drop_view') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'drop_index') == 0 and preg_match("/;\s*$/i", $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'drop_seq') == 0 and preg_match("/;\s*$/i", $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'alter_type') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'alter_seq') == 0 and preg_match('/;\s*$/i', $line)) {
      $type = array_pop($stack);
    }
    // if we're in a recognized SQL statement then let's keep track of lines
    if ($type or sizeof($stack) > 0) {
      $query .= "$line";
    }
    else {
      throw new Exception("UNHANDLED $i, $in_string: $line");
    }
    if (preg_match_all("/\n/", $query, $temp) > 1000) {
      throw new Exception("SQL query is too long.  Terminating:\n$query\n");
    }
    if ($type and sizeof($stack) == 0) {
      //print "Adding $type: line $i\n";
      // rewrite the set search_path to make 'public' be 'chado', but only if the
      // chado schema exists
      if (strcmp($type, 'set') == 0 and $chado_local) {
        $query = preg_replace("/public/m", "chado", $query);
      }

      // execute the statement
      try {
        $result = db_query($query);
      }
      catch (Exception $e) {
        $error  = $e->getMessage();
        throw new Exception("FAILED. Line  $i, $in_string\n$error:\n$query\n\n");
      }

      if (!$result) {
        $error  = pg_last_error();
        throw new Exception("FAILED. Line  $i, $in_string\n$error:\n$query\n\n");
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

  db_query("set search_path to default");
}
