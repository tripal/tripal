<?php

class data__sequence_checksum_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Sequence checksum';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_checksum'];

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    foreach ($items as $delta => $item) {
      $content = key_exists('value', $item) ? $item['value'] : '';
      $element[$delta] = [
        // We create a render array to produce the desired markup,
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
  }
}
