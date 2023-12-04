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
 *   description = @Translation("The sequence coordinates widget allows curators to manually enter feature sequence coordinates information on the content edit page."),
 *   field_types = {
 *     "chado_sequence_coordinates_default"
 *   }
 * )
 */
class ChadoSequenceCoordinatesFormatterTable extends ChadoFormatterBase {

  public static $default_settings = [
    'caption' => 'This @content_type has the following sequence coordinates:',
    'expand_strand' => TRUE,
  ];

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $reference_term = 'data:3002';

    // Get the settings and set defaults.
    $settings = $display['settings'];
    foreach ($this::$default_settings as $key => $value) {
      if (!isset($settings[$key])) {
        $settings[$key] = $value;
      }
    }

    // Replace tokens in the caption.
    $settings['caption'] = t($settings['caption'],
      ['@content_type' => $entity->rdfs__type['und'][0]['value']]);

    // For each location, add it to the table.
    $header = ['Name', 'Location', 'Strand', 'Phase'];

    $locations = [];

    foreach ($items as $item) {

      if (!empty($item['value'])) {
        $fmin_term = $item->get('fmin')->getString();
        $fmax_term = $item->get('fmax')->getString();
        $strand_term = $item->get('strand')->getString();
        $phase_term = $item->get('phase')->getString();

        $strand_val = $item['value'][$strand_term];
        if ($settings['expand_strand']) {
          $strand_symb = match( $strand_val ) {
            -1 => '-',
            1 => '+',
            default => '<span style="color:#B3B3B3">unknown</span>',
          };
        }
      }

      $locations[] = [
        $item['value'][$reference_term],
        $item['value'][$fmin_term] . '..' . $fmax = $item['value'][$fmax_term],
        $strand_symb, $item['value'][$phase_term],
      ];
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
      '#caption' => $settings['caption'],        
    ];

    return $elements;
  }
}