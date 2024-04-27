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

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        '#type' => 'processed_text',
        '#text' => $item->get('value'),
        '#format' => 'full_html',
        '#langcode' => $item->getLangcode(),
      ];
    }

    return $elements;
  }
}
