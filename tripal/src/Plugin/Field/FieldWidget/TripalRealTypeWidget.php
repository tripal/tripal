<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;

/**
 * Plugin implementation of default Tripal real type widget.
 */
#[TripalFieldWidget(
  id: 'default_tripal_real_type_widget',
  label: new TranslatableMarkup('Tripal Real Widget'),
  description: new TranslatableMarkup('The default real type widget.'),
  field_types: [
    'tripal_real_type',
  ],
)]
class TripalRealTypeWidget extends TripalWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element + [
      '#type' => 'textfield',
      '#default_value' => $items[$delta]->value ?? '',
      '#placeholder' => $this->getSetting('placeholder'),
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
      '#element_validate' => [[$this, 'validateRealValue']],
    ];
    return $element;
  }

  /**
   * Form element validation: ensures the entered value is a proper real number.
   */
  public function validateRealValue(array &$element, FormStateInterface $form_state): void {
    $value = trim($element['#value'] ?? '');
    if ($value === '') {
      return;
    }
    // Special infinity values, they can be abbreviated.
    if (preg_match('/^[+-]inf|[+-]infinity$/i', $value)) {
      return;
    }
    // Remove thousands separator, same as done in massage.
    $value = $this->removeThousandSeparators($value);

    // Perform validation with native php function.
    if (filter_var($value, FILTER_VALIDATE_FLOAT) === FALSE) {
      $form_state->setError(
        $element,
        $this->t(
          '"@value" is not a valid real number.',
          ['@value' => $value]
        )
      );
    }
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Handle any empty values. We can't pass an empty string when a
    // real number is expected.
    foreach (array_keys($values) as $val_key) {
      // Remove any thousand separator characters.
      $values[$val_key]['value'] = $this->removeThousandSeparators($values[$val_key]['value']);
      // Remove empty values.
      if (trim($values[$val_key]['value']) == '') {
        unset($values[$val_key]);
      }
    }
    return $values;
  }

  /**
   * Removes any thousand separator characters.
   *
   * @param string $value
   *   The string value containing a real number to process.
   *
   * @return string
   *   The value with any thousand separators characters removed.
   */
  protected function removeThousandSeparators(string $value): string {
    // Thousand separator is hardcoded until we find a need to specify locale.
    // To do so, we would need to include the php intl module in our docker.
    $thousand_separator = ',';
    $value = preg_replace('/' . $thousand_separator . '/', '', $value);
    return $value;
  }

}
