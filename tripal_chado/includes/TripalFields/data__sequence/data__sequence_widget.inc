<?php

class data__sequence_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the field defaults.
    $residues = '';
    if (count($items) > 0 and array_key_exists('chado-feature__residues', $items[0])) {
      $residues = $items[0]['chado-feature__residues'];
    }
    if (array_key_exists('values', $form_state)) {
      //$residues = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'feature__residues');
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    $widget['chado-feature__residues'] = array(
      '#type' => 'textarea',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#default_value' => $residues,
      '#delta' => $delta,
      '#cols' => 30,
    );
  }

  /**
   * Performs validation of the widgetForm.
   *
   * Use this validate to ensure that form values are entered correctly.  Note
   * this is different from the validate() function which ensures that the
   * field data meets expectations.
   *
   * @param $form
   * @param $form_state
   */
  public function validate($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];

    // Remove any white spaces.
    $residues = isset($form_state['values'][$field_name][$langcode][$delta]['chado-feature__residues']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-feature__residues'] : '';
    if ($residues) {
      $residues = preg_replace('/\s/', '', $residues);
      $form_state['values'][$field_name][$langcode][$delta]['chado-feature__residues'] = $residues;
    }
  }
}
