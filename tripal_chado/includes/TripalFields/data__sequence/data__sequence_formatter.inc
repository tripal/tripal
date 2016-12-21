<?php

class data__sequence_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $label = 'Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence');

  // The list of default settings for this formatter.
  public static $settings = array();

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
    $element[0] = array(
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => '',
    );

    $num_bases = 50;
    foreach ($items as $delta => $item) {
      $content = '<pre class="residues-formatter">';
      $content .= wordwrap($item['value'], $num_bases, "<br>", TRUE);
      $content .= '</pre>';

      $element[$delta] = array(
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }
}
