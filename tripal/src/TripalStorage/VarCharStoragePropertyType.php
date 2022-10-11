<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

/**
 * Defines the variable character Tripal storage property type.
 */
class VarCharStoragePropertyType extends StoragePropertyTypeBase {

  /**
   * Constructs a new variable character tripal storage property type.
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
   * @param int size
   *   The maximum size of characters for this type.
   *
   * @param array storage_settings
   *   An array of settings required for this property by the storage backend.*
   */
  public function __construct($entityType, $fieldType, $key, $size = 255, $storage_settings = []) {
    parent::__construct($entityType, $fieldType, $key, "varchar", $storage_settings);
    $this->maxCharacterSize = $size;
  }

  /**
   * Returns the maximum character size of this storage property type.
   *
   * @return int
   *   The character size.
   */
  public function getMaxCharacterSize() {
    return $this->maxCharacterSize;
  }

  /**
   * The maximum character size of this storage property type.
   *
   * @var int
   */
  private $maxCharacterSize;

}
