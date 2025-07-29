<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\TripalWidgetBase;

/**
 * Plugin implementation of default Tripal string type widget.
 */
#[FieldWidget(
  id: 'default_tripal_string_type_widget',
  label: new TranslatableMarkup('Tripal String Widget'),
  description: new TranslatableMarkup('The default string type widget.'),
  field_types: [
    'tripal_string_type',
  ],
)]
class TripalStringTypeWidget extends TripalWidgetBase {


  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $element['value'] = $element + [
      '#type' => 'textfield',
      '#default_value' => $items[$delta]->value ?? '',
      '#placeholder' => $this->getSetting('placeholder'),
      '#maxlength' => $this->getFieldSetting('max_length'),
      '#attributes' => ['class' => ['js-text-full', 'text-full']],
    ];
    return $element;
  }
}
