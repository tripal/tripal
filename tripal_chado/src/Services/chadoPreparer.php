<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;

class ChadoPreparer {

  /**
   * The name of the schema we are interested in installing/updating chado for.
   */
  protected $schemaName;

  /**
   * The DRUPAL-managed database connection.
   */
  protected $connection;

  /**
   * The drupal logger for tripal.
   */
  protected $logger;

  /**
   * Holds the Job object
   */
  protected $job = NULL;

  /**
   * Constructor: initialize connections.
   */
  public function __construct() {
    $this->connection = \Drupal::database();

    // Initialize the logger.
    $this->logger = \Drupal::service('tripal.logger');
  }

  /**
   * Set the schema name.
   */
  public function setSchema($schema_name) {
    // Schema name must be all lowercase with no special characters.
    // It should also be a single word.
    if (preg_match('/^[a-z][a-z0-9]+$/', $schema_name) === 0) {
      $this->logger->error('The schema name must be a single word containing only lower case letters or numbers and cannot begin with a number.');
      return FALSE;
    }
    else {
      $this->logger->info('Setting Schema to "' . $schema_name . '".');
      $this->schemaName = $schema_name;
      return TRUE;
    }
}

  /**
   * A setter for the job object if this class is being run using a Tripal job.
   */
  public function setJob(\Drupal\tripal\Services\TripalJob $job) {
    $this->job = $job;
    $this->logger->setJob($job);
    return TRUE;
  }

  /**
   * Retrieve the Drupal connection to the database.
   *
   * @return Drupal\database
   *   Current Drupal connection.
   */
  public function getDrupalConnection() {
    return $this->connection;
  }

  /**
   * Retrieves the message logger.
   *
   * @return object
   *
   */
  public function getLogger() {
    return $this->logger;
  }

  /**
   *
   */
  public function prepare() {

  }
}
