<?php

/**
 * @file
 * Provides an application programming interface (API) for describing Chado
 *   tables.
 *
 * @ingroup tripal_chado
 */
use Drupal\Core\Database\Database;

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
 * not look in any other schema but the one where Drupal is installed
 *
 * @param string $table
 *   The name of the chado table whose existence should be checked.
 * @param string $chado_schema
 *   The name of the chado schema you expect the table to exist in.
 *
 * @return bool
 *   TRUE if the table exists in the chado schema and FALSE if it does not.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_table_exists($table, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to check the table exists.
  $schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
  return $schema->checkTableExists($table);
}

/**
 * Check that any given column in a Chado table exists.
 *
 * This function is necessary because Drupal's db_field_exists() will not
 * look in any other schema but the one where Drupal is installed
 *
 * @param string $table
 *   The name of the chado table.
 * @param string $column
 *   The name of the column in the chado table.
 * @param string $chado_schema
 *   The name of the chado schema you want to check a column in.
 *
 * @return bool
 *   TRUE if the column exists for the table in the chado schema and
 *   FALSE if it does not.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_column_exists($table, $column, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to check the column exists.
  $schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
  return $schema->checkColumnExists($table, $column);
}

/**
 * Check that any given column in a Chado table exists.
 *
 * This function is necessary because Drupal's db_field_exists() will not
 * look in any other schema but the one where Drupal is installed
 *
 * @param string $sequence
 *   The name of the sequence.
 * @param string $chado_schema
 *   The name of the chado schema to check in.
 *
 * @return bool
 *   TRUE if the seqeuence exists in the chado schema and FALSE if it does not.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_sequence_exists($sequence, $chado_schema = NULL) {

    // Retrieve the default name of the chado schema if it's not provided.
    if ($chado_schema === NULL) {
      $chado_schema = chado_get_schema_name('chado');
    }

    // VALIDATION: Schema name validated in ChadoSchema Contructor.

    // Create a new ChadoSchema instance and use it to check the column exists.
    $schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
    return $schema->checkSequenceExists(NULL, NULL, $sequence);
}

/**
 * A Chado-aware replacement for the db_index_exists() function.
 *
 * @param string $table
 *   The table to be altered.
 * @param string $name
 *   The name of the index.
 * @param string $chado_schema
 *   The name of the chado schema you would like to look in.
 * @return bool
 *   TRUE if the index exists and FALSE otherwise.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_index_exists($table, $name, $no_suffix = FALSE, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to check if the index exists.
  $schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
  return $schema->checkIndexExists($table, $name, $no_suffix);
}

/**
 * A Chado-aware replacement for the db_add_index() function.
 *
 * @param string $table
 *   The table to be altered.
 * @param string $name
 *   The name of the index.
 * @param string $fields
 *   An array of field names.
 * @param string $chado_schema
 *   The name of the chado schema you would like to add the index to.
 * @return bool
 *   The return value of the create index query.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_add_index($table, $name, $fields, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to check if the index exists.
  $schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
  return $schema->addIndex($table, $name, $fields);
}

/**
 * Check that any given schema exists.
 *
 * @param string $schema
 *   The name of the schema to check the existence of
 *
 * @return bool
 *   TRUE/FALSE depending upon whether or not the schema exists.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_dbschema_exists($chado_schema) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to check if the index exists.
  return \Drupal\tripal_chado\api\ChadoSchema::schemaExists($chado_schema);
}

/**
 * Retrieve the name of the PostgreSQL schema housing Chado or Drupal.
 *
 * @param string $schema
 *   Whether you want the schema name for 'chado' or 'drupal'. Chado is the
 *   default.
 *
 * @return string
 *   The name of the PostgreSQL schema housing the $schema specified.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_get_schema_name($schema = 'chado') {

  // First we will set our default. This is what will be returned in most cases.
  if ($schema == 'chado') {

    // Currently we just return a random version of chado.
    $installed = chado_get_installed_schemas();
    if (is_array($installed) && !empty($installed)) {
      $chadodefn = array_shift($installed);
      if (is_object($chadodefn) && isset($chadodefn->schema_name)) {
        $schema_name = $chadodefn->schema_name;
      }
      else {
        $schema_name = 'chado';
      }
    }
    else {
      $schema_name = 'chado';
    }
  }
  else {
    $schema_name = 'public';
  }

  // There are cases where modules or admin might need to change the default
  // names for the schema. Thus we provide an alter hook here to allow
  // the names to be changed and ensure that schema names are never hardcoded
  // directly into queries.
  $context = ['schema' => $schema];
  \Drupal::moduleHandler()->alter('chado_get_schema_name', $schema_name, $context);

  return $schema_name;
}

/**
 * List all installed chado schema.
 *
 * @return array
 *   An array of objects where each object describes an installed schema
 *   and includes install_id, schema_name, version, created, updated.
 */
function chado_get_installed_schemas() {
  $installs = [];

  $install_select = \Drupal::database()->select('chado_installations' ,'i')
    ->fields('i', ['install_id', 'schema_name', 'version',
      'created', 'updated'])
    ->execute();
  $results = $install_select->fetchAll();

  // Check that each schema is still installed.
  foreach ($results as $k => $i) {
    // If it is installed then add it to the array to be returned.
    if (chado_dbschema_exists($i->schema_name)) {
      $installs[$i->schema_name] = $i;
    }
    // If it's not still installed then remove it.
    else {
      \Drupal::database()->delete('chado_installations')
        ->condition('install_id', $i->install_id)
        ->execute();
    }
  }

  return $installs;
}

/**
 * Check that the Chado schema exists within the local database
 *
 * @param bool $force_recheck
 *   Indicate whether or not to use the cached value.
 * @param string $chado_schema
 *   The schema to check if it's local.
 * @return bool
 *   TRUE/FALSE depending upon whether it exists
 *
 * @ingroup tripal_chado_schema_api
 *
 * @upgrade
 */
function chado_is_local($force_recheck = FALSE, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to check if the index exists.
  return \Drupal\tripal_chado\api\ChadoSchema::schemaExists($chado_schema);
}

/**
 * Check whether chado is installed (either in the same or a different database)
 *
 * @param string $chado_schema
 *   The name of the chado schema you want to check the existence of.
 * @return bool
 *   TRUE/FALSE depending upon whether chado is installed.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_is_installed($chado_schema = NULL) {

  // @todo currently not supporting external chado instances.

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to check if the index exists.
  return \Drupal\tripal_chado\api\ChadoSchema::schemaExists($chado_schema);
}

/**
 * Returns the version number of the currently installed Chado instance.
 * It can return the real or effective version.  Note, this function
 * is executed in the hook_init() of the tripal_chado module which then
 * sets the $GLOBAL['exact_chado_version'] and $GLOBAL['chado_version']
 * variable.  You can access these variables rather than calling this function.
 *
 * @param bool $exact
 *   Set this argument to 1 to retrieve the exact version that is installed.
 *   Otherwise, this function will set the version to the nearest 'tenth'.
 *   Chado versioning numbers in the hundreds represent changes to the
 *   software and not the schema.  Changes in the tenth's represent changes
 *   in the schema.
 * @param bool $warn_if_unsupported
 *   If the currently installed version of Chado is not supported by Tripal
 *   this generates a Drupal warning.
 * @param string $chado_schema
 *   The name of the schema you want to check the version of.
 * @return string
 *   The version of Chado
 *
 * @ingroup tripal_chado_schema_api
 *
 * @upgrade
 */
function chado_get_version($exact = FALSE, $warn_if_unsupported = FALSE, $chado_schema = NULL) {

  $version = '';
  $is_local = FALSE;
  $chado_exists = FALSE;
  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // Schema name must be a single word containing only lower case letters
  // or numbers and cannot begin with a number.
  $tripalDbxApi = \Drupal::service('tripal.dbx');
  if ($tripalDbxApi->isInvalidSchemaName($chado_schema, TRUE)) {
    \Drupal::logger('tripal_chado')->error(
      "chado_get_version: Schema name must be a single alphanumeric word beginning with a number and all lowercase.");
    return FALSE;
  }

  // Check that Chado is installed if not return uninstalled as the version.
  $is_local = chado_is_local(TRUE, $chado_schema);
  if (!$is_local) {
    // @todo currently do not support external chado.
    return 'not installed';
  }
  else {
    $chado_exists = TRUE;
    // we cannot use chado_table_exists here because it causes a loop.
    $sql = "SELECT true FROM pg_tables
      WHERE schemaname=:schema AND tablename=:table";
    $prop_exists = \Drupal::database()->query($sql,
      [':schema' => $chado_schema, ':table' => 'chadoprop'])->fetchField();
  }

  // if the table doesn't exist then we don't know what version but we know
  // it must be 1.11 or older.
  if (!$prop_exists) {
    $version = "1.11 or older";
  }
  else {
    $sql = "
      SELECT value
      FROM $chado_schema.chadoprop CP
        INNER JOIN $chado_schema.cvterm CVT on CVT.cvterm_id = CP.type_id
        INNER JOIN $chado_schema.cv CV on CVT.cv_id = CV.cv_id
      WHERE CV.name = 'chado_properties' and CVT.name = 'version'
    ";
    $results = \Drupal::database()->query($sql);
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
      \Drupal::messenger()->addMessage(t("WARNING: Tripal does not fully support Chado version less than v1.11.  If you are certain this is v1.11
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
    \Drupal::messenger()->addMessage(t("WARNING: The currently installed version of Chado, v$exact_version, is not fully compatible with Tripal."), 'warning');
  }
  // if the callee has requested the exact version then return it
  if ($exact) {
    return $exact_version;
  }

  return $effective_version;
}

/**
 * Retrieves the list of tables in the Chado schema.  By default it only
 * returns the default Chado tables, but can return custom tables added to the
 * Chado schema if requested.
 *
 * @param bool $include_custom
 *   Optional.  Set as TRUE to include any custom tables created in the
 *   Chado schema. Custom tables are added to Chado using the
 *   tripal_chado_chado_create_table() function.
 * @param string $chado_schema
 *   The schema you want table names from.
 * @return array
 *   An associative array where the key and value pairs are the Chado table
 *   names.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_table_names($include_custom = NULL, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to get the table names.
  $chado_schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
  $tables = $chado_schema->getTableNames($include_custom);
  return array_combine($tables, $tables);
}

/**
 * Retrieves the chado tables Schema API array.
 *
 * @param string $table
 *   The name of the table to retrieve.  The function will use the appopriate
 *   Tripal chado schema API hooks (e.g. v1.11 or v1.2).
 * @param string $chado_schema
 *   The name of the chado schema you would like to get the table schema for.
 * @return array
 *   A Drupal Schema API array defining the table.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_schema($table, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to get the table schema array.
  $chado_schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
  return $chado_schema->getTableSchema($table);
}


/**
 * Retrieves the schema in an array for the specified custom table.
 *
 * @param string $table
 *   The name of the table to create.
 * @param string $chado_schema
 *   The name of the chado schema you would like to get the custom table from.
 * @return array
 *   A Drupal-style Schema API array definition of the table. Returns
 *   FALSE on failure.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_custom_table_schema($table, $chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  // Create a new ChadoSchema instance and use it to get the schema array.
  $chado_schema = new \Drupal\tripal_chado\api\ChadoSchema();
  return $chado_schema->getCustomTableSchema($table);
}

/**
 *  Returns all chado base tables.
 *
 *  Base tables are those that contain the primary record for a data type. For
 *  example, feature, organism, stock, are all base tables.  Other tables
 *  include linker tables (which link two or more base tables), property tables,
 *  and relationship tables.  These provide additional information about
 *  primary data records and are therefore not base tables.  This function
 *  retrieves only the list of tables that are considered 'base' tables.
 *
 * @param string $chado_schema
 *   The chado schema you are interested in.
 * @return array
 *    An array of base table names.
 *
 * @ingroup tripal_chado_schema_api
 */
function chado_get_base_tables($chado_schema = NULL) {

  // Retrieve the default name of the chado schema if it's not provided.
  if ($chado_schema === NULL) {
    $chado_schema = chado_get_schema_name('chado');
  }

  // VALIDATION: Schema name validated in ChadoSchema Contructor.

  $chado_schema = new \Drupal\tripal_chado\api\ChadoSchema(NULL, $chado_schema);
  return $chado_schema->getBaseTables();
}


/**
 * Get information about which Chado base table a cvterm is mapped to.
 *
 * Vocabulary terms that represent content types in Tripal must be mapped to
 * Chado tables.  A cvterm can only be mapped to one base table in Chado.
 * This function will return an object that contains the chado table and
 * foreign key field to which the cvterm is mapped.  The 'chado_table' property
 * of the returned object contains the name of the table, and the 'chado_field'
 * property contains the name of the foreign key field (e.g. type_id), and the
 * 'cvterm' property contains a cvterm object.
 *
 * @param array $params
 *   An associative array that contains the following keys:
 *     - cvterm_id:  the cvterm ID value for the term.
 *     - vocabulary: the short name for the vocabulary (e.g. SO, GO, PATO)
 *     - accession:  the accession for the term.
 *     - bundle_id:  the ID for the bundle to which a term is associated.
 *   The 'vocabulary' and 'accession' must be used together, the 'cvterm_id' can
 *   be used on its own.
 * @return object
 *   An object containing the chado_table and chado_field properties or NULL if
 *   if no mapping was found for the term.
 *
 * @upgrade
 *
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
