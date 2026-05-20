<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\tripal\Plugin\Field\FieldWidget\TripalDatetimeTypeWidget;

/**
 * Plugin implementation of the default Chado datetime type widget.
 */
#[TripalFieldWidget(
  id: 'chado_datetime_type_widget',
  label: new TranslatableMarkup('Chado Datetime Widget'),
  description: new TranslatableMarkup('The default datetime type widget for Chado.'),
  field_types: [
    'chado_datetime_type_default',
  ],
)]
class ChadoDatetimeWidgetDefault extends TripalDatetimeTypeWidget {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $item_vals = $items[$delta]->getValue();
    $element = parent::formElement($items, $delta, $element, $form, $form_state);
    $element['record_id'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['record_id'] ?? 0,
    ];
    return $element;
  }

}
