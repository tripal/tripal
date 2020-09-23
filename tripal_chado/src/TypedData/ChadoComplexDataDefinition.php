<?php

namespace Drupal\tripal_chado\TypedData;

use Drupal\Core\TypedData\ComplexDataDefinitionBase;

/**
 * @{inheritdoc}
 */
class ChadoComplexDataDefinition extends ComplexDataDefinitionBase {

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
      $this->propertyDefinitions = [];
    }
    return $this->propertyDefinitions;
  }
}
