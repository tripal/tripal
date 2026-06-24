<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
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
    $settings['date_format'] = 'Y-m-d H:i:s';
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
    $date_format = $this->getSetting('date_format') ?: 'Y-m-d H:i:s';

    foreach ($items as $delta => $item) {
      $raw = $item->get('value')->getValue() ?? '';
      if ($raw === '-infinity' || $raw === '0') {
        continue;
      }
      $hide = ((($hide_condition == 'if_value') and ($raw == $hide_value))
            or (($hide_condition == '') and !strlen($raw)));
      if (!$hide) {
        $display = $this->formatTimestamp($raw, $date_format);
        $elements[$delta] = [
          '#markup' => $field_prefix . $display . $field_suffix,
        ];
      }
    }

    return $elements;
  }

  /**
   * Parses a raw PostgreSQL timestamp string and formats it.
   *
   * Handles both full-precision ('Y-m-d H:i:s.u') and second-precision
   * ('Y-m-d H:i:s') variants that Chado/PostgreSQL returns. Falls back to
   * returning the raw string if parsing fails.
   */
  protected function formatTimestamp(string $raw, string $format): string {
    if (!strlen($raw)) {
      return $raw;
    }
    // Try microsecond precision first, then second precision.
    $dt = \DateTime::createFromFormat('Y-m-d H:i:s.u', $raw)
       ?: \DateTime::createFromFormat('Y-m-d H:i:s', $raw);
    if ($dt === FALSE) {
      // Last resort: let PHP's generic parser try.
      try {
        $dt = new \DateTime($raw);
      }
      catch (\Exception $e) {
        return $raw;
      }
    }
    return $dt->format($format);
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $form = parent::settingsForm($form, $form_state);

    $form['date_format'] = [
      '#title' => $this->t('Date/time display format'),
      '#description' => $this->t(
        'PHP date format string used to display the timestamp. Common examples: <code>Y-m-d H:i:s</code> (2025-01-15 10:30:00), <code>F j, Y g:i A</code> (January 15, 2025 10:30 AM), <code>Y-m-d</code> (date only). See the <a href="https://www.php.net/manual/en/function.date.php" target="_blank">PHP date() documentation</a> for all tokens.'
      ),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('date_format'),
      '#required' => TRUE,
    ];
    $form['field_prefix'] = [
      '#title' => $this->t('Text to display before the field value'),
      '#description' => $this->t('Enter text here that will be displayed before the field value, or leave blank for no additional text'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('field_prefix'),
      '#required' => FALSE,
    ];
    $form['field_suffix'] = [
      '#title' => $this->t('Text to display after the field value'),
      '#description' => $this->t('Enter text here that will be displayed after the field value, or leave blank for no additional text'),
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
      '#default_value' => $this->getSetting('hide_condition') ?? 'if_value',
    ];
    $form['hide_value'] = [
      '#title' => $this->t('Specific value to be hidden'),
      '#description' => $this->t('A value that you do not want displayed, e.g. "N/A" for a string, or "0" for a number'),
      '#type' => 'textfield',
      '#default_value' => $this->getSetting('hide_value') ?? '-infinity',
      '#required' => FALSE,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $summary = parent::settingsSummary();
    $format = $this->getSetting('date_format') ?: 'Y-m-d H:i:s';
    $example = (new \DateTime('2025-01-15 10:30:00'))->format($format);
    $summary[] = $this->t('Date format: @format (e.g. @example)', [
      '@format' => $format,
      '@example' => $example,
    ]);
    return $summary;
  }

}
