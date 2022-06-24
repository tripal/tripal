<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\tripal4\Base\StoragePropertyTypeBase;

/**
 * Defines the real Tripal storage property type. A real type is any real
 * floating point number.
 */
class RealStoragePropertyType extends StoragePropertyTypeBase {

  /**
   * Constructs a new real tripal storage property type.
   *
   * @param string entityType
   *   The entity type associated with this property type.
   *
   * @param string fieldType
   *   The field type associated with this property type.
   *
   * @param string key
   *   The key associated with this property type.
   */
  public function __construct($entityType,$fieldType,$key) {
    parent::__construct($entityType,$fieldType,$key,"real");
  }

}
