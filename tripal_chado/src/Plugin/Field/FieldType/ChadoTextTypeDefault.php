<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoTextStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Ajax\ReplaceCommand;


/**
 * Plugin implementation of the 'text' field type for Chado.
 *
 * @FieldType(
 *   id = "chado_text_type_default",
 *   category = "tripal_chado",
 *   label = @Translation("Chado Text Field Type"),
 *   description = @Translation("A text field with no length limit."),
 *   default_widget = "chado_text_type_widget",
 *   default_formatter = "chado_text_type_formatter",
 *   cardinality = 1
 * )
 */
class ChadoTextTypeDefault extends ChadoFieldItemBase {

  public static $id = "chado_text_type_default";

  // This is a flag to the ChadoFieldItemBase parent
  // class to provide a column selector in the form
  protected static $select_base_column = TRUE;

  // Valid column types to pass to the ChadoFieldItemBase parent class.
  protected static $valid_base_column_types = ['text'];

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
        'path' => $base_table . '.' . $base_pkey_col,
        //'chado_table' => $base_table,
        //'chado_column' => $base_pkey_col
      ]),
      new TextStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'path' => $base_table . '.' . $base_column,
        //'chado_table' => $base_table,
        //'chado_column' => $base_column,
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
    if ($base_table) {
      $table_columns = $this->getTableColumns($base_table, self::$valid_base_column_types);
      if (count($table_columns) < 1) {
        $compatible = FALSE;
      }
    }
    return $compatible;
  }

}
