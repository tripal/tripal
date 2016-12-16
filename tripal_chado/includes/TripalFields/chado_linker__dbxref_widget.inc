<?php

class chado_linker__dbxref_widget extends TripalFieldWidget {
  // The default lable for this field.
  public static $label = 'Cross references';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__dbxref');

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

    // Get the FK column that links to the base table.
    $chado_table = $this->field['settings']['chado_table'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($chado_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = $element['#entity'] ? $element['#entity']->chado_record_id : '';
    $dbxref_id = '';
    $db_id = '';
    $accession = '';
    $version = '';
    $description = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $fkey_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $fkey, $fkey_value);
      $dbxref_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__dbxref_id', $dbxref_id);
      $db_id = tripal_get_field_item_keyval($items, $delta, 'db_id', $db_id);
      $accession = tripal_get_field_item_keyval($items, $delta, 'accession', $accession);
      $version = tripal_get_field_item_keyval($items, $delta, 'version', $version);
      $description = tripal_get_field_item_keyval($items, $delta, 'description', $description);
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
      $record_id = $form_state['values'][$field_name]['und'][$delta][$field_table . '__' . $pkey];
      $fkey_value = $form_state['values'][$field_name]['und'][$delta][$field_table . '__' . $fkey];
      $dbxref_id = $form_state['values'][$field_name]['und'][$delta][$field_table . '__dbxref_id'];
      $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
      $accession = $form_state['values'][$field_name]['und'][$delta]['accession'];
      $version = $form_state['values'][$field_name]['und'][$delta]['version'];
      $description = $form_state['values'][$field_name]['und'][$delta]['description'];
    }

    $schema = chado_get_schema('dbxref');
    $options = tripal_get_db_select_options();

    $widget['#table_name'] = $chado_table;
    $widget['#fkey_field'] = $fkey;
    //$widget['#element_validate'] = array('chado_linker__dbxref_widget_validate');
    $widget['#theme'] = 'chado_linker__dbxref_widget';
    $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id-$delta'>";
    $widget['#suffix'] =  "</span>";


    $widget['value'] = array(
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    );

    $widget['chado-' . $field_table . '__' . $pkey] = array(
      '#type' => 'value',
      '#default_value' => $record_id,
    );
    $widget['chado-' . $field_table . '__' . $fkey] = array(
      '#type' => 'value',
      '#default_value' => $fkey_value,
    );
    $widget['chado-' . $field_table . '__dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    );
    $widget['dbxref_id'] = array(
      '#type' => 'value',
      '#default_value' => $dbxref_id,
    );
    $widget['db_id'] = array(
      '#type' => 'select',
      '#title' => t('Database'),
      '#options' => $options,
      '#required' => $element['#required'],
      '#default_value' => $db_id,
      '#ajax' => array(
        'callback' => "chado_linker__dbxref_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    $widget['accession'] = array(
      '#type' => 'textfield',
      '#title' => t('Accession'),
      '#default_value' => $accession,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
      '#size' => 15,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/dbxref/' . $db_id,
      '#ajax' => array(
        'callback' => "chado_linker__dbxref_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget['version'] = array(
      '#type' => 'textfield',
      '#title' => t('Version'),
      '#default_value' => $version,
      '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
      '#size' => 5,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    $widget['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => $description,
      '#size' => 20,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
    if (!$db_id) {
      $widget['links'] = array(
        '#type' => 'item',
        '#markup' => l('Add a database', 'admin/tripal/legacy/tripal_db/add', array('attributes' => array('target' => '_blank')))
      );
    }
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
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->field['settings']['chado_table'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];


    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $dbxref_id = $values['chado-' . $field_table . '__dbxref_id'];
      $db_id = $values['db_id'];
      $accession = $values['accession'];
      $version = $values['version'];
      $description = $values['description'];

      // Make sure that if a database ID is provided that an accession is also
      // provided.  Here we use the form_set_error function rather than the
      // form_error function because the form_error will add a red_highlight
      // around all of the fields in the fieldset which is confusing as it's not
      // clear to the user what field is required and which isn't. Therefore,
      // we borrow the code from the 'form_error' function and append the field
      // so that the proper field is highlighted on error.
      if (!$db_id and $accession) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided."),
          'error' => 'chado_linker__dbxref',
        );
      }
      if ($db_id and !$accession) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided."),
          'error' => 'chado_linker__dbxref',
        );
      }
      if (!$db_id and !$accession and ($version or $description)) {
        $errors[$field_name][$delta]['und'][] = array(
          'message' => t("A database and the accession must both be provided."),
          'error' => 'chado_linker__dbxref',
        );
      }
    }
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->field['settings']['chado_table'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $this->field['settings']['base_table'];
    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field values.
    $dbxref_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__dbxref_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__dbxref_id'] : '';
    $db_id = isset($form_state['values'][$field_name][$langcode][$delta]['db_id']) ? $form_state['values'][$field_name][$langcode][$delta]['db_id'] : '';
    $accession = isset($form_state['values'][$field_name][$langcode][$delta]['accession']) ? $form_state['values'][$field_name][$langcode][$delta]['accession'] : '';
    $version = isset($form_state['values'][$field_name][$langcode][$delta]['version']) ? $form_state['values'][$field_name][$langcode][$delta]['version'] : '';
    $description = isset($form_state['values'][$field_name][$langcode][$delta]['description']) ? $form_state['values'][$field_name][$langcode][$delta]['description'] : '';

    // If the dbxref_id does not match the db_id + accession then the user
    // has selected a new dbxref record and we need to update the hidden
    // value accordingly.
    if ($db_id and $accession) {
      $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
      if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__dbxref_id'] = $dbxref->dbxref_id;
        $form_state['values'][$field_name][$langcode][$delta]['dbxref_id'] = $dbxref->dbxref_id;
      }
    }
    // If the db_id and accession are not set, then remove the linker FK
    // value to the base table, but leave the primary key so the record
    // can be deleted.
    else {
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__' . $fkey] = '';
      $form_state['values'][$field_name][$langcode][$delta]['chado-' . $table_name . '__dbxref_id'] = '';
    }
  }
}
