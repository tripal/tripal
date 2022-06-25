<?php

namespace Drupal\tripal4\TripalStorage;

use Drupal\tripal4\Base\StoragePropertyTypeBase;

/**
 * Defines the integer Tripal storage property type.
 */
class IntStoragePropertyType extends StoragePropertyTypeBase {

  /**
   * Constructs a new integer tripal storage property type.
   *
   * @param string entityId
   *   The entity id associated with this property type.
   *
   * @param string fieldId
   *   The field id associated with this property type.
   *
   * @param string fieldKey
   *   The field key associated with this property type.
   */
  public function __construct($entityId,$fieldId,$fieldKey) {
    parent::__construct($entityId,$fieldId,$fieldKey,"int");
  }

}
