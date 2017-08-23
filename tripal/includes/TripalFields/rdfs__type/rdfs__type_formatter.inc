<?php

class rdfs__type_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Content Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('rdfs__type');

  // The list of default settings for this formatter.
  public static $default_settings = array();

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    foreach ($items as $delta => $item) {
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $item['value'],
      );
    }
  }
}
