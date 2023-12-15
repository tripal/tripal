<?php

namespace Drupal\tripal\Services;

use \Drupal\tripal\Entity\TripalEntity;
use \Drupal\tripal\Entity\TripalEntityType;
use \Drupal\tripal\TripalStorage\StoragePropertyValue;


class TripalEntityLookup {

  /**
   * The id of the entity type (bundle)
   *
   * @var string $bundle
   */
  protected $bundle = '';

  /**
   * The id of the TripalStorage plugin.
   *
   * @var string $datastore.
   */
  protected $datastore = '';

  /**
   * Constructor: initialize connections.
   */
  public function __construct() {

  }

  /**
   * Retrieve the entity corresponding to a record in a table.
   *
   * @param string $table
   *   The table name
   * @param int $pkey_id
   *   The primary key value in this table
   */
  protected function getEntity($table, $pkey_id) {
    $entity = NULL;
    return $entity;
  }

}
