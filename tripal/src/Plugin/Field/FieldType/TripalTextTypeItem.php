<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of the 'text' field type.
 *
 * @FieldType(
 *   id = "tripal_text_type",
 *   category = "tripal",
 *   label = @Translation("Tripal Text Field Type"),
 *   description = @Translation("A text field with no length limit."),
 *   default_widget = "default_tripal_text_type_widget",
 *   default_formatter = "default_tripal_text_type_formatter"
 * )
 */
class TripalTextTypeItem extends TripalFieldItemBase {

  public static $id = "tripal_text_type";

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSettings();
    $termIdSpace = $storage_settings['termIdSpace'];
    $termAccession = $storage_settings['termAccession'];

    return [
      new TextStoragePropertyType($entity_type_id, self::$id, "value", $termIdSpace . ':' . $termAccession),
    ];
  }

}
