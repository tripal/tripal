<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\DatetimeStoragePropertyType;

/**
 * Plugin implementation of the datetime field type.
 */
#[TripalFieldType(
  id: 'tripal_datetime_type',
  category: 'tripal',
  label: new TranslatableMarkup('Tripal Datetime Field Type'),
  description: new TranslatableMarkup('A datetime field.'),
  default_widget: 'default_tripal_datetime_type_widget',
  default_formatter: 'default_tripal_datetime_type_formatter',
)]
class TripalDatetimeTypeItem extends TripalFieldItemBase {

  /**
   * The id for this field. Must match the attribute value.
   *
   * @var string
   */
  public static $id = "tripal_datetime_type";

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = [];
    return $settings + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = [
      'storage_plugin_id' => 'drupal_sql_storage',
    ];
    return $settings + parent::defaultStorageSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSettings();
    $termIdSpace = $storage_settings['termIdSpace'];
    $termAccession = $storage_settings['termAccession'];

    // Use a default term if one is not set, i.e. 'NCIT:Date'.
    $term = 'NCIT:C25164';
    if ($termIdSpace) {
      $term = $termIdSpace . ':' . $termAccession;
    }

    return [
      new DatetimeStoragePropertyType($entity_type_id, self::$id, "value", $term),
    ];
  }

}
