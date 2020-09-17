<?php

class taxrank__infraspecific_taxon_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Infraspecific Taxon';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['taxrank__infraspecific_taxon'];

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $label_term = 'rdfs:label';
    $infraspecific_name_term = chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = chado_get_semweb_term('organism', 'type_id');

    if (is_array($items[0]['value']) and array_key_exists($infraspecific_name_term, $items[0]['value'])) {
      $infraspecific_name = $items[0]['value'][$infraspecific_name_term];
      $infraspecific_type = $items[0]['value'][$infraspecific_type_term];
      $content = $items[0]['value'][$label_term];

      // The cardinality of this field is 1 so we don't have to
      // iterate through the items array, as there will never be more than 1.
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
  }
}
