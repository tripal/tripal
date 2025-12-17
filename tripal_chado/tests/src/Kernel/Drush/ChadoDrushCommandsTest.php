<?php

namespace Drupal\Tests\tripal\Kernel\Drush;

use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Commands\ChadoManageCommands;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Services\ChadoMviewsManager;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Psr\Log\LoggerInterface;

/**
 * Tests the drush command to populate materialized views.
 *
 * @group drush-command
 */
#[Group('drush-command')]
#[RunTestsInSeparateProcesses]
class ChadoDrushCommandsTest extends ChadoTestKernelBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  /**
   * The database connection to the test chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * Materialized view service.
   *
   * @var Drupal\tripal_chado\Services\ChadoMviewsManager
   */
  protected ChadoMviewsManager $mview_manager;

  /**
   * An object of the chado drush commands class.
   *
   * @var Drupal\tripal_chado\Commands\ChadoManageCommands
   */
  protected ChadoManageCommands $drush_command;

  /**
   * Stores output from the mock logger, accessed using getLogOutput().
   *
   * @var string
   */
  protected string $log_output = '';

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Initialize services.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
    $this->mview_manager = \Drupal::service('tripal_chado.materialized_views');

    // Install needed schemas.
    $this->installSchema('tripal_chado', ['tripal_custom_tables', 'tripal_mviews']);

    // Create a mock logger to access log output.
    $mock_logger = $this->getMockBuilder(LoggerInterface::class)
      ->getMock();
    $mock_logger->method('notice')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message;
          return NULL;
      });
    $mock_logger->method('error')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message;
          return NULL;
      });

    // An instance of the chado drush command class.
    $this->drush_command = new ChadoManageCommands();
    $this->drush_command->setLogger($mock_logger);

  }

  /**
   * Gets stored mocked log output and then resets it.
   */
  protected function getLogOutput(): string {
    $output = $this->log_output;
    $this->log_output = '';
    return $output;
  }

  /**
   * Test the drush command to populate materialized views.
   */
  public function testDrushMviewPopulate() {

    // Drush command without required parameter.
    $this->drush_command->populateMview('', []);
    $this->assertStringContainsString('Provide a materialized view name', $this->getLogOutput(),
      'View name is a required parameter');

    // Try an invalid schema.
    $this->drush_command->populateMview('cv_root_mview', ['schema-name' => 'anyinvalidschemaname']);
    $this->assertStringContainsString('The schema "anyinvalidschemaname" does not exist', $this->getLogOutput(),
      'An invalid schema name generates an error');

    // So far no materialized views exist.
    $this->drush_command->populateMview('cv_root_mview, db2cv_mview', ['schema-name' => $this->testSchemaName]);
    $this->assertStringContainsString('No materialized views exist', $this->getLogOutput(),
      'No materialized views exist prior to initialization');

    // Test the --list option with no views present.
    $this->drush_command->populateMview('', ['schema-name' => $this->testSchemaName, 'list' => TRUE]);
    $this->assertStringContainsString('No materialized views exist', $this->getLogOutput(),
      'Expected log message should show that no views exist');

    // Create a materialized view, will start out empty.
    $this->createMviews();
    $nrecords = $this->countViewRecords('db2cv_mview');
    $this->assertEquals(0, $nrecords, 'View should exist and be empty');

    $this->drush_command->populateMview('db2cv_mview', ['schema-name' => $this->testSchemaName]);
    $this->assertStringContainsString('Populating "db2cv_mview"', $this->getLogOutput(),
      'Expected log message should be generated');
    $nrecords = $this->countViewRecords('db2cv_mview');
    // As of test creation, expect 30 records, but this could change
    // if we add dbs in the future.
    $this->assertGreaterThanOrEqual(30, $nrecords,
      'At least 30 records should have been populated in the db2cv_mview');

    // Test the --list option with a view present.
    $this->drush_command->populateMview('', ['schema-name' => $this->testSchemaName, 'list' => TRUE]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('The following materialized views exist', $command_output,
      'Expected log message should list materialized views');
    $this->assertStringContainsString('db2cv_mview', $command_output,
      'Expected log message should list materialized views');

    // Test the --all option.
    $this->deleteViewRecords('db2cv_mview');
    $nrecords = $this->countViewRecords('db2cv_mview');
    $this->assertEquals(0, $nrecords, 'View should have been cleared');
    $this->drush_command->populateMview('', ['schema-name' => $this->testSchemaName, 'all' => TRUE]);
    $this->assertStringContainsString('Populating "db2cv_mview"', $this->getLogOutput(),
      'Expected log message should be generated');
    $nrecords = $this->countViewRecords('db2cv_mview');
    // Expect 30 records again.
    $this->assertGreaterThanOrEqual(30, $nrecords,
      'At least 30 records should have been repopulated in the db2cv_mview');

    // Test the --time option.
    $this->drush_command->populateMview('db2cv_mview', ['schema-name' => $this->testSchemaName, 'time' => TRUE]);
    $this->assertStringContainsString('Elapsed time', $this->getLogOutput(),
      'Elapsed time should be included if --time option is specified.');

    // Test populating a view that does not exist.
    $this->drush_command->populateMview('db2cv_mview, bogus_mview', ['schema-name' => $this->testSchemaName]);
    $this->assertStringContainsString('does not exist', $this->getLogOutput(),
      'An error should be included for a materialized view that does not exist.');
  }

  /**
   * Helper function to count the number of records in a materialized view.
   *
   * @param string $view_name
   *   The name of the materialized view.
   *
   * @return int
   *   The number of records in the materialized view.
   */
  private function countViewRecords(string $view_name): int {
    $count = $this->chado_connection
      ->select('1:' . $view_name, 'v')
      ->fields('v', ['*'])
      ->countQuery()
      ->execute()
      ->fetchField();
    return $count;
  }

  /**
   * Helper function to empty a materialized view.
   *
   * @param string $view_name
   *   The name of the materialized view.
   *
   * @return void
   *   No return value.
   */
  private function deleteViewRecords(string $view_name): void {
    $this->chado_connection
      ->delete('1:' . $view_name)
      ->execute();
  }

  /**
   * Helper function to create some simple materialized views.
   *
   * @return void
   *   No return value.
   */
  private function createMviews(): void {
    // @todo If chado had materialized views defined in a yaml file, we could
    // use that and not need to duplicate the definition here in the test.
    $view_name = 'db2cv_mview';
    $comment = 'A table for quick lookup of the vocabularies and the databases they are associated with.';
    $schema = [
      'table' => $view_name,
      'description' => $comment,
      'fields' => [
        'cv_id' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'cvname' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'db_id' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
        'dbname' => [
          'type' => 'varchar',
          'length' => '255',
          'not null' => TRUE,
        ],
        'num_terms' => [
          'type' => 'int',
          'not null' => TRUE,
        ],
      ],
      'indexes' => [
        'cv_id_idx' => ['cv_id'],
        'cvname_idx' => ['cvname'],
        'db_id_idx' => ['db_id'],
        'dbname_idx' => ['db_id'],
      ],
    ];
    $sql = "
      SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname,
        COUNT(CVT.cvterm_id) as num_terms
      FROM cv CV
        INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
        INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN db DB on DB.db_id = DBX.db_id
      WHERE CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
      GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name
      ORDER BY DB.name
    ";

    $schema_name = $this->chado_connection->getSchemaName();
    $mview = $this->mview_manager->create($view_name, $schema_name);
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
    $mview->setLocked(TRUE);
    // The actual db table already exists in the test chado, but not
    // as a mview, so let's clear out any existing values to start clean.
    $this->deleteViewRecords($view_name);
  }

}
