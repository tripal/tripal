<?php

class data__sequence_checksum_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Sequence checksum';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__sequence_checksum'];

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
    $md5checksum = '';
    if (count($items) > 0 and array_key_exists('chado-feature__md5checksum', $items[0])) {
      $md5checksum = $items[0]['chado-feature__md5checksum'];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => $md5checksum,
    ];
    $widget['chado-' . $field_table . '__md5checksum'] = [
      '#type' => 'value',
      '#value' => $md5checksum,
    ];
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field = $this->field;
    $settings = $field['settings'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the residues so we can calculate the length.
    if ($form_state['values']['data__sequence']['und'][0]['chado-feature__residues']) {
      // Remove spaces and new lines from the residues string.
      $residues = $form_state['values']['data__sequence']['und'][0]['chado-feature__residues'];
      $residues = preg_replace('/\s/', '', $residues);
      $checksum = md5($residues);
      $form_state['values'][$field_name]['und'][0]['chado-feature__md5checksum'] = $checksum;
      $form_state['values'][$field_name]['und'][0]['value'] = $checksum;
    }
    else {
      // Otherwise, remove the md5 value
      $form_state['values'][$field_name]['und'][0]['chado-feature__md5checksum'] = '__NULL__';
    }
  }
}
