<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalTextTypeWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado text type widget.
 *
 * @FieldWidget(
 *   id = "chado_text_type_widget",
 *   label = @Translation("Chado Text Widget"),
 *   description = @Translation("The default text type widget."),
 *   field_types = {
 *     "chado_text_type_default"
 *   }
 * )
 */
class ChadoTextWidgetDefault extends TripalTextTypeWidget {

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
