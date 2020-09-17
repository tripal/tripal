<?php

class schema__additional_type_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['schema__additional_type'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $content = '';
    if (count($items) > 0) {
      $content = $items[0]['value'];
    }

    $element[0] = [
      '#type' => 'markup',
      '#markup' => $content,
    ];
  }
}
