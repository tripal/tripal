<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal boolean type widget.
 *
 * @FieldWidget(
 *   id = "default_tripal_boolean_type_widget",
 *   label = @Translation("Tripal Boolean Widget"),
 *   description = @Translation("The default boolean type widget."),
 *   field_types = {
 *     "tripal_boolean_type"
 *   }
 * )
 */
class TripalBooleanTypeWidget extends TripalWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
dpm($items[$delta], "TripalBooleanTypeWidget items[$delta]"); //@@@
$v = $items[$delta]->value;
dpm($v, 'items[$delta]->value CP1'); //@@@
$ib = is_bool($v);
dpm($ib, 'is_bool'); //@@@
$v = $v ?? false;
dpm($v, 'items[$delta]->value CP2'); //@@@
    $element['value'] = $element + [
      '#type' => 'checkbox',
      '#default_value' => $items[$delta]->value ?? false,
      '#placeholder' => $this->getSetting('placeholder'),
    ];
    return $element;
  }
}
