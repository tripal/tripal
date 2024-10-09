<?php

namespace Drupal\tripal\TripalDBX;

use Drupal\Core\Database\Driver\pgsql\Schema as PgSchema;
use Drupal\tripal\TripalDBX\TripalDbxConnection;
use Drupal\tripal\TripalDBX\Exceptions\SchemaException;

/**
 * Tripal DBX API Schema class.
 *
 * NOTE: This class should not be instantiated directly but rather it should
 * be accessed through a TripalDbxConnection object using the schema() method.
 *
 * This class provides a Tripal-specific implementation of the Drupal Schema
 * abstract class. The Drupal PostgreSQL (and other database driver)
 * implementations of the base Drupal Schema class follow the assumption that
 * there is a single schema. As such the core Drupal implementations focus on
 * managing tables within a single schema.
 *
 * This implementation extends that table-management functionality to also include
 * Schema-focused management including creation, cloning, renaming, dropping
 * and definition export. Additionally, it removes the assumption of a single
 * schema by allowing the default schema to be set based on a Tripal DBX
 * connection.
 *
 * Here are some useful functions to know that are inherited from Drupal Schema
 * classes (core + PgSchema implementation):
 *   - addField(), addIndex(), addPrimaryKey(), addUniqueKey(),
 *   - createTable(), dropTable()
 *   - dropField(), dropIndex(), dropPrimaryKey(), dropUniqueKey(),
 *   - fieldExists(), findPrimaryKeyColumns(),
 *   - renameTable()
 *   and more from the documentation.
 *
 * @see https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Driver!pgsql!Schema.php/class/Schema/9.0.x
 * @see https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/class/Schema/9.0.x
 */
abstract class TripalDbxSchema extends PgSchema {

  /**
   * Default schema name.
   *
   * OVERRIDES \Drupal\Core\Database\Schema:$defaultSchema.
   *
   * Drupal assumes that the default schema name is public. However, for multiple
   * schema support, we need to set this dynamically in the constructor and
   * thus set it to empty here.
   *
   * @var string
   */
  protected $defaultSchema = '';

  /**
   * PostgreSQL quoted default schema name.
   *
   * OVERRIDES \Drupal\Core\Database\Schema:$quotedDefaultSchema.
   *
   * Same reasoning as above since this is generated based on the default schema.
   *
   * @var string
   */
  protected $quotedDefaultSchema = '';

  /**
   * Tells if builtin functions have been (temporarily) loaded.
   *
   * @var bool
   */
  protected $initialized = FALSE;

  /**
   * TripalDbx service object.
   *
   * This provides in-class access to the non-schema specific Tripal DBX API
   * methods.
   *
   * @var object \Drupal\tripal\TripalDBX\TripalDbx
   */
  protected $tripalDbxApi = NULL;

  /**
   * Retrieve schema details from selected source in the requested format.
   *
   * @param array $parameters
   *   An array of key-value parameters:
   *   - 'source': either 'database' to extract data from database or 'file' to
   *     get the data from a static YAML file.
   *     Default: 'file'.
   *   - 'version': version of the Tripal DBX managed schema to fetch from a file.
   *     Ignored for 'database' source.
   *     Default: implementation specific.
   *   - 'format': return format, either 'SQL' for an array of SQL string,
   *     'Drupal' for Drupal schema API, 'none' to return nothing or anything
   *     else to provide a data structure as returned by
   *     TripalDbx::parseTableDdl. If the selected source is 'file', the format
   *     parameter will be ignored and 'Drupal' format will be used.
   *     Default: TripalDbx::parseTableDdl data structure structure.
   *   - 'clear': if not empty, cache will be cleared.
   *
   * @return
   *   An array with details for the current schema version as defined by
   *   $parameters values.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   */
  abstract public function getSchemaDef(array $parameters) :array;

  /**
   * Constructor.
   *
   * OVERRIDES \Drupal\Core\Database\Schema:__construct().
   *
   * Overrides default constructor to manage the Tripal DBX managed schema name.
   *
   * The TripalDbxSchema object should be instantiated by the
   * TripalDbxConnection::schema() method in order to avoid issues when the
   * default Tripal DBX managed schema name is changed in the TripalDbxConnection
   * object which could lead to issues.
   *
   * If you choose to instantiate a TripalDbxSchema object yourself, you are
   * responsible to not change the Tripal DBX managed schema name of the connection
   * object used to instantiate this TripalDbxSchema.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $connection
   *   A Tripal DBX connection object.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   */
  public function __construct(
    \Drupal\tripal\TripalDBX\TripalDbxConnection $connection
  ) {
    $schema_name = $connection->getSchemaName();
    // Get a TripalDbx object.
    $this->tripalDbxApi = \Drupal::service('tripal.dbx');
    // Make sure the schema name is not empty and valid.
    if ($schema_issue = $this->tripalDbxApi->isInvalidSchemaName($schema_name, TRUE)) {
      throw new SchemaException("Could not create TripalDbxSchema object with the schema name '$schema_name'.\n$schema_issue");
    }
    parent::__construct($connection);

    $this->defaultSchema = $schema_name;
    $this->quotedDefaultSchema = $this->connection->getQuotedSchemaName();
  }

  /**
   *
   */
  public function initialize() {
    if (!$this->initialized) {
      $check_func_sql_query = "
        SELECT COUNT(1) AS \"funcs\"
        FROM pg_proc p
          JOIN pg_namespace n ON (p.pronamespace = n.oid)
        WHERE
          n.oid = pg_my_temp_schema()
          AND p.proname IN ('tripal_get_table_ddl', 'tripal_clone_schema')
      ";
      $is_initialized = (bool) $this->connection->query($check_func_sql_query)->fetchField();
      if (!$is_initialized) {
        // Load functions.
        $logger = \Drupal::service('tripal.logger');
        $sql_cloner_path =
          \Drupal::service('extension.list.module')->getPath('tripal')
          . '/src/TripalDBX/pg-clone-schema/clone_schema.sql'
        ;
        // Retrieve the SQL file.
        $sql = file_get_contents($sql_cloner_path);
        if (!$sql) {
          $message = "Failed to load PostgreSQL cloning functions: unable to read '$sql_cloner_path' file content.";
          $logger->error($message);
        }

        // Remove starting comments (not the ones in functions).
        $sql = preg_replace('/^--[^\n]*\n(?:\s*\n)*/m', '', $sql);
        $this->connection->query(
          $sql,
          [],
          [
            'allow_delimiter_in_query' => TRUE,
          ]
        );

        // Check functions were installed.
        $func_count = $this->connection->query($check_func_sql_query)->fetch();
        if (!$func_count || ($func_count->funcs < 2)) {
          $message =
            "Failed to load PostgreSQL cloning functions ($sql_cloner_path).";
          $logger->error($message);
        }
      }
      $this->initialized = TRUE;
    }
  }

  /**
   * Returns current schema name.
   *
   * @return string
   *   Current schema name.
   */
  public function getSchemaName() :string {
    return $this->defaultSchema;
  }

  /**
   * Finds all tables that are like the specified base table name.
   *
   * OVERRIDES \Drupal\Core\Database\Schema:findTables().
   *
   * NOTE: In Drupal 10 individually prefixed tables will no longer be supported.
   *  At this point we should re-evaluate if this override is still needed.
   *
   * Overrides the PostgreSQL implementation for two reasons:
   *  1. Switch back to the generic Drupal implementation to prepare for other
   *     database driver support in the future.
   *  2. To override where tablePrefix() is called to include the optional
   *     parameter indicating to use the Tripal DBX API.
   *
   * @param string $table_expression
   *   An SQL expression, for example "cache_%" (without the quotes).
   * @return array
   *   Both the keys and the values are the matching tables.
   */
  public function findTables($table_expression) {

    if ($table_expression === NULL) {
      return [];
    }

    // Load all the tables up front in order to take into account per-table
    // prefixes. The actual matching is done at the bottom of the method.
    $individually_prefixed_tables = $this->connection
      ->getUnprefixedTablesMap();
    $tables = [];

    // Normally, we would heartily discourage the use of string
    // concatenation for conditionals like this however, we
    // couldn't use \Drupal::database()->select() here because it would prefix
    // information_schema.tables and the query would fail.
    // Don't use {} around information_schema.tables table.
    $condition = $this
      ->buildTableNameCondition('%', 'LIKE');
    $condition
      ->compile($this->connection, $this);
    $results = $this->connection
      ->query("SELECT table_name AS table_name FROM information_schema.tables WHERE " . (string) $condition, $condition
      ->arguments());
    foreach ($results as $table) {

      // Take into account tables that have an individual prefix.
      $prefix_length = 0;
      if (isset($individually_prefixed_tables[$table->table_name])) {
        $prefix = $this->connection
          ->tablePrefix($individually_prefixed_tables[$table->table_name], TRUE);

        if (!empty($prefix)) {
          $prefix_length = strlen($prefix);
        }
      }

      // Remove the prefix from the returned tables.
      $unprefixed_table_name = substr($table->table_name, $prefix_length);

      // The pattern can match a table which is the same as the prefix. That
      // will become an empty string when we remove the prefix, which will
      // probably surprise the caller, besides not being a prefixed table. So
      // remove it.
      if (!empty($unprefixed_table_name)) {
        $tables[$unprefixed_table_name] = $unprefixed_table_name;
      }
    }

    // Convert the table expression from its SQL LIKE syntax to a regular
    // expression and escape the delimiter that will be used for matching.
    $table_expression = str_replace([
      '%',
      '_',
    ], [
      '.*?',
      '.',
    ], preg_quote($table_expression, '/'));
    $tables = preg_grep('/^' . $table_expression . '$/i', $tables);
    return $tables;
  }

  /**
   * Fetch the list of blobs and sequences used on a table.
   *
   * OVERRIDES \Drupal\Core\Database\Schema:queryTableInformation().
   *
   * Override is required for proper table prefixing. While the exact mechanism
   * isn't clear, if this function is removed then tests will fail due to
   * incorrect table prefixing.
   *
   * @param string $table_name
   *   The non-prefixed name of the table.
   * @return object
   *    An object with two member variables:
   *     - 'blob_fields' that lists all the blob fields in the table.
   *     - 'sequences' that lists the sequences used in that table.
   */
  public function queryTableInformation($table) {

    // Generate a key to reference this table's information on.
    $key = $this->connection
      ->prefixTables('{' . $table . '}');

    // Take into account that temporary tables are stored in a different schema.
    // \Drupal\Core\Database\Connection::generateTemporaryTableName() sets the
    // 'db_temporary_' prefix to all temporary tables.
    if (strpos($key, '.') === FALSE && strpos($table, 'db_temporary_') === FALSE) {
      $key = 'public.' . $key;
    }
    elseif (strpos($table, 'db_temporary_') !== FALSE) {
      $key = $this
        ->getTempNamespaceName() . '.' . $key;
    }
    if (!isset($this->tableInformation[$key])) {
      $table_information = (object) [
        'blob_fields' => [],
        'sequences' => [],
      ];
      $this->connection
        ->addSavepoint();
      try {

        // The bytea columns and sequences for a table can be found in
        // pg_attribute, which is significantly faster than querying the
        // information_schema. The data type of a field can be found by lookup
        // of the attribute ID, and the default value must be extracted from the
        // node tree for the attribute definition instead of the historical
        // human-readable column, adsrc.
        $sql = <<<'EOD'
SELECT pg_attribute.attname AS column_name, format_type(pg_attribute.atttypid, pg_attribute.atttypmod) AS data_type, pg_get_expr(pg_attrdef.adbin, pg_attribute.attrelid) AS column_default
FROM pg_attribute
LEFT JOIN pg_attrdef ON pg_attrdef.adrelid = pg_attribute.attrelid AND pg_attrdef.adnum = pg_attribute.attnum
WHERE pg_attribute.attnum > 0
AND NOT pg_attribute.attisdropped
AND pg_attribute.attrelid = :key::regclass
AND (format_type(pg_attribute.atttypid, pg_attribute.atttypmod) = 'bytea'
OR pg_get_expr(pg_attrdef.adbin, pg_attribute.attrelid) LIKE 'nextval%')
EOD;
        $result = $this->connection
          ->query($sql, [
          ':key' => $key,
        ]);
      } catch (\Exception $e) {
        $this->connection
          ->rollbackSavepoint();
        throw $e;
      }
      $this->connection
        ->releaseSavepoint();

      // If the table information does not yet exist in the PostgreSQL
      // metadata, then return the default table information here, so that it
      // will not be cached.
      if (empty($result)) {
        return $table_information;
      }
      foreach ($result as $column) {
        if ($column->data_type == 'bytea') {
          $table_information->blob_fields[$column->column_name] = TRUE;
        }
        elseif (preg_match("/nextval\\('([^']+)'/", $column->column_default, $matches)) {

          // We must know of any sequences in the table structure to help us
          // return the last insert id. If there is more than 1 sequences the
          // first one (index 0 of the sequences array) will be used.
          $table_information->sequences[] = $matches[1];
          $table_information->serial_fields[] = $column->column_name;
        }
      }
      $this->tableInformation[$key] = $table_information;
    }
    return $this->tableInformation[$key];
  }

  /**
   * Checks if an index exists in the given table.
   *
   * OVERRIDES \Drupal\Core\Database\Schema:indexExists().
   *
   * Our version of this method adds an optional parameter $exact_name to support
   * exact name matches since many biological data-focused databases will not
   * follow the Drupal index naming pattern. For example, Chado does not follow
   * this pattern.
   *
   * @param $table
   *   The name of the table in Tripal DBX managed schema.
   * @param $name
   *   The full name of the index (including the '_idx' part for instance).
   * @param bool $exact_name
   *   If FALSE, Drupal will append to the given name the '__idx' suffix (added
   *   when ::addIndex is used) and will adjust the name if needed (length). If
   *   TRUE, the function assumes the given index name is complete.
   *
   * @return
   *   TRUE if the given index exists, otherwise FALSE.
   */
   public function indexExists($table, $index_name, bool $exact_name = FALSE) {
    if ($exact_name) {
      return (bool) $this->connection
        ->query("SELECT 1 FROM pg_indexes WHERE indexname = '{$index_name}'")
        ->fetchField();
    }
    else {
      return parent::indexExists($table, $index_name);
    }
  }

  /**
   * Returns the size in bytes of a PostgreSQL schema.
   *
   * @return integer
   *   The size in bytes of the schema or 0 if the size is not available.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   */
  public function getSchemaSize() :int {
    return $this->tripalDbxApi->getSchemaSize(
      $this->defaultSchema,
      $this->connection
    );
  }

  /**
   * Check that the given schema exists.
   *
   * @return bool
   *   TRUE/FALSE depending upon whether or not the schema exists.
   */
  public function schemaExists() :bool {
    return $this->tripalDbxApi->schemaExists(
      $this->defaultSchema,
      $this->connection
    );
  }

  /**
   * Check that the given function exists.
   *
   * Example:
   * @code
   * $exists = $tripaldbx_schema->functionExists('dummyfunc', ['bigint', 'varchar']);
   * @endcode
   *
   * @param string $function_name
   *   The name of the function.
   * @param array $func_parameters
   *   An ordered array of input parameter types that are part of the function
   *   signature.
   *
   * @return bool
   *   TRUE if the function exists in the schema and FALSE otherwise.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   */
  public function functionExists(
    string $function_name,
    array $function_parameters
  ) :bool {
    $schema_name = $this->defaultSchema;
    $db_name = $this->connection->getDatabaseName();

    if (empty($function_name) || !is_array($function_parameters)) {
      throw new SchemaException('Invalid parameters for functionExists().');
    }
    // Reformat function signature.
    $function_signature = preg_replace(
      '/(?:^\s+(\S))|\s+(,)|( )\s+|(?:(\S)\s+$)/',
      '\1\2\3\4',
      strtolower(implode(', ', $function_parameters))
    );
    // Remap invalid types (for people not using the appropriate names).
    $function_signature = preg_replace(
        [
          '/varchar/',
          '/(^|\W)int4?(\W|$)/',
          '/(^|\W)int8(\W|$)/',
          '/(^|\W)bool(\W|$)/',
          '/(^|\W)string(\W|$)/',
        ],
        [
          'character varying',
          '\1integer\2',
          '\1bigint\2',
          '\1boolean\2',
          '\1text\2',
        ],
        $function_signature
    );

    $sql_query = "
      SELECT TRUE
      FROM
        pg_proc p
        JOIN pg_namespace n ON (
          n.oid = p.pronamespace
          AND n.nspname = :schema
        )
      WHERE
        p.proname ILIKE :function_name
        AND pg_get_function_identity_arguments(p.oid) ILIKE :func_args
    ";
    $args = [
      ':function_name' => $function_name,
      ':func_args' => $function_signature,
      ':schema' => $schema_name,
    ];
    $result = $this->connection->query($sql_query, $args)->fetch();

    return !empty($result);
  }

  /**
   * Check that any given sequence in a table exists.
   *
   * When the sequence name is not known, a table name and a column name can be
   * specified instead. Then, the sequence name will be guessed from those and
   * returned into the given $sequence_name variable if provided. If both the
   * sequence name and the table and column names are specified, only the
   * sequence name will be taken into account.
   *
   * This information can also be extracted from
   * ::queryTableInformation()['sequences'].
   *
   * @param string $table_name
   *   The name of the table the sequence is used in.
   * @param string $column_name
   *   The name of the column the sequence is used to populate.
   * @param string &$sequence_name
   *   The name of the sequence is to check if known. Otherwise, the function
   *   will set it.
   *
   * @return bool
   *   TRUE if the seqeuence exists and FALSE if it does not.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   */
  public function sequenceExists(
    ?string $table_name = NULL,
    ?string $column_name = NULL,
    ?string &$sequence_name = NULL
  ) :bool {

    $schema_name = $this->defaultSchema;
    // If no sequence name is provided, guess it.
    if ($sequence_name === NULL) {
      // Make sure we have a table and a column.
      if (empty($table_name) || empty($column_name)) {
        throw new SchemaException('Invalid parameters for checkSchemaSequenceExists(). You must specify either at least a table and a column name, or a sequence name.');
      }

      $prefixed_table = $this->connection->prefixTables(
        '{1:' . $table_name . '}'
      );
      $sequence_name = $this->connection
        ->query(
          'SELECT pg_get_serial_sequence(:schema_table, :column);',
          [':schema_table' => $prefixed_table, ':column' => $column_name]
        )->fetchField();

      // Remove prefixed table from sequence name.
      if ($sequence_name !== NULL) {
        $sequence_name = str_replace($schema_name . '.', '', $sequence_name);
      }
    }

    $sql_query = "
      SELECT TRUE
      FROM information_schema.sequences
      WHERE
        sequence_name = :sequence_name
        AND sequence_schema = :schema
        AND sequence_catalog = :catalog
    ";
    $args = [
      ':sequence_name' => $sequence_name,
      ':schema' => $schema_name,
      ':catalog' => $this->connection->getDatabaseName(),
    ];
    $result = $this->connection->query($sql_query, $args)->fetch();

    return !empty($result);
  }

  /**
   * Check that the constraint exists.
   *
   * OVERRIDES \Drupal\Core\Database\Schema:constraintExists().
   *
   * Our version of this method adds an optional parameter $type to support
   * type-specific searching and exact name matches since many biological
   * data-focused databases will not follow the Drupal naming patterns.
   * For example, Chado does not follow these patterns.
   *
   * @param string $table
   *   The table the constraint applies to.
   * @param string $constraint_name
   *   The name of the constraint you want to check.
   * @param ?string $type
   *   The type of constraint. Should be one of "PRIMARY KEY", "UNIQUE", or
   *   "FOREIGN KEY".
   *
   * @return
   *   TRUE if the constraint exists and FALSE otherwise.
   */
  public function constraintExists(
    $table,
    $constraint_name,
    ?string $type = NULL
  ) {
    // Use parent method if no type was specified.
    if (empty($type)) {
      return parent::constraintExists($table, $constraint_name);
    }

    // Next check the constraint is there.
    $constraint_exists = $this->connection->query("
      SELECT TRUE
      FROM information_schema.table_constraints
      WHERE table_name ILIKE :table
        AND constraint_type ILIKE :type
        AND constraint_name ILIKE :name
      ;",
      [
        ':table' => $table,
        ':name' => $constraint_name,
        ':type' => $type,
      ])
      ->fetchField()
    ;

    return !empty($constraint_exists);
  }

  /**
   * Check that the primary key exists, has a sequence and a constraint.
   *
   * @param string $table
   *   The table you want to check the primary key for.
   * @param ?string $column
   *   (optional) The name of the primary key column.
   *
   * @return
   *   TRUE if the primary key meets all the requirements and FALSE otherwise.
   */
  public function primaryKeyExists(
    string $table,
    ?string $column = NULL
  ) :bool {

    // If they didn't supply the column, then we can look it up.
    if ($column === NULL) {
      $parameters = ['source' => 'database', 'format' => 'Drupal',];
      $table_schema = $this->getTableDef($table, $parameters);
      $column = $table_schema['primary key'][0];
    }

    // If there is no primary key then we can't check it.
    // It neither passes nor fails validation.
    if (empty($column)) {
      $this->connection->getMessageLogger()->notice(
        "Cannot check the validity of the primary key for '$table' since there is no record of one."
      );
      return FALSE;
    }

    // Check the column exists.
    $column_exists = $this->fieldExists($table, $column);
    if (!$column_exists) {
      return FALSE;
    }

    // First check that the sequence exists.
    $sequence_exists = $this->sequenceExists($table, $column);
    if (!$sequence_exists) {
      return FALSE;
    }

    // Next check the constraint is there.
    $constraint_exists = $this->connection->query(
      "SELECT TRUE
      FROM information_schema.table_constraints
      WHERE table_name ILIKE :table AND constraint_type = 'PRIMARY KEY';",
      [':table' => $table])->fetchField();

    return $constraint_exists;
  }

  /**
   * If a FK exists between two tables, return the left and right columns.
   *
   * @param string $left_table
   *   The name of the table the foreign key resides in. E.g. 'feature' for
   *   the feature.type_id => cvterm.cvterm_id foreign key.
   * @param string $right_table
   *   The name of the table the foreign key refers to. For the example
   *   above it would be cvterm.
   * @return array
   *    The foreign key definition
   */
  public function getForeignKeyDef(string $left_table, string $right_table) {
    $left_def = $this->getTableDef($left_table, ['format' => 'Drupal']);
    if (
      !array_key_exists('foreign keys', $left_def)
      || !array_key_exists($right_table, $left_def['foreign keys'])) {
        return NULL;
    }
    return $left_def['foreign keys'][$right_table];
  }

  /**
   * Indicates whether a FK exists between two tables.
   *
   * @param string $left_table
   *   The name of the table the foreign key resides in. E.g. 'feature' for
   *   the feature.type_id => cvterm.cvterm_id foreign key.
   * @param string $right_table
   *   The name of the table the foreign key refers to. For the example
   *   above it would be cvterm.
   * @return bool
   *   TRUE if a foreign key exists, FALSE if not.
   */
  public function foreignKeyExists(string $left_table, string $right_table) {
    $left_def = $this->getForeignKeyDef($left_table, $right_table);
    if ($left_def) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Check the foreign key constrain specified exists.
   *
   * @param string $base_table
   *   The name of the table the foreign key resides in. E.g. 'feature' for
   *     the feature.type_id => cvterm.cvterm_id foreign key.
   * @param string $base_column
   *   The name of the column that is a foreign key in. E.g. 'type_id' for
   *     the feature.type_id => cvterm.cvterm_id foreign key.
   *
   * @return
   *   TRUE if the constraint exists and FALSE otherwise.
   */
  public function foreignKeyConstraintExists(
    string $base_table,
    string $base_column
  ) {
    // Since we don't have a constraint name, we have to use the known pattern for
    // creating these names in order to make this check.
    // This is due to PostgreSQL not storing column information for constraints
    // in the information_schema tables.
    $constraint_name = $base_table . '_' . $base_column . '_fkey';

    return $this->constraintExists($base_table, $constraint_name, 'FOREIGN KEY');
  }

  /**
   * Retrieves the list of tables in the given schema.
   *
   * Note: only peristant tables (ie. no unlogged or temporary tables) visible
   * by current DB user are returned.
   *
   * @param array $include
   *   An associative array to select other element type to include.
   *   Supported keys are:
   *   'table': include all tables;
   *   'base': include only base tables (as defined in the original Tripal Dbx
   *           schema definition);
   *   'custom': include only custom tables (not part of the original Tripal Dbx
   *           schema definition);
   *   'view': include views;
   *   'partition': include partitions;
   *   'materialized view': include PostgreSQL materialized views (not to be
   *     confused with Tripal materialized views which currently are regular
   *     tables);
   *   If both 'base' and 'custom' are specified, all tables are returned.
   *   Default: tables only (empty array).
   *
   * @return array
   *   An associative array where the keys are the table names and values are
   *   array of properties such as:
   *   -'name': table name
   *   -'type': one of 'table', 'view', 'partition' and 'materialized view' for
   *     PostgreSQL materialized views.
   *   -'status': either 'base' for a base table, or 'custom' for a custom table
   *     or a tripal materialized view, or 'other' for other elements.
   */
  public function getTables(
    array $include = []
  ) :array {
    static $type_to_name = [
      'r' => 'table',
      'v' => 'view',
      'p' => 'partition',
      'm' => 'materialized view',
    ];
    static $name_to_type = [
      'table'                    => 'r',
      'base'                     => 'r',
      'custom'                   => 'r',
      'tripal materialized view' => 'r',
      'view'                     => 'v',
      'partition'                => 'p',
      'materialized view'        => 'm',
    ];
    $schema_name = $this->defaultSchema;
    $include_types = [];
    foreach ($include as $index => $type_name) {
      if (array_key_exists($type_name, $name_to_type)) {
        $include_types[$name_to_type[$type_name]] = TRUE;
      }
    }
    $include_types = array_keys($include_types);
    // We want tables by default.
    if (empty($include_types)) {
      $include_types = ['r'];
    }

    // No "{}" around table names as we query system tables.
    $sql_query = "
      SELECT
        DISTINCT c.relname,
        c.relkind
      FROM pg_class c
        JOIN pg_namespace n ON (n.oid = c.relnamespace)
      WHERE
        n.nspname = :schema_name
        AND c.relkind IN (:object_types[])
        AND c.relpersistence = 'p'
      ORDER BY c.relkind, c.relname;
    ";
    // Get tables.
    $results = $this
      ->connection
      ->query(
        $sql_query,
        [
          ':schema_name' => $schema_name,
          ':object_types[]' => $include_types,
        ]
      )
    ;

    $tables = [];
    // Get original schema to differentiate base and custom.
    $schema_def = $this->getSchemaDef(['source' => 'file']);

    // Check if tables should be filtered according to their origin
    // (base/custom).
    $base_tables = in_array('base', $include);
    $custom_tables = in_array('custom', $include);
    if (!$base_tables && !$custom_tables && in_array('table', $include)) {
      $base_tables = $custom_tables = TRUE;
    }
    foreach ($results as $table) {
      $table_status = array_key_exists($table->relname, $schema_def)
        ? 'base'
        : 'custom'
      ;
      if (('r' != $table->relkind)
          || ($base_tables && ($table_status == 'base'))
          || ($custom_tables && ($table_status == 'custom'))
      ) {
        $tables[$table->relname] = [
          'name' => $table->relname,
          'type' => $type_to_name[$table->relkind],
          'status' => ('r' == $table->relkind) ? $table_status : 'other',
        ];
      }
    }

    return $tables;
  }

  /**
   * Returns the specified table structure details.
   *
   * @param string $table
   *   The name of the table.
   * @param array $parameters
   *   An array of key-value parameters:
   *   - 'source': either 'database' to extract data from database or 'file' to
   *     get the data from a static YAML file or 'tripal' to get the data from
   *     Tripal records.
   *     Default: 'file'.
   *   - 'version': version of the Tripal DBX managed schema to fetch from a file.
   *     Ignored fot 'database' source.
   *     Default: implementation specific.
   *   - 'format': return format, either 'sql' for an array of SQL string,
   *     'drupal' for Drupal schema API, 'none' to return nothing or anything
   *     else (like 'default') to provide a data structure as returned by
   *     TripalDbx::parseTableDdl plus a 'referenced_by' key containing all the
   *     referencing tables returned by ::getReferencingTables. If the selected
   *     source is 'file' or 'tripal', the format parameter will be ignored and
   *    'Drupal' format will be used.
   *     Default: TripalDbx::parseTableDdl data structure structure.
   *   - 'clear': if not empty, cache will be cleared.
   *
   * @return
   *   An array with details from the specified source for the specified table
   *   using the specified format or an empty array if table not found.
   */
  public function getTableDef(string $table, array $parameters) :array {
    static $table_structures = [];

    $source = $parameters['source'] ?? 'file';
    $format = strtolower($parameters['format'] ?? '');
    $version = $parameters['version']
      ?? $this->connection->getVersion()
    ;
    if (!empty($parameters['clear'])) {
      $table_structures = [];
    }
    if ('none' == $format) {
      return [];
    }

    if ('file' == $source) {
      // Use Connection to get the whole schema definition from a file.
      $schema_parameters = [
        'source' => 'file',
        'format' => 'drupal',
        'version' => $version,
      ];
      // Adds 'clear' and 'none' if needed.
      $schema_parameters += $parameters;
      $schema_def = $this->getSchemaDef($schema_parameters);
      if (array_key_exists($table, $schema_def)) {
        $table_def = $schema_def[$table];
      }
      else {
        $table_def = [];
      }
    }
    elseif ('tripal' == $source) {
      $sql = "SELECT schema FROM {tripal_custom_tables} WHERE table_name = :table_name;";
      $results = $this->connection->query($sql, [':table_name' => $table]);
      $custom = $results->fetchObject();
      if (!$custom) {
        $table_def = [];
      }
      else {
        $table_def = unserialize($custom->schema);
      }
    }
    elseif ('database' == $source) {
      $cache_key = $this->defaultSchema . '/' . $table . '/' . $format;
      if (!isset($table_structures[$cache_key])) {
        $table_ddl = $this->getTableDdl($table);
        if ('sql' == $format) {
          $table_structures[$cache_key] = [$table_ddl];
        }
        elseif ('drupal' == $format) {
          $table_structures[$cache_key] =
            $this->tripalDbxApi->parseTableDdlToDrupal($table_ddl);
        }
        else {
          $table_structures[$cache_key] =
            $this->tripalDbxApi->parseTableDdl($table_ddl);
          $referencing_tables = $this->getReferencingTables($table);
          $table_structures[$cache_key]['referenced_by'] = $referencing_tables;
        }
      }
      $table_def = $table_structures[$cache_key];
    }
    else {
      throw new SchemaException("Invalid table definition source: '$source'.");
    }
    return $table_def;
  }

  /**
   * Retrieves the table DDL (table data definition language).
   *
   * @param string $table
   *   The name of the table to retrieve.
   * @param bool $clear_cache
   *   If TRUE, cache is cleared.
   *
   * @returns string
   *   A set of SQL queries used to create the table including its constraints
   *   or an empty string if the table was not found.
   */
  public function getTableDdl(
    string $table_name,
    bool $clear_cache = FALSE
  ) :string {
    static $db_ddls = [];

    if ($clear_cache) {
      $db_ddls = [];
    }

    $cache_key = $this->defaultSchema . '/' . $table_name;
    if (!isset($db_ddls[$cache_key])) {
      $schema_name = $this->defaultSchema;
      $this->initialize();

      $sql_query = "
        SELECT
          pg_temp.tripal_get_table_ddl(:schema, :table, TRUE)
          AS \"definition\";
      ";
      $result = $this->connection->query(
          $sql_query,
          [':schema' => $schema_name, ':table' => $table_name, ]
      );
      $table_raw_definition = '';
      if ($result) {
        $table_raw_definition = $result->fetch(\PDO::FETCH_OBJ)->definition;
      }
      $db_ddls[$cache_key] = $table_raw_definition;
    }
    return $db_ddls[$cache_key];
  }

  /**
   * Retrieves tables referencing a given one.
   *
   * @param string $table
   *   The name of the table used as foreign table by other tables.
   * @param bool $clear_cache
   *   If TRUE, cache is cleared.
   *
   * @returns array
   *   First level are referencing table names and values are local table column
   *   names => referencing table column name.
   *   If no referencing tables are found, returns an empty array.
   */
  public function getReferencingTables(
    string $table_name,
    bool $clear_cache = FALSE
  ) :array {
    static $db_dependencies = [];

    if ($clear_cache) {
      $db_dependencies = [];
    }

    $cache_key = $this->defaultSchema . '/' . $table_name;
    if (!isset($db_dependencies[$cache_key])) {
      $schema_name = $this->defaultSchema;
      $sql_query = "
        SELECT
          c.conname AS \"conname\",
          rel.relname AS \"deptable\",
          al.attname AS \"depcolumn\",
          af.attname AS \"column\"
        FROM pg_catalog.pg_constraint c
          JOIN pg_catalog.pg_namespace nsp
            ON nsp.oid = c.connamespace
          JOIN pg_catalog.pg_class ref
            ON ref.oid = c.confrelid
          JOIN pg_attribute af
            ON af.attrelid = c.confrelid AND af.attnum = ANY (c.confkey)
          JOIN pg_catalog.pg_class rel
            ON rel.oid = c.conrelid
          JOIN pg_attribute al
            ON al.attrelid = c.conrelid AND al.attnum = ANY (c.conkey)
        WHERE c.contype = 'f'
           AND nsp.nspname = :schema
           AND ref.relname = :table
        ;
      ";
      $all_referencing = $this->connection->query(
          $sql_query,
          [':schema' => $schema_name, ':table' => $table_name, ]
      );
      $referencing_tables = [];
      foreach ($all_referencing as $referencing) {
        $referencing_tables[$referencing->deptable] = [
          $referencing->column => $referencing->depcolumn
        ];
      }
      $db_dependencies[$cache_key] = $referencing_tables;
    }
    return $db_dependencies[$cache_key];
  }

  /**
   * Creates the given schema.
   *
   * The schema to create must not exist. If an error occurs, an exception
   * is thrown.
   *
   * @throws \Drupal\Core\Database\DatabaseExceptionWrapper
   */
  public function createSchema() :void {
    $this->tripalDbxApi->createSchema($this->defaultSchema, $this->connection);
  }

  /**
   * Clones a schema into new (unexisting) one.
   *
   * The target schema must not exist. If $target_schema is omitted, current
   * schema will be used. For instance, if you want to clone "chado" schema into
   * a "chado_copy" schema, you would do something like this:
   * @code
   * // We assume 'chado_copy' schema does not exist.
   * $new_schema = new ChadoConnection('chado_copy');
   * $new_schema->clone('chado');
   * @endcode
   *
   * @param string $source_schema
   *   Source schema to clone.
   * @param ?string $target_schema
   *   Destination schema that will be created and filled with a copy of
   *   $source_schema. If not set, current schema will be the target.
   *
   * @return int
   *   The new schema size in bytes or 0 if the operation failed or the schema
   *   to clone was empty.
   */
  public function cloneSchema(
    string $source_schema
  ) :int {
    $target_schema = $this->defaultSchema;
    // Clone schema.
    $return_value = 0;
    try {
      $this->tripalDbxApi->cloneSchema(
        $source_schema,
        $target_schema,
        $this->connection
      );
      $return_value = $this->tripalDbxApi->getSchemaSize(
        $target_schema,
        $this->connection
      );
    }
    catch (\Exception $e) {
      $this->connection->getMessageLogger()->error($e->getMessage());
    }
    return $return_value;
  }

  /**
   * Renames a schema.
   *
   * The new schema name must not be used by an existing schema. If an error
   * occurs, an exception is thrown.
   *
   * @param string $new_schema_name
   *   New name to use.
   *
   * @throws \Drupal\Core\Database\DatabaseExceptionWrapper
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   *   if there is no current schema name.
   */
  public function renameSchema(
      string $new_schema_name
  ) :void {
    if (empty($this->defaultSchema)) {
      throw new SchemaException('Unable to rename current schema: no current schema set.');
    }
    $this->tripalDbxApi->renameSchema(
      $this->defaultSchema,
      $new_schema_name,
      $this->connection
    );
    // No error thrown, update members.
    $this->defaultSchema = $new_schema_name;
    $this->connection->setSchemaName($new_schema_name);
  }

  /**
   * Removes the given schema.
   *
   * The schema to remove must exist. If an error occurs, an exception is
   * thrown.
   *
   * @throws \Drupal\Core\Database\DatabaseExceptionWrapper
   */
  public function dropSchema() :void {
    $this->tripalDbxApi->dropSchema($this->defaultSchema, $this->connection);
  }

  /**
   * Overrides Drupal\Core\Database\Schema->tableExists().
   *
   * We needed to override it because core Drupal makes some assumptions
   * when building the where condition that do not match our multi-schema setup.
   */
  public function tableExists($table, $add_prefix = true) {

    // We can't use \Drupal::database()->select() here
    // because it would prefix information_schema.tables
    // and the query would fail.
    // Don't use {} around information_schema.tables table.
    return (bool) $this->connection
      ->query('SELECT TRUE AS table_exists FROM pg_tables
          WHERE schemaname=:schema AND tablename = :table',
          [':table' => $table, ':schema' => $this->getSchemaName()])
      ->fetchField();
  }
}
