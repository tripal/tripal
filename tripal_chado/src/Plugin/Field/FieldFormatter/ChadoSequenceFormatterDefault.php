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
 *   id = "chado_sequence_formatter_default",
 *   label = @Translation("Chado Sequence Formatter"),
 *   description = @Translation("A chado sequence formatter"),
 *   field_types = {
 *     "chado_sequence_default"
 *   }
 * )
 */
class ChadoSequenceFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoSequenceFormatterDefault';

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => "<pre id='tripal-chado-sequence-format'>" . wordwrap($item->get('residues')->getString(),50,'<br>',TRUE) . "</pre>",
      ];
    }

    return $elements;
  }
}
