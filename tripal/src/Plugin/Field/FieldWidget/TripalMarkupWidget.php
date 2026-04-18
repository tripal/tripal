<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\Plugin\Field\FieldType\TripalMarkupTypeItem;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;

/**
 * Plugin implementation of the 'Tripal Markup Form Display' field widget.
 */
#[TripalFieldWidget(
  id: 'tripal_markup_widget',
  label: new TranslatableMarkup('Tripal Markup Form Display'),
  description: new TranslatableMarkup('Displays the static markup provided in the field settings on the form.'),
  field_types: [
    'tripal_markup',
  ],
)]
class TripalMarkupWidget extends TripalWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    // The value is set to int in the field settings, so we need to get it from
    // the field definition, not the field item.
    $value = TripalMarkupTypeItem::getMarkupValue($items->getEntity(), $items->getFieldDefinition());
    // Save whether this field has a value or not.
    $elements['has_value'] = [
      '#type' => 'value',
      '#value' => !empty($value['value']),
    ];

    // Now add the markup element to the form.
    $elements['markup'] = [
      '#type' => 'processed_text',
      '#text' => $value['value'] ?? '',
      '#format' => $value['format'] ?? '',
    ];

    return $elements;
  }

}
