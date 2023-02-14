<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal bpchar type widget.
 *
 * @FieldWidget(
 *   id = "default_tripal_bpchar_type_widget",
 *   label = @Translation("Tripal BpChar Widget"),
 *   description = @Translation("The default bpchar type widget."),
 *   field_types = {
 *     "tripal_bpchar_type"
 *   }
 * )
 */
class TripalBpCharTypeWidget extends TripalWidgetBase {


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
