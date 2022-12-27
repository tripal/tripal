<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;


/**
 * Plugin implementation of the 'integer' field type for Chado.
 *
 * @FieldType(
 *   id = "chado_integer_type",
 *   label = @Translation("Chado Integer Field Type"),
 *   description = @Translation("An integer field."),
 *   default_widget = "chado_integer_type_widget",
 *   default_formatter = "chado_integer_type_formatter"
 * )
 */
class ChadoIntegerTypeItem extends ChadoFieldItemBase {

  public static $id = "chado_integer_type";

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
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, "value", [
        'action' => 'store',
        'chado_table' => $base_table,
        'chado_column' => $base_column,
      ]),
    ];
  }
}