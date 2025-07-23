<?php

namespace Drupal\tripal_chado\Plugin\DataType;

use Drupal\Core\TypedData\TypedData;
use Drupal\Core\TypedData\ComplexDataInterface;
use Drupal\Core\TypedData\Attribute\DataType;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TypedData\ChadoDataDefinition;

#[DataType(
  id: 'chado_record',
  label: new TranslatableMarkup('Chado'),
  definition_class: ChadoDataDefinition::class,
)]
/**
 * Plugin implementation of the ChadoLinker data type.
 *
 * @DataType(
 *   id = "chado_record",
 *   label = @Translation("Chado"),
 *   definition_class = "\Drupal\tripal_chado\TypedData\ChadoDataDefinition"
 * )
 */
class ChadoDataType extends TypedData implements \IteratorAggregate, ComplexDataInterface {

  /**
   * The data definition.
   *
   * @var \Drupal\Core\TypedData\ComplexDataDefinitionInterface
   */
  protected $definition;

  /**
   * An array of values for the contained properties.
   *
   * @var array
   */
  protected $values = [];

  /**
   * The array of properties.
   *
   * @var \Drupal\Core\TypedData\TypedDataInterface[]
   */
  protected $properties = [];

  /**
   * {@inheritdoc}
   */
  public function getValue() {
    return $this->value;
  }

  /**
   * {@inheritdoc}
   */
  public function getIterator(): \Traversable {
    return new \ArrayIterator($this->list);
  }

  /**
   * {@inheritdoc}
   */
  public function get($property_name) {
    if (!isset($this->properties[$property_name])) {
      $value = NULL;
      if (isset($this->values[$property_name])) {
        $value = $this->values[$property_name];
      }

      // If the property is unknown, this will throw an exception.
      $this->properties[$property_name] = $this->getTypedDataManager()
        ->getPropertyInstance($this, $property_name, $value);
    }
    return $this->properties[$property_name];
  }

  /**
   * {@inheritdoc}
   */
  public function set($property_name, $value, $notify = TRUE) {

    // Separate the writing in a protected method, such that onChange
    // implementations can make use of it.
    $this->writePropertyValue($property_name, $value);
    $this->onChange($property_name, $notify);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getProperties($include_computed = FALSE) {
    $properties = [];
    foreach ($this->definition
      ->getPropertyDefinitions() as $name => $definition) {
      if ($include_computed || !$definition
        ->isComputed()) {
        $properties[$name] = $this
          ->get($name);
      }
    }
    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public function toArray() {
    $values = [];
    foreach ($this
      ->getProperties() as $name => $property) {
      $values[$name] = $property
        ->getValue();
    }
    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    foreach ($this->properties as $property) {
      $definition = $property
        ->getDataDefinition();
      if (!$definition
        ->isComputed() && $property
        ->getValue() !== NULL) {
        return FALSE;
      }
    }
    if (isset($this->values)) {
      foreach ($this->values as $name => $value) {
        if (isset($value) && !isset($this->properties[$name])) {
          return FALSE;
        }
      }
    }
    return TRUE;
  }

  /**
   * React to changes to a child property or item.
   *
   * Note that this is invoked after any changes have been applied.
   *
   * @param string $name
   *   The name of the property or the delta of the list item which is changed.
   */
  public function onChange($name) {

    // Notify the parent of changes.
    if ($notify && isset($this->parent)) {
      $this->parent->onChange($this->name);
    }
  }

}
