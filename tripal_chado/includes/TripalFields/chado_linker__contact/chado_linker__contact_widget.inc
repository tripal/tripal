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

    // Get the FK column that links to the base table.
    $table_name = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = array_key_exists('#entity', $element) and $element['#entity'] ? $element['#entity']->chado_record_id : NULL;
    $contact_id = '';
    $name = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__' . $pkey, $record_id);
      $contact_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $table_name . '__contact_id', $contact_id);
      if ($contact_id) {
        $contact = chado_generate_var('contact', array('contact_id' => $contact_id));
        $name = $contact->name;
      }
    }

    $schema = chado_get_schema('contact');

    $widget['#table_name'] = $table_name;
    $widget['#fkey_field'] = $fkey;
    $widget['#prefix'] =  "<span id='$table_name-$delta'>";
    $widget['#suffix'] =  "</span>";

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $table_name . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $table_name . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    $widget['chado-' . $table_name . '__contact_id'] = array(
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
        'wrapper' => "$table_name-$delta",
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
    // Get the FK column that links to the base table.
    $table_name = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];
    $field_name = $this->field['field_name'];

    // Get the field values.
    $fkey_value = isset($form_state['values'][$field_name][$langcode][$delta]['value']) ? $form_state['values'][$field_name][$langcode][$delta]['value'] : '';
    $contact_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id'] : '';
    $name = isset($form_state['values'][$field_name][$langcode][$delta]['name']) ? $form_state['values'][$field_name][$langcode][$delta]['name'] : '';

    // If the user provided a name then we want to set the foreign key
    // value to be the chado_record_id
    if ($name and !$contact_id) {
      $contact = chado_generate_var('contact', array('name' => $name));
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id'] = $contact->contact_id;
    }

    // In the widgetForm function we automatically add the foreign key
    // record.  But if the user did not provide a contact we want to take
    // it out so that the Chado field_storage infrastructure won't try to
    // write a record.
    if (!$name and !$contact_id) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
    }

    // If the user removed the contact from the contact_name field
    // then we want to clear out the rest of the hidden values.
    // Leave the primary key so the record can be deleted.
    if (!$name and $contact_id) {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__contact_id'] = '';
    }
  }
}
