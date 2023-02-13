<?php

class chado_linker__organism_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Organisms';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['chado_linker__organism'];

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

    // Get the FK column that links to the base table.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];

    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Get the field defaults.
    $record_id = '';
    $fk_value = (array_key_exists('#entity', $element) and $element['#entity']) ? $element['#entity']->chado_record_id : NULL;
    $organism_id = '';
    $name = '';
    $name_label = '';

    // Because the scientific name is constructed from multiple table
    // columns, it requires its own cv term.
    $name_term = 'data:2909'; // EDAM:Organism name

    // If the field already has a value then it will come through the $items
    // array. This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      if (is_array($items[$delta]) and is_array($items[$delta]['value']) and array_key_exists($name_term, $items[$delta]['value'])) {
        $name = $items[$delta]['value'][$name_term];
      }
      $fk_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $fkey_lcolumn, $fk_value);
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $organism_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__organism_id', $organism_id);
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $name = $form_state['values'][$field_name][$langcode][$delta]['organism_name'];
    }

    // Get element values added to help support insert/updates.
    $name_label = tripal_get_field_item_keyval($items, $delta, 'organism_name', $name);

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];

    $widget['chado-' . $field_table . '__' . $pkey] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $widget['chado-' . $field_table . '__' . $fkey_lcolumn] = [
      '#type' => 'value',
      '#default_value' => $fk_value,
    ];
    $widget['chado-' . $field_table . '__organism_id'] = [
      '#type' => 'value',
      '#default_value' => $organism_id,
    ];

    $widget['name'] = [
      '#type' => 'textfield',
      '#title' => t('organism'),
      '#default_value' => $name_label,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/organism',
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

    // Get information about this organism linker table.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);

    // Get the name from the form state.
    $name = $form_state['values'][$field_name][$langcode][$delta]['name'];

    // If the user provided a name then we want to set the foreign key
    // value to be the chado_record_id
    if ($name) {
      $matches = [];
      if (preg_match('/^.*\[id: (\d+)\]$/', $name, $matches)) {
        $organism_id = $matches[1];
      }
      else {
        $organism_ids = chado_get_organism_id_from_scientific_name($name);
        if (count($organism_ids)==1) {
          $organism_id = $organism_ids[0];
        }
      }
      if ($organism_id) {
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__organism_id'] = $organism_id;
        $form_state['values'][$field_name][$langcode][$delta]['value'] = $name;
      }
      else {
        form_set_error($field_name . '][' . $langcode . '][' . $delta . '][name', 'The organism does not exist');
      }
    }
    // If no name is provided then we want to set the field for deletion.
    else {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__' . $lfkey_field] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__organism_id'] = '';
    }
  }
}
