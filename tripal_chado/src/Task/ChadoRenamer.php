<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;

/**
 * Chado renamer.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of the Chado schema to rename and
 * // 'new_chado_name' is the new name to use.
 * $renamer = \Drupal::service('tripal_chado.renamer');
 * $renamer->setParameters([
 *   'output_schemas' => ['chado', 'new_chado_name'],
 * ]);
 * if (!$renamer->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoRenamer extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'renamer';

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include two output
   * schema as shown:
   * ```
   * ['output_schemas' => ['old_schema_name', 'new_schema_name'], ]
   * ```
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in case of invalid parameters.
   */
  public function validateParameters() :void {
    try {
      // Check input.
      if (!empty($this->parameters['input_schemas'])) {
        throw new ParameterException(
          "No input schema should be specified. The schema to rename should be the first output schema and the new schema name should be specified as the second output schema."
        );
      }
      // Check output.
      if (empty($this->parameters['output_schemas'])
          || (2 != count($this->parameters['output_schemas']))
      ) {
        throw new ParameterException(
          "Invalid number of output schemas. Two output schema must be specified. The schema to rename should be the first output schema and the new schema name should be specified as the second output schema."
        );
      }
      $tripal_dbx = \Drupal::service('tripal.dbx');
      $old_schema = $this->outputSchemas[0];
      $new_schema = $this->outputSchemas[1];

      // Note: schema names have already been validated through BioConnection.
      // Check if the schema to rename exists.
      if (!$old_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'Schema to rename "'
          . $old_schema->getSchemaName()
          . '" does not exist.'
        );
      }
      // Check the new name is not already in use.
      if ($new_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'New schema name "'
          . $new_schema->getSchemaName()
          . '" is already in use.'
        );
      }
      // Check the new name is not reserved.
      $issue = $tripal_dbx->isInvalidSchemaName($new_schema->getSchemaName());
      if ($issue) {
        throw new ParameterException($issue);
      }
    }
    catch (\Exception $e) {
      // Log.
      $this->logger->error($e->getMessage());
      // Rethrow.
      throw $e;
    }
  }

  /**
   * Renames a given Chado schema.
   *
   * Task parameter array provided to the class constructor includes:
   * - 'output_schemas' array: 2 output schemas. The first one is the old schema
   *   name and the second one is the new schema name.
   *
   * Example:
   * ```
   * ['output_schemas' => ['old_schema_name', 'new_schema_name'], ]
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
      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, ['progress' => 0.1]);
      $old_schema = $this->outputSchemas[0];
      $new_schema = $this->outputSchemas[1];
      $old_schema_name = $old_schema->getSchemaName();
      $old_schema->schema()->renameSchema($new_schema->getSchemaName());

      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, ['progress' => 0.5]);
      // Update Tripal.
      $this->connection->update('chado_installations')
        ->fields([
          'schema_name' => $new_schema->getSchemaName(),
          'updated' => \Drupal::time()->getRequestTime(),
        ])
        ->condition('schema_name', $old_schema_name)
        ->execute()
      ;
      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, ['progress' => 1]);
      $task_success = TRUE;

      // Release all locks.
      $this->releaseTaskLocks();

      // Cleanup state API.
      $this->state->delete(static::STATE_KEY_DATA_PREFIX . $this->id);
    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());
      // Cleanup state API.
      $this->state->delete(static::STATE_KEY_DATA_PREFIX . $this->id);
      // Release all locks.
      $this->releaseTaskLocks();

      throw new TaskException(
        "Failed to rename schema.\n"
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
    if (1 > $progress) {
      $status = 'Renaming in progress.';
    }
    else {
      $status = 'Schema renamed.';
    }
    return $status;
  }

}
