<?php

class data__sequence_coordinates_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Sequence Coordinates';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence_coordinates');


  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $reference_term = 'data:3002';
    $fmin_term = tripal_get_chado_semweb_term('featureloc', 'fmin');
    $fmax_term = tripal_get_chado_semweb_term('featureloc', 'fmax');
    $strand_term = tripal_get_chado_semweb_term('featureloc', 'strand');
    $phase_term = tripal_get_chado_semweb_term('featureloc', 'phase');

    $content = '';
    foreach ($items as $item) {
      if (!empty($item['value'])) {
        $srcfeature = $item['value'][$reference_term];
        $fmin = $item['value'][$fmin_term];
        $fmax = $item['value'][$fmax_term];
        $phase = $item['value'][$phase_term];
        $strand = $item['value'][$strand_term];
        $content .= $srcfeature . ':' . $fmin . '..' . $fmax . $strand;
      }
    }
    if (!$content) {
      $content = 'This feature is not located on any sequence.';
    }
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}
