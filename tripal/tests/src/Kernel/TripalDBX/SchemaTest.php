<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\TripalDBX\TripalDbxSchema;
use Drupal\tripal\TripalDBX\TripalDbxConnection;

/**
 * Tests for Tripal DBX schema on a real database.
 *
 * @see https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Schema.php/class/Schema/9.2.x
 *
 * @coversDefaultClass \Drupal\tripal\TripalDBX\TripalDbxSchema
 *
 * @group Tripal
 * @group Tripal TripalDBX
 * @group Tripal TripalDBX Schema
 */
class SchemaTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal'];

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
   * @var \Drupal\Core\Database\Connection
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
      ?? ['default' => '_test_tdbx', ]
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
    $this->proConfigFactory->get('tripaldbx.settings')->willReturn($this->config);
    $this->configFactory = $this->proConfigFactory->reveal();

    \Drupal::getContainer()->set('config.factory', $this->configFactory);
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
   * Builds an initialized TripalDbxSchema mock.
   *
   * @param $database_or_schema_name
   *  Either a \Drupal\tripal\TripalDBX\TripalDbxConnection object (or mock) or
   *  a schema name to use.
   *
   */
  protected function getTripalDbxSchemaMock($database_or_schema_name) {
    if (is_string($database_or_schema_name)) {
      $tdbx = $this->getMockBuilder(\Drupal\tripal\TripalDBX\TripalDbxConnection::class)
        ->setConstructorArgs([$database_or_schema_name])
        ->getMockForAbstractClass()
      ;
    }
    else {
      $tdbx = $database_or_schema_name;
    }
    // Create a mock for the abstract class.
    $scmock = $this->getMockBuilder(\Drupal\tripal\TripalDBX\TripalDbxSchema::class)
      ->setConstructorArgs([$tdbx])
      ->getMockForAbstractClass()
    ;

    // Return initialized mock.
    return $scmock;
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
   * Tests constructor.
   *
   * @cover ::__construct
   */
  public function testTripalDbxSchemaConstructor() {
    $scmock = $this->getTripalDbxSchemaMock('test');
    $this->assertNotNull($scmock->getSchemaSize(), 'No size.');
  }

  /**
   * Tests getPrefixInfo.
   *
   * @cover ::getPrefixInfo
   */
  public function testTripalDbxSchemaPrefixInfo() {
    $this->allowTestSchemas();
    $test_schema_base_name = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')['default']
    ;
    $scmock = $this->getTripalDbxSchemaMock($test_schema_base_name);

    // Hack to bypass protected restriction.
    $getPrefixInfo = function($table = 'default', $add_prefix = TRUE) {return $this->getPrefixInfo($table, $add_prefix);};
    $prefix_info = $getPrefixInfo->call($scmock, 'something');
    $this->assertEquals(
      [
        'schema' => $test_schema_base_name,
        'table' => 'something',
        'prefix' => $test_schema_base_name . '.',
      ],
      $prefix_info,
      'Prefix ok.'
    );
  }

  /**
   * Test a scenario.
   *
   * @cover ::getSchemaName
   * @cover ::schemaExists
   * @cover ::createSchema
   * @cover ::getSchemaSize
   * @cover ::findTables
   * @cover ::tableExists
   * @cover ::fieldExists
   * @cover ::indexExists
   * @cover ::constraintExists
   * @cover ::primaryKeyExists
   * @cover ::foreignKeyConstraintExists
   * @cover ::sequenceExists
   * @cover ::functionExists
   * @cover ::createTable
   * @cover ::renameTable
   * @cover ::changeField
   * @cover ::addIndex
   * @cover ::dropTable
   * @cover ::getTables
   * @cover ::getTableDef
   * @cover ::getTableDdl
   * @cover ::renameSchema
   * @cover ::cloneSchema
   * @cover ::dropSchema
   */
  public function testTripalDbxSchemaScenario1() {
    $db = \Drupal::database();
    self::$db = self::$db ?? $db;
    $this->allowTestSchemas();
    $test_schema_base_names = \Drupal::config('tripaldbx.settings')
      ->get('test_schema_base_names')
    ;
    $sch_1 = $test_schema_base_names['default'] . mt_rand(10000000, 99999999);
    $sch_2 = $test_schema_base_names['default'] . mt_rand(10000000, 99999999);

    // Get abstract mock.
    $tdbx = $this->getMockBuilder(\Drupal\tripal\TripalDBX\TripalDbxConnection::class)
      ->setConstructorArgs([$sch_1])
      ->getMockForAbstractClass()
    ;
    $scmock = $this->getTripalDbxSchemaMock($tdbx);
    $schema_name = $scmock->getSchemaName();
    $this->assertEquals($sch_1, $schema_name, 'Schema name set.');

    // Check schema does not exist.
    $exists = $scmock->schemaExists();
    $this->assertFalse($exists, 'Schema does not exist.');

    // Create schema.
    $scmock->createSchema();
    self::$testSchemas[$sch_1] = TRUE;

    // Check schema exists.
    $exists = $scmock->schemaExists();
    $this->assertTrue($exists, 'Schema exists.');

    // Get initial size.
    $init_size = $scmock->getSchemaSize();
    $this->assertLessThan(1000, $init_size, 'New schema empty.');

    // Load test data fixture into test schema.
    $success = $tdbx->executeSqlFile(__DIR__ . '/../../../fixtures/test_schema.sql', 'none');
    $this->assertTrue($success, 'Schema test data loaded.');

    // Get new size.
    $new_size = $scmock->getSchemaSize();
    $this->assertGreaterThan($init_size, $new_size, 'New schema filled.');

    // Search for tables.
    // https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/function/Schema%3A%3AfindTables/9.2.x
    $found = $scmock->findTables('%test%');
    $this->assertNotEmpty($found, 'Tables found.');
    $this->assertEquals(3, count($found), '2 tables and 1 view found.');

    // Exists.
    // https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/function/Schema%3A%3AtableExists/9.2.x
    $exists = $scmock->tableExists('testtable');
    $this->assertTrue($exists, 'Table "testtable" exists.');

    $exists = $scmock->tableExists('testtableabc');
    $this->assertFalse($exists, 'Table "testtableabc" does not exist.');

    // https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/function/Schema%3A%3AfieldExists/9.2.x
    $exists = $scmock->fieldExists('testtable', 'fieldreal');
    $this->assertTrue($exists, 'Field "testtable.fieldreal" exists.');

    $exists = $scmock->fieldExists('testtable', 'fieldrealabc');
    $this->assertFalse($exists, 'Field "testtable.fieldrealabc" does not exist.');

    // https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/function/Schema%3A%3AindexExists/9.2.x
    $exists = $scmock->indexExists('testtable', 'testtable_idx1', TRUE);
    $this->assertTrue($exists, 'Index "testtable_idx1" exists.');

    $exists = $scmock->indexExists('testtable', 'testtable_idx', TRUE);
    $this->assertFalse($exists, 'Index "testtable_idx" does not exist.');

    // Constraint exists.
    $exists = $scmock->constraintExists('testtable', 'testtable_c1', 'unique');
    $this->assertTrue($exists, 'Constraint "testtable_c1" exists.');

    $exists = $scmock->constraintExists('testtable', 'testtable_c42', 'unique');
    $this->assertFalse($exists, 'Constraint "testtable_c42" does not exist.');

    // Primary key exists.
    $exists = $scmock->primaryKeyExists('testtable', 'id');
    $this->assertTrue($exists, '"testtable.id" is a primary key.');

    $exists = $scmock->primaryKeyExists('testtable', 'foreign_id');
    $this->assertFalse($exists, '"testtable.foreign_id" is not a primary key.');

    // Foreign key exists.
    $exists = $scmock->foreignKeyConstraintExists('testtable', 'foreign_id');
    $this->assertTrue($exists, '"testtable.foreign_id" has a foreign key constraint.');

    $exists = $scmock->foreignKeyConstraintExists('testtable', 'id');
    $this->assertFalse($exists, '"testtable.id" has not a foreign key constraint.');

    // Sequence exists.
    $exists = $scmock->sequenceExists('testtable', 'id', $sequence_name);
    $this->assertTrue($exists, 'A sequence exists on "testtable.id".');
    $this->assertEquals('testtable_id_seq', $sequence_name, 'Got the sequence name.');

    $exists = $scmock->sequenceExists(NULL, NULL, $sequence_name);
    $this->assertTrue($exists, 'The sequence exists.');

    $sequence_name .= 'abc';
    $exists = $scmock->sequenceExists(NULL, NULL, $sequence_name);
    $this->assertFalse($exists, 'Sequence does not exist.');
    $exists = $scmock->sequenceExists('testtable', 'fieldbigint', $sequence_name);
    $this->assertFalse($exists, 'No sequence on field "testtable.fieldbigint".');

    // Function exists.
    $exists = $scmock->functionExists('dummy', ['bigint']);
    $this->assertTrue($exists, 'Function "dummy(bigint)" exists.');

    $exists = $scmock->functionExists('dummy', ['']);
    $this->assertFalse($exists, 'Function "dummy()" does not exist.');

    /**
     These tests do not work in a mocked setting.
     However, when tested manually with the Chado implementation
     they do work so we're commenting them out for now.

    // Create a table.
    $scmock->createTable(
          'table_1',
          [
            "fields" => [
              "thing" => [
                "type" => "text",
                "not null" => TRUE,
                "pgsql_type" => "integer",
              ],
            ],
          ]
        );
    $exists = $scmock->tableExists('table_1');
    $this->assertTrue($exists, 'Table "table_1" was created.');

    // Rename table.
    $scmock->renameTable('table_1', 'table_1_renamed');
    $exists = $scmock->tableExists('table_1');
    $this->assertFalse($exists, 'Table "table_1" renamed into something else as it no longer exists.');
    $exists = $scmock->tableExists('table_1_renamed');
    $this->assertTrue($exists, 'Table "table_1_renamed" is the new table name since it does exist.');

    // Change field.
    $scmock->changeField(
      'table_1_renamed',
      'thing',
      'thing_renamed',
      [
        "type" => "text",
        "not null" => FALSE,
        "pgsql_type" => "bigint",
      ]
    );
    $exists = $scmock->fieldExists('table_1_renamed', 'thing_renamed');
    $this->assertTrue($exists, 'Field "table_1_renamed.thing_renamed" exists.');

    // Add an index.
    $scmock->addIndex(
      'table_1_renamed',
      'table_1_renamed_thing_renamed',
      ['thing_renamed'],
      [
        "fields" => [
          "thing_renamed" => [
            "type" => "text",
            "not null" => TRUE,
            "pgsql_type" => "bigint",
          ],
        ],
      ]
    );
    $exists = $scmock->indexExists('table_1_renamed', 'table_1_renamed_thing_renamed');
    $this->assertTrue($exists, 'Index "table_1_renamed_thing_renamed__idx" exists.');

    // Drop Table
    $success = $scmock->dropTable('table_1_renamed');
    $this->assertTrue($success, 'Table "table_1_renamed" dropped.');
    $exists = $scmock->tableExists('table_1_renamed');
    $this->assertFalse($exists, 'Table "table_1_renamed" does not exist.');
    */

    // Get tables.
    $tables = $scmock->getTables(['table']);
    $this->assertEquals(2, count($tables), 'Got the right number of tables.');

    // Get table definitions.
    $table_def = $scmock->getTableDef('testtable', ['source' => 'database']);
    $this->assertNotEmpty($table_def, 'Got a table definition.');
    $test_schema = $scmock->getSchemaName();
    $expected = [
      'columns' => [
        'id' => [
          'type'     => 'integer',
          'not null' => TRUE,
          'default'  => "nextval('testtable_id_seq'::regclass)",
        ],
        'foreign_id' => [
          'type'     => 'integer',
          'not null' => NULL,
          'default'  => NULL,
        ],
        'fieldbigint' => [
          'type'     => 'bigint',
          'not null' => NULL,
          'default'  => NULL,
        ],
        'fieldsmallint' => [
          'type'     => 'smallint',
          'not null' => NULL,
          'default'  => NULL,
        ],
        'fieldbool' => [
          'type'     => 'boolean',
          'not null' => TRUE,
          'default'  => 'false',
        ],
        'fieldreal' => [
          'type'     => 'real',
          'not null' => NULL,
          'default'  => '1.0',
        ],
        'fielddouble' => [
          'type' => 'double precision',
          'not null' => NULL,
          'default' => NULL,
        ],
        'fieldchar' => [
          'type'     => 'character varying(255)',
          'not null' => NULL,
          'default'  => NULL,
        ],
        'fieldtext' => [
          'type'     => 'text',
          'not null' => TRUE,
          'default'  => NULL,
        ],
        'fieldbytea' => [
          'type'     => 'bytea',
          'not null' => NULL,
          'default'  => "'x'::bytea",
        ],
      ],
      'constraints' => [
        'testtable_pkey'            => "PRIMARY KEY (id)",
        'testtable_c1'              => "UNIQUE (fieldbigint, fieldsmallint)",
        'testtable_foreign_id_fkey' => "FOREIGN KEY (foreign_id) REFERENCES othertesttable(id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED",
      ],
      'indexes' => [
        'testtable_c1' => [
          'query' => "CREATE UNIQUE INDEX testtable_c1 ON $test_schema.testtable USING btree (fieldbigint, fieldsmallint);",
          'name'  => "testtable_c1",
          'table' => "$test_schema.testtable",
          'using' => "btree (fieldbigint, fieldsmallint)",
        ],
        'testtable_c2' => [
          'query' => "CREATE UNIQUE INDEX testtable_c2 ON $test_schema.testtable USING btree (fieldbigint, fieldsmallint);",
          'name'  => "testtable_c2",
          'table' => "$test_schema.testtable",
          'using' => "btree (fieldbigint, fieldsmallint)",
        ],
        'testtable_idx1' => [
          'query' => "CREATE INDEX testtable_idx1 ON $test_schema.testtable USING btree (foreign_id);",
          'name'  => "testtable_idx1",
          'table' => "$test_schema.testtable",
          'using' => "btree (foreign_id)",
        ],
      ],
      'dependencies' => [
        'othertesttable' => ['foreign_id' => 'id',],
      ],
      'comment' => "Some long description\non multiple lines.",
      'referenced_by' => [
        'othertesttable' => ['id' => 'fk',],
      ],
    ];
    $this->assertEquals(
      $expected,
      $table_def,
      'Table definition ok.'
    );

    // DDL.
    $expected =
"CREATE TABLE $test_schema.testtable (
  id integer NOT NULL DEFAULT nextval('testtable_id_seq'::regclass),
  foreign_id integer NULL,
  fieldbigint bigint NULL,
  fieldsmallint smallint NULL,
  fieldbool boolean NOT NULL DEFAULT false,
  fieldreal real NULL DEFAULT 1.0,
  fielddouble double precision NULL,
  fieldchar character varying(255) NULL,
  fieldtext text NOT NULL,
  fieldbytea bytea NULL DEFAULT 'x'::bytea,
  CONSTRAINT testtable_pkey PRIMARY KEY (id),
  CONSTRAINT testtable_c1 UNIQUE (fieldbigint, fieldsmallint),
  CONSTRAINT testtable_foreign_id_fkey FOREIGN KEY (foreign_id) REFERENCES othertesttable(id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED
);
CREATE UNIQUE INDEX testtable_c1 ON $test_schema.testtable USING btree (fieldbigint, fieldsmallint);
CREATE UNIQUE INDEX testtable_c2 ON $test_schema.testtable USING btree (fieldbigint, fieldsmallint);
CREATE INDEX testtable_idx1 ON $test_schema.testtable USING btree (foreign_id);
COMMENT ON TABLE $test_schema.testtable IS 'Some long description
on multiple lines.';
";
    $table_ddl = $scmock->getTableDdl('testtable');
    $this->assertNotEmpty($table_ddl, 'Got a table DDL.');
    $this->assertEquals($expected, $table_ddl, 'Table DDL ok.');

    // Schema renaming.
    $scmock->renameSchema($sch_2);
    $schema_name = $scmock->getSchemaName();
    $this->assertEquals($sch_2, $schema_name, 'Schema renamed.');
    self::$testSchemas[$sch_1] = FALSE;
    self::$testSchemas[$sch_2] = TRUE;

    // Cloning.
    /*
    $scmock_clone = $this->getTripalDbxSchemaMock($sch_1);
    $exists = $scmock_clone->schemaExists();
    $this->assertFalse($exists, 'First schema is now free.');
    $clone_size = $scmock_clone->cloneSchema($sch_2);
    $this->assertGreaterThan(1000, $clone_size, 'Schema cloned.');
    $exists = $scmock_clone->schemaExists();
    $this->assertTrue($exists, 'Clone exists.');
    self::$testSchemas[$sch_1] = TRUE;
    */

    $scmock->dropSchema();
    $exists = $scmock->schemaExists();
    $this->assertFalse($exists, 'Second schema removed.');
    self::$testSchemas[$sch_2] = FALSE;

    /*
    $scmock_clone->dropSchema();
    $exists = $scmock_clone->schemaExists();
    $this->assertFalse($exists, 'First schema removed.');
    self::$testSchemas[$sch_1] = FALSE;
    */
  }
}
