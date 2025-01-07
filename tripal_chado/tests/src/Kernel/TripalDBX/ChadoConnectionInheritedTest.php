<?php

namespace Drupal\Tests\tripal\Kernel\TripalDBX;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal\TripalDBX\TripalDbx;
use Drupal\tripal\TripalDBX\Exceptions\ConnectionException;
use Drupal\tripal_chado\Database\ChadoConnection;

/**
 * Tests for ChadoConnection.
 *
 * @group Tripal Chado
 * @group TripalDBX
 * @group ChadoDBX
 * @group ChadoConnection
 */
class ChadoConnectionInheritedTest extends ChadoTestKernelBase {

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
   * Tests TripalDbxConnection getters across different chado versions.
   *
   * @dataProvider provideChadoSchemaVersions
   *
   * @param string $version
   *   The version of chado to test against.
   */
  public function testConnectionGettersSetters(string $version) {
    $chado_connection = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY, $version);

    // Database name.
    // Use the drupal way to get what we expect.
    $options = $chado_connection->getConnectionOptions();
    $expected_database = $options['database'];
    // Now retrieve it and see if it matches.
    $retrieved_db_name = $chado_connection->getDatabaseName();
    $this->assertEquals(
      $expected_database,
      $retrieved_db_name,
      "TripalDbxConnection::getDatabaseName() didn't return the name of our test schema as we expected."
    );

    // Database Key.
    $expected_database_key = 'default';
    $retrieved_dbkey = $chado_connection->getDatabaseKey();
    $this->assertEquals(
      $expected_database_key,
      $retrieved_dbkey,
      "TripalDbxConnection::getDatabaseKey() didn't return the database key was not what we expected."
    );

    // Logger.
    $logger = \Drupal::service('tripal.logger');
    $chado_connection->setMessageLogger($logger);
    $retrieved_logger = $chado_connection->getMessageLogger();
    $this->assertInstanceOf(
      \Drupal\tripal\Services\TripalLogger::class,
      $retrieved_logger,
      "TripalDbxConnection::getMessageLogger() did not return a Tripal message logger."
    );
    $this->assertEquals(
      $logger,
      $retrieved_logger,
      "TripalDbxConnection::getMessageLogger() did not return the logger that we set previously."
    );
  }

  /**
   * Tests TripalDbxConnection extraSchema methods across chado versions.
   *
   * @dataProvider provideChadoSchemaVersions
   *
   * @param string $version
   *   The version of chado to test against.
   */
  public function testConnectionExtraSchemas(string $version) {
    $chado_connection = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY, $version);

    // Ensure that getting extra schema when none are set returns none.
    $returned_extra_schema = $chado_connection->getExtraSchemas();
    $this->assertIsArray(
      $returned_extra_schema,
      "TripalDbxConnection::getExtraSchemas() should return an array but didn't even if no extra schema were set yet."
    );
    $this->assertEmpty(
      $returned_extra_schema,
      "TripalDbxConnection::getExtraSchemas() should not return any when none have been set yet."
    );

    // Now set an extra schema that actually exists.
    $extra_schema = $this->createTestSchema(ChadoTestKernelBase::CREATE_SCHEMA);
    $chado_connection->addExtraSchema($extra_schema->getSchemaName());

    // And another one that is a valid name but does not exist.
    $extra_schema2 = $this->createTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);
    $chado_connection->addExtraSchema($extra_schema2->getSchemaName());

    // Now confirm both are gettable.
    $returned_extra_schema = $chado_connection->getExtraSchemas();
    $this->assertIsArray(
      $returned_extra_schema,
      "TripalDbxConnection::getExtraSchemas() should return an array but didn't."
    );
    $this->assertCount(
      2,
      $returned_extra_schema,
      "We added 2 extra schema so we expected to be able to get that many."
    );
    $this->assertEquals(
      $extra_schema->getSchemaName(),
      $returned_extra_schema[2],
      "We expected the first extra schema to be the one we added first."
    );
    $this->assertEquals(
      $extra_schema2->getSchemaName(),
      $returned_extra_schema[3],
      "We expected the second extra schema to be the one we added second."
    );

    // Then add one by setting the index directly where the index is free.
    $extra_schema3 = $this->createTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);
    $chado_connection->setExtraSchema($extra_schema3->getSchemaName(), 4);
    // And override the first schema set with a new one.
    $extra_schema4 = $this->createTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);
    $chado_connection->setExtraSchema($extra_schema4->getSchemaName(), 2);

    // Now check that we can get all the current ones set.
    $returned_extra_schema = $chado_connection->getExtraSchemas();
    $this->assertIsArray(
      $returned_extra_schema,
      "TripalDbxConnection::getExtraSchemas() should return an array but didn't."
    );
    $this->assertCount(
      3,
      $returned_extra_schema,
      "We added one more extra schema and reset an existing one therefore we expect a total of 3 not 4."
    );
    $this->assertEquals(
      $extra_schema2->getSchemaName(),
      $returned_extra_schema[3],
      "We expected the second extra schema to be the one we originally added second."
    );
    $this->assertEquals(
      $extra_schema3->getSchemaName(),
      $returned_extra_schema[4],
      "We expected the third extra schema to be the one we set directly."
    );
    $this->assertEquals(
      $extra_schema4->getSchemaName(),
      $returned_extra_schema[2],
      "We expected the first extra schema to be the one we reset after the fact."
    );

    // Now clear the extra schema and ensure the list is finally empty.
    $chado_connection->clearExtraSchemas();
    $returned_extra_schema = $chado_connection->getExtraSchemas();
    $this->assertIsArray(
      $returned_extra_schema,
      "TripalDbxConnection::getExtraSchemas() should return an array even after we clear all the extra schema."
    );
    $this->assertEmpty(
      $returned_extra_schema,
      "TripalDbxConnection::getExtraSchemas() should not return any when we just cleared them."
    );
  }

  /**
   * Tests TripalDbxConnection extraSchema methods across chado versions.
   *
   * @dataProvider provideChadoSchemaVersions
   *
   * @param string $version
   *   The version of chado to test against.
   */
  public function testConnectionExtraSchemasExceptions(string $version) {
    $chado_connection = $this->createTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY, $version);
    $extra_schema = $this->createTestSchema(ChadoTestKernelBase::CREATE_SCHEMA);

    // TEST: Invalid schema name.
    // Check invalid schema name being set.
    $invalid_schema_name = '0a!b#[].';

    // Now addExtraSchema should throw an exception because we can't add an
    // invalid schema name.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_connection->addExtraSchema($invalid_schema_name);
    } catch (ConnectionException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue(
      $exception_caught,
      "We expect an exception by addExtraSchema() when the schema name is invalid."
    );
    $this->assertEquals(
      "The schema name must not begin with a number and only contain lower case letters, numbers, underscores and diacritical marks.",
      $exception_message,
      "We expect to be told the issue is an invalid schema name since that is what we did to trigger the exception."
    );

    // Now setExtraSchema() should throw an exception because we can't add an
    // extra schema when we haven't set a new one yet.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_connection->setExtraSchema($invalid_schema_name, 2);
    } catch (ConnectionException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue(
      $exception_caught,
      "We expect an exception by setExtraSchema() when the primary schema is not set."
    );
    $this->assertEquals(
      "The schema name must not begin with a number and only contain lower case letters, numbers, underscores and diacritical marks.",
      $exception_message,
      "We expect to be told the issue is an invalid schema name since that is what we did to trigger the exception."
    );

    // TEST: Setting extra with invalid index.
    // index is less than 2 (i.e. is ones reserved for drupal and primary).
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_connection->setExtraSchema($extra_schema->getSchemaName(), 1);
    } catch (ConnectionException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue(
      $exception_caught,
      "We expect an exception by setExtraSchema() when the index is less than 2."
    );
    $this->assertEquals(
      "Invalid extra schema index '1'.",
      $exception_message,
      "We expect to be told the issue is an invalid index since that is what we did to trigger the exception."
    );

    // index is not the next one in line.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_connection->setExtraSchema($extra_schema->getSchemaName(), 99);
    } catch (ConnectionException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue(
      $exception_caught,
      "We expect an exception by setExtraSchema() when the index is not the next in line."
    );
    $this->assertEquals(
      "Invalid extra schema index '99'. Intermediate schemas are missing.",
      $exception_message,
      "We expect to be told the issue is an invalid index since that is what we did to trigger the exception."
    );

    // TEST: no primary schema set.
    // Reset the connection so no current schema is set.
    $chado_connection->setSchemaName('');

    // Now addExtraSchema() should throw an exception because we can't add an
    // extra schema when we haven't set a new one yet.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_connection->addExtraSchema($extra_schema->getSchemaName());
    }
    catch(ConnectionException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue(
      $exception_caught,
      "We expect an exception by addExtraSchema() when the primary schema is not set."
    );
    $this->assertEquals(
      "Cannot add an extra schema. No current schema (specified by ::setSchemaName).",
      $exception_message,
      "We expect to be told the issue is a missing primary schema since that is what we did to trigger the exception."
    );

    // Now setExtraSchema() should throw an exception because we can't add an
    // extra schema when we haven't set a new one yet.
    $exception_caught = FALSE;
    $exception_message = '';
    try {
      $chado_connection->setExtraSchema($extra_schema->getSchemaName(), 2);
    } catch (ConnectionException $e) {
      $exception_caught = TRUE;
      $exception_message = $e->getMessage();
    }
    $this->assertTrue(
      $exception_caught,
      "We expect an exception by setExtraSchema() when the primary schema is not set."
    );
    $this->assertEquals(
      "Cannot add an extra schema. No current schema (specified by ::setSchemaName).",
      $exception_message,
      "We expect to be told the issue is a missing primary schema since that is what we did to trigger the exception."
    );
  }
}
