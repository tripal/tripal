<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_sequence_coordinates_formatter_default",
 *   label = @Translation("Chado sequence coordinates default formatter"),
 *   description = @Translation("The default sequence coordinates formatter allows curators to view sequence coordinates (min, max, strand and phase) of the feature."),
 *   field_types = {
 *     "chado_sequence_coordinates_default"
 *   }
 * )
 */
class ChadoSequenceCoordinatesFormatterDefault extends ChadoFormatterBase
{
  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode)
  {
    $elements = [];
    $reference_term = 'data:3002';

    $locations = [];
    
    foreach ($items as $item) {

      if (!empty($item['value'])) {
        $loc_rec = '';
        $feature_ref_val = $item['value'][$reference_term];

        $fmin_val = $item->get('fmin')->getString();
        if (!empty($fmin_val)) {
        $loc_rec .= $item['value'][$feature_ref_val] . ':' .$fmin_val . "..";
        }
        $fmax_val = $item->get('fmax')->getString();
        if (!empty($fmax_val)) {
            $loc_rec .= $item['value'][$fmax_val].'; ';
        }
        $phase_val = $item->get('phase')->getString();
        if (!empty($phase_val)) {
            $loc_rec .= $item['value'][$phase_val].'; ';
        }
        $strand_term = $item->get('strand')->getString();
        if (!empty($strand_val)) {
            $strand_symb = match( $item['value'][$strand_term] ) {
                -1 => '-',
                1 => '+',
                default => '',
            };
            $loc_rec .= $strand_symb;
        }
        $locations[] = $loc_rec;
      }
    }
    if ( !$locations ) {
        $content = 'This feature is not located on any sequence.';
    }
    else {
        $content = implode('<br />', $locations);
    }
    // The cardinality of this field is always 1, so only create element for $delta of zero.
    $elements[0] = [
        '#type' => 'markup',
        '#markup' => $content,
    ];
    return $elements;
  }
}