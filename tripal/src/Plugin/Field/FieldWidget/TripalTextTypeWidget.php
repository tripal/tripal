<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\TripalWidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal text type widget.
 *
 * @FieldWidget(
 *   id = "default_tripal_text_type_widget",
 *   label = @Translation("Tripal Text Widget"),
 *   description = @Translation("The default text type widget."),
 *   field_types = {
 *     "tripal_text_type"
 *   }
 * )
 */
class TripalTextTypeWidget extends TripalWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element + [
      '#base_type' => 'textarea',
      '#type' => 'text_format',
      '#format' => 'full_html',
      '#default_value' => $items[$delta]->value ?? '',
      '#placeholder' => $this->getSetting('placeholder'),
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
    ];
    return $element;
  }
}
