<?php
namespace Drupal\Tests\tripal_chado\Functional;


use Drupal\tripal_biodb\Database\BioDbTool;
use Drupal\tripal_chado\Database\ChadoConnection;


trait ChadoTestTrait  {
   
  /**
   * Bio database tool instance.
   */
  protected $bioTool;
  
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
   * A database connection.
   *
   * It should be set if not set in any test function that adds schema names to
   * $testSchemas: `self::$db = self::$db ?? \Drupal::database();`
   * This connection will be used during ::tearDownAfterClass when it will not
   * be possible to instanciate a new connection so it needs to be instanciated
   * before, when a test schema is created.
   *
   * @var \Drupal\Core\Database\Driver\pgsql\Connection
   */
  protected static $db = NULL;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Init Tripal.
    $this->createChadoInstallationsTable();

    // Get config.
    $this->getRealConfig();

    // BioDbTool.
    $this->initBioDbTool();

    // Allow test schemas.
    $this->allowTestSchemas();
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
    // Instanciate a new config storage.
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
   * Initializes BioDbTool member.
   */
  protected function initBioDbTool() {
    $this->bioTool = \Drupal::service('tripal_biodb.tool');
    // Hack to clear BioDbTool cache on each run.
    $clear = function() {
      BioDbTool::$drupalSchema = NULL;
      BioDbTool::$reservedSchemaPatterns = NULL;
    };
    $clear->call(new BioDbTool());
    // Adds live schema reservation.
    $reserved_schema_patterns = $this->realConfigFactory->get('tripal_biodb.settings')
      ->get('reserved_schema_patterns', [])
    ;
    foreach ($reserved_schema_patterns as $pattern => $description) {
      $this->bioTool->reserveSchemaPattern($pattern, $description);
    }
  }

  /**
   * Allows a test to use reserved Chado test schema names.
   */
  protected function allowTestSchemas() {
    $this->testSchemaBaseNames = $this->realConfigFactory
      ->get('tripal_biodb.settings')
      ->get('test_schema_base_names', [])
    ;
    $this->bioTool->freeSchemaPattern(
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
   *
   * @param int $init_level
   *   One of the constant to select the schema initialization level.
   *
   * @return \Drupal\tripal_biodb\Database\BioConnection
   *   A bio database connection using the generated schema.
   */
  protected function getTestSchema(int $init_level = 0) {
    $schema_name = $this->testSchemaBaseNames['chado']
      . '_'
      . bin2hex(random_bytes(8))
    ;
    $biodb = new ChadoConnection($schema_name);
    // Make sure schema is free.
    if ($biodb->schema()->schemaExists()) {
      $this->markTestSkipped(
        "Failed to generate a free test schema ($schema_name)."
      );
    }
    switch ($init_level) {
      case static::INIT_CHADO_DUMMY:
        $biodb->schema()->createSchema();
        $this->assertTrue($biodb->schema()->schemaExists(), 'Test schema created.');
        $success = $biodb->executeSqlFile(
          __DIR__ . '/../../../chado_schema/chado-only-1.3.sql',
          ['chado' => $schema_name]
        );
        $this->assertTrue($success, 'Chado schema loaded.');
        $success = $biodb->executeSqlFile(
          __DIR__ . '/../../fixtures/fill_chado.sql',
          'none'
        );
        $this->assertTrue($success, 'Dummy Chado schema loaded.');
        $this->assertGreaterThan(100, $biodb->schema()->getSchemaSize(), 'Test schema not empty.');
        break;

      case static::INIT_CHADO_EMPTY:
        $biodb->schema()->createSchema();
        $this->assertTrue($biodb->schema()->schemaExists(), 'Test schema created.');
        $success = $biodb->executeSqlFile(
          __DIR__ . '/../../../chado_schema/chado-only-1.3.sql',
          ['chado' => $schema_name]
        );
        $this->assertTrue($success, 'Chado schema loaded.');
        $this->assertGreaterThan(100, $biodb->schema()->getSchemaSize(), 'Test schema not empty.');
        break;

      case static::INIT_DUMMY:
        $biodb->schema()->createSchema();
        $this->assertTrue($biodb->schema()->schemaExists(), 'Test schema created.');
        $success = $biodb->executeSqlFile(
          __DIR__ . '/../../fixtures/test_schema.sql',
          'none'
        );
        $this->assertTrue($success, 'Dummy schema loaded.');
        $this->assertGreaterThan(100, $biodb->schema()->getSchemaSize(), 'Test schema not empty.');
        break;

      case static::CREATE_SCHEMA:
        $biodb->schema()->createSchema();
        $this->assertTrue($biodb->schema()->schemaExists(), 'Test schema created.');
        break;

      case static::SCHEMA_NAME_ONLY:
        break;

      default:      
        break;
    }
    self::$db = self::$db ?? \Drupal::database();
    self::$testSchemas[$schema_name] = TRUE;

    return $biodb;
  }

  /**
   * Removes a Chado test schema and keep track it has been removed correctly.
   *
   * @param \Drupal\tripal_biodb\Database\BioConnection $biodb
   *   A bio database connection using the test schema.
   */
  protected function freeTestSchema(
    \Drupal\tripal_biodb\Database\BioConnection $biodb
  ) {
    self::$testSchemas[$biodb->getSchemaName()] = FALSE;
    try {
      $biodb->schema()->dropSchema();
    }
    catch (\Exception $e) {
      // Ignore issues.
    }
  }

}
