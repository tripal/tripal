<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\TripalField\TripalFieldItemBase;
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;
use Drupal\tripal\TripalStorage\IntStoragePropertyType;
use Drupal\tripal\TripalStorage\StoragePropertyValue;
use Drupal\core\Form\FormStateInterface;
use Drupal\core\Field\FieldDefinitionInterface;

/**
 * Plugin implementation of Tripal string field type.
 *
 * @FieldType(
 *   id = "tripal_string_type",
 *   label = @Translation("Tripal String Field Type"),
 *   description = @Translation("A string field."),
 *   default_widget = "default_tripal_string_type_widget",
 *   default_formatter = "default_tripal_string_type_formatter"
 * )
 */
class TripalStringTypeItem extends TripalFieldItemBase {

  public static $id = "tripal_string_type";

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    return [] + parent::defaultFieldSettings();
  }

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    return [
      'max_length' => 255,
    ] + parent::defaultStorageSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $element = [];
    $element['max_length'] = [
      '#type' => 'number',
      '#title' => t('Maximum length'),
      '#default_value' => $this->getSetting('max_length'),
      '#required' => TRUE,
      '#description' => t('The maximum length of the field in characters.'),
      '#min' => 1,
      '#disabled' => $has_data,
    ];
    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $values = [];
    //$random = new Random();
    //$values['value'] = $random->word(mt_rand(1, $field_definition->getSetting('max_length')));
    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();
    if ($max_length = $this->getSetting('max_length')) {
      $constraint_manager = \Drupal::typedDataManager()->getValidationConstraintManager();
      $constraints[] = $constraint_manager->create('ComplexData', [
        'value' => [
          'Length' => [
            'max' => $max_length,
            'maxMessage' => t('%name: may not be longer than @max characters.', [
              '%name' => $this
              ->getFieldDefinition()
              ->getLabel(),
              '@max' => $max_length,
            ]),
          ],
        ],
      ]);
    }
    return $constraints;
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $max_length = $field_definition->getSetting('max_length');
    return [
      new VarCharStoragePropertyType($entity_type_id, self::$id, "value", $max_length),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function tripalValuesTemplate() {
    $entity = $this->getEntity();
    $entity_type_id = $entity->getEntityTypeId();
    return [
      new StoragePropertyValue($entity_type_id, self::$id, "value", $entity->id()),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function tripalLoad($properties, $entity) {
//     foreach ($properties as $property) {
//       if ($property->getKey() == "value") {
//         $entity->blah = $property->value();
//       }
//     }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalSave($properties,$entity) {
//     foreach ($properties as $property) {
//       if ($property->getKey() == "value") {
//         $property->setValue($entity->blah);
//       }
//     }
  }

  /**
   * {@inheritdoc}
   */
  public function tripalClear($entity) {
    //$entity->blah = "";
  }
}
