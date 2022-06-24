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
  
  /**
   * An associative array that contains all of hte property types that 
   * have been added to this object.  It is indexed by entityType ->
   * fieldType -> key and the value is the 
   * Drupal\tripal\TripalStoreage\StoragePropertyValue object.
   * 
   * @var array
   */
  protected $property_types = [];
  
  /**
   * An associative array that holds the data for mapping an 
   * entityTypes to Chado tables.  It is indexed by entityType and the
   * value is the object containing the mapping information. 
   * 
   * @var array
   */
  protected $type_mapping = [];
  
  /**
   * An associative array that holds the data for mapping a 
   * fieldType key to a Chado table column for a given entity.  It is indexed 
   * by entityType -> entityID and the value is the object containing the 
   * mapping information. 
   * 
   * @var array
   */
  protected $id_mapping = [];

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
    $indexed_values = $this->indexValues($values);
    
  }
  
  /**
   * @{inheritdoc}
   */
  
  public function updateValues($values) {
    $indexed_values = $this->indexValues($values);
    
  }
  
  /**
   * @{inheritdoc}
   */
  
  public function loadValues($values) {
    $indexed_values = $this->indexValues($values);
    $selects = $this->buildSelect($indexed_values);
    $results = $this->executeSelect($selects);
    $this->populateValues($indexed_values, $results);
  }
  
  /**
   * Indexes a values array for easy lookup.
   * 
   * @param array $values
   *   Array of \Drupal\tripal4\TripalStorage\StoragePropertyValue objects.  
   * @return array
   *   An associative array indexed by enhtityType -> entityId -> 
   *   fieldType -> key with the value being the value.
   */
  protected function indexValues($values) {
    
    $logger = \Drupal::service('tripal.logger');
    
    $indexed_values = [];  
    
    // First, index the properties by entity type, field type and key.
    foreach ($values as $value) {
      
      $entity_id = $value->getEntityId();
      $entity_type = $value->getEntityType();
      $field_type = $value->getFieldType();
      $key = $value->getKey();
      
      // Build the index structure
      if (!array_key_exists($entity_type, $indexed_values)) {
        $indexed_values[$entity_type] = [];
      }
      if (!array_key_exists($entity_id, $indexed_values[$entity_type])) {
        $indexed_values[$entity_type][$entity_id] = [];
      }
      if (!array_key_exists($field_type, $indexed_values[$entity_type][$entity_id])) {
        $indexed_values[$entity_type][$entity_id][$field_type] = [];
      }
      if (array_key_exists($key, $indexed_values[$entity_type][$entity_id][$field_type])) {
        $logger->error('Cannot get values for a duplicate property, "@prop".',
            ['@prop' => $entity_type . '.' . $field_type . '.' . $key]);
        return False;
      }
      
      // Ignore property value objects that don't map to a proper 
      // property type.
      if (!array_key_exists($entity_type, $this->property_types) or
          !array_key_exists($field_type, $this->property_types[$entity_type]) or
          !array_key_exists($key, $this->property_types[$entity_type][$field_type])) {
        $logger->error('The value, @key, has an unknown type. Ignoring it.',
            ['@key' => $entity_type . '.' . $field_type . '.' . $key]);
        continue;
      }
      
      // Ignore property value for types that aren't valid.
      $type = $this->property_types[$entity_type][$field_type][$key];     
      if (!$type->isValid()) {
        $logger->error('The value, @key, has an invalid type. Ignoring it.',
            ['@key' => $entity_type . '.' . $field_type . '.' . $key]);
        continue;
      }
      
      // Property value objects that are assigend to an entity type that isn't 
      // mapped to Chado should be ignored.
      $type_mapping = $this->getEntityTypeMapping($entity_type);
      if (!$type_mapping) {
        $logger->error('There is no mapping of entity type, "@type", to Chado. Ignoring property, "@prop".',
            ['@type' => $entity_type, '@prop' => $entity_type . '.' . $field_type . '.' . $key]);
        continue;
      }
      $this->type_mapping[$entity_type] = $type_mapping;
      
      // Property value objects that don't have an ID mapping to Chado
      // should be ignored.
      $id_mapping = $this->getEntityMapping($entity_type, $entity_id);
      if (!$id_mapping) {
        $logger->error('There is no known mapping of entity ID, "@id", to Chado. Ignoring property, "@prop".',
            ['@id' => $entity_id, '@prop' => $entity_type . '.' . $field_type . '.' . $key]);
        continue;
      }
      $this->id_mapping[$entity_type][$entity_id] = $id_mapping;
      
      
      // Index the value.
      $indexed_values[$entity_type][$entity_id][$field_type][$key] = $value;
    }
    
    return $indexed_values;
  }
  
  /**
   * Builds a set of dynamic update queries for Chado based on the values.
   * 
   * @param array $indexed_values
   *   An associative array as returned by the indexValues function.
   * @return array
   *   An associative array indexed by entityType -> entityId -> Chado 
   *   table name. The values are Dynamic query object ready for execution.
   */
  protected function buildUpdate($indexed_values) {

    $logger = \Drupal::service('tripal.logger');
    $chado = \Drupal::service('tripal_chado.database');
    
    // An array of dynamic query select statements.
    $updates = [];
    
    return $updates;
  }
  
  /**
   * Builds a set of dynamic update queries for Chado based on the values.
   *
   * @param array $indexed_values
   *   An associative array as returned by the indexValues function.
   * @return array
   *   An associative array indexed by entityType -> entityId -> Chado
   *   table name. The values are Dynamic query object ready for execution.
   */
  protected function buildInsert($indexed_values) {
    
    $logger = \Drupal::service('tripal.logger');
    $chado = \Drupal::service('tripal_chado.database');
    
    // An array of dynamic query select statements.
    $inserts = [];
    
    return $inserts;
  }
  
  /**
   * Builds a set of dynamic select queries for Chado based on the values.
   * 
   * @param array $indexed_values
   *   An associative array as returned by the indexValues function.
   * @return array
   *   An associative array indexed by entityType -> entityId -> Chado 
   *   table name. The values are Dynamic query object ready for execution.
   */
  protected function buildSelect($indexed_values) {
    
    $chado = \Drupal::service('tripal_chado.database');
    
    // Holds the list of query statements.
    $selects = [];
    
    foreach ($indexed_values as $entity_type => $entity_ids) {
      $selects[$entity_type] = [];      
      foreach ($entity_ids as $entity_id => $field_types) {
        $selects[$entity_type][$entity_id] = [];        
        foreach ($field_types as $field_type => $keys) {          
          foreach ($keys as $key => $value) {
            $type_mapping = $this->type_mapping[$entity_type];
            $id_mapping = $this->id_mapping[$entity_type][$entity_id];
            $type = $this->property_types[$entity_type][$field_type][$key];
            $base_table = $type_mapping->data_table;
            $record_id = $id_mapping->record_id;
            $chado_table = $type->getChadoTable();
            $chado_column = $type->getChadoColumn();
            
            // If this is the first time we've seen this Chado table then
            // start the query for it.
            if (!array_key_exists($chado_table, $selects[$entity_type][$entity_id])) {
              $chado_table_def = $chado->schema()->getTableDef($chado_table, ['format' => 'drupal']);
              $base_table_def = $chado->schema()->getTableDef($base_table, ['format' => 'drupal']);              
              
              // @todo remove the "1:" prefix once issue #217 is fixed.
              $query = $chado->select('1:' . $chado_table, 'T');              
              
              // If the chado table for this property is not the base table then
              // we need to link to the base table to get the records in this
              // table that are linked to the record_id for the entity.
              if ($chado_table != $base_table) {                
                $base_fk = array_keys($chado_table_def['foreign keys'][$base_table])[0];
                $query->condition('T.' . $base_fk, $record_id);
              }
              else {
                $pkey = $base_table_def['primary key'];
                $query->condition('T.' . $pkey, $record_id);
              }
              
              // If the table has a rank column then order by rank.
              if (array_key_exists('rank', $chado_table_def['fields'])) {
                $query->orderBy('T.rank', 'ASC');
              }
               
              // Store the query so we can add to it.
              $selects[$entity_type][$entity_id][$chado_table] = $query;
            }
            
            // Add the field for this property to the query.
            $query = $selects[$entity_type][$entity_id][$chado_table];
            $query->addField('T', $chado_column, $this->sanitizeFieldKey($key));
          }
        }
      }
    }    
    
    return $selects;
  }
  
  /**
   * Executes an array of dynamic select queries.
   * 
   * @param array $selects
   *   An array of select queries as returned by the buildSelect function.
   * @return array
   *   An associative array indexed by entityType -> entityId -> Chado 
   *   table name. The value is the query results object.
   */
  protected function executeSelect($selects) {
    $logger = \Drupal::service('tripal.logger');
    
    $results = [];
    foreach ($selects as $entity_type => $entity_ids) {
      $results[$entity_type] = [];
      foreach ($entity_ids as $entity_id => $chado_tables) {
        $results[$entity_type][$entity_id] = [];
        foreach ($chado_tables as $chado_table => $query) {
          $result = $query->execute();
          if (!$result) {
            $results[$entity_type][$entity_id][$chado_table] = NULL;
            $logger->error('Problem executing select query for Chado table, "@table", on entity ID, "@id".',
                ['@table' => $chado_table, '@id' => $entity_id]);
            continue;
          }
          $results[$entity_type][$entity_id][$chado_table] = $result;
        }
      }
    }
    return $results;
  }
  
  /**
   * Sets the values for each property.
   * 
   * @param array $results
   *   An array of query results as returned by executeSelect
   */
  protected function populateValues($indexed_values, $results) {
    
    foreach ($indexed_values as $entity_type => $entity_ids) {
      foreach ($entity_ids as $entity_id => $field_types) {
        foreach ($field_types as $field_type => $keys) {
          foreach ($keys as $key => $value) {
            $skey = $this->sanitizeFieldKey($key);
            $type = $this->property_types[$entity_type][$field_type][$key];
            $chado_table = $type->getChadoTable();
            $values = [];
            $result = $results[$entity_type][$entity_id][$chado_table];
            while ($record = $result->fetchAssoc()) {
              $values[] = $record[$skey];
            }
            
            // @todo: check the cardinatlity of a property and make sure we don't
            // exceed it.                        
            if (count($values) == 1) {
              $value->setValue($values[0]);
            }
            if (count($values) > 1) {
              $value->setValue($values);
            }
          }
        }
      }
    }    
  }
   
  
  /**
   * Cleans the names of the field types for use in SQL queries.
   * 
   * @param string $field_type
   *   The name of the field type.
   * @return string
   *   The sanitized name.
   */
  protected function sanitizeFieldKey($key) {
    return preg_replace('/[^\w]/', '_', $key);
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
    if (!$public->schema()->tableExists($entity_type_table)) {
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
/* // Get the base record as the starting point.
 // @todo remove the "1:" prefix once issue #217 is fixed.
 $query = $chado->select("1:" . $data_table, 'B');
 $query->fields('B', [$pkey]);
 $query->condition("B." . $pkey, $id_mapping->record_id);
 
 // If this entity type needs a type to distinguish it then add that in.
 if (!empty($type_table)) {
 $type_table_def = $chado->schema()->getTableDef($type_table, ['format' => 'drupal']);
 $linker_col = array_keys($type_table_def['foreign keys'][$data_table])[0];
 $query->join($type_table , 'L', 'B.' . $pkey . ' = ' .  'L.' . $linker_col);
 if (!empty($type_column)) {
 if (!empty($type_id)) {
 $query->addCondition('L.' . $type_column, $type_id, '=');
 }
 // For prop tables there could be a value that is needed to match.
 if (!empty($type_value)) {
 $query->addCondition('L.value', $type_value, '=');
 }
 }
 }
 // If a type_id is set but not a type_table then the table has a
 // type_id column.
 else {
 if (!empty($type_id)) {
 $query->addCondition('B.type_id', $type_id, '=');
 }
 }
 
 $selects[$entity_id][$data_table] = $query; */