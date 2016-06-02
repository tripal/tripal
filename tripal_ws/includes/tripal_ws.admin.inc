<?php
/**
 * Provide form to store information of other Tripal sites
 * 
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_form($form, &$form_state) {
  $form = array();
  $form['add_tripal_site'] = array(
    '#markup' => l('Add Tripal site', '/admin/tripal/ws/tripal_sites/add') 
  );
  $sites = array('Select a Tripal site');
  $form['edit_tripal_site'] = array(
    '#type' => 'select',
    '#description' => 'Make change to an existing Tripal site',
    '#options' => $sites
  );
  return $form;
}