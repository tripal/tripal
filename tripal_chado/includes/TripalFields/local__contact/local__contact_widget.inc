<?php

class local__contact_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Contact';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local_contact'];

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $base_table = $this->instance['settings']['base_table'];
    $field_table = $this->instance['settings']['chado_table'];
    $chado_column = $this->instance['settings']['chado_column'];
    $instance = $this->instance;

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];

    // Get the field defaults.
    $record_id = '';
    $contact_id = '';
    $name = '';
    $value = '';

    $name_term = chado_get_semweb_term('contact', 'name');

    // Set the linker field appropriately.
    $linker_field = 'chado-' . $field_table . '__' . $chado_column;

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $name = array_key_exists($name_term, $items[$delta]['value']) ? $items[$delta]['value'][$name_term] : $name;
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      if ($field_table == 'biomaterial') {
        $contact_id = tripal_get_field_item_keyval($items, $delta, $linker_field, $contact_id);
      }
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $name = $form_state['values'][$field_name]['und'][$delta]['name'];
    }

    $schema = chado_get_schema('contact');

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];

    $widget[$linker_field] = [
      '#type' => 'value',
      '#default_value' => $contact_id,
    ];
    $widget['name'] = [
      '#type' => 'textfield',
      '#title' => $element['#title'],
      '#default_value' => $name,
      '#required' => $element['#required'],
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/contact',
      '#maxlength' => 100000,
    ];

  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $base_table = $this->instance['settings']['base_table'];
    $field_table = $this->instance['settings']['chado_table'];
    $chado_column = $this->instance['settings']['chado_column'];
    $instance = $this->instance;

    // Get information about this contact linke rtable.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];

    // Get the name from the form state.
    $name = $form_state['values'][$field_name]['und'][$delta]['name'];

    // Set the linker field appropriately.
    $linker_field = 'chado-' . $field_table . '__' . $chado_column;

    // If the user provided a name then we want to set the foreign key
    // value to be the chado_record_id
    if ($name) {
      $contact = chado_generate_var('contact', ['name' => $name]);
      if ($contact) {
        $form_state['values'][$field_name]['und'][$delta][$linker_field] = $contact->contact_id;
        $form_state['values'][$field_name]['und'][$delta]['value'] = $name;
      }
    }
    // If no name is provided then we want to set the field for deletion.
    else {
      $form_state['values'][$field_name]['und'][$delta][$linker_field] = '__NULL__';
    }
  }
}
