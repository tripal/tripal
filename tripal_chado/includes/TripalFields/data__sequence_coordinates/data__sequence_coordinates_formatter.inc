<?php

class data__sequence_coordinates_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Sequence Coordinate List';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_coordinates'];


  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $reference_term = 'data:3002';
    $fmin_term = chado_get_semweb_term('featureloc', 'fmin');
    $fmax_term = chado_get_semweb_term('featureloc', 'fmax');
    $strand_term = chado_get_semweb_term('featureloc', 'strand');
    $phase_term = chado_get_semweb_term('featureloc', 'phase');

    $locations = [];
    foreach ($items as $item) {
      if (!empty($item['value'])) {
        $srcfeature = $item['value'][$reference_term];
        $fmin = $item['value'][$fmin_term];
        $fmax = $item['value'][$fmax_term];
        $phase = $item['value'][$phase_term];
        $strand = $item['value'][$strand_term];
        $locations[] = $srcfeature . ':' . $fmin . '..' . $fmax . $strand;
      }
    }
    if (!$locations) {
      $content = 'This feature is not located on any sequence.';
    }
    else {
      $content = implode('<br />', $locations);
    }
    $element[0] = [
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
