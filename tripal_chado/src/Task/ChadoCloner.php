<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;

/**
 * Chado cloner.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of an existing Chado schema and 'new_chado_copy'
 * // is the name of an unexisting schema that will be created and receive the
 * // full copy of the 'chado' schema content.
 * $cloner = \Drupal::service('tripal_chado.cloner');
 * $cloner->setParameters([
 *   'input_schemas'  => ['chado'],
 *   'output_schemas' => ['new_chado_copy'],
 * ]);
 * if (!$cloner->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoCloner extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'cloner';

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include one input
   * schema and one output schema as shown:
   * ```
   * ['input_schemas' => ['original_name'], 'output_schemas' => ['copy_name'], ]
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
      if (empty($this->parameters['output_schemas'])
          || (1 != count($this->parameters['output_schemas']))
      ) {
        throw new ParameterException(
          "Invalid number of output schemas. Only one output schema must be specified."
        );
      }
      // Make sure both schemas are in the same database.
      $input_schema = $this->inputSchemas[0];
      $output_schema = $this->outputSchemas[0];
      if ($input_schema->getDatabaseName() != $output_schema->getDatabaseName()) {
        throw new ParameterException(
          "Schemas must reside in a same database. Could not clone schemas from one database to another with this implementation."
        );
      }
      $tripal_dbx = \Drupal::service('tripal.dbx');

      // Note: schema names have already been validated through BioConnection.
      // Check if the target schema is free.
      if ($output_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'Target schema "'
          . $output_schema->getSchemaName()
          . '" already exists. Please remove that schema first.'
        );
      }

      // Check target name is not reserved.
      $issue = $tripal_dbx->isInvalidSchemaName($output_schema->getSchemaName());
      if ($issue) {
        throw new ParameterException($issue);
      }

      // Check if the source schema exists.
      if (!$input_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'The source schema to clone "'
          . $input_schema->getSchemaName()
          . '" does not exist. Please select an existing schema to clone.'
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
   * Clone a given chado schema into the specified schema.
   *
   * The cloning procedure uses a custom PostgreSQL function
   * (tripal_clone_schema() installed with Tripal Chado) to clone schema rather
   * than using a schema dump, modifying it to change schema name (with possible
   * side effects) and reloading that dump. It is faster and avoids using
   * temporary files (risks of content disclosure).
   *
   * Task parameter array provided to the class constructor includes:
   * - 'input_schemas' array: one input schema that must exist (required)
   * - 'output_schemas' array: one output schema that must not exist (required)
   *
   * Example:
   * ```
   * ['input_schemas' => ['original_name'], 'output_schemas' => ['copy_name'], ]
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
      $source_schema = $this->inputSchemas[0];
      $target_schema = $this->outputSchemas[0];
      $tripal_dbx = \Drupal::service('tripal.dbx');

      // Get initial database size.
      $db_size = $tripal_dbx->getDatabaseSize($target_schema);

      // Get Chado size.
      $chado_size = $source_schema->schema()->getSchemaSize();

      // Save task initial data for later progress computation.
      $data = ['db_size' => $db_size, 'chado_size' => $chado_size];
      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);

      // Get a Chado connection object to our source schema to pass into
      // the cloneSchema method of the Tripal DBX API. We need to do this to
      // ensure the database used has the schema->initialize() method which
      // is needed to have the cloning functions available.
      // Note: $this->connection is a typical Drupal database connection without
      // the initialize functionality we need.
      $chado_connection = \Drupal::service('tripal_chado.database');
      $chado_connection->setSchemaName( $source_schema->getSchemaName() );
      // Clone schema.
      $tripal_dbx->cloneSchema(
        $source_schema->getSchemaName(),
        $target_schema->getSchemaName(),
        $chado_connection
      );
      $this->logger->info("Schema cloning completed\n");

      // Check target schema exists.
      if ($target_schema->schema()->schemaExists()) {
        $task_success = TRUE;
      }

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
        "Failed to complete schema cloning task.\n"
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
      // Compute progress.
      $tripal_dbx = \Drupal::service('tripal.dbx');
      $target_schema = $this->output_schemas[0];
      $db_size = $tripal_dbx->getDatabaseSize($target_schema);
      $progress = ($db_size - $data['db_size']) / $data['chado_size'];
      if (0.01 > $progress) {
        $progress = 0.01;
      }
      else if (1 <= $progress) {
        // Not done yet since we have data in state API.
        $progress = 0.99;
      }
    }
    return $progress;
  }

  /**
   * {@inheritdoc}
   */
  public function getStatus() :string {
    $status = '';
    $progress = $this->getProgress();
    if (0.01 >= $progress) {
      $status = 'Cloning not started yet.';
    }
    elseif (1 > $progress) {
      $status = 'Cloning in progress.';
    }
    else {
      $status = 'Cloning done.';
    }
    return $status;
  }

}
