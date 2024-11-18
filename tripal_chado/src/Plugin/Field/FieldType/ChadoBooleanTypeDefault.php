<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType;


/**
 * Plugin implementation of the 'boolean' field type for Chado.
 *
 * @FieldType(
 *   id = "chado_boolean_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Boolean Field Type"),
 *   description = @Translation("A boolean field."),
 *   default_widget = "chado_boolean_type_widget",
 *   default_formatter = "chado_boolean_type_formatter",
 *   cardinality = 1
 * )
 */
class ChadoBooleanTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_boolean_type_default";

  // This is a flag to the ChadoFieldItemBase parent
  // class to provide a column selector in the form
  protected static $select_base_column = TRUE;

  // Valid column types to pass to the ChadoFieldItemBase parent class.
  protected static $valid_base_column_types = ['boolean'];

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
    if (!$base_table) {
      return;
    }

    // Get the base table columns needed for this field.
    $base_column = $settings['base_column'];
    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_schema_def = $schema->getTableDef($base_table, ['format' => 'Drupal']);
    $base_pkey_col = $base_schema_def['primary key'];

    // Get the property terms by using the Chado table columns they map to.
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');
    $value_term = $mapping->getColumnTermId($base_table, $base_column) ?: 'NCIT:C25712';

    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $base_pkey_col,
      ]),
      new ChadoBoolStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'path' => $base_table . '.' . $base_column,
      ]),
    ];
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\TripalField\ChadoFieldItemBase::isCompatible()
   */
  public function isCompatible(TripalEntityType $entity_type) : bool {
    $compatible = TRUE;

    // Get the base table for the content type.
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    $table_columns = $this->getTableColumns($base_table, self::$valid_base_column_types);
    if (count($table_columns) < 1) {
      $compatible = FALSE;
    }
    return $compatible;
  }

}
