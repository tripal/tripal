<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of Default Tripal field formatter for sequence data 
 *
 * @FieldFormatter(
 *   id = "chado_sequence_checksum_formatter_default",
 *   label = @Translation("Chado Sequence checksum Formatter"),
 *   description = @Translation("A chado sequence checksum formatter"),
 *   field_types = {
 *     "chado_sequence_checksum_default"
 *   }
 * )
 */
class ChadoSequenceChecksumFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    foreach($items as $delta => $item) {
      $seqlen_val = $item->get('seqlen')->getString();
      if ( intval($seqlen_val) > 0 ) {
        $elements[$delta] = [
          "#markup" => $item->get('md5checksum')->getString()
        ];
      } 
    }

    return $elements;
  }

}
