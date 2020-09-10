<?php

/**
 * @file
 * Contains \Drupal\tripal_chado\Plugin\Field\FieldFormatter\OBIOrganismDefaultFormatter.
 */

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * Provides a default comment formatter.
 *
 * @FieldFormatter(
 *   id = "obi__organism_default",
 *   module = "tripal_chado",
 *   label = @Translation("Organism default"),
 *   field_types = {
 *     "obi__organism"
 *   }
 * )
 */
class OBIOrganismDefaultFormatter extends FormatterBase {
  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = array();

    foreach ($items as $delta => $item) {

      // Render output using snippets_default theme.
      $source = array(
        '#theme' => 'snippets_default',
        //'#scientific_name' => $item->scientific_name,
      );

      $elements[$delta] = array('#markup' => \Drupal::service('renderer')->render($source));
    }

    return $elements;
  }
}