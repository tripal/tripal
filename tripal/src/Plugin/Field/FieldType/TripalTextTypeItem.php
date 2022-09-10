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
 *   label = @Translation("Tripal Text Field Type"),
 *   description = @Translation("A text field."),
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
    $settings = $field_definition->getSetting('storage_plugin_settings');
    $value_settings = $settings['property_settings']['value'];
    $types = [
      new TextStoragePropertyType($entity_type_id, self::$id, "value", $value_settings),
    ];
    $default_types = TripalFieldItemBase::defaultTripalTypes($entity_type_id, self::$id);
    $types = array_merge($types, $default_types);
    return $types;
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate() {
    $entity = $this->getEntity();
    $entity_type_id = $entity->getEntityTypeId();
    $entity_id = $entity->id();
    $values = [
      new StoragePropertyValue($entity_type_id, self::$id, "value", $entity_id),
    ];
    $default_values = TripalFieldItemBase::defaultTripalValuesTemplate($entity_type_id, self::$id, $entity_id);
    $values = array_merge($values, $default_values);
    return $values;
  }
}