<?php

class ogi__location_on_map_formatter extends TripalFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Location on Map';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('ogi__location_on_map');

  // The list of default settings for this formatter.
  public static $default_settings = array();
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
