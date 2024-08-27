<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal integer type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_integer_type_formatter",
 *   label = @Translation("Default Integer Type Formatter"),
 *   description = @Translation("The default integer type formatter."),
 *   field_types = {
 *     "tripal_integer_type"
 *   }
 * )
 */
class DefaultTripalIntegerTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['field_prefix'] = '';
    $settings['field_suffix'] = '';
    $settings['thousand_separator'] = '';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $field_prefix = $this->getSetting('field_prefix');
    $field_suffix = $this->getSetting('field_suffix');
    $thousand_separator = $this->getSetting('thousand_separator');

    foreach($items as $delta => $item) {
      $value = $item->get("value")->getValue();
      if (!is_null($value) and strlen($value)) {
        if (strlen($thousand_separator)) {
          // For an integer we can hardcode the unused decimal setting
          $value = number_format(floatval($value), 0, '.', $thousand_separator);
        }
        $elements[$delta] = [
          "#markup" => $field_prefix . $value . $field_suffix,
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

    $form['field_prefix'] = [
      '#title' => $this->t('Text to display before the field value'),
      '#description' => $this->t('Enter text here that will be displayed before the'
                     . ' field value, or leave blank for no additional text'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_prefix'),
      '#required' => FALSE,
    ];
    $form['field_suffix'] = [
      '#title' => $this->t('Text to display after the field value'),
      '#description' => $this->t('Enter text here that will be displayed after the'
                     . ' field value, or leave blank for no additional text'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_suffix'),
      '#required' => FALSE,
    ];
    $form['thousand_separator'] = [
      '#title' => $this->t('Thousand Separator'),
      '#description' => $this->t('Character to display every three digits'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('thousand_separator'),
      '#required' => FALSE,
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

}
