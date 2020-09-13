<?php

class uo__unit_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Unit';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['uo__unit'];


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

    $unittype_id = 0;
    if (count($items) > 0 and array_key_exists('chado-' . $field_table . '__unittype_id', $items[0])) {
      $unittype_id = $items[0]['chado-' . $field_table . '__unittype_id'];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $cv = chado_get_cv(['name' => 'featuremap_units']);
    $options = chado_get_cvterm_select_options($cv->cv_id);
    unset($options[0]);
    $widget['chado-' . $field_table . '__unittype_id'] = [
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $unittype_id,
      '#empty_option' => '- Select a Unit -',
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
    $field_table = $this->instance['settings']['chado_table'];

    // Make sure the value is set to the organism_id
    $unittype_id = $form_state['values'][$field_name]['und'][0]['chado-' . $field_table . '__unittype_id'];
    $form_state['values'][$field_name]['und'][0]['value'] = $unittype_id;
  }
}
