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
 *   id = "chado_synonym_formatter_default",
 *   label = @Translation("Chado Synonym Formatter"),
 *   description = @Translation("A chado synonym formatter"),
 *   field_types = {
 *     "chado_synonym_default"
 *   }
 * )
 */
class ChadoSynonymFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $elements['#attached']['library'][] = 'tripal_chado/tripal_chado.field.ChadoSynonymFormatterDefault';

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('name')->getString()
      ];
    }

    return $elements;
  }
}

