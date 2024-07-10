<?php

namespace Drupal\tripal_biodb\Task;

use Drupal\tripal_biodb\Lock\SharedLockBackendInterface;
use Drupal\tripal_biodb\Exception\TaskException;
use Drupal\tripal_biodb\Exception\ParameterException;
use Drupal\tripal_biodb\Exception\LockException;
use Drupal\tripal\TripalDBX\TripalDbxConnection;

/**
 * Defines the base class for tasks on one or more biological schemas.
 */
abstract class BioTaskBase implements BioTaskInterface {

  /**
   * Name of the task.
   *
   * Should be overridden by implementing classes.
   */
  public const TASK_NAME = 'task';

  /**
   * Prefix for state keys to store data in Drupal State API.
   */
  public const STATE_KEY_DATA_PREFIX = 'tripal_biodb_';

  /**
   * Task identifier.
   *
   * @var string
   */
  protected $id;

  /**
   * The main database used.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $connection;

  /**
   * The logger.
   *
   * @var \Psr\Log\LoggerInterface
   */
  protected $logger;

  /**
   * The schema and task locker.
   *
   * @var \Drupal\tripal_biodb\Lock\SharedLockBackendInterface
   */
  protected $locker;

  /**
   * The state storage service.
   *
   * @var \Drupal\Core\State\StateInterface
   */
  protected $state;

  /**
   * Task parameters.
   *
   * @var array
   */
  protected $parameters = ['input_schemas' => [], 'output_schemas' => [], ];

  /**
   * Input schemas as an array of \Drupal\tripal\TripalDBX\TripalDbxConnection.
   *
   * @var array
   */
  protected $inputSchemas = [];

  /**
   * Output schemas as an array of \Drupal\tripal\TripalDBX\TripalDbxConnection.
   *
   * @var array
   */
  protected $outputSchemas = [];

  /**
   * Creates a BioTaskBase object.
   *
   * @param ?\Drupal\Core\Database\Connection $connection
   *   The main database connection.
   * @param ?\Psr\Log\LoggerInterface $logger
   *   The logger.
   * @param ?\Drupal\tripal_biodb\Lock\SharedLockBackendInterface $locker
   *   The lock backend used to lock task and used schemas.
   * @param \Drupal\Core\State\StateInterface $state
   *   Drupal state service.
   *
   * @see https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Database.php/function/Database%3A%3AgetConnection/9.3.x
   * @see https://www.drupal.org/docs/8/api/database-api/database-configuration
   */
  public function __construct(
    ?\Drupal\Core\Database\Connection $database = NULL,
    ?\Psr\Log\LoggerInterface $logger = NULL,
    ?\Drupal\tripal_biodb\Lock\SharedLockBackendInterface $locker = NULL,
    ?\Drupal\Core\State\StateInterface $state = NULL
  ) {
    // Database.
    if (!isset($database)) {
      $database = \Drupal::database();
    }
    $this->connection = $database;
    // Logger.
    if (!isset($logger)) {
      $logger = \Drupal::service('tripal_biodb.logger');
    }
    $this->logger = $logger;
    // Locker.
    if (!isset($locker)) {
      $locker = \Drupal::service('tripal_biodb.lock');
    }
    $this->locker = $locker;
    // State.
    if (!isset($state)) {
      $state = \Drupal::state();
    }
    $this->state = $state;

    // Initializes task identifer.
    $this->initId();
  }

  /**
   * {@inheritdoc}
   */
  public function setParameters(array $parameters = []) :void {
    // Task parameters.
    $this->parameters =
      $parameters
      + ['input_schemas' => [], 'output_schemas' => [], ]
    ;

    // Initializes schema data.
    $this->inputSchemas = $this->prepareSchemas(
      $this->parameters['input_schemas']
    );
    $this->outputSchemas = $this->prepareSchemas(
      $this->parameters['output_schemas']
    );

    // Initializes task identifer.
    $this->initId();
  }

  /**
   * Gets the task-specific class for the specified category.
   *
   * Returns the task-specific override class if any for the specified class
   * category.
   *
   * @param string $class
   *   The class category for which we want the specific class.
   *
   * @return string
   *   The name of the class that should be used.
   */
  public function getTripalDbxClass($class) {
    static $classes = [
      'Connection' => TripalDbxConnection::class,
    ];
    if (!array_key_exists($class, $classes)) {
      throw new ConnectionException("Invalid Tripal DBX class '$class'.");
    }
    return $classes[$class];
  }

  /**
   * Parses schema names and extract the database name if one.
   *
   * @param array $schema_list
   *   An ordered array of biological schema names that may be prefixed by a
   *   Drupal database key followed by a dot (see
   *   \Drupal\Core\Database\Database::getConnection()).
   *
   * @return array
   *   An ordered array of \Drupal\tripal\TripalDBX\TripalDbxConnection objects.
   *
   * @see https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Database.php/function/Database%3A%3AgetConnection/9.3.x
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   */
  protected function prepareSchemas(array $schema_list) :array {
    $schemas = [];
    foreach ($schema_list as $schema) {
      // We need to take into account schemas prefixed with a database key.
      // The following regex separate the key from the schema name and also do a
      // pre-check on schema names that will be fully validated when the
      // connection object will be instantiated.
      if (!preg_match(
            '/^((?:.+\.)?)([a-z_\\xA0-\\xFF][a-z_\\xA0-\\xFF0-9]*)$/',
            $schema,
            $match
          )
      ) {
        throw new ParameterException("Invalid schema specification: '$schema'.");
      }
      $schema_name = $match[2];
      $db_key = $match[1];
      // Check if a specific database has been specified.
      $class = $this->getTripalDbxClass('Connection');

      if (!empty($db_key)) {
        // Yes, remove trailing dot from key.
        $db_key = substr($db_key, 0, -1);
        $schema_db = new $class($schema_name, $db_key);
      }
      else {
        // No, use default database.
        $schema_db = new $class($schema_name);
      }
      $schemas[] = $schema_db;
    }
    return $schemas;
  }

  /**
   * Returns the lock name to use for the given schema.
   *
   * @param \Drupal\tripal\TripalDBX\TripalDbxConnection $db
   *   A schema connection.
   *
   * @return string
   *   The lock name.
   */
  protected function getSchemaLockName(
    \Drupal\tripal\TripalDBX\TripalDbxConnection $db
  ) :string {
    return $db->getDatabaseName() . '.' . $db->getSchemaName();
  }

  /**
   * Initializes task identifier.
   */
  protected function initId() :void {
    $raw_id =
      static::TASK_NAME
      . '-'
      . $this->connection->getConnectionOptions()['database']
    ;
    if (!empty($this->inputSchemas)) {
      $raw_id .= '-' . count($this->inputSchemas) . 'i';
    }
    foreach ($this->inputSchemas as $schema) {
      if (!empty($schema->getDatabaseKey())
          && ('default' != $schema->getDatabaseKey())
      ) {
        $raw_id .=
          '-'
          . $schema->getDatabaseName()
          . '.'
          . $schema->getSchemaName()
        ;
      }
      else {
        $raw_id .= '-' . $schema->getSchemaName();
      }
    }
    if (!empty($this->outputSchemas)) {
      $raw_id .= '-' . count($this->outputSchemas) . 'o';
    }
    foreach ($this->outputSchemas as $schema) {
      if (!empty($schema->getDatabaseKey())
          && ('default' != $schema->getDatabaseKey())
      ) {
        $raw_id .=
          '-'
          . $schema->getDatabaseName()
          . '.'
          . $schema->getSchemaName()
        ;
      }
      else {
        $raw_id .= '-' . $schema->getSchemaName();
      }
    }
    // Note: we may consider using md5() if people report issues on lock
    // conflicts when using schema or db names that contains special characters.
    $this->id = preg_replace(
      '/[^\w\.\-]+/',
      '_',
      $raw_id
    );
  }

  /**
   * Lock what is needed before performing the task.
   *
   * This method should be called by extending classes before starting their
   * job on schemas (ie. in `performTask()`) as it will make sure a same
   * task is not already running and it will also lock the schemas as needed.
   *
   * @return bool
   *   TRUE if all the needed locks have been acquired, FALSE otherwise.
   */
  protected function acquireTaskLocks() :bool {
    $all_locked = FALSE;
    try {
      // Lock task.
      $success = $this->locker->acquire($this->id);
      if (!$success) {
        throw new TaskException(
          "Unable to lock task '"
          . $this->id
          . "'. Another process may be already running this task."
        );
      }

      // Lock output schemas (exclusive).
      foreach ($this->outputSchemas as $schema) {
        $success = $this->locker->acquire($this->getSchemaLockName($schema));
        if (!$success) {
          throw new TaskException(
            "Unable to lock (exclusive) output schema '"
            . $schema->getSchemaName()
            . "'."
          );
        }
      }

      // Lock input schemas (shared).
      foreach ($this->inputSchemas as $schema) {
        $lock_name = $this->getSchemaLockName($schema);
        $success = $this->locker->acquireShared($lock_name);
        if (!$success) {
          throw new TaskException(
            "Unable to lock (shared) input schema '"
            . $schema->getSchemaName()
            . "' with the lock '$lock_name'."
          );
        }
      }

      $all_locked = TRUE;
    }
    catch (\Exception $e) {
      $this->logger->error(
        'Unable to acquire all locks for task "'
        . $this->id
        . '". '
        . $e->getMessage()
      );
      // Release what was locked.
      $this->locker->releaseAll();
    }
    return $all_locked;
  }

  /**
   * Example implementation of performTask().
   *
   * This implementation should be replaced by extending classes (do not call
   * parent::performTask method as it throws an error). It is provided as
   * an example skeleton and for testing: first, it check parameters by calling
   * ::validateParameters, it acquires required locks by calling
   * ::acquireTaskLocks, manages lock failures, then it performs the task and
   * finally releases the locks by calling ::releaseTaskLocks.
   *
   * @return bool
   *   TRUE if the task was performed with success and FALSE otherwise. In
   *   some cases, exceptions can also be thrown in order to report major
   *   failures. FALSE would be returned if the task was completed but without
   *   the expected success.
   *
   * @throws \Drupal\tripal_biodb\Exception\TaskException
   *   Thrown when a major failure prevents the task from being performed.
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   Thrown if parameters are incorrect.
   *
   * @throws \Drupal\tripal_biodb\Exception\LockException
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
      throw new LockException(
        "Unable to acquire all locks for task. See logs for details."
      );
    }

    // Perform the actual task.
    // The following exception must be removed in implementations.
    throw new TaskException("Not implemented.");
    // Do long stuff here.
    // ...
    $task_success = TRUE;

    // Release all locks.
    $this->releaseTaskLocks();
    return $task_success;
  }

  /**
   * Release the locks used by the task.
   *
   * This method should be called by extending classes once their job on schemas
   * (ie. in `performTask()`) is over.
   */
  protected function releaseTaskLocks() :void {
    $this->locker->releaseAll();
  }

  /**
   * {@inheritdoc}
   */
  public function getId() :string {
    return $this->id;
  }

  /**
   * {@inheritdoc}
   */
  public function getStatus() :string {
    $progress = $this->getProgress();
    if (0 == $progress) {
      $status = t('Not started yet.');
    }
    elseif (1 <= $progress) {
      $status = t('Done.');
    }
    elseif (0 > $progress) {
      $status = t('An error occurred.');
    }
    else {
      $status = t('In progress');
    }
    return $status;
  }

  /**
   * {@inheritdoc}
   */
  public function getLogger() :\Psr\Log\LoggerInterface {
    return $this->logger;
  }

}
