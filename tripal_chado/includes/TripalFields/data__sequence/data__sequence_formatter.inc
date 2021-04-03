<?php

class data__sequence_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence'];

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // If there are no items, we don't want to return any markup.
    if (count($items) == 0 or (count($items) == 1 and empty($items[0]['value']))) {
      $element[0] = [
        '#type' => 'markup',
        '#markup' => 'No sequence is available.',
      ];
      return;
    }

    $num_bases = 50;
    $content = '<pre class="residues-formatter">';
    $content .= wordwrap($items[0]['value'], $num_bases, "<br>", TRUE);
    $content .= '</pre>';
    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
