<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of Default Tripal field formatter for sequence data 
 *
 * @FieldFormatter(
 *   id = "chado_sequence_length_formatter_commas",
 *   label = @Translation("Chado Sequence Length with Commas Formatter 1,000,000"),
 *   description = @Translation("A chado sequence length commas (ex. 1,000,000) formatter"),
 *   field_types = {
 *     "chado_sequence_length_default"
 *   }
 * )
 */
class ChadoSequenceLengthFormatterCommas extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoSequenceLengthFormatterCommas';

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => number_format(floatval($item->get('seqlen')->getString()))
      ];
    }

    return $elements;
  }
}
