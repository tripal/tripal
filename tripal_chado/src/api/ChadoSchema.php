<?php

namespace Drupal\tripal_chado\api;

use Symfony\Component\Yaml\Yaml;
use Drupal\Core\Database\Database;

/**
 * Provides an application programming interface (API) for describing Chado
 * tables.
 *
 * If you need the Drupal-style array definition for any table, use the
 * following:
 *
 * @code
 *
 * $chado_schema = new \ChadoSchema();
 * $table_schema = $chado_schema->getTableSchema($table_name);
 * @endcode
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The getTableSchema method determines the appropriate version of
 * Chado and uses the Drupal hook infrastructure to call the appropriate
 * hook function to retrieve the table schema.
 *
 * Additionally, here are some other examples of how to use this class:
 * @code
 *
 * // Retrieve the schema array for the organism table in chado 1.2
 * $chado_schema = new \ChadoSchema('1.2');
 * $table_schema = $chado_schema->getTableSchema('organism');
 *
 * // Retrieve all chado tables.
 * $chado_schema = new \ChadoSchema();
 * $tables = $chado_schema->getTableNames();
 * $base_tables = $chado_schema->getbaseTables();
 *
 * // Check the feature.type_id foreign key constraint
 * $chado_schema = new \ChadoSchema();
 * $exists = $chado_schema ->checkFKConstraintExists('feature','type_id');
 *
 * // Check Sequence exists
 * $chado_schema = new \ChadoSchema();
 * $exists = $chado_schema->checkSequenceExists('organism','organism_id');
 * // Or just check the primary key directly
 * $compliant = $chado_schema->checkPrimaryKey('organism');
 * @endcode
 */
class ChadoSchema {

  /**
   * @var string
   *   The current version for this site. E.g. "1.3".
   */
  protected $version = '';

  /**
   * @var string
   *   The name of the schema chado was installed in.
   */
  protected $schema_name = 'chado';

  /**
   * @var array
   *   A description of all tables which should be in the current schema.
   */
  protected $schema = [];

  /**
   * @var object \Drupal
   * Saves the logger.
   */
  protected $logger = NULL;

  /**
   * @var object \Drupal
   * Saves the Drupal database connection.
   */
  protected $connection = NULL;

  /**
   * @var string
   * The default database.
   */
  protected $default_db = NULL;

  /**
   * The ChadoSchema constructor.
   *
   * @param string $version
   *   The current version for this site. E.g. "1.3". If a version is not
   *   provided, the version of the current database will be looked up.
   */
  public function __construct($version = NULL, $schema_name = NULL) {

    // Setup a logger.
    $this->logger = \Drupal::logger('tripal_chado');

    // Cache the connection to the database.
    $this->connection = Database::getConnection();
    $databases = $this->connection->getConnectionOptions();
    $this->default_db = $databases['database'];

    // Set the version of the schema.
    if ($version === NULL) {
      $this->version = chado_get_version(TRUE, $schema_name);
    }
    else {
      $this->version = $version;
    }

    // Set the name of the schema.
    if ($schema_name === NULL) {
      $this->schema_name = 'chado';
    }
    else {
      $tripalDbxApi = \Drupal::service('tripal.dbx');
      if ($tripalDbxApi->isInvalidSchemaName($schema_name, TRUE)) {
        // Schema name must be a single word containing only lower case letters
        // or numbers and cannot begin with a number.
        $this->logger->error(
          "Schema name must be a single alphanumeric word beginning with a letter and all lowercase.");
        return FALSE;
      }
      else {
        $this->schema_name = $schema_name;
      }
    }

    // Check functions require the chado schema be local and installed...
    // So lets check that now...
    if (ChadoSchema::schemaExists($schema_name) !== TRUE) {
      $this->logger->error(
        'Schema must already exist and be in the same database as your
        Drupal installation.');
      return FALSE;
    }
  }

  /**
   * Check that any given chado schema exists.
   *
   * @param string $schema
   *   The name of the schema to check the existence of
   *
   * @return bool
   *   TRUE/FALSE depending upon whether or not the schema exists.
   */
  static function schemaExists($schema_name) {

    // First make sure we have a valid schema name.
    if (preg_match('/^[a-z_][a-z0-9_]+$/', $schema_name) === 0) {
      // Schema name must be a single word containing only lower case letters
      // or numbers and cannot begin with a number.
      // No "$this" in static context.
      // $this->logger->error(
      //   "Schema name must be a single alphanumeric word beginning with a number and all lowercase.");
      \Drupal::messenger()->addMessage(
        "Schema name must be a single alphanumeric word beginning with a number and all lowercase."
      );
      return FALSE;
    }

    $sql = "
      SELECT true
      FROM pg_namespace
      WHERE
        has_schema_privilege(nspname, 'USAGE') AND
        nspname = :nspname
    ";
    $query = \Drupal::database()->query($sql, [':nspname' => $schema_name]);
    $schema_exists = $query->fetchField();
    if ($schema_exists) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Returns the version number of the Chado this object references.
   *
   * @returns
   *   The version of Chado
   */
  public function getVersion() {
    return $this->version;
  }

  /**
   * Retrieve the name of the PostgreSQL schema housing Chado.
   *
   * @return
   *   The name of the schema.
   */
  public function getSchemaName() {
    return $this->schema_name;
  }

  /**
   * Retrieves the list of tables in the Chado schema.  By default it only
   * returns the default Chado tables, but can return custom tables added to
   * the Chado schema if requested.
   *
   * @param $include_custom
   *   Optional.  Set as TRUE to include any custom tables created in the
   *   Chado schema. Custom tables are added to Chado using the
   *   tripal_chado_chado_create_table() function.
   *
   * @returns
   *   An associative array where the key and value pairs are the Chado table
   *   names.
   */
  public function getTableNames($include_custom = FALSE) {

    $schema = $this->getSchemaDetails();
    $tables = array_keys($schema);

    // now add in the custom tables too if requested
    // @todo change this to the variable once custom tables are supported.
    if (FALSE) {
      $sql = "SELECT table FROM {tripal_custom_tables}";
      $resource = $this->connection->query($sql);

      foreach ($resource as $r) {
        $tables[$r->table] = $r->table;
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
   */
  public function getTableSchema($table) {

    $schema = $this->getSchemaDetails();

    $table_arr =  FALSE;
    if (isset($schema[$table])) {
      $table_arr = $schema[$table];
    }
    else {
      // Try to check if it's a custom table
      $table_arr = $this->getCustomTableSchema($table);
      if($table_arr == FALSE) {
        return NULL;
      }
    }

    // Ensure all the parts are set.
    if (!isset($table_arr['primary key'])) {
      $table_arr['primary key'] = [];
    }
    if (!isset($table_arr['unique keys'])) {
      $table_arr['unique keys'] = [];
    }
    if (!isset($table_arr['foreign keys'])) {
      $table_arr['foreign keys'] = [];
    }
    if (!isset($table_arr['referring_tables'])) {
      $table_arr['referring_tables'] = [];
    }

    // Ensures consistency regardless of the number of columns of the pkey.
    $table_arr['primary key'] = (array) $table_arr['primary key'];

    // Ensure this is parsed as an array.
    if (is_string($table_arr['referring_tables'])) {
      $table_arr['referring_tables'] = explode(', ', $table_arr['referring_tables']);
    }

    // Ensure the unique keys are arrays.
    foreach ($table_arr['unique keys'] as $ukname => $ukcolumns) {
      if (is_string($ukcolumns)) {
        $table_arr['unique keys'][$ukname] = explode(', ', $ukcolumns);
      }
    }

    // Ensure foreign key array is present for consistency.
    if (!isset($table_arr['foreign keys'])) {
      $table_arr['foreign keys'] = [];
    }

    return $table_arr;

  }

  /**
   * Retrieves the schema array for the specified custom table.
   *
   * @param $table
   *   The name of the table to create.
   *
   * @return
   *   A Drupal-style Schema API array definition of the table. Returns
   *   FALSE on failure.
   */
  public function getCustomTableSchema($table) {

    $sql = "SELECT schema FROM {tripal_custom_tables} WHERE table_name = :table_name";
    $results = $this->connection->query($sql, [':table_name' => $table]);
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
   *  Base tables are those that contain the primary record for a data type.
   * For
   *  example, feature, organism, stock, are all base tables.  Other tables
   *  include linker tables (which link two or more base tables), property
   * tables, and relationship tables.  These provide additional information
   * about primary data records and are therefore not base tables.  This
   * function retrieves only the list of tables that are considered 'base'
   * tables.
   *
   * @return
   *    An array of base table names.
   *
   * @ingroup tripal_chado_schema_api
   */
  function getBaseTables() {

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
    $schema = $this->getTableSchema('cvterm');
    if (isset($schema['referring_tables'])) {
      foreach ($schema['referring_tables'] as $tablename) {

        $is_base_table = TRUE;

        // Ignore the cvterm tables + chadoprop tables.
        if (in_array($tablename, ['cvterm_dbxref', 'cvterm_relationship', 'cvtermpath', 'cvtermprop', 'chadoprop', 'cvtermsynonym'])) {
          $is_base_table = FALSE;
        }
        // Ignore relationship linked tables.
        elseif (preg_match('/_relationship$/', $tablename)) {
          $is_base_table = FALSE;
        }
        // Ignore cvterm annotation linking tables.
        elseif (preg_match('/_cvterm$/', $tablename)) {
          $is_base_table = FALSE;
        }
        // Ignore property tables.
        elseif (preg_match('/prop$/', $tablename) || preg_match('/prop_.+$/', $tablename)) {
          $is_base_table = FALSE;
        }
        // Ignore natural diversity tables.
        elseif (preg_match('/^nd_/', $tablename)) {
          $is_base_table = FALSE;
        }

        // If it's not any of the above then add it to the list.
        if ($is_base_table === TRUE) {
          array_push($base_tables, $tablename);
        }
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
      $schema = $this->getTableSchema($tablename);
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
   * Retrieve schema details from YAML file.
   *
   * @return
   *   An array with details for the current schema version.
   */
  public function getSchemaDetails() {

    if (empty($this->schema)) {
      $filename = \Drupal::service('extension.list.module')->getPath('tripal_chado') . '/chado_schema/chado_schema-1.3.yml';
      $this->schema = Yaml::parse(file_get_contents($filename));
    }

    return $this->schema;
  }

  /**
   * Get information about which Chado base table a cvterm is mapped to.
   *
   * Vocabulary terms that represent content types in Tripal must be mapped to
   * Chado tables.  A cvterm can only be mapped to one base table in Chado.
   * This function will return an object that contains the chado table and
   * foreign key field to which the cvterm is mapped.  The 'chado_table'
   * property of the returned object contains the name of the table, and the
   * 'chado_field' property contains the name of the foreign key field (e.g.
   * type_id), and the
   * 'cvterm' property contains a cvterm object.
   *
   * @params
   *   An associative array that contains the following keys:
   *     - cvterm_id:  the cvterm ID value for the term.
   *     - vocabulary: the short name for the vocabulary (e.g. SO, GO, PATO)
   *     - accession:  the accession for the term.
   *     - bundle_id:  the ID for the bundle to which a term is associated.
   *   The 'vocabulary' and 'accession' must be used together, the 'cvterm_id'
   *   can be used on it's own.
   *
   * @return
   *   An object containing the chado_table and chado_field properties or NULL
   *   if if no mapping was found for the term.
   *
  public function getCvtermMapping($params) {
    return chado_get_cvterm_mapping($params);
  }*/

  /**
   * Check that any given Chado table exists.
   *
   * This function is necessary because Drupal's db_table_exists() function will
   * not look in any other schema but the one where Drupal is installed
   *
   * @param $table
   *   The name of the chado table whose existence should be checked.
   *
   * @return
   *   TRUE if the table exists in the chado schema and FALSE if it does not.
   */
  public function checkTableExists($table) {

    // Get the default database and chado schema.
    $default_db = $this->default_db;
    $chado_schema = $this->schema_name;

    // Ensure they gave us a table.
    if (empty($table)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_WARNING,
        'You must pass in a table name when calling checkTableExists().'
      );
      return NULL;
    }

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
      ':table_name' => strtolower($table),
      ':chado' => $chado_schema,
      ':default_db' => $default_db,
    ];
    $query = $this->connection->query($sql, $args);
    $results = $query->fetchAll();
    if (empty($results)) {
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
   * look in any other schema but the one where Drupal is installed
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
  public function checkColumnExists($table, $column) {

    // Get the default database and chado schema.
    $default_db = $this->default_db;
    $chado_schema = $this->schema_name;

    // Ensure they gave us a table.
    if (empty($table)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_WARNING,
        'You must pass in a table name when calling checkColumnExists().'
      );
      return NULL;
    }
    // Ensure they gave us a column.
    if (empty($column)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_WARNING,
        'You must pass in a column name when calling checkColumnExists().'
      );
      return NULL;
    }

    // @upgrade $cached_obj = cache_get('chado_table_columns', 'cache');
    // if ($cached_obj) {
    //   $cached_cols = $cached_obj->data;
    //   if (is_array($cached_cols) and
    //     array_key_exists($table, $cached_cols) and
    //     array_key_Exists($column, $cached_cols[$table])) {
    //     return $cached_cols[$table][$column]['exists'];
    //   }
    // }

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
      ':table_name' => strtolower($table),
      ':column_name' => $column,
      ':chado' => $chado_schema,
      ':default_db' => $default_db,
    ];
    $query = $this->connection->query($sql, $args);
    $results = $query->fetchAll();
    if (empty($results)) {
      // @upgrade $cached_cols[$table][$column]['exists'] = FALSE;
      // cache_set('chado_table_columns', $cached_cols, 'cache', CACHE_TEMPORARY);
      return FALSE;
    }

    // @upgrade $cached_cols[$table][$column]['exists'] = TRUE;
    // cache_set('chado_table_columns', $cached_cols, 'cache', CACHE_TEMPORARY);
    return TRUE;
  }

  /**
   * Check that any given column in a Chado table exists.
   *
   * This function is necessary because Drupal's db_field_exists() will not
   * look in any other schema but the one where Drupal is installed
   *
   * @param $table
   *   The name of the chado table.
   * @param $column
   *   The name of the column in the chado table.
   * @param $type
   *   (OPTIONAL) The PostgreSQL type to check for. If not supplied it will be
   *   looked up via the schema (PREFERRED).
   *
   * @return
   *   TRUE if the column type matches what we expect and
   *   FALSE if it does not.
   *
   * @ingroup tripal_chado_schema_api
   */
  public function checkColumnType($table, $column, $expected_type = NULL) {

    // Ensure this column exists before moving forward.
    if (!$this->checkColumnExists($table, $column)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_WARNING,
        'Unable to check the type of !table!column since it doesn\'t appear to exist in your site database.',
        ['!column' => $column, '!table' => $table]
      );
      return FALSE;
    }

    // Look up the type using the Schema array.
    if ($expected_type === NULL) {
      $schema = $this->getTableSchema($table, $column);

      if (is_array($schema) AND isset($schema['fields'][$column])) {
        $expected_type = $schema['fields'][$column]['type'];
      }
      else {
        tripal_report_error(
          'ChadoSchema',
          TRIPAL_WARNING,
          'Unable to check the type of !table!column due to being unable to find the schema definition.',
          ['!column' => $column, '!table' => $table]
        );
        return FALSE;
      }
    }

    // There is some flexibility in the expected type...
    // Fix that here.
    switch ($expected_type) {
      case 'int':
        $expected_type = 'integer';
        break;
      case 'serial':
        $expected_type = 'integer';
        break;
      case 'varchar':
        $expected_type = 'character varying';
        break;
      case 'datetime':
        $expected_type = 'timestamp without time zone';
        break;
      case 'char':
        $expected_type = 'character';
        break;
    }

    // Grab the type from the current database.
    $query = 'SELECT data_type
              FROM information_schema.columns
              WHERE
                table_name = :table AND
                column_name = :column AND
                table_schema = :schema
              ORDER  BY ordinal_position
              LIMIT 1';
    $type = $this->connection->query($query,
      [
        ':table' => $table,
        ':column' => $column,
        ':schema' => $this->schema_name,
      ])->fetchField();

    // Finally we do the check!
    if ($type === $expected_type) {
      return TRUE;
    }
    elseif (($expected_type == 'float') AND (($type == 'double precision') OR ($type == 'real'))) {
      return TRUE;
    }
    elseif ($type == 'smallint' AND $expected_type == 'integer') {
      return TRUE;
    }
    elseif ($type == 'bigint' AND $expected_type == 'integer') {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Check that any given sequence in a Chado table exists.
   *
   * @param table
   *   The name of the table the sequence is used in.
   * @param column
   *   The name of the column the sequence is used to populate.
   *
   * @return
   *   TRUE if the seqeuence exists in the chado schema and FALSE if it does
   *   not.
   *
   * @ingroup tripal_chado_schema_api
   */
  public function checkSequenceExists($table, $column, $sequence_name = NULL) {

    if ($sequence_name === NULL) {

      // Ensure they gave us a table.
      if (empty($table)) {
        tripal_report_error(
          'ChadoSchema',
          TRIPAL_WARNING,
          'You must pass in a table name when calling checkSequenceExists().'
        );
        return NULL;
      }
      // Ensure they gave us a table.
      if (empty($column)) {
        tripal_report_error(
          'ChadoSchema',
          TRIPAL_WARNING,
          'You must pass in a column name when calling checkSequenceExists().'
        );
        return NULL;
      }

      $prefixed_table = $this->schema_name . '.' . $table;
      $sequence_name = $this->connection->query('SELECT pg_get_serial_sequence(:table, :column);',
        [':table' => $prefixed_table, ':column' => $column])->fetchField();
      // Remove prefixed table from sequence name
      if (!empty($sequence_name)) {
        $sequence_name = str_replace($this->schema_name . '.', '', $sequence_name);
      }
      else {
        return FALSE;
      }
    }

    // Get the default database and chado schema.
    $default_db = $this->default_db;
    $chado_schema = $this->schema_name;

    // @upgrade $cached_obj = cache_get('chado_sequences', 'cache');
    // $cached_seqs = $cached_obj->data;
    // if (is_array($cached_seqs) and array_key_exists($sequence, $cached_seqs)) {
    //  return $cached_seqs[$sequence]['exists'];
    // }

    $sql = "
      SELECT 1
      FROM information_schema.sequences
      WHERE
        sequence_name = :sequence_name AND
        sequence_schema = :sequence_schema AND
        sequence_catalog = :sequence_catalog
    ";
    $args = [
      ':sequence_name' => strtolower($sequence_name),
      ':sequence_schema' => $chado_schema,
      ':sequence_catalog' => $default_db,
    ];
    $query = $this->connection->query($sql, $args);
    $results = $query->fetchAll();
    if (empty($results)) {
      // @upgrade $cached_seqs[$sequence]['exists'] = FALSE;
      // cache_set('chado_sequences', $cached_seqs, 'cache', CACHE_TEMPORARY);
      return FALSE;
    }
    // @upgrade $cached_seqs[$sequence]['exists'] = FALSE;
    // cache_set('chado_sequences', $cached_seqs, 'cache', CACHE_TEMPORARY);
    return TRUE;
  }

  /**
   * Check that the primary key exists, has a sequence and a constraint.
   *
   * @param $table
   *   The table you want to check the primary key for.
   * @param $column
   *   (OPTIONAL) The name of the primary key column.
   *
   * @return
   *   TRUE if the primary key meets all the requirements and false otherwise.
   */
  public function checkPrimaryKey($table, $column = NULL) {

    // If they didn't supply the column, then we can look it up.
    if ($column === NULL) {
      $table_schema = $this->getTableSchema($table);
      $column = $table_schema['primary key'][0];
    }

    // If there is no primary key then we can't check it.
    // It neither passes nore fails validation.
    if (empty($column)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_NOTICE,
        'Cannot check the validity of the primary key for ":table" since there is no record of one.',
        [':table' => $table]
      );
      return NULL;
    }

    // Check the column exists.
    $column_exists = $this->checkColumnExists($table, $column);
    if (!$column_exists) {
      return FALSE;
    }

    // First check that the sequence exists.
    $sequence_exists = $this->checkSequenceExists($table, $column);
    if (!$sequence_exists) {
      return FALSE;
    }

    // Next check the constraint is there.
    $constraint_exists = $this->connection->query(
      "SELECT 1
      FROM information_schema.table_constraints
      WHERE table_name=:table AND constraint_type = 'PRIMARY KEY'",
      [':table' => $table])->fetchField();
    if (!$constraint_exists) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Check that the constraint exists.
   *
   * @param $table
   *   The table the constraint applies to.
   * @param $constraint_name
   *   The name of the constraint you want to check.
   * @param $type
   *   The type of constraint. Should be one of "PRIMARY KEY", "UNIQUE", or
   *   "FOREIGN KEY".
   *
   * @return
   *   TRUE if the constraint exists and false otherwise.
   */
  function checkConstraintExists($table, $constraint_name, $type) {

    // Next check the constraint is there.
    $constraint_exists = $this->connection->query(
      "SELECT 1
      FROM information_schema.table_constraints
      WHERE table_name=:table AND constraint_type = :type AND constraint_name = :name",
      [
        ':table' => $table,
        ':name' => $constraint_name,
        ':type' => $type,
      ])->fetchField();
    if (!$constraint_exists) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Check the foreign key constrain specified exists.
   *
   * @param $base_table
   *   The name of the table the foreign key resides in. E.g. 'feature' for
   *     the feature.type_id => cvterm.cvterm_id foreign key.
   * @param $base_column
   *   The name of the column that is a foreign key in. E.g. 'type_id' for
   *     the feature.type_id => cvterm.cvterm_id foreign key.
   *
   * @return
   *   TRUE if the constraint exists and false otherwise.
   */
  function checkFKConstraintExists($base_table, $base_column) {


    // Since we don't have a constraint name, we have to use the known pattern for
    // creating these names in order to make this check.
    // This is due to PostgreSQL not storing column information for constraints
    // in the information_schema tables.
    $constraint_name = $base_table . '_' . $base_column . '_fkey';

    return $this->checkConstraintExists($base_table, $constraint_name, 'FOREIGN KEY');
  }

  /**
   * A Chado-aware replacement for the db_index_exists() function.
   *
   * @param string $table
   *   The table to be altered.
   * @param string $name
   *   The name of the index.
   * @param bool $no_suffix
   */
  function checkIndexExists($table, $name, $no_suffix = FALSE) {

    if (empty($table)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_NOTICE,
        'You must provide the table name when calling checkIndexExists().'
      );
      return NULL;
    }
    if (empty($name)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_NOTICE,
        'You must provide the name of the index when calling checkIndexExists().'
      );
      return NULL;
    }

    if ($no_suffix) {
      $indexname = strtolower($table . '_' . $name);
    }
    else {
      $indexname = strtolower($table . '_' . $name . '_idx');
    }

    // Get the default database and chado schema.
    $default_db = $this->default_db;
    $chado_schema = $this->schema_name;

    $sql = "
      SELECT 1 as exists
      FROM pg_indexes
      WHERE
        indexname = :indexname AND
        tablename = :tablename AND
        schemaname = :schemaname
    ";
    $args = [
      ':indexname' => $indexname,
      ':tablename' => strtolower($table),
      ':schemaname' => $chado_schema,
    ];
    $query = $this->connection->query($sql, $args);
    $results = $query->fetchAll();
    if (empty($results)) {
      return FALSE;
    }
    return TRUE;
  }

  /**
   * A Chado-aware replacement for db_add_index().
   *
   * @param $table
   *   The table to be altered.
   * @param $name
   *   The name of the index.
   * @param string $fields
   *   An array of field names.
   */
   function addIndex($table, $name, $fields, $no_suffix = FALSE) {

     if ($no_suffix) {
       $indexname = strtolower($table . '_' . $name);
     }
     else {
       $indexname = strtolower($table . '_' . $name . '_idx');
     }

     // Get the default database and chado schema.
     $default_db = $this->default_db;
     $chado_schema = $this->schema_name;
     $chado_dot = $chado_schema . '.';

     // Determine the create index SQL command.
     // Note: we don't use place holders here because we cannot
     // have quotes around these parameters.
     $query = 'CREATE INDEX "' . $indexname . '" ON ' . $chado_dot . $table . ' ';
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

     // Now execute it!
     return $this->connection->query($query);
   }


   function createTableSql($name, $table) {
    $sql_fields = [];
    foreach ($table['fields'] as $field_name => $field) {
      $sql_fields[] = $this
        ->createFieldSql($field_name, $this
        ->processField($field));
    }
    $sql_keys = [];
    if (!empty($table['primary key']) && is_array($table['primary key'])) {
      $this
        ->ensureNotNullPrimaryKey($table['primary key'], $table['fields']);
      $sql_keys[] = 'CONSTRAINT ' . $this
        ->ensureIdentifiersLength($name, '', 'pkey') . ' PRIMARY KEY (' . $this
        ->createPrimaryKeySql($table['primary key']) . ')';
    }
    if (isset($table['unique keys']) && is_array($table['unique keys'])) {
      foreach ($table['unique keys'] as $key_name => $key) {
        $sql_keys[] = 'CONSTRAINT ' . $this
          ->ensureIdentifiersLength($name, $key_name, 'key') . ' UNIQUE (' . implode(', ', $key) . ')';
      }
    }
    $sql = "CREATE TABLE {" . $name . "} (\n\t";
    $sql .= implode(",\n\t", $sql_fields);
    if (count($sql_keys) > 0) {
      $sql .= ",\n\t";
    }
    $sql .= implode(",\n\t", $sql_keys);
    $sql .= "\n)";
    $statements[] = $sql;
    if (isset($table['indexes']) && is_array($table['indexes'])) {
      foreach ($table['indexes'] as $key_name => $key) {
        $statements[] = $this
          ->_createIndexSql($name, $key_name, $key);
      }
    }

    // Add table comment.
    if (!empty($table['description'])) {
      $statements[] = 'COMMENT ON TABLE {' . $name . '} IS ' . $this
        ->prepareComment($table['description']);
    }

    // Add column comments.
    foreach ($table['fields'] as $field_name => $field) {
      if (!empty($field['description'])) {
        $statements[] = 'COMMENT ON COLUMN {' . $name . '}.' . $field_name . ' IS ' . $this
          ->prepareComment($field['description']);
      }
    }
    return $statements;
  }

  function createFieldSql($name, $spec) {

    // The PostgreSQL server converts names into lowercase, unless quoted.
    $sql = '"' . $name . '" ' . $spec['pgsql_type'];
    if (isset($spec['type']) && $spec['type'] == 'serial') {
      unset($spec['not null']);
    }
    if (in_array($spec['pgsql_type'], [
      'varchar',
      'character',
    ]) && isset($spec['length'])) {
      $sql .= '(' . $spec['length'] . ')';
    }
    elseif (isset($spec['precision']) && isset($spec['scale'])) {
      $sql .= '(' . $spec['precision'] . ', ' . $spec['scale'] . ')';
    }
    if (!empty($spec['unsigned'])) {
      $sql .= " CHECK ({$name} >= 0)";
    }
    if (isset($spec['not null'])) {
      if ($spec['not null']) {
        $sql .= ' NOT NULL';
      }
      else {
        $sql .= ' NULL';
      }
    }
    if (array_key_exists('default', $spec)) {
      $default = $this
        ->escapeDefaultValue($spec['default']);
      $sql .= " default {$default}";
    }
    return $sql;
  }


  function processField($field) {
    if (!isset($field['size'])) {
      $field['size'] = 'normal';
    }

    // Set the correct database-engine specific datatype.
    // In case one is already provided, force it to lowercase.
    if (isset($field['pgsql_type']) AND ($field['pgsql_type'] !== NULL)) {
      $field['pgsql_type'] = mb_strtolower($field['pgsql_type']);
    }
    else {
      $map = $this
        ->getFieldTypeMap();
      $field['pgsql_type'] = $map[$field['type'] . ':' . $field['size']];
    }
    if (!empty($field['unsigned'])) {

      // Unsigned data types are not supported in PostgreSQL 9.1. In MySQL,
      // they are used to ensure a positive number is inserted and it also
      // doubles the maximum integer size that can be stored in a field.
      // The PostgreSQL schema in Drupal creates a check constraint
      // to ensure that a value inserted is >= 0. To provide the extra
      // integer capacity, here, we bump up the column field size.
      if (!isset($map)) {
        $map = $this
          ->getFieldTypeMap();
      }
      switch ($field['pgsql_type']) {
        case 'smallint':
          $field['pgsql_type'] = $map['int:medium'];
          break;
        case 'int':
          $field['pgsql_type'] = $map['int:big'];
          break;
      }
    }
    if (isset($field['type']) && $field['type'] == 'serial') {
      unset($field['not null']);
    }
    return $field;
  }


  function ensureNotNullPrimaryKey(array $primary_key, array $fields) {
    foreach (array_intersect($primary_key, array_keys($fields)) as $field_name) {
      if (!isset($fields[$field_name]['not null']) || $fields[$field_name]['not null'] !== TRUE) {
        throw new SchemaException("The '{$field_name}' field specification does not define 'not null' as TRUE.");
      }
    }
  }


  function ensureIdentifiersLength($table_identifier_part, $column_identifier_part, $tag, $separator = '__') {
    $info = $this
      ->getPrefixInfo($table_identifier_part);
    $table_identifier_part = $info['table'];
    $identifierName = implode($separator, [
      $table_identifier_part,
      $column_identifier_part,
      $tag,
    ]);

    // Retrieve the max identifier length which is usually 63 characters
    // but can be altered before PostgreSQL is compiled so we need to check.
    if (empty($this->maxIdentifierLength)) {
      $this->maxIdentifierLength = $this->connection
        ->query("SHOW max_identifier_length")
        ->fetchField();
    }
    if (strlen($identifierName) > $this->maxIdentifierLength) {
      $saveIdentifier = '"drupal_' . $this
        ->hashBase64($identifierName) . '_' . $tag . '"';
    }
    else {
      $saveIdentifier = $identifierName;
    }
    return $saveIdentifier;
  }
}
