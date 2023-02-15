<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\tripal\Plugin\Field\FieldWidget\TripalIntegerTypeWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Chado varchar type widget.
 *
 * @FieldWidget(
 *   id = "chado_varchar_type_widget",
 *   label = @Translation("Chado VarChar Widget"),
 *   description = @Translation("The default varchar type widget."),
 *   field_types = {
 *     "chado_varchar_type"
 *   }
 * )
 */
class ChadoVarCharTypeWidget extends TripalVarCharTypeWidget {

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
