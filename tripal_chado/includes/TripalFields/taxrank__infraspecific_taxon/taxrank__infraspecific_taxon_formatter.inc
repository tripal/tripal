<?php

class taxrank__infraspecific_taxon_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $label = 'Infraspecific Taxon';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('taxrank__infraspecific_taxon');

  // The list of default settings for this formatter.
  public static $settings = array();
  /**
   * 
   * @see TripalFieldFormatter::settingsForm()
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   * 
   * @see TripalFieldFormatter::view()
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

  }
}
