<?php

class remote__data_widget extends WebServicesFieldWidget {

  // The default label for this field.
  public static $default_label = 'Remote Data';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['remote__data'];

  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

  }

  /**
   * Performs validation of the widgetForm.
   *
   * Use this validate to ensure that form values are entered correctly.
   * The 'value' key of this field must be set in the $form_state['values']
   * array anytime data is entered by the user.  It may be the case that there
   * are other fields for helping select a value. In the end those helper
   * fields must be used to set the 'value' field.
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    //TODO validate the tokens, site, and query. Test that query returns data.

  }
}
