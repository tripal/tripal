<?php

class chado_linker__prop extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Property';

  // The default description for this field.
  public static $default_description = 'Add details about this property.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {

    $field_name = $this->field['field_name'];
    $chado_table = $this->instance['settings']['chado_table'];

    $properties = array();
    foreach ($items as $delta => $item) {
      $properties[] = $item['chado-' . $chado_table . '__value'];
    }
    $content = implode(', ', $properties);

    if (count($items) > 0) {
      $element[0] = array(
        '#type' => 'markup',
        '#markup' => $content,
      );
    }
  }
  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::widgetForm($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
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
    $base_table = $this->instance['settings']['base_table'];
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
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $table_name = $this->instance['settings']['chado_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $base_table = $this->instance['settings']['base_table'];
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

  /**
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    $table_name = $matches[1];
    $cvterm_id = $matches[2];
    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $chado_record = $entity->chado_record;
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => $chado_record->{$fkey_lcolumn},
      'chado-' . $field_table . '__value' => '',
      'chado-' . $field_table . '__type_id' => '',
      'chado-' . $field_table . '__rank' => '',
    );

    // Get the properties associated with this base record for this fields
    // given type.
    $columns = array('*');
    $match = array(
      $fkey_lcolumn => $chado_record->{$fkey_lcolumn},
      'type_id' => $cvterm_id,
    );
    $options = array(
      'return_array' => TRUE,
      'order_by' => array('rank' => 'ASC')
    );
    $properties = chado_select_record($field_table, $columns, $match, $options);
    for ($i = 0; $i < count($properties); $i++) {
      $property = $properties[$i];
      foreach ($schema['fields'] as $fname => $details) {
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(),
          'chado-' . $field_table . '__' . $pkey => $property->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $property->$fkey_lcolumn,
          'chado-' . $field_table . '__value' => $property->value,
          'chado-' . $field_table . '__type_id' => $property->type_id,
          'chado-' . $field_table . '__rank' => $property->rank,
        );
      }
    }
  }
}
