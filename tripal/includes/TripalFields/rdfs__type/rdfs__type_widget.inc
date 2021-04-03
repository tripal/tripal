<?php

class rdfs__type_widget extends TripalFieldWidget {

  // The default label for this field.
  public static $default_label = 'Content Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['rdfs__type'];

  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items,
                       $delta, $element) {

    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
  }
}
