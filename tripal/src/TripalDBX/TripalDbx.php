<?php

namespace Drupal\tripal\TripalDBX;

use Drupal\tripal\TripalDBX\Exceptions\ConnectionException;
use Drupal\tripal\TripalDBX\Exceptions\SchemaException;

/**
 * Tripal DBX
 *
 * This class provides methods which form the Tripal DBX API.
 * Specifically, this API focuses on extending Drupal to better handle cross
 * database and cross schema querying.
 *
 * This class should be accessed through the tripal.dbx service
 * and NOT initiated directly. For example,
 *   $tripaldbx = \Drupal::service('tripal.dbx');
 *
 * Additional Note: This class makes use of static member variables/properties
 * to avoid using global variables.
 */
class TripalDbx {

  /**
   * Schema name validation regular expression.
   *
   * Schema name must be all lowercase with no special characters with the
   * exception of underscores and diacritical marks (which can be uppercase).
   * ref.:
   * https://www.postgresql.org/docs/9.5/sql-syntax-lexical.html#SQL-SYNTAX-IDENTIFIERS
   * It should also not contain any space and must not begin with "pg_".
   * Note: capital letter could be used but are silently converted to
   * lowercase by PostgreSQL. Here, we want to avoid ambiguity so we forbid
   * uppercase. We also prevent the use of dollar sign in names '$' while it
   * should be valid, in order to stick to SQL standard and prevent issues
   * with PHP string interpolation.
   */
  public const SCHEMA_NAME_REGEXP =
    '[a-zA-Z_\\xA0-\\xFF][a-zA-Z_\\xA0-\\xFF0-9]{0,63}';

  /**
   * Table name validation regular expression.
   *
   * Table name must be all lowercase with no special characters with the
   * exception of underscores and diacritical marks (which can be uppercase).
   * ref.:
   * https://www.postgresql.org/docs/9.5/sql-syntax-lexical.html#SQL-SYNTAX-IDENTIFIERS
   * It should also not contain any space and must not begin with "pg_".
   * Note: capital letter could be used but are silently converted to
   * lowercase by PostgreSQL. Here, we want to avoid ambiguity so we forbid
   * uppercase. We also prevent the use of dollar sign in names '$' while it
   * should be valid, in order to stick to SQL standard and prevent issues
   * with PHP string interpolation.
   */
  public const TABLE_NAME_REGEXP =
    '[a-zA-Z_\\xA0-\\xFF][a-zA-Z_\\xA0-\\xFF0-9]{0,63}';

  /**
   * The Drupal schema name.
   *
   * @var string
   */
  protected static $drupalSchema;

  /**
   * Reserved schema name patterns.
   *
   * Schema names matching the given pattern will be considered invalid by
   * ::isInvalidSchemaName and will not be allowed in TripalDbxConnection
   * or TripalDbxSchema objects.
   *
   * @var ?array
   */
  protected static $reservedSchemaPatterns;

  /**
   * Get Drupal schema name.
   *
   * This function may return an empty string if the Drupal schema was not
   * found. It can happen if Drupal is stored in a different database
   * (and using a different connection) than the Chado (or biological schema)
   * one.
   *
   * Use:
   * @code
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   $drupal_schema = $tripaldbx->getDrupalSchemaName();
   * @endcode
   *
   * @return string
   *   The name of the schema used by Drupal installation or an empty string if
   *   not found/available.
   */
  public function getDrupalSchemaName() :string {

    // We only need to look this up if it hasn't been set yet.
    if (!isset(static::$drupalSchema)) {

      // Get Drupal connection details.
      $drupal_database = \Drupal::database();
      $connection_options = $drupal_database->getConnectionOptions();
      // Drupal <= 10.1 driver will be 'pgsql', Drupal 10.2 it will be 'Drupal\pgsql\Driver\Database\pgsql'
      if (array_key_exists('driver', $connection_options) AND (!preg_match('/pgsql$/', $connection_options['driver']))) {
        // Not using PostgreSQL. There might be something wrong!
        // @todo we may want to evaluate this further as it does tie our Drupal
        // database to being in pgsql. It doesn't support the case where Drupal
        // is in a separate database from Chado and thus may be of a different type.
        $schema_name = '';
      }
      else {
        // Check if Drupal has been installed in a specific schema other than
        // 'public'. If it is the case, Drupal database configuration 'prefix'
        // parameter will contain the schema name followed by a dot.
        if (!empty($connection_options['prefix']['default'])
            && (FALSE !== strrpos($connection_options['prefix']['default'], '.'))) {
          $schema_name = substr($connection_options['prefix']['default'], 0, -1);
        }
        else {
          // Otherwise, it should be the first schema used by PostgreSQL
          // (current_schema()) but we make sure the PostgreSQL "search_path" has
          // not been altered by looking for a table rather specific to Drupal
          // 'key_value'.
          $sql_query = "
            SELECT table_schema AS \"schema\"
            FROM information_schema.tables
            WHERE
              table_name = 'key_value'
              AND table_schema = current_schema()
              AND table_catalog = :database_name;
          ";
          $args = [
            ':database_name' => $connection_options['database'],
          ];
          $result = \Drupal::database()->query($sql_query, $args)->fetch();
          if ($result) {
            $schema_name = $result->schema;
          }
          else {
            $schema_name = '';
          }
        }
      }
      static::$drupalSchema = $schema_name;
    }
    return static::$drupalSchema;
  }

  /**
   * Check that the given schema name is a valid schema name.
   *
   * Schema name validation can be altered through the configuration variable
   * reserved_schema_patterns of tripaldbx.settings. This configuration
   * variable contains a list of regex with their description, used to reserve
   * schema name patterns. For instance, the key '_chado*' with the value
   * 'external (non-Drupal) chado instances' will make this function returns a
   * issue message saying that the pattern is reserved for 'external
   * (non-Drupal) chado instances' when a schema name '_chado_beta' is checked.
   *
   * Extending modules willing to reserve schema names should use something
   * similar to the following code in their "<module name>.install" file:
   * @code
   *   function <module name>_install($is_syncing) {
   *     // Reserves 'myschema' schema in 'reserved_schema_patterns' settings.
   *     $config = \Drupal::service('config.factory')
   *       ->getEditable('tripaldbx.settings');
   *     $reserved_schema_patterns = $config->get('reserved_schema_patterns') ?? [];
   *     $reserved_schema_patterns['myschema'] = 'my schema';
   *     $config->set('reserved_schema_patterns', $reserved_schema_patterns)->save();
   *   }
   *
   *   function <module name>_uninstall() {
   *     // Unreserves 'myschema' schemas in 'reserved_schema_patterns' settings.
   *     $config = \Drupal::service('config.factory')
   *       ->getEditable('tripaldbx.settings')
   *     ;
   *     $reserved_schema_patterns = $config->get('reserved_schema_patterns') ?? [];
   *     unset($reserved_schema_patterns['myschema']);
   *     $config->set('reserved_schema_patterns', $reserved_schema_patterns)->save();
   *   }
   * @endcode
   *
   * Use:
   * @code
   *   $schema_name = 'name_to_check';
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   if ($issue = $tripaldbx->isInvalidSchemaName($schema_name)) {
   *     throw new \Exception('Invalid schema name: ' . $issue);
   *   }
   * @endcode
   *
   * @param string $schema_name
   *   The name of the schema to validate.
   * @param bool $ignore_reservation
   *   If TRUE, reserved schema names are considered as valid.
   *   Default: FALSE
   * @param bool $reload_config
   *   Forces schema reserved names config reloading.
   *   Default: FALSE
   *
   * @return string
   *   An empty string if the schema name is valid or a string describing the
   *   issue in the name otherwise.
   */
  public function isInvalidSchemaName(
    string $schema_name,
    bool $ignore_reservation = FALSE,
    bool $reload_config = FALSE
  ) :string {

    // @todo: Maybe add a flag to enable message translation.
    // Reminder: exception messages should not be translated while user
    // interface should be. Here, we may use the messages in both situation.

    $issue = '';
    // Make sure we have a valid schema name.
    // -- Check that we were even given a schema name.
    if (empty($schema_name)) {
      $issue = 'No schema name was provided.';
    }
    // -- Check the name is not too long.
    if (63 < strlen($schema_name)) {
      $issue =
        'The schema name is too long and must contain strictly less than 64 characters.'
      ;
    }
    // -- Check it matches the set regex (i.e. does not contain illegal characters).
    elseif (!preg_match('#^' . static::SCHEMA_NAME_REGEXP . '$#', $schema_name)) {
      $issue =
        'The schema name must not begin with a number and only contain lower case letters, numbers, underscores and diacritical marks.'
      ;
    }
    // -- Does not begin with a reserved prefix.
    elseif ((0 === strpos($schema_name, 'pg_')) && !$ignore_reservation) {
      $issue =
        'The schema name must not begin with "pg_" (PostgreSQL reserved prefix).'
      ;
    }
    if (!$ignore_reservation) {
      //  -- Check reserved patterns.
      // Note: other reserved patterns should be added by other extensions when
      // they are installed, through config modifications.
      // See tripal_install() for an example.
      static::initSchemaReservation($reload_config);
      if ($reserved = static::isSchemaReserved($schema_name)) {
        $pattern = array_key_first($reserved);
        $description = $reserved[$pattern];
        $issue =
          "'$schema_name' matches the reservation pattern '$pattern' used for: $description."
        ;
      }
    }
    return $issue;
  }

  /**
   * Initializes schema reservations.
   *
   * @param bool $reload_config
   *  Forces config reloading.
   *  Default: FALSE
   */
  protected function initSchemaReservation(bool $reload_config = FALSE) :void {
    if ($reload_config || !isset(static::$reservedSchemaPatterns)) {
      $reserved_schema_patterns = \Drupal::config('tripaldbx.settings')
        ->get('reserved_schema_patterns')
        ?? [];
      static::$reservedSchemaPatterns = $reserved_schema_patterns;
    }
  }

  /**
   * Adds a schema name pattern for reservation.
   *
   * Schema names matching the given pattern will be considered invalid by
   * ::isInvalidSchemaName and will not be allowed in TripalDbxConnection or TripalDbxSchema
   * objects.
   *
   * @param string $pat_regex
   *   A simple schema name or a regular expression. Do not include regex
   *   delimiters nor starting '^' and ending '$' in the expression as they will
   *   be automatically added by the check system. Note that the '*' sign not
   *   preceded by a dot will be replaced by '.*'. It simplifies the way
   *   schema patterns can be defined by non-regex aware persons. If you need
   *   to use the '*' quantifier for a specific character, replace it by '{0,}'.
   *   ex.: 'internal_schemax{0,}' would match 'internal_schema' and
   *   'internal_schemaxxx' while 'internal_schemax*' would also match
   *   'internal_schemaxabcd'.
   * @param string $description
   *   The description of the reservation that may be displayed to users when a
   *   schema name is denied.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   *   if the pattern is empty or does not contain any valid schema name
   *   character.
   */
  public function reserveSchemaPattern(
    string $pat_regex,
    string $description = ''
  ) :void {
    static::initSchemaReservation();
    if (empty($pat_regex)
        || (!preg_match('/[a-z_\\xA0-\\xFF0-9]/i', $pat_regex))
    ) {
      throw new SchemaException("Invalid schema name pattern.");
    }
    static::$reservedSchemaPatterns[$pat_regex] = $description;
  }

  /**
   * Returns the list of reserved schema name pattern.
   *
   * @return array
   *   The list of reserved patterns as keys and their associated descriptions
   *   as values.
   */
  public function getReservedSchemaPattern() :array {
    static::initSchemaReservation();
    return static::$reservedSchemaPatterns;
  }

  /**
   * Removes a schema name reservation pattern from the list.
   *
   * @param string $pat_regex
   *   The regular expression to remove from the list if it is there.
   * @param bool $free_all_matching
   *   If TRUE, the provided pattern will be considered as a regular string with
   *   no special characters meaning and any current pattern matching that
   *   string will be removed form current reservation list.
   *
   * @return array
   *   Returns an associative array containing the removed patterns as keys and
   *   their associated descriptions as values. An empty array if no pattern has
   *   been removed.
   */
  public function freeSchemaPattern(
    string $pat_regex,
    bool $free_all_matching = FALSE
  ) :array {
    static::initSchemaReservation();
    $removed_patterns  = [];
    if (array_key_exists($pat_regex, static::$reservedSchemaPatterns)) {
      $removed_patterns[$pat_regex] =
        static::$reservedSchemaPatterns[$pat_regex];
      unset(static::$reservedSchemaPatterns[$pat_regex]);
    }
    if ($free_all_matching) {
      foreach (static::$reservedSchemaPatterns as $regex => $reason) {
        $regex_fix = preg_replace('/(?<!\.)\*/', '.*', $regex);
        if (preg_match("/^$regex_fix\$/", $pat_regex)) {
          $removed_patterns[$regex] =
            static::$reservedSchemaPatterns[$regex];
          unset(static::$reservedSchemaPatterns[$regex]);
        }
      }
    }
    return $removed_patterns;
  }

  /**
   * Tells if a schema name is reserved or not.
   *
   * @param string $schema_name
   *   The name of the schema to check.
   *
   * @return bool|array
   *   FALSE if the given schema name is not reserved, otherwise it will return
   *   an array with the reservation pattern matching the name as keys and their
   *   associated descriptions as values.
   */
  public function isSchemaReserved(string $schema_name) {
    static::initSchemaReservation();
    $reserved = FALSE;
    foreach (static::$reservedSchemaPatterns as $reserved_pattern => $description) {
      // Adds regex wildcard
      $reserved_pattern = preg_replace('/(?<!\.)\*/', '.*', $reserved_pattern);
      if (preg_match("/^$reserved_pattern\$/", $schema_name)) {
        if ($reserved === FALSE) {
          $reserved = [];
        }
        $reserved[$reserved_pattern] = $description;
      }
    }
    return $reserved;
  }

  /**
   * Returns a PostgreSQL quoted object name.
   *
   * Use PostgreSQL to quote an object identifier if needed for SQL queries.
   * For instance, a schema or a table name using special characters may need to
   * be quoted if used in SQL queries.
   *
   * For instance, with a schema called "schema" and a table "co$t", a query
   * should look like:
   * @code
   *   SELECT * FROM schema."co$t";
   * @endcode
   * while with a schema called "schéma" and a table "cost", a query should look
   * like:
   * @code
   *   SELECT * FROM "schéma".cost;
   * @endcode
   * Inappropriate object quoting would lead to SQL errors.
   * This function has to be called for each object separately (one time for the
   * schema and one time for the table in above examples) and it only adds quote
   * when necessary.
   *
   * @param string $object_id
   *  Object name to quote if needed.
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or TripalDBX connection object.
   *   If NULL, current Drupal database is used.
   *
   * @return string
   *   The quoted object name or the initial name if no quote needed.
   */
  public function quoteDbObjectId(
    string $object_id,
    ?\Drupal\Core\Database\Driver\pgsql\Connection $db = NULL
  ) :string {
    $db = $db ?? \Drupal::database();
    $sql = "SELECT quote_ident(:object_id) AS \"qi\";";
    $quoted_object_id = $db
      ->query($sql, [':object_id' => $object_id])
      ->fetch()
      ->qi ?: $object_id
    ;
    return $quoted_object_id;
  }

  /**
   * Check that the given schema exists.
   *
   * Use:
   * @code
   *   $schema_name = 'name_to_test';
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   if ($tripaldbx->schemaExists($schema_name)) {
   *     // Schema exists.
   *   }
   * @endcode
   *
   * @param string $schema_name
   *   Schema name.
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or TripalDBX connection object.
   *   If NULL, current Drupal database is used.
   *
   * @return bool
   *   TRUE/FALSE depending upon whether or not the schema exists.
   */
  public function schemaExists(
    string $schema_name,
    ?\Drupal\Core\Database\Driver\pgsql\Connection $db = NULL
  ) :bool {
    $db = $db ?? \Drupal::database();

    // First make sure we have a valid schema name.
    $tripaldbx = \Drupal::service('tripal.dbx');
    $issue = $tripaldbx->isInvalidSchemaName($schema_name, TRUE);
    if (!empty($issue)) {
      return FALSE;
    }

    $sql_query = "
      SELECT TRUE
      FROM pg_namespace
      WHERE
        has_schema_privilege(nspname, 'USAGE')
        AND nspname = :nspname
      ;
    ";
    $schema_exists = $db
      ->query($sql_query, [':nspname' => $schema_name])
      ->fetchField()
    ;
    return ($schema_exists ? TRUE : FALSE);
  }

  /**
   * Creates the given schema.
   *
   * The schema to create must not exist. If an error occurs, an exception
   * is thrown.
   *
   * Use:
   * @code
   *   $schema_name = 'name_to_create';
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   $tripaldbx->createSchema($schema_name);
   * @endcode
   *
   * @param string $schema_name
   *   Name of schema to create.
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or TripalDBX connection object.
   *   If NULL, current Drupal database is used.
   *
   * @throws \Drupal\Core\Database\DatabaseExceptionWrapper
   */
  public function createSchema(
    string $schema_name,
    ?\Drupal\Core\Database\Driver\pgsql\Connection $db = NULL
  ) :void {
    $db = $db ?? \Drupal::database();
    $tripaldbx = \Drupal::service('tripal.dbx');
    $schema_name_quoted = $tripaldbx->quoteDbObjectId($schema_name);
    // Create schema.
    $sql_query = "CREATE SCHEMA $schema_name_quoted;";
    $db->query($sql_query);
  }

  /**
   * Clones a schema into new (unexisting) one.
   *
   * The target schema must not exist.
   *
   * @code
   *   $source_schema_name = 'source';
   *   $target_schema_name = 'target';
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   $tripaldbx->cloneSchema($source_schema_name, $target_schema_name);
   * @endcode
   *
   * @param string $source_schema
   *   Source schema to clone.
   * @param string $target_schema
   *   Destination schema that will be created and filled with a copy of
   *   $source_schema.
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or TripalDBX connection object.
   *   If NULL, current Drupal database is used.
   *
   * @throws \Drupal\Core\Database\DatabaseExceptionWrapper
   */
  public function cloneSchema(
    string $source_schema,
    string $target_schema,
    ?object $db = NULL
  ) :void {

    // Initialize database if one is not supplied.
    //$db = $db ?? \Drupal::database();

    // Make sure we have the cloning PostgreSQL function.
    if (method_exists($db->schema(), 'initialize')) {
      $db->schema()->initialize();
    }
    else {
      throw new \Exception('Cloning a schema requires access to the initialize method for your schema. Please pass in a Tripal DBX connection with this method implemented (e.g. Chado implementation).');
    }

    // Clone schema.
    $sql_query =
      "SELECT pg_temp.tripal_clone_schema(:source_schema, :target_schema, TRUE, FALSE);"
    ;
    $args = [
      ':source_schema' => $source_schema,
      ':target_schema' => $target_schema,
    ];
    $db->query($sql_query, $args);
  }

  /**
   * Renames a schema.
   *
   * The new schema name must not be used by an existing schema. If an error
   * occurs, an exception is thrown.
   *
   * @code
   *   $old_schema_name = 'old';
   *   $new_schema_name = 'new';
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   $tripaldbx->renameSchema($old_schema_name, $new_schema_name);
   * @endcode
   *
   * @param string $old_schema_name
   *   The old schema name to rename.
   * @param string $new_schema_name
   *   New name to use.
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or Tripal DBX connection object.
   *   If NULL, current Drupal database is used.
   *
   * @throws \Drupal\Core\Database\DatabaseExceptionWrapper
   */
  public function renameSchema(
    string $old_schema_name,
    string $new_schema_name,
    ?\Drupal\Core\Database\Driver\pgsql\Connection $db = NULL
  ) :void {
    $db = $db ?? \Drupal::database();

    // Quote schema names if needed.
    $tripaldbx = \Drupal::service('tripal.dbx');
    $old_schema_name_quoted = $tripaldbx->quoteDbObjectId($old_schema_name);
    $new_schema_name_quoted = $tripaldbx->quoteDbObjectId($new_schema_name);

    // Rename schema.
    $sql_query =
      "ALTER SCHEMA $old_schema_name_quoted RENAME TO $new_schema_name_quoted;";
    $db->query($sql_query);
  }

  /**
   * Removes the given schema.
   *
   * The schema to remove must exist. If an error occurs, an exception is
   * thrown.
   *
   * @code
   *   $schema_name = 'schema_to_delete';
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   $tripaldbx->dropSchema($schema_name);
   * @endcode
   *
   * @param ?string $schema_name
   *   Name of schema to remove.
   * @param string $schema_name
   *   Schema name.
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or Tripal DBX connection object.
   *   If NULL, current Drupal database is used.
   *
   * @throws \Drupal\Core\Database\DatabaseExceptionWrapper
   */
  public function dropSchema(
    string $schema_name,
    ?\Drupal\Core\Database\Driver\pgsql\Connection $db = NULL
  ) :void {
    $db = $db ?? \Drupal::database();
    $tripaldbx = \Drupal::service('tripal.dbx');
    $schema_name_quoted = $tripaldbx->quoteDbObjectId($schema_name);
    // Drop schema.
    $sql_query = "DROP SCHEMA $schema_name_quoted CASCADE;";
    $db->query($sql_query);
  }

  /**
   * Returns the size in bytes of a PostgreSQL schema.
   *
   * @code
   *   $schema_name = 'schema';
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   $schema_size = $tripaldbx->getSchemaSize($schema_name);
   * @endcode
   *
   * @param string $schema_name
   *   Schema name.
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or Tripal DBX connection object.
   *   If NULL, current Drupal database is used.
   *
   * @return integer
   *   The size in bytes of the schema or 0 if the size is not available.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\SchemaException
   */
  public function getSchemaSize(
    string $schema_name,
    ?\Drupal\Core\Database\Driver\pgsql\Connection $db = NULL
  ) :int {
    $db = $db ?? \Drupal::database();

    $schema_size = 0;
    $sql_query = "
        SELECT
          SUM(
            pg_total_relation_size(
              quote_ident(schemaname)
              || '.'
              || quote_ident(tablename)
            )
          )::BIGINT AS \"size\"
        FROM pg_tables
        WHERE schemaname = :schema;
      ";
    $size_data = $db
      ->query($sql_query, [':schema' => $schema_name])
      ->fetch();
    if ($size_data) {
      $schema_size = $size_data->size ?: 0;
    }
    return $schema_size;
  }

  /**
   * Returns the size in bytes of a TripalDBX managed database.
   *
   * @code
   *   $tripaldbx = \Drupal::service('tripal.dbx');
   *   $db_size = $tripaldbx->getDatabaseSize();
   * @endcode
   *
   * @param ?\Drupal\Core\Database\Driver\pgsql\Connection $db
   *   A Drupal PostgreSQL or Tripal DBX connection object.
   *
   * @return int
   *   The size in bytes of the database or 0 if the size is not available.
   */
  public function getDatabaseSize(
    ?\Drupal\Core\Database\Connection $db = NULL
  ) :int {
    $db = $db ?? \Drupal::database();
    $db_size = 0;
    $sql_query = '
      SELECT pg_catalog.pg_database_size(d.datname) AS "size"
      FROM pg_catalog.pg_database d
      WHERE d.datname = current_database();
    ';
    $size_data = $db->query($sql_query)->fetch();
    if ($size_data) {
      $db_size = $size_data->size ?: 0;
    }
    return $db_size;
  }

  /**
   * Run an SQL file.
   *
   * @param string $sql_file
   *   Path to an SQL file.
   * @param array $replacements
   *   An array of search-and-replace values used with preg_replace() to replace
   *   placeholders in the SQL file with replacement values. The 'search' values
   *   will be searched and replaced with the 'replace' values.
   *   Default: [] (no replacements).
   * @param ?\Drupal\Core\Database\Connection $db
   *   A connection to the database you want to run the SQL file on.
   */
  public function runSqlFile(
    string $sql_file,
    array $replacements,
    ?\Drupal\Core\Database\Connection $db = NULL
  ) {

    // Get the default database.
    $logger = \Drupal::service('tripal.logger');
    $db = $db ?? \Drupal::database();

    // Retrieve the SQL file.
    $sql = file_get_contents($sql_file);
    if (!$sql) {
      $message = "Run SQL file failed: unable to read '$sql_file' file content.";
      $logger->error($message);
      throw new \Exception($message);
    }

    // Remove starting comments (not the ones in functions).
    $replacements['search'][] = '/^--[^\n]*\n(?:\s*\n)*/m';
    $replacements['replace'][] = '';
    $sql = preg_replace($replacements['search'], $replacements['replace'], $sql);
    $x = $db->query(
      $sql,
      [],
      [
        'allow_delimiter_in_query' => TRUE,
      ]
    );
  }

  /**
   * Turns a table DDL string into a more usable structure.
   *
   * @param string $table_ddl
   *   A string containing table definition as returned by
   *   \Drupal\tripal\TripalDBX\TripalDbxSchema::getTableDdl().
   *
   * @returns array
   *   An associative array with the following structure:
   *   @code
   *   [
   *     'columns' => [
   *       <column name> => [
   *        'type'     => <PostgreSQL column type>,
   *        'not null' => <TRUE if column cannot be NULL, FALSE otherwise>,
   *        'default'  => <column default value or NULL for no default>,
   *       ],
   *       ...
   *     ],
   *     'constraints' => [
   *       <constraint name> => <constraint definition>,
   *       ...
   *     ],
   *     'indexes' => [
   *       <index name> => [
   *         'query' => <index creation query>,
   *         'name'  => <index name>,
   *         'table' => <'table.column' names owning the index>,
   *         'using' => <index type/structure>,
   *       ],
   *       ...
   *     ],
   *     'comment' => <table description>,
   *     'dependencies' => [
   *       <foreign table name> => [
   *         <this table column name> => <foreign table column name>,
   *         ...
   *       ],
   *       ...
   *     ],
   *   ];
   *   @endcode
   */
  public function parseTableDdl(string $table_ddl) :array {
    $table_definition = [
      'columns' => [],
      'constraints' => [],
      'indexes' => [],
      'dependencies' => [],
    ];
    // Note: if we want to process more exotic table creation strings not
    // comming from ::getTableDdl(), we will have to reformat the
    // string first here.
    $table_raw_definition = explode("\n", $table_ddl);

    // Skip "CREATE TABLE" line.
    $i = 1;
    // Loop until end of table definition.
    while (($i < count($table_raw_definition))
        && (!preg_match('/^\s*\)\s*;\s*$/', $table_raw_definition[$i]))
    ) {
      if (empty($table_raw_definition[$i])) {
        ++$i;
        continue;
      }
      if (
          preg_match(
            '/^\s*CONSTRAINT\s*([\w\$\x80-\xFF\.]+)\s+(.+?),?\s*$/',
            $table_raw_definition[$i],
            $match
          )
      ) {
        // Constraint.
        $constraint_name = $match[1];
        $constraint_def = $match[2];
        $table_definition['constraints'][$constraint_name] = $constraint_def;
        if (preg_match(
              '/
                # Match "FOREIGN KEY ("
                FOREIGN\s+KEY\s*\(
                   # Capture current table columns (one or more).
                  (
                    (?:[\w\$\x80-\xFF\.]+\s*,?\s*)+
                  )
                \)\s*
                # Match "REFERENCES"
                REFERENCES\s*
                  # Caputre evental schema name.
                  ([\w\$\x80-\xFF]+\.|)
                  # Caputre foreign table name.
                  ([\w\$\x80-\xFF]+)\s*
                  \(
                    # Capture foreign table columns (one or more).
                    (
                      (?:[\w\$\x80-\xFF]+\s*,?\s*)+
                    )
                  \)
              /ix',
              $constraint_def,
              $match
            )
        ) {
          $table_columns =  preg_split('/\s*,\s*/', $match[1]);
          $foreign_table_schema = $match[2];
          $foreign_table = $match[3];
          $foreign_table_columns =  preg_split('/\s*,\s*/', $match[4]);
          if (count($table_columns) != count($foreign_table_columns)) {
            throw new SchemaException("Failed to parse foreign key definition:\n'$constraint_def'");
          }
          else {
            for ($j = 0; $j < count($table_columns); ++$j) {
              $tcol = $table_columns[$j];
              $ftcol = $foreign_table_columns[$j];
              $table_definition['dependencies'][$foreign_table] =
                $table_definition['dependencies'][$foreign_table]
                ?? [];
              $table_definition['dependencies'][$foreign_table][$tcol] = $ftcol;
            }
          }
        }
      }
      elseif (
        preg_match(
          '/^\s*(\w+)\s+(\w+.*?)(\s+NOT\s+NULL|\s+NULL|)(\s+DEFAULT\s+.+?|),?\s*$/',
          $table_raw_definition[$i],
          $match
        )
      ) {
        // Column.
        $table_definition['columns'][$match[1]] = [
          'type'     => $match[2],
          'not null' => (FALSE !== stripos($match[3], 'NOT')),
          'default'  => ($match[4] === '')
            ? NULL
            : preg_replace('/(?:^\s+DEFAULT\s+)|(?:\s+$)/', '', $match[4])
          ,
        ];
      }
      else {
        // If it happens, it means the tripal_get_table_ddl() SQL function
        // changed and this script should be adapted.
        throw new SchemaException(
          'Failed to parse unexpected table definition line format for "'
          . $table_raw_definition[0]
          . '": "'
          . $table_raw_definition[$i]
          . '"'
        );
      }
      ++$i;
    }

    // Parses the rest (indexes and comment).
    if (++$i < count($table_raw_definition)) {
      while ($i < count($table_raw_definition)) {
        if (empty($table_raw_definition[$i])) {
          ++$i;
          continue;
        }
        // Parse index name for later comparison.
        if (preg_match(
              '/
                ^\s*
                CREATE\s+
                (?:UNIQUE\s+)?INDEX\s+(?:CONCURRENTLY\s+)?
                (?:IF\s+NOT\s+EXISTS\s+)?
                # Capture index name.
                ([\w\$\x80-\xFF\.]+)\s+
                # Capture table column.
                ON\s+([\w\$\x80-\xFF\."]+)\s+
                # Capture index structure.
                USING\s+(.+);\s*
                $
              /ix',
              $table_raw_definition[$i],
              $match
            )
        ) {
          // Constraint.
          $table_definition['indexes'][$match[1]] = [
            'query' => trim($match[0]),
            'name'  => $match[1],
            'table'  => $match[2],
            'using' => $match[3],
          ];
        }
        elseif (
          preg_match(
            '/^\s*COMMENT\s+ON\s+TABLE\s+\S+\s+IS\s+\'((?:[^\'\\\\]|\\\\.|\'\')*)(\'\s*;\s*|)$/i',
            $table_raw_definition[$i],
            $match
          )
        ) {
          $table_definition['comment'] = $match[1];
          // Complete the comment if needed (multiline comments).
          while (empty($match[2]) && ($i+1 < count($table_raw_definition))) {
            ++$i;
            preg_match(
              '/^((?:[^\'\\\\]|\\\\.)*)(\'\s*;\s*|)/i',
              $table_raw_definition[$i],
              $match
            );
            $table_definition['comment'] .= "\n" . $match[1];
          }
        }
        else {
          // If it happens, it means the tripal_get_table_ddl() SQL function
          // changed and this script should be adapted.
          throw new SchemaException(
            'Failed to parse unexpected table DDL line format for "'
            . $table_raw_definition[0]
            . '": "'
            . $table_raw_definition[$i]
            . '"'
          );
        }
        ++$i;
      }
    }
    return $table_definition;
  }

  /**
   * Parses a table DDL and returns a Drupal schema definition.
   *
   * An exception is thrown if the table is not found.
   *
   * @param string $table_ddl
   *   A string containing table definition as returned by
   *   \Drupal\tripal\TripalDBX\TripalDbxSchema::getTableDdl().
   *
   * @return array
   *   An array with details of the table reflecting what is in database.
   *
   * @see https://www.drupal.org/docs/7/api/schema-api/data-types/data-types-overview
   * @see https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!database.api.php/group/schemaapi/9.3.x
   */
  public function parseTableDdlToDrupal(string $table_ddl) :array {
    $tripaldbx = \Drupal::service('tripal.dbx');
    $table_structure = $tripaldbx->parseTableDdl($table_ddl);
    // Start with the name of the table.
    $table_def = [];

    // Description.
    if (!empty($table_structure['comment'])) {
      $table_def['description'] = $table_structure['comment'];
    }

    // Columns/fields:
    $table_def['fields'] = [];
    foreach ($table_structure['columns'] as $column => $column_def) {
      $column_def['type'] = trim($column_def['type']);
      $size = NULL;
      $is_int = FALSE;
      $is_float = FALSE;
      // Check for serial.
      if (isset($column_def['default'])
          && preg_match('/^nextval\(/i', $column_def['default'])
      ) {
        unset($column_def['default']);
        if ($column_def['type'] == 'bigint') {
          $column_def['type'] = 'bigserial';
          $size = 'big';
        }
        elseif ($column_def['type'] == 'smallint') {
          $column_def['type'] = 'smallserial';
          $size = 'small';
        }
        else {
          $column_def['type'] = 'serial';
        }
      }
      // Check specified string length or precision and extract it.
      $length = $precision = $scale = NULL;
      if (preg_match('/\(\s*(\d+)\s*\)$/i', $column_def['type'], $match)) {
        $length = intval($match[1]);
        $column_def['type'] = substr(
          $column_def['type'],
          0,
          strpos($column_def['type'], '(')
        );
      }
      elseif (
        preg_match(
          '/\(\s*(\d+)\s*,\s*(\d+)\s*\)$/i',
          $column_def['type'],
          $match
        )
      ) {
        $length = intval($match[1]);
        $scale =  intval($match[2]);
        $column_def['type'] = substr(
          $column_def['type'],
          0,
          strpos($column_def['type'], '(')
        );
      }
      // Remove extra stuff from type name.
      $short_type = $column_def['type'];
      $i = strpos($short_type, ' ');
      if (FALSE !== $i) {
        $short_type = substr($short_type, 0, $i);
      }

      // Remap types if needed.
      // Supported types are :
      // 'char', 'varchar', 'text', 'blob', 'int', 'float', 'numeric', 'serial'.
      $pg_type = NULL;
      switch ($short_type) {
        case 'bigint':
          $column_type = 'int';
          $pg_type = $column_def['type'];
          $size = 'big';
          $is_int = TRUE;
          break;

        case 'int':
        case 'integer':
          $column_type = 'int';
          $pg_type = $column_def['type'];
          $size = 'medium';
          $is_int = TRUE;
          break;

        case 'smallint':
          $column_type = 'int';
          $pg_type = $column_def['type'];
          $size = 'small';
          $is_int = TRUE;
          break;

        case 'boolean':
          $column_type = 'text';
          $pg_type = $column_def['type'];
          break;

        case 'decimal':
          $column_type = 'float';
          $pg_type = $column_def['type'];
          $precision = $length;
          $length = NULL;
          $is_float = TRUE;
          break;

        case 'real':
          $column_type = 'float';
          $pg_type = $column_def['type'];
          $is_float = TRUE;
          break;

        case 'double':
          $column_type = 'float';
          $pg_type = $column_def['type'];
          $size = 'big';
          $is_float = TRUE;
          break;

        case 'bigserial':
          $column_type = 'serial';
          $pg_type = $column_def['type'];
          $size = 'big';
          $is_int = TRUE;
          break;

        case 'serial':
          $column_type = 'serial';
          $pg_type = $column_def['type'];
          $size = 'medium';
          $is_int = TRUE;
          break;

        case 'smallserial':
          $column_type = 'serial';
          $pg_type = $column_def['type'];
          $size = 'small';
          $is_int = TRUE;
          break;

        case 'character':
          if (FALSE !== stripos($column_def['type'], 'var')) {
            $column_type = 'varchar';
          }
          else {
            $column_type = 'char';
          }
          $pg_type = $column_def['type'];
          break;

        case 'bytea':
          $column_type = 'blob';
          $pg_type = $column_def['type'];
          break;

        default:
          // Date/time, money, enumerated, geometric, network address, etc.
          $column_type = 'text';
          $pg_type = $column_def['type'];
          break;
      }

      $table_def['fields'][$column] = [
        'type' => $column_type,
        'not null' => $column_def['not null'],
      ];
      if (!empty($pg_type)) {
        $table_def['fields'][$column]['pgsql_type'] = $pg_type;
      }
      if (isset($length)) {
        $table_def['fields'][$column]['length'] = $length;
      }
      if (isset($size)) {
        $table_def['fields'][$column]['size'] = $size;
      }
      if (isset($precision)) {
        $table_def['fields'][$column]['precision'] = $precision;
      }
      if (isset($scale)) {
        $table_def['fields'][$column]['scale'] = $scale;
      }
      if (isset($column_def['default'])) {
        if ($is_int) {
          $table_def['fields'][$column]['default'] = (int) $column_def['default'];
        }
        elseif ($is_float) {
          $table_def['fields'][$column]['default'] = (float) $column_def['default'];
        }
        else {
          $table_def['fields'][$column]['default'] = $column_def['default'];
        }
      }
    }

    // Constraints.
    foreach ($table_structure['constraints'] as $constraint => $cdef) {
      $cdef = trim($cdef);
      if (preg_match('/^PRIMARY\s+KEY\s+\((.+)\)/i', $cdef, $match)) {
        $table_def['primary key'] = preg_split('/\s*,\s*/', $match[1]);
      }
      elseif (preg_match('/^UNIQUE\s+\((.+)\)/i', $cdef, $match)) {
        if (!array_key_exists('unique keys', $table_def)) {
          $table_def['unique keys'] = [];
        }
        $table_def['unique keys'][$constraint] =
          preg_split('/\s*,\s*/', $match[1]);
      }
      elseif (preg_match(
        '/
          # Match "FOREIGN KEY ("
          FOREIGN\s+KEY\s*\(
             # Capture current table columns (one or more).
            (
              (?:[\w\$\x80-\xFF\.]+\s*,?\s*)+
            )
          \)\s*
          # Match "REFERENCES"
          REFERENCES\s*
            # Caputre evental schema name.
            ([\w\$\x80-\xFF]+\.|)
            # Caputre foreign table name.
            ([\w\$\x80-\xFF]+)\s*
            \(
              # Capture foreign table columns (one or more).
              (
                (?:[\w\$\x80-\xFF]+\s*,?\s*)+
              )
            \)
        /ix',
        $cdef,
        $match
      )) {
        if (!array_key_exists('foreign keys', $table_def)) {
          $table_def['foreign keys'] = [];
        }
        $table_columns =  preg_split('/\s*,\s*/', $match[1]);
        $foreign_table_schema = $match[2];
        $foreign_table = $match[3];
        $foreign_table_columns =  preg_split('/\s*,\s*/', $match[4]);
        $table_def['foreign keys'][$constraint] = [
          'table' => $foreign_table,
          'columns' => [],
        ];
        if (count($table_columns) == count($foreign_table_columns)) {
          for ($col = 0; $col < count($table_columns); ++$col) {
            $tcol = $table_columns[$col];
            $ftcol = $foreign_table_columns[$col];
            $table_def['foreign keys'][$constraint]['columns'][$tcol] = $ftcol;
          }
        }
      }
    }
    return $table_def;
  }
}
