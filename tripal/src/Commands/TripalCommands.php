<?php

namespace Drupal\tripal\Commands;

use Drupal\Core\Session\AccountSwitcherInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\tripal\Services\SyncTripalFieldStorage;
use Drupal\tripal\Services\TripalEntityTypeCollection;
use Drupal\tripal\Services\TripalFieldCollection;
use Drush\Attributes as CLI;
use Drush\Commands\DrushCommands;

/**
 * Drush commands.
 */
class TripalCommands extends DrushCommands {

  use StringTranslationTrait;

  /**
   * TripalCommands Drush command class constructor.
   *
   * This is used to inject the services used by the various commands.
   */
  public function __construct(
    protected AccountSwitcherInterface $accountSwitcherService,
    protected TripalEntityTypeCollection $entityTypeCollectionService,
    protected TripalFieldCollection $fieldCollectionService,
    protected SyncTripalFieldStorage $syncFieldStorageService,
  ) {
    // Parent currently doesn't do anything here.
    parent::__construct();
  }

  /**
   * Helper function to set the proper user when running the drush command.
   *
   * @param string|null $uname
   *   The name of the user to be switched to.
   *
   * @return bool
   *   True if successful, false if not.
   */
  protected function switchUser(?string $uname): bool {
    if (!$uname) {
      $this->logger->error($this->t('The --username argument is required.'));
      return FALSE;
    }

    $user = user_load_by_name($uname);
    if (!$user) {
      $this->logger->error($this->t('The --username argument does not specify a valid user.'));
      return FALSE;
    }
    $this->accountSwitcherService->switchTo($user);
    return TRUE;
  }

  /**
   * Executes one or more jobs in the Tripal Jobs Queue.
   *
   * @param array $options
   *   Options passed on the drush command line.
   */
  #[CLI\Command(name: 'tripal:trp-run-jobs', aliases: ['trp-run-jobs'])]
  #[CLI\Option(name: 'parallel', description: 'Set to 1 if the job is allowed to run in parallel with other Tripal jobs.')]
  #[CLI\Option(name: 'job_id', description: 'The numeric ID of the job. If no job ID is provided then all of the jobs waiting in the queue will be run.')]
  #[CLI\Option(name: 'max_jobs', description: 'The maximum number of jobs that should be run concurrently. If -1 then unlimited.')]
  #[CLI\Option(name: 'single', description: 'Ensures only a single job is run rather than the entire queue.')]
  #[CLI\Option(name: 'username', description: 'The name of the user for whom the job run is associated.')]
  #[CLI\Usage(
    name: 'drush trp-run-jobs --username=[USERNAME]',
    description: 'Executes all jobs waiting in the queue and associates the runs with the provided user.',
  )]
  #[CLI\Usage(
    name: 'drush trp-run-jobs --job_id=[JOB_ID] --username=[USERNAME]',
    description: 'Executes a job, using the provided job ID and associates the run with the provided user.',
  )]
  public function runJobs(
    array $options = [
      'username' => NULL,
      'job_id' => NULL,
      'parallel' => 0,
      'max_jobs' => -1,
      'single' => 0,
    ],
  ) {

    $parallel = $options['parallel'] ?? 0;
    $job_id = $options['job_id'] ?? NULL;
    $max_jobs = $options['max_jobs'] ?? -1;
    $single = $options['single'] ?? 0;
    $uname = $options['username'] ?? NULL;

    if (!$this->switchUser($uname)) {
      return;
    }

    $this->output()->writeln("\n" . date('Y-m-d H:i:s'));
    $this->output()->writeln('Tripal Job Launcher' . ($parallel ? ' (in parallel)' : ''));
    if ($max_jobs !== -1) {
      $this->output()->writeln("Maximum number of jobs is " . $max_jobs);
    }
    $this->output()->writeln("Running as user '$uname'");
    $this->output()->writeln("-------------------");
    tripal_launch_job($parallel, $job_id, $max_jobs, $single);
  }

  /**
   * Reruns a job in the Tripal Jobs queue.
   *
   * @param array $options
   *   Options passed on the drush command line.
   */
  #[CLI\Command(name: 'tripal:trp-rerun-job', aliases: ['trp-rerun-job'])]
  #[CLI\Option(name: 'parallel', description: 'Set to 1 if the job is allowed to run in parallel with other Tripal jobs.')]
  #[CLI\Option(name: 'job_id', description: 'The numeric ID of the job. If no job ID is provided then all of the jobs waiting in the queue will be run.')]
  #[CLI\Option(name: 'max_jobs', description: 'The maximum number of jobs that should be run concurrently. If -1 then unlimited.')]
  #[CLI\Option(name: 'single', description: 'Ensures only a single job is run rather than the entire queue.')]
  #[CLI\Option(name: 'username', description: 'The name of the user for whom the job run is associated.')]
  #[CLI\Usage(
    name: 'drush trp-run-job --job_id=[JOB_ID] --username=[USERNAME]',
    description: 'Re-runs a job by first resubmitting it then executing it.',
  )]
  public function rerunJob(
    array $options = [
      'username' => NULL,
      'job_id' => NULL,
      'parallel' => 0,
      'max_jobs' => -1,
      'single' => 0,
    ],
  ) {

    $parallel = $options['parallel'] ?? 0;
    $job_id = $options['job_id'] ?? NULL;
    $max_jobs = $options['max_jobs'] ?? -1;
    $single = $options['single'] ?? 0;
    $uname = $options['username'] ?? NULL;

    if (!$job_id) {
      $this->logger->error($this->t('The --job_id argument is required.'));
      return;
    }

    if (!$this->switchUser($uname)) {
      return;
    }

    $new_job_id = tripal_rerun_job($job_id, FALSE);

    $this->output()->writeln("\n" . date('Y-m-d H:i:s'));
    $this->output()->writeln('Tripal Job Launcher' . ($parallel ? ' (in parallel)' : ''));
    $this->output()->writeln("Running as user '$username'");
    $this->output()->writeln("-------------------");
    tripal_launch_job($parallel, $new_job_id, $max_jobs, $single);
  }

  /**
   * Returns the current version of Tripal that is installed.
   */
  #[CLI\Command(name: 'tripal:version', aliases: ['trp-version'])]
  #[CLI\Usage(
    name: 'drush trp-version',
    description: 'Returns the current Tripal version string.',
  )]
  public function tripalVersion() {
    // Don't use logger here so that output is plain since this
    // could be used in a script.
    $this->output()->writeln(tripal_version());
  }

  /**
   * Imports a collection of Tripal Content Types and associated fields.
   *
   * Requires specifying a username and a specific collection id.
   *
   * @param array $options
   *   Options passed on the drush command line.
   */
  #[CLI\Command(name: 'tripal:trp-import-types', aliases: ['trp-import-types'])]
  #[CLI\Option(
    name: 'collection_id',
    description: 'The id specified in the YAML file for the particular TripalEntityType-Collection you would like to import.
      Note: fields will also be added automatically if the TripalField-Collection YAML file has the same id.',
  )]
  #[CLI\Option(name: 'username', description: 'The name of the user for whom the content types created are associated.')]
  #[CLI\Usage(
    name: 'drush trp-import-types --username=[USERNAME] --collection_id=genomic_chado',
    description: 'Runs a job importing the genomic content types focused on a Chado backend.',
  )]
  public function tripalImportContentTypes(array $options = ['username' => NULL, 'collection_id' => NULL]) {

    if (!$options['username']) {
      $this->logger->error($this->t('The --username argument is required.'));
      return;
    }
    if (!$options['collection_id']) {
      $this->logger->error($this->t('The --collection_id argument is required.'));
      return;
    }

    // Check that the id supplied is valid.
    $collections = $this->entityTypeCollectionService->getTypeCollections();
    if (!array_key_exists($options['collection_id'], $collections)) {
      $this->logger->notice($this->t('The following collection identifiers are defined:'));
      foreach ($collections as $id => $details) {
        $this->logger->notice($this->t('- "@id" (@description)',
          ['@id' => $id, '@description' => $details['description']]));
      }
      $this->logger->error($this->t('The collection identifier "@id" is not valid. Please try again with one of the identifiers listed above (e.g. general_chado).',
        ['@id' => $options['collection_id']]));
      return;
    }

    $chosen_collection_ids = [$options['collection_id']];

    // Import the content types.
    $this->entityTypeCollectionService->install($chosen_collection_ids);

    // Import the fields.
    $this->fieldCollectionService->install($chosen_collection_ids);

  }

  /**
   * Ensures the Drupal field table schema is up to date for all fields.
   *
   * @command tripal:trp-sync-field-schema
   */
  #[CLI\Command(name: 'tripal:trp-sync-field-schema', aliases: [])]
  public function tripalSyncFieldSchema() {

    $this->logger->notice($this->t("Checking Tripal Entity types for discrepancies between field schema definitions and the underlying Drupal tables."));

    $columns_added = $this->syncFieldStorageService->resolveDifferences();

    foreach ($columns_added as $field_name => $field_differences) {
      $this->logger->notice($this->t('@fn needed @fd difference(s) fixed.',
        ['@fn' => $field_name, '@fd' => count($field_differences)]));
    }

    $fields_needing_updates = count($columns_added);
    $num_columns_added = array_sum(array_map("count", $columns_added));
    if ($fields_needing_updates > 0) {
      $this->logger->notice($this->t("Added @num_columns_added columns across @fields_needing_updates fields."),
        ['@num_columns_added' => $num_columns_added, '@fields_needing_updates' => $fields_needing_updates]);
    }
    else {
      $this->logger->notice($this->t("No discrepancies found."));
    }
  }

}
