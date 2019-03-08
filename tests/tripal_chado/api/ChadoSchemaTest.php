<?php

namespace Tests\tripal_chado\api;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

module_load_include('inc', 'tripal_chado', 'api/ChadoSchema');

/**
 * Tests the ChadoSchema class.
 *
 * @todo test "Check" functions in the ChadoSchema class.
 */
class ChadoSchemaTest extends TripalTestCase {

  use DBTransaction;

  /**
   * Tests that the class can be initiated with or without a record specified
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  public function testInitClass() {

    // Test with no parameters.
    $chado_schema = new \ChadoSchema();
    $this->assertNotNull($chado_schema);

    // Test with version.
    $chado_schema = new \ChadoSchema('1.3');
    $this->assertNotNull($chado_schema);
  }

  /**
   * Tests the ChadoSchema->getVersion() method.
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  public function testGetVersion() {

    // Generate a fake version.
    $faker = Factory::create();
    $version = $faker->randomFloat(2, 1, 5);

    // Check version can be retrieved when we set it.
    $chado_schema = new \ChadoSchema($version);
    $retrieved_version = $chado_schema->getVersion();
    $this->assertEquals(
      $version,
      $retrieved_version,
      t('The version retrieved via ChadoSchema->getVersion, "!ret", should equal that set, "!set"',
        ['!ret' => $retrieved_version, '!set' => $version])
    );

    // @todo Check version can be retrieved when it's looked up?
  }

  /**
   * Tests the ChadoSchema->getSchemaName() method.
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  public function testGetSchemaName() {

    // Generate a fake version.
    $faker = Factory::create();
    $version = $faker->randomFloat(2, 1, 5);
    $schema_name = $faker->word();

    // Check the schema name can be retrieved when we set it.
    $chado_schema = new \ChadoSchema($version, $schema_name);
    $retrieved_schema = $chado_schema->getSchemaName();
    $this->assertEquals(
      $schema_name,
      $retrieved_schema,
      t('The schema name retrieved via ChadoSchema->getSchemaName, "!ret", should equal that set, "!set"',
        ['!ret' => $retrieved_schema, '!set' => $schema_name])
    );

    // @todo Check schema name can be retrieved when it's looked up?
  }

  /**
   * Tests ChadoSchema->getTableNames() method.
   *
   * @dataProvider knownTableProvider
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  public function testGetTableNames($version, $known_tables) {

    // Check: Known tables for a given version are returned.
    $chado_schema = new \ChadoSchema($version);
    $returned_tables = $chado_schema->getTableNames();

    foreach ($known_tables as $table_name) {
      $this->assertArrayHasKey(
        $table_name,
        $returned_tables,
        t('The table, "!known", should exist in the returned tables list for version !version.',
          [':known' => $table_name, ':version' => $version])
      );
    }
  }

  /**
   * Tests ChadoSchema->getTableSchema() method.
   *
   * @dataProvider chadoTableProvider
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  public function testGetTableSchema($version, $table_name) {

    // Check: a schema is returned that matches what we expect.
    $chado_schema = new \ChadoSchema($version);

    $table_schema = $chado_schema->getTableSchema($table_name);

    $this->assertNotEmpty(
      $table_schema,
      t('Returned schema for "!table" in chado v!version should not be empty.',
        ['!table' => $table_name, '!version' => $version])
    );

    $this->assertArrayHasKey(
      'fields',
      $table_schema,
      t('The schema array for "!table" should have columns listed in an "fields" array',
        ['!table' => $table_name])
    );

    // Instead of asserting these keys exist. Lets assert that if they do exist,
    // they match the expected format.

    if (isset($table_schema['primary key'])) {
      $this->assertTrue(is_array($table_schema['primary key']),
        t('The primary key of the Tripal Schema definition for "!table" must be an array.',
          ['!table' => $table_name]));

    }

    $this->assertArrayHasKey(
      'foreign keys',
      $table_schema,
      t('The schema array for "!table" should have foreign keys listed in an "foreign keys" array',
        ['!table' => $table_name])
    );

  }

  /**
   * Tests ChadoSchema->getCustomTableSchema() method.
   *
   * @dataProvider knownCustomTableProvider
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  public function testGetCustomTableSchema($table_name) {

    // Check: a schema is returned that matches what we expect.
    $chado_schema = new \ChadoSchema();
    $table_schema = $chado_schema->getCustomTableSchema($table_name);

    $this->assertNotEmpty(
      $table_schema,
      t('Returned schema for "!table" in chado v!version should not be empty.',
        ['!table' => $table_name, '!version' => $version])
    );

    $this->assertArrayHasKey(
      'fields',
      $table_schema,
      t('The schema array for "!table" should have columns listed in an "fields" array',
        ['!table' => $table_name])
    );

    // NOTE: Other then ensuring fields are set, we can't test further since all other
    // keys are technically optional and these arrays are set by admins.

  }

  /**
   * Tests ChadoSchema->getBaseTables() method.
   *
   * @dataProvider knownBaseTableProvider
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  public function testGetBaseTables($version, $known_tables) {

    // Check: Known base tables for a given version are returned.
    $chado_schema = new \ChadoSchema($version);
    $returned_tables = $chado_schema->getBaseTables();

    foreach ($known_tables as $table_name) {

      $found = FALSE;

      foreach ($returned_tables as $check_table) {

        if ($check_table == $table_name) {
          $found = TRUE;
        }
      }
      $this->assertTrue($found, "{$table_name} was not returned by getBaseTables for Chado v {$version}");
    }

  }

  /**
   * Tests ChadoSchema->getCvtermMapping() method.
   *
   * @dataProvider chadoTableProvider
   *
   * @group api
   * @group chado
   * @group chado-schema
   */
  // public function testGetCvtermMapping($version, $table_name) {

  //
  //    // Ideally we would create a new chado table + mapping and then test this pulls it out
  //    // since admin can re-map terms. However, that's more then I meant to bite off right
  //    // now...
  //
  //    // @todo Test that known terms match the tables we expect.
  //
  //    // @todo Test that a non-existent term throws an error.
  //
  //    // @todo Test that an fake unmapped term returns no mapping.
  // }

  /**
   * Data Provider: returns known tables specific to a given chado version.
   *
   * @return array
   */
  public function knownTableProvider() {
    // chado version, array of 3 tables specific to version.

    return [
      ['1.2', ['cell_line_relationship', 'cvprop', 'chadoprop']],
      ['1.3', ['analysis_cvterm', 'dbprop', 'organism_pub']],
    ];
  }

  /**
   * Data Provider: returns known tables specific to a given chado version.
   *
   * @return array
   */
  public function knownBaseTableProvider() {
    // chado version, array of 3 tables specific to version.

    return [
      [
        '1.2',
        ['organism', 'feature', 'stock', 'project', 'analysis', 'phylotree'],
      ],
      [
        '1.3',
        ['organism', 'feature', 'stock', 'project', 'analysis', 'phylotree'],
      ],
    ];
  }

  /**
   * Data Provider: returns known custom tables specific to a given chado
   * version.
   *
   * NOTE: These tables are provided by core Tripal so we should be able to
   *  depend on them. Also, for the same reason, chado version doesn't matter.
   *
   * @return array
   */
  public function knownCustomTableProvider() {

    return [
      ['library_feature_count'],
      ['organism_feature_count'],
      ['tripal_gff_temp'],
    ];
  }

  /**
   * DataProvider, a list of all chado tables.
   *
   * @return array
   */
  public function chadoTableProvider() {

    // Provide the table list for all versions.
    $dataset = [];
    foreach (['1.11', '1.2', '1.3'] as $version) {
      $chado_schema = new \ChadoSchema();
      $version = $chado_schema->getVersion();

      foreach ($chado_schema->getTableNames() as $table_name) {
        $dataset[] = [$version, $table_name];
      }
    }

    return $dataset;
  }
}
