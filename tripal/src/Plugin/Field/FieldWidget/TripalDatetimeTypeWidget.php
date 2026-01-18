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
class TripalDatetimeTextTypeWidget extends TripalWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element + [
      '#type' => 'textfield',
      '#default_value' => $items[$delta]->value ?? '',
      '#placeholder' => $this->getSetting('placeholder'),
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
    ];
    return $element;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {

    // Handle any empty values. We can't pass an empty string when a date is expected.
    foreach ($values as $val_key => $value) {
      if ($value['value'] == '') {
        unset($values[$val_key]);
      }
    }
    return $values;
  }
}
