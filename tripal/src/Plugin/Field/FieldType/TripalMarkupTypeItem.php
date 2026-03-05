<?php

namespace Drupal\tripal\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\Entity\TripalEntity;
use Drupal\tripal\TripalField\Attribute\TripalFieldType;
use Drupal\tripal\TripalField\TripalFieldItemBase;
// Make sure to include the Property type class you are going to create
// in your addTypes() method below.
use Drupal\tripal\TripalStorage\BoolStoragePropertyType;

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
  cardinality: 1,
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
    $settings = parent::defaultFieldSettings();

    $settings['termIdSpace'] = 'schema';
    $settings['termAccession'] = 'WebPageElement';

    $settings['markup'] = [
      'value'  => '',
      'format' => '',
    ];

    return $settings;
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
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    $elements = [];
    $settings = $this->getSettings();

    $elements['markup'] = [
      '#type' => 'text_format',
      '#title' => $this->t('Markup'),
      '#default_value' => $settings['markup']['value'] ?? '',
      '#format' => !empty($settings['markup']['format']) ? $settings['markup']['format'] : filter_default_format(),
      '#required' => TRUE,
      '#rows' => 15,
    ];

    return $elements + parent::fieldSettingsForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public static function tripalTypes($field_definition) {
    $entity_type_id = $field_definition->getTargetEntityTypeId();

    return [
      new BoolStoragePropertyType($entity_type_id, self::$id, 'has_value', 'owl:hasValue', []),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function generateSampleValue(FieldDefinitionInterface $field_definition) {
    $values = [];

    $values['has_value'] = TRUE;

    return $values;
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    $value = $this->getFieldDefinition()->getSetting('markup')['value'];
    return $value === NULL || $value === '';
  }

  /**
   * Retrieves the markup value for this field saved in the field settings.
   *
   * @param \Drupal\tripal\Entity\TripalEntity $entity
   *   The entity this field is attached to. Not used in this implementation
   *   but provided for context.
   * @param \Drupal\Core\Field\FieldDefinitionInterface $field_definition
   *   The field definition object, which contains the field settings where the
   *   markup value is stored.
   *
   * @return array
   *   The value for this field, which is the markup string from the field
   *   settings. This will be used by the formatter to display the markup in
   *   views and by the widget to display it on the form.
   */
  public static function getMarkupValue(TripalEntity $entity, FieldDefinitionInterface $field_definition) {
    $markup_settings = $field_definition->getSetting('markup');

    return [
      'value' => $markup_settings['value'],
      'format' => $markup_settings['format'] ?? filter_default_format(),
    ];
  }

}
