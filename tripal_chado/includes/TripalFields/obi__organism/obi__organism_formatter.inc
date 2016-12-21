<?php

class obi__organism_formatter extends TripalFieldFormatter {

  // The default lable for this field.
  public static $default_label = 'Organism';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('obi__organism');

  // The list of default settings for this formatter.
  public static $default_settings = array();

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if (count($items) > 0) {
      $content = $items[0]['value']['rdfs:label'];
      if (array_key_exists('entity', $items[0]['value'])) {
        list($entity_type, $entity_id) = explode(':', $items[0]['value']['entity']);
        $content = l(strip_tags($items[0]['value']['rdfs:label']), 'bio_data/' . $entity_id);
      }

      // The cardinality of this field is 1 so we don't have to
      // iterate through the items array, as there will never be more than 1.
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }
}