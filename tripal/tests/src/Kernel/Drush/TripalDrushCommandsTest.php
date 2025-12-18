<?php

namespace Drupal\Tests\tripal\Kernel\Drush;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\tripal\Commands\TripalCommands;
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
class TripalDrushCommandsTest extends TripalTestKernelBase {

  use UserCreationTrait;

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  /**
   * An object of the chado drush commands class.
   *
   * @var Drupal\tripal\Commands\TripalCommands
   */
  protected TripalCommands $drush_command;

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

    $this->installEntitySchema('user');
    $this->installSchema('tripal', ['tripal_jobs']);

    // Create and log-in a user.
    $this->setUpCurrentUser();

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

    // Create a mock io to access its output.
    $mock_io = $this->createMock(OutputInterface::class);
    $mock_io->method('writeln')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message;
          return NULL;
      });

    // An instance of the tripal drush command class.
    $this->drush_command = $this->container->get('tripal.command');
    $this->drush_command->setLogger($mock_logger);
    $this->drush_command->setOutput($mock_io);
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
  public function testTripalCommands() {

    // Case: tripal:version
    // This is the simplest drush commmand, just outputs version.
    $expected_version = tripal_version();
    $this->drush_command->tripalVersion();
    $this->assertEquals($expected_version, $this->getLogOutput(),
      'Should retrieve the current tripal version');

    // Case: tripal:trp-run-job.
    $this->drush_command->runJobs([]);
    $this->assertStringContainsString('The --username argument is required', $this->getLogOutput(),
      'Need a user name to run a job');

    $this->drush_command->runJobs(['username' => 'fakeuser']);
    $this->assertStringContainsString('The --username argument does not specify a valid use', $this->getLogOutput(),
      'Need a valid user name to run a job');

    $username = \Drupal::currentUser()->getAccountName();
    $this->drush_command->runJobs(['username' => $username]);
    $this->assertStringContainsString('Tripal Job Launcher', $this->getLogOutput(),
      'All jobs launch with a valid user');

    $username = \Drupal::currentUser()->getAccountName();
    $this->drush_command->runJobs(['username' => $username, 'job_id' => '123NotValidInt456']);
    $this->assertStringContainsString('The --job_id argument must be a positive integer', $this->getLogOutput(),
      'Invalid job ID generates an error');

    // Case: tripal:trp-rerun-job.
    $this->drush_command->rerunJob([]);
    $this->assertStringContainsString('The --username argument is required', $this->getLogOutput(),
      'Need a user name to run a job');

    $this->drush_command->rerunJob(['username' => 'fakeuser']);
    $this->assertStringContainsString('The --username argument does not specify a valid use', $this->getLogOutput(),
      'Need a valid user name to run a job');

    $username = \Drupal::currentUser()->getAccountName();
    $this->drush_command->rerunJob(['username' => $username]);
    $this->assertStringContainsString('The --job_id argument is required', $this->getLogOutput(),
      'Need a job_id to rerun a job');

    $this->drush_command->rerunJob(['username' => $username, 'job_id' => '123NotValidInt456']);
    $this->assertStringContainsString('The --job_id argument must be a positive integer', $this->getLogOutput(),
      'Invalid job ID generates an error');

    $caught = FALSE;
    try {
      $this->drush_command->rerunJob(['username' => $username, 'job_id' => '1']);
    }
    catch (\Exception $e) {
      $caught = TRUE;
    }
    $this->assertTrue($caught, 'An invalid job_id throws an exception');

    // Case: tripal:trp-import-types.
    $this->drush_command->tripalImportContentTypes([]);
    $this->assertStringContainsString('The --collection_id argument is required', $this->getLogOutput(),
      'Need to specify a collection to import');

    // No types are defined in this test environment,
    // actual install gets tested elsewhere.
    $this->drush_command->tripalImportContentTypes(['collection_id' => 'wxyz']);
    $this->assertStringContainsString('The collection identifier "wxyz" is not valid', $this->getLogOutput(),
      'Need to specify a valid collection to import');

    // Case: tripal:trp-sync-field-schema.
    $this->drush_command->tripalSyncFieldSchema([]);
    $this->assertStringContainsString('No discrepancies found.', $this->getLogOutput(),
      'No discrepancies expected in the test environment');
  }

}
