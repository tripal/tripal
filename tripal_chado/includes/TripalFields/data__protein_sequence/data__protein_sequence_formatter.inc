<?php

class data__protein_sequence_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Protein Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__protein_sequence'];

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $content = 'There is no protein sequence.';
    if (count($items) > 0 and $items[0]['value']) {
      $num_bases = 50;
      $content = '<pre class="protein-residues-formatter">';
      $content .= wordwrap($items[0]['value'], $num_bases, "<br>", TRUE);
      $content .= '</pre>';
    }
    $element[0] = [
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
