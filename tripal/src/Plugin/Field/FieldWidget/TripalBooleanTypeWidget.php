<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\TripalWidgetBase;

/**
 * Plugin implementation of default Tripal boolean type widget.
 */
#[TripalFieldWidget(
  id: 'default_tripal_boolean_type_widget',
  label: new TranslatableMarkup('Tripal Boolean Widget'),
  description: new TranslatableMarkup('The default boolean type widget.'),
  field_types: [
    'tripal_boolean_type',
  ],
)]
class TripalBooleanTypeWidget extends TripalWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element + [
      '#type' => 'checkbox',
      '#default_value' => $items[$delta]->value ?? NULL,
      '#placeholder' => $this->getSetting('placeholder'),
    ];
    return $element;
  }
}
