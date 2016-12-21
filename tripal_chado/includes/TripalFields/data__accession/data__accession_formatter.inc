<?php

class data__accession_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Accession';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__accession');


  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    foreach ($items as $delta => $item) {
      if ($item['value']) {
        $content = $item['value']['vocabulary'] . ':' . $item['value']['accession'];
        $element[$delta] = array(
          '#type' => 'markup',
          '#markup' => $content,
        );
      }
    }
  }
}