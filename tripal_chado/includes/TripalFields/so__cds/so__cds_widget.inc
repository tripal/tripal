<?php

class so__cds_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Coding Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['so__cds'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

  }




  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
  }
}
