<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal organism formatter.
 *
 * @FieldFormatter(
 *   id = "chado_organism_formatter_default",
 *   label = @Translation("Chado Organism Reference Formatter"),
 *   description = @Translation("A chado organism reference formatter"),
 *   field_types = {
 *     "chado_organism_type_default"
 *   }
 * )
 */
class ChadoOrganismFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('label')->getString()
      ];
    }

    return $elements;
  }
}
