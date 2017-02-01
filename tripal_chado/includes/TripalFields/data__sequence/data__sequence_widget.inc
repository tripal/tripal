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
    if (count($items) > 0 and array_key_exists('value', $items[0])) {
      $residues = $items[0]['value'];
    }
    if (array_key_exists('values', $form_state)) {
      $residues = $form_state['values'][$field_name][$langcode][$delta]['value'];
    }

    $widget['value'] = array(
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
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];

    // Remove any white spaces.
    $residues = isset($form_state['values'][$field_name][$langcode][$delta]['value']) ? $form_state['values'][$field_name][$langcode][$delta]['value'] : '';
    if ($residues) {
      $residues = preg_replace('/\s/', '', $residues);
      $form_state['values'][$field_name][$langcode][$delta]['value'] = $residues;
    }
  }
}
