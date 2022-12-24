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
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_table'] = '';
    $settings['storage_plugin_settings']['type_table'] = '';
    $settings['storage_plugin_settings']['type_column'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the base Chado table and column this field maps to.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
    $type_table = $storage_settings['type_table'];
    $type_column = $storage_settings['type_column'];

    // Get the base table columns needed for this field.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    $properties = [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id,'record_id', [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'value', [
        'action' => 'store',
        'chado_table' => $type_table,
        'chado_column' => $type_column,
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'term_name', 128, [
        'action' => 'join',
        'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id',
        'chado_column' => 'name',
        'as' => 'term_name'
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'id_space', 128, [
        'action' => 'join',
        'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id',
        'chado_column' => 'name',
        'as' => 'idSpace'
      ]),
      new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'accession', 128, [
        'action' => 'join',
        'path' => $type_table. '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id',
        'chado_column' => 'accession',
        'as' => 'accession'
      ]),
    ];

    // If the chado table is not the same as the base table then we are storing the type
    // in a property table.  We need to set the FK column of the prop table.
    if ($type_table != $base_table) {
      $chado = \Drupal::service('tripal_chado.database');
      $schema = $chado->schema();
      $schema_def = $schema->getTableDef($type_table, ['format' => 'Drupal']);
      $pk_field = $schema_def['primary key'];
      $fk_field = array_keys($schema_def['foreign keys'][$base_table]['columns'])[0];
      $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'linker_id', [
        'drupal_store' => TRUE,
        'action' => 'link',
        'chado_table' => $type_table,
        'chado_column' => $pk_field,
        'link_column' => $fk_field,
      ]);
    }
    return $properties;
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
    $type_table = $storage_settings['type_table'];

    $properties = [
      new StoragePropertyValue($entity_type_id, self::$id, 'record_id', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, 'value', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, 'term_name', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, 'id_space', $entity_id),
      new StoragePropertyValue($entity_type_id, self::$id, 'accession', $entity_id),
    ];
    if ($type_table != $base_table) {
      $properties[] = new StoragePropertyValue($entity_type_id, self::$id, 'linker_id', $entity_id);
    }
    return $properties;
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
