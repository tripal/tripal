<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalStringTypeWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado string type widget.
 *
 * @FieldWidget(
 *   id = "chado_string_type_widget",
 *   label = @Translation("Chado String Widget"),
 *   description = @Translation("The default string type widget."),
 *   field_types = {
 *     "chado_string_type_default"
 *   }
 * )
 */
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
