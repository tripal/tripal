<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\Core\DependencyInjection\DependencySerializationTrait;

/**
 * Defines the Tripal storage property value.
 */
class StoragePropertyValue extends StoragePropertyBase {

  use DependencySerializationTrait;

  /**
   * Constructs a new tripal storage property value.
   *
   * @param string $entityType
   *   The entity type associated with this storage property value.
   * @param string $fieldType
   *   The field type associated with this storage property value.
   * @param string $key
   *   The key associated with this storage property value.
   * @param string $term_id
   *   The controlled vocabulary term asssociated with this property. It must be
   *   in the form of "IdSpace:Accession" (e.g. "rdfs:label" or "OBI:0100026")
   * @param string $entityId
   *   The entity id associated with this storage property value.
   * @param ? $value
   *   An optional initial value for this storage property value.
   */
  public function __construct($entityType, $fieldType, $key, $term_id, $entityId, $value = NULL) {
    parent::__construct($entityType, $fieldType, $key, $term_id);

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
    if (is_null($this->value)) {
      return $this->default_value;
    }
    else {
      return $this->value;
    }
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
   * Sets the default value of this storage property value to the given value.
   *
   * @param mixed $default_value
   *   The value to use as the default value.
   */
  public function setDefaultValue($default_value) {
    $this->default_value = $default_value;
  }

  /**
   * Gets the default value of this storage property value.
   *
   * @return mixed
   *   The value to use as the default value.
   */
  public function getDefaultValue() {
    return $this->default_value;
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

  /**
   * The default value for an empty property value.
   *
   * @var ?
   */
  private $default_value;

}
