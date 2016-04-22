<?php
/**
 * Implements hook_info() for fields.
 *
 * This is a hook provided by the tripal_chado module for offloading the
 * hook_field_info() hook for each field to specify.
 */
function chado_base__dbxref_id_info() {
  return array(
    'label' => t('Cross reference'),
    'description' => t('This record can be cross referenced with a record in
      another online database. This field is intended for the most prominent
      reference.  At a minimum, the database and accession must be provided.'),
    'default_widget' => 'chado_base__dbxref_id_widget',
    'default_formatter' => 'chado_base__dbxref_id_formatter',
    'settings' => array(),
    'storage' => array(
      'type' => 'field_chado_storage',
      'module' => 'tripal_chado',
      'active' => TRUE
    ),
  );
}
/**
 * Implements hook_widget_info.
 *
 * This is a hook provided by the tripal_chado module for offloading
 * the hook_field_widget_info() hook for each field to specify.
 */
function chado_base__dbxref_id_widget_info() {
  return array(
    'label' => t('Cross reference'),
    'field types' => array('chado_base__dbxref_id'),
    'description' => t('This record can be cross referenced with a record in
        another online database. This field is intended for the most
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
function chado_base__dbxref_id_formatter_info() {
  return array(
    'label' => t('Database Cross Reference'),
    'field types' => array('chado_base__dbxref_id'),
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
function chado_base__dbxref_id_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  foreach ($items as $delta => $item) {
    $accession = '';
    if ($item['value']) {
      $dbxref = chado_generate_var('dbxref', array('dbxref_id' => $item['value']));
      $accession = $dbxref->db_id->name . ':' . $dbxref->accession;
      if ($dbxref->db_id->urlprefix) {
        $accession = l($accession, $dbxref->db_id->urlprefix . '/' . $dbxref->accession,
            array('attributes' => array('target' => '_blank')));
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
function chado_base__dbxref_id_widget(&$widget, $form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $field_name = $field['field_name'];

  // Get the field defaults.
  $dbxref_id = '';
  $db_id = '';
  $accession = '';
  $version = '';
  $description = '';

  // If the field already has a value then it will come through the $items
  // array.  This happens when editing an existing record.
  if (array_key_exists($delta, $items)) {
    $dbxref_id = $items[$delta]['value'];
    $db_id = $items[$delta]['dbxref__db_id'];
    $accession = $items[$delta]['dbxref__accession'];
    $version = $items[$delta]['dbxref__version'];
    $description = $items[$delta]['dbxref__description'];
  }

  // Check $form_state['values'] to see if an AJAX call set the values.
  if (array_key_exists('values', $form_state)) {
    $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_name);
    $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__db_id");
    $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__accession");
    $version = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__version");
    $description = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__description");
  }

  // If we are here because our parent was triggered in a form submit
  // then that means an ajax call was made and we don't want the fieldset to
  // be closed when it returns from the ajax call.
//   $collapsed = TRUE;
//   if (array_key_exists('triggering_element', $form_state) and
//       $form_state['triggering_element']['#parents'][0] == $field_name) {
//     $collapsed = FALSE;
//   }
//   if ($dbxref_id) {
//     $collapsed = FALSE;
//   }

  $schema = chado_get_schema('dbxref');
  $options = tripal_get_db_select_options();

  $widget['#element_validate'] = array('chado_base__dbxref_id_widget_validate');
  $widget['#theme'] = 'chado_base__dbxref_id_widget';
  $widget['#prefix'] =  "<span id='$field_name-dbxref--db-id'>";
  $widget['#suffix'] =  "</span>";

  // A temporary element used for theming the fieldset.
  $widget['#theme_settings'] = array(
    '#title' => $element['#title'],
    '#description' =>  $element['#description'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#theme' => 'chado_base__dbxref_id_widget',
    //'#collapsible' => TRUE,
    //'#collapsed' => $collapsed,
  );

  $widget['value'] = array(
    '#type' => 'value',
    '#default_value' => $dbxref_id,
  );

  $widget['dbxref__db_id'] = array(
    '#type' => 'select',
    '#title' => t('Database'),
    '#options' => $options,
    '#required' => $element['#required'],
    '#default_value' => $db_id,
    '#ajax' => array(
      'callback' => "chado_base__dbxref_id_widget_form_ajax_callback",
      'wrapper' => "$field_name-dbxref--db-id",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  $widget['dbxref__accession'] = array(
    '#type' => 'textfield',
    '#title' => t('Accession'),
    '#default_value' => $accession,
    '#required' => $element['#required'],
    '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
    '#size' => 15,
    '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/dbxref/' . $db_id,
    '#ajax' => array(
      'callback' => "tripal_chado_dbxref_widget_form_ajax_callback",
      'wrapper' => "$field_name-dbxref--db-id",
      'effect' => 'fade',
      'method' => 'replace'
    ),
    '#disabled' => $db_id ? FALSE : TRUE,
  );
  $widget['dbxref__version'] = array(
    '#type' => 'textfield',
    '#title' => t('Version'),
    '#default_value' => $version,
    '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
    '#size' => 5,
    '#disabled' => $db_id ? FALSE : TRUE,
  );
  $widget['dbxref__description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#default_value' => $description,
    '#size' => 20,
    '#disabled' => $db_id ? FALSE : TRUE,
  );
  $widget['links'] = array(
    '#type' => 'item',
    '#markup' => l('Add a new database', 'admin/tripal/chado/tripal_db/add', array('attributes' => array('target' => '_blank')))
  );
}
/**
 * An Ajax callback for the tripal_chado_admin_publish_form..
 */
function chado_base__dbxref_id_widget_form_ajax_callback($form, $form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];
  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'dbxref__db_id');
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, 'dbxref__accession');
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

  return $form[$field_name];
}

/**
 * Callback function for validating the tripal_chado_organism_select_widget.
 */
function chado_base__dbxref_id_widget_validate($element, &$form_state) {
  $field_name = $element['#parents'][0];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }

  // Get the field values.
  $dbxref_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, $field_name);
  $db_id = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__db_id");
  $accession = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__accession");
  $version = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__version");
  $description = tripal_chado_get_field_form_values($field_name, $form_state, 0, "dbxref__description");

  // Make sure that if a database ID is provided that an accession is also
  // provided.  Here we use the form_set_error function rather than the
  // form_error function because the form_error will add a red_highlight
  // around all of the fields in the fieldset which is confusing as it's not
  // clear to the user what field is required and which isn't. Therefore,
  // we borrow the code from the 'form_error' function and append the field
  // so that the proper field is highlighted on error.
  if (!$db_id and $accession) {
    form_set_error(implode('][', $element ['#parents']) . '][dbxref__db_id', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if ($db_id and !$accession) {
    form_set_error(implode('][', $element ['#parents']) . '][dbxref__accession', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if (!$db_id and !$accession and ($version or $description)) {
    form_set_error(implode('][', $element ['#parents']) . '][dbxref__db_id', t("A database and the accession must both be provided for the primary cross reference."));
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
      tripal_chado_set_field_form_values($field_name, $form_state, $dbxref->dbxref_id);
    }
  }
}
/**
 * Theme function for the dbxref_id_widget.
 *
 * @param $variables
 */
function theme_chado_base__dbxref_id_widget($variables) {
  $element = $variables['element'];

  $layout = "
    <div class=\"primary-dbxref-widget\">
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__db_id']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__accession']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__version']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
        drupal_render($element['dbxref__description']) . "
      </div>
      <div class=\"primary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
    </div>
  ";

//   $classes = array();
//   $classes[] = 'collapsible';
//   $theme_settings = $element['#theme_settings'];
//   if ($theme_settings['#collapsed'] == FALSE) {
//     $classes[] = 'collapsed';
//   }
  $fieldset = array(
    '#title' => $element['#title'],
    '#value' => '',
    '#description' => $element['#description'],
    '#children' => $layout,
//    '#attributes' => array('class' => $classes),
  );

  return theme('fieldset', array('element' => $fieldset));
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
function chado_base__dbxref_id_load($field, $entity, $base_table, $record) {

  $field_name = $field['field_name'];
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];

  // Set some defauls for the empty record
  $entity->{$field_name}['und'][0] = array(
    'value' => '',
    'dbxref__db_id' => '',
    'dbxref__accession' => '',
    'dbxref__version' => '',
    'dbxref__description' => '',
  );

  // Get the primary dbxref record (if it's not NULL).  Because we have a
  // dbxref_id passed in by the base record, we will only have one record.
  if ($record->$field_column) {
    $columns = array('*');
    $match = array('dbxref_id' => $record->$field_column->$field_column);
    $options = array('return_array' => TRUE);
    $records = chado_select_record('dbxref', $columns, $match, $options);
    if (count($records) > 0) {
      $dbxref = $records[0];
      $entity->{$field_name}['und'][0] = array(
        'value' => $dbxref->dbxref_id,
        'dbxref__db_id' => $dbxref->db_id,
        'dbxref__accession' => $dbxref->accession,
        'dbxref__version' => $dbxref->version,
        'dbxref__description' => $dbxref->description,
      );
    }
  }
}

