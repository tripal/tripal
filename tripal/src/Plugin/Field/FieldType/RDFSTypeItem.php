<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\tripal\Plugin\Field\TripalFieldItemBase;
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
 *   default_formatter = "rdfs__type_formatter",
 *   cardinality = 1
 * )
 */
class RDFSTypeItem extends TripalFieldItemBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultFieldSettings() {
    $settings = parent::defaultFieldSettings();

    // -- Define the Vocabulary.
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    $settings['term_vocabulary'] = 'rdfs';
    // The full name of the vocabulary.
    $settings['vocab_name'] = 'RDF Schema 1.1';
    // The description of the vocabulary.
    $settings['vocab_description'] = 'RDF Schema provides a data-modelling vocabulary for RDF data. RDF Schema is an extension of the basic RDF vocabulary.';

    // -- Define the Vocabulary Term.
    // The name of the term.
    $settings['term_name'] = 'type';
    // The unique ID (i.e. accession) of the term.
    $settings['term_accession'] = 'type';
    // The definition of the term.
    $settings['term_definition'] = 'rdf:type is an instance of rdf:Property that is used to state that a resource is an instance of a class.';

    // -- Additional Settings.
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    $settings['term_fixed'] = TRUE;
    // Set to TRUE if the field should be automatically attached to an entity
    // when it is loaded. Otherwise, the callee must attach the field
    // manually.  This is useful to prevent really large fields from slowing
    // down page loads.  However, if the content type display is set to
    // "Hide empty fields" then this has no effect as all fields must be
    // attached to determine which are empty.  It should always work with
    // web services.
    $settings['auto_attach'] = TRUE;

    return $settings;
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
