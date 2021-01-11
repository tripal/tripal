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
   *
   */
  public function __construct() {

  }

  /**
   *
   */
  public function setSchema($schema_name) {}

  /**
   *
   */
  public function setJob(\Drupal\tripal\Services\TripalJob $job) {}

  /**
   *
   */
  public function prepare() {}
}
