<?php

class rdfs__type_formatter extends TripalFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Content Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['rdfs__type'];

  // The list of default settings for this formatter.
  public static $default_settings = [];

  /**
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
