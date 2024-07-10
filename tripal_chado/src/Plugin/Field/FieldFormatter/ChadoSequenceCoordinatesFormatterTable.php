<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal string type formatter.
 *
 * @FieldFormatter(
 *   id = "chado_sequence_coordinates_formatter_table",
 *   label = @Translation("Chado sequence coordinates table formatter"),
 *   description = @Translation("The table sequence coordinates formatter allows curators to view sequence coordinates (min, max, strand and phase) of the feature in a tabular format."),
 *   field_types = {
 *     "chado_sequence_coordinates_default"
 *   }
 * )
 */
class ChadoSequenceCoordinatesFormatterTable extends ChadoFormatterBase {

  public static $default_settings = [
    'expand_strand' => TRUE,
  ];

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    // For each location, add it to the table.
    $header = ['Name', 'Loc.Min', 'Loc.Max','Phase','Strand'];

    $elements = [];
    $locations = [];

    foreach ($items as $item) {

      $ft_uniqname_val = $item->get('uniquename')->getString();
      $fmin_val = $item->get('fmin')->getString();
      $fmax_val = $item->get('fmax')->getString();

      $strand_val = $item->get('strand')->getString();
      $strand_symb = match( $strand_val ) {
        '-1' => '-',
        '1' => '+',
        default => 'unknown',
      };

      $phase_val = $item->get('phase')->getString();
      $locations[] = [ $ft_uniqname_val,
                        $fmin_val,
                        $fmax_val,
                        $phase_val,
                        $strand_symb ];
    }

    if ( !$locations ) {
      $content = 'This feature is not located on any sequence.';
    } 
    else {
      $content = $locations;
    }

    // The cardinality of this field is always 1, so only create element for $delta of zero.
    $elements[0] = [
      '#type' => 'markup',
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $content,
    ];

    return $elements;
  }
}