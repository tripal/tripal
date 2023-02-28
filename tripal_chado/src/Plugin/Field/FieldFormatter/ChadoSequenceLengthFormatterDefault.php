<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of Default Tripal field formatter for sequence data 
 *
 * @FieldFormatter(
 *   id = "chado_sequence_length_formatter_default",
 *   label = @Translation("Chado Sequence Length Formatter"),
 *   description = @Translation("A chado sequence length formatter"),
 *   field_types = {
 *     "chado_sequence_length_default"
 *   }
 * )
 */
class ChadoSequenceLengthFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoSequenceLengthFormatterDefault';

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('seqlen')->getString()
      ];
    }

    return $elements;
  }
}

#"#markup" => number_format(floatval($item->get('seqlen')))->getString()
