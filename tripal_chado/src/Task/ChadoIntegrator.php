<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;

/**
 * Chado integrator.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of the Chado schema to integrate into Tripal.
 * $integrator = \Drupal::service('tripal_chado.integrator');
 * $integrator->setParameters([
 *   'input_schemas' => ['chado'],
 * ]);
 * if (!$integrator->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoIntegrator extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'integrator';

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include one input
   * schema and no output schema as shown:
   * ```
   * ['input_schemas' => ['schema_name'], ]
   * ```
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in case of invalid parameters.
   */
  public function validateParameters() :void {
    try {
      // Check input.
      if (empty($this->parameters['input_schemas'])
          || (1 != count($this->parameters['input_schemas']))
      ) {
        throw new ParameterException(
          "Invalid number of input schemas. Only one input schema must be specified."
        );
      }
      // Check output.
      if (!empty($this->parameters['output_schemas'])) {
        throw new ParameterException(
          "No output schema must be specified."
        );
      }
      $tripal_dbx = \Drupal::service('tripal.dbx');
      $input_schema = $this->inputSchemas[0];

      // Note: schema names have already been validated through BioConnection.
      // Check if the target schema exists.
      if (!$input_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'Input schema "'
          . $input_schema->getSchemaName()
          . '" does not exist.'
        );
      }

      // Check version.
      $version = $input_schema->findVersion();
      if ($version < 1.3) {
        throw new ParameterException(
          'Input schema "'
          . $input_schema->getSchemaName()
          . '" does not use a supported version of Chado schema.'
        );
      }
      // Keep version number.
      $this->parameters['version'] = $version;

      // Check the schema is not already integrated with Tripal.
      $install_select = $this->connection->select('chado_installations' ,'i')
        ->fields('i', ['install_id'])
        ->condition('schema_name', $input_schema->getSchemaName())
        ->execute();
      $results = $install_select->fetchAll();
      if ($results) {
        throw new ParameterException(
          'The schema "'
          . $input_schema->getSchemaName()
          . '" is already integrated into Tripal and does not need to be imported.'
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
   * Imports a given existing chado schema into Tripal system.
   *
   * By "integrating" or "importing" we mean that a Chado schema may have been
   * loaded into the database without Tripal and Tripal has not been configured
   * to use it. Therefor, such a schema needs to be "integrated" into Tripal
   * system in order to be used under Tripal/Drupal.
   *
   * Task parameter array provided to the class constructor includes:
   * - 'input_schemas' array: one input Chado schema that must exist and be at
   *   version >=1.3 (required)
   * - 'output_schemas' array: no output schema
   *
   * Example:
   * ```
   * ['input_schemas' => ['original_name'], ]
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
      $chado_schema = $this->inputSchemas[0];

      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, ['progress' => 0.5]);

      // Set the version and tell Tripal.
      $this->connection->insert('chado_installations')
        ->fields([
          'schema_name' => $chado_schema->getSchemaName(),
          'version' => $this->parameters['version'],
          'created' => \Drupal::time()->getRequestTime(),
          'updated' => \Drupal::time()->getRequestTime(),
        ])
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
        "Failed to complete schema integration task.\n"
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
      $status = 'Integration in progress.';
    }
    else {
      $status = 'Integration done.';
    }
    return $status;
  }

}
