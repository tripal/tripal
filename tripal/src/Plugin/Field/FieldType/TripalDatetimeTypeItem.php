<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\DatetimeStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of the 'datetime' field type.
 *
 * @FieldType(
 *   id = "tripal_datetime_type",
 *   category = "tripal",
 *   label = @Translation("Tripal Datetime Field Type"),
 *   description = @Translation("A datetime field."),
 *   default_widget = "default_tripal_datetime_type_widget",
 *   default_formatter = "default_tripal_datetime_type_formatter"
 * )
 */
class TripalDatetimeTypeItem extends TripalFieldItemBase {

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

    // Use a default term if one is not set.
    $term = 'NCIT:C25164'; // 'NCIT:Date'
    if ($termIdSpace) {
      $term = $termIdSpace . ':' . $termAccession;
    }

    return [
      new DatetimeStoragePropertyType($entity_type_id, self::$id, "value", $term),
    ];
  }

}
