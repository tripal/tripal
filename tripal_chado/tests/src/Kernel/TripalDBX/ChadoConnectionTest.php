<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX;

use Drupal\tripal_chado\Database\ChadoSchema;
use Drupal\Core\Database\DatabaseExceptionWrapper;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal\TripalDBX\Exceptions\ConnectionException;
use Drupal\tripal\TripalDBX\TripalDbxConnection;
use Drupal\tripal\TripalDBX\TripalDbxSchema;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\CoversMethod;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests for ChadoConnection.
 *
 * @group Tripal Chado
 * @group TripalDBX
 * @group ChadoDBX
 * @group ChadoConnection
 * @covers \Drupal\tripal\TripalDBX\TripalDbxConnection
 * @covers \Drupal\tripal\TripalDBX\TripalDbxSchema
 * @covers \Drupal\tripal_chado\Database\ChadoConnection::getAvailableInstances
 * @covers \Drupal\tripal_chado\Database\ChadoConnection::findVersion
 * @covers \Drupal\tripal_chado\Database\ChadoConnection::removeAllTestSchemas
 */
#[CoversClass(TripalDbxConnection::class)]
#[CoversClass(TripalDbxSchema::class)]
#[CoversMethod(ChadoConnection::class, 'getAvailableInstances')]
#[CoversMethod(ChadoConnection::class, 'findVersion')]
#[CoversMethod(ChadoConnection::class, 'removeAllTestSchemas')]
#[Group('tripal-dbx')]
#[Group('chado')]
class ChadoConnectionTest extends ChadoTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'user', 'views', 'path', 'tripal', 'tripal_chado'];

  /**
   * Lists all supported versions of chado.
   *
   * @var array
   */
  protected static array $supported_chado_versions = [
    '1.3',
    '1.3.3.001',
    '1.3.3.002',
    '1.3.3.003',
    '1.3.3.004',
    '1.3.3.005',
    '1.3.3.006',
    '1.3.3.007',
    '1.3.3.008',
    '1.3.3.009',
    '1.3.3.011',
    '1.3.3.013',
  ];

  /**
   * Lists init levels that are worth testing for schema actions.
   *
   * @var array
   */
  protected static array $init_levels = [
  // self::$INIT_CHADO_EMPTY,.
    3,
  // self::$INIT_CHADO_DUMMY,.
    4,
  // self::$PREPARE_TEST_CHADO,.
    5,
  ];

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Install the settings for tripal chado which includes the default chado.
    // Note: This gets rid of the following kernel test error when installing
    // the tripal_chado config. We also need system + views in the module list.
    // Note2: We would normally install the chado_installations schema as well
    // but this is already done by the parent::setUp().
    $this->installSchema('tripal_chado', ['tripal_custom_tables']);

    // Now we can install the config needed for the default database setting.
    $this->installConfig('tripal_chado');
  }

  /**
   * Provides each supported version of chado to the tests.
   *
   * @return array
   *   Each scenario is just a version of chado as a string.
   */
  public static function provideChadoSchemaVersions() {
    $scenarios = [];

    foreach (self::$supported_chado_versions as $version_string) {
      $scenarios[] = [
        (string) $version_string,
      ];
    }

    return $scenarios;
  }

  /**
   * Provides each supported version of chado to the tests.
   *
   * @return array
   *   Each scenario is a unique chado version and init level combination.
   */
  public static function provideChadoSchemaVersionsAcrossInitLevels() {
    $scenarios = [];

    foreach (self::$supported_chado_versions as $version_string) {
      foreach (self::$init_levels as $init_level) {
        $scenarios[] = [
          (string) $version_string,
          $init_level,
        ];
      }
    }

    return $scenarios;
  }

  /**
   * Tests ChadoConnection::getAvailableInstances() across chado versions.
   *
   * @dataProvider provideChadoSchemaVersionsAcrossInitLevels
   *
   * @param string $version
   *   The version of chado to test against.
   * @param int $init_level
   *   The init level to create the test database with.
   */
  #[DataProvider('provideChadoSchemaVersionsAcrossInitLevels')]
  public function testGetAvailableInstances(string $version, int $init_level) {

    // Get Chado in place.
    $chado_connection = $this->getTestSchema(
      $init_level,
      $version
    );
    $this->assertInstanceOf(
      'Drupal\tripal_chado\Database\ChadoConnection',
      $chado_connection,
      "Unable to create test chado with the specified version (i.e. $version)."
    );

    // Ensure the version returned by ChadoConnection matches what we asked for.
    $this->assertEquals(
      $version,
      $chado_connection->getVersion(),
      "We expect that the chado version returned by the connection matches what we requested."
    );

    $instances = $chado_connection->getAvailableInstances();
    $this->assertIsArray(
      $instances,
      "We expect ChadoConnection::getAvailableInstances() will always return an array."
    );
    // Each instance returned should be fully described. Lets check the keys
    // to confirm. The value in the following array is an array of types that
    // the value of that key might be.
    $expected_keys = [
      'schema_name' => ['string'],
      'version' => ['string'],
      'is_default' => ['boolean'],
      'is_test' => ['boolean', 'string'],
      'is_reserved' => ['boolean'],
      'has_data' => ['boolean'],
      'size' => ['integer'],
      'integration' => ['boolean', 'array'],
    ];
    foreach ($instances as $instance_key => $retrieved_instance) {
      foreach ($expected_keys as $key_to_check => $value_types) {
        $this->assertArrayHasKey(
          $key_to_check,
          $retrieved_instance,
          "Each instance returned by ChadoConnection::getAvailableInstances() should have the '$key_to_check' indicated but '$instance_key' does not."
        );
        $this->assertContains(
          gettype($retrieved_instance[$key_to_check]),
          $value_types,
          "The value for ['$instance_key']['$key_to_check'] as returned by ChadoConnection::getAvailableInstances() is not what is expected. The value is: " . print_r($retrieved_instance[$key_to_check], TRUE)
        );
      }
    }

    // Next lets check our current test schema is there and that it is indicated
    // to be a test schema.
    $test_schema_name = $chado_connection->getSchemaName();
    $this->assertArrayHasKey(
      $test_schema_name,
      $instances,
      "Our current test schema should be in the list of instances returned by ChadoConnection::getAvailableInstances()."
    );
    $test_schema_instance = $instances[$test_schema_name];
    $this->assertEquals(
      $version,
      $test_schema_instance['version'],
      "The version returned for our test schema using ChadoConnection::getAvailableInstances() did not match what we created it as."
    );
    $this->assertEquals(
      'chado',
      $test_schema_instance['is_test'],
      "We expect our test instance returned by ChadoConnection::getAvailableInstances() to indicate that it is a test schema of chado."
    );

  }

  /**
   * Tests table prefixing by the ChadoConnection + TripalDbxConnection classes.
   *
   * @dataProvider provideChadoSchemaVersionsAcrossInitLevels
   *
   * NOTE:
   * In Drupal you can execute queries directly using CONNECTION->query()
   * or you can use the various query builders: CONNECTION->select(),
   * CONNECTION->update(), CONNECTION->merge(), CONNECTION->upsert(), CONNECTION->delete().
   *
   * This test is focusing on CONNECTION->query() since a code analysis shows
   * that the other options are simply preparing a query and then executing it
   * using CONNECTION->query().
   *
   * That said, at some point we may want to add additional tests to show that
   * the query builders are building queries appropriately but because these
   * are Drupal functionalities and our differences come during execution
   * and not at the query building stage, we are currently going to assume that
   * the Drupal testing is sufficient for the query builders.
   *
   * @param string $version
   *   The version of chado to test against.
   * @param int $init_level
   *   The init level to create the test database with.
   */
  #[DataProvider('provideChadoSchemaVersionsAcrossInitLevels')]
  public function testDefaultTablePrefixing(string $version, int $init_level) {
    $this->createTestSchema($init_level, $version);

    // Open a Connection to the default Tripal DBX managed Chado schema.
    $chado_connection = \Drupal::service('tripal_chado.database');
    $chado_1_prefix = $chado_connection->getSchemaName();

    // Create a situation where we should be using the core chado schema for our query.
    $query = $chado_connection->query("SELECT name, uniquename FROM {1:feature} LIMIT 1");
    $sqlStatement = $query->getQueryString();
    // We expect: "SCHEMAPREFIX"."feature" but since the quotes are not
    // necessary and could be interchanged by Drupal, we use the following pattern.
    $we_expect_pattern = str_replace('SCHEMAPREFIX', $chado_1_prefix, '/["\']+SCHEMAPREFIX["\']+\.["\']+feature["\']+/');
    $this->assertMatchesRegularExpression(
      $we_expect_pattern,
      $sqlStatement,
      "The sql statement does not have the table prefix we expect."
    );

    // Test the API realizes that chado is the default schema for this query.
    // We expect this to fail as the default database is chado unless Tripal DBX
    // is told otherwise.
    // NOTE: we use try/catch here so we can continue with our testing.
    // When using expectException the execution of all other assertions is skipped.
    try {
      $query = $chado_connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(TRUE, "We expect to have an exception thrown when TripalDBX incorrectly assumes the feature table is in Drupal, which it's not.");
    }

    // Now we want to tell Tripal DBX that the default schema for this query should be chado.
    // Using useTripalDbxSchemaFor():
    // ---------------------------------
    // PARENT CLASS: Let's check if it works when a parent class is white listed.
    $chado_connection = \Drupal::service('tripal_chado.database');
    $chado_connection->useTripalDbxSchemaFor(ChadoTestKernelBase::class);
    try {
      $query = $chado_connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "Now TripalDBX should know that chado is the default schema for this test class and it should not throw an exception.");
    }
    // We expect: "SCHEMAPREFIX"."feature" but since the quotes are not
    // necessary and could be interchanged by Drupal, we use the following pattern.
    $sqlStatement = $query->getQueryString();
    $we_expect_pattern = str_replace('SCHEMAPREFIX', $chado_1_prefix, '/["\']+SCHEMAPREFIX["\']+\.["\']+feature["\']+/');
    $this->assertMatchesRegularExpression(
      $we_expect_pattern,
      $sqlStatement,
      "The sql statement does not have the table prefix we expect."
    );

    // CURRENT CLASS: Let's test it works when the current class is whitelisted.
    $chado_connection = \Drupal::service('tripal_chado.database');
    $chado_connection->useTripalDbxSchemaFor(\Drupal\Tests\tripal_chado\Functional\ChadoConnectionTest::class);
    try {
      $query = $chado_connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "Now TripalDBX should know that chado is the default schema for this test class and it should not throw an exception.");
    }
    // We expect: "SCHEMAPREFIX"."feature" but since the quotes are not
    // necessary and could be interchanged by Drupal, we use the following pattern.
    $sqlStatement = $query->getQueryString();
    $we_expect_pattern = str_replace('SCHEMAPREFIX', $chado_1_prefix, '/["\']+SCHEMAPREFIX["\']+\.["\']+feature["\']+/');
    $this->assertMatchesRegularExpression(
      $we_expect_pattern,
      $sqlStatement,
      "The sql statement does not have the table prefix we expect."
    );

    // CURRENT OBJECT: Let's test it works when the current class is whitelisted.
    $chado_connection = \Drupal::service('tripal_chado.database');
    $chado_connection->useTripalDbxSchemaFor($this);
    try {
      $query = $chado_connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "Now TripalDBX should know that chado is the default schema for this test class and it should not throw an exception.");
    }
    // We expect: "SCHEMAPREFIX"."feature" but since the quotes are not
    // necessary and could be interchanged by Drupal, we use the following pattern.
    $sqlStatement = $query->getQueryString();
    $we_expect_pattern = str_replace('SCHEMAPREFIX', $chado_1_prefix, '/["\']+SCHEMAPREFIX["\']+\.["\']+feature["\']+/');
    $this->assertMatchesRegularExpression(
      $we_expect_pattern,
      $sqlStatement,
      "The sql statement does not have the table prefix we expect."
    );

    // Also test TripalDbxConnection::tablePrefix().
    $prefix = $chado_connection->tablePrefix('default', TRUE);
    $this->assertEquals(
      $chado_1_prefix . '.',
      $prefix,
      "We did not get the chado prefix despite asking for tripaldbx to be used."
    );
  }

  /**
   * Tests the Drupal query builders while querying chado.
   *
   * @dataProvider provideChadoSchemaVersions
   *
   * @param string $version
   *   The version of chado to test against.
   */
  #[DataProvider('provideChadoSchemaVersions')]
  public function testChadoQueryBuilding(string $version) {
    $chado = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY, $version);

    // INSERT:
    try {
      $query = $chado->insert('1:db')
        ->fields(['name' => 'GO']);
      $query->execute();
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "We should be able to insert into the Chado db table.");
    }
    $db_id = $chado->query("SELECT db_id FROM {1:db} WHERE name='GO'")->fetchField();
    $this->assertIsNumeric($db_id, "We should be able to select the primary key of the newly inserted db record.");

    // SELECT:
    try {
      $queryBuilder_db_id = $chado->select('1:db', 'db')
        ->fields('db', ['db_id'])
        ->condition('db.name', 'GO', '=')
        ->execute()
        ->fetchField();
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "We should be able to select from the Chado db table using the query builder.");
    }
    $this->assertIsNumeric($queryBuilder_db_id, "We expect the returned db_id to be numeric.");
    $this->assertEquals($db_id, $queryBuilder_db_id, "Both the query builder and query directly should provide the same result.");

    // UPDATE:
    $description = 'This is the description we will add during update.';
    try {
      $query = $chado->update('1:db')
        ->fields(['description' => $description])
        ->condition('name', 'GO', '=');
      $query->execute();
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "We should be able to update the Chado db table using the query builder.");
    }
    $results = $chado->query("SELECT * FROM {1:db} WHERE name='GO'")->fetchAll();
    $this->assertEquals(1, sizeof($results), "There should be only a single GO db record.");
    $this->assertIsNumeric($results[0]->db_id, "We should be able to select the primary key of the newly updated db record.");
    $this->assertEquals($db_id, $results[0]->db_id, "The primary key should remain unchanged during update.");

    // DELETE:
    try {
      $chado->delete('1:db')
        ->condition('db.name', 'GO', '=')
        ->execute();
    }
    catch (DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "We should be able to delete from the Chado db table using the query builder.");
    }
    $results = $chado->query("SELECT * FROM {1:db} WHERE name='GO'")->fetchAll();
    $this->assertEquals(0, sizeof($results), "There should not be any GO db record left.");
  }

  /**
   * Tests the ChadoConnection::findVersion() across unique version/init levels.
   *
   * @dataProvider provideChadoSchemaVersionsAcrossInitLevels
   *
   * @param string $version
   *   The version of chado to test against.
   * @param int $init_level
   *   The init level to create the test database with.
   */
  #[DataProvider('provideChadoSchemaVersionsAcrossInitLevels')]
  public function testFindVersion(string $version, int $init_level) {
    $expected_version = $version;

    $chado_connection = $this->createTestSchema($init_level, $version);

    // Default parameters;
    // version NOT in chado_installations table.
    $retrieved_version_defaults = $chado_connection->findVersion();
    $this->assertEquals(
      $expected_version,
      $retrieved_version_defaults,
      "Unable to extract the version from $init_level test schema with no parameters provided."
    );

    // Schema name provided;
    // version NOT in chado_installations table.
    $schema_name = $chado_connection->getSchemaName();
    $retrieved_version = $chado_connection->findVersion($schema_name);
    $this->assertEquals(
      $expected_version,
      $retrieved_version,
      "Unable to extract the version from $init_level test schema with the schema name provided."
    );
    $this->assertEquals(
      $retrieved_version_defaults,
      $retrieved_version,
      "We expect that providing the schema name should not change the version returned."
    );

    // Schema name + exact version requested;
    // version NOT in chado_installations table.
    $retrieved_version = $chado_connection->findVersion($schema_name, TRUE);
    $this->assertEquals(
      $expected_version,
      $retrieved_version,
      "Unable to extract the Exact Version from $init_level test schema with the schema name provided."
    );
    $this->assertEquals(
      $retrieved_version_defaults,
      $retrieved_version,
      "We expect that the exact version should not change the version returned."
    );

    // Default parameters;
    // version IN chado_installations table.
    // -- first insert into drupal chado_installations table.
    $drupal_connection = \Drupal::service('database');
    $drupal_connection->insert('chado_installations')
      ->fields([
        'schema_name' => $chado_connection->getSchemaName(),
        'version' => $version,
      ])
      ->execute();
    // -- now try to find the version.
    $retrieved_version = $chado_connection->findVersion();
    $this->assertEquals(
      $expected_version,
      $retrieved_version,
      "Unable to extract the version from $init_level test schema with no parameters provided."
    );
    $this->assertEquals(
      $retrieved_version_defaults,
      $retrieved_version,
      "We expect that retrieving the version from chado_installations should not change the version returned."
    );
  }

  /**
   * Tests the ChadoConnection::findVersion() when version unsupported.
   */
  public function testFindVersionUnsupported() {

    // Test version 1.2.
    // Chado version 1.2 will have a chadoprop table.
    $expected_version = '1.2';
    $chado_connection = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);
    // -- Update the chadoprop version to 1.2.
    $result = $chado_connection->select('1:cvterm', 'cvt')
      ->fields('cvt', ['cvterm_id']);
    $result->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
    $result->condition('cv.name', 'chado_properties');
    $result->condition('cvt.name', 'version');
    $result = $result->execute();
    $version_cvterm_id = $result->fetchField();
    $chado_connection->update('1:chadoprop')
      ->fields([
        'value' => $expected_version,
      ])
      ->condition('type_id', $version_cvterm_id)
      ->execute();
    // -- Now find the version and confirm it matches our expectations.
    $retrieved_version = $chado_connection->findVersion();
    $this->assertEquals(
      $expected_version,
      $retrieved_version,
      "The version returned for a chado instance with a chadoprop table but not 1.3+ version was not what we expected."
    );

    // Test version <= 1.11
    // Chado version 1.11 and less does not have a chadoprop table.
    $expected_version = '<=1.11';
    // -- Drop the chadoprop table.
    $chado_connection->schema()->dropTable('chadoprop');
    $this->assertFalse(
      $chado_connection->schema()->tableExists('chadoprop'),
      "We were unable to drop the chadoprop table when setting up our test."
    );
    // -- Now find the version and confirm it matches our expectations.
    $retrieved_version = $chado_connection->findVersion();
    $this->assertEquals(
      $expected_version,
      $retrieved_version,
      "The version returned for a chado instance with NO chadoprop table was not what we expected."
    );

    // Test finding the version of not a chado schema.
    $expected_version = '';
    $chado_connection = $this->createTestSchema(ChadoTestKernelBase::CREATE_SCHEMA);
    $retrieved_version = $chado_connection->findVersion();
    $this->assertEquals(
      $expected_version,
      $retrieved_version,
      "The version returned for a chado imposter schema was not what we expected."
    );
  }

  /**
   * Tests ChadoConnection::getTripalDbxClass() directly.
   */
  public function testGetTripalDbxClass() {

    // We just need a ChadoSchema of some type so no need to initialize anything.
    $chado_connection = $this->createTestSchema(ChadoTestKernelBase::CREATE_SCHEMA);

    // Ask for something valid and ensure and ensure we get it.
    $retrieved_schema = $chado_connection->getTripalDbxClass('Schema');
    $this->assertEquals(
      ChadoSchema::class,
      $retrieved_schema,
      "ChadoConnection::getTripalDbxClass() should return the full namespaced ChadoSchema."
    );

    // Ensure we get an exception when we ask for something invalid.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $retrieved_schema = $chado_connection->getTripalDbxClass('FRED');
    }
    catch (ConnectionException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue(
      $exception_caught,
      "ChadoConnection::getTripalDbxClass() should throw an exception when asked for an invalid class."
    );
    $this->assertEquals(
      "Invalid Tripal DBX class 'FRED'.",
      $exception_message,
      "ChadoConnection::getTripalDbxClass() exception message should indicate we asked for an invalid class."
    );
  }

  /**
   * Tests ChadoConnection::_construct() directly.
   */
  public function testChadoConnectionConstructor() {

    // Create with no parameters.
    $created_object = new ChadoConnection();
    $this->assertInstanceOf(
      ChadoConnection::class,
      $created_object,
      "We expected to be able to create a ChadoConnection with no parameters."
    );
  }

  /**
   * Tests ChadoConnection::removeAllTestSchemas().
   */
  public function testRemoveAllTestSchemas() {

    // We should not get an error when removing test schema even when there
    // are no test schema create yet.
    $created_object = new ChadoConnection();
    $created_object->removeAllTestSchemas();

    // Create a number of test schema.
    $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);
    $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);
    $chado_connection = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);

    // Confirm that we have multiple registered test schema.
    $instances = $chado_connection->getAvailableInstances();
    $total_num_instances = count($instances);
    $this->assertGreaterThanOrEqual(
      3,
      $total_num_instances,
      "We expect there to be at least 3 instances since we created that many test instances."
    );

    // Now remove the test instances and ensure we have the number of instances
    // we expect which is the number last time - the 3 test instances we created.
    $expected_num_instances = $total_num_instances - 3;
    $chado_connection->removeAllTestSchemas();
    $instances = $chado_connection->getAvailableInstances();
    $remaining_num_instances = count($instances);
    $this->assertNotEquals(
      $total_num_instances,
      $remaining_num_instances,
      "We should not have the same number of instances after removing test schema."
    );
    $this->assertEquals(
      $expected_num_instances,
      $remaining_num_instances,
      "We did not have the number of expected instances after removing the test instances."
    );

    // Make sure to indicate that the removed schema are no longer in use
    // so that the tearDownAfterClass() doesn't fail when the test schema
    // has already been removed.
    foreach (self::$testSchemas as $test_schema => $in_use) {
      self::$testSchemas[$test_schema] = FALSE;
    }
  }

}
