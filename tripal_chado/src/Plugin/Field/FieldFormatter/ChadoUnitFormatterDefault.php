<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_unit_formatter_default",
 *   label = @Translation("Chado unit type formatter"),
 *   description = @Translation("A Chado unit type formatter."),
 *   field_types = {
 *     "chado_unit_type_default"
 *   }
 * )
 */
class ChadoUnitFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode)  {

    $elements = [];
    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('cv_name')->getString()
      ];
    }

    return $elements;
  }
}