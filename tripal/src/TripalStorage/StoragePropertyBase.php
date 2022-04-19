<?php

namespace Drupal\tripal\TripalStorage;

/**
 * Base class for a Tripal storage property type or value.
 */
class StoragePropertyBase {

  /**
   * Constructs a new Tripal storage property base object.
   *
   * @param string entityTypeId
   *   The entity type id associated with this storage property base object.
   *
   * @param string fieldId
   *   The field id associated with this storage property base object.
   *
   * @param string fieldKey
   *   The field key associated with this storage property base object.
   */
  public function __construct($entityTypeId,$fieldId,$fieldKey) {
    $this->entityTypeId = $entityTypeId;
    $this->fieldId = $fieldId;
    $this->fieldKey = $fieldKey;
  }

  /**
   * Returns the entity type id associated with this storage property base
   * object.
   *
   * @return string
   *   The entity type id.
   */
  public function getEntityTypeId() {
    return $this->entityTypeId;
  }

  /**
   * Returns the field id associated with this storage property base object.
   *
   * @return string
   *   The field id.
   */
  public function getFieldId() {
    return $this->fieldId;
  }

  /**
   * Returns the field key associated with this storage property base object.
   *
   * @return string
   *   The field key.
   */
  public function getFieldKey() {
    return $this->fieldKey;
  }

  /**
   * The entity type id associated with this storage property base object.
   *
   * @var string
   */
  private $entityTypeId;

  /**
   * The field id associated with this storage property base object.
   *
   * @var string
   */
  private $fieldId;

  /**
   * The field key associated with this storage property base object.
   *
   * @var string
   */
  private $fieldKey;
}
