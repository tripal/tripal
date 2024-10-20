<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\tripal\TripalStorage\StoragePropertyTypeBase;

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
   *
   * @param string term_id
   *   The controlled vocabulary term asssociated with this property. It must be
   *   in the form of "IdSpace:Accession" (e.g. "rdfs:label" or "OBI:0100026")
   *
   * @param array storage_settings
   *   An array of settings required for this property by the storage backend.
   */
  public function __construct($entityType, $fieldType, $key, $term_id, $storage_settings = []) {
    parent::__construct($entityType, $fieldType, $key, $term_id, "real", $storage_settings);
  }

  /**
   * Returns the default empty value of the correct type for this storage property type.
   *
   * @return float
   *   A zero.
   */
  public function getDefaultValue() {
    return 0.0;
  }

}
