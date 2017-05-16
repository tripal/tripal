<?php

class sio__vocabulary_widget extends ChadoFieldWidget {

  // The default lable for this field.
  public static $default_label = 'Vocabulary';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sio__vocabulary');


  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $cv_id = 0;

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $widget['chado-' . $field_table . '__cv_id'] = array(
      '#type' => 'value',
      '#value' => $items[$delta]['chado-' . $field_table . '__cv_id'],
    );
    $widget['vocabulary_name'] = array(
      '#type' => 'item',
      '#title' => 'Vocabulary',
      '#markup' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
  }

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];

    // Make sure the value is set to the cv_id
    $cv_id = $form_state['values'][$field_name][$langcode][0]['chado-' . $field_table . '__cv_id'];
    $form_state['values'][$field_name][$langcode][0]['value'] = $cv_id;
  }
}
