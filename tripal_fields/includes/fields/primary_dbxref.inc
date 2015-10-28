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
function tripal_fields_primary_dbxref_formatter(&$element, $entity_type, $entity, $field,
    $instance, $langcode, $items, $display) {

  foreach ($items as $delta => $item) {
    $accession = '';
    if ($item['value']) {
      $dbxref = chado_generate_var('dbxref', array('dbxref_id' => $item['value']));
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
function tripal_fields_primary_dbxref_widget($form, $form_state, $field, $instance, $langcode, $items, $delta, $element) {

  $widget = $element;
  $field_name = $field['field_name'];

  // Get the field defaults from the database if a record exists.
  $dbxref_id = '';
  $db_id = '';
  $accession = '';
  $version = '';
  $description = '';
  if (count($items) > 0 and $items[0]['value']) {
    $dbxref = chado_generate_var('dbxref', array('dbxref_id' => $items[0]['value']));
    $dbxref_id = $dbxref->dbxref_id;
    $db_id = $dbxref->db_id->db_id;
    $accession  = $dbxref->accession;
    $version = $dbxref->version;
    $description = $dbxref->description;
  }
  $temp = tripal_fields_get_field_form_values($field_name, $form_state, 'dbxref__db_id');
  if (count($temp) > 0) {
    $db_id = $temp[0];
  }
  // If we are here because our parent was triggered in a form submit
  // then that means an ajax call was made and we don't want the fieldset to
  // be closed when it returns from the ajax call.
  $collapsed = TRUE;
  if (array_key_exists('triggering_element', $form_state) and
      $form_state['triggering_element']['#parents'][0] == $field_name) {
    $collapsed = FALSE;
  }

  $schema = chado_get_schema('dbxref');
  $options = tripal_get_db_select_options();
  $widget +=  array(
    '#element_validate' => array('tripal_fields_primary_dbxref_widget_validate'),
    '#type' => 'fieldset',
    '#title' => $element['#title'],
    '#description' =>  $element['#description'],
    '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
    '#delta' => $delta,
    '#theme' => 'tripal_fields_primary_dbxref_widget',
    '#group' => 'entity_vetical_tabs',
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
    array(
      $element['#field_name'] => array(
        '#type' => 'hidden',
        '#default_value' => $dbxref_id,
      ),
      'dbxref__db_id' => array(
        '#type' => 'select',
        '#title' => t('Database'),
        '#options' => $options,
        '#required' => $element['#required'],
        '#default_value' => $db_id,
        '#ajax' => array(
          'callback' => "tripal_fields_primary_dbxref_widget_form_ajax_callback",
          'wrapper' => "$field_name-dbxref--db-id",
          'effect' => 'fade',
          'method' => 'replace'
        )
      ),
      'dbxref__accession' => array(
        '#type' => 'textfield',
        '#title' => t('Accession'),
        '#default_value' => $accession,
        '#required' => $element['#required'],
        '#maxlength' => array_key_exists('length', $schema['fields']['accession']) ? $schema['fields']['accession']['length'] : 255,
        '#size' => 15,
        '#autocomplete_path' => "admin/tripal/chado/tripal_db/dbxref/auto_name/$db_id",
        '#ajax' => array(
          'callback' => "tripal_fields_primary_dbxref_widget_form_ajax_callback",
          'wrapper' => "$field_name-dbxref--db-id",
          'effect' => 'fade',
          'method' => 'replace'
        )
      ),
      'dbxref__version' => array(
        '#type' => 'textfield',
        '#title' => t('Version'),
        '#default_value' => $version,
        '#maxlength' => array_key_exists('length', $schema['fields']['version']) ? $schema['fields']['version']['length'] : 255,
        '#size' => 5,
      ),
      'dbxref__description' => array(
        '#type' => 'textfield',
        '#title' => t('Description'),
        '#default_value' => $description,
        '#size' => 20,
      ),
      'links' => array(
        '#type' => 'item',
        '#markup' => l('Add a new database', 'admin/tripal/chado/tripal_db/add', array('attrbutes', array('target' => '_blank')))
      )
    ),
    '#prefix' => "<span id='$field_name-dbxref--db-id'>",
    '#suffix' => "</span>"
  );
  return $widget;
}
/**
 * An Ajax callback for the tripal_fields_admin_publish_form..
 */
function tripal_fields_primary_dbxref_widget_form_ajax_callback($form, $form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];
  $db_id = tripal_fields_get_field_form_values($field_name, $form_state, 'dbxref__db_id');
  $accession = tripal_fields_get_field_form_values($field_name, $form_state, 'dbxref__accession');
  if (count($db_id) > 0 and count($accession) > 0) {
    $values = array(
      'db_id' => $db_id[0],
      'accession' => $accession[0],
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
 * Callback function for validating the tripal_fields_organism_select_widget.
 */
function tripal_fields_primary_dbxref_widget_validate($element, &$form_state) {
  $field_name = $element['#field_name'];

  // If the form ID is field_ui_field_edit_form, then the user is editing the
  // field's values in the manage fields form of Drupal.  We don't want
  // to validate it as if it were being used in a data entry form.
  if ($form_state['build_info']['form_id'] =='field_ui_field_edit_form') {
    return;
  }

  // Get the field values.
  $db_id = tripal_fields_get_field_form_values($field_name, $form_state, "dbxref__db_id");
  $accession = tripal_fields_get_field_form_values($field_name, $form_state, "dbxref__accession");
  $version = tripal_fields_get_field_form_values($field_name, $form_state, "dbxref__version");
  $description = tripal_fields_get_field_form_values($field_name, $form_state, "dbxref__description");

  // Make sure that if a database ID is provided that an accession is also
  // provided.  Here we use the form_set_error function rather than the
  // form_error function because the form_error will add a red_highlight
  // around all of the fields in the fieldset which is confusing as it's not
  // clear to the user what field is required and which isn't. Therefore,
  // we borrow the code from the 'form_error' function and append the field
  // so that the proper field is highlighted on error.
  if (count($db_id) == 0 and count($accession) > 0) {
    form_set_error(implode('][', $element ['#parents']) . '][0][dbxref__db_id', t("A database and the accession must both be provided for the primary cross reference."));
  }
  if (count($db_id) > 0 and count($accession) == 0) {
    form_set_error(implode('][', $element ['#parents']) . '][0][dbxref__accession', t("A database and the accession must both be provided for the primary cross reference."));
  }

  // If user did not select a database, we want to remove dbxref_id from the
  // field.
  if (count($db_id) == 0) {
    tripal_fields_set_field_form_values($field_name, $form_state, '__NULL__', $field_name);
  }
}
/**
 * Theme function for the primary_dbxref_widget.
 *
 * @param $variables
 */
function theme_tripal_fields_primary_dbxref_widget($variables) {
  $element = $variables['element'];
  $layout = "
    <div class=\"primary-dbxref-widget\">
      <div class=\"primary-dbxref-widget-item\">" .
      drupal_render($element[0]['dbxref__db_id']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
      drupal_render($element[0]['dbxref__accession']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
      drupal_render($element[0]['dbxref__version']) . "
      </div>
      <div class=\"primary-dbxref-widget-item\">" .
      drupal_render($element[0]['dbxref__description']) . "
      </div>
      <div class=\"primary-dbxref-widget-links\">" . drupal_render($element[0]['links']) . "</div>
    </div>
  ";
      return $layout;
}