<?php
/**
 * @file
 * Contains functions for administration of tripal field settings.
 *
 */

/**
 * Provides the field settings form.
 */
function tripal_chado_field_settings_admin_form($form, $form_state) {

  $form['general'] = [
    '#type' => 'fieldset',
    '#title' => 'Field Display Options'
  ];

  $form['general']['max_delta']= [
    '#type' => 'textfield',
    '#title' => t('Maximum records to display'),
    '#size' => 20,
    '#description' => t('This option controls the maximum number of records'
       . ' to display in a tripal field. If more than this number of records'
       . ' are stored in chado, the field will display a summary view.'),
    '#default_value' => variable_get('chado_linker_load_limit', 500),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save Configuration'
  ];

  return $form;
}

/**
 * Validate the field settings form.
 */
function tripal_chado_field_settings_admin_form_validate($form, &$form_state) {

  $max_delta = $form_state['values']['max_delta'];
  if (!preg_match('/^\d+$/', $max_delta) or ($max_delta < 1)) {
    form_set_error('max_delta', 'Please enter a positive integer.');
  }
}

/**
 * Submit the field settings form.
 */
function tripal_chado_field_settings_admin_form_submit($form, $form_state) {

  variable_set('chado_linker_load_limit', $form_state['values']['max_delta']);
}
