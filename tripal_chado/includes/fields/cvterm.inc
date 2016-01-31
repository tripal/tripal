<?php

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
function tripal_chado_cvterm_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

/*   $chado_table = $field['settings']['chado_table'];
  foreach ($items as $delta => $item) {
    $accession = '';
    if ($item[$chado_table . '__cvterm_id']) {
      $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $item[$chado_table . '__cvterm_id']));
      $dbxref = $cvterm->dbxref_id;
      $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
      if ($dbxref->db_id->urlprefix) {
        $accession = l($accession, $dbxref->db_id->urlprefix . '/' . $dbxref->accession, array('attributes' => array('target' => '_blank')));
      }
    }
    $element[$delta] = array(
      '#type' => 'markup',
      '#markup' => $accession,
    );
  } */
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
function tripal_chado_cvterm_widget(&$widget, $form, $form_state, $field,
    $instance, $langcode, $items, $delta, $element) {

  $field_name = $field['field_name'];

  // Get the FK column that links to the base table.
  $chado_table = $field['settings']['chado_table'];
  $base_table = $field['settings']['base_table'];
  $schema = chado_get_schema($chado_table);
  $pkey = $schema['primary key'][0];
  $fkey = array_values($schema['foreign keys'][$base_table]['columns'])[0];

  // Get the field defaults.
  $record_id = '';
  $fkey_value = $element['#entity']->chado_record_id;
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
  if (array_key_exists('values', $form_state)) {
    $record_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name);
    $fkey_value = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__' . $fkey);
    $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '__dbxref_id');
    $db_id = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__db_id');
    $accession = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__accession');
    $version = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__version');
    $description = tripal_chado_get_field_form_values($field_name, $form_state, $delta, $field_name . '--dbxref__description');
  }

  // If we are here because our parent was triggered in a form submit
  // then that means an ajax call was made and we don't want the fieldset to
  // be closed when it returns from the ajax call.
  $collapsed = TRUE;
  if (array_key_exists('triggering_element', $form_state) and
      $form_state['triggering_element']['#parents'][0] == $field_name) {
    $collapsed = FALSE;
  }
  if ($dbxref_id) {
    $collapsed = FALSE;
  }

  $schema = chado_get_schema('dbxref');
  $options = tripal_get_db_select_options();

  $widget['#table_name'] = $chado_table;
  $widget['#fkey_field'] = $fkey;
  $widget['#element_validate'] = array('tripal_chado_cvterm_widget_validate');
  $widget['#theme'] = 'tripal_chado_cvterm_widget';
  $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id-$delta'>";
  $widget['#suffix'] =  "</span>";

  // A temporary element used for theming the fieldset.
  $widget['#theme_settings'] = array(
    '#title' => $element['#title'],
    '#description' =>  $element['#description'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#theme' => 'tripal_chado_cvterm_widget',
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

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
      'callback' => "tripal_chado_cvterm_widget_form_ajax_callback",
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
      'callback' => "tripal_chado_cvterm_widget_form_ajax_callback",
      'wrapper' => "$field_name-dbxref--db-id-$delta",
      'effect' => 'fade',
      'method' => 'replace'
    )
  );
  $widget[$field_name . '--dbxref__version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version'),
    '#default_value' => $version,
    '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
    '#size' => 5,
  );
  $widget[$field_name . '--dbxref__description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#default_value' => $description,
    '#size' => 20,
  );
  $widget['links'] = array(
    '#type' => 'item',
    '#markup' => l('Add a new database', 'admin/tripal/chado/tripal_db/add', array('attributes' => array('target' => '_blank')))
  );
}
/**
 * An Ajax callback for the dbxref widget.
 */
function tripal_chado_cvterm_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];
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
 * Callback function for validating the tripal_chado_organism_select_widget.
 */
function tripal_chado_cvterm_widget_validate($element, &$form_state) {

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
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__db_id', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if ($db_id and !$accession) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__accession', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if (!$db_id and !$accession and ($version or $description)) {
    form_set_error(implode('][', $element ['#parents']) . '][' . $table_name . '--dbxref__db_id', t("A database and the accession must both be provided for the primary cross reference."));
  }

  // If user did not select a database, we want to remove dbxref_id from the
  // field.
  if (!$db_id) {
    tripal_chado_set_field_form_values($field_name, $form_state, '__NULL__');
  }
  // If the dbxref_id does not match the db_id + accession then the user
  // has selected a new dbxref record and we need to update the hidden
  // value accordingly.
  if ($db_id and $accession) {
    $dbxref = chado_generate_var('dbxref', array('db_id' => $db_id, 'accession' => $accession));
    if ($dbxref and $dbxref->dbxref_id != $dbxref_id) {
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, $delta, $table_name . '__dbxref_id');
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id, $delta, $table_name . '--dbxref__dbxref_id');
    }
  }
}
/**
 * Theme function for the dbxref_id_widget.
 *
 * @param $variables
 */
function theme_tripal_chado_cvterm_widget($variables) {
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
function tripal_chado_cvterm_field_load($field, $entity, $base_table, $record) {

  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $chado_table = $field['settings']['chado_table'];
  $schema = chado_get_schema($chado_table);
  $pkey = $schema['primary key'][0];
  $fkey = array_values($schema['foreign keys'][$base_table]['columns'])[0];

  // Set some defaults for the empty record.
  $entity->{$field_name}['und'][0] = array(
    'value' => '',
    $chado_table . '__' . $fkey => '',
    $chado_table . '__' . 'cvterm_id' => '',
    $chado_table . '__' . 'pub_id' => '',
    $chado_table . '__' . 'is_not' => '',
    $chado_table . '__' . 'rank' => '',
    $chado_table . '--' . 'cvterm__name' => '',
    $chado_table . '--' . 'cvterm__cvterm_id' => '',
  );

  $linker_table = $base_table . '_cvterm';
  /* $options = array('return_array' => 1);
  $record = chado_expand_var($record, 'table', $linker_table, $options);
  $i = 0;
  foreach ($record->$linker_table as $index => $linker) {
    $cvterm = $linker->cvterm_id;
    $entity->{$field_name}['und'][$i] = array(
      'value' => $linker->$pkey,
      $chado_table . '__' . $fkey => $linker->$fkey->$fkey,
      $chado_table . '__' . 'cvterm_id' => $linker->cvterm_id->cvterm_id,
      $chado_table . '__' . 'pub_id' => property_exists($linker, 'pub_id') ? $linker->pub_id : '',
      $chado_table . '__' . 'is_not' => property_exists($linker, 'is_not') ? $linker->is_not : '',
      $chado_table . '__' . 'rank' => property_exists($linker, 'rank') ? $linker->rank : '',
      $chado_table . '__' . 'cvterm_type_id' => property_exists($linker, 'cvterm_type_id') ? $linker->cvterm_type_id : '',
      $chado_table . '--' . 'cvterm__name' => $cvterm->name,
      $chado_table . '--' . 'cvterm__cvterm_id' => $cvterm->cvterm_id,
    );
    $i++;
  } */
}

