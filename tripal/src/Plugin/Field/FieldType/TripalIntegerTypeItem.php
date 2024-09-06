<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of the 'integer' field type.
 *
 * @FieldType(
 *   id = "tripal_integer_type",
 *   category = "tripal",
 *   label = @Translation("Tripal Integer Field Type"),
 *   description = @Translation("An integer field."),
 *   default_widget = "default_tripal_integer_type_widget",
 *   default_formatter = "default_tripal_integer_type_formatter"
 * )
 */
class TripalIntegerTypeItem extends TripalFieldItemBase {

  public static $id = "tripal_integer_type";

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSettings();
    $termIdSpace = $storage_settings['termIdSpace'];
    $termAccession = $storage_settings['termAccession'];

    return [
      new IntStoragePropertyType($entity_type_id, self::$id, "value", $termIdSpace . ':' . $termAccession),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $values = [];

    $values['value'] = mt_rand(1, 100000);

    return $values;
  }
}
