<?php

class local__gbrowse_image_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'GBrowse View';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__gbrowse_image');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    // TODO: add a widget...
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];

  }
}
