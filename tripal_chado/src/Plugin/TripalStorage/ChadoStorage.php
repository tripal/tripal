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
    foreach ($this->property_types as $field_types) {
      foreach ($field_types as $keys) {
        foreach ($keys as $type) {
          $types[] = $type;
        }
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
    
    $logger = \Drupal::service('tripal.logger');
    $chado = \Drupal::service('tripal.dbx');
        
    // Holds a list of all the indexed property values.
    $indexed_values = [];  
    
    // An array for quick lookup of entity IDs.
    $entity_ids = [];
    
    // An array of dynamic query select statements.
    $selects = [];

    
    // First, index the properties by entity type, field type and key.
    foreach ($values as $value) {
      $entity_id = $value->getEntityId();
      $entity_type = $value->getEntityType();
      $field_type = $value->getFieldType();
      $key = $value->getKey();
      
      // Index the entity IDs.
      if (!array_key_exists($entity_type, $entity_ids)) {
        $entity_ids[$entity_type] = [];
      }
      $entity_ids[$entity_type][] = $entity_id;
      
      // Index the values.
      if (!array_key_exists($entity_type, $indexed_values)) {
        $indexed_values[$entity_type] = [];
      }
      if (!array_key_exists($field_type, $indexed_values[$entity_type])) {
        $indexed_values[$entity_type][$field_type] = [];
      }
      if (array_key_exists($key, $indexed_values[$entity_type])) {        
        $logger->error('Cannot get values for a duplicate property, "@prop".',
            ['@prop' => $entity_type . '.' . $field_type . '.' . $key]);
        return False;
      }
      $indexed_values[$entity_type][$field_type][$key] = $value;
    }
    print_r($entity_ids);
    
    // Constrcut the Dynamic queries.
    foreach (array_keys($entity_ids) as $entity_type) {    
      $type_mapping = $this->getEntityTypeMapping($entity_type);
      if (!$type_mapping) {
        $logger->error('There is no known entity type: "@type".',
            ['@type' => $entity_type]);
        return False;
      }
      foreach ($entity_ids[$entity_type] as $entity_id);
        $id_mapping = $this->getEntityMapping($entity_type, $entity_id);
        $data_table = $type_mapping->data_table;
        $type_table = $type_mapping->type_linker_table;
        $type_id = $type_mapping->type_id;
        $schema = $chado->schema()->getTableDef($data_table, ['format' => 'drupal']);
        $pkey = $schema['primary key'][0];
        
        // @todo remove the "1:" prefix once issue #217 is fixed.
        $query = $chado->select("1:" . $data_table, 'B');
        $selects[$entity_id][$data_table] = $query;
        
        if (!empty($type_table)) {
          $query->join($type_table , 'L', '');
        }
    }
  }  
  
  /**
   * Gets the mapping of an Entity to a record in Chado.
   * 
   * @param string $entityType
   *   The name of the entity type.
   * @param int $entityId
   *   The numeric entity ID.
   *   
   * @return object|NULL
   *   An object mapping the entity ID to a record in the table of Chado 
   *   indicated by the $mapping variable. If a mapping cannot be found a
   *   NULL value is returned.
   */
  protected function getEntityMapping(string $entityType, int $entityId) {
    
    $logger = \Drupal::service('tripal.logger');
    $public = \Drupal::database();  
    
    $entity_type_table = 'chado_' . $entityType;
    
    // Make sure the entity type has a mapping table.
    if (!$public->schema->tableExists($entity_type_table)) {
      $logger->error('There is no Chado record mapping table for the entity type named: "@type".',
          ['@type' => $entityType]);
      return NULL;
    }
    
    // Get the record mapping.
    $result = $public->select($entity_type_table, 'ett')
      ->fields('ett')
      ->condition('entity_id', $entityId)
      ->execute();
    if (!$result) {
      $logger->error('The entity with ID, @id, does not have a matching record in Chado.',
          ['@id' => $entityId]);
      return NULL;
    }
    return $result->fetchObject();
  }
  
  /**
   * Gets the mapping of an EntityType to a Chado table.
   * 
   * @param string $entityType
   *   The name of the entity type.
   * 
   * @return object|NULL.
   *   An object mapping the entity type to a Chado table. If a mpaping cannot
   *   be found a NULL value is returned.
   */
  protected function getEntityTypeMapping(string $entityType) {
    
    $logger = \Drupal::service('tripal.logger');    
    $public = \Drupal::database();    
    
    print_r([$entityType]);
    
    // Get the entity type information, including the bundle_id.
    $result = $public->select('tripal_bundle', 'tb')
      ->fields('tb')
      ->condition('name', $entityType)
      ->execute();
    if (!$result) {     
      $logger->error('There is no known record in the tripal_bundle table for type: "@type".',
          ['@type' => $entityType]);      
      return NULL;
    }
    $tripal_bundle = $result->fetchObject();
    
    // Get the mapping of this bundle to Chado.
    $result = $public->select('chado_bundle', 'cb')
      ->fields('cb')
      ->condition('bundle_id', $tripal_bundle->id)
      ->execute();
    if (!$result) {
      $logger->error('There is no known record in the chado_bundle table for type: "@type".',
          ['@type' => $entityType]);  
      return NULL;
    }
   return $result->fetchObject();    
  }
  
  /**
   * @{inheritdoc}
   */
  
  public function deleteValues($values) {
    
  }
}
