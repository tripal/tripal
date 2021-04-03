<?php

class sio__vocabulary_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Vocabulary';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sio__vocabulary'];

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if (count($items) > 0) {
      // The cardinality of this field is 1 so we don't have to
      // iterate through the items array, as there will never be more than 1.
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $items[0]['value'],
      ];
    }
  }
}