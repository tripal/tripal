<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalStringTypeFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Chado string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_string_type_formatter",
 *   label = @Translation("Chado String Type Formatter"),
 *   description = @Translation("The Chado string type formatter."),
 *   field_types = {
 *     "chado_string_type_default"
 *   }
 * )
 */
class ChadoStringFormatterDefault extends DefaultTripalStringTypeFormatter {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['field_prefix'] = '';
    $settings['field_suffix'] = '';
    return $settings;
  }

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $field_prefix = $this->getSetting('field_prefix');
    $field_suffix = $this->getSetting('field_suffix');

    foreach($items as $delta => $item) {
      $value = $item->get('value')->getString();
      $elements[$delta] = [
        "#markup" => $field_prefix . $value . $field_suffix,
      ];
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
                     . ' value. This may include HTML for formatting, <em>e.g.</em> for italics use &lt;em&gt;'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_prefix'),
      '#required' => FALSE,
    ];
    $form['field_suffix'] = [
      '#title' => $this->t('Text to display after the field value'),
      '#description' => $this->t('Enter text here that will be displayed after the'
                     . ' value. This may include HTML for formatting, <em>e.g.</em> for italics use &lt;/em&gt;'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_suffix'),
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
