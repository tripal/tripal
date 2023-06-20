<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalBooleanTypeWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;
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
 *     "chado_boolean_type"
 *   }
 * )
 */
class ChadoBooleanTypeWidget extends TripalBooleanTypeWidget {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $item_vals = $items[$delta]->getValue();
dpm($item_vals, 'ChadoBooleanTypeWidget item_vals'); //@@@
    $element = parent::formElement($items, $delta, $element, $form, $form_state);
// drupal validation accepts: boolean, 0, 1, '0', '1' but not ''
//    $default_value = array_key_exists('record_id', $item_vals) ? ($item_vals['record_id'] ? 1 : 0) : 0;
    $default_value = array_key_exists('record_id', $item_vals) ? ($item_vals['record_id'] ? true : false) : false;
dpm($default_value, 'default_value'); //@@@
    $element['record_id'] = [
//      '#type' => 'checkbox',
      '#default_value' => $default_value, //$item_vals['record_id'] ?? false,
    ];
    return $element;
  }

  /**
   * {@inheritDoc}
   */
  public function massageFormValues(array $values, array $form, FormStateInterface $form_state) {
dpm($values, 'massageFormValues values 1'); //@@@
    // Convert to boolean.
    foreach ($values as $val_key => $value) {
dpm($value, 'massage '.$val_key); //@@@
      $values[$val_key]['value'] = $value['value'] ? true : false;
    }
dpm($values, 'massageFormValues values 2'); //@@@
    return $values;
  }
}
