<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests for ChadoConnection.
 *
 * @group Tripal Chado
 * @group TripalDBX
 * @group ChadoConnection
 */
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
  ];

  /**
   * Lists init levels that are worth testing for schema actions.
   *
   * @var array
   */
  protected static array $init_levels = [
    3, // self::$INIT_CHADO_EMPTY,
    4, // self::$INIT_CHADO_DUMMY,
    5, // self::$PREPARE_TEST_CHADO,
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
   * @covers \Drupal\tripal_chado\Database\ChadoConnection::getAvailableInstances
   */
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
      'is_test' => ['boolean','string'],
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
   * @covers \Drupal\tripal\TripalDBX\TripalDbxConnection
   * @covers \Drupal\tripal\TripalDBX\TripalDbxSchema
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
   */
  public function testDefaultTablePrefixing(string $version, int $init_level) {
    $this->createTestSchema($init_level, $version);

    // Open a Connection to the default Tripal DBX managed Chado schema.
    $connection = \Drupal::service('tripal_chado.database');
    $chado_1_prefix = $connection->getSchemaName();

    // Create a situation where we should be using the core chado schema for our query.
    $query = $connection->query("SELECT name, uniquename FROM {1:feature} LIMIT 1");
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
      $query = $connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
      $this->assertTrue(TRUE, "We expect to have an exception thrown when TripalDBX incorrectly assumes the feature table is in Drupal, which it's not.");
    }

    // Now we want to tell Tripal DBX that the default schema for this query should be chado.
    // Using useTripalDbxSchemaFor():
    //---------------------------------
    // PARENT CLASS: Let's check if it works when a parent class is white listed.
    $connection = \Drupal::service('tripal_chado.database');
    $connection->useTripalDbxSchemaFor(\Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase::class);
    try {
      $query = $connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
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

    // CURRENT CLASS: Let's test it works when the current class is whitelisted
    $connection = \Drupal::service('tripal_chado.database');
    $connection->useTripalDbxSchemaFor(\Drupal\Tests\tripal_chado\Functional\ChadoConnectionTest::class);
    try {
      $query = $connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
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

    // CURRENT OBJECT: Let's test it works when the current class is whitelisted
    $connection = \Drupal::service('tripal_chado.database');
    $connection->useTripalDbxSchemaFor($this);
    try {
      $query = $connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
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
  }

  /**
   * Tests the Drupal query builders while quering chado.
   *
   * @dataProvider provideChadoSchemaVersions
   *
   * @covers \Drupal\tripal\TripalDBX\TripalDbxConnection
   * @covers \Drupal\tripal\TripalDBX\TripalDbxSchema
   */
  public function testChadoQueryBuilding(string $version) {
    $chado = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY, $version);

    // INSERT:
    try {
      $query = $chado->insert('1:db')
      ->fields(['name' => 'GO']);
      $query->execute();
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
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
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
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
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
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
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
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
   * @covers \Drupal\tripal_chado\Database\ChadoConnection::findVersion
   *
   * Furthermore, we will test both when the findVersion() method is called with
   * A. no parameters
   * B. schema name supplied
   * C. exact version requested.
   */
  public function testFindVersion(string $version, int $init_level) {
    $expected_version = $version;

    $connection = $this->createTestSchema($init_level, $version);

    // Test A.
    $retrieved_version = $connection->findVersion();
    $this->assertEquals(
      $retrieved_version,
      $expected_version,
      "Unable to extract the version from $init_level test schema with no parameters provided."
    );

    // Test B.
    $schema_name = $connection->getSchemaName();
    $retrieved_version = $connection->findVersion($schema_name);
    $this->assertEquals(
      $retrieved_version,
      $expected_version,
      "Unable to extract the version from $init_level test schema with the schema name provided."
    );

    // Test C.
    $retrieved_version = $connection->findVersion($schema_name, TRUE);
    $this->assertEquals(
      $retrieved_version,
      $expected_version,
      "Unable to extract the Exact Version from $init_level test schema with the schema name provided."
    );

  }
}
