<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_string_type_formatter",
 *   label = @Translation("Default String Type Formatter"),
 *   description = @Translation("The default string type formatter."),
 *   field_types = {
 *     "tripal_string_type"
 *   }
 * )
 */
class DefaultTripalStringTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get("value")->getString(),
      ];
    }

    return $elements;
  }
}
