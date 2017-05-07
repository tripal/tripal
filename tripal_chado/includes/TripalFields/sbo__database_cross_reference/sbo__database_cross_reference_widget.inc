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

    // Get the FK column that links to the base table.
    $chado_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
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
    //$widget['#element_validate'] = array('sbo__database_cross_reference_widget_validate');
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
        'callback' => "sbo__database_cross_reference_widget_form_ajax_callback",
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
        'callback' => "sbo__database_cross_reference_widget_form_ajax_callback",
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
        '#markup' => l('Add a database', 'admin/tripal/loaders/chado_db/add', array('attributes' => array('target' => '_blank')))
      );
    }
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
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
    $dbxref_id = isset($form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__dbxref_id']) ? $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__dbxref_id'] : '';
    $db_id = isset($form_state['values'][$field_name][$langcode][$delta]['db_id']) ? $form_state['values'][$field_name][$langcode][$delta]['db_id'] : '';
    $accession = isset($form_state['values'][$field_name][$langcode][$delta]['accession']) ? $form_state['values'][$field_name][$langcode][$delta]['accession'] : '';
    $version = isset($form_state['values'][$field_name][$langcode][$delta]['version']) ? $form_state['values'][$field_name][$langcode][$delta]['version'] : '';
    $description = isset($form_state['values'][$field_name][$langcode][$delta]['description']) ? $form_state['values'][$field_name][$langcode][$delta]['description'] : '';


    // If the dbxref_id does not match the db_id + accession then the user
    // has selected a new dbxref record and we need to update the hidden
    // value accordingly.
    if ($db_id and $accession) {
      // If the dbxref doesn't exist then let's add it.
      $values = array(
        'db_id' => $db_id,
        'accession' => $accession,
      );
      $options = array('is_duplicate' => TRUE);
      $has_duplicate = chado_select_record('dbxref', array('*'), $values, $options);
      if (!$has_duplicate) {
        tripal_insert_dbxref(array(
          'db_id' => $db_id,
          'accession' => $accession,
        ));
      }
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

  /**
   * @see TripalFieldWidget::theme()
   */
  public function theme($element) {

    // These two fields were added to the widget to help identify the fields
    // for layout.
    $table_name = $element['#table_name'];
    $fkey = $element['#fkey_field'];

    $layout = "
      <div class=\"secondary-dbxref-widget\">
        <div class=\"secondary-dbxref-widget-item\">" .
          drupal_render($element['db_id']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
          drupal_render($element['accession']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
          drupal_render($element['version']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
          drupal_render($element['description']) . "
        </div>
        <div class=\"secondary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
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
  $delta = $form_state['triggering_element']['#parents'][2];
  $field = field_info_field($field_name);
  $field_type = $field['type'];
  $instance = $form[$field_name]['und'][$delta]['#instance'];
  $field_table = $instance['settings']['chado_table'];
  $field_column = $instance['settings']['chado_column'];
  $field_prefix = 'chado-' . $field_table . '__dbxref_id';

  // Check to see if this dbxref already exists. If not then
  // give a notice to the user that the dbxref will be added.
  $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
  $accession = $form_state['values'][$field_name]['und'][$delta]['accession'];
  if ($db_id and $accession) {
    $values = array(
      'db_id' => $db_id,
      'accession' => $accession,
    );
    $options = array('is_duplicate' => TRUE);
    $has_duplicate = chado_select_record('dbxref', array('*'), $values, $options);
    if (!$has_duplicate) {
      drupal_set_message('The selected cross reference is new and will be added.');
    }
  }

  return $form[$field_name]['und'][$delta];
}
