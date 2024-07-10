<?php

namespace Drupal\tripal_chado\TypedData;

use Drupal\Core\TypedData\ComplexDataDefinitionBase;

/**
 * @{inheritdoc}
 */
class ChadoDataDefinition extends ComplexDataDefinitionBase {

  /**
   * Creates a new chado data definition.
   *
   * @param string $type
   *   (optional) The data type of the chado data. Defaults to 'chado'.
   *
   * @return static
   */
  public static function create($type = 'chado') {
    $definition['type'] = $type;
    return new static($definition);
  }

  /**
   * {@inheritdoc}
   */
  public function getPropertyDefinitions() {
    if (!isset($this->propertyDefinitions)) {
      $properties = [];
      $this->propertyDefinitions = $properties;
    }
    return $this->propertyDefinitions;
  }

  /**
   * Allows fields to dynamically add nested chado data definitions.
   *
   * @param string $key
   *   The name of the data definition. This should be the key for a
   *   specific piece of data (e.g. genus).
   * @param object $definition
   *   The Data definition for this particular piece of data. This should be an
   *   object whose type implements the
   *   \Drupal\Core\TypedData\DataDefinitionInterface.
   */
  public function addPropertyDefinition($key, $definition) {
    $this->propertyDefinitions[$key] = $definition;
  }

  /**
   *
   */
  public function setSearchable($value) {
    $this->definition['settings']['searchable'] = $value;
    return $this;
  }

  /**
   *
   */
  public function setSearchOperations($value) {
    $this->definition['settings']['search_operations'] = $value;
    return $this;
  }

  /**
   *
   */
  public function setSortable($value) {
    $this->definition['settings']['sortable'] = $value;
    return $this;
  }
}
