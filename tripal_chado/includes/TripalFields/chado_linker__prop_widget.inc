<?php

class chado_linker__prop_widget extends TripalFieldWidget {
  // The default lable for this field.
  public static $label = 'Property';

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
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $instance = $this->instance;
    
    // Get the table name and cvterm that this field maps to.
    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    // If the field name is not properly formatted then we can't tell what
    // table and type this is.  So just return.
    if (count($matches) != 3) {
      return $widget;
    }
    $table_name = $matches[1];
    $cvterm_id = $matches[2];
    
    // Get the name of the pkey field for this property table and the name
    // of the FK field that links to the base table.
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $base_table = $this->field['settings']['base_table'];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);
    $rfkey_field = $schema['foreign keys'][$base_table]['columns'][$lfkey_field];
    
    // Get the field defaults.
    $fk_value =key_exists(0, $items) ? $items[0]['chado-' . $field_table . '__' . $lfkey_field] : '';
    $propval = '';
    if (array_key_exists($delta, $items)) {
      $propval = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__value', $propval);
    }
    
    $widget['value'] = array(
      '#type' => 'value',
      '#value' => key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );
    
    $widget['chado-' . $table_name . '__' . $pkey] = array(
      '#type' => 'hidden',
      '#default_value' => !empty($items[$delta]['chado-' . $field_table . '__' . $pkey]) ? $items[$delta]['chado-' . $field_table . '__' . $pkey] : '',
    );
    $widget['chado-' . $table_name . '__' . $lfkey_field] = array(
      '#type' => 'hidden',
      '#value' => $fk_value,
    );
    $widget['chado-' . $table_name . '__value'] = array(
      '#type' => 'textfield',
      '#default_value' => $propval,
      '#title' => $instance['label'] . ' value',
      '#description' => $instance['description'],
    );
    $widget['chado-' . $table_name . '__type_id'] = array(
      '#type' => 'hidden',
      '#value' => $cvterm_id,
    );
    $widget['chado-' . $table_name . '__rank'] = array(
      '#type' => 'hidden',
      '#value' => $delta,
    );
  }

  /**
   * Performs validation of the widgetForm.
   *
   * Use this validate to ensure that form values are entered correctly.  Note
   * this is different from the validate() function which ensures that the
   * field data meets expectations.
   *
   * @param $form
   * @param $form_state
   */
  public function validate($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $table_name = $this->field['settings']['chado_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $base_table = $this->field['settings']['base_table'];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);
    $rfkey_field = $schema['foreign keys'][$base_table]['columns'][$lfkey_field];
    
    $prop_value = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__value']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__value'] : '';
    
    // If the user removed the contact from the contact_name field
    // then we want to clear out the rest of the hidden values.
    // Leave the primary key so the record can be deleted.
    if (!$prop_value) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $pkey] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__rank'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__type_id'] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $lfkey_field] = '';
    }
  }
}
