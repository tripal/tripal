<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\TripalFormatterBase;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "obi__organism_formatter",
 *   label = @Translation("Chado Organism Reference Formatter"),
 *   description = @Translation("A chado organism reference formatter"),
 *   field_types = {
 *     "obi__organism"
 *   }
 * )
 */
class obi__organism_formatter extends TripalFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach($items as $delta => $item) {
      $elements[$delta] = [
        "#markup" => $item->get('rdfs_label')->getString()
      ];
    }

    return $elements;
  }
}
