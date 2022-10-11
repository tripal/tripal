<?php

namespace Drupal\tripal\TripalStorage;

/**
 * Base class for a Tripal storage property type or value.
 */
class StoragePropertyBase {

  /**
   * Constructs a new Tripal storage property base object.
   *
   * @param string entityType
   *   The entity type associated with this storage property base object.
   *
   * @param string fieldType
   *   The field type associated with this storage property base object.
   *
   * @param string key
   *   The key associated with this storage property base object.
   */
  public function __construct($entityType, $fieldType, $key) {
    $this->entityType = $entityType;
    $this->fieldType = $fieldType;

    // Drupal doesn't allow non alphanumeric characters in the key, so
    // remove any.
    $key = preg_replace('/[^\w]/', '_', $key);
    $this->key_ = $key;
  }

  /**
   * Returns the entity type associated with this storage property base
   * object.
   *
   * @return string
   *   The entity type.
   */
  public function getEntityType() {
    return $this->entityType;
  }

  /**
   * Returns the field type associated with this storage property base object.
   *
   * @return string
   *   The field type.
   */
  public function getFieldType() {
    return $this->fieldType;
  }

  /**
   * Returns the key associated with this storage property base object.
   *
   * @return string
   *   The key.
   */
  public function getKey() {
    return $this->key_;
  }

  /**
   * The entity type associated with this storage property base object.
   *
   * @var string
   */
  private $entityType;

  /**
   * The field type associated with this storage property base object.
   *
   * @var string
   */
  private $fieldType;

  /**
   * The field key associated with this storage property base object.
   *
   * @var string
   */
  private $key_;
}
