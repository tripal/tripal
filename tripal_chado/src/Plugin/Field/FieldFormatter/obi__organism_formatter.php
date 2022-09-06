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
    $storage = \Drupal::entityTypeManager()->getStorage('chado_term_mapping');
    $mapping = $storage->load('core_mapping');

    foreach($items as $delta => $item) {
      //dpm($item);
      $label_term = 'rdfs:label';
      $genus_term = $mapping->getColumnTermId('organism', 'genus');
      $species_term = $mapping->getColumnTermId('organism', 'species');
      $iftype_term = $mapping->getColumnTermId('organism', 'type_id');
      $ifname_term = $mapping->getColumnTermId('organism', 'infraspecific_name');
      $genus = $item->get(preg_replace('/[^\w]/', '_', $genus_term))->getString();
      $species = $item->get(preg_replace('/[^\w]/', '_', $species_term))->getString();

      // @todo: we need full support of organism names, including the
      // infraspecific type, and name. We should be showing the label term
      // instead.
      $elements[$delta] = [
        "#markup" => $item->get('value')->getString(),
      ];
    }

    return $elements;
  }
}
