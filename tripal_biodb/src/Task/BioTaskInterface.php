<?php

namespace Drupal\tripal_biodb\Task;

/**
 * Provides an interface to manage a task on one or more biological schemas.
 */
interface BioTaskInterface {

  /**
   * Returns the task identifier.
   *
   * This identifier is unique for a given task on a given set of schemas. It
   * can be used to identify and retrieve an task to get its status while
   * running from another independant process. It must only contain lower case
   * alpha-numeric characters, underscores, dashes and dots. The task
   * identifier should follow the following scheme:
   * <task_name>-<dash_separated_schema_list>
   * When a task works with more than one schema, the schema names should be
   * ordered. The order could be alpha-numeric if it is not important or schema
   * names could be ordered by argument order if this order is important.
   * Schema name should be prefixed by the associated database name followed by
   * a dot when it is not the default one.
   *
   * For example, a schema cloning task that clones the "chado" schema to a
   * new schema named "alpha_chado" would use the task identifier:
   * "clone-chado-alpha_chado".
   *
   * Another example would be a Chado data merging task that would merge the
   * data of 2 Chado instances (chado1 and chado2)into a new schema (big_chado).
   * The order of the 2 merged schema names is not relevant so they will be
   * sorted alpha-numericaly. The target schema would come first as it will be
   * unique while there could be more than 2 schemas to merge. So its task
   * identifier would look like:
   * "merge-big_chado-chado1-chado2"
   *
   * @return string
   *   The complete task identifier string.
   */
  public function getId() :string;

  /**
   * Sets task parameters.
   *
   * It sets (or sets again) task parameter before task execution. Only schema
   * names are checked here, no other parameter validation are performed.
   * Parameters are validate at runtime by ::performTask, just before the actual
   * task is started.
   *
   * @param $parameters
   *   A associative array of parameters used to configure the task. The
   *   array should include the keys 'input_schemas' and 'output_schemas', both
   *   containing an array of ordered schema names (or an empty array).
   *   'input_schemas' are biological schemas used for reading only which may be
   *   shared for reading with other concurrent tasks. 'output_schemas' are
   *   schemas that will be created or modified and must not be shared (
   *   exclusive use) during the task. If a schema comes from a different
   *   database than the default one (ie. the one used by Drupal), the schema
   *   name must be prefixed by the database key name (and not the "target", as
   *   describbed in \Drupal\Core\Database\Database::getConnection()) followed
   *   by a dot.
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in case of an invalid schema name.
   */
  public function setParameters(array $parameters = []) :void;

  /**
   * Validate task parameters.
   *
   * This method should be called just before executing a task in the
   * ::performTask method and should not need to be called from outside the
   * class.
   *
   * @throws \Drupal\tripal_biodb\Exception\ParameterException
   *   A descriptive exception is thrown in case of invalid parameters.
   */
  public function validateParameters() :void;

  /**
   * Performs the required task.
   *
   * @return bool
   *   TRUE if the task was performed with success and FALSE otherwise. In
   *   some cases, exceptions can also be thrown in order to report failures.
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
  public function performTask() :bool;

  /**
   * Returns the percent of progress of current task.
   *
   * This method can also be used to check if a task is currently running if
   * it has a > 0 value. A negative value means an error occured while running.
   *
   * @return float
   *   A value between -1 and 1, 0 meaning the task has not been started yet,
   *   1 meaning the task is completed with success and a negative value meaning
   *   that the task failed. The negative value may be used as a code to
   *   identify the error.
   */
  public function getProgress() :float;

  /**
   * Returns a string describing current status of the performed task.
   *
   * This function returns the last known status, even if the task ended.
   * In case of failure, this function may return the reason of the failure.
   *
   * @return string
   *   A localized description.
   */
  public function getStatus() :string;

  /**
   * Returns the logger used by this task.
   *
   * @return \Psr\Log\LoggerInterface
   *   The task logger.
   */
  public function getLogger() :\Psr\Log\LoggerInterface;

}
