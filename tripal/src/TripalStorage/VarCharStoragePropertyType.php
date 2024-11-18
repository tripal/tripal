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
   * @param string term_id
   *   The controlled vocabulary term asssociated with this property. It must be
   *   in the form of "IdSpace:Accession" (e.g. "rdfs:label" or "OBI:0100026")
   *
   * @param int size
   *   The maximum size of characters for this type.
   *
   * @param array storage_settings
   *   An array of settings required for this property by the storage backend.*
   */
  public function __construct($entityType, $fieldType, $key, $term_id, int $size = 255, $storage_settings = []) {
    parent::__construct($entityType, $fieldType, $key, $term_id, "varchar", $storage_settings);
    $this->maxCharacterSize = $size;
  }

  /**
   * Returns the default empty value of the correct type for this storage property type.
   *
   * @return string
   *   An empty string.
   */
  public function getDefaultValue() {
    return '';
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
