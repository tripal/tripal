<?php

class chado_linker__prop_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Property';

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
    $chado_table = $this->field['settings']['chado_table'];

    $properties = array();
    foreach ($items as $delta => $item) {
      $properties[] = $item['chado-' . $chado_table . '__value'];
    }
    $content = implode(', ', $properties);

    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }
}
