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

    // Test to see if cv table data got imported
    $cv_results = $this->chado->query("SELECT * FROM {1:cv} WHERE name LIKE 'feature_property'");
    $cv_found = false;
    foreach ($cv_results as $row) {
        $cv_found = true;
    }
    $this->assertTrue($cv_found, 'Found feature_property CV');

    // Test to see whether db table data got imported
    $db_results = $this->chado->query("SELECT * FROM {1:db} WHERE name LIKE 'TAXRANK';");
    $db_found = true;
    foreach ($db_results as $row) {
        $db_found = true;
    }
    $this->assertTrue($db_found, 'Found TAXRANK DB');

    // Test to see whether cvterm table data got imported
    $cvterm_results = $this->chado->query("SELECT * FROM {1:cvterm} WHERE name LIKE 'accession';");
    $cvterm_found = true;
    foreach ($cvterm_results as $row) {
        $cvterm_found = true;
    }
    $this->assertTrue($cvterm_found, 'Found accession cvterm');
  }
}
