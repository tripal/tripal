<?php

namespace Drupal\tripal_biodb\Database;

use Drupal\Core\Database\Driver\pgsql\Connection as PgConnection;
use Drupal\tripal_biodb\Database\BioSchema;
use Drupal\tripal_biodb\Exception\ConnectionException;

/**
 * Biological database connection API class.
 *
 * This class provides a Tripal-specific extension of the Drupal database
 * connection abstraction class. It extends the base class with specific
 * functions dedicated to manage a biological schema aside of the Drupal schema.
 * It has been designed mostly based on Chado schema and PostgreSQL features
 * allowing to have several schemas in a same database.
 *
 * Here are some useful inherited methods to know:
 *
 * - BioConnection::select(), insert(), update(), delete(), truncate(),
 *   upsert(), prepare(), startTransaction(), commit(), rollBack(), quote(),
 *   quoteIdentifiers(), escape*() methods, query*() methods, and more from
 *   \Drupal\Core\Database\Connection.
 *
 * - BioConnection::schema() that provides a \Drupal\Core\Database\Schema object
 *   and offers, beside others, the follwing methods: addIndex(),
 *   addPrimaryKey(), addUniqueKey(), createTable(), dropField(), dropIndex(),
 *   dropPrimaryKey(), dropTable(), dropUniqueKey(), fieldExists(),
 *   findPrimaryKeyColumns(), findTables(), indexExists(), renameTable(),
 *   tableExists() and more from the documentation.
 *
 * A couple of methods have been added to this class to complete the above list.
 *
 * Note: the setLogger() and getLogger() methods are reserved for database query
 * logging and is operated by Drupal. It works with a \Drupal\Core\Database\Log
 * class. To log messages in extending classes, use setMessageLogger() and
 * getMessageLogger() instead, which operates with the \Psr\Log\LoggerInterface
 * class. By default, the message logger is set by the constructor either using
 * the user-provided logger or by instanciating one using the log channel
 * 'tripal_biodb.logger'.
 *
 *
 * @see https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Connection.php/class/Connection/9.0.x
 * @see https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Connection.php/class/Connection/9.0.x
 */
abstract class BioConnection extends PgConnection {

  /**
   * {@inheritdoc}
   */
  protected $identifierQuotes = [
    '"',
    '"',
  ];

  /**
   * Class lineage to use when checking who called a method.
   *
   * @var array
   */
  protected $self_classes = [
    \Drupal\Core\Database\Connection::class => TRUE,
    \Drupal\Core\Database\Driver\pgsql\Connection::class => TRUE,
    \Drupal\tripal_biodb\Database\BioConnection::class => TRUE,
  ];

  /**
   * Database connection.
   *
   * @var \Drupal\Core\Database\Driver\pgsql\Connection
   */
  protected $database = NULL;

  /**
   * Schema database name.
   *
   * @var string
   */
  protected $databaseName = '';

  /**
   * Drupal settings database key.
   *
   * @var string
   */
  protected $dbKey = '';


  /**
   * The name of the biological schema used by this instance.
   *
   * @var string
   */
  protected $schemaName = '';

  /**
   * The PostgreSQL quoted name of the biological schema used by this instance.
   *
   * @var string
   */
  protected $quotedSchemaName = '';

  /**
   * An ordered list of extra schema that can be used.
   *
   * @var array
   */
  protected $extraSchemas = [];

  /**
   * The version for current biological schema instance.
   *
   * @var ?string
   */
  protected $version = NULL;

  /**
   * Logger.
   *
   * @var object \Psr\Log\LoggerInterface
   */
  protected $messageLogger = NULL;

  /**
   * BioDbTool tool.
   *
   * @var object \Drupal\tripal_biodb\Database\BioDbTool
   */
  protected $bioTool = NULL;

  /**
   * List of objects that will use biological schema as default.
   *
   * @var array
   */
  protected $objectsUsingBioDb = [];

  /**
   * List of classes that will use biological schema as default.
   *
   * @var array
   */
  protected $classesUsingBioDb = [];

  /**
   * Returns the version number of the given biological schema.
   *
   * @param ?string $schema_name
   *   A schema name or NULL to work on current schema.
   * @param bool $exact_version
   *   Returns the most precise version available. Default: FALSE.
   *
   * @return string
   *   The version in a simple format like '1.0', '2.3x' or '4.5+' or '0' if the
   *   version cannot be guessed but an instance of the biological schema has
   *   been detected or an empty string if the schema does not appear to be an
   *   instance of the biological schema. If $exact_version is FALSE , the
   *   returned version must always starts by a number and can be tested against
   *   numeric values (ie. ">= 1.2"). If $exact_version is TRUE, the format is
   *   free and can start by a letter and hold several dots like 'v1.2.3 alpha'.
   */
  abstract public function findVersion(
    ?string $schema_name = NULL,
    bool $exact_version = FALSE
  ) :string;

  /**
   * Get the list of available "bio-databases" instances in current database.
   *
   * This function returns both PostgreSQL schemas integrated with Tripal
   * and free schemas.
   *
   * @return array
   *   An array of available schema keyed by schema name and having the
   *   following structure:
   *   "schema_name": name of the schema (same as the key);
   *   "version": detected version of the biological schema;
   *   "is_test": TRUE if it is a test schema and FALSE otherwise;
   *   "has_data": TRUE if the schema contains more than just default records;
   *   "size": size of the schema in bytes;
   *   "is_integrated": FALSE if not integrated with Tripal and an array
   *     otherwise with the following fields: 'install_id', 'schema_name',
   *     'version', 'created', 'updated'.
   */
  abstract public function getAvailableInstances() :array;

  /**
   * Opens a new connection using the same settings as the provided one.
   *
   * Drupal Database class only opens new connection to a database when it is
   * "necessary", which means when a connection to the database is not opened
   * already. However, in the context of a BioConnection, we need a different
   * database context for each connection since the search_path may be changed.
   * To not mess up with Drupal stuff, we need to open a new and distinct
   * database connection for each BioConnection instance.
   *
   * @param \Drupal\Core\Database\Driver\pgsql\Connection $database
   *   The database connection to duplicate.
   *
   * @return \PDO
   *   A \PDO object.
   */
  protected static function openNewPdoConnection(
    \Drupal\Core\Database\Driver\pgsql\Connection $database
  ) {
    // We call this method in a context of an existing connection already
    // used by Drupal so we can avoid a couple of tests and assume it works.
    $database_info = \Drupal\Core\Database\Database::getAllConnectionInfo();
    $target = $database->target;
    $key = $database->key;

    $pdo_connection = \Drupal\Core\Database\Driver\pgsql\Connection::open(
      $database_info[$key][$target]
    );
    return $pdo_connection;
  }

  /**
   * Constructor for a biological database connection.
   *
   * @param string $schema_name
   *   The biological schema name to use.
   *   Default: '' (no schema). It will throw exceptions on methods needing a
   *   default schema but may work on others or when a schema can be passed
   *   as parameter.
   * @param \Drupal\Core\Database\Driver\pgsql\Connection|string $database
   *   Either a \Drupal\Core\Database\Driver\pgsql\Connection instance or a
   *   Drupal database key string (from current site's settings.php).
   *   Extra databases specified in settings.php do not need to specify a
   *   schema name as a database prefix parameter. The prefix will be managed by
   *   this connection class instance.
   * @param ?\Psr\Log\LoggerInterface $logger
   *   A logger in case of operations to log.
   *
   * @throws \Drupal\tripal_biodb\Exception\ConnectionException
   * @throws \Drupal\Core\Database\ConnectionNotDefinedException
   *
   * @see https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Database.php/function/Database%3A%3AgetConnection/9.0.x
   * @see https://api.drupal.org/api/drupal/sites%21default%21default.settings.php/9.0.x
   * @see https://www.drupal.org/docs/8/api/database-api/database-configuration
   */
  public function __construct(
    string $schema_name = '',
    $database = 'default',
    ?\Psr\Log\LoggerInterface $logger = NULL
  ) {
    // Check a key was provided instead of a connection object.
    if (is_string($database)) {
      $database = trim($database);
      if ('' == $database) {
        $database = 'default';
      }
      // Get the corresponding connection object.
      $this->dbKey = $database;
      $database = \Drupal\Core\Database\Database::getConnection(
        'default',
        $database
      );
    }
    // Make sure we are using a PostgreSQL connection.
    if (!is_a($database, \Drupal\Core\Database\Driver\pgsql\Connection::class)
    ) {
      throw new ConnectionException(
        "The provided connection object is not a PostgreSQL database connection."
      );
    }

    // Get a BioDbTool object.
    $this->bioTool = \Drupal::service('tripal_biodb.tool');

    // Get option array.
    $connection_options = $database->getConnectionOptions();
    $this->databaseName = $connection_options['database'];
    // Check if a biological schema name has been specified.
    if (!empty($schema_name)) {
      // We must use this PostgreSQL schema instead of the Drupal one as default
      // for this dtaabase connection. To do so, we use the schema name as a
      // prefix (which is supported by Drupal PostgreSQL implementation).
      // If there are table-specific prefixes set, we assume the user knows what
      // he/she wants to do and we won't change those.
      // Note: if the schema name is not valid, an exception will be thrown by
      // setSchemaName() at the end of this constructor.
      if (empty($connection_options['prefix'])) {
        $connection_options['prefix'] = ['1' => $schema_name . '.'];
      }
      elseif (is_array($connection_options['prefix'])) {
        $connection_options['prefix']['1'] = $schema_name . '.';
      }
      else {
        // $this->prefixes is a string.
        $connection_options['prefix'] = [
          'default' => $connection_options['prefix'],
          '1' => $schema_name . '.',
        ];
      }
      // Add search_path to avoid the use of Drupal schema by mistake.
      // Get biological schema name first.
      $sql =
        "SELECT quote_ident("
        . $database->connection->quote($schema_name)
        . ") AS \"qi\";"
      ;
      $quoted_schema_name = $database->connection
        ->query($sql)
        ->fetch(\PDO::FETCH_OBJ)
        ->qi ?: $schema_name
      ;
      // Then, get Drupal schema.
      $drupal_schema = $this->bioTool->getDrupalSchemaName();
      $connection_options['init_commands']['search_path'] =
        'SET search_path='
        . $quoted_schema_name
        . ','
        . $drupal_schema
      ;
    }

    // Get a new connection distinct from Drupal's to avoid search_path issues.
    $connection = static::openNewPdoConnection($database);
    $this->setTarget($database->target);
    $this->setKey($database->key);

    // Call parent constructor to initialize stuff well.
    parent::__construct($connection, $connection_options);

    // Logger.
    if (!isset($logger)) {
      // We need a logger.
      $logger = \Drupal::service('tripal_biodb.logger');
    }
    $this->messageLogger = $logger;

    // Set schema name after parent intialisation in order to setup schema
    // prefix appropriately.
    $this->setSchemaName($schema_name);

    // Register Schema class to use biological schema as default.
    // $this->useBioSchemaFor(PgConnection::class);
    // $this->useBioSchemaFor(\Drupal\Core\Database\Connection::class);
    $this->useBioSchemaFor(\Drupal\Core\Database\Schema::class);
    $this->useBioSchemaFor(\Drupal\Core\Database\Driver\pgsql\Schema::class);
    $this->useBioSchemaFor(\Drupal\tripal_biodb\Database\BioSchema::class);
  }

  /**
   * Returns current database name.
   *
   * @return string
   *  Current schema name.
   */
  public function getDatabaseName() :string {
    return $this->databaseName;
  }

  /**
   * Returns current database key in Drupal settings if one.
   *
   * @return string
   *  Database key in Drupal settings or an empty string if none.
   */
  public function getDatabaseKey() :string {
    return $this->dbKey;
  }

  /**
   * Returns current message logger.
   *
   * @return \Psr\Log\LoggerInterface
   *  A message logger.
   */
  public function getMessageLogger() :\Psr\Log\LoggerInterface {
    return $this->messageLogger;
  }

  /**
   * Sets current message logger.
   *
   * @param \Psr\Log\LoggerInterface $logger
   *  A message logger.
   */
  public function setMessageLogger(\Psr\Log\LoggerInterface $logger) :void {
    $this->messageLogger = $logger;
  }

  /**
   * (override) Returns a Schema object for manipulating the schema.
   *
   * This method overrides the parent one in order to force the use of the
   * \Drupal\tripal_biodb\Database\BioSchema class and manage biological schema
   * changes for this connection. The Schema object is updated on changes.
   *
   * @return \Drupal\Core\Database\Schema
   *   The database Schema object for this connection.
   */
  public function schema() {
    if (empty($this->schema)) {
      $class = $this->getBioClass('Schema');
      $this->schema = new $class($this);
    }
    return $this->schema;
  }

  /**
   * Sets current biological schema name.
   *
   * This method will resets any class member afected by a schema change such as
   * schema version and extra schemas for instance.
   * "No schema name" might be specified using an empty string but calling
   * methods that requires a schema to work on will through errors.
   * The given schema name format will be check and a ConnectionException must
   * be thrown by implementations if the name is incorrect (note: an empty name
   * is allowed).
   *
   * @param string $schema_name
   *   The biological schema name to use.
   *
   * @throws \Drupal\tripal_biodb\Exception\ConnectionException
   */
  public function setSchemaName(string $schema_name) :void {
    // Does schema change?
    if (!empty($this->schemaName) && ($schema_name == $this->schemaName)) {
      return;
    }

    // Check name is valid.
    if (!empty($schema_name)
        && ($issue = $this->bioTool->isInvalidSchemaName($schema_name, TRUE))
    ) {
      throw new ConnectionException(
        "Could not use the schema name '$schema_name'.\n$issue"
      );
    }
    // Resets some members.
    $this->version = NULL;
    $this->extraSchemas = [];
    $this->schema = NULL;
    $this->quotedSchemaName = '';

    // Update schema prefixes.
    $bioschema_prefix = empty($schema_name) ? '' : $schema_name . '.';
    if (empty($this->prefixes)) {
      $this->prefixes = ['1' => $bioschema_prefix];
    }
    elseif (is_array($this->prefixes)) {
      $this->prefixes['1'] = $bioschema_prefix;
    }
    else {
      // $this->prefixes is a string.
      $this->prefixes = [
        'default' => $this->prefixes,
        '1' => $bioschema_prefix,
      ];
    }
    $this->setPrefix($this->prefixes);

    // Update search_path.
    if (!empty($schema_name)) {
      $quoted_schema_name = $this->bioTool->quoteDbObjectId($schema_name, $this);
      $this->quotedSchemaName = $quoted_schema_name ?? $schema_name;
      $drupal_schema = $this->bioTool->getDrupalSchemaName();
      $search_path =
        $this->connectionOptions['init_commands']['search_path'] =
        'SET search_path=' . $quoted_schema_name . ',' . $drupal_schema;
      $this->connection->exec($search_path);
    }

    $this->schemaName = $schema_name;
  }

  /**
   * Returns current biological schema name.
   *
   * @return string
   *   Current biological schema name or an empty string if not set.
   */
  public function getSchemaName() :string {
    return $this->schemaName;
  }

  /**
   * Returns current biological schema name quoted for PostgreSQL queries.
   *
   * This getter should rarely be used (and in very specific cases).
   * If the schema name does not contain any special characters, it might not
   * require any quote and $quotedSchemaName will be the same as $schemaName.
   * The quoted schema name is only needed when writing special SQL queries that
   * need to qualify database objects with a schema name. The quoted schema name
   * must not be used as a field value in SQL queries.
   * For instance, the quoted schema name will be used to prefix a function:
   * @code
   *   $sql = 'SELECT ' . $quotedSchemaName . '.some_sql_function();';
   * @endcode
   * but it MUST NOT be used in these kinds of situation:
   * @code
   *   // This is WRONG:
   *   $sql = 'SELECT * FROM pg_tables WHERE schemaname = ' . $quotedSchemaName;
   * @endcode
   *
   * @return string
   *   Current biological schema name  quoted by PostgreSQL if necessary or an
   *   empty string if not set.
   */
  public function getQuotedSchemaName() :string {
    return $this->quotedSchemaName;
  }

  /**
   * Returns either the user provided schema name or current schema name.
   *
   * Helper function.
   * If $schema_name is not empty, its name will be checked and returned,
   * otherwise, the default schema name will be returned if set. If none of
   * those are available, an error is thrown.
   *
   * @param ?string $schema_name
   *   A user-provided schema name.
   * @param string $error_message
   *   An error message to throw if none of $schema_name and $this->schemaName
   *   are set. Default: 'Invalid schema name.'
   *
   * @return string
   *   $schema_name if set and valid, or the current schema name.
   *
   * @throws \Drupal\tripal_biodb\Exception\ConnectionException
   *  If the given schema name is invalid (ignoring schema name reservations)
   *  or none of $schema_name and $this->schemaName are set.
   */
  protected function getDefaultSchemaName(
    ?string $schema_name = NULL,
    string $error_message = ''
  ) :string {
    if (empty($error_message)) {
      $error_message =
        'Called '
        . debug_backtrace()[1]['function']
        . ' without a schema name.';
    }
    if (empty($schema_name)) {
      if (empty($this->schemaName)) {
        throw new ConnectionException($error_message);
      }
      $schema_name = $this->schemaName;
    }
    else {
      if ($issue = $this->bioTool->isInvalidSchemaName($schema_name, TRUE)) {
        throw new ConnectionException($issue);
      }
    }
    return $schema_name;
  }

  /**
   * Returns the ordered list of extra schema currently in use.
   *
   * @return array
   *   An ordered list of schema names.
   *   Note: returned schemas array starts from 2 as 0 and 1 indices are
   *   reserved (respectively) to Drupal schema and current schema.
   */
  public function getExtraSchemas() :array {
    return $this->extraSchemas;
  }

  /**
   * Clears the extra schemas list.
   */
  public function clearExtraSchemas() :void {
    $this->extraSchemas = [];
    $this->setPrefix($this->prefixes);
  }

  /**
   * Adds an extra schema to the list and returns its query index.
   *
   * @param string $schema_name
   *   A user-provided schema name from current database. There must be a
   *   non-empty "current schema" set by ::setSchemaName before adding an
   *   extra-schema.
   *
   * @return int
   *   The extra schema index that can be used in database queries in curly
   *   braces using the syntax '{index:table_name}' (where 'index' should be
   *   replaced by the returned integer and table_name should be an actual table
   *   name).
   *
   * @throws \Drupal\tripal_biodb\Exception\ConnectionException
   *   If the given schema name is invalid or does not exist in current
   *   database or there is no current schema.
   */
  public function addExtraSchema(string $schema_name) :int {
    if (empty($this->schemaName)) {
      throw new ConnectionException(
        "Cannot add an extra schema. No current schema (specified by ::setSchemaName)."
      );
    }
    // Check provided name.
    if ($issue = $this->bioTool->isInvalidSchemaName($schema_name, TRUE)) {
      throw new ConnectionException($issue);
    }
    // We reserve index 0 for Drupal schema and index 1 for current schema.
    if (empty($this->extraSchemas)) {
      // We restart at 2.
      $this->extraSchemas[2] = $schema_name;
    }
    else {
      $this->extraSchemas[] = $schema_name;
    }
    $this->setPrefix($this->prefixes);
    return array_key_last($this->extraSchemas);
  }

  /**
   * Adds an extra schema to the list and returns its query index.
   *
   * @param string $schema_name
   *   A user-provided schema name from current database. There must be a
   *   non-empty "current schema" set by ::setSchemaName before adding an
   *   extra-schema.
   * @param int $index
   *   The index of the extra schema. Note that '0' is reserved for Drupal
   *   schema and 1 for current schema. The first available extra schema index
   *   is therefore 2. Using higher values means any lower value has an
   *   associated schema set already.
   *   Default: 2.
   *
   * @throws \Drupal\tripal_biodb\Exception\ConnectionException
   *   If the given schema name is invalid or does not exist in current
   *   database or there is no current schema or a lower index has not
   *   associated schema or the index is invalid.
   */
  public function setExtraSchema(string $schema_name, int $index = 2) :void {
    if (2 > $index) {
      throw new ConnectionException(
        "Invalid extra schema index '$index'."
      );
    }
    elseif (max(array_key_last($this->extraSchemas)+1, 2) < $index) {
      throw new ConnectionException(
        "Invalid extra schema index '$index'. Intermediate schemas are missing."
      );
    }
    if (empty($this->schemaName)) {
      throw new ConnectionException(
        "Cannot add an extra schema. No current schema (specified by ::setSchemaName)."
      );
    }
    // Check provided name.
    if ($issue = $this->bioTool->isInvalidSchemaName($schema_name, TRUE)) {
      throw new ConnectionException($issue);
    }
    $this->extraSchemas[$index] = $schema_name;
    $this->setPrefix($this->prefixes);
  }

  /**
   * Get current schema version.
   *
   * Note: do not confuse this method with the inherited ::version() method that
   * returns the version of the database server.
   *
   * @return string
   *   A schema version or an empty string, just like findVersion.
   *
   * @see ::findVersion
   */
  public function getVersion() :string {

    if ((NULL === $this->version) && !empty($this->schemaName)) {
      // Get the version of the schema.
      $this->version = $this->findVersion();
    }

    return $this->version ?? '';
  }

  /**
   * Use the biological schema as default for the given things.
   *
   * Register an object or a class to make them use the biological schema as
   * default in any method of this instance of BioConnection.
   *
   * @param string|object
   *   Object or class to register.
   */
  public function useBioSchemaFor($object_or_class) {
    if (is_string($object_or_class)) {
      // Class.
      $this->classesUsingBioDb[$object_or_class] = $object_or_class;
    }
    else {
      // Object.
      $this->objectsUsingBioDb[] = $object_or_class;
    }
  }

  /**
   * Remove the given things from the lists using biological schema as default.
   *
   * @param string|object
   *   Object or class to unregister.
   */
  public function useDrupalSchemaFor($object_or_class) {
    if (is_string($object_or_class)) {
      // Remove class for the list.
      unset($this->classesUsingBioDb[$object_or_class]);
    }
    else {
      // Remove object from the list.
      $this->objectsUsingBioDb = array_filter(
        $this->objectsUsingBioDb,
        function ($o) { return $o != $object_or_class; }
      );
    }
  }

  /**
   * Gets the biological database-specific class for the specified category.
   *
   * Returns the biological database-specific override class if any for the
   * specified class category.
   *
   * @param string $class
   *   The class category for which we want the specific class.
   *
   * @return string
   *   The name of the class that should be used.
   */
  public function getBioClass($class) :string {
    static $classes = [
      'Schema' => BioSchema::class,
    ];
    if (!array_key_exists($class, $classes)) {
      throw new ConnectionException("Invalid BioDb class '$class'.");
    }
    return $classes[$class];
  }

  /**
   * (override) Sets the list of prefixes used by this database connection.
   *
   * Overrides parent method in order to manage multiple schema queries.
   *
   * In static Drupal SQL queries, table names must be wrapped in curly braces
   * (https://www.drupal.org/docs/drupal-apis/database-api/static-queries).
   * This allows Drupal to use table prefixes as specified in settings.php file.
   * When working with PostgreSQL and multiple schemas, each table should be
   * prefixed by its schema to avoid conflicting names (in wich case the
   * search_path order is not enought). Since we may work with several
   * biological "databases" stored in different schemas and we might need to
   * cross-query them, we introduce here a new table name denotation that
   * enables the use of multiple schemas in a same static query without
   * conflicts. This new denotation is backward compatible with Drupal's one.
   *
   * Table schemas can be selected using a number followed by a colon, just
   * after the opening curly brace. For instance, we have 2 biological schemas
   * named "chado_main" and "chado_other". We will refer them in the $prefix
   * array as $prefix['1'] = 'chado_main.' and $prefix['2'] = 'chado_other.'.
   * Then, for instance, if we want to write a Drupal static query that searches
   * all feature table entries that are in "chado_other" but not in
   * "chado_main", we would write somthing similar to:
   * @code
   * $sql_query = "
   *   SELECT f2.*
   *   FROM {2:feature} f2
   *   WHERE NOT EXISTS (
   *     SELECT TRUE FROM {1:feature} f1 WHERE f1.uniquename = f2.uniquename
   *   );";
   * @endcode
   * When no number is specified, we assume the 'default' table prefix will be
   * used. The '0' prefix is reserved to Drupal schema and could be used in
   * queries when Drupal table are used but it's optional.
   *
   * @param array|string $prefix
   *   Either a single prefix, or an array of prefixes.
   */
  protected function setPrefix($prefix) {
    if (is_array($prefix)) {
      $this->prefixes = $prefix + [
        'default' => '',
      ];
    }
    else {
      $this->prefixes = [
        'default' => $prefix,
      ];
    }
    [
      $start_quote,
      $end_quote,
    ] = $this->identifierQuotes;

    // Set up variables for use in prefixTables(). Replace table-specific
    // prefixes first.
    $this->prefixSearch = [];
    $this->prefixReplace = [];
    foreach ($this->prefixes as $key => $val) {
      if (!preg_match('/^(?:default|\d+)$/', $key)) {
        $this->prefixSearch[] = '{' . $key . '}';

        // $val can point to another database like 'database.users'. In this
        // instance we need to quote the identifiers correctly.
        $val = str_replace('.', $end_quote . '.' . $start_quote, $val);
        $this->prefixReplace[] = $start_quote . $val . $key . $end_quote;
      }
    }

    // Then replace schema prefixes (specied in settings).
    $i = 1;
    while (array_key_exists("$i", $this->prefixes)) {
      $this->prefixSearch[] = '{' . $i . ':';
      $this->prefixReplace[] =
        $start_quote
        . str_replace(
          '.',
          $end_quote . '.' . $start_quote,
          $this->prefixes[$i]
        );
      ++$i;
    }

    // Then replace tables in default Drupal schema.
    $this->prefixSearch[] = '{0:';

    // $this->prefixes['default'] can point to another database like
    // 'other_db.'. In this instance we need to quote the identifiers correctly.
    // For example, "other_db"."PREFIX_table_name".
    $this->prefixReplace[] =
      $start_quote
      . str_replace(
        '.',
        $end_quote . '.' . $start_quote,
        $this->prefixes['default']
      )
    ;

    if (!empty($this->schemaName)) {
      $this->prefixSearch[] = '{1:';

      $this->prefixReplace[] =
        $start_quote
        . $this->schemaName
        . $end_quote
        . '.'
        . $start_quote
      ;

      // Then replace tables in biological schemas.
      for ($i = 2; $i <= array_key_last($this->extraSchemas); ++$i) {
        $this->prefixSearch[] = '{' . $i . ':';
        $this->prefixReplace[] =
          $start_quote
          . $this->extraSchemas[$i]
          . $end_quote
          . '.'
          . $start_quote
        ;
      }
    }

    // Then replace remaining tables with the default prefix.
    $this->prefixSearch[] = '{';

    // $this->prefixes['default'] can point to another database like
    // 'other_db.'. In this instance we need to quote the identifiers correctly.
    // For example, "other_db"."PREFIX_table_name".
    $this->prefixReplace[] =
      $start_quote
      . str_replace(
        '.',
        $end_quote . '.' . $start_quote,
        $this->prefixes['default']
      )
    ;
    $this->prefixSearch[] = '}';
    $this->prefixReplace[] = $end_quote;

    // Set up a map of prefixed => un-prefixed tables.
    foreach ($this->prefixes as $table_name => $prefix) {
      if (!preg_match('/^(?:default|\d+)$/', $key)) {
        $this->unprefixedTablesMap[$prefix . $table_name] = $table_name;
      }
    }
  }

  /**
   * Tells if the caller assumes current schema is the biological schema.
   *
   * @return bool
   *   TRUE if default schema is not Drupal's but the biological one.
   */
  protected function shouldUseBioSchema() :bool {
    $should = FALSE;
    // We start at 2 because this protected method can only be called at level 1
    // from a local class method so we can skip level 1.
    $bt_level = 2;
    $backtrace = debug_backtrace();
    $calling_class = $backtrace[$bt_level]['class'] ?? '';
    $calling_object = $backtrace[$bt_level]['object'] ?? FALSE;
    // Look outside this class.
    while (isset($this->self_classes[$calling_class])
        && ($bt_level < count($backtrace))
    ) {
      ++$bt_level;
      $calling_class = $backtrace[$bt_level]['class'] ?? '';
      $calling_object = $backtrace[$bt_level]['object'] ?? FALSE;
    }
    if (!empty($this->classesUsingBioDb[$calling_class])
        || (in_array($calling_object, $this->objectsUsingBioDb))
    ) {
      $should = TRUE;
    }
    return $should;
  }

  /**
   * {@inheritdoc}
   */
  public function prefixTables($sql) {
    // Make sure there is no extra "{number:" in the query.
    if (preg_match_all('/\{(\d+):/', $sql, $matches)) {
      $max_index = array_key_last($this->extraSchemas) ?? 1;
      foreach ($matches[1] as $index) {
        if (($index > $max_index)
            && (!array_key_exists("$index", $this->prefixes))
        ) {
          throw new ConnectionException(
            "Invalid extra schema specification '$index' in statement:\n$sql\nMaximum schema index is currently $max_index."
          );
        }
        elseif ((1 == $index)
          && empty($this->schemaName)
          && (!array_key_exists('1', $this->prefixes))
        ) {
          throw new ConnectionException(
            "No main biological schema set for current connection while it has been referenced in the SQL statement:\n$sql."
          );
        }
      }
    }

    // Check if caller should use biological schema as default.
    $has_prefix = (FALSE !== strpos($sql, '{'));
    if ($has_prefix && $this->shouldUseBioSchema()) {
      // Replace default prefixes.
      $sql = preg_replace('/\{([a-z])/i', '{1:\1', $sql);
    }
    return parent::prefixTables($sql);
  }

  /**
   * (override) Find the prefix for a table.
   *
   * This function is for when you want to know the prefix of a table. This
   * is not used in prefixTables due to performance reasons.
   * This override adds the support for biological schema tables.
   *
   * @param string $table
   *   (optional) The table to find the prefix for.
   * @param bool $use_bio_schema
   *   (optional) if TRUE, table will be prefixed with the biological schema
   *   name (if not empty).
   */
  public function tablePrefix($table = 'default', bool $use_bio_schema = FALSE) {
    $use_bio_schema = ($use_bio_schema || $this->shouldUseBioSchema());
    if (('default' == $table) && $use_bio_schema) {
      $table = '1';
    }

    if (isset($this->prefixes[$table])) {
      return $this->prefixes[$table];
    }
    elseif ($use_bio_schema && !empty($this->schemaName)) {
      return $this->schemaName . '.';
    }
    else {
      return $this->prefixes['default'];
    }
  }

  /**
   * Executes all the given SQL statements into the current schema.
   *
   * For security reasons, only trusted SQL statments should be provided to this
   * method. No user-provided queries should be able, in any way, to reach to
   * this method. Use this method with caution as it is not as secure as the
   * regular ::query method.
   *
   * If no schema was set for this instance, the search_path will not be altered
   * and queries will be run using the current search_path. If a schema has been
   * set, the search_path will be altered to only use that schema during the
   * queries. If $search_path_mode is set to 'none', any "SET search_path =...;"
   * in $sql_queries not followed by a comment '--KEEP' will be removed. If
   * $search_path_mode is set to an array, each key would be considered as a
   * schema name and each corresponding value will be considered as the schema
   * name to use in replacement in every "SET search_path" queries not followed
   * by "--KEEP".
   *
   * @param string $sql_queries
   *   A list of SQL queries to execute.
   * @param $search_path_mode
   *   If set to an empty value or FALSE, no search_path is changed.
   *   If set to 'none', all "SET search_path" queries are removed from the SQL
   *   queries provided.
   *   If set to an array, keys are schema names to replace in every
   *   "SET search_path" by their corresponding values.
   *   Default: FALSE.
   * @param ?string $schema_name
   *   Name of the schema. Default NULL to use current schema.
   *
   * @return bool
   *   Whether the application succeeded.
   *
   * @throws \Drupal\tripal_biodb\Exception\ConnectionException
   *  If the schema can't be used (unexisting) or if the search_path can't be
   *  changed (while a specific schema should be used).
   */
  public function executeSqlQueries(
    string $sql_queries,
    $search_path_mode = FALSE,
    ?string $schema_name = NULL
  ) :bool {
    // Get schema to use.
    $schema_name = $this->getDefaultSchemaName($schema_name);
    // Set search_path.
    if (!empty($schema_name)) {
      $search_path = 'SET search_path = "' . $schema_name . '";';
      $this->connection->exec($search_path);
    }

    $success = TRUE;
    try {

      if (is_string($search_path_mode) && ('none' == $search_path_mode)) {
        // Remove any search_path commands not followed by the comment '--KEEP'.
        $sql_queries = preg_replace(
          '/SET\s*search_path\s*=(?:[^;]+);(?!\s*--\s*KEEP)/im',
          '',
          $sql_queries
        );
      }
      elseif (is_array($search_path_mode)) {
        $search = [];
        $replace = [];
        foreach ($search_path_mode as $old_name => $replacement) {
          // Secure replacement (we allow comas and spaces).
          $replacement = preg_replace(
            '/[^a-z_\\xA0-\\xFF0-9\s,]+/',
            '',
            $replacement
          );
          $search[] =
            '/(SET\s*search_path\s*=(?:[^;]+,)?)\s*'
            . preg_quote($old_name)
            . '\s*((?:,[^;]+)?;)(?!\s*--\s*KEEP)/im'
          ;
          $replace[] = '\1' . $replacement . '\2';
        }
        $sql_queries = preg_replace(
          $search,
          $replace,
          $sql_queries
        );
      }

      // Apply the SQL to the database.
      $success = (FALSE !== $this->connection->exec($sql_queries));
    }
    catch (\Exception $e) {
      $this->messageLogger->error($e->getMessage());
      $success = FALSE;
    }
    // Restore search_path.
    if (!empty($schema_name)) {
      $search_path = $this->connectionOptions['init_commands']['search_path'];
      $this->connection->exec($search_path);
    }

    return $success;
  }

  /**
   * Executes all the SQL statements from a given file into the given schema.
   *
   * This method uses ::executeSqlQueries methods and have the same security
   * concerns. Please read ::executeSqlQueries description.
   *
   * @param string $sql_queries
   *   A list of SQL queries to execute.
   * @param $search_path_mode
   *   If set to an empty value or FALSE, no search_path is changed.
   *   If set to 'none', all "SET search_path" queries are removed from the SQL
   *   queries provided.
   *   If set to an array, keys are schema names to replace in every
   *   "SET search_path" by their corresponding values.
   *   Default: FALSE.
   *
   * @return bool
   *   Whether the application succeeded.
   *
   * @throws \Drupal\tripal_biodb\Exception\ConnectionException
   */
  public function executeSqlFile(
    string $sql_file_path,
    $search_path_mode = FALSE
  ) :bool {
    // Retrieve the SQL file.
    $sql_queries = file_get_contents($sql_file_path);
    return $this->executeSqlQueries(
      $sql_queries,
      $search_path_mode
    );
  }

  /**
   * Implements the magic __toString method.
   */
  public function __toString() {
    return $this->databaseName . '.' . $this->schemaName;
  }

}
