<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\TripalImporter;

use Drupal\Core\Url;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\user\Traits\UserCreationTrait;

/**
 * Tests the Chado Importer base class transaction functionality.
 *
 * @group TripalImporter
 * @group ChadoImporter
 */
class ChadoImporterTransactionTest extends ChadoTestKernelBase {

  use UserCreationTrait;

	protected $defaultTheme = 'stark';

	protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  protected $connection;

  /**
   * Annotations associated with the mock_plugin.
   * @var Array
   */
  protected $plugin_definition = [
    'id' => 'fakeImporterName',
    'label' => 'Gemstone Loader',
    'description' => 'Imports details on the incredible diversity of gemstones created by our earth into Chado.',
    'file_types' => ["gem", "txt"],
    'upload_description' => "Please provide a plain text, tab-delimited file of gemstone descriptions making sure to include the details which make them most unique and beautiful.",
    'upload_title' => 'Gemstone Descriptions',
    'use_analysis' => FALSE,
    'require_analysis' => FALSE,
    'button_text' => 'Import file',
    'file_upload' => FALSE,
    'file_load' => FALSE,
    'file_remote' => FALSE,
    'file_required' => FALSE,
    'cardinality' => 1,
  ];

  protected $importer;

	/**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Open connection to Chado
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Ensure we can access file_managed related functionality from Drupal.
    // ... users need access to system.action config?
    $this->installConfig('system');
    // ... managed files are associated with a user.
    $this->installEntitySchema('user');
    // ... Finally the file module + tables itself.
    $this->installEntitySchema('file');
    $this->installSchema('file', ['file_usage']);
    $this->installSchema('tripal_chado', ['tripal_custom_tables']);
    // Ensure we have our tripal import tables.
    $this->installSchema('tripal', ['tripal_import', 'tripal_jobs']);
    // Create and log-in a user.
    $this->setUpCurrentUser();

    $expected_args = ['run_args' => [], 'files' => []];
    $plugin_defn = $this->plugin_definition;
    $configuration = [];
    $plugin_id = 'fakeImporterName';
    $this->importer = $this->getMockForAbstractClass(
      '\Drupal\tripal_chado\TripalImporter\ChadoImporterBase',
      [$configuration, $plugin_id, $plugin_defn, $this->connection]
    );
    $import_id = $this->importer->createImportJob(['schema_name' => $this->connection->getSchemaName()]);
    $this->assertIsNumeric($import_id, "We were unable to create a tripal import record during setup.");

  }

  public function testTransactionRollback() {
    // Override the run() method of our mock importer to:
    // 1. Insert a record into the chado.organism table
    // 2. Throw an exception
    // This mimics the situation where an importer run encounters an
    // exception partway through a transaction
    $this->importer
      ->method('run')
      ->willReturnCallback(function (): bool {
        $connection = \Drupal::service('tripal_chado.database');
        $connection->insert('1:organism')
          ->fields([
            'genus' => 'Tripalus',
            'species' => 'databasica'
          ])
          ->execute();
          throw new \Exception(
            t('Mock importer throws new exception.')
          );
      });
    $logger = \Drupal::service("tripal.logger");

    // Try running our importer to ensure an exception is thrown
    $exception_caught = FALSE;
    try {
      tripal_run_importer_run($this->importer, $logger);
    } catch (\Exception $e) {
      $exception_caught = TRUE;
    }
    $this->assertTrue($exception_caught, 'Did not catch exception that should have been thrown by overriding the run() method.');

    // Now query organism table to ensure the database transaction was
    // successfully rolled back
    $organism_count_query = $this->connection->select('1:organism', 'o')
      ->countQuery()->execute()->fetchField();
    $this->assertEquals($organism_count_query, 0, 'The chado.organism table is not empty despite triggering a database rollback.');
    // Also ensure the Drupal database was not rolled back
    // by confirming the tripal import record is still there.
    $tripal_import_count_query = $this->connection->select('tripal_import', 'o')
      ->countQuery()->execute()->fetchField();
    $this->assertEquals($tripal_import_count_query, 1, 'The drupal tripal_import table doesn\'t contain the record even though a rollback on chado should not effect Drupal.');
  }

  public function testTransactionCommit() {
    // Override the run() method of our mock importer to:
    // 1. Insert a record into the chado.organism table
    // This mimics the situation where a database transaction
    // should occur to completion
    $this->importer
      ->method('run')
      ->willReturnCallback(function (): bool {
        $connection = \Drupal::service('tripal_chado.database');
        $connection->insert('1:organism')
          ->fields([
            'genus' => 'Tripalus',
            'species' => 'databasica'
          ])
          ->execute();
          return true;
      });
    $logger = \Drupal::service("tripal.logger");

    // Try running our importer to ensure NO exception is thrown
    $exception_caught = FALSE;
    try {
      tripal_run_importer_run($this->importer, $logger);
    } catch (\Exception $e) {
      $exception_caught = TRUE;
    }
    $this->assertFalse($exception_caught, 'Caught an exception that should not have been thrown from overriding the run() method.');

    // Now query organism table to ensure the database transaction was
    // successfully committed
    $organism_count_query = $this->connection->select('1:organism', 'o')
      ->countQuery()->execute()->fetchField();
    $this->assertEquals($organism_count_query, 1, 'The chado.organism table does not contain one record as expected from overriding the run() method.');
  }

}
