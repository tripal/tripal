<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;


/**
 * Plugin implementation of the 'integer' field type for Chado.
 *
 * @FieldType(
 *   id = "chado_integer_type_default",
 *   label = @Translation("Chado Integer Field Type"),
 *   description = @Translation("An integer field."),
 *   default_widget = "chado_integer_type_widget",
 *   default_formatter = "chado_integer_type_formatter",
 *   select_base_column = TRUE,
 *   valid_base_column_types = {
 *     "smallint",
 *     "integer",
 *     "bigint",
 *     "serial",
 *   },
 *   cardinality = 1
 * )
 */
class ChadoIntegerTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_integer_type_default";

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

    // If we don't have a base table then we're not ready to specify the
    // properties for this field.
    if (!$base_table) {
      return;
    }

    // Get the base table columns needed for this field.
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_column = $settings['base_column'];
    $base_pkey_col = $base_schema_def['primary key'];

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $record_id_term = 'SIO:000729';
    $value_term = $mapping->getColumnTermId($base_table, $base_column);

    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', $record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'chado_table' => $base_table,
        'chado_column' => $base_pkey_col
      ]),
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'chado_table' => $base_table,
        'chado_column' => $base_column,
      ]),
    ];
  }

}
