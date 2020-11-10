<?php

namespace Drupal\tripal_chado\Plugin\Field;

use Drupal\tripal\Plugin\Field\TripalFieldItemBase;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\TypedData\DataDefinitionInterface;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\Core\TypedData\Plugin\DataType\Map;
use Drupal\Core\TypedData\TypedDataInterface;


/**
 * A Chado-based entity field item.
 *
 * Entity field items making use of this base class have to implement
 * the static method propertyDefinitions().
 *
 */
abstract class ChadoFieldItemBase extends TripalFieldItemBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultStorageSettings() {
    $settings = parent::defaultStorageSettings();
    $settings['max_length'] = 255;
    $settings['tripal_custom_storage'] = 'chado';

    // -- Chado Table.
    // The table in Chado that the field maps to.
    $settings['chado_table'] = '';
    // The column of the table in Chado where the value comes from.
    $settings['chado_column'] = '';
    // The base table.
    $settings['base_table'] = '';

    return $settings;
  }

  /**
  * Selects the record from chado and formats it for the field.
  *
  * @param int $record_id
  *   The chado record_id of the record to lookup.
  * @return array
  *   Returns an array with values matching the field definition.
  */
  public function selectChadoValue($record_id, $item) {
    return NULL;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    $settings = ChadoFieldItemBase::defaultStorageSettings();

    $schema = [
      'columns' => [
        'value' => [
          'type' => 'text',
        ],
        'chado_schema' => [
          'type' => 'varchar',
          'description' => 'The name of the chado schema this record resides in.',
          'length' => $settings['max_length'],
        ],
        'record_id' => [
          'type' => 'int',
          'description' => 'The primary key of this record in the chado.',
          'size' => 'big',
        ],
      ],
    ];

    return $schema;
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];

    $elements['max_length'] = [
      '#type' => 'number',
      '#title' => t('Maximum length for the chado schema name'),
      '#default_value' => $this->getSetting('max_length'),
      '#required' => TRUE,
      '#description' => t('The maximum length of the field in characters.'),
      '#min' => 1,
      '#disabled' => TRUE,
    ];

    $elements['chado_table'] = [
      '#type' => 'textfield',
      '#title' => t('Chado Table'),
      '#description' => t('The chado table data for this field is stored in.'),
      '#required' => TRUE,
      '#default_value' => $this->getSetting('chado_table'),
      '#disabled' => TRUE,
    ];

    $elements['chado_column'] = [
      '#type' => 'textfield',
      '#title' => t('Chado Column'),
      '#description' => t('The chado table column that data for this field is stored in.'),
      '#required' => TRUE,
      '#default_value' => $this->getSetting('chado_column'),
      '#disabled' => TRUE,
    ];

    return $elements;
  }

  /**
   * @{inheritdoc}
   */
  public function getValue() {
    return $this->value;
  }

  /**
   * @{inheritdoc}
   */
  public function setValue($values, $notify = TRUE) {
    parent::setValue($values, $notify);
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    $value = $this->get('value')->getValue();
    return is_int($value) && $value > 0;
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();

    $constraint_manager = \Drupal::typedDataManager()->getValidationConstraintManager();
    $constraints[] = $constraint_manager->create('ComplexData', [
      'chado_schema' => [
        'Length' => [
          'max' => 255,
          'maxMessage' => t('%name: The name of the chado schema this record is associated with may not be longer than @max characters.', [
            '%name' => $this->getFieldDefinition()->getLabel(),
            '@max' => 255
          ]),
        ],
      ],
    ]);

    return $constraints;
  }
}
