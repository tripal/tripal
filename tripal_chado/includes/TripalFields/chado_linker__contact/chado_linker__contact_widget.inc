<?php

class chado_linker__contact_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Contacts';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__contact');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $base_table = $this->instance['settings']['base_table'];
    $chado_table = $this->instance['settings']['chado_table'];
    $chado_column = $this->instance['settings']['chado_column'];
    $instance = $this->instance;

    // Get the FK column that links to the base table.
    $schema = chado_get_schema($chado_table);
    $pkey = $schema['primary key'][0];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);

    // Get the field defaults.
    $record_id = '';
    $fk_value = array_key_exists('#entity', $element) and $element['#entity'] ? $element['#entity']->chado_record_id : NULL;
    $contact_id = '';
    $name = '';
    $value = '';


    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0) {
      // Check for element values that correspond to fields in the Chado table.
      $fk_value = tripal_get_field_item_keyval($items, 0, 'chado-' . $chado_table . '__' . $lfkey_field, $fk_value);
      if (array_key_exists($delta, $items)) {
        $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $chado_table . '__' . $pkey, $record_id);
        $contact_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $chado_table . '__contact_id', $contact_id);
      }
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $chado_table . '__' . $pkey];
      $fk_value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $chado_table . '__' . $lfkey_field];
      $contact_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $chado_table . '__contact_id'];
    }


    $schema = chado_get_schema('contact');

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $chado_table . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $chado_table . '__' . $lfkey_field] = array(
      '#type' => 'value',
      '#default_value' => $fk_value,
    );
    $widget['chado-' . $chado_table . '__contact_id'] = array(
      '#type' => 'value',
      '#default_value' => $contact_id,
    );

    $widget['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Contact'),
      '#default_value' => $name,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/contact',
      '#ajax' => array(
        'callback' => "chado_linker__contact_widget_form_ajax_callback",
        'wrapper' => "$chado_table-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
      '#maxlength' => 100000,
    );
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $base_table = $this->instance['settings']['base_table'];
    $chado_table = $this->instance['settings']['chado_table'];
    $chado_column = $this->instance['settings']['chado_column'];
    $instance = $this->instance;
    $schema = chado_get_schema($chado_table);

    $pkey = $schema['primary key'][0];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);

    // Get the field values.
    $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $chado_table . '__' . $pkey];
    $fk_value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $chado_table . '__' . $lfkey_field];
    $contact_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $chado_table . '__contact_id'];
    $name = $form_state['values'][$field_name]['und'][$delta]['name'];

    // If the user provided a name then we want to set the foreign key
    // value to be the chado_record_id
    if ($name) {
      $contact = chado_generate_var('contact', array('name' => $name));
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $chado_table . '__contact_id'] = $contact->contact_id;
      $form_state['values'][$field_name][$langcode][$delta]['value'] = $name;
    }
    dpm($form_state['values']);
  }
}
