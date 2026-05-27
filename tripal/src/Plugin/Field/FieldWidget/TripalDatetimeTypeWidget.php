<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;

/**
 * Plugin implementation of default Tripal datetime type widget.
 */
#[TripalFieldWidget(
  id: 'default_tripal_datetime_type_widget',
  label: new TranslatableMarkup('Tripal Datetime Widget'),
  description: new TranslatableMarkup('The default datetime type widget.'),
  field_types: [
    'tripal_datetime_type',
  ],
)]
class TripalDatetimeTypeWidget extends TripalWidgetBase {

  /**
   * Accepted input formats, tried in order during validation and massage.
   *
   * The first entry that parses successfully wins. Formats with more tokens
   * are listed first so they take precedence over shorter variants.
   */
  private const ACCEPTED_FORMATS = [
    '!Y-m-d H:i:s.u',  // 2025-01-15 10:30:00.123456
    '!Y-m-d H:i:s',    // 2025-01-15 10:30:00
    '!Y-m-d\TH:i:s',   // 2025-01-15T10:30:00  (ISO 8601)
    '!Y-m-d H:i',      // 2025-01-15 10:30     → seconds default to 00
    '!Y-m-d\TH:i',     // 2025-01-15T10:30
    '!Y-m-d',          // 2025-01-15            → time defaults to 00:00:00
  ];

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element + [
      '#type' => 'textfield',
      '#default_value' => $items[$delta]->value ?? '',
      '#placeholder' => 'YYYY-MM-DD HH:MM:SS',
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
      '#element_validate' => [[$this, 'validateDatetimeValue']],
    ];
    $element['value']['#description'] = $this->t(
      'Enter a date and time using the format <code>YYYY-MM-DD HH:MM:SS</code> '
      . '(e.g., <code>2025-01-15 10:30:00</code>). '
      . 'The time portion is optional and defaults to <code>00:00:00</code> when omitted. '
      . 'Sub-second precision is also accepted (e.g., <code>2025-01-15 10:30:00.123456</code>).'
    );
    return $element;
  }

  /**
   * Form element validation: ensures the entered value is a proper datetime.
   */
  public function validateDatetimeValue(array &$element, FormStateInterface $form_state): void {
    $value = trim($element['#value'] ?? '');
    if ($value === '') {
      return;
    }
    if ($this->parseDateTime($value) === NULL) {
      $form_state->setError(
        $element,
        $this->t(
          '"@value" is not a valid date/time. Use the format YYYY-MM-DD HH:MM:SS (e.g., 2025-01-15 10:30:00).',
          ['@value' => $value]
        )
      );
    }
  }

  /**
   * {@inheritdoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
    foreach ($values as $delta => $value) {
      $raw = trim($value['value'] ?? '');
      if ($raw === '') {
        unset($values[$delta]);
        continue;
      }
      $dt = $this->parseDateTime($raw);
      if ($dt !== NULL) {
        // Preserve sub-second precision only when the user actually provided it.
        if (str_contains($raw, '.')) {
          $values[$delta]['value'] = $dt->format('Y-m-d H:i:s.u');
        }
        else {
          $values[$delta]['value'] = $dt->format('Y-m-d H:i:s');
        }
      }
    }
    return $values;
  }

  /**
   * Tries to parse a datetime string using the accepted format list.
   *
   * @return \DateTime|null
   *   A DateTime object on success, or NULL if no format matched.
   */
  private function parseDateTime(string $value): ?\DateTime {
    foreach (self::ACCEPTED_FORMATS as $format) {
      $dt = \DateTime::createFromFormat($format, $value);
      if ($dt !== FALSE) {
        $errors = \DateTime::getLastErrors();
        if (empty($errors['warnings']) && empty($errors['errors'])) {
          return $dt;
        }
      }
    }
    return NULL;
  }

}
