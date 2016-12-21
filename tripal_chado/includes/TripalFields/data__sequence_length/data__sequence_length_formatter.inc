<?php

class data__sequence_length_formatter extends ChadoFieldFormatter {
  // The default lable for this field.
  public static $default_label = 'Sequence length';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence_length');

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
    foreach ($items as $delta => $item) {
      $element[$delta] = array(
        '#type' => 'markup',
        '#markup' => $item['value'],
      );
    }
  }
}
