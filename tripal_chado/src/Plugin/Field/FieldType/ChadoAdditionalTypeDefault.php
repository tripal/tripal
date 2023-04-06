<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoVarCharStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal additional type field type.
 *
 * @FieldType(
 *   id = "chado_additional_type_default",
 *   label = @Translation("Chado Type Reference"),
 *   description = @Translation("A Chado type reference"),
 *   default_widget = "chado_additional_type_widget_default",
 *   default_formatter = "chado_additional_type_formatter_default"
 * )
 */
class chado_additional_type_default extends ChadoFieldItemBase {

  public static $id = "chado_additional_type_default";


  /**
   * {@inheritdoc}
   */
  public static function mainPropertyName() {
    return 'term_name';
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();
    // If a fixed value is set, then the field will will always use the
    // same value and the user will not be allowed the change it using the
    // widget.  This is necessary for content types that correspond to Chado
    // tables with a type_id that should always match the content type (e.g.
    // gene).
    $settings['fixed_value'] = FALSE;
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['type_table'] = '';
    $settings['storage_plugin_settings']['type_column'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    // Get the Chado table and column this field maps to.
    $storage_settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $storage_settings['base_table'];
    $type_table = $storage_settings['type_table'] ?? '';
    $type_column = $storage_settings['type_column'] ?? '';

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table or !$type_table) {
      $record_id_term = 'SIO:000729';
      return [
        new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
          'action' => 'store_id',
          'drupal_store' => TRUE,
        ])
      ];
    }

    // Get the the connecting information about the base table and the
    // table where the type is stored. If the base table has a `type_id`
    // column then the base table and the type table are the same. If we
    // are using a prop table to store the type_id then the type table and
    // base table will be different.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_table_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_table_def['primary key'];

    // Create variables to store the terms for the properties. We can use terms
    // from Chado tables if appropriate.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $type_id_term = $mapping->getColumnTermId($type_table, $type_column);
    $name_term = $mapping->getColumnTermId('cvterm', 'name');
    $idspace_term = 'SIO:000067';
    $accession_term = $mapping->getColumnTermId('dbxref', 'accession');

    // Always store the record id of the base record that this field is
    // associated with in Chado.
    $properties = [];
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
      'action' => 'store_id',
      'drupal_store' => TRUE,
      'chado_table' => $base_table,
      'chado_column' => $base_pkey_col
    ]);

    // If the type table and the base table are not the same then we are
    // storing the type in a prop table and we need the pkey for the prop
    // table, the fkey linking to the base table, and we'll set a value
    // of the type name.
    if ($type_table != $base_table) {
      $type_table_def = $schema->getTableDef($type_table, ['format' => 'Drupal']);
      $type_pkey_col = $type_table_def['primary key'];
      $type_fkey_col = array_keys($type_table_def['foreign keys'][$base_table]['columns'])[0];
      $link_term = $mapping->getColumnTermId($type_table, $type_fkey_col);
      $value_term = $mapping->getColumnTermId($type_table, 'value');

      $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'prop_id', $record_id_term, [
        'action' => 'store_pkey',
        'drupal_store' => TRUE,
        'chado_table' => $type_table,
        'chado_column' => $type_pkey_col,
      ]);
      $properties[] =  new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'link_id', $link_term, [
        'action' => 'store_link',
        'chado_table' => $type_table,
        'chado_column' => $type_fkey_col,
      ]);
      $properties[] =  new ChadoTextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'chado_table' => $type_table,
        'chado_column' => 'value',
      ]);
    }

    // We need to store the numeric cvterm ID for this field.
    $properties[] = new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'type_id', $type_id_term, [
      'action' => 'store',
      'chado_table' => $type_table,
      'chado_column' => $type_column,
      'empty_value' => 0
    ]);
    // This field needs the term name, idspace and accession for proper
    // display of the type.
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'term_name', $name_term, 128, [
      'action' => 'join',
      'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id',
      'chado_column' => 'name',
      'as' => 'term_name'
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'id_space', $idspace_term, 128, [
      'action' => 'join',
      'path' => $type_table . '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id;dbxref.db_id>db.db_id',
      'chado_column' => 'name',
      'as' => 'idSpace'
    ]);
    $properties[] = new ChadoVarCharStoragePropertyType($entity_type_id, self::$id, 'accession', $accession_term, 128, [
      'action' => 'join',
      'path' => $type_table. '.' . $type_column . '>cvterm.cvterm_id;cvterm.dbxref_id>dbxref.dbxref_id',
      'chado_column' => 'accession',
      'as' => 'accession'
    ]);

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
