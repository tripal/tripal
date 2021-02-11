<?php

class chado_linker__prop_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Chado Property';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['chado_linker__prop'];

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
    $rows = array_key_exists('rows', $this->instance['settings']) ? $this->instance['settings']['rows'] : 1;
    $instance = $this->instance;

    // Get the name of the pkey field for this property table and the name
    // of the FK field that links to the base table.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
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
    if (count($items) > 0) {
      // Check for element values that correspond to fields in the Chado table.
      $fk_value = tripal_get_field_item_keyval($items, 0, 'chado-' . $field_table . '__' . $lfkey_field, $fk_value);
      $type_id = tripal_get_field_item_keyval($items, 0, 'chado-' . $field_table . '__type_id', $type_id);
      if (array_key_exists($delta, $items)) {
        $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
        $value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__value', $value);
        $rank = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__rank', $rank);
      }
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey];
      $fk_value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $lfkey_field];
      $type_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'];
      $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'];
      $rank = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'];
    }

    // Use default value for the field if it's not already set
    if (!$value && isset($instance['default_value'][$delta])) {
      $value = $instance['default_value'][$delta]['value'];
    }
    if (!$type_id) {
      $vocabulary = $this->instance['settings']['term_vocabulary'];
      $accession = $this->instance['settings']['term_accession'];
      $cvterm = chado_get_cvterm([
        'dbxref_id' => [
          'db_id' => [
            'name' => $vocabulary,
          ],
          'accession' => $accession,
        ],
      ]);
      if ($cvterm) {
        $type_id = $cvterm->cvterm_id;
      }
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];

    $widget['chado-' . $field_table . '__' . $pkey] = [
      '#type' => 'hidden',
      '#default_value' => $record_id,
    ];
    $widget['chado-' . $field_table . '__' . $lfkey_field] = [
      '#type' => 'hidden',
      '#value' => $fk_value,
    ];
    $widget['chado-' . $field_table . '__value'] = [
      '#type' => 'textarea',
      '#default_value' => $value,
      '#title' => $instance['label'],
      '#description' => $instance['description'],
      '#rows' => $rows,
      '#required' => $instance['required'],
    ];
    $widget['chado-' . $field_table . '__type_id'] = [
      '#type' => 'hidden',
      '#value' => $type_id,
    ];
    $widget['chado-' . $field_table . '__rank'] = [
      '#type' => 'hidden',
      '#value' => $rank,
    ];
  }

  /**
   * @see TripalFieldWidget::validateDefaults()
   */
  public function validateDefaults($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];

    $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'];
    $form_state['values'][$field_name]['und'][$delta]['value'] = $value;
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

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $base_table = $this->instance['settings']['base_table'];
    $lfkey_field = key($schema['foreign keys'][$base_table]['columns']);
    $rfkey_field = $schema['foreign keys'][$base_table]['columns'][$lfkey_field];

    $value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'];
    $form_state['values'][$field_name]['und'][$delta]['value'] = $value;

    // A value of zero gets set to empty when sbumitted and some prop
    // tables don't have a default value set (e.g. pubprop).
    if (!$form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank']) {
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'] = 0;
    }

    // If the user removed the property then we want to clear out the other
    // fields except the pkey value. If the pkey field is present and the value
    // is present then the chado storage backend will delete the record.
    if (!$value) {
      $form_state['values'][$field_name]['und'][$delta]['value'] = 'delete_me';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $lfkey_field] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__value'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'] = '';
    }
  }
}
