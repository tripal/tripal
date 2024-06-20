<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;
use Drupal\tripal_chado\Services\ChadoCustomTable;
use Drupal\tripal_chado\Services\ChadoMviews;
use Drupal\tripal_chado\Services\ChadoMview;

/**
 * Chado remover.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of the Chado schema to remove.
 * $remover = \Drupal::service('tripal_chado.remover');
 * $remover->setParameters([
 *   'output_schemas' => ['chado'],
 * ]);
 * if (!$remover->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoRemover extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'remover';

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include two output
   * schema as shown:
   * ```
   * ['output_schemas' => ['chado'], ]
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
          "No input schema should be specified."
        );
      }
      // Check output.
      if (empty($this->parameters['output_schemas'])
          || (1 != count($this->parameters['output_schemas']))
      ) {
        throw new ParameterException(
          "Invalid number of output schemas. Only one output schema to remove should be specified."
        );
      }
      $tripal_dbx = \Drupal::service('tripal.dbx');
      $old_schema = $this->outputSchemas[0];

      // Note: schema names have already been validated through BioConnection.
      // Check if the schema to remove exists.
      if (!$old_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'Schema to remove "'
          . $old_schema->getSchemaName()
          . '" does not exist.'
        );
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
   * Removes a given Chado schema.
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

      // Remove any materialized views in this schema.
      $mviews = \Drupal::service('tripal_chado.materialized_views');
      $all_mviews = $mviews->getTables($old_schema->getSchemaName());
      foreach ($all_mviews as $table_name) {
        $mviews = \Drupal::service('tripal_chado.materialized_views');
        $mview = $mviews->create($table_name, $old_schema->getSchemaName());
        $mview->delete();
      }

      // Remove any custom tables in this schema.
      $custom_tables = \Drupal::service('tripal_chado.custom_tables');
      $all_custom_tables = $custom_tables->getTables($old_schema->getSchemaName());
      foreach ($all_custom_tables as $table_id => $table_name) {
        $custom_table = $custom_tables->loadById($table_id);
        $custom_table->delete();
      }

      // Drop the schema.
      $old_schema->schema()->dropSchema();

      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, ['progress' => 0.5]);
      // Update Tripal.
      $this->connection->delete('chado_installations')
        ->condition('schema_name', $old_schema->getSchemaName())
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
        "Failed to remove schema.\n"
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
      $status = 'Removing schema...';
    }
    else {
      $status = 'Schema removed.';
    }
    return $status;
  }

}
