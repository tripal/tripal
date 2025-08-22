<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal unit type formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_unit_formatter_default',
  label: new TranslatableMarkup('Chado unit type formatter'),
  description: new TranslatableMarkup('A Chado unit type formatter.'),
  field_types: [
    'chado_unit_type_default',
  ],
)]
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
