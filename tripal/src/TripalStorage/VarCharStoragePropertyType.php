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
   * @param string entityId
   *   The entity id associated with this property type.
   *
   * @param string fieldId
   *   The field id associated with this property type.
   *
   * @param string fieldKey
   *   The field key associated with this property type.
   *
   * @param int size
   *   The maximum size of characters for this type.
   */
  public function __construct($entityId,$fieldId,$fieldKey,$size = 255) {
    parent::__construct($entityId,$fieldId,$fieldKey,"varchar");
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
