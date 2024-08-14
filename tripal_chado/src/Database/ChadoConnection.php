<?php

namespace Drupal\tripal_chado\Database;

use Drupal\tripal\TripalDBX\TripalDbxConnection;
use Drupal\tripal\TripalDBX\Exceptions\ConnectionException;
use Drupal\tripal_chado\Database\ChadoSchema;

/**
 * Provides an API for Chado schema.
 *
 * Provides an application programming interface (API) for describing Chado
 * schema and tables. It provides both static and instance methods. Static
 * methods are designed to work regardless any specific Chado schema while
 * instance methods work on a given Chado schema instance specified when the
 * ChadoSchema object is instantiated. Default schema used for instances is
 * 'chado'.
 *
 * If you need the Drupal-style array definition for any table, use the
 * following:
 *
 * @code
 * $chado = new \Drupal\tripal_chado\Database\ChadoConnection();
 * $parameters = ['format' => 'drupal'];
 * $table_schema = $chado->schema()->getTableDef($table_name, $parameters);
 * @endcode
 *
 * where the variable $table_name contains the name of the table you want to
 * retireve.  The getTableDef method determines the appropriate version of
 * Chado but it can be forced through the $parameters array.
 * See \Drupal\tripal_chado\Database\ChadoSchema::getTableDef for details.
 *
 * Additionally, here are some other examples of how to use this class:
 * @code
 * // Retrieve all chado tables.
 * $chado = new \Drupal\tripal_chado\Database\ChadoConnection();
 * $all_tables = $chado->schema()->getTables();
 * $base_tables = $chado->schema()->getTables(['base' => TRUE,]);
 *
 * // Check the feature.type_id foreign key constraint.
 * $chado = new \Drupal\tripal_chado\Database\ChadoConnection();
 * $exists = $chado->schema()->foreignKeyConstraintExists('feature','type_id');
 *
 * // Check Sequence exists.
 * $chado = new \Drupal\tripal_chado\Database\ChadoConnection();
 * $exists = $chado->schema()->checkSequenceExists('organism','organism_id');
 * // Or just check the primary key directly.
 * $compliant = $chado->schema()->checkPrimaryKey('organism');
 * @endcode
 */
class ChadoConnection extends TripalDbxConnection {

  /**
   * Reserved schema name of the Chado schema used for testing.
   */
  public const EMPTY_CHADO_SIZE = 8388608;

  /**
   * Default Chado schema version.
   */
  public const DEFAULT_VERSION  = '1.3';

  /**
   * {@inheritdoc}
   */
  public function __construct(
    string $schema_name = '',
    $database = 'default',
    ?\Psr\Log\LoggerInterface $logger = NULL
  ) {
    if (empty($schema_name)) {
      // Get default Chado schema name from the config.
      $schema_name = \Drupal::config('tripal_chado.settings')
        ->get('default_schema')
      ;
      // Still empty (config missing or set to an empty value)?
      if (empty($schema_name)) {
        // Fallback to 'chado'.
        $schema_name = 'chado';
      }
    }
    parent::__construct($schema_name, $database, $logger);
  }

  /**
   * {@inheritdoc}
   */
  public function getTripalDbxClass($class) :string {
    static $classes = [
      'Schema' => ChadoSchema::class,
    ];
    if (!array_key_exists($class, $classes)) {
      throw new ConnectionException("Invalid Tripal DBX class '$class'.");
    }
    return $classes[$class];
  }

  /**
   * Returns the version number of the given Chado schema.
   *
   * For recent Chado instances, version is stored in the schema while version
   * number has to be guessed in older versions (using specific table presence).
   *
   * @param ?string $schema_name
   *   A schema name or NULL to work on current schema.
   * @param bool $exact_version
   *   Returns the most precise version available. Default: FALSE.
   *
   * @return string
   *   The version of Chado ('1.0', '1.1x', '1.2x' '1.3x', '1.4+') or '0' if the
   *   version cannot be guessed but a Chado instance has been detected or ''
   *   if the schema is not a Chado schema.
   */
  public function findVersion(
    ?string $schema_name = NULL,
    bool $exact_version = FALSE
  ) :string {
    // By default, we don't know the version.
    $version = '';

    // If we don't have a schema name then grab the default one.
    // If a schema name is passed in then check it is valid.
    try {
      $schema_name = $this->getDefaultSchemaName($schema_name);
    }
    catch (ConnectionException $e) {
      return $version;
    }

    // Check the drupal table containing all chado instances installed by Tripal.
    $result = $this->select('chado_installations' ,'i')
      ->fields('i', ['version'])
      ->condition('schema_name', $schema_name, '=')
      ->execute()
      ->fetch();
    if ($result) {
      return $result->version;
    }

    // Since it's not integrated into Tripal, we want to make sure it is a
    // Chado schema. To do this we're going to check if an arbitrary list of
    // tables typically in chado are in this schema by counting them.
    $chado_tables = ['db', 'dbxref', 'cv', 'cvterm', 'project', 'organism',
      'synonym', 'feature', 'stock', 'analysis', 'study', 'contact', 'pub',
      'phylonode', 'phylotree', 'library' ];
    $sql_query = "
      SELECT COUNT(1) AS \"cnt\"
      FROM pg_tables
      WHERE schemaname=:schema AND tablename IN (:tables[])";
    $table_match_count = $this->query(
        $sql_query,
        [':schema' => $schema_name, ':tables[]' => $chado_tables]
      )->fetchField();

    // If all of our chado tables were present...
    if (count($chado_tables) == $table_match_count) {

      // We will check for a chadoprop table and get the version from there
      // if it's available.
      if ($this->schema()->tableExists('chadoprop')) {

        $quoted_schema_name = $this->tripalDbxApi->quoteDbObjectId($schema_name);
        $sql_query = "
          SELECT value
          FROM $quoted_schema_name.chadoprop cp
            JOIN $quoted_schema_name.cvterm cvt ON cvt.cvterm_id = cp.type_id
            JOIN $quoted_schema_name.cv CV ON cvt.cv_id = cv.cv_id
          WHERE
            cv.name = 'chado_properties'
            AND cvt.name = 'version'";
        $v = $this->query($sql_query)->fetchObject();
        if ($v) {
          return $v->value;
        }
      }

      // If we don't have a version in the chadoprop table then it must be
      // v1.11 or older...
      // Try to guess it from schema content from table specific to newer
      // versions (https://github.com/GMOD/Chado/tree/master/chado/schemas).

      // 'feature_organism' table added in 0.02.
      if ($this->schema()->tableExists('feature_organism')) {
        $version = '0.02';
      }

      // @bug currently weird prefixing when fieldExists is used.
      // 'cv.cvname' column replaced by 'cv.name' after 0.03.
      // if ($this->schema()->fieldExists('cv ', 'cvname')) {
      //   $version = '0.03';
      // }

      // 'feature_cvterm_dbxref' table added in 1.0.
      if ($this->schema()->tableExists('feature_cvterm_dbxref')) {
        $version = '1.0';
      }

      // 'cell_line' table added in 1.1-1.11.
      if ($this->schema()->tableExists('cell_line')) {
        $version = '1.1';
      }

      // 'cvprop' table added in 1.2-1.24.
      if ($this->schema()->tableExists('cvprop')) {
        $version = '1.2';
      }

      // 'analysis_cvterm' table added in 1.3-1.31.
      if ($this->schema()->tableExists('analysis_cvterm')) {
        $version = '1.3';
      }

      // @bug currently weird prefixing when fieldExists is used.
      // 'featureprop.cvalue_id' column added in 1.4.
      // if ($this->schema()->fieldExists('featureprop', 'cvalue_id')) {
      //   $version = '1.4';
      // }
    }

    return $version;
  }

  /**
   * Get the list of available Chado instances in current database.
   *
   * This function returns both Chado schema integrated with Tripal and free
   * Chado schemas.
   *
   * @return array
   *   An array of available schema keyed by schema name and having the
   *   following structure:
   *   "schema_name": name of the schema (same as the key);
   *   "version": detected version of Chado;
   *   "is_default": TRUE if it is the default Chado schema, FALSE otherwise.
   *   "is_test": if it is a test schema, the key of the corresponding prefix
   *     as it is set in the config and FALSE otherwise;
   *   "is_reserved": the value returned by
   *     Drupal\tripal\TripalDBX\TripalDbx::isSchemaReserved;
   *   "has_data": TRUE if the schema contains more than just default records;
   *   "size": size of the schema in bytes;
   *   "integration": FALSE if not integrated with Tripal and an array
   *     otherwise with the following fields: 'install_id', 'schema_name',
   *     'version', 'created', 'updated'.
   */
  public function getAvailableInstances() :array {
    $chado_schemas = [];

    // Get test schema prefix. If none set, we use '0' so we can test the prefix
    // and avoid false-positive since no schema name is allowed to start by a
    // number.
    $test_prefixes = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names', '0')
    ;
    // Get default schema name.
    $default_schema_name = \Drupal::config('tripal_chado.settings')
      ->get('default_schema')
    ;

    // First we get a list of available schemas excluding obvious non-chado
    // schemas.
    // Here we did not escape schemata table using curly braces because it is a
    // PostgreSQL system table (information_schema) and it should not be processed
    // by Drupal.
    $sql_query = "
      SELECT schema_name AS \"name\"
      FROM information_schema.schemata
      WHERE
        schema_name NOT IN ('information_schema', 'pg_catalog');
    ";
    $schemas = $this->query($sql_query)->fetchAll();

    // Then we get schema part of Tripal.
    $integrated_schemas = $this
      ->select('chado_installations' ,'i')
      ->fields(
        'i',
        ['install_id', 'schema_name', 'version', 'created', 'updated']
      )
      ->execute()
      ->fetchAllAssoc('schema_name', \PDO::FETCH_ASSOC)
    ;

    foreach ($schemas as $schema) {
      $version = $this->findVersion($schema->name);
      if ('' !== $version) {
        // Get size.
        $schema_size = $this->tripalDbxApi->getSchemaSize($schema->name);
        $has_data = (static::EMPTY_CHADO_SIZE < $schema_size);
        // Check for test schema.
        $is_test = FALSE;
        foreach ($test_prefixes as $key => $prefix) {
          if (str_starts_with($schema->name, $prefix)) {
            $is_test = $key;
          }
        }
        // Check if part of Tripal.
        $integration = $integrated_schemas[$schema->name] ?? FALSE;
        // Check if default.
        $is_default = FALSE;
        if ($integration && ($schema->name == $default_schema_name)) {
          $is_default = TRUE;
        }
        // Add schema to available Chado schema list.
        $schema_class = $this->getTripalDbxClass('Schema');
        $chado_schemas[$schema->name] = [
          'schema_name' => $schema->name,
          'version'     => $version,
          'is_default'  => $is_default,
          'is_test'     => $is_test,
          'is_reserved' => $this->tripalDbxApi->isSchemaReserved($schema->name),
          'has_data'    => $has_data,
          'size'        => $schema_size,
          'integration' => $integration,
        ];
      }
    }
    return $chado_schemas;
  }

  /**
   * Removes all existing Chado test schemas.
   *
   * Use this function when tests schemas were not removed properly by the
   * automated test system.
   *
   * Usage:
   *   \Drupal\tripal_chado\Database\ChadoConnection::removeAllTestSchemas();
   */
  public static function removeAllTestSchemas() :void {
    // Get Chado test schema prefix.
    $test_schema = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names', [])['chado']
    ;
    // Remove all matching schemas.
    $db = \Drupal::database();
    $schemas = $db->query(
      "SELECT schema_name FROM information_schema.schemata WHERE schema_name LIKE '$test_schema%';"
    );
    $dropped = [];
    foreach ($schemas as $schema) {
      try {
        $schema_name = $schema->schema_name;
        $db->query("DROP SCHEMA \"$schema_name\" CASCADE;");
        $dropped[] = $schema_name;
      }
      catch (\Drupal\Core\Database\DatabaseException $e) {
        // ignore errors.
      }
    }
    \Drupal::logger('tripal_chado')->notice(
      "Removed test schemas: " . implode(', ', $dropped)
    );
  }

  /**
   * {@inheritdoc}
   */
  public static function open(array &$connection_options = []) {
    parent::open($connection_options);
  }

  /**
   * {@inheritdoc}
   */
  public function upsert($table, array $options = []) {
    parent::upsert($table, $options);
  }

  /**
   * {@inheritdoc}
   */
  public function queryRange($query, $from, $count, array $args = [], array $options = []) {
    parent::queryRange($query, $from, $count, $args, $options);
  }

  /**
   * {@inheritdoc}
   */
  public function driver() {
    parent::driver();
  }

  /**
   * {@inheritdoc}
   */
  public function databaseType() {
    parent::databaseType();
  }

  /**
   * {@inheritdoc}
   */
  public function createDatabase($database) {
    parent::createDatabase($database);
  }

  /**
   * {@inheritdoc}
   */
  public function mapConditionOperator($operator) {
    parent::mapConditionOperator($operator);
  }

}
