<?php
namespace Tests\tripal_chado\api;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('inc', 'tripal_chado', 'api/ChadoSchema');

/**
 * Tests the current Chado Database is compliant with the schema definition used by Tripal
 */
class ChadoComplianceTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * DataProvider, a list of all chado tables.
   *
   * @return array
   */
  public function chadoTableProvider() {

    $chado_schema = new \ChadoSchema();
    $version = $chado_schema->getVersion();

    $dataset = [];
    foreach ($chado_schema->getTableNames() as $table_name) {
      $dataset[] = [$version, $table_name];
    }

    return $dataset;
  }

  /**
   * Tests Compliance for a given table.
   *
   * The following is tested:
   *   1. The table exists in the correct schema.
   *   2. It has all the fields we expect.
   *   3. Each field is the type we expect.
   *   4. It has all the constraints we expect.
   *   5. Each constraint consists of the columns we expect.
   *
   * @dataProvider chadoTableProvider
   *
   * @group api
   * @group chado
   * @group chado-compliance
   */
  public function testTableCompliance($schema_version, $table_name) {

    // Create the ChadoSchema class to aid in testing.
    $chado_schema = new \ChadoSchema();
    $version = $chado_schema->getVersion();

    // Check #1: The table exists in the correct schema.
    $this->assertTrue(
      $chado_schema->checkTableExists($table_name),
      t('"!table_name" should exist in the "!chado" schema v!version.',
        array('!table_name' => $table_name, '!chado' => $schema_name, '!version' => $version))
    );

    // Retrieve the schema for this table.
    $table_schema = $chado_schema->getTableSchema($table_name);
    //print "Table ($table_name)! " . print_r($table_schema,TRUE) . "\n";

    // For each column in this table...
    foreach ($table_schema['fields'] as $column_name => $column_details) {

      // Check #2: The given field exists in the table.
      $this->assertTrue(
        $chado_schema->checkColumnExists($table_name, $column_name),
        t('The column "!column" must exist in "!table" for chado v!version.',
          array('!column' => $column_name, '!table' => $table_name, '!version' => $version))
      );

      // Check #3: The field is the type we expect.
      $this->assertTrue(
        $chado_schema->checkColumnType($table_name, $column_name, $column_details['type']),
        t('The column "!table.!column" must be of type "!type" for chado v!version.',
          array('!column' => $column_name, '!table' => $table_name,
            '!version' => $version, '!type' => $column_details['type']))
      );
    }

    // For each constraint on this table...

      // Check #4: The constraint exists.

      // Check #5: The constraint consists of the columns we expect.
  }
}
