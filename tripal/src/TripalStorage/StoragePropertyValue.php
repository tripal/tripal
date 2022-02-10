<?php

namespace Drupal\tripal4\TripalStorage;

use Drupal\tripal4\TripalStorage\StoragePropertyBase;

/**
 * Defines the Tripal storage property value.
 */
class StoragePropertyValue extends StoragePropertyBase {

  /**
   * Constructs a new tripal storage property value.
   *
   * @param string entityTypeId
   *   The entity type id associated with this storage property value.
   *
   * @param string fieldId
   *   The field id associated with this storage property value.
   *
   * @param string fieldKey
   *   The field key associated with this storage property value.
   *
   * @param string entityId
   *   The entity id associated with this storage property value.
   *
   * @param ? $value
   *   An optional initial value for this storage property value.
   */
  public function __construct($entityTypeId,$fieldTypeId,$fieldKey,$entityId,$value = Null) {
    parent::__construct($entityId,$fieldId,$fieldKey);
    $this->entityId = $entityId;
    $this->value = $value;
  }

  /**
   * Returns the entity id associated with this storage property value.
   *
   * @return string
   *   The entity id.
   */
  public function getEntityId() {
    return $this->entityId;
  }

  /**
   * Returns the value of this storage property value.
   *
   * @return ?
   *   The value.
   */
  public function getValue() {
    return $this->value;
  }

  /**
   * Sets the value of this storage property value to the given value.
   *
   * @param ? $value
   *   The value.
   */
  public function setValue($value) {
    $this->value = $value;
  }

  /**
   * The entity id associated with this storage property value.
   *
   * @var string
   */
  private $entityId;

  /**
   * The value of this storage property value.
   *
   * @var ?
   */
  private $value;
}
