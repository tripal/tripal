<?php

class obi__organism_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Organism';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['obi__organism'];

  /**
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if ($items[0]['value']) {
      $content = $items[0]['value']['rdfs:label'];
      if (array_key_exists('entity', $items[0]['value'])) {
        list($entity_type, $entity_id) = explode(':', $items[0]['value']['entity']);
        $content = l(strip_tags($items[0]['value']['rdfs:label']), 'bio_data/' . $entity_id);
      }

      // The cardinality of this field is 1 so we don't have to
      // iterate through the items array, as there will never be more than 1.
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
  }
}
