<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal additional type formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_additional_type_formatter_default',
  label: new TranslatableMarkup('Chado Type Reference Formatter'),
  description: new TranslatableMarkup('A Chado type reference formatter'),
  field_types: [
    'chado_additional_type_type_default',
  ],
)]
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
