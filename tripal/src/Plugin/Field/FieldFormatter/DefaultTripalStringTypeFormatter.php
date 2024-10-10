<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_string_type_formatter",
 *   label = @Translation("Default String Type Formatter"),
 *   description = @Translation("The default string type formatter."),
 *   field_types = {
 *     "tripal_string_type"
 *   }
 * )
 */
class DefaultTripalStringTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['field_prefix'] = '';
    $settings['field_suffix'] = '';
    $settings['hide_condition'] = '';
    $settings['hide_value'] = '';
    return $settings;
  }

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $field_prefix = $this->getSetting('field_prefix');
    $field_suffix = $this->getSetting('field_suffix');
    $hide_condition = $this->getSetting('hide_condition') ?? '';
    $hide_value = $this->getSetting('hide_value') ?? '';

    foreach($items as $delta => $item) {
      $value = $item->get('value')->getString() ?? '';
      $hide = ((($hide_condition == 'if_value') and ($value == $hide_value))
            or (($hide_condition == '') and !strlen($value)));
      if (!$hide) {
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
    $form['hide_condition'] = [
      '#title' => $this->t('You may provide a condition when the field is not displayed'),
      '#type' => 'radios',
      '#options' => [
        '' => $this->t('Hide if empty'),
        'never' => $this->t('Never hide'),
        'if_value' => $this->t('Hide if empty or equal to a specific value'),
      ],
      '#default_value' => $this->getSetting('hide_condition') ?? '',
    ];
    $form['hide_value'] = [
      '#title' => $this->t('Specific value to be hidden'),
      '#description' => $this->t('A value that you do not want displayed, e.g. "N/A" for a string, or "0" for a number'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('hide_value') ?? '',
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
