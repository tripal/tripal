<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoBoolStoragePropertyType;

/**
 * Plugin implementation of the 'boolean' field type for Chado.
 */
#[TripalFieldType(
  id: 'chado_boolean_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Boolean Field Type'),
  description: new TranslatableMarkup('A boolean field.'),
  default_widget: 'chado_boolean_type_widget',
  default_formatter: 'chado_boolean_type_formatter',
  cardinality: 1,
)]
class ChadoBooleanTypeDefault extends ChadoFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = "chado_boolean_type_default";

  /**
   * Indicate if we should provide a column selector in the add field form.
   *
   * @var bool
   *   If TRUE then provide the select element for the column; if FALSE don't.
   * @see ChadoFieldItemBase
   */
  protected static $select_base_column = TRUE;

  /**
   * Indicate the types of base table columns this field can manage.
   *
   * @var array
   *   Simple list of valid base column types for this field to manage.
   * @see ChadoFieldItemBase
   */
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
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $value = [];

    $value['record_id'] = 0;
    $value['value'] = FALSE;

    return [$value];
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
    $base_pkey_col = self::getPrimaryKey($base_table, $schema);
    $base_column = $settings['base_column'];

    // Get the property terms by using the Chado table columns they map to.
    $value_term = self::getColumnTermId($base_table, $base_column, 'NCIT:C25712');

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
   *
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
