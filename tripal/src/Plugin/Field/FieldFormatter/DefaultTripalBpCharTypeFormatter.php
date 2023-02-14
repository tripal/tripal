<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal bpchar type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_bpchar_type_formatter",
 *   label = @Translation("Default BpChar Type Formatter"),
 *   description = @Translation("The default bpchar type formatter."),
 *   field_types = {
 *     "tripal_bpchar_type"
 *   }
 * )
 */
class DefaultTripalBpCharTypeFormatter extends TripalFormatterBase {

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
