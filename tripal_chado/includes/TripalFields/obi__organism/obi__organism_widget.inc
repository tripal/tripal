<?php

class obi__organism_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Organism';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['obi__organism'];


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
    if ($field_table == 'biomaterial') {
      $linker_field = 'chado-biomaterial__taxon_id';
    }
    else {
      $linker_field = 'chado-' . $field_table . '__organism_id';
    }

    $organism_id = 0;
    if (count($items) > 0 and array_key_exists($linker_field, $items[0])) {
      $organism_id = $items[0][$linker_field];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $options = chado_get_organism_select_options(FALSE);
    $widget[$linker_field] = [
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $options,
      '#default_value' => $organism_id,
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

    // Set the linker field appropriately.
    if ($field_table == 'biomaterial') {
      $linker_field = 'chado-biomaterial__taxon_id';
    }
    else {
      $linker_field = 'chado-' . $field_table . '__organism_id';
    }

    // Make sure the value is set to the organism_id
    $organism_id = $form_state['values'][$field_name]['und'][0][$linker_field];
    if ($organism_id > 0) {
      $form_state['values'][$field_name]['und'][0]['value'] = $organism_id;
      $form_state['values'][$field_name]['und'][$delta][$linker_field] = $organism_id;
    }
    else {
      $form_state['values'][$field_name]['und'][0]['value'] = '';
      $form_state['values'][$field_name]['und'][$delta][$linker_field] = '__NULL__';
    }
  }
}
