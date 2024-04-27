<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal text type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_text_type_formatter",
 *   label = @Translation("Default Text Type Formatter"),
 *   description = @Translation("The default text type formatter."),
 *   field_types = {
 *     "tripal_text_type"
 *   }
 * )
 */
class DefaultTripalTextTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    // Default filter format.
    $filter_format = 'basic_html';

    // We need to get the format set in the widget settings
    // because they need to match.
    $entity_type = $this->fieldDefinition->get('entity_type');
    $bundle = $this->fieldDefinition->get('bundle');
    $field_name = $this->fieldDefinition->get('field_name');
    $form_display = \Drupal::service('entity_display.repository')->getFormDisplay($entity_type, $bundle);
    $widget = $form_display->getComponent($field_name);
    if (array_key_exists('filter_format', $widget['settings'])) {
      $filter_format = $widget['settings']['filter_format'];
    }

    foreach($items as $delta => $item) {
      $value_string = $item->get('value')->getValue();
      $elements[$delta] = [
        '#type' => 'processed_text',
        '#text' => $value_string,
        '#format' => $filter_format,
        '#langcode' => $item->getLangcode(),
      ];
    }

    return $elements;
  }
}
