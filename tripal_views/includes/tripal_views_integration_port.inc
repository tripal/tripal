<?php
/**
 * @file
 * This file contains the UI to import/export tripal views integration setups
 * between sites
 */

/**
 * Form: The form to export a particular tripal views integration
 *
 * No submit is needed since the setup_id is in the path and the export code is
 * rendered based on that
 *
 * @param $form_state
 *   The state of the form
 * @param $setup_id
 *   The tripal views integration setup id
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_export_form($form_state, $setup_id) {
  $form = array();

  $defn_array = tripal_views_integration_export_entry($setup_id);

  $t = var_export($defn_array, TRUE);
  $t = preg_replace("/\n\s+array/", "array", $t); // move array( to previous line
  $t = preg_replace("/true/", "TRUE", $t); // upper case true
  $t = preg_replace("/false/", "FALSE", $t); // upper case false
  $t = preg_replace("/array\(/", "array (", $t); // put a space between array and paren

  $form['export'] = array(
    '#type' => 'textarea',
    '#title' => 'Export',
    '#description' => t('Simply copy the provided export into the Import text area on '
      . 'another tripal views enabled website to port the integration between websites.'),
    '#rows' => 20,
    '#value' => $t,
  );

  return $form;
}

/**
 * Form: Imports a tripal views integration
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_import_form() {
  $form = array();

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Name (optional)',
    '#description' => t('A human-readable name for your integration.')
  );

  $priorities = array();
  foreach (range(-10, 10) as $v) {
    $priorities[$v] = (string) $v;
  }
  $form['views_type']['row_priority'] = array(
    '#type' => 'select',
    '#title' => t('Priority (optional)'),
    '#description' => t('The level of priority your Views integration has in relation to the '
      .'default core and module definitions. The views integration definition with the '
      .'lightest priority will be used. For example, if there is a definition created by '
      .'core with a priority of 10 and another by a custom module of 5 and yours is -1 then '
      .'you definition will be used for that table because -1 is lighter then both 5 and 10.'),
    '#options' => $priorities,
    '#default_value' => -1,
  );

  $form['import'] = array(
    '#type' => 'textarea',
    '#title' => 'Import',
    '#description' => t('Simply copy the provided export into the text area to port the integration between websites.'),
    '#rows' => 20,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit'
  );

  return $form;
}

/**
 * Submit: Imports a tripal views integration
 *
 * @param $form
 * @param $form_state
 *
 * @ingroup tripal_views
 */
function tripal_views_integration_import_form_submit($form, &$form_state) {

  //$defn_array = unserialize($form_state['values']['import']);
  // convert the array into a real PHP array
  $defn_array = array();
  eval("\$defn_array = " . $form_state['values']['import'] . ";");

  // Add optional parameters
  if ($form_state['values']['name']) {
    $defn_array['name'] = $form_state['values']['name'];
  }
  if ($form_state['values']['row_priority']) {
    $defn_array['priority'] = $form_state['values']['row_priority'];
  }

  // Add the views integration
  $success = tripal_views_integration_add_entry($defn_array);
  if ($success) {
    drupal_set_message(t("Successfully imported %name Integration", array('%name' => $defn_array['name'])));
  }
  else {
    drupal_set_message(t("Unable to import %name Integration", array('%name' => $defn_array['name'])), 'error');
  }
}
