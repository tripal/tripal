<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\tripal\TripalStorage\StoragePropertyBase;

/**
 * Defines the Tripal storage property value.
 */
class StoragePropertyValue extends StoragePropertyBase {

  /**
   * Constructs a new tripal storage property value.
   *
   * @param string entityType
   *   The entity type associated with this storage property value.
   *
   * @param string fieldType
   *   The field type associated with this storage property value.
   *
   * @param string key
   *   The key associated with this storage property value.
   *
   * @param string entityId
   *   The entity id associated with this storage property value.
   *
   * @param ? $value
   *   An optional initial value for this storage property value.
   */
  public function __construct($entityType,$fieldType,$key,$entityId,$value = Null) {
    parent::__construct($entityType,$fieldType,$key);
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
   * @return mixed
   *   The value.
   */
  public function getValue() {
    return $this->value;
  }

  /**
   * Sets the value of this storage property value to the given value.
   *
   * @param mixed $value
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
