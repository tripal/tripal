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

    $content = '';
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
