<?php
/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_linker__dbxref_info() {
  return array(
    'label' => t('Cross references'),
    'description' => t('This record can be cross referenced with a record in
        another online database. This field is intended for one or more
        references.  At a minimum, the database and accession must be provided.'),
    'default_widget' => 'chado_linker__dbxref_widget',
    'default_formatter' => 'chado_linker__dbxref_formatter',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_attach_info().
 *
 * This is a hook provided by the tripal_Chado module. It allows the field
 * to specify which bundles it will attach to and to specify thee settings.
 *
 * @param $entity_type
 * @param $entity
 * @param $term
 *
 * @return
 *   A field array
 */
function chado_linker__dbxref_attach_info($entity_type, $bundle, $target) {
  $field_info = array();

  $table_name = $target['data_table'];
  $type_table = $target['type_table'];
  $type_field = $target['field'];
  $cv_id      = $target['cv_id'];
  $cvterm_id  = $target['cvterm_id'];


  // If the linker table does not exists then we don't want to add attach.
  $dbxref_table = $table_name . '_dbxref';
  if (!chado_table_exists($dbxref_table)) {
    return $field_info;
  }

  // We already have a dbxref_id field.
  $schema = chado_get_schema($dbxref_table);
  $pkey = $schema['primary key'][0];

  // Initialize the field array.
  $field_info = array(
    'field_name' => $dbxref_table,
    'field_type' => 'chado_linker__dbxref',
    'widget_type' => 'chado_linker__dbxref_widget',
    'widget_settings' => array('display_label' => 1),
    'description' => '',
    'label' => 'Cross References',
    'is_required' => 0,
    'cardinality' => FIELD_CARDINALITY_UNLIMITED,
    'storage' => 'field_chado_storage',
    'field_settings' => array(
      'chado_table' => $dbxref_table,
      'chado_column' => $pkey,
      'base_table' => $table_name,
      'semantic_web' => array(
        'type' => '',
        'ns' => '',
        'nsurl' => '',
      ),
    ),
  );

  // If the base table has a 'dbxref_id' then change the label to
  // indicate these are secondary cross references.
  $schema = chado_get_schema($table_name);
  if (array_key_exists('dbxref_id', $schema['fields'])) {
    $field_info['label'] = 'Secondary Cross References';
  }

  return $field_info;
}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_linker__dbxref_widget_info() {
  return array(
    'label' => t('Cross references'),
    'field types' => array('chado_linker__dbxref'),
    'description' => t('This record can be cross referenced with a record
        in another online database. This field is intended for the most
        prominent reference.  At a minimum, the database and accession
        must be provided.'),
  );
}
/**
 * Implements hook_formatter_info.
 *
 * This is a hook provided by the tripal_chado module for
 * offloading the hook_field_formatter_info() for each field
 * to specify.
 *
 */
function chado_linker__dbxref_formatter_info() {
  return array(
    'label' => t('Cross references'),
    'field types' => array('chado_linker__dbxref'),
    'settings' => array(
    ),
  );
}
/**
 *
 * @param unknown $entity_type
 * @param unknown $entity
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $display
 */
function chado_linker__dbxref_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  $chado_table = $field['settings']['chado_table'];
  foreach ($items as $delta => $item) {
    $accession = '';
    if ($item[$chado_table . '__dbxref_id']) {
      $dbxref = chado_generate_var('dbxref', array('dbxref_id' => $item[$chado_table . '__dbxref_id']));
      $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
      if ($dbxref->db_id->urlprefix) {
        $accession = l($accession, $dbxref->db_id->urlprefix . '/' . $dbxref->accession, array('attributes' => array('target' => '_blank')));
      }
    }
    $element[$delta] = array(
      '#type' => 'markup',
      '#markup' => $accession,
    );
  }
}
/**
 *
 * @param unknown $field_name
 * @param unknown $widget
 * @param unknown $form
 * @param unknown $form_state
 * @param unknown $field
 * @param unknown $instance
 * @param unknown $langcode
 * @param unknown $items
 * @param unknown $delta
 * @param unknown $element
 */
function chado_linker__dbxref_widget(&$widget, $form, $form_state, $field,
    $instance, $langcode, $items, $delta, $element) {

  $field_name = $field['field_name'];

  // Get the FK column that links to the base table.
  $chado_table = $field['settings']['chado_table'];
  $base_table = $field['settings']['base_table'];
  $schema = chado_get_schema($chado_table);
  $pkey = $schema['primary key'][0];
  $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
  $fkey = $fkeys[0];

  // Get the field defaults.
  $record_id = '';
  $fkey_value = '';
  $dbxref_id = '';
  $db_id = '';
  $accession = '';
  $version = '';
  $description = '';

  // If the field already has a value then it will come through the $items
  // array.  This happens when editing an existing record.
  if (array_key_exists($delta, $items)) {
    $record_id = $items[$delta]['value'];
    $fkey_value = $items[$delta][$field_name . '__' . $fkey];
    $dbxref_id = $items[$delta][$field_name . '__dbxref_id'];
    $db_id = $items[$delta][$field_name . '--dbxref__db_id'];
    $accession = $items[$delta][$field_name . '--dbxref__accession'];
    $version = $items[$delta][$field_name . '--dbxref__version'];
    $description = $items[$delta][$field_name . '--dbxref__description'];
  }

  // Check $form_state['values'] to see if an AJAX call set the values.
  if (array_key_exists('values', $form_state) and array_key_exists($delta, $form_state['values'])) {
    $record_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name);
    $fkey_value = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__' . $fkey);
    $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__dbxref_id');
    $db_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__db_id');
    $accession = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__accession');
    $version = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__version');
    $description = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__description');
  }

  $schema = chado_get_schema('dbxref');
  $options = tripal_get_db_select_options();

  $widget['#table_name'] = $chado_table;
  $widget['#fkey_field'] = $fkey;
  $widget['#element_validate'] = array('chado_linker__dbxref_widget_validate');
  $widget['#theme'] = 'chado_linker__dbxref_widget';
  $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id-$delta'>";
  $widget['#suffix'] =  "</span>";

  $widget['value'] = array(
    '#type' => 'value',
    '#default_value' => $record_id,
  );
  $widget[$field_name . '__dbxref_id'] = array(
    '#type' => 'value',
    '#default_value' => $dbxref_id,
  );
  $widget[$field_name . '__' . $fkey] = array(
    '#type' => 'value',
    '#default_value' => $fkey_value,
  );
  $widget[$field_name . '--dbxref__dbxref_id'] = array(
    '#type' => 'value',
    '#default_value' => $dbxref_id,
  );
  $widget[$field_name . '--dbxref__db_id'] = array(
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
  $widget[$field_name . '--dbxref__accession'] = array(
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
  $widget[$field_name . '--dbxref__version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version'),
    '#default_value' => $version,
    '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
    '#size' => 5,
    '#disabled' => $db_id ? FALSE : TRUE,
  );
  $widget[$field_name . '--dbxref__description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#default_value' => $description,
    '#size' => 20,
    '#disabled' => $db_id ? FALSE : TRUE,
  );
  if (!$db_id) {
    $widget['links'] = array(
      '#type' => 'item',
      '#markup' => l('Add a database', 'admin/tripal/chado/tripal_db/add', array('attributes' => array('target' => '_blank')))
    );
  }
}
/**
 * An Ajax callback for the dbxref widget.
 */
function chado_linker__dbxref_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  // Check to see if this dbxref already exists. If not then
  // give a notice to the user that the dbxref will be added.
  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__db_id');
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__accession');
  if ($db_id and $accession) {
    $values = array(
      'db_id' => $db_id,
      'accession' => $accession,
    );
    $options = array('is_duplicate' => TRUE);
    $has_duplicate = chado_select_record('dbxref', array('*'), $values, $options);
    if (!$has_duplicate) {
      drupal_set_message('The selected cross reference is new and will be added for future auto completions.');
    }
  }

  return $form[$field_name]['und'][$delta];
}

/**
 * Callback function for validating the chado_linker__dbxref_widget.
 */
function chado_linker__dbxref_widget_validate($element, &$form_state) {

  $field_name = $element['#field_name'];
  $delta = $element['#delta'];
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }

  // Get the field values.
  $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__dbxref_id');
  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__db_id');
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__accession');
  $version = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__version');
  $description = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__description');

  // Make sure that if a database ID is provided that an accession is also
  // provided.  Here we use the form_set_error function rather than the
  // form_error function because the form_error will add a red_highlight
  // around all of the fields in the fieldset which is confusing as it's not
  // clear to the user what field is required and which isn't. Therefore,
  // we borrow the code from the 'form_error' function and append the field
  // so that the proper field is highlighted on error.
  if (!$db_id and $accession) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__db_id', t("A database and the accession must both be provided."));
  }
  if ($db_id and !$accession) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__accession', t("A database and the accession must both be provided."));
  }
  if (!$db_id and !$accession and ($version or $description)) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__db_id', t("A database and the accession must both be provided."));
  }

  // If the dbxref_id does not match the db_id + accession then the user
  // has selected a new dbxref record and we need to update the hidden
  // value accordingly.
  if ($db_id and $accession) {
    $fkey_value = $element['#entity']->chado_record_id;
    tripal_chado_set_field_form_values($field_name, $form_state, $fkey_value, $delta, $table_name . '__' . $fkey);

    $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
    if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, $delta, $table_name . '__dbxref_id');
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, $delta, $table_name . '--dbxref__dbxref_id');
    }

  }
  else {
    // If the db_id and accession are not set, then remove the linker FK value to the base table.
    tripal_chado_set_field_form_values($field_name, $form_state, '', $delta, $table_name . '__' . $fkey);
  }
}
/**
 * Theme function for the dbxref_id_widget.
 *
 * @param $variables
 */
function theme_chado_linker__dbxref_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
    <div class=\"secondary-dbxref-widget\">
      <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__db_id']) . "
      </div>
      <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__accession']) . "
      </div>
      <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__version']) . "
      </div>
      <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element[$table_name . '--dbxref__description']) . "
      </div>
      <div class=\"secondary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
    </div>
  ";

  return $layout;
}

/**
 * Loads the field values with appropriate data.
 *
 * This function is called by the tripal_chado_field_storage_load() for
 * each property managed by the field_chado_storage storage type.  This is
 * an optional hook function that is only needed if the field has
 * multiple form elements.
 *
 * @param $field
 * @param $entity
 * @param $base_table
 * @param $record
 */
function chado_linker__dbxref_load($field, $entity, $base_table, $record) {

  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $chado_table = $field['settings']['chado_table'];
  $schema = chado_get_schema($chado_table);
  $pkey = $schema['primary key'][0];
  $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
  $fkey = $fkeys[0];

  // Set some defaults for the empty record.
  $entity->{$field_name}['und'][0] = array(
    'value' => '',
    $chado_table . '__' . $fkey => '',
    $chado_table . '__' . 'dbxref_id' => '',
    $chado_table . '--' . 'dbxref__dbxref_id' => '',
    $chado_table . '--' . 'dbxref__db_id' => '',
    $chado_table . '--' . 'dbxref__accession' => '',
    $chado_table . '--' . 'dbxref__version' => '',
    $chado_table . '--' . 'dbxref__description' => '',
  );

  $linker_table = $base_table . '_dbxref';
  $options = array('return_array' => 1);
  $record = chado_expand_var($record, 'table', $linker_table, $options);
  if (count($record->$linker_table) > 0) {
    $i = 0;
    foreach ($record->$linker_table as $index => $linker) {
      $dbxref = $linker->dbxref_id;
      $entity->{$field_name}['und'][$i] = array(
        'value' => $linker->$pkey,
        $chado_table . '__' . $fkey => $linker->$fkey->$fkey,
        $chado_table . '__' . 'dbxref_id' => $dbxref->dbxref_id,
        $chado_table . '--' . 'dbxref__dbxref_id' => $dbxref->dbxref_id,
        $chado_table . '--' . 'dbxref__db_id' => $dbxref->db_id->db_id,
        $chado_table . '--' . 'dbxref__accession' => $dbxref->accession,
        $chado_table . '--' . 'dbxref__version' => $dbxref->version,
        $chado_table . '--' . 'dbxref__description' => $dbxref->description,
      );
      $i++;
    }
  }
}

/**
 * Implements hook_ws_formatter().
 */
function chado_linker__dbxref_ws_formatter($entity_type, $entity,
    $field, $instance, $items) {

  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $chado_table = $field['settings']['chado_table'];
  $record = $entity->chado_record;

  $values = array();
  $options = array('return_array' => 1);
  $record = chado_expand_var($record, 'table', $chado_table, $options);
  $linker_dbxrefs = $record->$chado_table;
  foreach ($linker_dbxrefs as $linker_dbxref) {
    $dbxref = $linker_dbxref->dbxref_id;
    unset($dbxref->tablename);
    unset($dbxref->db_id->tablename);
    $values[] = $dbxref;
  }

  return $values;
}