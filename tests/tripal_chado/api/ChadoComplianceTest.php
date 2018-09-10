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
  // use DBTransaction;

  /**
   * DataProvider, a list of all chado tables.
   *
   * @return array
   */
  public function chadoTableProvider() {

    // @todo expose all tables.
    return [
      ['organism'],
      ['feature'],
      ['stock'],
    ];
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
   * @group chado-schema
   * @group chado-compliance
   * @group lacey
   */
  public function testTableCompliance($table_name) {

    // Create the ChadoSchema class to aid in testing.
    $chado_schema = new \ChadoSchema();

    // Check #1: The table exists in the correct schema.
    $this->assertTrue(
      $chado_schema->checkTableExists($table_name),
      t(':table_name should exist in the :chado schema.',
        array(':table_name' => $table_name, ':chado' => $schema_name))
    );

    // Retrieve the schema for this table.

    // For each column in this table...

      // Check #2: The given field exists in the table.

      // Check #3: The field is the type we expect.


    // For each constraint on this table...

      // Check #4: The constraint exists.

      // Check #5: The constraint consists of the columns we expect.
  }
}
