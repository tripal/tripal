<?php

namespace Drupal\tripal\TripalDBX;

use Drupal\Core\Database\Driver\pgsql\Connection as PgConnection;
use Drupal\tripal\TripalDBX\TripalDbxSchema;
use Drupal\tripal\TripalDBX\Exceptions\ConnectionException;

/**
 * Tripal DBX API Connection class.
 *
 * This class provides a Tripal-specific extension of the Drupal database
 * connection abstract class. It extends the base class with specific
 * functions dedicated to querying separate schema aside from the Drupal schema.
 * It has been designed mostly based on Chado schema and PostgreSQL features
 * allowing you have several schemas in the same database and query across them.
 *
 * The core functionality provided by extending the core Drupal Connection class
 * is to support additional schema using the {tablename} notation for the
 * Drupal schema, {1: tablename} notation for the current Tripal DBX Managed
 * schema and {2+: tablename} notation for any additional Tripal DBX managed
 * schema.
 *
 * For example, the following code joins chado feature data between two Tripal
 * DBX managed chado schema and includes a join to the drupal node_field_data
 * table.
 *
 *  $dbxdb = \Drupal::service('tripal_chado.database');
 *  $dbxdb->setSchemaName('chado1');
 *  $dbxdb->addExtraSchema('chado2');
 *  $sql = "
 *    SELECT * FROM
 *      {1:feature} f1,
 *      {2:feature} f2,
 *      {node_field_data} fd
 *    WHERE fd.title = f1.uniquename
 *    AND f1.uniquename = f2.uniquename;";
 * $results = $dbxdb->query($sql);
 *
 * Additionally, this class allows you to use the native PHP/Drupal PDO
 * query builder as shown in this next example:
 *
 * $dbxdb = \Drupal::service('tripal_chado.database');
 * $query = $dbxdb->select('feature', 'x');
 * $query->condition('x.is_obsolete', 'f', '=');
 * $query->fields('x', ['name', 'residues']);
 * $query->range(0, 10);
 * $result = $query->execute();
 * foreach ($result as $record) {
 *   // Do something with the $record object here.
 *   // e.g. echo $record->name;
 * }
 *
 * Here are some useful inherited methods to know:
 *
 * - TripalDbxConnection::select(), insert(), update(), delete(), truncate(),
 *   upsert(), prepare(), startTransaction(), commit(), rollBack(), quote(),
 *   quoteIdentifiers(), escape*() methods, query*() methods, and more from
 *   \Drupal\Core\Database\Connection.
 *
 * - TripalDbxConnection::schema() that provides a \Drupal\Core\Database\Schema object
 *   and offers, beside others, the follwing methods: addIndex(),
 *   addPrimaryKey(), addUniqueKey(), createTable(), dropField(), dropIndex(),
 *   dropPrimaryKey(), dropTable(), dropUniqueKey(), fieldExists(),
 *   findPrimaryKeyColumns(), findTables(), indexExists(), renameTable()
 *   and more from the documentation.
 *
 * A couple of methods have been added to this class to complete the above list.
 *
 * NOTE: It has been documented that in some cases extraSchema set in previous
 * connections have been cached. For example, you create $connection1 and set
 * an extraSchema('fred') then when you create a new $connection2, 'fred' may
 * already be set as an extraSchema(). This would be a problem if 2 different
 * parts of code want to use different extra schemas and expect the schema they
 * added to be the second one while it would be the third one for the last
 * one using addExtraSchema(). More work needs to be done to determine the core
 * cause for this.
 *
 * NOTE: the setLogger() and getLogger() methods are reserved for database query
 * logging and is operated by Drupal. It works with a \Drupal\Core\Database\Log
 * class. To log messages in extending classes, use setMessageLogger() and
 * getMessageLogger() instead, which operates with the \Drupal\tripal\Services\TripalLogger
 * class. By default, the message logger is set by the constructor either using
 * the user-provided logger or by instanciating one using the log channel
 * 'tripal.logger'.
 *
 * @see https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Connection.php/class/Connection/9.0.x
 * @see https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Connection.php/class/Connection/9.0.x
 */
abstract class TripalDbxConnection extends PgConnection {

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
    \Drupal\pgsql\Driver\Database\pgsql\Connection::class => TRUE,
    \Drupal\tripal\TripalDBX\TripalDbxConnection::class => TRUE,
  ];

  /**
   * Supported Connection classes.
   * These must inherit from \Drupal\Core\Database\Connection
   *
   * NOTE: These are in order of preference with the first entry available
   *  being used to open new connections.
   * NOTE: the pgsql driver changed namespace in 9.4.x
   *  Drupal\Core\Database\Driver\pgsql\Connection => Drupal\pgsql\Driver\Database\pgsql\Connection
   *
   * @var array
   */
  protected static $supported_classes = [
    'Drupal\pgsql\Driver\Database\pgsql\Connection',
    'Drupal\Core\Database\Driver\pgsql\Connection'
  ];

  /**
   * Database connection.
   *
   * @var \Drupal\Core\Database\Connection
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
   * An ordered list of used schemas.
   *
   * Schema [0] should be the default schema name (Drupal's one) and [1]
   * should contain the main biological data schema name.
   *
   * @var array
   */
  protected $usedSchemas = [];

  /**
   * The version for current Tripal DBX managed schema instance.
   *
   * @var ?string
   */
  protected $version = NULL;

  /**
   * Logger.
   *
   * @var object \Drupal\tripal\Services\TripalLogger
   */
  protected $messageLogger = NULL;

  /**
   * Access to the TripalDBX API Service.
   *
   * @var object \Drupal\tripal\TripalDBX\TripalDbx
   */
  protected $tripalDbxApi = NULL;

  /**
   * List of objects that will use TripalDBX managed schema as default.
   *
   * @var array
   */
  protected $objectsUsingTripalDbx = [];

  /**
   * List of classes that will use TripalDBX managed schema as default.
   *
   * @var array
   */
  protected $classesUsingTripalDbx = [];

  /**
   * Returns the version number of the given Tripal DBX managed schema.
   *
   * @param ?string $schema_name
   *   A schema name or NULL to work on current schema.
   * @param bool $exact_version
   *   Returns the most precise version available. Default: FALSE.
   *
   * @return string
   *   The version in a simple format like '1.0', '2.3x' or '4.5+' or '0' if the
   *   version cannot be guessed but an instance of the Tripal DBX managed schema has
   *   been detected or an empty string if the schema does not appear to be an
   *   instance of the Tripal DBX managed schema. If $exact_version is FALSE , the
   *   returned version must always starts by a number and can be tested against
   *   numeric values (ie. ">= 1.2"). If $exact_version is TRUE, the format is
   *   free and can start by a letter and hold several dots like 'v1.2.3 alpha'.
   */
  abstract public function findVersion(
    ?string $schema_name = NULL,
    bool $exact_version = FALSE
  ) :string;

  /**
   * Get the list of available "Tripal DBX Managed schema" instances in current database.
   *
   * This function returns both PostgreSQL schemas integrated with Tripal
   * and free schemas.
   *
   * @return array
   *   An array of available schema keyed by schema name and having the
   *   following structure:
   *   "schema_name": name of the schema (same as the key);
   *   "version": detected version of the Tripal DBX managed schema;
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
   * already. However, in the context of a TripalDbxConnection, we need a different
   * database context for each connection since the search_path may be changed.
   * To not mess up with Drupal stuff, we need to open a new and distinct
   * database connection for each TripalDbxConnection instance.
   *
   * @param \Drupal\Core\Database\Connection $database
   *   The database connection to duplicate.
   *
   * @return \PDO
   *   A \PDO object.
   */
  protected static function openNewPdoConnection(
    \Drupal\Core\Database\Connection $database
  ) {
    // We call this method in a context of an existing connection already
    // used by Drupal so we can avoid a couple of tests and assume it works.
    $database_info = \Drupal\Core\Database\Database::getAllConnectionInfo();
    $target = $database->target;
    $key = $database->key;

    // Open a new connection with the first supported connection available.
    $database_class = NULL;
    array_walk(self::$supported_classes, function($class_name) use(&$database_class) {
      if (class_exists($class_name) AND is_null($database_class)) {
        $database_class = $class_name;
      }
    });
    $pdo_connection = $database_class::open(
      $database_info[$key][$target]
    );
    return $pdo_connection;
  }

  /**
   * Constructor for a Tripal DBX connection.
   *
   * @param string $schema_name
   *   The Tripal DBX managed schema name to use.
   *   Default: '' (no schema). It will throw exceptions on methods needing a
   *   default schema but may work on others or when a schema can be passed
   *   as parameter.
   * @param \Drupal\Core\Database\Connection|string $database
   *   Either a \Drupal\Core\Database\Connection instance or a
   *   Drupal database key string (from current site's settings.php).
   *   Extra databases specified in settings.php do not need to specify a
   *   schema name as a database prefix parameter. The prefix will be managed by
   *   this connection class instance.
   * @param ?\Drupal\tripal\Services\TripalLogger $logger
   *   A logger in case of operations to log.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\ConnectionException
   * @throws \Drupal\Core\Database\ConnectionNotDefinedException
   *
   * @see https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Database.php/function/Database%3A%3AgetConnection/9.0.x
   * @see https://api.drupal.org/api/drupal/sites%21default%21default.settings.php/9.0.x
   * @see https://www.drupal.org/docs/8/api/database-api/database-configuration
   */
  public function __construct(
    string $schema_name = '',
    $database = 'default',
    ?\Drupal\tripal\Services\TripalLogger $logger = NULL
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


    // Make sure we are using a supported connection.
    if (is_object($database)) {
      $database_class = get_class($database);
      if (!in_array($database_class, self::$supported_classes)) {
        throw new ConnectionException("The provided connection object is not a PostgreSQL database connection but is instead from $database_class.");
      }
    }
    else {
      $type = gettype($database);
      throw new ConnectionException("We expected a PostgreSQL database connection or Drupal database key string but instead recieved a $type.");
    }

    // Get a TripalDBX object.
    $this->tripalDbxApi = \Drupal::service('tripal.dbx');

    // Get option array.
    $connection_options = $database->getConnectionOptions();
    $this->databaseName = $connection_options['database'];

    // Get a new connection distinct from Drupal's to avoid search_path issues.
    $connection = static::openNewPdoConnection($database);
    $this->setTarget($database->target);
    $this->setKey($database->key);

    // Call parent constructor to initialize stuff well.
    parent::__construct($connection, $connection_options);

    // Logger.
    if (!isset($logger)) {
      // We need a logger.
      $logger = \Drupal::service('tripal.logger');
    }
    $this->messageLogger = $logger;

    // Set schema name after parent intialisation in order to setup schema
    // prefix appropriately.
    $this->setSchemaName($schema_name);

    // Register Schema class to use Tripal DBX managed schema as default.
    // $this->useTripalDbxSchemaFor(PgConnection::class);
    // $this->useTripalDbxSchemaFor(\Drupal\Core\Database\Connection::class);
    $this->useTripalDbxSchemaFor(\Drupal\Core\Database\Schema::class);
    $this->useTripalDbxSchemaFor(\Drupal\Core\Database\Driver\pgsql\Schema::class);
    $this->useTripalDbxSchemaFor(\Drupal\tripal\TripalDBX\TripalDbxSchema::class);
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
   * Note: the setLogger() and getLogger() methods are reserved for database query
   * logging and is operated by Drupal. It works with a \Drupal\Core\Database\Log
   * class. To log messages in extending classes, use setMessageLogger() and
   * getMessageLogger() instead, which operates with the \Drupal\tripal\Services\TripalLogger
   * class. By default, the message logger is set by the constructor either using
   * the user-provided logger or by instanciating one using the log channel
   * 'tripal.logger'.
   *
   * @return \Drupal\tripal\Services\TripalLogger
   *  A message logger.
   */
  public function getMessageLogger() :\Drupal\tripal\Services\TripalLogger {
    return $this->messageLogger;
  }

  /**
   * Sets current message logger.
   *
   * Note: the setLogger() and getLogger() methods are reserved for database query
   * logging and is operated by Drupal. It works with a \Drupal\Core\Database\Log
   * class. To log messages in extending classes, use setMessageLogger() and
   * getMessageLogger() instead, which operates with the \Drupal\tripal\Services\TripalLogger
   * class. By default, the message logger is set by the constructor either using
   * the user-provided logger or by instanciating one using the log channel
   * 'tripal.logger'.
   *
   * @param \Drupal\tripal\Services\TripalLogger $logger
   *  A message logger.
   */
  public function setMessageLogger(\Drupal\tripal\Services\TripalLogger $logger) :void {
    $this->messageLogger = $logger;
  }

  /**
   * Returns a Schema object for manipulating the schema.
   *
   * OVERRIDES \Drupal\Core\Database\Connection:schema()
   *
   * This method overrides the parent one in order to force the use of the
   * \Drupal\tripal\TripalDBX\TripalDbxSchema class and manage Tripal DBX managed schema
   * changes for this connection. The Schema object is updated on changes.
   *
   * @return \Drupal\Core\Database\Schema
   *   The database Schema object for this connection.
   */
  public function schema() {
    if (empty($this->schema)) {
      $class = $this->getTripalDbxClass('Schema');
      $this->schema = new $class($this);
    }
    return $this->schema;
  }

  /**
   * Sets current Tripal DBX managed schema name.
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
   *   The Tripal DBX managed schema name to use.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\ConnectionException
   */
  public function setSchemaName(string $schema_name) :void {
    // Does schema change?
    if (!empty($this->usedSchemas[1])
        && ($schema_name == $this->usedSchemas[1])
    ) {
      return;
    }

    // Check name is valid.
    if (!empty($schema_name)
        && ($issue = $this->tripalDbxApi->isInvalidSchemaName($schema_name, TRUE))
    ) {
      throw new ConnectionException(
        "Could not use the schema name '$schema_name'.\n$issue"
      );
    }
    // Resets some members.
    $this->usedSchemas = ['' , '', ];
    $this->schema = NULL;
    $this->version = NULL;

    // Set instance schema name.
    $this->usedSchemas[1] = $schema_name;

    // Update search_path.
    if (!empty($schema_name)) {
      [$start_quote, $end_quote] = $this->identifierQuotes;
      $search_path =
        'SET search_path='
        . $start_quote
        . $schema_name
        . $end_quote
      ;
      $drupal_schema = $this->tripalDbxApi->getDrupalSchemaName();
      if (!empty($drupal_schema)) {
        $search_path .=
          ','
          . $start_quote
          . $drupal_schema
          . $end_quote
        ;
      }
      $this->connectionOptions['init_commands']['search_path'] = $search_path;
      $this->connection->exec($search_path);
    }
  }

  /**
   * Returns current Tripal DBX managed schema name.
   *
   * @return string
   *   Current Tripal DBX managed schema name or an empty string if not set.
   */
  public function getSchemaName() :string {
    return $this->usedSchemas[1] ?? '';
  }

  /**
   * Returns current Tripal DBX managed schema name quoted for PostgreSQL queries.
   *
   * This getter should rarely be used (and in very specific cases).
   * If the schema name does not contain any special characters, it might not
   * require any quote and returned quoted name will be the same as $schemaName.
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
   *   Current Tripal DBX managed schema name  quoted by PostgreSQL if necessary or an
   *   empty string if not set.
   */
  public function getQuotedSchemaName() :string {
    $quoted_schema_name = '';
    if (array_key_exists(1, $this->usedSchemas) and !empty($this->usedSchemas[1])) {
      [$start_quote, $end_quote] = $this->identifierQuotes;
      $quoted_schema_name = $start_quote . $this->usedSchemas[1] . $end_quote;
    }
    return $quoted_schema_name;
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
   *   An error message to throw if none of $schema_name and
   *   $this->usedSchemas[1] are set. Default: 'Invalid schema name.'
   *
   * @return string
   *   $schema_name if set and valid, or the current schema name.
   *
   * @throws \Drupal\tripal\TripalDBX\Exceptions\ConnectionException
   *  If the given schema name is invalid (ignoring schema name reservations)
   *  or none of $schema_name and $this->usedSchemas[1] are set.
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
      if (empty($this->usedSchemas[1])) {
        throw new ConnectionException($error_message);
      }
      $schema_name = $this->usedSchemas[1];
    }
    else {
      if ($issue = $this->tripalDbxApi->isInvalidSchemaName($schema_name, TRUE)) {
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
    $extra_schemas = $this->usedSchemas;
    unset($extra_schemas[0], $extra_schemas[1]);
    return $extra_schemas;
  }

  /**
   * Clears the extra schemas list.
   */
  public function clearExtraSchemas() :void {
    $this->usedSchemas = array_splice($this->usedSchemas, 0, 2);
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
   * @throws \Drupal\tripal\TripalDBX\Exceptions\ConnectionException
   *   If the given schema name is invalid or does not exist in current
   *   database or there is no current schema.
   */
  public function addExtraSchema(string $schema_name) :int {
    if (empty($this->usedSchemas[1])) {
      throw new ConnectionException(
        "Cannot add an extra schema. No current schema (specified by ::setSchemaName)."
      );
    }
    // Check provided name.
    if ($issue = $this->tripalDbxApi->isInvalidSchemaName($schema_name, TRUE)) {
      throw new ConnectionException($issue);
    }
    // Append new schema.
    $this->usedSchemas[] = $schema_name;
    return array_key_last($this->usedSchemas);
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
   * @throws \Drupal\tripal\TripalDBX\Exceptions\ConnectionException
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
    elseif (count($this->usedSchemas) < $index) {
      throw new ConnectionException(
        "Invalid extra schema index '$index'. Intermediate schemas are missing."
      );
    }
    if (empty($this->usedSchemas[1])) {
      throw new ConnectionException(
        "Cannot add an extra schema. No current schema (specified by ::setSchemaName)."
      );
    }
    // Check provided name.
    if ($issue = $this->tripalDbxApi->isInvalidSchemaName($schema_name, TRUE)) {
      throw new ConnectionException($issue);
    }
    $this->usedSchemas[$index] = $schema_name;
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

    if (!is_numeric($this->version) && !empty($this->usedSchemas[1])) {
      // Get the version of the schema.
      $this->version = (string) $this->findVersion();
    }

    return $this->version ?? '';
  }

  /**
   * Use the Tripal DBX managed schema as default for the given things.
   *
   * Register an object or a class to make them use the Tripal DBX managed schema as
   * default in any method of this instance of TripalDbxConnection.
   *
   * @param string|object
   *   Object or class to register.
   */
  public function useTripalDbxSchemaFor($object_or_class) {
    if (is_string($object_or_class)) {
      // Class.
      $this->classesUsingTripalDbx[$object_or_class] = $object_or_class;
    }
    else {
      // Object.
      $this->objectsUsingTripalDbx[] = $object_or_class;
    }
  }

  /**
   * Remove the given things from the lists using Tripal DBX managed schema as default.
   *
   * @param string|object
   *   Object or class to unregister.
   */
  public function useDrupalSchemaFor($object_or_class) {
    if (is_string($object_or_class)) {
      // Remove class for the list.
      unset($this->classesUsingTripalDbx[$object_or_class]);
    }
    else {
      // Remove object from the list.
      $this->objectsUsingTripalDbx = array_filter(
        $this->objectsUsingTripalDbx,
        function ($o) { return $o != $object_or_class; }
      );
    }
  }

  /**
   * Gets the Tripal DBX-specific class for the specified category.
   *
   * Returns the Tripal DBX-specific override class if any for the
   * specified class category.
   *
   * @param string $class
   *   The class category for which we want the specific class.
   *
   * @return string
   *   The name of the class that should be used.
   */
  public function getTripalDbxClass($class) :string {
    static $classes = [
      'Schema' => TripalDbxSchema::class,
    ];
    if (!array_key_exists($class, $classes)) {
      throw new ConnectionException("Invalid Tripal DBX class '$class'.");
    }
    return $classes[$class];
  }

  /**
   * Tells if the caller assumes current schema is the Tripal DBX managed schema.
   *
   * @return bool
   *   TRUE if default schema is not Drupal's but the Tripal DBX managed one.
   */
  public function shouldUseTripalDbxSchema() :bool {
    $should = FALSE;

    // Check the class/object who is using Tripal DBX:
    // We do this using the backtrace functionality with the assumption that
    // the class at the deepest level of the backtrace is the one to check.
    //
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
    if (!empty($this->classesUsingTripalDbx[$calling_class])) {
      $should = TRUE;
    }
    elseif (in_array($calling_object, $this->objectsUsingTripalDbx)) {
      $should = TRUE;
    }

    // Check all parents of the class who is using Tripal DBX:
    // This allows for APIs to be added to the whitelist and all children class
    // implementations to then automatically use the Tripal DBX managed schema.
    if (class_exists($calling_class)) {
      $class = new \ReflectionClass($calling_class);
      $inheritance_level = 0;
      while ($parent = $class->getParentClass()) {
        $inheritance_level++;
        $parent_class = $parent->getName();
        if (!empty($this->classesUsingTripalDbx[$parent_class])) {
          $should = TRUE;
        }
        $class = $parent;
      }
    }
    // If Tripal DBX was called from a stand-alone function (i.e. not within
    // a class) then the calling class will be empty. We do not want to throw
    // an exception in that case.
    elseif (!empty($calling_class)) {
      throw new \Exception("TripalDBX unable to find class for checking inheritance. This class must exist and be available in the current application space: $calling_class. Hint: make sure to 'use' all needed classes in your application.");
    }

    return $should;
  }

  /**
   * Appends a database prefix to all tables in a query.
   *
   * OVERRIDES \Drupal\Core\Database\Connection:prefixTables().
   *
   * This API expects all table names to be wrapped in curly brackets with an
   * integer indicating the schema the table is in. For example, {1: feature}
   * would indicate the feature table in the current Tripal DBX managed schema,
   * {0: system} would indicate the drupal system table and additional numeric
   * indices would be used for extra Tripal DBX managed schema.
   *
   * For Example, lets say the schema name of the current TripalDBX managed
   * schema is "chado", Drupal is in the "public" schema and we have a second
   * Tripal DBX managed schema named "genotypes".
   *
   * Now assume the following query was submitted to this function:
   *  SELECT f.name as marker_name, g.allele
   *    FROM {1: feature} f
   *    LEFT JOIN {2: genotype_call} g ON g.marker_id = f.feature_id
   *    WHERE f.uniquename = 'MarkerICareAbout'
   *
   * Then the returned, properly prefixed query would be:
   *  SELECT f.name as marker_name, g.allele
   *    FROM chado.feature f
   *    LEFT JOIN genotypes.genotype_call g ON g.marker_id = f.feature_id
   *    WHERE f.uniquename = 'MarkerICareAbout'
   *
   * @param string $sql
   *   A string containing a partial or complete SQL query.
   *
   * @return string
   *   The same query passed in  but now with properly prefixed table names.
   */
  public function prefixTables($sql) {

    // Replace schema prefixes if some.
    if (preg_match('#\{\d+:#', $sql)) {
      $sql = preg_replace_callback(
        '#\{(\d+):(' . TripalDbx::TABLE_NAME_REGEXP . ')\}#',
        function ($matches) {
          // If the schema key is 0 then it indicates to use the drupal prefixing.
          // As such., we will just remove the schema prefix and keet the curly
          // brackets for the parent call replacements.
          if (0 == $matches[1]) {
            $prefixed = '{' . $matches[2] . '}';
          }
          // Next, check that the schema key we are given is associated with a
          // known schema...
          elseif (array_key_exists($matches[1], $this->usedSchemas) AND !empty($this->usedSchemas[ $matches[1] ])) {
            // Quote schema.
            $prefixed =
              $this->identifierQuotes[0]
              . $this->usedSchemas[$matches[1]]
              . $this->identifierQuotes[1]
              . '.'
              . $this->identifierQuotes[0]
              . $matches[2]
              . $this->identifierQuotes[1];
          }
          // If this is not a known schema then throw an exception.
          else {
            // Note: Cannot include $sql here since it's not in scope.
            // Add available schema info for easier debugging since this can
            // be thrown if the schema exists but just was not set for the primary
            // schema (key 1) and for unset Extra Schema (key 2,3,4).
            $schema_note = [];
            foreach ($this->usedSchemas as $key => $name) {
              if (!empty($name)) {
                $schema_note[] = "$name ($key)";
              }
            }
            if (!empty($schema_note)) {
              $schema_note_rendered = 'Available schema set for this connection: ' . implode(', ', $schema_note);
            }
            else {
              $schema_note_rendered = 'No schema set for this connection.';
            }
            throw new ConnectionException(
              "Invalid schema specification '{"
              . $matches[1]
              . ":"
              . $matches[2]
              . "}'. "
              . $schema_note_rendered
            );
          }
          return $prefixed;
        },
        $sql
      );
    }
    else {
      // No schema-prefixed tables in $sql.
      // Check if caller should use cross schema as default.
      $has_prefix = (FALSE !== strpos($sql, '{'));
      if ($has_prefix && $this->shouldUseTripalDbxSchema()) {
        // Replace default prefixes.
        $default_replacement =
          $this->identifierQuotes[0]
          . $this->usedSchemas[1]
          . $this->identifierQuotes[1]
          . '.'
          . $this->identifierQuotes[0]
          . '\1'
          . $this->identifierQuotes[1]
        ;
        $sql = preg_replace(
          '#\{(' . TripalDbx::TABLE_NAME_REGEXP . ')\}#',
          $default_replacement,
          $sql
        );
      }
      // In updates we have to replace the schema-prefixed table name in
      // the escapeTables() method. That results in there being the following
      // case in here: where our chado is in a schema named 'teapot',
      // we may see {teapot.chadotable} at this point in the code.
      // Here we want to remove the surrounding curly brackets.
      $match_pattern = '#\{'
        . '(' . TripalDbx::SCHEMA_NAME_REGEXP . ')'
        . '\.'
        . '(' . TripalDbx::TABLE_NAME_REGEXP . ')'
        . '\}#';
      if (preg_match($match_pattern, $sql, $matches) === 1) {
        $sql = preg_replace_callback(
          $match_pattern,
          function ($matches) {
            // For example, if given {teapot.chadotable}
            // then return "teapot"."chadotable".
            return
              $this->identifierQuotes[0]
              . $matches[1]
              . $this->identifierQuotes[1]
              . '.'
              . $this->identifierQuotes[0]
              . $matches[2]
              . $this->identifierQuotes[1]
            ;
          },
          $sql
        );
      }
    }

    // Finally let Drupal deal with any remaining table prefixing that is needed.
    return parent::prefixTables($sql);
  }

  /**
   * Find the prefix for a table.
   *
   * OVERRIDES \Drupal\Core\Database\Connection:tablePrefix().
   * REMOVED IN Drupal 10.1.x
   * SEE https://www.drupal.org/node/3260849
   *
   * This function is for when you want to know the prefix of a table. This
   * is not used in prefixTables due to performance reasons.
   *
   * This override adds the support for Tripal DBX managed schema tables
   * by returning the prefix used for a table in a Tripal DBX managed schema
   * if applicable.
   *
   * This override adds the optional $use_tdbx_schema parameter which defaults to
   * False to maintain backwards compatibility for non-cross-schema-aware queries.
   * Additionally, there is support through this API with this function and
   * prefixTables() for non-prefixed tables (i.e. {tablename}) to be used for
   * both Drupal and Chado tables depending on the situation.
   *
   * There are a couple of ways to use this. Call this function with:
   *   A) $table matching to the index you would use for your Tripal DBX
   *      managed schema (i.e. 0: drupal, 1:current, 2+:extra in order added).
   *      This would return the expected prefix used by Tripal DBX (e.g. "chado.")
   *   B) $myinstance->tablePrefix('default', TRUE).
   *      This would return the Tripal DBX prefix of the current schema (i.e. index 1).
   *   C) a Drupal table name  only (i.e. $use_tdbx_schema = FALSE) not realizing
   *      it's been overriden and get the Drupal table prefix. (Backwards Compatible)
   *   D) any table name and $use_tdbx_schema = TRUE and get the prefix for the
   *      current Tripal DBX Managed schema.
   *
   * NOTE: This function does not support Drupal per-table prefixing. While
   *   Drupal supported this originally, it has been deprecated in Drupal 8.3
   *   according to https://www.drupal.org/project/drupal/issues/2551549
   *
   * @param string $table
   *   (optional) The table to find the prefix for.
   * @param bool $use_tdbx_schema
   *   (optional) if TRUE, table will be prefixed with the Tripal DBX managed schema
   *   name (if not empty).
   *
   * @return string
   *   The prefix that would be used for a table in the specified schema.
   */
  public function tablePrefix(
    $table = 'default',
    bool $use_tdbx_schema = FALSE
  ) {
    $use_tdbx_schema = ($use_tdbx_schema || $this->shouldUseTripalDbxSchema());
    if (('default' == $table) && $use_tdbx_schema) {
      $table = '1';
    }
    $use_cross_schema = ($use_tdbx_schema || $this->shouldUseTripalDbxSchema());

    if ($use_cross_schema && !empty($this->usedSchemas[1])) {
      return $this->usedSchemas[1] . '.';
    }
    else {
      return parent::tablePrefix($table);
    }
  }

  /**
   * Returns the prefix of the tables.
   *
   * OVERRIDES \Drupal\Core\Database\Connection:getPrefix().
   *
   * @return string $prefix
   */
  public function getPrefix(): string {
    return $this->usedSchemas[1] . '.';
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
   * @throws \Drupal\tripal\TripalDBX\Exceptions\ConnectionException
   *  If the schema can't be used (unexisting) or if the search_path can't be
   *  changed (while a specific schema should be used).
   */
  public function executeSqlQueries(
    string $sql_queries,
    $search_path_mode = FALSE,
    ?string $schema_name = NULL
  ) :bool {
    // Get schema to use.
    if (empty($schema_name)) {
      $schema_name = $this->getDefaultSchemaName($schema_name);
    }
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

          // Ensure the replacement pattern is sanitized.
          // Secure replacement (we allow commas and spaces).
          $replacement = preg_replace(
            '/[^a-z_\\xA0-\\xFF0-9\s,]+/',
            '',
            $replacement
          );

          // Find/Replace any search path queries.
          $search[] =
            '/(SET\s*search_path\s*=(?:[^;]+,)?)\s*'
            . preg_quote($old_name)
            . '\s*((?:,[^;]+)?;)(?!\s*--\s*KEEP)/im'
          ;
          $replace[] = '\1' . $replacement . '\2';

          // Find/replace any in-query table prefixing.
          $search[] = '/([ \'])'. preg_quote($old_name) . '\.(\w+[ \'])/';
          $replace[] = '\1' . $replacement . '.\2';

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
   * @throws \Drupal\tripal\TripalDBX\Exceptions\ConnectionException
   */
  public function executeSqlFile(
    string $sql_file_path,
    $search_path_mode = FALSE,
    ?string $schema_name = NULL
  ) :bool {
    // Retrieve the SQL file.
    $sql_queries = file_get_contents($sql_file_path);
    return $this->executeSqlQueries(
      $sql_queries,
      $search_path_mode,
      $schema_name
    );
  }

  /**
   * Escapes a table name string.
   *
   * OVERRIDES \Drupal\Core\Database\Connection:escapeTable().
   *
   * This function is meant to force all table names to be strictly
   * alphanumeric-plus-underscore. According to the Drupal documentation,
   * database drivers should never wrap the table name in database-specific
   * escape characters.
   *
   * We have a different use case however, as we need to add prefixes to our
   * table names based on schema which is indicated using a numerical indicator
   * before the table name (i.e. '2:'' for the second schema).
   *
   * As such, we need to prefix the table names now to ensure that information
   * is not lost as the parent:escapeTable() method removes the ':'.
   *
   * @param string $table
   *   The value within the curley brackets (i.e. '{2:feature}').
   * @return string
   *   The sanitized version of the table name. For Tripal DBX managed schema
   *   this will include the schema prefix (e.g. 'chado2.feature').
   */
  public function escapeTable($table) {

    if (preg_match('/^\d+:/', $table)) {
      $table = $this->prefixTables('{' . $table . '}');
    }

    return parent::escapeTable($table);
  }

  /**
   * Retrieve a list of classes which are using Tripal DBX byb default.
   *
   * @return array
   *  An array of class names including namespace.
   */
  public function getListClassesUsingTripalDbx() {
    return $this->classesUsingTripalDbx;
  }

  /**
   * Implements the magic __toString method.
   */
  public function __toString() {
    return $this->databaseName . '.' . $this->usedSchemas[1];
  }

}
