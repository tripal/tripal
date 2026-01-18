<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Datetime\DrupalDateTime;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal\TripalField\TripalFormatterBase;

/**
 * Plugin implementation of default Tripal datetime type formatter.
 */
#[TripalFieldFormatter(
  id: 'default_tripal_datetime_type_formatter',
  label: new TranslatableMarkup('Default Datetime Type Formatter'),
  description: new TranslatableMarkup('The default datetime type formatter.'),
  field_types: [
    'tripal_datetime_type',
  ],
)]
class DefaultTripalDatetimeTypeFormatter extends TripalFormatterBase {

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
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    $field_prefix = $this->getSetting('field_prefix');
    $field_suffix = $this->getSetting('field_suffix');
    $hide_condition = $this->getSetting('hide_condition') ?? '';
    $hide_value = $this->getSetting('hide_value') ?? '';

    // We need to get the filter format which is set in the widget settings
    // because widget and formatter must match.
    $entity_type = $this->fieldDefinition->get('entity_type');
    $bundle = $this->fieldDefinition->get('bundle');
    $form_display = \Drupal::service('entity_display.repository')->getFormDisplay($entity_type, $bundle);
    $field_name = $this->fieldDefinition->get('field_name');
    $widget = $form_display->getComponent($field_name);
    $filter_format = $widget['settings']['filter_format'] ?? 'basic_html';

    foreach($items as $delta => $item) {
      $value = $item->get('value')->getValue() ?? '';
      $hide = ((($hide_condition == 'if_value') and ($value == $hide_value))
            or (($hide_condition == '') and !strlen($value)));
      if (!$hide) {
        $elements[$delta] = [
          '#type' => 'processed_text',
          '#text' => $value,
          '#format' => $filter_format,
          '#langcode' => $item->getLangcode(),
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
