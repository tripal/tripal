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
   * An object of a drush commands class.
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
   * Name of the Drupal user for testing.
   *
   * @var string
   */
  protected string $username;

  /**
   * A tripal job for testing.
   *
   * @var int
   */
  protected int $job_id;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Install items required for these tests.
    $this->installEntitySchema('user');
    $this->installSchema('tripal', ['tripal_jobs']);
    $this->installConfig('system');

    // Create and log-in a user.
    $user = $this->setUpCurrentUser();
    $this->username = \Drupal::currentUser()->getAccountName();

    // Create a mock logger to access log output.
    $mock_logger = $this->getMockBuilder(LoggerInterface::class)
      ->getMock();
    $mock_logger->method('notice')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message . "\n";
          return NULL;
      });
    $mock_logger->method('error')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message . "\n";
          return NULL;
      });

    // Create a mock output to access output.
    $mock_output = $this->createMock(OutputInterface::class);
    $mock_output->method('writeln')
      ->willReturnCallback(function ($message, $options) {
          $this->log_output .= $message . "\n";
          return NULL;
      });

    // An instance of the TripalCommands drush command class.
    $this->drush_command = new TripalCommands(
      $this->container->get('account_switcher'),
      $this->container->get('tripal.tripalentitytype_collection'),
      $this->container->get('tripal.tripalfield_collection'),
      $this->container->get('tripal.sync_tripal_field_storage'),
    );
    $this->drush_command->setLogger($mock_logger);
    $this->drush_command->setOutput($mock_output);

    // Create a tripal job.
    $details = [
      'job_name' => 'Do-nothing job',
      'modulename' => 'tripal',
      'callback' => '\Drupal\Tests\tripal\Kernel\Services\TripalJob\FakeClasses\callableClassForTripalJobs::myCallbackMethod',
      'arguments' => [],
      'uid' => $user->id(),
    ];
    $this->job_id = \Drupal::service('tripal.job')->create($details);
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
   * Test all of the Tripal drush commands.
   */
  public function testTripalCommands() {

    // Case: tripal:version
    // This is the simplest drush commmand, just outputs version.
    $expected_version = tripal_version() . "\n";
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

    // Tripal jobs write directly to the console! Need to capture it.
    ob_start();
    $this->drush_command->runJobs(['username' => $this->username]);
    $this->log_output .= ob_get_clean();
    $this->assertStringContainsString('Tripal Job Launcher', $this->getLogOutput(),
      'All jobs launch with a valid user');

    $this->drush_command->runJobs(['username' => $this->username, 'max_jobs' => 2, 'parallel' => 1]);
    $output = $this->getLogOutput();
    $this->assertStringContainsString('Maximum number of jobs is 2', $output,
      'max_jobs parameter should work');
    $this->assertStringContainsString('in parallel', $output,
      'parallel parameter should work');

    $this->drush_command->runJobs(['username' => $this->username, 'job_id' => '123NotValidInt456']);
    $this->assertStringContainsString('The --job_id argument must be a positive integer', $this->getLogOutput(),
      'Invalid job ID generates an error');

    // Case: tripal:trp-rerun-job.
    $this->drush_command->rerunJob([]);
    $this->assertStringContainsString('The --username argument is required', $this->getLogOutput(),
      'Need a user name to run a job');

    $this->drush_command->rerunJob(['username' => 'fakeuser']);
    $this->assertStringContainsString('The --username argument does not specify a valid user', $this->getLogOutput(),
      'Need a valid user name to run a job');

    $this->drush_command->rerunJob(['username' => $this->username]);
    $this->assertStringContainsString('The --job_id argument is required', $this->getLogOutput(),
      'Need a job_id to rerun a job');

    $this->drush_command->rerunJob(['username' => $this->username, 'job_id' => '123NotValidInt456']);
    $this->assertStringContainsString('The --job_id argument must be a positive integer', $this->getLogOutput(),
      'Invalid job ID generates an error');

    $caught = FALSE;
    try {
      $this->drush_command->rerunJob(['username' => $this->username, 'job_id' => '1000']);
    }
    catch (\Exception $e) {
      $caught = TRUE;
    }
    $this->assertTrue($caught, 'An invalid job_id throws an exception');

    // Tripal jobs write directly to the console! Need to capture it.
    ob_start();
    $this->drush_command->rerunJob(['username' => $this->username, 'job_id' => $this->job_id]);
    $this->log_output .= ob_get_clean();
    $this->assertStringContainsString('Tripal Job Launcher', $this->getLogOutput(),
      'A valid job_id can be rerun');

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
    // The underlying service is functionally tested elsewhere by
    // SyncTripalFieldStorageTest.php.
    $this->drush_command->tripalSyncFieldSchema([]);
    $this->assertStringContainsString('No discrepancies found.', $this->getLogOutput(),
      'No discrepancies expected in the test environment');
  }

}
