<?php

namespace Drupal\tripal_test\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\Entity\TripalEntityType;
use Drupal\tripal\TripalStorage\TextStoragePropertyType;


/**
 * Plugin implementation of the 'boolean' field type.
 *
 * @FieldType(
 *   id = "tripal_test_text_type",
 *   category = "tripal",
 *   label = @Translation("Tripal Test Type"),
 *   label = @Translation("Tripal Test Text Field Type"),
 *   description = @Translation("A test text field with no length limit."),
 *   default_widget = "default_tripal_text_type_widget",
 *   default_formatter = "default_tripal_text_type_formatter"
 * )
 */
class TripalTestTextTypeItem extends TripalFieldItemBase {

  public static $id = "tripal_test_text_type";

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

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal\TripalField\Interfaces\TripalFieldItemInterface::discover()
   */
  public static function discover(TripalEntityType $bundle, string $field_id, array $field_definitions) : array {

    $base_field = [
      'name' => self::generateFieldName($bundle, 'test_field', 0),
      'content_type' => $bundle->getID(),
      'label' => 'Test',
      'type' => self::$id,
      'description' => 'A test field',
      'cardinality' => 1,
      'required' => TRUE,
      'storage_settings' => [
        'storage_plugin_id' => 'drupal_sql_storage',
        'storage_plugin_settings' => [],
        'max_length' => 255,
      ],
      'settings' => [
        'termIdSpace' => 'OBI',
        'termAccession' => '0100026'
      ],
      'display' => [
        'view' => [
          'default' => [
            'region' => 'content',
            'label' => 'above',
            'weight' => 10,
          ],
        ],
        'form' => [
          'default' => [
            'region' => 'content',
            'weight' => 10
          ],
        ],
      ],
    ];

    // Initialize with an empty field list.
    $field_list = [];

    // Create a valid field.
    $field_list[] = $base_field;

    // The same field but with a long name including spaces and unicode that
    // will be truncated to 32 characters: 'organism__test_field_but_with__1'
    // cvterm_id is passed and should appear at the end of the field name
    $field_2 = $base_field;
    $field_2['name'] = self::generateFieldName($bundle, '🙈test field_but with_a very_very long_name', 1);
    $field_list[] = $field_2;

    // The same except cvterm_id is not passed, a random unique id should be appended
    $field_3 = $base_field;
    $field_3['name'] = self::generateFieldName($bundle, '🙈test field_but with_a very_very long_name');
    $field_list[] = $field_3;

    // Create an invalid field.
    $field_4 = $base_field;
    $field_4['name'] = self::generateFieldName($bundle, 'test_field4', 0);
    $field_4['type'] = 'this_type_does_not_exist';
    $field_list[] = $field_4;

    return $field_list;
  }
}
