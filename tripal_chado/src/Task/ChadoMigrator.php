<?php

namespace Drupal\tripal_chado\Task;

use Drupal\tripal_chado\Task\ChadoTaskBase;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal_biodb\Exception\ParameterException;

/**
 * Chado entity ID migrator.
 *
 * Usage:
 * @code
 * // Where 'file_name' is a data file generated from an
 * // existing tripal 3 site with export_tripal3_entity_mapping.php
 * $migrator = \Drupal::service('tripal_chado.migrator');
 * $migrator->setParameters([
 *   'filename' => 'tripal3_entity_mapping.tsv',
 * ]);
 * if (!$migrator->performTask()) {
 *   // Display a message telling the user the task failed and details are in
 *   // the site logs.
 * }
 * @endcode
 */
class Chadomigrator extends ChadoTaskBase {

  /**
   * Name of the task.
   */
  public const TASK_NAME = 'migrator';

  /**
   * Stores the Tripal 3 data
   */
  protected array $tripal3ids = [];

  /**
   * Stores the Tripal 3 maximum entity ID
   */
  protected int $tripal3maxid = 0;

  /**
   * {@inheritdoc}
   */
  public function __construct(
    ?\Drupal\Core\Database\Connection $database = NULL,
    ?\Psr\Log\LoggerInterface $logger = NULL,
    ?\Drupal\tripal_biodb\Lock\SharedLockBackendInterface $locker = NULL,
    ?\Drupal\Core\State\StateInterface $state = NULL
  ) {
    parent::__construct($database, $logger, $locker, $state);
//@@@    $this->tripalDbxApi = \Drupal::service('tripal.dbx');
  }

  /**
   * Validate task parameters.
   *
   * Parameter array provided to the class constructor must include the
   * filename parameter
   * ```
   * [
   *   'filename' => 'tripal3_entity_mapping.tsv',
   * ]
   * ```
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in case of invalid parameters.
   */
  public function validateParameters() :void {
    try {
      if (empty($this->parameters['filename'])) {
        throw new ParameterException(
          "The filename parameter is required."
          );
      }
      if (!file_exists($this->parameters['filename'])) {
        throw new ParameterException(
          "The file '" . $this->parameters['filename'] . "' does not exist."
          );
      }
      $fh = fopen($this->parameters['filename'], 'r');
      if (!$fh) {
        throw new ParameterException("Failed to open '" . $this->parameters['filename'] . "' for reading.");
      }
      $this->parameters['fh'] = $fh;
    }
    catch (\Exception $e) {
      // Log.
      $this->logger->error($e->getMessage());
      // Rethrow.
      throw $e;
    }
  }

  /**
   * Migrate entity IDs.
   *
   * The migration process depends on having reserved a block of existing
   * entity IDs when the existing Tripal 3 site was migrated and published.
   * The procedure for this is described at
   * https://tripaldoc.readthedocs.io/en/latest/upgrade_guide/site/migrating_chado.html
   *
   * *Parameters*
   *
   * Task parameter array provided to the class constructor includes:
   * - 'filename' string: a data file generated from the existing Tripal 3
   *   site using export_tripal3_entity_mapping.php
   *   The file path can be absolute (starting with a '/') or relative to the
   *   site 'files' directory (or private directory if it is the default).
   *
   * Example:
   * ```
   * [
   *   'filename' => 'tripal3_entity_mapping.tsv',
   * ]
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
    // After validation, $this->parameters['fh'] is an open handle to the data file.

    // Acquire locks.
    $success = $this->acquireTaskLocks();
    if (!$success) {
      throw new LockException("Unable to acquire all locks for task. See logs for details.");
    }

    try {

      // Store data to $this->tripal3ids
      $this->loadMigrationData();
      $this->setProgress(0.02);

      // @todo next, get minimum tripal 4 entity ID and validate range is available





      $this->setProgress(1);
      $task_success = TRUE;

      // Release all locks.
      $this->releaseTaskLocks();

    }
    catch (\Exception $e) {
      $this->logger->error($e->getMessage());
      // Release all locks.
      $this->releaseTaskLocks();

      throw new TaskException(
        "Failed to complete Chado entity migration task.\n"
        . $e->getMessage()
      );
    }

    return $task_success;
  }

  /**
   * Reads the Tripal 3 data file and stores it at $this->tripal3ids
   * and also stores the maximum entity ID value at $this->tripal3maxid
   */
  private function loadMigrationData() {
    $nlines = 0;
    while (($line = fgets($this->parameters['fh'])) !== false) {
      $nlines++;
      $cols = explode("\t", rtrim($line));
      if (count($cols) != 4) {
        throw new TaskException(
          "Invalid file format line $nlines, expected exactly 4 columns \"$line\"\n"
        );
      }
      $this->tripal3ids[$cols[0]][$cols[1]][$cols[2]] = $cols[3];
      if ($cols[3] > $this->tripal3maxid) {
        $this->tripal3maxid = $cols[3];
      }
    }
    fclose($this->parameters['fh']);
    $this->logger->notice(
      t(
        "Ready to migrate @n entity IDs, maximum Tripal 3 ID was @max",
        [
         '@n' => number_format($nlines),
         '@max' => number_format($this->tripal3maxid),
        ]
      )
    );
  }

  /**
   * Set progress value.
   *
   * @param float $value
   *   New progress value.
   */
  protected function setProgress(float $value) {
    $data = ['progress' => $value];
    $this->state->set(static::STATE_KEY_DATA_PREFIX . $this->id, $data);
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
    if (0 >= $progress) {
      $status = 'Entity migration not started yet.';
    }
    elseif (1 > $progress) {
      $status = 'Entity migration in progress';
    }
    else {
      $status = 'Entity migration done.';
    }
    return $status;
  }

}
