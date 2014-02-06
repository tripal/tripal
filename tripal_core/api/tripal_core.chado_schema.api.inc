<?php

/**
 * @defgroup tripal_chado_schema_api Chado Schema API
 * @ingroup tripal_chado_api
 * @{
 * Provides an application programming interface (API) for describing Chado tables.
 * This API consists of a set of functions, one for each table in Chado.  Each
 * function simply returns a Drupal style array that defines the table.
 *
 * Because Drupal 6 does not handle foreign key (FK) relationships, however FK
 * relationships are needed to for Tripal Views.  Therefore, FK relationships
 * have been added to the schema defintitions below.
 *
 * The functions provided in this documentation should not be called as is, but if you need
 * the Drupal-style array definition for any table, use the following function
 * call:
 *
 *   $table_desc = chado_get_schema($table)
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The chado_get_schema function determines the appropriate version of
 * Chado and uses the Drupal hook infrastructure to call the appropriate
 * hook function to retrieve the table schema.
 * @}
 */

/**
 * Check that any given Chado table exists.  This function
 * is necessary because Drupa's db_table_exists will not
 * look in any other schema but the one were Drupal is installed
 *
 * @return
 *   TRUE/FALSE depending upon whether it exists
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_table_exists($table) {
  global $databases;

  $default_db = $databases['default']['default']['database'];

  $sql = "
    SELECT 1
    FROM information_schema.tables
    WHERE
      table_name = :table_name AND
      table_schema = 'chado' AND
      table_catalog = '$default_db'
  ";
  $results = db_query($sql, array(':table_name' => $table));
  $exists = $results->fetchObject();
  if (!$exists) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Check that any given schema exists
 *
 * @param $schema
 *   The name of the schema to check the existence of
 *
 * @return
 *   TRUE/FALSE depending upon whether or not the schema exists
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_dbschema_exists($schema) {

  // check that the chado schema now exists
  $sql = "
    SELECT nspname
    FROM pg_namespace
    WHERE
      has_schema_privilege(nspname, 'USAGE') AND
      nspname = :nspname
    ORDER BY nspname
  ";
  $results = db_query($sql, array(':nspname' => $schema));
  $name = $results->fetchObject();
  if (strcmp($name->nspname, $schema) != 0) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Check that the Chado schema exists within the local database
 *
 * @return
 *   TRUE/FALSE depending upon whether it exists
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_is_local() {

  // This is postgresql-specific code to check the existence of the chado schema
  // @coder-ignore: acting on pg_catalog schema rather then drupal schema therefore, table prefixing does not apply
  $sql = "
    SELECT nspname
    FROM pg_namespace
    WHERE
      has_schema_privilege(nspname, 'USAGE') AND
      nspname = 'chado'
  ";
  $results = db_query($sql);
  $name = $results->fetchObject();
  if ($name) {
    variable_set('chado_schema_exists', FALSE);
    return TRUE;
  }
  else {
    variable_set('chado_schema_exists', TRUE);
    return FALSE;
  }
}

/**
 * Check whether chado is installed (either in the same or a different database)
 *
 * @return
 *   TRUE/FALSE depending upon whether chado is installed.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_is_installed() {
  global $databases;

  // first check if chado is in the $databases variable of the settings.php file
  if (array_key_exists('chado', $databases)) {
    return TRUE;
  }

  // check to make sure the chado schema exists
  return chado_is_local();
}

/**
 * Returns the version number of the currently installed Chado instance.
 * It can return the real or effective version.  Note, this function
 * is executed in the hook_init() of the tripal_core module which then
 * sets the $GLOBAL['exact_chado_version'] and $GLOBAL['chado_version']
 * variable.  You can access these variables rather than calling this function.
 *
 * @param $exact
 *   Set this argument to 1 to retrieve the exact version that is installed.
 *   Otherwise, this function will set the version to the nearest 'tenth'.
 *   Chado versioning numbers in the hundreds represent changes to the
 *   software and not the schema.  Changes in the tenth's represent changes
 *   in the schema.
 *
 * @param $warn_if_unsupported
 *   If the currently installed version of Chado is not supported by Tripal
 *   this generates a Drupal warning.
 *
 * @returns
 *   The version of Chado
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_version($exact = FALSE, $warn_if_unsupported = FALSE) {

  global $databases;
  $version = '';
  $is_local = 0;

  // check that Chado is installed if not return 'uninstalled as the version'
  $chado_exists = chado_is_local();
  if (!$chado_exists) {
    // if it's not in the drupal database check to see if it's specified in the $db_url
    // in the settings.php
    if (!array_key_exists('chado', $databases)) {
      // if it's not in the drupal database or specified in the $db_url then
      // return uninstalled as the version
      return 'not installed';
    }
    $is_local = 0;
    $previous_db = chado_set_active('chado');
    $prop_exists = db_table_exists('chadoprop');
    chado_set_active($previous_db);
  }
  else {
    $is_local = 1;
    $prop_exists = db_table_exists('chado.chadoprop');
  }

  // if the table doesn't exist then we don't know what version but we know
  // it must be 1.11 or older.
  if (!$prop_exists) {
    $version = "1.11 or older";
  }
  else {
    $sql = "
      SELECT value
      FROM {chadoprop} CP
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = CP.type_id
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
      WHERE CV.name = 'chado_properties' and CVT.name = 'version'
    ";
    if (!$is_local) {
      $previous_db = chado_set_active('chado');
      $results = db_query($sql);
      chado_set_active($previous_db);
    }
    else {
      $results = chado_query($sql);
    }
    $v = $results->fetchObject();

    // if we don't have a version in the chadoprop table then it must be
    // v1.11 or older
    if (!$v) {
      $version =  "1.11 or older";
    }
    $version =  $v->value;
  }

  // next get the exact Chado version that is installed
  $exact_version = $version;

  // Tripal only supports v1.11 or newer.. really this is the same as v1.1
  // but at the time the v1.11 schema API was written we didn't know that so
  // we'll return the version 1.11 so the schema API will work.
  if (strcmp($exact_version, '1.11 or older') == 0) {
    $exact_version = "1.11";
    if ($warn_if_unsupported) {
      drupal_set_message(t("WARNING: Tripal does not fully support Chado version less than v1.11.  If you are certain this is v1.11
         or if Chado was installed using an earlier version of Tripal then all is well. If not please upgrade to v1.11 or later"),
         'warning');
    }
  }

  // if not returing an exact version, return the version to the nearest 10th.
  // return 1.2 for all versions of 1.2x
  $effective_version = $exact_version;
  if (preg_match('/^1\.2\d+$/', $effective_version)) {
    $effective_version = "1.2";
  }
  if ($warn_if_unsupported and ($effective_version != 1.11 and $effective_version != 1.2 and $effective_version != 'not installed')) {
    drupal_set_message(t("WARNING: The currently installed version of Chado, v$exact_version, is not fully compatible with Tripal."), 'warning');
  }
  // if the callee has requested the exact version then return it
  if ($exact) {
    return $exact_version;
  }

  return $effective_version;
}

/**
 * Retrieves the list of tables in the Chado schema.  By default it only returns
 * the default Chado tables, but can return custom tables added to the
 * Chado schema if requested
 *
 * @param $include_custom
 *   Optional.  Set as TRUE to include any custom tables created in the
 *   Chado schema. Custom tables are added to Chado using the
 *   tripal_core_chado_create_table() function.
 *
 * @returns
 *   An associative array where the key and value pairs are the Chado table names.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_table_names($include_custom = NULL) {

  // first get the chado version that is installed
  $v = $GLOBALS["chado_version"];

  $tables = array();
  if ($v == '1.2') {
    $tables_v1_2 = tripal_core_chado_get_v1_2_tables();
    foreach ($tables_v1_2 as $table) {
      $tables[$table] = $table;
    }
  }
  if ($v == '1.11' or $v == '1.11 or older') {
    $tables_v1_11 = tripal_core_chado_get_v1_11_tables();
    foreach ($tables_v1_11 as $table) {
      $tables[$table] = $table;
    }
  }

  // now add in the custom tables too if requested
  if ($include_custom) {
    $sql = "SELECT table_name FROM {tripal_custom_tables}";
    $resource = db_query($sql);

    foreach ($resource as $r) {
      $tables[$r->table_name] = $r->table_name;
    }
  }

  asort($tables);
  return $tables;
}

/**
 * Retrieves the chado tables Schema API array.
 *
 * @param $table
 *   The name of the table to retrieve.  The function will use the appopriate
 *   Tripal chado schema API hooks (e.g. v1.11 or v1.2).
 *
 * @returns
 *   A Drupal Schema API array defining the table.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_schema($table) {

  // first get the chado version that is installed
  $v = $GLOBALS["chado_version"];

  // get the table array from the proper chado schema
  $v = preg_replace("/\./", "_", $v); // reformat version for hook name
  $table_arr = module_invoke_all("chado_schema_v" . $v . "_" . $table);

  // if the table_arr is empty then maybe this is a custom table
  if (!is_array($table_arr) or count($table_arr) == 0) {
    $table_arr = chado_get_custom_table_schema($table);
  }

  return $table_arr;
}

/**
 * Retrieves the schema in an array for the specified custom table.
 *
 * @param $table
 *   The name of the table to create.
 *
 * @return
 *   A Drupal-style Schema API array definition of the table. Returns
 *   FALSE on failure.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_custom_table_schema($table) {

  $sql = "SELECT schema FROM {tripal_custom_tables} WHERE table_name = :table_name";
  $results = db_query($sql, array(':table_name' => $table));
  $custom = $results->fetchObject();
  if (!$custom) {
    return FALSE;
  }
  else {
    return unserialize($custom->schema);
  }
}