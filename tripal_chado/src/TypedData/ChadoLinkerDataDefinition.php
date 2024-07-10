<?php
namespace Drupal\tripal_chado\TypedData;

use Drupal\Core\TypedData\DataDefinition;
use Drupal\Core\TypedData;

/**
 * @{inheritdoc}
 */
class ChadoLinkerDataDefinition extends ChadoComplexDataDefinition {

  /**
   * Creates a new chado data definition.
   *
   * @param string $type
   *   (optional) The data type of the chado data. Defaults to 'chado'.
   *
   * @return static
   */
  public static function create($type = 'chado_linker') {
    $definition['type'] = $type;
    return new static($definition);
  }


  public function getPropertyDefinitions() {
    if (!isset($this->propertyDefinitions)) {
      $this->propertyDefinitions['linker_field'] = DataDefinition::create('string')
      ->setLabel('Linker Field')
      ->setComputed(TRUE)
      ->setReadOnly(TRUE)
      ->setRequired(TRUE);

      $this->propertyDefinitions['linker_id'] = DataDefinition::create('integer')
      ->setLabel('Linker ID')
      ->setComputed(TRUE)
      ->setReadOnly(TRUE)
      ->setRequired(TRUE);
    }
    return $this->propertyDefinitions;
  }
}
