<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "schema__additional_type",
 *   label = @Translation("Chado Type Reference"),
 *   description = @Translation("A Chado type reference"),
 *   default_widget = "schema__additional_type_widget",
 *   default_formatter = "schema__additional_type_formatter"
 * )
 */
class schema__additional_type extends ChadoFieldItemBase {

  public static $id = "schema__additional_type"; 

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the Chado table and column this field maps to.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];     
    $property_settings = $storage_settings['property_settings'];
    $chado_table = $property_settings['value']['chado_table'];
    $chado_column = $property_settings['value']['chado_column'];

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return TripalFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    }

    // Indicate the action to perform for each property.
    $value_settings = $property_settings['value'];
    $idspace_settings = [
      'action' => 'join',
      'path' => $chado_table . '.' . $chado_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id',
      'chado_column' => 'name',
      'as' => 'idSpace'
    ];
    $accession_settings = [
      'action' => 'join',
      'path' => $chado_table. '.' . $chado_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id',
      'chado_column' => 'accession',
      'as' => 'accession'
    ];
    $name_settings = [
      'action' => 'join',
      'path' => $chado_table . '.' . $chado_column . '>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'term_name'
    ];
    
    $value = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'value', $value_settings);
    $name = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'schema_name', 128, $name_settings);
    $idSpace = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'NCIT_C42699', 128, $idspace_settings);
    $accession = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'data_2091', 128, $accession_settings);    
    $types = [$value, $name, $idSpace, $accession];
    
    // If the chado table is not the same as the base table then we are storing the type
    // in a property table.  We need to set the FK column of the prop table.
    if ($chado_table != $base_table) {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();
      $schema_def = $schema->getTableDef($chado_table, ['format' => 'Drupal']);
      $pk_field = $schema_def['primary key'];
      $fk_field = array_keys($schema_def['foreign keys'][$base_table]['columns'])[0];
      $link_settings = [
        'cache' => TRUE,
        'action' => 'link',
        'chado_table' => $chado_table,
        'chado_column' => $pk_field,
        'link_column' => $fk_field,
      ];
      $link = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', $link_settings);
      $types[] = $link;
    }
        
    // Return the list of property types.
    $default_types = ChadoFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    $types = array_merge($types, $default_types);    
    return $types;
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate($field_definition) {
    $entity = $this->getEntity();
    $entity_type_id = $entity->getEntityTypeId();
    $entity_id = $entity->id();
    
    // Get the Chado table and column this field maps to.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
    $property_settings = $storage_settings['property_settings'];
    $chado_table = $property_settings['value']['chado_table'];
    $chado_column = $property_settings['value']['chado_column'];
    
    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return TripalFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    }

    // Build the values array.
    $values = [
      new StoragePropertyValue($entity_type_id, self::$id, 'value', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, 'schema_name', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, 'NCIT_C42699', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, 'data_2091', $entity_id),
    ];
    if ($chado_table != $base_table) {
      $values[] = new StoragePropertyValue($entity_type_id, self::$id, 'linker_id', $entity_id);
    }
    $default_values = ChadoFieldItemBase::defaultTripalValuesTemplate($entity_type_id, self::$id, $entity_id);
    $values = array_merge($values, $default_values);
    return $values;
  }

  /**
   * A callback function for setting the type property value.
   *
   * This function is called by the ChadoStorage class if a property type
   * sets the action as 'function'.
   */
  public static function setTypePropertyValue() {

  }

}
