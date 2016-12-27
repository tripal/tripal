<?php

class chado_linker__prop_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Chado Property';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__prop');

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
    $instance = $this->instance;

    // Get the name of the pkey field for this property table and the name
    // of the FK field that links to the base table.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $base_table = $this->instance['settings']['base_table'];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);
    $rfkey_field = $schema['foreign keys'][$base_table]['columns'][$lfkey_field];

    // Get the field defaults.
    $record_id = '';
    $fk_value = '';
    $value = '';
    $rank = $delta;
    $type_id = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      // Check for element values that correspond to fields in the Chado table.
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $fk_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $lfkey_field, $fk_value);
      $type_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__type_id', $type_id);
      $value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__value', $value);
      $rank = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__rank', $rank);
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey];
      $fk_value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $lfkey_field];
      $type_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'];
      $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'];
      $rank = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'];
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $field_table . '__' . $pkey] = array(
      '#type' => 'hidden',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $field_table . '__' . $lfkey_field] = array(
      '#type' => 'hidden',
      '#value' => $fk_value,
    );
    $widget['chado-' . $field_table . '__value'] = array(
      '#type' => 'textfield',
      '#default_value' => $value,
      '#title' => $instance['label'] . ' value',
      '#description' => $instance['description'],
    );
    $widget['chado-' . $field_table . '__type_id'] = array(
      '#type' => 'hidden',
      '#value' => $type_id,
    );
    $widget['chado-' . $field_table . '__rank'] = array(
      '#type' => 'hidden',
      '#value' => $rank,
    );
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $base_table = $this->instance['settings']['base_table'];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);
    $rfkey_field = $schema['foreign keys'][$base_table]['columns'][$lfkey_field];

    $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'];
    $form_state['values'][$field_name]['und'][$delta]['value'] = $value;

    // If the user removed the property then we want to clear out the other
    // fields so there is no insert.
    if (!$value) {
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $lfkey_field] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'] = '';
    }
  }
}
