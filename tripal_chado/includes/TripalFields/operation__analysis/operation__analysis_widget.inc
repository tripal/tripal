<?php

class operation__analysis_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Analysis';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['operation__analysis'];


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

    // Set the linker field appropriately.
    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    $analysis_id = 0;
    if (count($items) > 0 and array_key_exists($linker_field, $items[0])) {
      $analysis_id = $items[0][$linker_field];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $sql = "SELECT analysis_id, name FROM {analysis} ORDER BY name";
    $results = chado_query($sql);
    $options = ['' => '- Select an analysis -'];
    while ($r = $results->fetchObject()) {
      $options[$r->analysis_id] = $r->name;
    }
    $widget[$linker_field] = [
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $analysis_id,
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
    ];
  }

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Set the linker field appropriately.
    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    // Make sure the value is set to the organism_id
    $analysis_id = $form_state['values'][$field_name]['und'][0][$linker_field];
    $form_state['values'][$field_name]['und'][0]['value'] = $analysis_id;
  }
}
