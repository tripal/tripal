<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;

/**
 * Chado installer.
 *
 * Usage:
 * @code
 * // Where 'chado' is the name of the Chado schema to instantiate.
 * $installer = \Drupal::service('tripal_chado.installer');
 * $installer->setParameters([
 *   'output_schemas' => ['chado'],
 *   'version' => '1.3',
 * ]);
 * if (!$installer->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class ChadoInstaller extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'installer';

  /**
   * Default version.
   */
  public const DEFAULT_CHADO_VERSION = '1.3';

  /**
   * The number of chunk files per version we can install.
   *
   * @todo: We should use one single SQL file.
   */
  protected $installNumChunks = [
    '1.3' => 41,
  ];

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include one output
   * schema and it may include a version number:
   * ```
   * ['output_schemas' => ['chado'], 'version' => '1.3']
   * ```
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in cas of invalid parameters.
   */
  public function validateParameters() :void {
    try {
      // Select a default version if needed.
      if (empty($this->parameters['version'])) {
        $this->parameters['version'] = static::DEFAULT_CHADO_VERSION;
      }
      // Check the version passed in is not an array or object.
      if (is_array($this->parameters['version'])
        || is_object($this->parameters['version'])) {

        throw new ParameterException(
          "The requested version must be a string; whereas, you passed an"
          . " array or object: " . print_r($this->parameters['version'], TRUE)
        );
      }
      // If the version is not a string then make it so...
      if (!is_string($this->parameters['version'])) {
        $this->parameters['version'] = strval($this->parameters['version']);
      }
      // Check the version is valid.
      if (!array_key_exists(
          $this->parameters['version'],
          $this->installNumChunks
        )
      ) {
        throw new ParameterException(
          "That requested version ("
          . $this->parameters['version']
          . ") is not supported by this installer."
        );
      }

      // Check input.
      if (!empty($this->parameters['input_schemas'])) {
        throw new ParameterException(
          "Chado installer does not take input schemas. Only one output schema must be specified."
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
      $tripal_dbx = \Drupal::service('tripal.dbx');
      $output_schema = $this->outputSchemas[0];

      // Note: schema names have already been validated through BioConnection.
      // Check if the target schema is free.
      if ($output_schema->schema()->schemaExists()) {
        throw new ParameterException(
          'Target schema "'
          . $output_schema->getSchemaName()
          . '" already exists. Please remove that schema first.'
        );
      }
      // Check name is not reserved.
      $issue = $tripal_dbx->isInvalidSchemaName($output_schema->getSchemaName());
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
   * Installs a given chado schema version into the specified schema.
   *
   * The install procedure uses a set of SQL files.
   *
   * Task parameter array provided to the class constructor includes:
   * - 'output_schemas' array: one output schema that must not exist (required)
   * - 'input_schemas' array: no input schema
   * - 'version' string: a version number (optional, default to
   *   ::DEFAULT_CHADO_VERSION)
   *
   * Example:
   * ```
   * ['output_schemas' => ['chado'], 'version' => '1.3']
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
      $target_schema = $this->outputSchemas[0];
      $tripal_dbx = \Drupal::service('tripal.dbx');

      // Save task initial data for later progress computation.
      // @todo: We should use one single SQL file.
      $data = ['progress' => 0];
      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);

      // Install schema.
      $version = $this->parameters['version'];
      $num_chunks = $this->installNumChunks[$version];
      // 1) Drop the common schemas if they already exist.
      // We do not need to drop Chado schema since it is a parameter requirement
      // that the schema must not exist already.
      // @todo: The Chado SQL file should use 'IF NOT EXISTS' everywhere
      // possible to avoid touching those schemas.
      if ($tripal_dbx->schemaExists('genetic_code')) {
        $tripal_dbx->dropSchema('genetic_code');
      }
      if ($tripal_dbx->schemaExists('so')) {
        $tripal_dbx->dropSchema('so');
      }
      if ($tripal_dbx->schemaExists('frange')) {
        $tripal_dbx->dropSchema('frange');
      }

      // 2) Create the schema.
      $target_schema->schema()->createSchema();

      // 3) Apply SQL files containing table definitions.
      $module_path = \Drupal::service('extension.list.module')
        ->getPath('tripal_chado');
      $path = $module_path . '/chado_schema/parts-v' . $version . '/';
      for ($i = 1; $i <= $num_chunks; $i++) {
        $file = $path . 'default_schema-' . $version . '.part' . $i . '.sql';
        $success = $target_schema->executeSqlFile(
          $file,
          ['chado' => $target_schema->getSchemaName(),]
        );

        if (!$success) {
          throw new TaskException(
            "Schema installation part $i of $num_chunks Failed...\nInstallation (Step 1 of 2) problems!"
          );
        }
        $this->logger->info("Import part $i of $num_chunks Successful!");
        $data = ['progress' => ($i/$num_chunks)*0.80];
        $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);
      }
      $this->logger->info("Install of Chado v1.3 (Step 1 of 3) successful.");

      // 4) Initialize the schema with basic data.
      $init_file =
        $module_path
        . '/chado_schema/initialize-'
        . $version
        . '.sql'
      ;
      $success = $target_schema->executeSqlFile(
        $init_file,
        ['chado' => $target_schema->getSchemaName(),]
      );
      if (!$success) {
        throw new TaskException("Installation (Step 2 of 3) problems!");
      }
      $this->logger->info("Install of Chado v1.3 (Step 2 of 3) successful.");
      $data = ['progress' => 0.90];
      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);

      // 5) Finally set the version and tell Tripal.
      $vsql = "
        INSERT INTO {1:chadoprop} (type_id, value)
          VALUES (
           (SELECT cvt.cvterm_id
            FROM {1:cvterm} cvt
              INNER JOIN {1:cv} cv ON cvt.cv_id = cv.cv_id
             WHERE cv.name = 'chado_properties' AND cvt.name = 'version'),
           :version)
      ";
      $target_schema->query($vsql, [':version' => $version]);
      $data = ['progress' => 0.95];
      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);
      $this->connection
        ->insert('chado_installations')
        ->fields([
          'schema_name' => $target_schema->getSchemaName(),
          'version' => $version,
          'created' => \Drupal::time()->getRequestTime(),
          'updated' => \Drupal::time()->getRequestTime(),
        ])
        ->execute()
      ;
      $data = ['progress' => 1.];
      $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);

      // If this is the first installed schema then we want to set it as the default.
      // The default schema is stored in the settings for tripal_chado.
      $config = \Drupal::service('config.factory')
        ->getEditable('tripal_chado.settings');
      $default_schema = $config->get('default_schema');
      if (empty($default_schema)) {
        $config->set('default_schema', $target_schema->getSchemaName())->save();
      }

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
        "Failed to complete Chado installation task.\n"
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
    if (0.8 >= $progress) {
      $status = 'Installation (Step 1 of 3)';
    }
    elseif (0.9 >= $progress) {
      $status = 'Installation (Step 2 of 3)';
    }
    elseif (1 > $progress) {
      $status = 'Installation (Step 3 of 3)';
    }
    else {
      $status = 'Installation done.';
    }
    return $status;
  }

}
