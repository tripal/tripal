<?php

namespace Drupal\tripal_chado\TripalStorage;

use Drupal\tripal\TripalStorage\BoolStoragePropertyType;

/**
 * Defines the boolean Tripal storage property type.
 */
class ChadoBoolStoragePropertyType extends BoolStoragePropertyType {

  /**
   * Constructs a new boolean chado storage property type.
   *
   * @param string $entityType
   *   The entity type associated with this property type.
   * @param string $fieldType
   *   The field type associated with this property type.
   * @param string $key
   *   The key associated with this property type.
   * @param string $term_id
   *   The controlled vocabulary term asssociated with this property. It must be
   *   in the form of "IdSpace:Accession" (e.g. "rdfs:label" or "OBI:0100026")
   * @param array $storage_settings
   *   An array of settings required for this property by the storage backend.
   */
  public function __construct($entityType, $fieldType, $key, $term_id, $storage_settings = []) {
    parent::__construct($entityType, $fieldType, $key, $term_id, $storage_settings, 'chado_id_space');
  }

}
