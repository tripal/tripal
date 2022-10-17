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
    $query = $connection->query("SELECT name, uniquename FROM {feature} LIMIT 1");
    $sqlStatement = $query->getQueryString();
    // We expect: "SCHEMAPREFIX"."feature" but since the quotes are not
    // necessary and could be interchanged by Drupal, we use the following pattern.
    $we_expect_pattern = str_replace('SCHEMAPREFIX', $chado_1_prefix, '/["\']+SCHEMAPREFIX["\']+\.["\']+feature["\']+/');
    $this->assertMatchesRegularExpression($we_expect_pattern, $sqlStatement,
      "Unable to determine that chado is the default schema because the sql statement does not have the table prefix we expect.");
  }
}
