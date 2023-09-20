<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal analysis formatter.
 *
 * @FieldFormatter(
 *   id = "chado_analysis_formatter_default",
 *   label = @Translation("Chado analysis formatter"),
 *   description = @Translation("A chado analysis formatter"),
 *   field_types = {
 *     "chado_analysis_default"
 *   }
 * )
 */
class ChadoAnalysisFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('analysis_name')->getString()
      ];
    }

    return $elements;
  }
}
