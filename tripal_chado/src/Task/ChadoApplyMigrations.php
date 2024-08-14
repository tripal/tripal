<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;
use Drupal\Component\Serialization\Yaml;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal\Services\TripalJob;

/**
 * Applies Chado Migrations usually handled by Flyway.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of the Chado schema to apply migrations to.
 * $flyway = \Drupal::service('tripal_chado.apply_migrations');
 * $flyway->setParameters([
 *   'input_schemas' => ['chado'],
 * ]);
 * if (!$flyway->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoApplyMigrations extends ChadoTaskBase {

  /**
   * Name of the BioTask.
   * @var string
   */
  public const TASK_NAME = 'apply_migrations';

  /**
   * Default chado version.
   * @var string
   */
  public const BASELINE_CHADO_VERSION = '1.3';

  /**
   * The path + filename of the YAML describing the migrations.
   * This path is relative to the tripal_chado module directory.
   * @var string
   */
  public const MIGRATIONS_INFO_YAML = '/chado_schema/migrations/tripal_chado.chado_migrations.yml';

  /**
   * The path to the directory containing the chado migration SQL files.
   * This path is relative to the tripal_chado module directory.
   * @var string
   */
  public const MIGRATION_DIR = '/chado_schema/migrations/';

  /**
   * An array summarizing all available migrations and including the status
   * of each migration for this schema.
   *
   * This variable is set by checkMigrationStatus().
   *
   * @var array
   *  The key of this array is the migration version number and each element is
   *  an object with the following keys:
   *   - version: a string of the form '1.3.3.002'.
   *   - install_id: the ID of the chado installation as managed by tripal.
   *   - schema_name: the name of the schema the status applies to.
   *   - description: a short description of the migration.
   *   - applied_on: the date the migration was applied on.
   *   - success: an integer indicating the success of applying the migration.
   *       If successful, it will be '1' and otherwise, '0'. If the migration
   *       has not been attempted then this will be NULL.
   *   - status: a string indicating the status. It will be one of "Pending",
   *       "Successful", or "Failed".
   */
  public array $migration_status = [];

  /**
   * A TripalDBX connection to the chado database.
   * @var ChadoConnection
   */
  public ChadoConnection $chado_connection;

  /**
   * A Drupal connection to the drupal database/schema.
   * @var Connection
   */
  public $drupal_connection;

  /**
   * The job that the migrations are being applied during.
   * @var TripalJob
   */
  protected TripalJob $job;

  /**
   * The unique ID of the above saved Tripal job.
   * @var int
   */
  protected int $job_id;

  /**
   * The unique id of the Tripal-managed Chado installation.
   * @see Drupal chado_installations table.
   * @var int
   */
  protected int $install_id;

  /**
   * The cvterm_id for the chado_properties:version term.
   * @var int
   */
  protected int $version_cvterm_id;

  /**
   * Sets the Tripal job that the migrations are being applied during.
   *
   * @param TripalJob $job
   * @return void
   */
  public function setTripalJob(TripalJob $job) {
    $this->job = $job;
    $this->job_id = $job->getJobID();
  }

  /**
   * Sets the unique id of the Tripal-managed chado installation this biotask
   * is focused on.
   *
   * @param int $install_id
   * @return void
   */
  public function setInstallID(int $install_id) {
    $this->install_id = $install_id;
  }

  /**
   * A callable function to provide to tripal jobs as the callback.
   *
   * Simply sets up the biotask with the details saved in the job
   * and then performs the task.
   *
   * @param string $schema_name
   *   The schema to apply all pending migrations to.
   * @param int $install_id
   *   The unique IF od the Tripal-managed chado installation the
   *   schema represents.
   */
  public static function runTripalJob(string $schema_name, int $install_id, TripalJob $job) {
    $migrator = \Drupal::service('tripal_chado.apply_migrations');
    $migrator->setParameters([
      'input_schemas' => [$schema_name],
    ]);
    $migrator->setInstallID($install_id);
    $migrator->setTripalJob($job);
    if (!$migrator->performTask()) {
      \Drupal::logger('tripal_chado')->error(
        "Failed to apply migrations to the Chado schema '"
        . $schema_name
        . "'. See previous log messages for details."
      );
    }
  }

  /**
   * Gets the highest version number available in our migrations.
   *
   * @return string
   *   The version of the latest migration. It will be a string of the form
   *   1.3.3.005. If we are unable to get migrations or if there are not any
   *   migrations then the version returned will be the basline version (i.e. 1.3).
   */
  public static function getHighestVersion() {

    $migrations = self::getAvailableMigrations();
    if (is_array($migrations) && count($migrations) > 1) {
      $last_migration = end($migrations);

      if (property_exists($last_migration, 'version')) {
        return $last_migration->version;
      }
    }

    return self::BASELINE_CHADO_VERSION;
  }

  /**
   * Gets details for all the current migrations available.
   *
   * @return array
   *   An array of the migrations available where each element is an array
   *   with the keys filename, version, description.
   */
  public static function getAvailableMigrations() {
    $migration_info = [];

    $tripal_chado_path = \Drupal::service('extension.list.module')
      ->getPath('tripal_chado');
    $yaml_full_path = $tripal_chado_path . static::MIGRATIONS_INFO_YAML;

    $yaml_raw = file_get_contents($yaml_full_path);
    if ($yaml_raw) {
      $yaml = YAML::decode($yaml_raw);
      if (is_array($yaml) && array_key_exists('migrations', $yaml)) {
        $migration_info = $yaml['migrations'];
      }
      else {
        throw \Exception("Unable to decode the content of the $yaml_full_path YAML file and retrieve the 'migrations' key.");
      }
    }
    else {
      throw \Exception("Unable to retrieve the content of the $yaml_full_path YAML file which contains the migration info.");
    }

    // Now format it for easier consumption.
    $formatted = [];
    if ($migration_info) {
      foreach ($migration_info as $result) {
        $migration = new \StdClass();
        $migration->version = $result['version'];
        $migration->description = $result['description'];
        $migration->filename = $result['filename'];

        $formatted[ $result['version'] ] = $migration;
      }

    }

    return $formatted;
  }

  /**
   * Saves the status of the current migration to the database.
   *
   * @param object $migration
   *   - version: a string of the form '1.3.3.002'.
   *   - install_id: the ID of the chado installation as managed by tripal.
   *   - schema_name: the name of the schema the status applies to.
   *   - description: a short description of the migration.
   *   - applied_on: the date the migration was applied on.
   *   - success: an integer indicating the success of applying the migration.
   *       If successful, it will be '1' and otherwise, '0'. If the migration
   *       has not been attempted then this will be NULL.
   *   - status: a string indicating the status. It will be one of "Pending",
   *       "Successful", or "Failed".
   * @param bool $status
   * @return void
   */
  public function reportMigrationStatus(object $migration, bool $status) {

    $current_date = \Drupal::time()->getRequestTime();

    $short_status = 0;
    if ($status) {
      $short_status = 1;
    }

    // Always update the migration records
    $this->drupal_connection->insert('chado_migrations')
      ->fields([
        'job_id' => $this->job_id,
        'install_id' => $this->install_id,
        'version' => $migration->version,
        'filename' => $migration->filename,
        'applied_on' => $current_date,
        'success' => $short_status,
      ])
      ->execute();

    // Only update versions and what-not if the migration was successful.
    if ($short_status == 1) {
    // Update the chado installation table version.
      $this->drupal_connection->update('chado_installations')
        ->fields([
          'version' => $migration->version,
          'updated' => $current_date,
        ])
        ->condition('install_id', $this->install_id)
        ->execute();

      // We need the chado_properties:version term to update the version.
      $this->chado_connection->setSchemaName($migration->schema_name);
      if (empty($this->version_cvterm_id)) {
        $result = $this->chado_connection->select('1:cvterm', 'cvt')
          ->fields('cvt', ['cvterm_id']);
        $result->join('1:cv', 'cv', 'cv.cv_id = cvt.cv_id');
        $result->condition('cv.name', 'chado_properties');
        $result->condition('cvt.name', 'version');
        $result = $result->execute();
        $this->version_cvterm_id = $result->fetchField();
      }

      // Update this chado installations version.
      $this->chado_connection->update('1:chadoprop')
        ->fields([
          'value' => $migration->version,
        ])
        ->condition('type_id', $this->version_cvterm_id)
        ->execute();
    }
  }

  /**
   * Checks the status of this schema. Specifically, which migrations have been
   * applied and which are still pending.
   *
   * @return array
   *  The key of this array is the migration version number and each element is
   *  an object with the following keys:
   *   - version: a string of the form '1.3.3.002'.
   *   - install_id: the ID of the chado installation as managed by tripal.
   *   - schema_name: the name of the schema the status applies to.
   *   - description: a short description of the migration.
   *   - applied_on: the date the migration was applied on.
   *   - success: an integer indicating the success of applying the migration.
   *       If successful, it will be '1' and otherwise, '0'. If the migration
   *       has not been attempted then this will be NULL.
   *   - status: a string indicating the status. It will be one of "Pending",
   *       "Successful", or "Failed".
   */
  public function checkMigrationStatus() {
    $this->drupal_connection = \Drupal::service('database');
    $schema_name = $this->parameters['input_schemas'][0];

    // Get all the migration records for this chado installation.
    $query = $this->drupal_connection->select('chado_migrations', 'm')
      ->fields('m', ['version', 'applied_on', 'success']);
    $query->join('chado_installations', 'i', 'i.install_id = m.install_id');
    $query->fields('i', ['install_id']);
    $query->condition('i.schema_name', $schema_name);
    $applied_migrations = $query->execute()->fetchAllAssoc('version');

    // Get the list of possible migrations (schema indifferent).
    $all_migrations = self::getAvailableMigrations();
    foreach ($all_migrations as $version => $migration) {

      $migration->schema_name = $schema_name;

      // Add details if the migration was applied.
      if (array_key_exists($version, $applied_migrations)) {
        $migration->applied_on = $applied_migrations[$version]->applied_on;
        $migration->success = $applied_migrations[$version]->success;
        $migration->install_id = $applied_migrations[$version]->install_id;

        if ($applied_migrations[$version]->success == 1) {
          $migration->status = 'Successful';
        }
        else {
          $migration->status = 'Failed';
        }
      }
      else {
        $migration->install_id = NULL;
        $migration->applied_on = NULL;
        $migration->success = NULL;
        $migration->status = 'Pending';
      }

      $all_migrations[$version] = $migration;
    }

    return $all_migrations;
  }

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include one output
   * schema:
   * ```
   * ['input_schemas' => ['chado']]
   * ```
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in case of invalid parameters.
   */
  public function validateParameters() :void {

    // Check input.
    if (!empty($this->parameters['output_schemas'])) {
      throw new ParameterException(
        "Chado installer does not take output schemas as migrations are applied directly to an existing chado schema."
      );
    }
    // Check output.
    if (empty($this->parameters['input_schemas'])
        || (1 != count($this->parameters['input_schemas']))
    ) {
      throw new ParameterException(
        "Invalid number of input schemas. Only one input schema can be specified."
      );
    }
    $tripal_dbx = \Drupal::service('tripal.dbx');

    // Check if the source schema exists.
    $input_schema = $this->inputSchemas[0];
    if (!$input_schema->schema()->schemaExists()) {
      throw new ParameterException(
        'The source schema to apply migrations to (i.e. "'
        . $input_schema->getSchemaName()
        . '") does not exist. Please select an existing schema to apply migrations to.'
      );
    }
  }

  /**
   * Applies all outstanding migrations to a given chado instance.
   *
   * The migration procedure uses a set of SQL files where each migration is
   * in it's own file.
   *
   * Task parameter array provided to the class constructor includes:
   * - 'input_schemas' array: one input schema that must exist (required)
   * - 'output_schemas' array: no output schema as migrations are applied in place.
   *
   * Example:
   * ```
   * ['input_schemas' => ['chado']]
   * ```
   *
   * @return bool
   *   TRUE if the task was performed with success and FALSE if the task was
   *   completed but without the expected success.
   *
   * @throws Drupal\tripal_biodb\Exception\TaskException
   *   Thrown when a major failure prevents the task from being performed.
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   Thrown if parameters are incorrect.
   *
   * @throws Drupal\tripal_biodb\Exception\LockException
   *   Thrown when the locks can't be acquired.
   */
  public function performTask() :bool {
    // Task return status.
    $task_success = FALSE;

    // Validate parameters.
    $this->validateParameters();

    // Acquire locks.
    $success = $this->acquireTaskLocks();
    if (!$success) {
      throw new LockException("Unable to acquire all locks for task. See logs for details.");
    }

    try
    {
      // The schema to apply migrations to is the first input schema.
      $target_schema = $this->parameters['input_schemas'][0];

      // We will use ChadoConnection to apply the migration file.
      $this->chado_connection = \Drupal::service('tripal_chado.database');

      // We need the path to tripal_chado to get absolute paths to
      // the migration files.
      $path = \Drupal::service('extension.list.module')->getPath('tripal_chado') . static::MIGRATION_DIR;

      // Now for each migration, in order...
      $migrations = $this->checkMigrationStatus();
      foreach ($migrations as $migration) {

        if ($migration->success !== '1') {
          // Get the absolute path to this specific migration.
          $migration_file = $path . $migration->filename;
          // Apply the migration.
          $this->logger->notice("Applying '$migration_file' to schema '$target_schema'");
          $exception_message = 'No exception thrown.';
          try {
            $success = $this->chado_connection->executeSqlFile(
              $migration_file,
              ['chado' => $target_schema]
            );
          }
          catch (\Exception $e) {
            $success = FALSE;
            $exception_message = $e->getMessage();
          }

          // Report on progress.
          if ($success) {
            $this->reportMigrationStatus($migration, TRUE);
          }
          else {
            $this->reportMigrationStatus($migration, FALSE);
            $migration_name = $migration->version . '(' . $migration->description . ')';
            $this->logger->error("Error encountered. Unable to apply $migration_name ($migration_file) to $target_schema. Exception Message: $exception_message.");
          }
        }
      }
      // @todo Only mark the task successfull if no migrations failed.
      $task_success = TRUE;
    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());

      // Release all locks.
      $this->releaseTaskLocks();

      throw new TaskException(
        "Failed to apply migrations to this chado instance.\n"
        . $e->getMessage()
      );
    }

    return $task_success;
  }

  /**
   * {@inheritdoc}
   */
  public function getProgress() :float {
    $data = $this->state->get(static::STATE_KEY_DATA_PREFIX . $this->id, []);

    if (empty($data)) {
      // No more data available. Assume process ended.
      $progress = 1;
    }
    else {
      $progress = $data['progress'];
    }
    return $progress;
  }

  /**
   * {@inheritdoc}
   */
  public function getStatus() :string {
    $status = '';
    $progress = $this->getProgress();
    return $status;
  }

}
