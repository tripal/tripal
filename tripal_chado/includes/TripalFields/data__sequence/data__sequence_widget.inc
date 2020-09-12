<?php

class data__sequence_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Sequence';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence'];

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
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $residues = $form_state['values'][$field_name][$langcode][$delta]['value'];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => $residues,
    ];

    $widget['chado-' . $field_table . '__' . $field_column] = [
      '#type' => 'textarea',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#default_value' => $residues,
      '#delta' => $delta,
      '#cols' => 30,
    ];
  }

  /**
   * @see TripalFieldWidget::validateDefaults()
   */
  public function validateDefaults($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column];
    $form_state['values'][$field_name]['und'][$delta]['value'] = $value;
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Remove any white spaces.
    $residues = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column];
    if ($residues) {
      $residues = preg_replace('/\s/', '', $residues);
      $form_state['values'][$field_name]['und'][$delta]['value'] = $residues;
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column] = $residues;
    }
    // If the residue information has been removed then we want to signal such.
    // When it's removed the value != residues but if it was never set then they're both empty.
    elseif (!empty($form_state['values'][$field_name]['und'][$delta]['value'])) {
      $form_state['values'][$field_name]['und'][$delta]['value'] = 'delete_me';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $field_column] = '';
    }
  }
}
