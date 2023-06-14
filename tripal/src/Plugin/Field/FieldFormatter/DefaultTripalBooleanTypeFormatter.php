<?php

namespace Drupal\tripal\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal boolean type formatter.
 *
 * @FieldFormatter(
 *   id = "default_tripal_boolean_type_formatter",
 *   label = @Translation("Default Boolean Type Formatter"),
 *   description = @Translation("The default boolean type formatter."),
 *   field_types = {
 *     "tripal_boolean_type"
 *   }
 * )
 */
class DefaultTripalBooleanTypeFormatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get("value")->getValue() ? 'True' : 'False',
      ];
    }

    return $elements;
  }
}
