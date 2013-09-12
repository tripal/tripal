<?php
/**
 * @file
 * The Tripal AJAX/AHAH API
 *
 * This file provides the API to help Tripal modules more easily use 
 * Drupal's AHAH functionality with forms.
 */

/**
 * @defgroup tripal_ahah_api AHAH API
 * @ingroup tripal_core_api
 * @{
 * Provides an application programming interface (API) for improved 
 * support of Drupal's AHAH functionality.
 *
 * @} 
 */

/**
 * This function should be called to initialize the page
 * for use of AHAH elements in a form.
 * 
 * See http://tripal.info/documentation/ahah_api for example usage
 *
 * @returns
 *   nothing
 *
 * @ingroup tripal_ahah_api
 */
function tripal_core_ahah_init_form() {
  // If form elements have autocomplete elements returned in
  // an ahah call they won't work because the following JS file
  // doesn't get included. If we include it first before any 
  // ahah calls it will be there when we need it.
  drupal_add_js('misc/autocomplete.js');
}

/**
 * This function simply gets the posted form ID, builds the form
 * and retrieves the specified element.  This function should be
 * called in an AHAH callback function to retrieve the form
 *
 * See http://tripal.info/documentation/ahah_api for example usage
 *
 * @returns
 *   A Drupal form array.  If no form is available then FALSE is returned
 *
 * @ingroup tripal_ahah_api
 */
function tripal_core_ahah_prepare_form(&$form_state = array()) {
  
  // Retrieve the form from the cache
  $form_state['storage'] = NULL;
  $form_build_id = filter_xss($_POST['form_build_id']);
  if (!$form_build_id) {
    return FALSE;
  }
  $form = form_get_cache($form_build_id, $form_state);

  // Preparing to process the form
  $args = $form['#parameters']; 
  if (!is_array($args)) {
    // if there is no form #parameters as an array then the form was not built property
    return FALSE;
  }  
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  // we don't want to submit the form or have required fields validated on
  // an ahah callback.
  $form_state['submitted'] = TRUE;
  $form['#validate'] = NULL;
  $form['#submit'] = NULL;
  $form_state['submit_handlers'] = NULL;
  $form_state['validate_handlers'] = NULL;
  tripal_core_ahah_form_element_disable_validation($form);

  // Sets the form_state so that the validate and submit handlers can tell
  // when the form is submitted via AHAH
  $form_state['ahah_submission'] = TRUE;

  // Process the form with drupal_process_form. This function calls the submit
  // handlers, which put whatever was worthy of keeping into $form_state.
  drupal_process_form($form_id, $form, $form_state);
  
  // You call drupal_rebuild_form which destroys $_POST.
  // The form generator function is called and creates the form again but since
  // it knows to use $form_state, the form will be different.
  // The new form gets cached and processed again, but because $_POST is
  // destroyed, the submit handlers will not be called again.
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  return $form;
}

/**
 * This function rebuilds the $settings array needed by the 
 * javascript that handles AHAH data returns.  This function should be
 * called in an AHAH callback function prior to returning.
 *
 * See http://tripal.info/documentation/ahah_api for example usage
 *
 * @returns
 *   an associative array with an 'ahah' key.
 *
 * @ingroup tripal_ahah_api
 */
function tripal_core_ahah_bind_events() {
  
  // Get the JS settings so we can merge them.
  $javascript = drupal_add_js(NULL, NULL, 'header');
  $settings = call_user_func_array('array_merge_recursive', $javascript['setting']);
  
  return array('ahah' => $settings['ahah']);
}

/**
 * This function is a helperfunction of the 
 * tripal_core_ahah_prepare_form() function.  It simply
 * disables field validations for all fields (recursively) so that
 * when the form is rebuilt it doesn't try to validate and submit the form.
 *
 * See http://tripal.info/documentation/ahah_api for example usage
 *
 * @returns
 *   nothing 
 *
 * @ingroup tripal_ahah_api
 */
function tripal_core_ahah_form_element_disable_validation(&$form) {
  // --START code borrowed from ahah_helper module
  foreach (element_children($form) as $child) {
    $form[$child]['#validated'] = TRUE;
    tripal_core_ahah_form_element_disable_validation($form[$child]);
  }
  // --END code borrowed from ahah_helper module
}