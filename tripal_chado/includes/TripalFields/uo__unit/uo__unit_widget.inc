<?php

class uo__unit_widget extends ChadoFieldWidget {

  // The default lable for this field.
  public static $default_label = 'Unit';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('uo__unit');


  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // TODO: add a form for changing the unit types.
  }
}
