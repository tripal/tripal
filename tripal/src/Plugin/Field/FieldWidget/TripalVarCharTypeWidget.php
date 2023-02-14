<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal varchar type widget.
 *
 * @FieldWidget(
 *   id = "default_tripal_varchar_type_widget",
 *   label = @Translation("Tripal VarChar Widget"),
 *   description = @Translation("The default varchar type widget."),
 *   field_types = {
 *     "tripal_varchar_type"
 *   }
 * )
 */
class TripalVarCharTypeWidget extends TripalWidgetBase {


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
}
