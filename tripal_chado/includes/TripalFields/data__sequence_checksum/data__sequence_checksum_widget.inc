<?php

class data__sequence_checksum_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Sequence checksum';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('data__sequence_checksum');

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

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => $md5checksum,
    );
    $widget['chado-feature__md5checksum'] = array(
      '#type' => 'value',
      '#value' => $md5checksum,
    );
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field = $this->field;
    $settings = $field['settings'];
    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the residues so we can calculate the length.
    $residues = isset($form_state['values']['data__sequence'][$langcode][0]['chado-feature__residues']) ? $form_state['values']['data__sequence'][$langcode][0]['chado-feature__residues'] : '';

    if ($residues) {
      // Remove spaces and new lines from the residues string.
      $residues = preg_replace('/\s/', '', $residues);
      $form_state['values']['data__sequence'][$langcode][0]['chado-feature__residues'] = $residues;
      $form_state['values'][$field_name][$langcode][$delta]['chado-feature__md5checksum'] = md5($residues);
    }
    else {
      // Otherwise, remove the md5 value
      $form_state['values'][$field_name][$langcode][$delta]['chado-feature__md5checksum'] = '__NULL__';
    }
  }
}
