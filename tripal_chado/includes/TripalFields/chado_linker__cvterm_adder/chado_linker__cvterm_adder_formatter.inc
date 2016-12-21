<?php

class chado_linker__cvterm_adder_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Add an Annotation Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__cvterm_adder');

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
