<?php

class sbo__database_cross_reference_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Cross reference';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('sbo__database_cross_reference');

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
    $base_table = $this->instance['settings']['base_table'];

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field defaults.
    $record_id = '';
    $fkey_value = $element['#entity'] ? $element['#entity']->chado_record_id : '';
    $dbxref_id = '';
    $db_id = '';
    $accession = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0 and array_key_exists($delta, $items)) {
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $fkey_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $fkey, $fkey_value);
      $dbxref_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__dbxref_id', $dbxref_id);
      $db_id = tripal_get_field_item_keyval($items, $delta, 'db_id', $db_id);
      $accession = tripal_get_field_item_keyval($items, $delta, 'accession', $accession);
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state)) {
      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey];
      $fkey_value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey];
      $dbxref_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'];
      $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
      $accession = $form_state['values'][$field_name]['und'][$delta]['accession'];
    }

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
    $options = tripal_get_db_select_options();
    $widget['db_id'] = array(
      '#type' => 'select',
      '#title' => t('Database'),
      '#options' => $options,
      '#required' => $element['#required'],
      '#default_value' => $db_id,
      '#ajax' => array(
        'callback' => "sbo__database_cross_reference_widget_form_ajax_callback",
        'wrapper' => "$field_name-dbxref--db-id-$delta",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    $schema = chado_get_schema('dbxref');
    $widget['accession'] = array(
      '#type' => 'textfield',
      '#title' => t('Accession'),
      '#default_value' => $accession,
      '#required' => $element['#required'],
      '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
      '#size' => 15,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/dbxref/' . $db_id,
      '#disabled' => $db_id ? FALSE : TRUE,
    );
  }

  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $table_name = $this->instance['settings']['chado_table'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $schema = chado_get_schema($table_name);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];

    // Get the field values.
    $dbxref_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'];
    $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
    $accession = $form_state['values'][$field_name]['und'][$delta]['accession'];

    // If user did not select a database, we want to remove the dbxref record.
    // We do this by setting all values to empty except the value and the
    // primary key.
    if (!$db_id) {
      $form_state['values'][$field_name]['und'][$delta]['value'] = 'delete_me';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'] = '';
    }
    // If the dbxref_id does not match the db_id + accession then the user
    // has selected a new dbxref record and we need to update the hidden
    // value accordingly.
    if ($db_id and $accession) {
      $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
      if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__dbxref_id'] = $dbxref->dbxref_id;
        $form_state['values'][$field_name]['und'][$delta]['value'] = $dbxref->dbxref_id;
      }
    }
  }

  /**
   * @see TripalFieldWidget::theme()
   */
  public function theme($element) {

    $layout = "
      <div class=\"secondary-dbxref-widget\">
        <div class=\"secondary-dbxref-widget-item\">" .
          drupal_render($element['db_id']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
          drupal_render($element['accession']) . "
        </div>
      </div>
    ";

    return $layout;
  }
}


/**
 * An Ajax callback for the dbxref widget.
 */
function sbo__database_cross_reference_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];

  return $form[$field_name]['und'][$delta];
}
