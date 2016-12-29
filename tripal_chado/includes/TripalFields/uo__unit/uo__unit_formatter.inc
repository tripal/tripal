<?php

class uo__unit_formatter extends ChadoFieldFormatter {

  // The default lable for this field.
  public static $default_label = 'Unit';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('uo__unit');

  /**
   * @see TripalFieldFormatter::view()
  */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $items[0]['value'],
      );
    }
  }
}