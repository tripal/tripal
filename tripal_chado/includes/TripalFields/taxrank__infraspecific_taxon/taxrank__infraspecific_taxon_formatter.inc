<?php

class taxrank__infraspecific_taxon_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Infraspecific Taxon';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('taxrank__infraspecific_taxon');

  /**
   *
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

  }
}
