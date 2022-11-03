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

  public function testChadoPreparer() {

    $test_chado = $this->chado;

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
    $this->runPrepareStepAssertions();
  }

  /**
   * Tests ChadoBrowserTestBase->prepareTestChado().
   *
   * This is added to speed up automated tests which require the prepare step.
   * This automated test ensures this simulated prepare (using SQL dumps) produces
   * an accurate representation of the official Prepare Task.
   */
  public function testPrepareTestChadoSimulation() {
    $this->prepareTestChado();
    $this->runPrepareStepAssertions();
  }

  /**
   * Helper Method: Check that the prepare step created what we expected it to.
   */
  protected function runPrepareStepAssertions() {

    // 1: CREATE CHADO CUSTOM TABLES.
    // --------------------------------
    $schema = $this->chado->schema();
    $expected_tables = [
      'tripal_gff_temp',
      'tripal_gffcds_temp',
      'tripal_gffprotein_temp',
      'tripal_obo_temp'
    ];
    foreach ($expected_tables as $table_name) {
      $this->assertTrue($schema->tableExists($table_name),
          "The Tripal Custom Table $table_name doesn't exist but should have been created during the prepare step.");
    }

    // 2: CREATE CHADO MVIEWS.
    // --------------------------------
    $schema = $this->chado->schema();
    $expected_tables = [
      'organism_stock_count',
      'library_feature_count',
      'organism_feature_count',
      'analysis_organism',
      'db2cv_mview',
      'cv_root_mview'
    ];
    foreach ($expected_tables as $table_name) {
      $this->assertTrue($schema->tableExists($table_name),
          "The Tripal Materialized View $table_name doesn't exist but should have been created during the prepare step.");
    }

    // 3: IMPORT ONTOLOGIES.
    // --------------------------------
    // Check for some specific cv / db which should have been inserted.
    $cv_found = $this->chado->query("SELECT 1 FROM {1:cv} WHERE name = 'feature_property'")->fetchField();
    $this->assertEquals(1, $cv_found, 'Found feature_property CV');
    $db_found = $this->chado->query("SELECT 1 FROM {1:db} WHERE name = 'TAXRANK';")->fetchField();
    $this->assertEquals(1, $db_found, 'Found TAXRANK DB');
    $cvterm_found = $this->chado->query("SELECT 1 FROM {1:cvterm} WHERE name = 'accession'")->fetchField();
    $this->assertEquals(1, $cvterm_found, 'Found accession cvterm');

    // 4: POPULATE CV_ROOT_MVIEW.
    // --------------------------------

    // 5: POPULATE DB2CV_MVIEW.
    // --------------------------------

    // 6: POPULATE CHADO_SEMWEB TABLE.
    // --------------------------------
    // Functionality not complete in the prepare step yet.

    // 7: CHADO CVS TO TRIPAL TERMS.
    // --------------------------------
    // Functionality not complete in the prepare step yet.
  }
}
