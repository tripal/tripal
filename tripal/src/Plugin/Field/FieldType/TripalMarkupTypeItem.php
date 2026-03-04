<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\Component\Utility\Random;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
// Make sure to include the Property type class you are going to create
// in your addTypes() method below.
use Drupal\tripal\TripalStorage\VarCharStoragePropertyType;

/**
 * Plugin implementation of the 'Tripal Markup' field type.
 */
#[TripalFieldType(
  id: 'tripal_markup',
  category: 'tripal',
  label: new TranslatableMarkup('Tripal Markup'),
  description: new TranslatableMarkup('Adds static markup (e.g. instructions) to the form or view display.'),
  default_widget: 'tripal_markup_widget',
  default_formatter: 'tripal_markup_formatter',
)]
class TripalMarkupTypeItem extends TripalFieldItemBase {

  /**
   * The unique identifier of this field.
   *
   * NOTE: must match the id in the Attribute.
   *
   * @var string
   */
  public static string $id = "tripal_markup";

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
      // @example 'max_length' => 255,
    ];
    return $settings + parent::defaultStorageSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function storageSettingsForm(array &$form, FormStateInterface $form_state, $has_data) {
    $elements = [];

    /**
     * Provides a form element to set the max_length setting.
     * @code
     * $elements['max_length'] = [
     *   '#type' => 'number',
     *   '#title' => t('Maximum length'),
     *   '#default_value' => $this->getSetting('max_length'),
     *   '#required' => TRUE,
     *   '#description' => t('The maximum length of the field in characters.'),
     *   '#min' => 1,
     *   '#disabled' => $has_data,
     * ];
     * @endcode
     */

    return $elements + parent::storageSettingsForm($form, $form_state, $has_data);
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {

    // Use the field settings to extract the information we need to make
    // generic fields.
    $entity_type_id = $field_definition->getTargetEntityTypeId();
    $storage_settings = $field_definition->getSettings();
    $termIdSpace = 'schema';
    $termAccession = 'WebPageElement';

    // Here we set the max length to the one configured in the settings form.
    $max_length = $storage_settings['max_length'];

    return [
      // Example Property Type for strings.
      new VarCharStoragePropertyType($entity_type_id, self::$id, "value", $termIdSpace . ':' . $termAccession, $max_length),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $values = [];

    // Use something like the following to retrieve storage settings.
    // $max_length = $field_definition->getSetting('max_length')
    $max_length = 100;

    // Generate a random value to use as a sample.
    $random = new Random();
    $values['value'] = $random->word(mt_rand(1, $max_length));

    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public function getConstraints() {
    $constraints = parent::getConstraints();

    /**
     * Ensure that the value entered is not larger then the max length.
     * @code
     * if ($max_length = $this->getSetting('max_length')) {
     *   $constraint_manager = \Drupal::typedDataManager()->getValidationConstraintManager();
     *   $constraints[] = $constraint_manager->create('ComplexData', [
     *     'value' => [
     *       'Length' => [
     *         'max' => $max_length,
     *         'maxMessage' => t('%name: may not be longer than @max characters.', [
     *           '%name' => $this
     *           ->getFieldDefinition()
     *           ->getLabel(),
     *           '@max' => $max_length,
     *         ]),
     *       ],
     *     ],
     *   ]);
     * }
     * @endcode
     */

    return $constraints;
  }

}
