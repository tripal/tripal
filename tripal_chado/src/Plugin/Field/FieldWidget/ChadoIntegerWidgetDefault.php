<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalIntegerTypeWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado integer type widget.
 *
 * @FieldWidget(
 *   id = "chado_integer_type_widget",
 *   label = @Translation("Chado Integer Widget"),
 *   description = @Translation("The default integer type widget."),
 *   field_types = {
 *     "chado_integer_type_default"
 *   }
 * )
 */
class ChadoIntegerWidgetDefault extends TripalIntegerTypeWidget {

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
