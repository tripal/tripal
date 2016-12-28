<?php

class chado_linker__prop_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Chado Property';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__prop');


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
    $field_name = $this->field['field_name'];
    $chado_table = $this->instance['settings']['chado_table'];

    foreach ($items as $delta => $item) {
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $item['value'],
      );
    }
  }
}
