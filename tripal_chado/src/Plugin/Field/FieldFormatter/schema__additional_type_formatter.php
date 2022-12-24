<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "schema__additional_type_formatter",
 *   label = @Translation("Chado Type Reference Formatter"),
 *   description = @Translation("A Chado type reference formatter"),
 *   field_types = {
 *     "schema__additional_type"
 *   }
 * )
 */
class schema__additional_type_formatter extends TripalFormatterBase {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('term_name')->getString()
      ];
    }

    return $elements;
  }

}