<?php

class data__accession_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Site Accession';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__accession');


  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $content = 'N/A';
    if ($items[0]['value']) {
      $content = $items[0]['value'];
    }
    $element[0] = array(
      // We create a render array to produce the desired markup,
      '#type' => 'markup',
      '#markup' => $content,
    );
  }
}