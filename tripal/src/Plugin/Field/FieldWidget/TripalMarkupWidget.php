<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
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

    // Define your form elements here.
    // If you have chosen to make a widget which does not allow editing of the
    // data then you should display it in disabled elements.
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
