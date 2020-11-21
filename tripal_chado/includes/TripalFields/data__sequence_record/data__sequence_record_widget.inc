<?php

class data__sequence_record_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Sequences';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_record'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

  }
}
