<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\Plugin\Field\FieldWidget\TripalStringTypeWidget;

/**
 * Plugin implementation of default Chado string type widget.
 */
#[TripalFieldWidget(
  id: 'chado_string_type_widget',
  label: new TranslatableMarkup('Chado String Widget'),
  description: new TranslatableMarkup('The default string type widget.'),
  field_types: [
    'chado_string_type_default',
  ],
)]
class ChadoStringWidgetDefault extends TripalStringTypeWidget {

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
