<?php

/**
 * @file
 * Provides an application programming interface (API) for describing Chado
 *   tables.
 *
 * @ingroup tripal_chado
 */

/**
 * @defgroup tripal_chado_schema_api Chado Schema
 * @ingroup tripal_chado_api
 * @{
 * Provides an application programming interface (API) for describing Chado
 *   tables. This API consists of a set of functions, one for each table in
 *   Chado.  Each function simply returns a Drupal style array that defines the
 *   table.
 *
 * Because Drupal 6 does not handle foreign key (FK) relationships, however FK
 * relationships are needed to for Tripal Views.  Therefore, FK relationships
 * have been added to the schema defintitions below.
 *
 * The functions provided in this documentation should not be called as is, but
 *   if you need the Drupal-style array definition for any table, use the
 *   following function call:
 *
 *   $table_desc = chado_get_schema($table)
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The chado_get_schema function determines the appropriate version
 *   of Chado and uses the Drupal hook infrastructure to call the appropriate
 *   hook function to retrieve the table schema.
 * @}
 */

/**
 * Check that any given Chado table exists.
 *
 * This function is necessary because Drupal's db_table_exists() function will
 * not look in any other schema but the one were Drupal is installed
 *
 * @param $table
 *   The name of the chado table whose existence should be checked.
 *
 * @return
 *   TRUE if the table exists in the chado schema and FALSE if it does not.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_table_exists($table) {

  // Get the default database and chado schema.
  global $databases;
  $default_db = $databases['default']['default']['database'];
  $chado_schema = chado_get_schema_name('chado');

  // If we've already lookup up this table then don't do it again, as
  // we don't need to keep querying the database for the same tables.
  if (array_key_exists("chado_tables", $GLOBALS) and
    array_key_exists($default_db, $GLOBALS["chado_tables"]) and
    array_key_exists($chado_schema, $GLOBALS["chado_tables"][$default_db]) and
    array_key_exists($table, $GLOBALS["chado_tables"][$default_db][$chado_schema])) {
    return TRUE;
  }

  $sql = "
    SELECT 1
    FROM information_schema.tables
    WHERE
      table_name = :table_name AND
      table_schema = :chado AND
      table_catalog = :default_db
  ";
  $args = [
    ':table_name' => $table,
    ':chado' => $chado_schema,
    ':default_db' => $default_db,
  ];
  $results = db_query($sql, $args);
  $exists = $results->fetchObject();
  if (!$exists) {
    return FALSE;
  }

  // Set this table in the GLOBALS so we don't query for it again the next time.
  $GLOBALS["chado_tables"][$default_db][$chado_schema][$table] = TRUE;
  return TRUE;
}

/**
 * Check that any given column in a Chado table exists.
 *
 * This function is necessary because Drupal's db_field_exists() will not
 * look in any other schema but the one were Drupal is installed
 *
 * @param $table
 *   The name of the chado table.
 * @param $column
 *   The name of the column in the chado table.
 *
 * @return
 *   TRUE if the column exists for the table in the chado schema and
 *   FALSE if it does not.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_column_exists($table, $column) {
  global $databases;

  $default_db = $databases['default']['default']['database'];
  $cached_obj = cache_get('chado_table_columns', 'cache');
  if ($cached_obj) {
    $cached_cols = $cached_obj->data;
    if (is_array($cached_cols) and
      array_key_exists($table, $cached_cols) and
      array_key_Exists($column, $cached_cols[$table])) {
      return $cached_cols[$table][$column]['exists'];
    }
  }

  $sql = "
    SELECT 1
    FROM information_schema.columns
    WHERE
      table_name = :table_name AND
      column_name = :column_name AND
      table_schema = :chado AND
      table_catalog = :default_db
  ";
  $args = [
    ':table_name' => $table,
    ':column_name' => $column,
    ':chado' => chado_get_schema_name('chado'),
    ':default_db' => $default_db,
  ];
  $results = db_query($sql, $args);
  $exists = $results->fetchField();
  if (!$exists) {
    $cached_cols[$table][$column]['exists'] = FALSE;
    cache_set('chado_table_columns', $cached_cols, 'cache', CACHE_TEMPORARY);
    return FALSE;
  }
  $cached_cols[$table][$column]['exists'] = TRUE;
  cache_set('chado_table_columns', $cached_cols, 'cache', CACHE_TEMPORARY);
  return TRUE;
}

/**
 * Check that any given column in a Chado table exists.
 *
 * This function is necessary because Drupal's db_field_exists() will not
 * look in any other schema but the one were Drupal is installed
 *
 * @param sequence
 *   The name of the sequence
 *
 * @return
 *   TRUE if the seqeuence exists in the chado schema and FALSE if it does not.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_sequence_exists($sequence) {
  global $databases;

  $default_db = $databases['default']['default']['database'];
  $cached_obj = cache_get('chado_sequences', 'cache');
  $cached_seqs = $cached_obj->data;
  if (is_array($cached_seqs) and array_key_exists($sequence, $cached_seqs)) {
    return $cached_seqs[$sequence]['exists'];
  }
  $sql = "
    SELECT 1
    FROM information_schema.sequences
    WHERE
      sequence_name = :sequence_name AND
      sequence_schema = :sequence_schema AND
      sequence_catalog = :sequence_catalog
  ";
  $args = [
    ':sequence_name' => $sequence,
    ':sequence_schema' => chado_get_schema_name('chado'),
    ':sequence_catalog' => $default_db,
  ];
  $results = db_query($sql, $args);
  $exists = $results->fetchField();
  if (!$exists) {
    $cached_seqs[$sequence]['exists'] = FALSE;
    cache_set('chado_sequences', $cached_seqs, 'cache', CACHE_TEMPORARY);
    return FALSE;
  }
  $cached_seqs[$sequence]['exists'] = FALSE;
  cache_set('chado_sequences', $cached_seqs, 'cache', CACHE_TEMPORARY);
  return TRUE;
}

/**
 * A Chado-aware replacement for the db_index_exists() function.
 *
 * @param $table
 *   The table to be altered.
 * @param $name
 *   The name of the index.
 */
function chado_index_exists($table, $name) {
  global $databases;

  $indexname = $table . '_' . $name . '_idx';

  $default_db = $databases['default']['default']['database'];

  $sql = "
    SELECT 1 as exists
    FROM pg_indexes
    WHERE indexname = :indexname
  ";

  $result = db_query($sql, [':indexname' => $indexname]);
  $exists = $result->fetchObject();
  return $exists->exists;
}

/**
 * A Chado-aware wrapper for the db_add_index() function.
 *
 * @param $table
 *   The table to be altered.
 * @param $name
 *   The name of the index.
 * @param $fields
 *   An array of field names.
 */
function chado_add_index($table, $name, $fields) {
  $indexname = $table . '_' . $name . '_idx';

  $query = 'CREATE INDEX "' . $indexname . '" ON {' . $table . '} ';
  $query .= '(';
  $temp = [];
  foreach ($fields as $field) {
    if (is_array($field)) {
      $temp[] = 'substr(' . $field[0] . ', 1, ' . $field[1] . ')';
    }
    else {
      $temp[] = '"' . $field . '"';
    }
  }
  $query .= implode(', ', $temp);
  $query .= ')';
  return chado_query($query);
}

/**
 * Check that any given schema exists.
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

  $sql = "
    SELECT nspname
    FROM pg_namespace
    WHERE
      has_schema_privilege(nspname, 'USAGE') AND
      nspname = :nspname
    ORDER BY nspname
  ";
  $schema = db_query($sql, [':nspname' => $schema])->fetchField();
  if ($schema) {
    return TRUE;
  }
  return FALSE;
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

  // If the is_local variable has been set then we've already checked if
  // Chado is local and we don't need to repeat it again.
  if (isset($GLOBALS["chado_is_local"])) {
    return $GLOBALS["chado_is_local"];
  }

  // This is postgresql-specific code to check the existence of the chado schema
  // @coder-ignore: acting on pg_catalog schema rather then drupal schema
  // therefore, table prefixing does not apply
  $sql = "
    SELECT nspname
    FROM pg_namespace
    WHERE
      has_schema_privilege(nspname, 'USAGE') AND
      nspname = :chado
  ";
  $results = db_query($sql, [':chado' => chado_get_schema_name('chado')]);
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
  if (array_key_exists(chado_get_schema_name('chado'), $databases)) {
    return TRUE;
  }

  // check to make sure the chado schema exists
  return chado_is_local();
}

/**
 * Returns the version number of the currently installed Chado instance.
 * It can return the real or effective version.  Note, this function
 * is executed in the hook_init() of the tripal_chado module which then
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
  $is_local = FALSE;
  $chado_exists = FALSE;

  // Check that Chado is installed if not return 'uninstalled as the version'
  $is_local = isset($GLOBALS["chado_is_local"]) && $GLOBALS["chado_is_local"];
  if (!$is_local) {
    // If it's not in the drupal database check to see if it's specified in
    // the $db_url in the settings.php
    if (!array_key_exists(chado_get_schema_name('chado'), $databases)) {
      // if it's not in the drupal database or specified in the $db_url then
      // return uninstalled as the version
      return 'not installed';
    }
    $is_local = 0;
    $previous_db = chado_set_active('chado');
    $prop_exists = chado_table_exists('chadoprop');
    chado_set_active($previous_db);
  }
  else {
    $chado_exists = TRUE;
    // @todo we need a chado aware db_table_exists.
    $prop_exists = db_table_exists(chado_get_schema_name('chado') . '.chadoprop');
  }

  // if the table doesn't exist then we don't know what version but we know
  // it must be 1.11 or older.
  if (!$prop_exists) {
    $version = "1.11 or older";
  }
  else {
    // We need to hardcode the schema here rather than rely on the db_query
    // function to settle it because the db_query function needs to know the
    // Chado version to make the table prefixes set properly (chicken and
    // egg problem).
    $chado_schema = chado_get_schema_name('chado');
    $sql = "
      SELECT value
      FROM $chado_schema.chadoprop CP
        INNER JOIN $chado_schema.cvterm CVT on CVT.cvterm_id = CP.type_id
        INNER JOIN $chado_schema.cv CV on CVT.cv_id = CV.cv_id
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
      $version = "1.11 or older";
    }
    else {
      $version = $v->value;
    }
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
  else {
    if (preg_match('/^1\.3\d+$/', $effective_version)) {
      $effective_version = "1.3";
    }
  }
  if ($warn_if_unsupported and ($effective_version < 1.11 and $effective_version != 'not installed')) {
    drupal_set_message(t("WARNING: The currently installed version of Chado, v$exact_version, is not fully compatible with Tripal."), 'warning');
  }
  // if the callee has requested the exact version then return it
  if ($exact) {
    return $exact_version;
  }

  return $effective_version;
}

/**
 * Retrieves the list of tables in the Chado schema.  By default it only
 * returns
 * the default Chado tables, but can return custom tables added to the
 * Chado schema if requested
 *
 * @param $include_custom
 *   Optional.  Set as TRUE to include any custom tables created in the
 *   Chado schema. Custom tables are added to Chado using the
 *   tripal_chado_chado_create_table() function.
 *
 * @returns
 *   An associative array where the key and value pairs are the Chado table
 *   names.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_table_names($include_custom = NULL) {

  // first get the chado version that is installed
  $v = array_key_exists('chado_version', $GLOBALS) ? $GLOBALS["chado_version"] : '';

  $tables = [];
  if ($v == '1.3') {
    $tables_v1_3 = tripal_chado_chado_get_v1_3_tables();
    foreach ($tables_v1_3 as $table) {
      $tables[$table] = $table;
    }
  }
  if ($v == '1.2') {
    $tables_v1_2 = tripal_chado_chado_get_v1_2_tables();
    foreach ($tables_v1_2 as $table) {
      $tables[$table] = $table;
    }
  }
  if ($v == '1.11' or $v == '1.11 or older') {
    $tables_v1_11 = tripal_chado_chado_get_v1_11_tables();
    foreach ($tables_v1_11 as $table) {
      $tables[$table] = $table;
    }
  }

  // Now add in the custom tables too if requested
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
  $v = array_key_exists("chado_version", $GLOBALS) ? $GLOBALS["chado_version"] : '';

  // get the table array from the proper chado schema
  $v = preg_replace("/\./", "_", $v); // reformat version for hook name

  // Call the module_invoke_all.
  $hook_name = "chado_schema_v" . $v . "_" . $table;
  $table_arr = module_invoke_all($hook_name);

  // If the module_invoke_all returned nothing then let's make sure there isn't
  // An API call we can call directly.  The only time this occurs is
  // during an upgrade of a major Drupal version and tripal_core is disabled.
  if ((!$table_arr or !is_array($table_arr)) and
    function_exists('tripal_chado_' . $hook_name)) {
    $api_hook = "tripal_chado_" . $hook_name;
    $table_arr = $api_hook();
  }

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
  $results = db_query($sql, [':table_name' => $table]);
  $custom = $results->fetchObject();
  if (!$custom) {
    return FALSE;
  }
  else {
    return unserialize($custom->schema);
  }
}

/**
 *  Returns all chado base tables.
 *
 *  Base tables are those that contain the primary record for a data type. For
 *  example, feature, organism, stock, are all base tables.  Other tables
 *  include linker tables (which link two or more base tables), property tables,
 *  and relationship tables.  These provide additional information about
 *  primary data records and are therefore not base tables.  This function
 *  retreives only the list of tables that are considered 'base' tables.
 *
 * @return
 *    An array of base table names.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_base_tables() {

  // Initialize the base tables with those tables that are missing a type.
  // Ideally they should have a type, but that's for a future version of Chado.
  $base_tables = [
    'organism',
    'project',
    'analysis',
    'biomaterial',
    'eimage',
    'assay',
  ];

  // We'll use the cvterm table to guide which tables are base tables. Typically
  // base tables (with a few exceptions) all have a type.  Iterate through the
  // referring tables.
  $schema = chado_get_schema('cvterm');
  $referring = $schema['referring_tables'];
  foreach ($referring as $tablename) {

    // Ignore the cvterm tables, relationships, chadoprop tables.
    if ($tablename == 'cvterm_dbxref' || $tablename == 'cvterm_relationship' ||
      $tablename == 'cvtermpath' || $tablename == 'cvtermprop' || $tablename == 'chadoprop' ||
      $tablename == 'cvtermsynonym' || preg_match('/_relationship$/', $tablename) ||
      preg_match('/_cvterm$/', $tablename) ||
      // Ignore prop tables
      preg_match('/prop$/', $tablename) || preg_match('/prop_.+$/', $tablename) ||
      // Ignore nd_tables
      preg_match('/^nd_/', $tablename)) {
      continue;
    }
    else {
      array_push($base_tables, $tablename);
    }
  }

  // Remove any linker tables that have snuck in.  Linker tables are those
  // whose foreign key constraints link to two or more base table.
  $final_list = [];
  foreach ($base_tables as $i => $tablename) {
    // A few tables break our rule and seems to look
    // like a linking table, but we want to keep it as a base table.
    if ($tablename == 'biomaterial' or $tablename == 'assay' or $tablename == 'arraydesign') {
      $final_list[] = $tablename;
      continue;
    }

    // Remove the phenotype table. It really shouldn't be a base table as
    // it is meant to store individual phenotype measurements.
    if ($tablename == 'phenotype') {
      continue;
    }
    $num_links = 0;
    $schema = chado_get_schema($tablename);
    $fkeys = $schema['foreign keys'];
    foreach ($fkeys as $fkid => $details) {
      $fktable = $details['table'];
      if (in_array($fktable, $base_tables)) {
        $num_links++;
      }
    }
    if ($num_links < 2) {
      $final_list[] = $tablename;
    }
  }

  // Now add in the cvterm table to the list.
  $final_list[] = 'cvterm';

  // Sort the tables and return the list.
  sort($final_list);
  return $final_list;
}

/**
 * Get information about which Chado base table a cvterm is mapped to.
 *
 * Vocbulary terms that represent content types in Tripal must be mapped to
 * Chado tables.  A cvterm can only be mapped to one base table in Chado.
 * This function will return an object that contains the chado table and
 * foreign key field to which the cvterm is mapped.  The 'chado_table' property
 * of the returned object contains the name of the table, and the 'chado_field'
 * property contains the name of the foreign key field (e.g. type_id), and the
 * 'cvterm' property contains a cvterm object.
 *
 * @params
 *   An associative array that contains the following keys:
 *     - cvterm_id:  the cvterm ID value for the term.
 *     - vocabulary: the short name for the vocabulary (e.g. SO, GO, PATO)
 *     - accession:  the accession for the term.
 *     - bundle_id:  the ID for the bundle to which a term is associated.
 *   The 'vocabulary' and 'accession' must be used together, the 'cvterm_id' can
 *   be used on it's own.
 *
 * @return
 *   An object containing the chado_table and chado_field properties or NULL if
 *   if no mapping was found for the term.
 */
function chado_get_cvterm_mapping($params) {
  $cvterm_id = array_key_exists('cvterm_id', $params) ? $params['cvterm_id'] : NULL;
  $vocabulary = array_key_exists('vocabulary', $params) ? $params['vocabulary'] : NULL;
  $accession = array_key_exists('accession', $params) ? $params['accession'] : NULL;
  $cvterm = NULL;

  if ($cvterm_id) {
    $cvterm = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);
  }
  else {
    if ($vocabulary and $accession) {
      $match = [
        'dbxref_id' => [
          'db_id' => [
            'name' => $vocabulary,
          ],
          'accession' => $accession,
        ],
      ];
      $cvterm = chado_generate_var('cvterm', $match);
    }
  }

  if ($cvterm) {
    $result = db_select('chado_cvterm_mapping', 'tcm')
      ->fields('tcm')
      ->condition('cvterm_id', $cvterm->cvterm_id)
      ->execute();
    $result = $result->fetchObject();
    if ($result) {
      $result->cvterm = $cvterm;
    }
    return $result;
  }
  return NULL;
}
