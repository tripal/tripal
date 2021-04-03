<?php

class data__sequence_length_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Sequence length';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_length'];


  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    foreach ($items as $delta => $item) {
      $element[$delta] = [
        '#type' => 'markup',
        '#markup' => $item['value'],
      ];
    }
  }
}
