<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal_chado\TripalField\ChadoFieldItemBase;
use Drupal\tripal_chado\TripalStorage\ChadoIntStoragePropertyType;
use Drupal\tripal_chado\TripalStorage\ChadoDatetimeStoragePropertyType;

/**
 * Plugin implementation of the datetime field type for Chado.
 */
#[TripalFieldType(
  id: 'chado_datetime_type_default',
  category: 'tripal_chado',
  label: new TranslatableMarkup('Chado Datetime Field Type'),
  description: new TranslatableMarkup('A datetime field backed by a Chado timestamp column.'),
  default_widget: 'chado_datetime_type_widget',
  default_formatter: 'chado_datetime_type_formatter',
  cardinality: 1,
)]
class ChadoDatetimeTypeDefault extends ChadoFieldItemBase {

  public static $id = 'chado_datetime_type_default';

  /**
   * {@inheritdoc}
   */
  protected static $select_base_column = TRUE;

  /**
   * PostgreSQL column types this field can manage.
   *
   * {@inheritdoc}
   */
  protected static $valid_base_column_types = [
    'timestamp without time zone',
    'timestamp with time zone',
    'date',
  ];

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
    return [
      'record_id' => 0,
      'value' => date('Y-m-d H:i:s', mt_rand(0, time())),
    ];
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

    $chado = \Drupal::service('tripal_chado.database');
    $schema = $chado->schema();
    $base_pkey_col = self::getPrimaryKey($base_table, $schema);
    $base_column = $settings['base_column'];

    $value_term = self::getColumnTermId($base_table, $base_column, 'NCIT:C25164');

    return [
      new ChadoIntStoragePropertyType($entity_type_id, self::$id, 'record_id', self::$record_id_term, [
        'action' => 'store_id',
        'drupal_store' => TRUE,
        'path' => $base_table . '.' . $base_pkey_col,
      ]),
      new ChadoDatetimeStoragePropertyType($entity_type_id, self::$id, 'value', $value_term, [
        'action' => 'store',
        'path' => $base_table . '.' . $base_column,
      ]),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function isCompatible(TripalEntityType $entity_type): bool {
    $base_table = $entity_type->getThirdPartySetting('tripal', 'chado_base_table');
    return count($this->getTableColumns($base_table, self::$valid_base_column_types)) > 0;
  }

}
