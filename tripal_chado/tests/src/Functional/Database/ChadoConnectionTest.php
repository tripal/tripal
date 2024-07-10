<?php

namespace Drupal\Tests\tripal_chado\Functional;

/**
 * Tests for the Chado Connection implementation of Tripal DBX Connection.
 *
 * @group Tripal
 * @group Tripal TripalDBX
 * @group Tripal TripalDBX Connection
 * @group TripalDBX Chado
 */
class ChadoConnectionTest extends ChadoTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado'];

  /**
   * Tests table prefixing by the ChadoConnection + TripalDbxConnection classes.
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
  public function testDefaultTablePrefixing() {
    $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);

    // Open a Connection to the default Tripal DBX managed Chado schema.
    $connection = \Drupal::service('tripal_chado.database');
    $chado_1_prefix = $connection->getSchemaName();

    // Create a situation where we should be using the core chado schema for our query.
    $query = $connection->query("SELECT name, uniquename FROM {1:feature} LIMIT 1");
    $sqlStatement = $query->getQueryString();
    // We expect: "SCHEMAPREFIX"."feature" but since the quotes are not
    // necessary and could be interchanged by Drupal, we use the following pattern.
    $we_expect_pattern = str_replace('SCHEMAPREFIX', $chado_1_prefix, '/["\']+SCHEMAPREFIX["\']+\.["\']+feature["\']+/');
    $this->assertMatchesRegularExpression($we_expect_pattern, $sqlStatement,
      "The sql statement does not have the table prefix we expect.");

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
    $connection->useTripalDbxSchemaFor(\Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase::class);
    try {
      $query = $connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    } catch (\Drupal\Core\Database\DatabaseExceptionWrapper $e) {
      $this->assertTrue(FALSE, "Now TripalDBX should know that chado is the default schema for this test class and it should not throw an exception.");
    }
    // We expect: "SCHEMAPREFIX"."feature" but since the quotes are not
    // necessary and could be interchanged by Drupal, we use the following pattern.
    $sqlStatement = $query->getQueryString();
    $we_expect_pattern = str_replace('SCHEMAPREFIX', $chado_1_prefix, '/["\']+SCHEMAPREFIX["\']+\.["\']+feature["\']+/');
    $this->assertMatchesRegularExpression($we_expect_pattern, $sqlStatement,
      "The sql statement does not have the table prefix we expect.");

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
    $this->assertMatchesRegularExpression($we_expect_pattern, $sqlStatement,
      "The sql statement does not have the table prefix we expect.");

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
    $this->assertMatchesRegularExpression($we_expect_pattern, $sqlStatement,
      "The sql statement does not have the table prefix we expect.");
  }

  /**
   * Tests the Drupal query builders while quering chado.
   */
  public function testChadoQueryBuilding() {
    $chado = $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);

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
   * This tests the ChadoConnection::findVersion() method.
   *
   * We will test that the version can be obtained from the test schema when it
   * is generated in the following ways:
   * 1. INIT_CHADO_EMPTY
   * 2. INIT_CHADO_DUMMY
   * 3. PREPARE_TEST_CHADO
   *
   * Furthermore, we will test both when the findVersion() method is called with
   * A. no parameters
   * B. schema name supplied
   * C. exact version requested.
   */
  public function testFindVersion() {
    $expected_version = '1.3';

    // Test 1A
    $connection = $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);
    $version = $connection->findVersion();
    $this->assertEquals($version, $expected_version,
      "Unable to extract the version from INIT_CHADO_EMPTY test schema with no parameters provided.");
      $version = $connection->findVersion();
    // Test 1B
    $schema_name = $connection->getSchemaName();
    $version = $connection->findVersion($schema_name);
    $this->assertEquals($version, $expected_version,
      "Unable to extract the version from INIT_CHADO_EMPTY test schema with the schema name provided.");
    // Test 1C
    $version = $connection->findVersion($schema_name, TRUE);
    $this->assertEquals($version, $expected_version,
      "Unable to extract the Exact Version from INIT_CHADO_EMPTY test schema with the schema name provided.");

    // Test 2A
    $connection = $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_DUMMY);
    $version = $connection->findVersion();
    $this->assertEquals($version, $expected_version,
      "Unable to extract the version from INIT_CHADO_DUMMY test schema with no parameters provided.");
      $version = $connection->findVersion();
    // Test 2B
    $schema_name = $connection->getSchemaName();
    $version = $connection->findVersion($schema_name);
    $this->assertEquals($version, $expected_version,
      "Unable to extract the version from INIT_CHADO_DUMMY test schema with the schema name provided.");
    // Test 2C
    $version = $connection->findVersion($schema_name, TRUE);
    $this->assertEquals($version, $expected_version,
      "Unable to extract the Exact Version from INIT_CHADO_DUMMY test schema with the schema name provided.");

    // Test 3A
    $connection = $this->createTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);
    $version = $connection->findVersion();
    $this->assertEquals($version, $expected_version,
      "Unable to extract the version from PREPARE_TEST_CHADO test schema with no parameters provided.");
      $version = $connection->findVersion();
    // Test 3B
    $schema_name = $connection->getSchemaName();
    $version = $connection->findVersion($schema_name);
    $this->assertEquals($version, $expected_version,
      "Unable to extract the version from PREPARE_TEST_CHADO test schema with the schema name provided.");
    // Test 3C
    $version = $connection->findVersion($schema_name, TRUE);
    $this->assertEquals($version, $expected_version,
      "Unable to extract the Exact Version from PREPARE_TEST_CHADO test schema with the schema name provided.");

  }
}
