<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class sep__protocol_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Protocol';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sep__protocol'];

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
    $base_table = $this->instance['settings']['base_table'];
    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    $protocol_term = chado_get_semweb_term($field_column, $field_table);
    $protocol_name_term = chado_get_semweb_term('protocol', 'name');
    $protocol_type_term = chado_get_semweb_term('protocol', 'type_id');

    // Set defaults for the form elements.
    $protocol_name = '';
    $protocol_id = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $protocol_name = array_key_exists($protocol_name_term, $items[$delta]['value']) ? $items[$delta]['value'][$protocol_name_term] : $protocol_name;
    }

    // Get the list of protocols
    $protocols = [];
    $sql = "SELECT * FROM {protocol}";
    $results = chado_query($sql);
    foreach ($results as $protocol) {
      $protocols[$protocol->protocol_id] = $protocol->name;
      if ($protocol->name == $protocol_name) {
        $protocol_id = $protocol->protocol_id;
      }
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];

    $widget[$linker_field] = [
      '#type' => 'select',
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#options' => $protocols,
      '#empty_option' => '- Select a Protocol -',
      '#required' => $element['#required'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#delta' => $delta,
      '#default_value' => $protocol_id,
    ];
  }

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    // Make sure the value is set to the organism_id
    $protocol_id = $form_state['values'][$field_name]['und'][0][$linker_field];
    if ($protocol_id) {
      $form_state['values'][$field_name]['und'][0]['value'] = $protocol_id;
    }
    else {
      $form_state['values'][$field_name]['und'][0]['value'] = '';
      $form_state['values'][$field_name]['und'][0][$linker_field] = '__NULL__';
    }
  }

  /**
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
  }

}