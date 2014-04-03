<?php
/**
 * @file
 * Functions to install chado schema through Drupal
 */

/**
 * Load Chado Schema Form
 *
 * @ingroup tripal_core
 */
function tripal_core_chado_load_form() {

  // we want to force the version of Chado to be set properly
  $real_version = chado_get_version(TRUE);

  // get the effective version.  Pass true as second argument
  // to warn the user if the current version is not compatible
  $version = chado_get_version(FALSE, TRUE);

  $form['current_version'] = array(
    '#type' => 'item',
    '#title' => t("Current installed version of Chado:"),
    '#description' => $real_version,
  );

  $form['action_to_do'] = array(
     '#type' => 'radios',
     '#title' => 'Installation/Upgrade Action',
     '#options' => array(
        'Install Chado v1.2' => t('New Install of Chado v1.2 (erases all existing Chado data if Chado already exists)'),
        'Upgrade Chado v1.11 to v1.2' => t('Upgrade existing Chado v1.11 to v1.2 (no data is lost)'),
        'Install Chado v1.11' => t('New Install of Chado v1.11 (erases all existing Chado data if Chado already exists)')
     ),
     '#description' => t('Select an action to perform'),
     '#required' => TRUE
  );

  $form['description'] = array(
    '#type' => 'item',
    '#value' => t("<font color=\"red\">WARNING:</font> A new install of Chado v1.2 or v1.11 "
      . "will install Chado within the Drupal database in a \"chado\" schema. If the \"chado\" schema already exists it will "
      . "be overwritten and all data will be lost.  You may choose to update an existing Chado v1.11 if it was installed with a previous "
      . "version of Tripal (e.g. v0.3b or v0.3.1). The update will not erase any data. "
      . "If you are using chado in a database external to the "
      . "Drupal database with a 'chado' entry in the 'settings.php' \$db_url argument "
      . "then Chado will be installed but will not be used .  The external "
      . "database specified in the settings.php file takes precedence."),
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Install/Upgrade Chado'),
    '#weight' => 2,
  );

  return $form;
}

/**
 * Submit Load Chado Schema Form
 *
 * @ingroup tripal_core
 */
function tripal_core_chado_load_form_submit($form, &$form_state) {
  global $user;
  $action_to_do = trim($form_state['values']['action_to_do']);
  $args = array($action_to_do);
  tripal_add_job($action_to_do, 'tripal_core', 'tripal_core_install_chado', $args, $user->uid);
}

/**
 * Install Chado Schema
 *
 * @ingroup tripal_core
 */
function tripal_core_install_chado($action) {

  $vsql = "
    INSERT INTO {chadoprop} (type_id, value)
      VALUES (
       (SELECT cvterm_id
        FROM {cvterm} CVT
          INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
         WHERE CV.name = 'chado_properties' AND CVT.name = 'version'),
       :version)
  ";

  if ($action == 'Install Chado v1.2') {
    $schema_file = drupal_get_path('module', 'tripal_core') . '/chado_schema/default_schema-1.2.sql';
    $init_file = drupal_get_path('module', 'tripal_core') . '/chado_schema/initialize-1.2.sql';
    if (tripal_core_reset_chado_schema()) {
      $success = tripal_core_install_sql($schema_file);
      if ($success) {
        print "Install of Chado v1.2 (Step 1 of 2) Successful!\n";
      }
      else {
        print "Installation (Step 1 of 2) Problems!  Please check output above for errors.\n";
        exit;
      }
      $success = tripal_core_install_sql($init_file);
      if ($success) {
        print "Install of Chado v1.2 (Step 2 of 2) Successful.\nInstallation Complete\n";
      }
      else {
        print "Installation (Step 2 of 2) Problems!  Please check output above for errors.\n";
        exit;
      }
      chado_query($vsql, array(':version' => '1.2')); # set the version
    }
    else {
      print "ERROR: cannot install chado.  Please check database permissions\n";
      exit;
    }
  }
  elseif ($action == 'Upgrade Chado v1.11 to v1.2') {
    $schema_file = drupal_get_path('module', 'tripal_core') . '/chado_schema/default_schema-1.11-1.2-diff.sql';
    $init_file = drupal_get_path('module', 'tripal_core') . '/chado_schema/upgrade-1.11-1.2.sql';
    $success = tripal_core_install_sql($schema_file);
    if ($success) {
      print "Upgrade from v1.11 to v1.2 (Step 1 of 2) Successful!\n";
    }
    else {
      print "Upgrade (Step 1 of 2) problems!  Please check output above for errors.\n";
      exit;
    }
    $success = tripal_core_install_sql($init_file);
    if ($success) {
      print "Upgrade from v1.11 to v1.2 (Step 2 of 2) Successful.\nUpgrade Complete!\n";
    }
    else {
      print "Upgrade (Step 2 of 2) problems!  Please check output above for errors.\n";
      exit;
    }
    chado_query($vsql, array(':version' => '1.2')); # set the version
  }
  elseif ($action == 'Install Chado v1.11') {
    $schema_file = drupal_get_path('module', 'tripal_core') . '/chado_schema/default_schema-1.11.sql';
    $init_file = drupal_get_path('module', 'tripal_core') . '/chado_schema/initialize-1.11.sql';
    if (tripal_core_reset_chado_schema()) {
      $success = tripal_core_install_sql($schema_file);
      if ($success) {
        print "Install of Chado v1.11 (Step 1 of 2) Successful!\n";
      }
      else {
        print "Installation (Step 1 of 2) Problems!  Please check output above for errors.\n";
        exit;
      }
      $success = tripal_core_install_sql($init_file);
      if ($success) {
        print "Install of Chado v1.11 (Step 2 of 2) Successful.\nInstallation Complete!\n";
      }
      else {
        print "Installation (Step 2 of 2) Problems!  Please check output above for errors.\n";
        exit;
      }
    }
    else {
      print "ERROR: cannot install chado.  Please check database permissions\n";
      exit;
    }
  }
}

/**
 * Reset the Chado Schema
 * This drops the current chado and chado-related schema and re-creates it
 *
 * @ingroup tripal_core
 */
function tripal_core_reset_chado_schema() {

  // drop current chado and chado-related schema
  if (chado_dbschema_exists('chado')) {
    print "Dropping existing 'chado' schema\n";
    db_query("drop schema chado cascade");
  }
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
 * @ingroup tripal_core
 */
function tripal_core_install_sql($sql_file) {

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
    if (preg_match('/^\s*CREATE\s+TABLE/i', $line) and !$in_string) {
      $stack[] = 'table';
      $line = preg_replace("/public./", "chado.", $line);
    }
    if (preg_match('/^\s*ALTER\s+TABLE/i', $line) and !$in_string) {
      $stack[] = 'alter table';
      $line = preg_replace("/public./", "chado.", $line);
    }
    if (preg_match('/^\s*SET/i', $line) and !$in_string) {
      $stack[] = 'set';
    }
    if (preg_match('/^\s*CREATE\s+SCHEMA/i', $line) and !$in_string) {
      $stack[] = 'schema';
    }
    if (preg_match('/^\s*CREATE\s+SEQUENCE/i', $line) and !$in_string) {
      $stack[] = 'sequence';
      $line = preg_replace("/public./", "chado.", $line);
    }
    if (preg_match('/^\s*CREATE\s+(?:OR\s+REPLACE\s+)*VIEW/i', $line) and !$in_string) {
      $stack[] = 'view';
      $line = preg_replace("/public./", "chado.", $line);
    }
    if (preg_match('/^\s*COMMENT/i', $line) and !$in_string and sizeof($stack)==0) {
      $stack[] = 'comment';
      $line = preg_replace("/public./", "chado.", $line);
    }
    if (preg_match('/^\s*CREATE\s+(?:OR\s+REPLACE\s+)*FUNCTION/i', $line) and !$in_string) {
      $stack[] = 'function';
      $line = preg_replace("/public./", "chado.", $line);
    }
    if (preg_match('/^\s*CREATE\s+INDEX/i', $line) and !$in_string) {
      $stack[] = 'index';
    }
    if (preg_match('/^\s*INSERT\s+INTO/i', $line) and !$in_string) {
      $stack[] = 'insert';
      $line = preg_replace("/public./", "chado.", $line);
    }
    if (preg_match('/^\s*CREATE\s+TYPE/i', $line) and !$in_string) {
      $stack[] = 'type';
    }
    if (preg_match('/^\s*GRANT/i', $line) and !$in_string) {
      $stack[] = 'grant';
    }
    if (preg_match('/^\s*CREATE\s+AGGREGATE/i', $line) and !$in_string) {
      $stack[] = 'aggregate';
    }

    // determine if we are in a string that spans a line
    $matches = preg_match_all("/[']/i", $line, $temp);
    $in_string = $in_string - ($matches % 2);
    $in_string = abs($in_string);

    // if we've reached the end of an object the pop the stack
    if (strcmp($stack[sizeof($stack)-1], 'table') == 0 and preg_match('/\);\s*$/', $line)) {
      $type = array_pop($stack);
    }
    if (strcmp($stack[sizeof($stack)-1], 'alter table') == 0 and preg_match('/;\s*$/', $line) and !$in_string) {
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
    if (strcmp($stack[sizeof($stack)-1], 'function') == 0 and preg_match("/LANGUAGE.*?;\s+$/i", $line)) {
      $type = array_pop($stack);
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
    // if we're in a recognized SQL statement then let's keep track of lines
    if ($type or sizeof($stack) > 0) {
      $query .= "$line";
    }
    else {
      print "UNHANDLED $i, $in_string: $line";
      tripal_core_chado_install_done();
      return FALSE;
    }
    if (preg_match_all("/\n/", $query, $temp) > 100) {
      print "SQL query is too long.  Terminating:\n$query\n";
      tripal_core_chado_install_done();
      return FALSE;
    }
    if ($type and sizeof($stack) == 0) {
      //print "Adding $type: line $i\n";
      // rewrite the set search_path to make 'public' be 'chado', but only if the
      // chado schema exists
      if (strcmp($type, 'set') == 0 and $chado_local) {
        $query = preg_replace("/public/m", "chado", $query);
      }

      // execute the statement
      $result = db_query($query);

      if (!$result) {
        $error  = pg_last_error();
        print "FAILED. Line  $i, $in_string\n$error:\n$query\n\n";
        tripal_core_chado_install_done();
        $success = 0;
        return $success;
      }
      $query = '';
    }
  }
  tripal_core_chado_install_done();
  return $success;
}

/**
 * Finish the Chado Schema Installation
 *
 * @ingroup tripal_core
 */
function tripal_core_chado_install_done() {

  db_query("set search_path to default");
}
