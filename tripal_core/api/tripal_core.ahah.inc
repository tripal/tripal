<?php

/*
 * @file
 * Part of the Tripal Core API.
 *
 * Provides support for dynamic forms through AHAH
 */

/*
 * This function simply gets the posted form ID, builds the form
 * and retrieves the specified element
 */
function tripal_core_ahah_prepare_form() {
  
  // Retrieve the form from the cache
  $form_state = array('storage' => NULL);
  $form_build_id = filter_xss($_POST['form_build_id']);
  $form = form_get_cache($form_build_id, $form_state);

  // Preparing to process the form
  $args = $form['#parameters'];
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

/*
 * 
 */
function tripal_core_ahah_bind_events() {
  
  // Get the JS settings so we can merge them.
  $javascript = drupal_add_js(NULL, NULL, 'header');
  $settings = call_user_func_array('array_merge_recursive', $javascript['setting']);
  
  return array('ahah' => $settings['ahah']);
}

/*
 * 
 */
function tripal_core_ahah_form_element_disable_validation(&$form) {
  // --START code borrowed from ahah_helper module
  foreach (element_children($form) as $child) {
    $form[$child]['#validated'] = TRUE;
    tripal_core_ahah_form_element_disable_validation($form[$child]);
  }
  // --END code borrowed from ahah_helper module
}