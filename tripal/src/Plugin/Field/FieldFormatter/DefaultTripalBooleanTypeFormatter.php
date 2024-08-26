<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal boolean type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_boolean_type_formatter",
 *   label = @Translation("Default Boolean Type Formatter"),
 *   description = @Translation("The default boolean type formatter."),
 *   field_types = {
 *     "tripal_boolean_type"
 *   }
 * )
 */
class DefaultTripalBooleanTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['true_string'] = t('True');
    $settings['false_string'] = t('False');
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $true_string = $this->getSetting('true_string');
    $false_string = $this->getSetting('false_string');
    $elements = [];

    foreach($items as $delta => $item) {
      $value = $item->get("value")->getValue();
      if (!is_null($value) and strlen($value)) {
        $elements[$delta] = [
          "#markup" => $value ? $true_string : $false_string,
        ];
      }
    }

    return $elements;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['true_string'] = [
      '#title' => $this->t('Text to display for a boolean TRUE value'),
      '#description' => $this->t('Enter text here to represent a boolean TRUE value.'
                     . ' For example "True", "Yes", "Present", "Active", etc.'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('true_string'),
      '#required' => TRUE,
      '#element_validate' => [[static::class, 'settingsFormValidateBoolean']],
    ];
    $form['false_string'] = [
      '#title' => $this->t('Text to display for a boolean FALSE value'),
      '#description' => $this->t('Enter text here to represent a boolean FALSE value.'
                     . ' For example "False", "No", "Absent", "Disabled", etc.'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('false_string'),
      '#required' => TRUE,
      '#element_validate' => [[static::class, 'settingsFormValidateBoolean']],
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $summary[] = $this->t('Set display format');
    return $summary;
  }

  /**
   * Form element validation handler for boolean strings
   *
   * @param array $form
   *   The form where the settings form is being included in.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state of the (entire) form.
   */
  public static function settingsFormValidateBoolean(array $form, FormStateInterface $form_state) {
    // This form state contains settings for all of the fields for the
    // current content type, we only validate our own field.
    $field_values = $form_state->getValue('fields');
    foreach ($field_values as $field => $field_settings) {
      // We are here at Tripal level, but this field may be at Chado level,
      // so just rely on the ending of the field type being the same.
      if (preg_match('/_boolean_type_formatter$/', $field_settings['type'])
          and (array_key_exists('settings_edit_form', $field_settings))) {
        $true_string = $field_settings['settings_edit_form']['settings']['true_string'];
        $false_string = $field_settings['settings_edit_form']['settings']['false_string'];
        // An empty textbox is prevented by the '#required' attribute.
        // Here we just need to validate that both values are not the same.
        if ($true_string == $false_string) {
          $form_state->setErrorByName('fields][' . $field . '][settings_edit_form][settings][false_string',
              t('Both values cannot be the same'));
        }
      }
    }
  }

}
