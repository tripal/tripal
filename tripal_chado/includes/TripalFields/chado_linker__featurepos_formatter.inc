<?php

class chado_linker__featurepos_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $label = 'Map positions';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__featurepos');

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
