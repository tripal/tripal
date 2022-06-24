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
  
  public function loadValues(&$values) {
    $indexed_values = $this->indexValues($values);
    $selects = $this->buildSelect($indexed_values);
    $results = $this->executeSelect($selects);
    $this->populateValues($indexed_values, $results, $values);
  }
  
  /**
   * @{inheritdoc}
   */  
  public function deleteValues($values) {
    
  }
  
  /**
   * @{inheritdoc}
   */
  public function findValues($match) {
    
  }
  
  /**
   * Indexes a values array for easy lookup.
   * 
   * @param array $values
   *   Array of \Drupal\tripal\TripalStorage\StoragePropertyValue objects.  
   * @return array
   *   An associative array indexed by enhtityType -> entityId -> 
   *   fieldType -> key with the value being the value.
   */
  protected function indexValues($values) {
    
    $logger = \Drupal::service('tripal.logger');
    
    $indexed_values = [];  
    
    // First, index the properties by entity type, field type and key.
    foreach ($values as $val_index => $value) {
      
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
      // mapped to Chado should be ignored. But don't report an error. Some 
      // fields may not be mapped to a table.
      $type_mapping = $this->getEntityTypeMapping($entity_type);
      if (!$type_mapping) {
        continue;
      }
      $this->type_mapping[$entity_type] = $type_mapping;
      
      // Property value objects that don't have an ID mapping to Chado
      // should be ignored. But don't report an error. Some
      // fields may not be mapped to a table.
      $id_mapping = $this->getEntityMapping($entity_type, $entity_id);
      if (!$id_mapping) {
        continue;
      }
      $this->id_mapping[$entity_type][$entity_id] = $id_mapping;
      
      
      // Index the value.
      $indexed_values[$entity_type][$entity_id][$field_type][$key] = $val_index;
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
   * Builds a set of dynamic update queries for Chado based on the values.
   *
   * @param array $indexed_values
   *   An associative array as returned by the indexValues function.
   * @return array
   *   An associative array indexed by entityType -> entityId -> Chado
   *   table name. The values are Dynamic query object ready for execution.
   */
  protected function buildDelete($indexed_values) {
    
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
          foreach (array_keys($keys) as $key) {
            $type_mapping = $this->type_mapping[$entity_type];
            $id_mapping = $this->id_mapping[$entity_type][$entity_id];
            $type = $this->property_types[$entity_type][$field_type][$key];
            $base_table = $type_mapping->data_table;
            $record_id = $id_mapping->record_id;
            $chado_table = $type->getTable();
            $chado_column = $type->getColumn();
            $base_fk_column = $type->getBaseFkColumn();
            $chado_fk_column = $type->getTableFkColumn();
            
            // If this is the first time we've seen this Chado table then
            // start the query for it.
            if (!array_key_exists($chado_table, $selects[$entity_type][$entity_id])) {
              
              $selects[$entity_type][$entity_id][$chado_table] = NULL;
              
              // Get the table schema definitions.
              $chado_table_def = $chado->schema()->getTableDef($chado_table, ['format' => 'drupal']);
              $base_table_def = $chado->schema()->getTableDef($base_table, ['format' => 'drupal']);              
              
              // @todo remove the "1:" prefix once issue #217 is fixed.
              $query = $chado->select('1:' . $chado_table, 'prop');              
              
              // If the chado table for this property is not the base table then
              // we need to link to the base table.
              if ($chado_table != $base_table) {
                
                // If the base table links to the chado table, do a join.
                if (!empty($base_fk_column)) {
                  
                  // If the requested linker column is missing in the base table
                  // then we can't find a value.
                  if (!array_key_exists($base_fk_column, $base_table_def['fields'])) {
                    continue;
                  }
                  $chado_table_pkey = $chado_table_def['primary key'];
                  $base_table_pkey = $base_table_def['primary key'];
                  // @todo remove the "1:" prefix once issue #217 is fixed.
                  $query->join('1:' . $base_table, 'base', 'base.' . $base_fk_column . ' = ' . 'prop.' . $chado_table_pkey);
                  $query->condition('base.' . $base_table_pkey, $record_id);
                }
                // Else, the chado table links to the base table.
                else {         
                  
                  // If there is no FK column between this table and the base
                  // table then we can't find any values for it.
                  if (!array_key_exists($base_table, $chado_table_def['foreign keys'])) {
                    continue;
                  }
                  
                  // If no FK column was specified then use the first one found.
                  if (!$chado_fk_column) {
                    $chado_fk_column = array_keys($chado_table_def['foreign keys'][$base_table]['columns'])[0];
                  }
                  $query->condition('prop.' . $chado_fk_column, $record_id);
                }                
              }
              // Else, the base table and the chado table are the same.
              else {
                $base_table_pkey = $base_table_def['primary key'];
                $query->condition('prop.' . $base_table_pkey, $record_id);
              }
              
              // If the table has a rank column then order by rank.
              if (array_key_exists('rank', $chado_table_def['fields'])) {
                $query->orderBy('prop.rank', 'ASC');
              }
               
              // Store the query so we can add to it.
              $selects[$entity_type][$entity_id][$chado_table] = $query;
            }
            
            // Add the field for this property to the query.
            $query = $selects[$entity_type][$entity_id][$chado_table];            
            if (!is_null($query) and array_key_exists($chado_column, $chado_table_def['fields'])) {              
              $query->addField('prop', $chado_column, $this->sanitizeFieldKey($key));
            }
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
   *   table name. The value is an array of results as returned by fetchAssoc().
   */
  protected function executeSelect($selects) {
    $logger = \Drupal::service('tripal.logger');
    
    
    $results = [];
    foreach ($selects as $entity_type => $entity_ids) {
      $results[$entity_type] = [];
      foreach ($entity_ids as $entity_id => $chado_tables) {
        $results[$entity_type][$entity_id] = [];
        foreach ($chado_tables as $chado_table => $query) {
          $results[$entity_type][$entity_id][$chado_table] = [];
          if (is_null($query)) {
            continue;
          }
          $result = $query->execute();
          if (!$result) {            
            $logger->error('Problem executing select query for Chado table, "@table", on entity ID, "@id".',
                ['@table' => $chado_table, '@id' => $entity_id]);
            continue;
          }
          while ($record = $result->fetchAssoc()) {
            $results[$entity_type][$entity_id][$chado_table][] = $record;
          }
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
  protected function populateValues($indexed_values, $results, &$values) {
    
    $logger = \Drupal::service('tripal.logger');
    
    foreach ($indexed_values as $entity_type => $entity_ids) {
      foreach ($entity_ids as $entity_id => $field_types) {
        foreach ($field_types as $field_type => $keys) {
          foreach ($keys as $key => $val_index) {
            $skey = $this->sanitizeFieldKey($key);
            $type = $this->property_types[$entity_type][$field_type][$key];
            $cardinality = $type->getCardinality();
            $chado_table = $type->getTable();
            
            // Get the values from the Chado records for this property.
            $vals = [];
            $records = $results[$entity_type][$entity_id][$chado_table];
            foreach ($records as $record) {
              $vals[] = $record[$skey];
            }
            
            // Set the value for this property.
            if (count($vals) == 0) {
              $values[$val_index]->setValue(NULL);
            }
            if (count($vals) == 1) {
              $values[$val_index]->setValue($vals[0]);
            }
            if (count($vals) > 1) {
              
              if ($cardinality == 0 or count($vals) <= $cardinality) {
                $values[$val_index]->setValue($vals);
              }
              else {
                $values[$val_index]->setValue(NULL);
                $logger->error('The property, "@prop" has @n values but only allows @m. Skipping.',[
                  '@prop' => $entity_type . "." . $field_type . '.' . $key, 
                  '@n' => count($vals),
                  '@m' => $cardinality                  
                ]);
              }              
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
}