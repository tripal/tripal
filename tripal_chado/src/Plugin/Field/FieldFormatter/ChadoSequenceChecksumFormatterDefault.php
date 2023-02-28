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
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoSequenceChecksumFormatterDefault';

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('md5checksum')->getString()
      ];
    }

    return $elements;
  }
}

