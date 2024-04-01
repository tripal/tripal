<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\TripalDBX\TripalDbxConnection;

/**
 * Tests for Tripal DBX connection on a real database.
 *
 * @coversDefaultClass \Drupal\tripal\TripalDBX\TripalDbxConnection
 *
 * @group Tripal
 * @group Tripal TripalDBX
 * @group Tripal TripalDBX Connection
 */
class ConnectionTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

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
    // Instantiate a new config factory.
    $config_factory = new \Drupal\Core\Config\ConfigFactory(
      $config_storage,
      $event_dispatcher,
      $typed_config
    );
    // Get real config elements.
    $config = $config_factory->get('tripaldbx.settings');
    $reserved_schema_patterns = $config->get('reserved_schema_patterns');
    $this->assertNotEmpty($reserved_schema_patterns, 'Reserved schema patterns not empty.');
    $test_schema_base_names = $config
      ->get('test_schema_base_names')
      ?? ['default' => '_test_tripaldbx', ]
    ;

    // Mock the Config object.
    $this->proConfig = $this->prophesize(\Drupal\Core\Config\ImmutableConfig::class);
    $this->proConfig->get('reserved_schema_patterns')->willReturn(
      [
        // Added when the module is installed.
        'public' => 'Drupal installation',
        // Added from config YAML but removed for tests.
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
    $this->proConfigFactory->get('tripaldbx.settings')->willReturn($this->config);
    $this->configFactory = $this->proConfigFactory->reveal();

    \Drupal::getContainer()->set('config.factory', $this->configFactory);
  }

  /**
   * Builds an initialized TripalDbxConnection mock.
   *
   * @cover ::__construct
   */
  protected function getConnectionMock(
    $schema_name = '',
    $database = 'default',
    $logger = NULL
  ) {
    // Create a mock for the abstract class.
    $dbmock = $this->getMockBuilder(\Drupal\tripal\TripalDBX\TripalDbxConnection::class)
      ->setConstructorArgs([$schema_name, $database, $logger])
      ->onlyMethods(['getTripalDbxClass', 'findVersion', 'getAvailableInstances'])
      ->getMockForAbstractClass();

    $dbmock
      ->expects($this->any())
      ->method('getTripalDbxClass')
      ->with('Schema')
      ->willReturn('\Drupal\Tests\tripal\Kernel\TripalDBX\Subclass\TripalDbxSchemaFake');

    // Return initialized mock.
    return $dbmock;
  }

  /**
   * Allow a test to use reserved default test schema names.
   */
  protected function allowTestSchemas() {
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $tripaldbx = \Drupal::service('tripal.dbx');
    $tripaldbx->freeSchemaPattern($test_schema_base_names['default'], TRUE);
  }

  /**
   * Tests constructor: check constructor calls.
   *
   * @cover ::__construct
   * @cover ::getDatabaseName
   * @cover ::getDatabaseKey
   * @cover ::getMessageLogger
   */
  public function testConnectionConstructorAllDefault() {
    // Create a mock for the abstract class.
    $dbmock = $this->getMockBuilder(\Drupal\tripal\TripalDBX\TripalDbxConnection::class)
      ->disableOriginalConstructor()
      ->onlyMethods(['setTarget', 'setKey', 'setSchemaName', 'findVersion'])
      ->getMockForAbstractClass()
    ;

    $dbmock->expects($this->once())
      ->method('setTarget')
      ->with($this->equalTo('default'))
    ;
    $dbmock->expects($this->once())
      ->method('setKey')
      ->with($this->equalTo('default'))
    ;
    $dbmock->expects($this->once())
      ->method('setSchemaName')
      ->with($this->equalTo(''))
    ;
    $dbmock
      ->expects($this->any())
      ->method('findVersion')
      ->willReturn('');

    // Call the constructor.
    $reflected_class = new \ReflectionClass(\Drupal\tripal\TripalDBX\TripalDbxConnection::class);
    $constructor = $reflected_class->getConstructor();
    $constructor->invoke($dbmock);

    // Check default values.
    $this->assertEquals('', $dbmock->getSchemaName(), 'No schema name.');
    $this->assertInstanceOf('Drupal\tripal\Services\TripalLogger', $dbmock->getMessageLogger(), 'Logger.');
    $this->assertNotEmpty($dbmock->getDatabaseName(), 'Database name.');
    $this->assertEquals('default', $dbmock->getDatabaseKey(), 'Database key');
    $this->assertEquals('', $dbmock->getVersion(), 'No version.');
    $this->assertEquals('', $dbmock->getQuotedSchemaName(), 'No quoted schema name.');
    $this->assertStringEndsWith('\TripalDbxSchema', $dbmock->getTripalDbxClass('Schema'), 'Schema class.');
  }

  /**
   * Tests constructor: test schema, default key.
   *
   * @cover ::__construct
   */
  public function testConnectionConstructorTestSchemaDefaultKey() {
    $dbmock = $this->getConnectionMock('test');
    $this->assertEquals('test', $dbmock->getSchemaName(), 'Schema name.');
    $this->assertEquals('"test"', $dbmock->getQuotedSchemaName(), 'Quoted schema name.');
  }

  /**
   * Tests constructor: no schema, default database.
   *
   * @cover ::__construct
   * @cover ::getDatabaseName
   */
  public function testConnectionConstructorNoSchemaDefaultDb() {
    $db = \Drupal::database();
    $dbmock = $this->getConnectionMock('', $db);
    $this->assertEquals('', $dbmock->getSchemaName(), 'Schema name.');
    $this->assertEquals('', $dbmock->getQuotedSchemaName(), 'Quoted schema name.');
    $this->assertNotEmpty($dbmock->getDatabaseName(), 'Database name.');
  }

  /**
   * Tests constructor: test schema, default database.
   *
   * @cover ::__construct
   * @cover ::getDatabaseName
   */
  public function testConnectionConstructorTestSchemaDefaultDb() {
    $db = \Drupal::database();
    $dbmock = $this->getConnectionMock('test', $db);
    $this->assertEquals('test', $dbmock->getSchemaName(), 'Schema name.');
    $this->assertEquals('"test"', $dbmock->getQuotedSchemaName(), 'Quoted schema name.');
    $this->assertNotEmpty($dbmock->getDatabaseName(), 'Database name.');
  }

  /**
   * Tests constructor: special character schema, default key.
   *
   * @cover ::__construct
   * @cover ::getDatabaseName
   */
  public function testConnectionConstructorSpecialSchemaDefaultKey() {
    $dbmock = $this->getConnectionMock('voilà');
    $this->assertEquals('voilà', $dbmock->getSchemaName(), 'Schema name.');
    $this->assertEquals('"voilà"', $dbmock->getQuotedSchemaName(), 'Quoted schema name.');
    $this->assertNotEmpty($dbmock->getDatabaseName(), 'Database name.');
  }

  /**
   * Tests constructor: public schema, default key.
   *
   * @cover ::__construct
   */
  public function testConnectionConstructorPublicSchemaDefaultKey() {
    // Now schema can use reserved schema name as long as they are valid.
    // This is to allow working on reserved schemas without messing up with
    // schema name reservations. We assume that the schema name provided has
    // been checked before for reservation through methods
    // TripalDbx::isSchemaReserved (or TripalDbx::isInvalidSchemaName with
    // $ignore_reservation = FALSE).
    // $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    // $this->expectExceptionMessage('reserved');

    $schema_name = 'public';
    $tripaldbx = \Drupal::service('tripal.dbx');
    $issue = $tripaldbx->isInvalidSchemaName($schema_name);
    $this->assertNotEmpty($issue, 'Reserved schema name not allowed.');
    // But the connection should be created.
    $dbmock = $this->getConnectionMock($schema_name);
    $this->assertEquals($schema_name, $dbmock->getSchemaName(), 'Connection instantiated.');
  }

  /**
   * Tests constructor: reserved schema, default key.
   *
   * @cover ::__construct
   */
  public function testConnectionConstructorReservedSchemaDefaultKey() {
    // Now schema can use reserved schema name as long as they are valid.
    // This is to allow working on reserved schemas without messing up with
    // schema name reservations. We assume that the schema name provided has
    // been checked before for reservation through methods
    // TripalDbx::isSchemaReserved (or TripalDbx::isInvalidSchemaName with
    // $ignore_reservation = FALSE).
    // $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    // $this->expectExceptionMessage('reserved');

    // The schema name should be invalid because it is reserved.
    $schema_name = '_test_new';
    $tripaldbx = \Drupal::service('tripal.dbx');
    $issue = $tripaldbx->isInvalidSchemaName($schema_name);
    $this->assertNotEmpty($issue, 'Reserved schema name not allowed.');
    // But the connection should be created.
    $dbmock = $this->getConnectionMock($schema_name);
    $this->assertEquals($schema_name, $dbmock->getSchemaName(), 'Connection instantiated.');
  }

  /**
   * Tests constructor: invalid schema, default key.
   *
   * @cover ::__construct
   */
  public function testConnectionConstructorInvalidSchemaDefaultKey() {
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('must not');
    $dbmock = $this->getConnectionMock('0test');
  }

  /**
   * Tests constructor: test schema, invalid database.
   *
   * @cover ::__construct
   */
  public function testConnectionConstructorTestSchemaInvalidDatabase() {
    $mocked_mysqldb = $this->getMockBuilder(\Drupal\Core\Database\Driver\mysql\Connection::class)
      ->disableOriginalConstructor()
      ->getMock()
    ;
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('PostgreSQL');
    $dbmock = $this->getConnectionMock('test', $mocked_mysqldb);
  }

  /**
   * Tests constructor: test schema, invalid key.
   *
   * @cover ::__construct
   */
  public function testConnectionConstructorTestSchemaInvalidKey() {
    $this->expectException(\Drupal\Core\Database\ConnectionNotDefinedException::class);
    $dbmock = $this->getConnectionMock('test', 'someunexistingdatabasekey');
  }

  /**
   * Tests constructor: test schema, secondary database key.
   *
   * @cover ::__construct
   * @cover ::getDatabaseKey
   */
  public function testConnectionConstructorTestSchemaSecondaryKey() {
    // Create a secondary connection on-the-fly that clones the default one.
    $db = \Drupal::database();
    $options = $db->getConnectionOptions();
    \Drupal\Core\Database\Database::addConnectionInfo('secondary', 'default', $options);
    $dbmock = $this->getConnectionMock('test', 'secondary');
    $this->assertEquals($dbmock->getDatabaseKey(), 'secondary');
  }

  /**
   * Tests constructor: test search_path isolation.
   *
   * @cover ::__construct
   */
  public function testConnectionConstructorSearchPath() {
    $db = \Drupal::database();
    $dbmock = $this->getConnectionMock('test', $db);
    $sql = "SELECT setting FROM pg_settings WHERE name = 'search_path';";
    $search_path_drupal = $db->query($sql)->fetch()->setting;
    $search_path_tdbx = $dbmock->query($sql)->fetch()->setting;
    $this->assertNotEquals($search_path_drupal, $search_path_tdbx, 'Different search paths.');
    $tripaldbx = \Drupal::service('tripal.dbx');
    $drupal_schema = $tripaldbx->getDrupalSchemaName();
    $this->assertRegexp('/^test\W/', $search_path_tdbx, 'TripalDbx search_path has test schema.');
    $this->assertRegexp("/,\\s*$drupal_schema(?:\W|$)/", $search_path_tdbx, 'TripalDbx search_path has Drupal schema as well.');
    $this->assertNotRegexp('/(?:^|\W)test(?:\W|$)/', $search_path_drupal, 'Drupal search_path has not test schema.');
    $this->assertRegexp("/(?:^|\\W)$drupal_schema(?:\W|$)/", $search_path_drupal, 'Drupal search_path has Drupal schema.');
  }

  /**
   * Tests ::schema  when no schema was set.
   *
   * @cover ::schema
   */
  public function testSchemaNoSchema() {
    $dbmock = $this->getConnectionMock('');
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\SchemaException::class);
    $this->expectExceptionMessage('schema');
    $dbmock->schema();
  }

  /**
   * Tests schema name changes with TripalDbxSchema object.
   *
   * @cover ::schema
   * @cover ::setSchemaName
   */
  public function testSchemaChange() {
    $schema_name = 'first';
    $dbmock = $this->getConnectionMock($schema_name);
    $schema = $dbmock->schema();
    $this->assertNotNull($schema, 'Got a first schema.');
    $internal_schema = $schema->getDefaultSchema();
    $this->assertEquals($schema_name, $dbmock->getSchemaName(), 'TripalDbxConnection schema 1 correct.');
    $this->assertEquals($schema_name, $internal_schema, 'TripalDbxSchema schema 1 correct.');

    $schema_name = 'second';
    $dbmock->setSchemaName($schema_name);
    $schema = $dbmock->schema();
    $this->assertNotNull($schema, 'Got a second schema.');
    $internal_schema = $schema->getDefaultSchema();
    $this->assertEquals($schema_name, $dbmock->getSchemaName(), 'TripalDbxConnection schema 2 correct.');
    $this->assertEquals($schema_name, $internal_schema, 'TripalDbxSchema schema 2 correct.');

    $dbmock->setSchemaName('');
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\SchemaException::class);
    $this->expectExceptionMessage('schema');
    $schema = $dbmock->schema();
  }

  /**
   * Tests schema name changes impacts on other members and methods.
   *
   * @cover ::schema
   * @cover ::setSchemaName
   * @cover ::getVersion
   * @cover ::findVersion
   * @cover ::getQuotedSchemaName
   * @cover ::addExtraSchema
   * @cover ::getExtraSchemas
   * @cover ::prefixTables
   */
  public function testSchemaNameChangeImpacts() {
    $drupal_prefix = $this->get_drupal_prefix();

    $dbmock = $this->getConnectionMock('first');
    // Manages fake versions. First schema would be 42 and next 806.
    $dbmock
      ->expects($this->exactly(2))
      ->method('findVersion')
      ->will($this->onConsecutiveCalls('42', '806'))
    ;

    $version = $dbmock->getVersion();
    $this->assertEquals('42', $version, 'Version set.');
    // The next call should not call findVersion and use cached version.
    $version = $dbmock->getVersion();
    $this->assertEquals('42', $version, 'Version in cache. No ::findVersion call.');
    $quoted_name = $dbmock->getQuotedSchemaName();
    $this->assertEquals('"first"', $quoted_name, 'Quoted schema name.');
    $extra_index = $dbmock->addExtraSchema('other');
    $this->assertEquals(2, $extra_index, 'Extra schema index number.');
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([2 => 'other'], $extra_schemas, 'Extra schemas.');

    $prefix_test = $dbmock->prefixTables(
      'X {drupal_table}, {0:drupal_table2}, {1:tdbx1_table}, {2:tdbx2_table}, {1:tdbx1_table2}'
    );

    $this->assertEquals(
      'X "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2", "first"."tdbx1_table", "other"."tdbx2_table", "first"."tdbx1_table2"',
      $prefix_test,
      'Correct table prefixing X.'
    );

    // Version member is set so any schema change must reset that version.
    $dbmock->setSchemaName('deuxième');
    // Now, findVersion should be called again a second time.
    $version = $dbmock->getVersion();
    $this->assertEquals('806', $version, 'New version set.');
    $quoted_name = $dbmock->getQuotedSchemaName();
    $this->assertEquals('"deuxième"', $quoted_name, 'Second quoted schema name.');

    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([], $extra_schemas, 'No more extra schemas.');
    $prefix_test = $dbmock->prefixTables(
      'Y {drupal_table}, {0:drupal_table2}, {1:tdbx1_table}, {1:tdbx1_table2}'
    );
    $this->assertEquals(
      'Y "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2", "deuxième"."tdbx1_table", "deuxième"."tdbx1_table2"',
      $prefix_test,
      'Correct table prefixing Y.'
    );
  }

  /**
   * Tests ::addExtraSchema with no Tripal DBX schema.
   *
   * @cover ::addExtraSchema
   */
  public function testAddExtraSchemaNoSchema() {
    $drupal_prefix = $this->get_drupal_prefix();
    $dbmock = $this->getConnectionMock();

    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('No current schema');
    $prefix_test = $dbmock->addExtraSchema('toto');
  }

  /**
   * Tests ::setExtraSchema with index 0.
   *
   * @cover ::setExtraSchema
   */
  public function testSetExtraSchemaZero() {
    $drupal_prefix = $this->get_drupal_prefix();
    $dbmock = $this->getConnectionMock();

    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('Invalid extra schema index');
    $prefix_test = $dbmock->setExtraSchema('toto', 0);
  }

  /**
   * Tests ::setExtraSchema with index 1.
   *
   * @cover ::setExtraSchema
   */
  public function testSetExtraSchemaOne() {
    $drupal_prefix = $this->get_drupal_prefix();
    $dbmock = $this->getConnectionMock();

    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('Invalid extra schema index');
    $prefix_test = $dbmock->setExtraSchema('toto', 1);
  }

  /**
   * Tests ::prefixTables with no Tripal DBX schema.
   *
   * @cover ::prefixTables
   */
  public function testPrefixNoSchema() {
    $drupal_prefix = $this->get_drupal_prefix();
    $dbmock = $this->getConnectionMock();

    $prefix_test = $dbmock->prefixTables(
      'X {drupal_table}, {0:drupal_table2}'
    );
    $this->assertEquals(
      'X "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2"',
      $prefix_test,
      'Correct table prefixing X.'
    );

    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('No schema set for this connection');
    $prefix_test = $dbmock->prefixTables(
      'Y {drupal_table}, {0:drupal_table2}, {1:tdbx1_table}'
    );
  }

  /**
   * Tests ::prefixTables with a Tripal DBX schema but no extra.
   *
   * @cover ::prefixTables
   */
  public function testPrefixNoExtraSchema() {
    $drupal_prefix = $this->get_drupal_prefix();
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();

    $sch_1 = $test_schema_base_names['default'] . '_a';

    // Try with 1 schema.
    $dbmock = $this->getConnectionMock($sch_1);
    // Test prefixing without that schema.
    $prefix_test = $dbmock->prefixTables(
      'X {drupal_table}, {0:drupal_table2}'
    );
    $this->assertEquals(
      'X "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2"',
      $prefix_test,
      'Correct table prefixing X.'
    );

    // Test prefixing with the Tripal DBX schema.
    $prefix_test = $dbmock->prefixTables(
      'Y {drupal_table}, {0:drupal_table2}, {1:tdbx1_table}'
    );
    $this->assertEquals(
      'Y "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2", "'
      . $sch_1
      . '"."tdbx1_table"',
      $prefix_test,
      'Correct table prefixing Y.'
    );

    // Test prefixing with an unexisting/not set extra schema.
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('Invalid schema');
    $prefix_test = $dbmock->prefixTables(
      'Z {drupal_table}, {0:drupal_table2}, {1:tdbx1_table}, {2:tdbx2_table}, {1:tdbx1_table2}'
    );
  }

  /**
   * Tests scenario with a Tripal DBX schema and 2 extra.
   *
   * @cover ::prefixTables
   * @cover ::addExtraSchema
   * @cover ::setExtraSchema
   */
  public function testConnectionScenario1() {
    $drupal_prefix = $this->get_drupal_prefix();
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();

    $sch_1 = $test_schema_base_names['default'] . '_a';
    $sch_2 = $test_schema_base_names['default'] . '_b';
    $sch_3 = $test_schema_base_names['default'] . '_c';

    // Try with 3 schemas.
    $dbmock = $this->getConnectionMock($sch_1);
    $dbmock->addExtraSchema($sch_2);
    $dbmock->addExtraSchema($sch_3);

    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([2 => $sch_2, 3 => $sch_3], $extra_schemas, 'Extra schemas set.');

    // Test prefixing without a schema.
    $prefix_test = $dbmock->prefixTables(
      'X {drupal_table}, {0:drupal_table2}'
    );
    $this->assertEquals(
      'X "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2"',
      $prefix_test,
      'Correct table prefixing X.'
    );

    // Test prefixing with the default Tripal DBX schema.
    $prefix_test = $dbmock->prefixTables(
      'Y {drupal_table}, {0:drupal_table2}, {1:tdbx1_table}'
    );
    $this->assertEquals(
      'Y "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2", "'
      . $sch_1
      . '"."tdbx1_table"',
      $prefix_test,
      'Correct table prefixing Y.'
    );

    // Test prefixing with the 2 extra schema.
    $prefix_test = $dbmock->prefixTables(
      'Z {drupal_table}, {0:drupal_table2}, {3:tdbx3_table}, {2:tdbx2_table}, {1:tdbx1_table2}'
    );
    $this->assertEquals(
      'Z "'
      . $drupal_prefix
      . 'drupal_table", "'
      . $drupal_prefix
      . 'drupal_table2", "'
      . $sch_3
      . '"."tdbx3_table", "'
      . $sch_2
      . '"."tdbx2_table", "'
      . $sch_1
      . '"."tdbx1_table2"',
      $prefix_test,
      'Correct table prefixing Z.'
    );

    // Try setting an extra schema with too high index.
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('Invalid extra schema index');
    $dbmock->setExtraSchema('toto', 5);
  }

  /**
   * Tests scenario with a Tripal DBX schema and 2 extra.
   *
   * @cover ::addExtraSchema
   * @cover ::setExtraSchema
   * @cover ::clearExtraSchemas
   */
  public function testConnectionScenario2() {
    $drupal_prefix = $this->get_drupal_prefix();
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();

    $sch_1 = $test_schema_base_names['default'] . '_a';
    $sch_2 = $test_schema_base_names['default'] . '_b';
    $sch_3 = $test_schema_base_names['default'] . '_c';
    $sch_4 = $test_schema_base_names['default'] . '_d';

    // Try with schemas changes.
    $dbmock = $this->getConnectionMock($sch_1);
    $dbmock->setExtraSchema($sch_2, 2);

    // Replaces previous.
    $dbmock->setExtraSchema($sch_3, 2);
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([2 => $sch_3], $extra_schemas, 'Extra schemas replaced.');

    // Add a new one.
    $extra_index3 = $dbmock->addExtraSchema($sch_4);
    $this->assertEquals(3, $extra_index3, 'Extra schemas with correct index.');
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([2 => $sch_3, 3 => $sch_4], $extra_schemas, 'Extra schemas added.');

    // Replace first one again.
    $dbmock->setExtraSchema($sch_2, 2);
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([2 => $sch_2, 3 => $sch_4], $extra_schemas, 'Extra schemas replaced again.');

    // Add one and replace second one.
    $dbmock->setExtraSchema($sch_4, 4);
    $dbmock->setExtraSchema($sch_3, 3);
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([2 => $sch_2, 3 => $sch_3, 4 => $sch_4], $extra_schemas, 'Extra schemas replaced once more.');

    // Clear extra schema.
    $dbmock->clearExtraSchemas();
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([], $extra_schemas, 'Extra schemas cleared.');

    $extra_index = $dbmock->addExtraSchema($sch_2);
    $this->assertEquals(2, $extra_index, 'Extra schemas with restarted index.');

    // Try setting an extra schema with too high index.
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('Invalid extra schema index');
    $dbmock->setExtraSchema($sch_4, 4);
  }

  /**
   * Tests scenario for prefixTables with extra schema modified.
   *
   * @cover ::prefixTables
   * @cover ::addExtraSchema
   * @cover ::setExtraSchema
   */
  public function testConnectionScenario3() {
    $drupal_prefix = $this->get_drupal_prefix();
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();

    $sch_1 = $test_schema_base_names['default'] . '_a';
    $sch_2 = $test_schema_base_names['default'] . '_b';
    $sch_3 = $test_schema_base_names['default'] . '_c';
    $sch_4 = $test_schema_base_names['default'] . '_d';

    // Try with 3 schemas.
    $dbmock = $this->getConnectionMock($sch_1);
    $dbmock->addExtraSchema($sch_2);
    // Using set with default index to 2, so it will replace previous schema.
    $dbmock->setExtraSchema($sch_3);
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([2 => $sch_3], $extra_schemas, 'Extra schemas set.');

    // Test prefixing with the extra schema.
    $prefix_test = $dbmock->prefixTables(
      'X {2:tdbx2_table}, {1:tdbx1_table}, {2:tdbx2_table2}, {1:tdbx1_table2}'
    );
    $this->assertEquals(
      'X "'
      . $sch_3
      . '"."tdbx2_table", "'
      . $sch_1
      . '"."tdbx1_table", "'
      . $sch_3
      . '"."tdbx2_table2", "'
      . $sch_1
      . '"."tdbx1_table2"',
      $prefix_test,
      'Correct table prefixing X.'
    );

    // Add a new one.
    $extra_index = $dbmock->addExtraSchema($sch_4);
    $this->assertEquals(3, $extra_index, 'Extra schemas with correct index.');

    // Test prefixing with the extra schema.
    $prefix_test = $dbmock->prefixTables(
      'Y {2:tdbx2_table}, {1:tdbx1_table}, {3:tdbx3_table}, {1:tdbx1_table2}'
    );
    $this->assertEquals(
      'Y "'
      . $sch_3
      . '"."tdbx2_table", "'
      . $sch_1
      . '"."tdbx1_table", "'
      . $sch_4
      . '"."tdbx3_table", "'
      . $sch_1
      . '"."tdbx1_table2"',
      $prefix_test,
      'Correct table prefixing Y.'
    );

    // Clear extra schema.
    $dbmock->clearExtraSchemas();
    $extra_schemas = $dbmock->getExtraSchemas();
    $this->assertEquals([], $extra_schemas, 'Extra schemas cleared.');

    // Test prefixing with an unexisting/not set extra schema.
    $this->expectException(\Drupal\tripal\TripalDBX\Exceptions\ConnectionException::class);
    $this->expectExceptionMessage('Invalid schema');
    $prefix_test = $dbmock->prefixTables(
      'Z {drupal_table}, {0:drupal_table2}, {1:tdbx1_table}, {2:tdbx2_table}, {1:tdbx1_table2}'
    );
  }

  /**
   * Tests ::tablePrefix.
   *
   * @cover ::tablePrefix
   */
  public function testTablePrefix() {
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();
    $sch_1 = $test_schema_base_names['default'] . '_a';
    $dbmock = $this->getConnectionMock($sch_1);
    $result = $dbmock->tablePrefix();
    $this->assertNotEmpty($result, 'Drupal test database prefix.');
    $this->assertNotEquals($sch_1 . '.', $result, 'Prefix for regular tables not in Tripal DBX schema.');

    $result2 = $dbmock->tablePrefix('whatever');
    $this->assertEquals($result, $result2, 'Prefix for regular tables stable.');

    $result2 = $dbmock->tablePrefix('whatever', TRUE);
    $this->assertNotEquals($result, $result2, 'Prefix for biological tables different from Drupal test database.');
    $this->assertEquals($sch_1 . '.', $result2, 'Prefix for biological tables.');
  }

  /**
   * Tests ::__toString.
   *
   * @cover ::__toString
   */
  public function testToString() {
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();
    $sch_1 = $test_schema_base_names['default'] . '_a';
    $dbmock = $this->getConnectionMock($sch_1);
    $dbname = $dbmock->getDatabaseName();
    $text = ''.$dbmock;
    $this->assertEquals("$dbname.$sch_1", $text);

    $dbmock = $this->getConnectionMock();
    $text = ''.$dbmock;
    $this->assertEquals("$dbname.", $text);
  }

  /**
   * Tests ::executeSqlQueries.
   *
   * @cover ::executeSqlQueries
   * @cover ::query
   */
  public function testExecuteSqlQueries() {
    // Get a test schema.
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();
    $sch_1 = $test_schema_base_names['default'] . '_a';
    $dbmock = $this->getConnectionMock($sch_1);

    // Queries to check schema and table.
    $schema_exists_sql = "SELECT 1 FROM pg_namespace WHERE nspname = '$sch_1';";
    $table_exists_sql = "SELECT 1 FROM pg_tables WHERE schemaname = '$sch_1' AND tablename = 'someothertable';";

    if (!empty($dbmock->query($schema_exists_sql)->fetch())) {
      $this->markTestSkipped(
        "The test schema '$sch_1' already exists and cannot be used for testing."
      );
      return;
    }

    $table_not_exists = $dbmock->schema()->tableExists('someothertable');
    $this->assertFalse($table_not_exists, "Test table 'someothertable' does not exist.");

    // Create schema and use more than one SQL statement in one string.
    $sql = "START TRANSACTION;CREATE SCHEMA $sch_1;SELECT TRUE; CREATE TABLE someothertable (\n  id serial NOT NULL,\n  CONSTRAINT othertable_pkey PRIMARY KEY (id)\n);COMMIT;";
    $success = $dbmock->executeSqlQueries($sql);
    $schema_exists = !empty($dbmock->query($schema_exists_sql)->fetch());
    $table_exists = !empty($dbmock->query($table_exists_sql)->fetch());
    $sql = "DROP TABLE someothertable; DROP SCHEMA $sch_1 CASCADE;";
    $success2 = $dbmock->executeSqlQueries($sql);
    $table_not_exists = !empty($dbmock->query($table_exists_sql)->fetch());
    $schema_not_exists = !empty($dbmock->query($schema_exists_sql)->fetch());

    $this->assertTrue($success, 'SQL queries run.');
    $this->assertTrue($schema_exists, 'Test schema created.');
    $this->assertTrue($table_exists, "Test table 'someothertable' created.");
    $this->assertTrue($success, 'SQL cleaning queries run.');
    $this->assertFalse($table_not_exists, "Test table 'someothertable' removed.");
    $this->assertFalse($schema_not_exists, 'Test schema removed.');
  }

  /**
   * Tests ::executeSqlQueries with force search_path.
   *
   * @cover ::executeSqlQueries
   * @cover ::query
   */
  public function testExecuteSqlQueriesForceSearchPath() {
    // Get a test schema.
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();
    $sch_1 = $test_schema_base_names['default'] . '_a';
    $sch_2 = $test_schema_base_names['default'] . '_b';
    $dbmock = $this->getConnectionMock($sch_1);

    // Queries to check schema and table.
    $schema_exists_sql = "SELECT 1 FROM pg_namespace WHERE nspname = '$sch_1';";
    $schema2_exists_sql = "SELECT 1 FROM pg_namespace WHERE nspname = '$sch_2';";
    $table_exists_sql = "SELECT 1 FROM pg_tables WHERE schemaname = '$sch_1' AND tablename = 'someothertable';";
    $table2_exists_sql = "SELECT 1 FROM pg_tables WHERE schemaname = '$sch_2' AND tablename = 'someothertable2';";

    if (!empty($dbmock->query($schema_exists_sql)->fetch())
        && !empty($dbmock->query($schema2_exists_sql)->fetch())
    ) {
      $this->markTestSkipped(
        "The test schema '$sch_1' or '$sch_2' already exists and cannot be used for testing."
      );
      return;
    }

    $table_not_exists = $dbmock->schema()->tableExists('someothertable');
    $this->assertFalse($table_not_exists, "Test table 'someothertable' does not exist.");

    // Create schema and use more than one SQL statement in one string.
    $sql = "START TRANSACTION;CREATE SCHEMA $sch_1;CREATE SCHEMA $sch_2;SET search_path=$sch_2;-- A comment.\nSELECT TRUE;\n   set  SEARCH_PATH =  non_existing,$sch_2,public;\nCREATE TABLE someothertable (\n  id serial NOT NULL,\n  CONSTRAINT othertable_pkey PRIMARY KEY (id)\n);\nSET search_path=$sch_2;--KEEP\nCREATE TABLE someothertable2();COMMIT;";
    $success = $dbmock->executeSqlQueries($sql, 'none');
    $this->assertTrue($success, 'SQL queries run.');
    $schema_exists = !empty($dbmock->query($schema_exists_sql)->fetch());
    $schema2_exists = !empty($dbmock->query($schema2_exists_sql)->fetch());
    $table_exists = !empty($dbmock->query($table_exists_sql)->fetch());
    $table2_exists = !empty($dbmock->query($table2_exists_sql)->fetch());

    $sql = "DROP TABLE someothertable; DROP SCHEMA $sch_1 CASCADE; DROP SCHEMA $sch_2 CASCADE;";
    $success2 = $dbmock->executeSqlQueries($sql);
    $table_not_exists = !empty($dbmock->query($table_exists_sql)->fetch());
    $schema_not_exists = !empty($dbmock->query($schema_exists_sql)->fetch());
    $schema2_not_exists = !empty($dbmock->query($schema2_exists_sql)->fetch());

    $this->assertTrue($schema_exists, 'Test schema created.');
    $this->assertTrue($schema2_exists, 'Test schema 2 created.');
    $this->assertTrue($table_exists, "Test table 'someothertable' created.");
    $this->assertTrue($table2_exists, "Test table 2 'someothertable2' created at the right place.");
    $this->assertTrue($success, 'SQL cleaning queries run.');
    $this->assertFalse($table_not_exists, "Test table 'someothertable' removed.");
    $this->assertFalse($schema_not_exists, 'Test schema removed.');
    $this->assertFalse($schema2_not_exists, 'Test schema 2 removed.');

  }

  /**
   * Tests ::executeSqlQueries with force search_path.
   *
   * @cover ::executeSqlQueries
   * @cover ::query
   */
  public function testExecuteSqlFile() {
    // Get a test schema.
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $this->allowTestSchemas();
    $sch_1 = $test_schema_base_names['default'] . '_a';
    $dbmock = $this->getConnectionMock($sch_1);
    $schema_exists_sql = "SELECT 1 FROM pg_namespace WHERE nspname = '$sch_1';";
    $table_exists_sql = "SELECT 1 FROM pg_tables WHERE schemaname = '$sch_1' AND tablename = 'testtable';";


    $dbmock->query("CREATE SCHEMA $sch_1;");
    $schema_exists = !empty($dbmock->query($schema_exists_sql)->fetch());
    $this->assertTrue($schema_exists, "Schema $sch_1 created.");

    try {
      // Execute SQL file.
      $success = $dbmock->executeSqlFile(__DIR__ . '/../../../fixtures/test_schema.sql', 'none');
      $table_exists = !empty($dbmock->query($table_exists_sql)->fetch());
    }
    catch (\Exception $e) {
      // Drop test schema.
      $dbmock->query("DROP SCHEMA $sch_1 CASCADE;");
      throw $e;
    }
    // Drop test schema.
    $dbmock->query("DROP SCHEMA $sch_1 CASCADE;");

    $this->assertTrue($success, 'SQL file run.');
    $this->assertTrue($table_exists, "Test table 'testtable' created.");
    $schema_not_exists = !empty($dbmock->query($schema_exists_sql)->fetch());
    $this->assertFalse($schema_not_exists, "Schema $sch_1 dropped.");
  }

  /**
   * HELPER: Retrieve the Drupal table prefix for the current site.
   */
  protected function get_drupal_prefix() {
    $database_options = \Drupal::database()->getConnectionOptions();

    $drupal_prefix = '';
    if (array_key_exists('prefix', $database_options)) {
      $drupal_prefix = $database_options['prefix'];
      if (is_array($drupal_prefix)) {
        if (array_key_exists('default', $drupal_prefix)) {
          $drupal_prefix = $drupal_prefix['default'];
        }
        else {
          $drupal_prefix = 'cannot_determine';
        }
      }
    }
    $drupal_prefix = str_replace('.', '"."', $drupal_prefix);

    return $drupal_prefix;
  }

}
