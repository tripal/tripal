<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal additional type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_additional_type_formatter_default",
 *   label = @Translation("Chado Type Reference Formatter"),
 *   description = @Translation("A Chado type reference formatter"),
 *   field_types = {
 *     "chado_additional_type_type_default"
 *   }
 * )
 */
class ChadoAdditionalTypeFormatterDefault extends ChadoFormatterBase {

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
