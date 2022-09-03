<?php

namespace Drupal\tripal\TripalField;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;



/**
 * Defines the Tripal field widget base class.
 */
abstract class TripalWidgetBase extends WidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $element['record_id'] = [
      '#type' => 'value',
      '#default_value' => $items[$delta]->record_id ?? 0,
    ];
    return $element;
  }
}
