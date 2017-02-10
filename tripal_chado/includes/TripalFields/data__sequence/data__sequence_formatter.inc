<?php

class data__sequence_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence');

  /**
   *
   * @param unknown $element
   * @param unknown $entity_type
   * @param unknown $entity
   * @param unknown $langcode
   * @param unknown $items
   * @param unknown $display
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $content = 'There is no sequence.';
    if ($items[0]['value']) {
      $num_bases = 50;
      $content = '<pre class="residues-formatter">';
      $content .= wordwrap($items[0]['value'], $num_bases, "<br>", TRUE);
      $content .= '</pre>';
    }
    $element[0] = array(
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}
