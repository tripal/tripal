<?php

namespace Drupal\Tests\tripal_biodb\Functional\Database;

use Drupal\KernelTests\KernelTestBase;
use Drupal\tripal_biodb\Database\BioDbTool;

/**
 * Tests for biological database tool on a real database.
 *
 * @coversDefaultClass \Drupal\tripal_biodb\Database\BioDbTool
 *
 * @group Tripal
 * @group Tripal BioDb
 * @group Tripal BioDb Tool
 */
class BioDbToolFunctionalTest extends KernelTestBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal_biodb'];

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
   *
   * @var \Drupal\Core\Database\Driver\pgsql\Connection
   */
  protected static $db = NULL;

  /**
   * Test members.
   *
   * "pro*" members are prophesize objects while their "non-pro*" equivqlent are
   * the revealed objects.
   */
  protected $proConfigFactory;
  protected $configFactory;
  protected $proConfig;
  protected $config;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Get original config from Drupal real installation.
    // This is done by getting a connection to the real database first.
    // Then instanciate a new config factory that will use that database through
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
    $config_factory = new \Drupal\Core\Config\ConfigFactory(
      $config_storage,
      $event_dispatcher,
      $typed_config
    );
    // Get real config elements.
    $config = $config_factory->get('tripal_biodb.settings');
    $reserved_schema_patterns = $config->get('reserved_schema_patterns');
    $this->assertNotEmpty($reserved_schema_patterns, 'Reserved schema patterns not empty.');
    $test_schema_base_names = $config
      ->get('test_schema_base_names')
      ?? ['default' => '_test_biodb', ]
    ;

    // Mock the Config object.
    $this->proConfig = $this->prophesize(\Drupal\Core\Config\ImmutableConfig::class);
    $this->proConfig->get('reserved_schema_patterns')->willReturn(
      [
        // Added when the module is installed.
        'public' => 'Drupal installation',
        // Added from config YAML.
        '_test*' => 'testing purposes',
      ]
      + $reserved_schema_patterns
    );
    $this->proConfig->get('test_schema_base_names')->willReturn(
      $test_schema_base_names
    );
    $this->config = $this->proConfig->reveal();

    // Mock the ConfigFactory service.
    $this->proConfigFactory = $this->prophesize(\Drupal\Core\Config\ConfigFactory::class);
    $this->proConfigFactory->get('tripal_biodb.settings')->willReturn($this->config);
    $this->configFactory = $this->proConfigFactory->reveal();

    \Drupal::getContainer()->set('config.factory', $this->configFactory);

    // Hack to clear BioDbTool cache on each run.
    $clear = function() {BioDbTool::$drupalSchema = NULL;};
    $clear->call(new BioDbTool());
  }

  /**
   * {@inheritdoc}
   */
  public static function tearDownAfterClass() :void {
    // Try to cleanup.
    if (isset(self::$db)) {
      $errors = [];
      foreach (self::$testSchemas as $test_schema => $value) {
        try {
          self::$db->query("DROP SCHEMA $test_schema CASCADE;");
        }
        catch (\Exception $e) {
          if ($value) {
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
   * Tests getDrupalSchemaName() method.
   *
   * @cover ::getDrupalSchemaName
   */
  public function testGetDrupalSchemaNameReal() {
    // Get Drupal schema.
    $bio_tool = new BioDbTool();
    $drupal_schema = $bio_tool->getDrupalSchemaName();
    $this->assertNotEmpty($drupal_schema, 'Got a schema name.');
  }

  /**
   * Tests isInvalidSchemaName() method.
   *
   * @cover ::isInvalidSchemaName
   */
  public function testIsInvalidSchemaNameReal() {
    // Get Drupal schema.
    $bio_tool = new BioDbTool();
    $drupal_schema = $bio_tool->getDrupalSchemaName();
    $invalid = $bio_tool->isInvalidSchemaName($drupal_schema);
    $this->assertNotEmpty($invalid, 'Drupal schema name is reserved.');

    $valid = $bio_tool->isInvalidSchemaName('aschema');
    $this->assertEquals('', $valid, 'A regular schema name is allowed.');
  }

  /**
   * Tests schemaExists() method.
   *
   * @cover ::schemaExists
   */
  public function testSchemaExistsReal() {

    $bio_tool = new BioDbTool();
    $exists = $bio_tool->schemaExists('public');
    $this->assertTrue($exists, 'Schema exists.');

    $exists = $bio_tool->schemaExists('xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx');
    $this->assertFalse($exists, 'Schema does not exist.');
  }

  /**
   * Tests schemaExists() method.
   *
   * @cover ::schemaExists
   * @cover ::createSchema
   * @cover ::renameSchema
   * @cover ::cloneSchema
   * @cover ::dropSchema
   * @cover ::getDatabaseSize
   * @cover ::getSchemaSize
   */
  public function testSchemaManagementScenario1() {

    $bio_tool = new BioDbTool();
    $db = \Drupal::database();
    self::$db = self::$db ?? $db;

    // Clear all reserved patterns.
    $bio_tool->freeSchemaPattern('.*', TRUE);

    // Get test schema  base name for BioDb from (real) settings (see ::setUp).
    $test_schema_base_names = \Drupal::config('tripal_biodb.settings')
      ->get('test_schema_base_names')
    ;

    // Choose a test schema.
    $test_schema = $test_schema_base_names['default'] . mt_rand(10000000, 99999999);
    self::$testSchemas[$test_schema] = TRUE;
    $test_schema2 = $test_schema_base_names['default'] . mt_rand(10000000, 99999999);
    self::$testSchemas[$test_schema2] = TRUE;

    // Make sure our random test schema does not exist.
    $exists = $bio_tool->schemaExists($test_schema);
    $this->assertFalse($exists, 'Test schema does not exist.');

    // Create schema.
    $bio_tool->createSchema($test_schema);
    $exists = $bio_tool->schemaExists($test_schema);
    $this->assertTrue($exists, 'Test schema created.');

    // Rename schema.
    $bio_tool->renameSchema($test_schema, $test_schema2);
    $exists = $bio_tool->schemaExists($test_schema);
    $this->assertFalse($exists, 'Test schema has been renamed.');
    $exists = $bio_tool->schemaExists($test_schema2);
    $this->assertTrue($exists, 'Test schema 2 is the new test schema.');

    // Test size.
    $ini_size = $bio_tool->getSchemaSize($test_schema);
    $this->assertEquals(0, $ini_size, 'Test schema does not exist and has a size of 0.');
    $ini_size = $bio_tool->getSchemaSize($test_schema2);
    $db_size = $bio_tool->getDatabaseSize($db);
    $this->assertGreaterThan(1000, $db_size, 'Database has a size.');

    $sql = "CREATE TABLE $test_schema2.toto (x int);";
    $ok = $db->query($sql);
    $this->assertNotEmpty($ok, 'Table created in tests schema 2.');
    $sql = "INSERT INTO $test_schema2.toto SELECT * FROM generate_series(0, 100000);";
    $ok = $db->query($sql);
    $this->assertNotEmpty($ok, 'Table created in tests schema 2.');

    $new_size = $bio_tool->getSchemaSize($test_schema2);
    $this->assertGreaterThan($ini_size, $new_size, 'Test schema 2 has grown.');

    // Clone schema.
    $bio_tool->cloneSchema($test_schema2, $test_schema);
    $exists = $bio_tool->schemaExists($test_schema);
    $this->assertTrue($exists, 'Test schema 2 has been cloned into test schema.');
    $exists = $bio_tool->schemaExists($test_schema2);
    $this->assertTrue($exists, 'Test schema 2 still exist.');

    // Drop schema.
    $bio_tool->dropSchema($test_schema);
    $exists = $bio_tool->schemaExists($test_schema);
    $this->assertFalse($exists, 'Test schema removed.');
    $bio_tool->dropSchema($test_schema2);
    $exists = $bio_tool->schemaExists($test_schema2);
    $this->assertFalse($exists, 'Test schema 2 removed.');

    self::$testSchemas[$test_schema] = FALSE;
    self::$testSchemas[$test_schema2] = FALSE;
  }

}
