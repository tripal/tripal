<?php

namespace Drupal\Tests\tripal_chado\Kernel\Drush;

use Drupal\pgsql\Driver\Database\pgsql\Connection;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\tripal\TripalBackendPublish\Exceptions\TripalPublishException;
use Drupal\tripal\TripalBackendPublish\PluginManager\TripalBackendPublishManager;
use Drupal\tripal_biodb\Exception\ParameterException;
use Drupal\tripal_chado\Commands\ChadoManageCommands;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Plugin\TripalBackendPublish\ChadoPublish;
use Drupal\tripal_chado\Services\ChadoMviewsManager;
use Drupal\tripal_chado\Task\ChadoApplyMigrations;
use Drupal\tripal_chado\Task\ChadoInstaller;
use Drupal\tripal_chado\Task\ChadoPreparer;
use Drupal\tripal_chado\Task\ChadoRemover;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Psr\Log\LoggerInterface;
use Symfony\Component\Console\Output\OutputInterface;

/**
 * Tests the drush command to populate materialized views.
 *
 * @group drush-command
 */
#[Group('drush-command')]
#[RunTestsInSeparateProcesses]
class ChadoDrushCommandsTest extends ChadoTestKernelBase {

  use UserCreationTrait;

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'field', 'file', 'tripal', 'tripal_chado', 'views'];

  /**
   * The database connection to the drupal public schema.
   *
   * @var Drupal\pgsql\Driver\Database\pgsql\Connection
   */
  protected Connection $drupal_connection;

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
   * Stores the name of the test chado schema.
   *
   * @var string
   */
  protected string $test_schema;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Initialize services.
    $this->drupal_connection = \Drupal::database();
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
    $this->mview_manager = \Drupal::service('tripal_chado.materialized_views');

    // Install needed schemas.
    $this->installSchema('tripal', ['tripal_id_space_collection', 'tripal_vocabulary_collection', 'tripal_import']);
    $this->installSchema('tripal_chado', ['tripal_custom_tables', 'tripal_mviews', 'tripal_cv_obo', 'chado_migrations']);
    $this->installConfig('system');
    $this->installConfig('tripal_chado');
    $this->installEntitySchema('user');

    // Create and log-in a user.
    $user = $this->setUpCurrentUser();

    // Store the test schema name for easy access.
    $this->test_schema = $this->chado_connection->getSchemaName();

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

    // Create a mock output to access output.
    $mock_output = $this->createMock(OutputInterface::class);
    $mock_output->method('writeln')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message;
          return NULL;
      });

    // Create a mock chado installer.
    $mock_installer = $this->createMock(ChadoInstaller::class);
    $mock_installer->expects($this->once())
      ->method('setParameters')
      ->with($this->equalTo(['output_schemas' => ['blueteapot'], 'version' => '1.3']));
    $mock_installer->expects($this->once())
      ->method('performTask')
      ->willReturn(TRUE);

    // Create a mock chado preparer.
    $mock_preparer = $this->createMock(ChadoPreparer::class);
    $mock_preparer->expects($this->once())
      ->method('setParameters')
      ->with($this->equalTo(['output_schemas' => [$this->test_schema]]));
    $mock_preparer->expects($this->once())
      ->method('performTask')
      ->willReturn(TRUE);

    // Create a mock chado migrator.
    $mock_migrator = $this->createMock(ChadoApplyMigrations::class);
    $mock_migrator->expects($this->once())
      ->method('setParameters');
    $mock_migrator->expects($this->once())
      ->method('lookupInstallID')
      ->willReturn(1);
    $mock_migrator->expects($this->once())
      ->method('checkMigrationStatus')
      ->willReturn(['1.3.3.013' => (object) [
        'version' => '1.3.3.013',
        'description' => 'Harmonizes linker tables involving contacts and projects by adding a type_id and rank column.',
        'filename' => 'V1.3.3.013__add_type_id_to_contact_and_project_linkers.sql',
        'schema_name' => $this->test_schema,
        'install_id' => NULL,
        'applied_on' => NULL,
        'success' => NULL,
        'status' => 'Pending',
      ]]);
    $mock_migrator->expects($this->once())
      ->method('performTask')
      ->willReturnOnConsecutiveCalls(FALSE, TRUE);

    // Create a mock chado remover.
    $mock_remover = $this->createMock(ChadoRemover::class);
    $mock_remover->expects($this->exactly(2))
      ->method('setParameters');
    $mock_remover->expects($this->exactly(2))
      ->method('performTask')
      ->willReturnOnConsecutiveCalls(FALSE, TRUE);

    // Create a mock publish manager returning a mock instance.
    $mock_publish_instance = $this->createMock(ChadoPublish::class);
    $mock_publish_instance->method('publish')
      ->willReturnCallback(function ($options) {
          $this->log_output .= 'mock_publish_instance options: ' . print_r($options, TRUE);
          return [];
      });
    $mock_publish = $this->createMock(TripalBackendPublishManager::class);
    $mock_publish->method('createInstance')
      ->willReturn($mock_publish_instance);

    // An instance of the tripal_chado drush command class.
    $this->drush_command = new ChadoManageCommands(
      $this->container->get('config.factory'),
      $this->container->get('date.formatter'),
      $mock_publish,
      $this->container->get('tripal.dbx'),
      $mock_migrator,
      $this->container->get('tripal_chado.database'),
      $mock_installer,
      $this->container->get('tripal_chado.integrator'),
      $this->container->get('tripal_chado.materialized_views'),
      $mock_preparer,
      $mock_remover,
    );
    $this->drush_command->setLogger($mock_logger);
    $this->drush_command->setOutput($mock_output);
  }

  /**
   * Retrieves stored mocked log output and then resets it.
   */
  protected function getLogOutput(): string {
    $output = $this->log_output;
    $this->log_output = '';
    return $output;
  }

  /**
   * Test all of the Tripal Chado drush commands.
   */
  public function testTripalChadoCommands() {

    // Case: tripal-chado:setup-tests alias trp-prep-tests.
    $this->drush_command->setuptests();
    $this->assertStringContainsString('There is no longer any need to prepare the chado test environment', $this->getLogOutput(),
      'Legacy command only prints an error message');

    // Case: tripal-chado:install-chado alias trp-install-chado (mocked).
    // Note that a real install also sets the installed schema as default.

    $this->drush_command->installChado(['schema-name' => 'blueteapot', 'chado-version' => '1.3']);
    $this->assertStringContainsString('Chado was successfully installed', $this->getLogOutput(),
      'Chado installs with valid parameters');

    // Case: tripal-chado:set_schema_name alias trp-set-default.
    $this->drush_command->setDefault([]);
    $this->assertStringContainsString('The "schema-name" parameter is required', $this->getLogOutput(),
      'Set default requires schema-name parameter');

    $this->drush_command->setDefault(['schema-name' => $this->test_schema]);
    $this->assertStringContainsString('Successfully set the schema "' . $this->test_schema . '" to be default', $this->getLogOutput(),
      'Can set default schema with valid schema-name');

    // Case: tripal-chado:add_to_tripal alias trp-add-chado
    $this->drush_command->addToTripal(['schema-name' => $this->test_schema]);
    $this->assertStringContainsString('Successfully added the Chado schema "' . $this->test_schema . '" to Tripal', $this->getLogOutput(),
      'Can add schema to tripal with valid schema-name');
    $caught = FALSE;
    try {
      $this->drush_command->addToTripal(['schema-name' => $this->test_schema]);
    }
    catch (ParameterException $e) {
      $this->log_output .= $e->getMessage();
      $caught = TRUE;
    }
    $this->assertTrue($caught, 'Existing schema throws a ParameterException exception');
    $this->assertStringContainsString('The schema "' . $this->test_schema . '" is already integrated into Tripal and does not need to be imported', $this->getLogOutput(),
      'Cannot add schema to tripal a second time');

    // Case: tripal-chado:prepare alias trp-prep-chado (mocked).
    $this->drush_command->prepareChado(['schema-name' => $this->test_schema]);
    $this->assertStringContainsString('Preparation complete', $this->getLogOutput(),
      'Can prepare schema with valid schema-name');

    // Case: tripal-chado:migrate-chado alias trp-migrate-chado (mocked).
    $this->drush_command->migrateChado(['schema-name' => 'greenteapot']);
    $this->assertStringContainsString('The schema "greenteapot" does not exist and therefore cannot be migrated', $this->getLogOutput(),
      'Migrating a nonexistent schema causes an error');
    $this->drush_command->migrateChado(['schema-name' => $this->test_schema, 'yes' => 1]);
    $this->assertStringContainsString('1.3.3.013', $this->getLogOutput(),
      'Migrating an existing schema is successful');

    // Case: tripal-chado:drop-chado alias trp-drop-chado (mocked).
    $this->drush_command->dropChado(['schema-name' => 'redteapot']);
    $this->assertStringContainsString('Unable to drop chado schema "redteapot"', $this->getLogOutput(),
      'Dropping a non-existent schema will fail');
    $this->drush_command->dropChado(['schema-name' => $this->test_schema]);
    $this->assertStringContainsString('Chado schema "' .$this->test_schema  . '" was successfully dropped', $this->getLogOutput(),
      'Can drop a chado schema using its schema-name');

    // Case: tripal-chado:publish alias trp-chado-publish (mocked).
    // We only test the validations provided by the drush command.
    $this->drush_command->publish('study', ['schema-name' => $this->test_schema, 'migration-file' => 'xxx']);
    $this->assertStringContainsString('The specified migration file "xxx" does not exist', $this->getLogOutput(),
      'Publish with invalid migration file should fail');
    $this->drush_command->publish('study', ['schema-name' => $this->test_schema, 'migration-file' => __FILE__, 'republish' => 1]);
    $this->assertStringContainsString('The options --republish and --migration-file cannot be combined', $this->getLogOutput(),
      'Publish with valid migration file but also republish flag should fail');
    $this->drush_command->publish('study', ['schema-name' => $this->test_schema, 'migration-file' => __FILE__]);
    $this->assertStringContainsString('mock_publish_instance', $this->getLogOutput(),
      'Publish with valid migration file should succeed');

    // Case: tripal-chado:unpublish alias trp-chado-unpublish (mocked).
    $this->drush_command->unpublish('study', ['schema-name' => $this->test_schema]);
    $this->assertStringContainsString('mock_publish_instance', $this->getLogOutput(),
      'Unpublish with valid bundle should succeed');
    $this->drush_command->unpublish('', ['all' => 1]);
    $this->assertStringContainsString('mock_publish_instance', $this->getLogOutput(),
      'Unpublish all without bundle should succeed');

    // Case: tripal-chado:populate-mview alias trp-pop-mview.
    // Drush command without required parameter.
    $this->drush_command->populateMview('', []);
    $this->assertStringContainsString('Provide a materialized view name', $this->getLogOutput(),
      'View name is a required parameter');

    $this->drush_command->populateMview('cv_root_mview', ['schema-name' => 'anyinvalidschemaname']);
    $this->assertStringContainsString('The schema "anyinvalidschemaname" does not exist', $this->getLogOutput(),
      'An invalid schema name generates an error');

    $this->drush_command->populateMview('cv_root_mview, db2cv_mview', ['schema-name' => $this->testSchemaName]);
    $this->assertStringContainsString('No materialized views exist', $this->getLogOutput(),
      'No materialized views exist prior to initialization');

    $this->drush_command->populateMview('', ['schema-name' => $this->testSchemaName, 'list' => TRUE]);
    $this->assertStringContainsString('No materialized views exist', $this->getLogOutput(),
      'With --list option, log message should show that no views exist');

    $this->createMviews();
    $nrecords = $this->countViewRecords('db2cv_mview');
    $this->assertEquals(0, $nrecords,
      'Materialized view should exist and be empty');

    $this->drush_command->populateMview('db2cv_mview', ['schema-name' => $this->testSchemaName]);
    $this->assertStringContainsString('Populating "db2cv_mview"', $this->getLogOutput(),
      'Expected log message should be generated when populating materialized view');
    $nrecords = $this->countViewRecords('db2cv_mview');
    // As of test creation, expect 30 records, but this could change
    // if we add dbs in the future.
    $this->assertGreaterThanOrEqual(30, $nrecords,
      'At least 30 records should have been populated in the db2cv_mview');

    $this->drush_command->populateMview('', ['schema-name' => $this->testSchemaName, 'list' => TRUE]);
    $command_output = $this->getLogOutput();
    $this->assertStringContainsString('The following materialized views exist', $command_output,
      'With --list option, log message should list materialized views');
    $this->assertStringContainsString('db2cv_mview', $command_output,
      'With --list option, log message should list materialized views');

    $this->deleteViewRecords('db2cv_mview');
    $nrecords = $this->countViewRecords('db2cv_mview');
    $this->assertEquals(0, $nrecords, 'View should have been cleared');
    $this->drush_command->populateMview('', ['schema-name' => $this->testSchemaName, 'all' => TRUE]);
    $this->assertStringContainsString('Populating "db2cv_mview"', $this->getLogOutput(),
      'With --all option, expected log message should be generated');
    $nrecords = $this->countViewRecords('db2cv_mview');
    // Expect 30 records again.
    $this->assertGreaterThanOrEqual(30, $nrecords,
      'At least 30 records should have been repopulated in the db2cv_mview');

    $this->drush_command->populateMview('db2cv_mview', ['schema-name' => $this->testSchemaName, 'time' => TRUE]);
    $this->assertStringContainsString('Elapsed time', $this->getLogOutput(),
      'With --time option, elapsed time should be included.');

    $this->drush_command->populateMview('db2cv_mview, bogus_mview', ['schema-name' => $this->testSchemaName]);
    $this->assertStringContainsString('does not exist', $this->getLogOutput(),
      'An error should be included when populating a materialized view that does not exist.');
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

    $mview = $this->mview_manager->create($view_name, $this->test_schema);
    $mview->setTableSchema($schema);
    $mview->setSqlQuery($sql);
    $mview->setComment($comment);
    $mview->setLocked(TRUE);
    // The actual db table already exists in the test chado, but not
    // as a mview, so let's clear out any existing values to start clean.
    $this->deleteViewRecords($view_name);
  }

}

