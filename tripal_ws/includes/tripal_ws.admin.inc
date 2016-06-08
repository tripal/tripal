<?php
/**
 * Provide form to store information of other Tripal sites
 * 
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_form($form, &$form_state) {
  $form = array();
  $values = key_exists('values', $form_state) ? $form_state['values'] : NULL;
  $tripal_site = $values ? $values['tripal_site'] : 0;
  
  $sites = array('Select a Tripal site', 'Add a Tripal site');
  $form['tripal_site'] = array(
    '#type' => 'select',
    '#description' => 'Make change to an existing Tripal site',
    '#options' => $sites,
    '#default_value' => $tripal_site,
    '#ajax' => array(
      'callback' => "tripal_ws_tripal_sites_form_ajax_callback",
      'wrapper' => "tripal-ws-tripal_sites-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  
  // Add/Edit a new tripal site
  if ($tripal_site != 0) {
    $form['tripal_site_info']['name'] = array(
      '#title' => t('Name of Site'),
      '#type' => 'textfield'
    );
    $form['tripal_site_info']['url'] = array(
      '#title' => t('URL'),
      '#type' => 'textfield'
    );
    $form['tripal_site_info']['description'] = array(
      '#title' => t('Description'),
      '#type' => 'textfield'
    );
    $form['submit_button'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#name' => 'save'
    );
  }
  
  $form['#prefix'] = '<div id="tripal-ws-tripal_sites-form">';
  $form['#suffix'] = '</div>';
  return $form;
}



/**
 *
 */
function tripal_ws_tripal_sites_form_ajax_callback($form, $form_state) {
  return $form;
}