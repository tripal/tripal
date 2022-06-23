<?php

namespace Drupal\tripal_chado\Plugin\TripalStorage;

use Drupal\Core\Plugin\PluginBase;
use Drupal\tripal\TripalStorage\Interfaces\TripalStorageInterface;

/**
 * Chado implementation of the TripalStorageInterface.
 *
 * @TripalStorage(
 *   id = "chado_storage",
 *   label = @Translation("Chado Storage"),
 *   description = @Translation("Interfaces with GMOD Chado for field values."),
 * )
 */
class ChadoStorage extends PluginBase implements TripalStorageInterface {
  
  protected $property_types = [];

	/**
	 * @{inheritdoc}
	 */
  public function addTypes($types) {
    
    // Index the types by their entity type, field type and key.
    foreach ($types as $type) {
      $entity_type = $type->getEntityType();
      $field_type = $type->getFieldType();
      $key = $type->getKey();
      if (!array_key_exists($entity_type, $this->property_types)) {
        $this->property_types[$entity_type] = [];
      }
      if (!array_key_exists($field_type, $this->property_types[$entity_type])) {
        $this->property_types[$entity_type][$field_type] = [];
      }
      if (array_key_exists($key, $this->property_types[$entity_type])) {
        $logger = \Drupal::service('tripal.logger'); 
        $logger->error('Cannot add a property type, "@prop", as it already exists', 
            ['@prop' => $entity_type . '.' . $field_type . '.' . $key]);
        return False;
      }
      $this->property_types[$entity_type][$field_type][$key] = $type;
    }
  }
  
  /**
   * @{inheritdoc}
   */  
  public function getTypes() {
    $types = [];
    foreach ($this->property_types as $entity_type => $field_types) {
      foreach ($field_types as $field_type => $keys) {
        $types[] = $this->property_types[$entity_type][$field_type][$key];
      }
    }
    return $types;
  }
  
  /**
	 * @{inheritdoc}
	 */
  public function removeTypes($types) {
    
    foreach ($types as $type) {
      $entity_type = $type->getEntityType();
      $field_type = $type->getFieldType();
      $key = $type->getKey();
      if (!array_key_exists($entity_type, $this->property_types)) {
        if (!array_key_exists($field_type, $this->property_types[$entity_type])) {
          if (array_key_exists($key, $this->property_types[$entity_type])) {
            unset($this->property_types[$entity_type][$field_type][$key]);
          }
        }
      }
    }
  }
  
  /**
	 * @{inheritdoc}
	 */
  public function insertValues($values) {
    
  }
  
  /**
   * @{inheritdoc}
   */
  
  public function updateValues($values) {
    
  }
  
  /**
   * @{inheritdoc}
   */
  
  public function loadValues($values) {
    
  }
  
  /**
   * @{inheritdoc}
   */
  
  public function deleteValues($values) {
    
  }
}
