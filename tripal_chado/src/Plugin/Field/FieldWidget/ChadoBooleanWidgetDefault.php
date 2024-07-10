<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalBooleanTypeWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado boolean type widget.
 *
 * @FieldWidget(
 *   id = "chado_boolean_type_widget",
 *   label = @Translation("Chado Boolean Widget"),
 *   description = @Translation("The default boolean type widget."),
 *   field_types = {
 *     "chado_boolean_type_default"
 *   }
 * )
 */
class ChadoBooleanWidgetDefault extends TripalBooleanTypeWidget {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $item_vals = $items[$delta]->getValue();
    $element = parent::formElement($items, $delta, $element, $form, $form_state);
    $element['record_id'] = [
      '#default_value' => !empty($item_vals['record_id']),
    ];
    return $element;
  }

}
