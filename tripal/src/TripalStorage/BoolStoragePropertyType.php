<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

/**
 * Defines the boolean Tripal storage property type.
 */
class BoolStoragePropertyType extends StoragePropertyTypeBase {

  /**
   * Constructs a new boolean tripal storage property type.
   *
   * @param string entityType
   *   The entity type associated with this property type.
   *
   * @param string fieldType
   *   The field type associated with this property type.
   *
   * @param string key
   *   The key associated with this property type.
   *
   * @param array storage_settings
   *   An array of settings required for this property by the storage backend.
   */
  public function __construct($entityType, $fieldType, $key, $storage_settings = []) {
    parent::__construct($entityType, $fieldType, $key, "bool", $storage_settings);
  }

}
