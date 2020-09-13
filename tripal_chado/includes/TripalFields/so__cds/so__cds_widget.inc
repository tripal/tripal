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
