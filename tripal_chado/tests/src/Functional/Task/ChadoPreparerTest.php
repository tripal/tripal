<?php

namespace Drupal\Tests\tripal_chado\Functional\Task;

use Drupal\Tests\tripal_chado\Functional\ChadoTestBrowserBase;
use Drupal\tripal_chado\Task\ChadoPreparer;

/**
 * Tests for Chado preparer task.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Task\ChadoPreparer
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Tripal Chado Task
 * @group Tripal Chado Preparer
 */
class ChadoPreparerTest extends ChadoTestBrowserBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['tripal', 'tripal_chado', 'tripal_biodb', 'field_ui'];

  /**
   * Tests the official Chado prepare task.
   */
  public function testChadoPreparer() {

    $test_chado = $this->createTestSchema(ChadoTestBrowserBase::INIT_CHADO_EMPTY);

    // Sanity check: make sure we have the necessary tables.
    $public = \Drupal::database();
    $schema = $public->schema();
    $this->assertTrue($schema->tableExists('tripal_custom_tables'),
        "The Tripal custom_table doesn't exist.");
    $this->assertTrue($schema->tableExists('tripal_mviews'),
        "The Tripal custom_table doesn't exist.");

    // First prepare Chado.
    $preparer = \Drupal::service('tripal_chado.preparer');
    $preparer->setParameters([
      'output_schemas' => [$test_chado->getSchemaName()],
    ]);
    $success = $preparer->performTask();
    $this->assertTrue($success, 'Task performed.');

    // Check that the prepare step created what we expected it to.
    $this->runPrepareStepAssertions($test_chado->getSchemaName());
  }

  /**
   * Tests ChadoBrowserTestBase->prepareTestChado().
   *
   * This is added to speed up automated tests which require the prepare step.
   * This automated test ensures this simulated prepare (using SQL dumps) produces
   * an accurate representation of the official Prepare Task.
   */
  public function testPrepareTestChadoSimulation() {
    $prepared_chado = $this->getTestSchema(ChadoTestBrowserBase::PREPARE_TEST_CHADO);
    $this->runPrepareStepAssertions($prepared_chado->getSchemaName());
  }

  /**
   * Helper Method: Check that the prepare step created what we expected it to.
   */
  protected function runPrepareStepAssertions($chadoSchema2check) {
    $chado2check = \Drupal::service('tripal_chado.database');
    $chado2check->setSchemaName($chadoSchema2check);
    $schema2check = $chado2check->schema();

    // 1: CREATE CHADO CUSTOM TABLES.
    // --------------------------------
    $expected_tables = [
      'tripal_gff_temp',
      'tripal_gffcds_temp',
      'tripal_gffprotein_temp',
      'tripal_obo_temp'
    ];
    foreach ($expected_tables as $table_name) {
      $this->assertTrue($schema2check->tableExists($table_name),
          "The Tripal Custom Table $table_name doesn't exist but should have been created during the prepare step.");
    }

    // 2: CREATE CHADO MVIEWS.
    // --------------------------------
    $expected_tables = [
      'organism_stock_count',
      'library_feature_count',
      'organism_feature_count',
      'analysis_organism',
      'db2cv_mview',
      'cv_root_mview'
    ];
    foreach ($expected_tables as $table_name) {
      $this->assertTrue($schema2check->tableExists($table_name),
          "The Tripal Materialized View $table_name doesn't exist but should have been created during the prepare step.");
    }

    // 3: IMPORT ONTOLOGIES.
    // --------------------------------
    $expected_counts_by_table = [
      'cv' => 32,
      'db' => 40,
      'cvterm' => 3178,
      'dbxref' => 3485,
    ];
    foreach ($expected_counts_by_table as $table_name => $expected_count) {
      $count = $chado2check->query("SELECT count(*) FROM {1:$table_name}")->fetchField();
      $this->assertGreaterThanOrEqual($expected_count, $count,
        "There was not the expected number of records in the $table_name table after preparing.");
    }

    // Check for some specific cv / db which should have been inserted.
    $cv_found = $chado2check->query("SELECT 1 FROM {1:cv} WHERE name = 'tripal_contact'")->fetchField();
    $this->assertEquals(1, $cv_found, 'Found feature_property CV');
    $db_found = $chado2check->query("SELECT 1 FROM {1:db} WHERE name = 'TAXRANK';")->fetchField();
    $this->assertEquals(1, $db_found, 'Found TAXRANK DB');
    $cvterm_found = $chado2check->query("SELECT 1 FROM {1:cvterm} WHERE name = 'accession'")->fetchField();
    $this->assertEquals(1, $cvterm_found, 'Found accession cvterm');

    // 4: POPULATE CV_ROOT_MVIEW.
    // --------------------------------
    $table_name = 'cv_root_mview';
    $expected_count = 9;
    $count = $chado2check->query("SELECT count(*) FROM {1:$table_name}")->fetchField();
    $this->assertGreaterThanOrEqual($expected_count, $count,
      "There was not the expected number of records in the $table_name table after preparing.");

    // 5: POPULATE DB2CV_MVIEW.
    // --------------------------------
    $table_name = 'db2cv_mview';
    $expected_count = 28;
    $count = $chado2check->query("SELECT count(*) FROM {1:$table_name}")->fetchField();
    $this->assertGreaterThanOrEqual($expected_count, $count,
      "There was not the expected number of records in the $table_name table after preparing.");

    // 6: POPULATE CHADO_SEMWEB TABLE.
    // --------------------------------
    // Functionality not complete in the prepare step yet.

    // 7: CHADO CVS TO TRIPAL TERMS.
    // --------------------------------
    // Functionality not complete in the prepare step yet.
  }
}
