<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;


/**
 * Plugin implementation of the 'text' field type for Chado.
 *
 * @FieldType(
 *   id = "chado_text_type",
 *   label = @Translation("Chado Text Field Type"),
 *   description = @Translation("A text field."),
 *   default_widget = "chado_text_type_widget",
 *   default_formatter = "chado_text_type_formatter"
 * )
 */
class ChadoTextTypeItem extends ChadoFieldItemBase {

  public static $id = "chado_text_type";

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['storage_plugin_settings']['base_column'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $base_table = $settings['base_table'];
    $base_column = $settings['base_column'];

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();

    // Get the base table columns needed for this field.
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id,'record_id', [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
      ]),
      new TextStoragePropertyType($entity_type_id, self::$id, "value", [
        'action' => 'store',
        'chado_table' => $base_table,
        'chado_column' => $base_column,
      ]),
    ];
  }
}