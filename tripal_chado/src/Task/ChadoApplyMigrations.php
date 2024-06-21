<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;

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
   * Name of the task.
   */
  public const TASK_NAME = 'apply_migrations';

  /**
   * Default version.
   */
  public const BASELINE_CHADO_VERSION = '1.3';

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
      $target_schema = $this->inputSchemas[0];
      $tripal_dbx = \Drupal::service('tripal.dbx');
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
