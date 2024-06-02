<?php
namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * This is a PHP Trait for Chado tests.
 *
 * It provides the functions and member variables that are
 * used for both any test class that needs testing with Chado.
 *
 * @group Tripal
 * @group Tripal Chado
 */

trait ChadoTestTrait  {

  /**
   * Tripal DBX tool instance.
   */
  protected $tripal_dbx = NULL;

  /**
   * Real (Drupal live, not test) config factory.
   */
  protected $realConfigFactory;

  /**
   * Base name for test schemas.
   */
  protected $testSchemaBaseNames;

  /**
   * List of tested schemas.
   *
   * Keys are schema names and values are boolean. At test shutdown, when the
   * test schema cleanup method is called, if a schema name is set to TRUE and
   * could not be removed, an error message will be reported.
   * Use: when a temporary test schema is created, it should be added to the
   * list `self::$testSchemas[$schema_name] = TRUE;` and when it has been
   * removed by a test, its value should be set to FALSE, indicating it is ok if
   * it cannot be dropped again `self::$testSchemas[$schema_name] = FALSE;`.
   *
   * @var array
   */
  protected static $testSchemas = [];

  /**
   * A string indicating the name of the current chado test schema.
   */
  protected $testSchemaName = NULL;

  /**
   * A database connection.
   *
   * It should be set if not set in any test function that adds schema names to
   * $testSchemas: `self::$db = self::$db ?? \Drupal::database();`
   * This connection will be used during ::tearDownAfterClass when it will not
   * be possible to instantiate a new connection so it needs to be instantiated
   * before, when a test schema is created.
   *
   * @var \Drupal\Core\Database\Driver\pgsql\Connection
   */
  protected static $db = NULL;

  /**
   * Returns the chado cvterm_id for the term with the given ID space + accession.
   * This is completely independant of Tripal terms.
   */
  protected function getCvtermID($idspace, $accession) {

    $connection = $this->getTestSchema();

    $query = $connection->select('1:cvterm', 'cvt');
    $query->fields('cvt', ['cvterm_id']);
    $query->join('1:dbxref', 'dbx', 'cvt.dbxref_id = dbx.dbxref_id');
    $query->join('1:db', 'db', 'db.db_id = dbx.db_id');
    $query->condition('db.name', $idspace, '=');
    $query->condition('dbx.accession', $accession, '=');
    $result = $query->execute();

    return $result->fetchField();

  }

  /**
   * {@inheritdoc}
   */
  public static function tearDownAfterClass() :void {
    // Try to cleanup.
    if (isset(self::$db)) {
      $errors = [];
      foreach (self::$testSchemas as $test_schema => $in_use) {
        try {
          self::$db->query("DROP SCHEMA $test_schema CASCADE;");
        }
        catch (\Exception $e) {
          if ($in_use) {
            $errors[] =
              'Unable to remove temporary tests schema "'
              . $test_schema
              . '": ' . $e->getMessage()
            ;
          }
        }
      }
      if (!empty($errors)) {
        trigger_error(
          implode("\n", $errors),
          E_USER_WARNING
        );
      }
    }
  }

  /**
   * Get real config data.
   */
  protected function getRealConfig() {
    // Get original config from Drupal real installation.
    // This is done by getting a connection to the real database first.
    // Then instantiate a new config factory that will use that database through
    // a new instance of config storage using that database.
    // Get Drupal real database.
    $drupal_db = \Drupal\Core\Database\Database::getConnection(
      'default',
      'simpletest_original_default'
    );
    // Instantiate a new config storage.
    $config_storage = new \Drupal\Core\Config\DatabaseStorage(
      $drupal_db,
      'config'
    );
    // Get an event dispatcher.
    $event_dispatcher = \Drupal::service('event_dispatcher');
    // Get a typed config (note: this will use the test config storage).
    $typed_config = \Drupal::service('config.typed');
    // Instanciate a new config factory.
    $this->realConfigFactory = new \Drupal\Core\Config\ConfigFactory(
      $config_storage,
      $event_dispatcher,
      $typed_config
    );
  }

  /**
   * Initializes TripalDbx member.
   */
  protected function initTripalDbx() {
    $this->tripal_dbx = \Drupal::service('tripal.dbx');
    // Hack to clear TripalDbx cache on each run.
    $clear = function() {
      TripalDbx::$drupalSchema = NULL;
      TripalDbx::$reservedSchemaPatterns = NULL;
    };
    $clear->call(new TripalDbx());
    // Adds live schema reservation.
    $reserved_schema_patterns = $this->realConfigFactory->get('tripaldbx.settings')
      ->get('reserved_schema_patterns', [])
    ;
    foreach ($reserved_schema_patterns as $pattern => $description) {
      $this->tripal_dbx->reserveSchemaPattern($pattern, $description);
    }
  }

  /**
   * Allows a test to use reserved Chado test schema names.
   */
  protected function allowTestSchemas() {
    $this->testSchemaBaseNames = $this->realConfigFactory
      ->get('tripaldbx.settings')
      ->get('test_schema_base_names', [])
    ;
    $this->tripal_dbx->freeSchemaPattern(
      $this->testSchemaBaseNames['chado'],
      TRUE
    );
  }

  /**
   * Creates Chado installations table.
   */
  protected function createChadoInstallationsTable() {
    $db = \Drupal::database();
    if (!$db->schema()->tableExists('chado_installations')) {
      $db->schema()->createTable('chado_installations',
        [
          'fields' => [
            'install_id' => [
              'type' => 'serial',
              'unsigned' => TRUE,
              'not null' => TRUE,
            ],
            'schema_name' => [
              'type' => 'varchar',
              'length' => 255,
              'not null' => TRUE,
            ],
            'version' => [
              'type' => 'varchar',
              'length' => 255,
              'not null' => TRUE,
            ],
            'created' => [
              'type' => 'varchar',
              'length' => 255,
            ],
            'updated' => [
              'type' => 'varchar',
              'length' => 255,
            ],
          ],
          'indexes' => [
            'schema_name' => ['schema_name'],
          ],
          'primary key' => ['install_id'],
        ]
      );
    }
  }

  /**
   * Gets a new Chado schema for testing.
   * Retrieves the current test schema.
   * If there is not currently a test schema set-up then one will be created.
   *
   * @param int $init_level
   *   One of the constant to select the schema initialization level.
   *   If this is supplied then it forces a new connection to be made for
   *   backwards compatibility.
   *
   * @return \Drupal\tripal\TripalDBX\TripalDbxConnection
   *   A bio database connection using the generated schema.
   */
  protected function getTestSchema(int $init_level = NULL) {

    if ($init_level !== NULL) {
      return $this->createTestSchema($init_level);
    }
    elseif ($this->testSchemaName === NULL) {
      return $this->createTestSchema();
    }
    else {
      return new ChadoConnection($this->testSchemaName);
    }
  }

  /**
   * Creates a new Chado schema for testing.
   *
   * @param int $init_level
   *   One of the constant to select the schema initialization level.
   *
   * @return \Drupal\tripal\TripalDBX\TripalDbxConnection
   *   A bio database connection using the generated schema.
   */
  protected function createTestSchema(int $init_level = 0) {
    $schema_name = $this->testSchemaBaseNames['chado']
      . '_'
      . bin2hex(random_bytes(8))
    ;
    $tripaldbx_db = new ChadoConnection($schema_name);
    // Make sure schema is free.
    if ($tripaldbx_db->schema()->schemaExists()) {
      $this->markTestSkipped(
        "Failed to generate a free test schema ($schema_name)."
      );
    }
    switch ($init_level) {
      case static::INIT_CHADO_DUMMY:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        $success = $tripaldbx_db->executeSqlFile(
          __DIR__ . '/../../../chado_schema/chado-only-1.3.sql',
          ['chado' => $schema_name]);
        $this->assertTrue($success, 'Chado schema loaded.');

        $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/fill_chado_test_prepare.sql',
            ['chado' => $schema_name]);
        $this->assertTrue($success, 'Prepared chado records added.');

        $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/fill_chado.sql',
            ['chado' => $schema_name]);
        $this->assertTrue($success, 'Dummy Chado schema loaded.');
        $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');
        break;

      case static::INIT_CHADO_EMPTY:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        $success = $tripaldbx_db->executeSqlFile(
          __DIR__ . '/../../../chado_schema/chado-only-1.3.sql',
          ['chado' => $schema_name]);
        $this->assertTrue($success, 'Chado schema loaded.');
        $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');

        // Add version information to the schema so the tests don't fail.
        $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/version.sql',
            ['chado' => $schema_name]);
        $this->assertTrue($success, 'Chado version loaded.');
        break;

      case static::PREPARE_TEST_CHADO:
          $tripaldbx_db->schema()->createSchema();
          $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
          $success = $tripaldbx_db->executeSqlFile(
            __DIR__ . '/../../../chado_schema/chado-only-1.3.sql',
            ['chado' => $schema_name]);
          $this->assertTrue($success, 'Chado schema loaded.');
          $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');

          // Add version information to the schema so the tests don't fail.
          $success = $tripaldbx_db->executeSqlFile(__DIR__ . '/../../fixtures/fill_chado_test_prepare.sql',
              ['chado' => $schema_name]);
          $this->assertTrue($success, 'Prepared chado records added.');
          break;

      case static::INIT_DUMMY:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        $success = $tripaldbx_db->executeSqlFile(
          __DIR__ . '/../../fixtures/test_schema.sql',
          'none'
        );
        $this->assertTrue($success, 'Dummy schema loaded.');
        $this->assertGreaterThan(100, $tripaldbx_db->schema()->getSchemaSize(), 'Test schema not empty.');
        break;

      case static::CREATE_SCHEMA:
        $tripaldbx_db->schema()->createSchema();
        $this->assertTrue($tripaldbx_db->schema()->schemaExists(), 'Test schema created.');
        break;

      case static::SCHEMA_NAME_ONLY:
        break;

      default:
        break;
    }
    self::$db = self::$db ?? \Drupal::database();
    self::$testSchemas[$schema_name] = TRUE;
    $this->testSchemaName = $schema_name;

    // Make sure that any other connections to TripalDBX will see this new test schema as
    // the default schema.
    $config = \Drupal::service('config.factory')->getEditable('tripal_chado.settings');
    $config->set('default_schema', $schema_name)->save();

    // As a safety check, make sure that the tripalDBX object is using the test schema.
    // We don't want to perform tests in a live schema.
    $this->assertTrue($tripaldbx_db->getSchemaName() == $schema_name, 'TripalDBX is not using the test schema.');

    // Set this to be the Chado connection used in the current test schema.
    $container = \Drupal::getContainer();
    $container->set('tripal_chado.database', $tripaldbx_db);

    return $tripaldbx_db;
  }

  /**
   * Removes a Chado test schema and keep track it has been removed correctly.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $tripaldbx_db
   *   A bio database connection using the test schema.
   */
  protected function freeTestSchema(
    \Drupal\tripal\TripalDBX\TripalDbxConnection $tripaldbx_db
  ) {
    self::$testSchemas[$tripaldbx_db->getSchemaName()] = FALSE;
    try {
      $tripaldbx_db->schema()->dropSchema();
    }
    catch (\Exception $e) {
      // Ignore issues.
    }
  }

}
