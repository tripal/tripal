<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal integer type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_integer_type_formatter",
 *   label = @Translation("Default Integer Type Formatter"),
 *   description = @Translation("The default integer type formatter."),
 *   field_types = {
 *     "tripal_integer_type"
 *   }
 * )
 */
class DefaultTripalIntegerTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get("value")->getValue(),
      ];
    }

    return $elements;
  }
}
