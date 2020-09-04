<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\Field\TripalFieldItemBase;
use Drupal\Component\Utility\Random;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemBase;
use Drupal\Core\Field\FieldStorageDefinitionInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\TypedData\DataDefinition;
use Drupal\tripal\Entity\TripalEntityType;

/**
 * Plugin implementation of the 'rdfs__type' field type.
 *
 * @FieldType(
 *   id = "rdfs__type",
 *   label = @Translation("Tripal Content Type"),
 *   description = @Translation("The type of Tripal content."),
 *   category = @Translation("Tripal: General"),
 *   default_widget = "rdfs__type_widget",
 *   default_formatter = "rdfs__type_formatter"
 * )
 */
class RDFSTypeItem extends TripalFieldItemBase {

  /**
   * {@inheritdoc}
   */
  public function applyDefaultValue($notify = TRUE) {

    // The default should be the label of the content type.
    // To do that we need to get the TripalEntity object,
    // then the TripalEntityType object... then finally we can grab the label.
    // This is why this value is cached...
    $entity = $this->getEntity();
    $entityType_id = $entity->getType();
    $entityType = TripalEntityType::load($entityType_id);

    $this->setValue([
      'value' => $entityType->getLabel(),
    ], $notify);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {
    // Retrieve the default storage settings.
    $settings = $field_definition->getSettings();

    // Define the content which will be stored in this field.
    // -- Tripal Content Type Human-Readable Name.
    $properties['value'] = DataDefinition::create('string')
      ->setLabel(new TranslatableMarkup('Content Type Name'))
      ->setDescription(new TranslatableMarkup('The human-reable name of the Tripal Content Type.'));

    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    return [
      'columns' => [
        'value' => [
          'type' => 'varchar',
          'length' => $field_definition->getSetting('max_length'),
        ],
      ],
    ];
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
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $random = new Random();
    $values['value'] = $random->word(mt_rand(1, 50));
    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraint_manager = \Drupal::typedDataManager()
      ->getValidationConstraintManager();
    $constraints = parent::getConstraints();
    if ($max_length = $this
      ->getSetting('max_length')) {
      $constraints[] = $constraint_manager
        ->create('ComplexData', [
        'value' => [
          'Length' => [
            'max' => $max_length,
            'maxMessage' => t('%name: the text may not be longer than @max characters.', [
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
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $element = [];
    $element['max_length'] = [
      '#type' => 'number',
      '#title' => t('Maximum length'),
      '#default_value' => $this
        ->getSetting('max_length'),
      '#required' => TRUE,
      '#description' => t('The maximum length of the field in characters.'),
      '#min' => 1,
      '#disabled' => $has_data,
    ];
    $element += parent::storageSettingsForm($form, $form_state, $has_data);
    return $element;
  }
}
