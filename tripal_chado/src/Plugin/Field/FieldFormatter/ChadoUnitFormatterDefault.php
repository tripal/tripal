<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_unit_formatter_default",
 *   label = @Translation("Chado Unit Formatter"),
 *   description = @Translation("The default unit widget which allows curators to enter unit on the Gene Map content edit page."),
 *   field_types = {
 *     "chado_unit_default"
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
        // "#markup" => $item->get('unittype_id')->getString()
        "#markup" => $item->get('cv_name')->getString()
      ];
    }

    return $elements;
  }
}