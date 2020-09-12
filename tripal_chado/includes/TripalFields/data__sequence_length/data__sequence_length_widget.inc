<?php

class data__sequence_length_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Sequence length';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_length'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];

    $widget['chado-feature__seqlen'] = [
      '#type' => 'value',
      '#value' => 0,
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    ];
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the residues so we can calculate the length.
    if ($form_state['values']['data__sequence']['und'][0]['chado-feature__residues']) {
      $residues = $form_state['values']['data__sequence']['und'][0]['chado-feature__residues'];
      // Remove spaces and new lines from the residues string.
      $residues = preg_replace('/\s/', '', $residues);
      $length = strlen($residues);
      $form_state['values'][$field_name]['und'][0]['chado-feature__seqlen'] = $length;
      $form_state['values'][$field_name]['und'][0]['value'] = $length;
    }
    else {
      // Otherwise, remove the md5 value
      $form_state['values'][$field_name]['und'][0]['chado-feature__seqlen'] = '__NULL__';
    }
  }
}
