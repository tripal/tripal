<?php

function tripal_chado_migrate_form($form, &$form_state) {

  $content_type = 'all';
  if (array_key_exists('values', $form_state)) {
    $content_type = $form_state['values']['content_type'];
  }
    
  // Get all available Tripal v2 chado tables
  $sql = "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public' AND table_name LIKE 'chado_%'";
  $result = db_query($sql);
  $tables = array();
  while ($field = $result->fetchField()) {
    array_push($tables, $field);
  }
  
  // List all available Tripal v2 content types
  $result = db_select('node_type', 'nt')
    ->fields('nt', array('type', 'name', 'description'))
    ->condition('type', 'chado_%', 'LIKE')
    ->execute();
  $options = array ('all' => 'All');
  while ($obj = $result->fetchObject()) {
    if (in_array($obj->type, $tables)) {
      $options[$obj->type] = $obj->name;
    }
  }
  
  $form['content_type'] = array(
    '#type' => 'select',
    '#title' => 'Content Type',
    '#description' => t('Select the content type to migrate.'),
    '#options' => $options,
    '#default_value' => $content_type,
    '#ajax' => array(
      'callback' => "tripal_chado_migrate_form_ajax_callback",
      'wrapper' => "tripal-chado-migrate-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  
  $form['migrate_btn'] = array(
    '#type' => 'submit',
    '#name' => 'migrate_btn',
    '#value' => "Migrate $options[$content_type] Content Type",
  );

  $form['#prefix'] = '<div id="tripal-chado-migrate-form">';
  $form['#suffix'] = '</div>';
  return $form;
}

function tripal_chado_migrate_form_validate($form, &$form_state) {

}

function tripal_chado_migrate_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'migrate_btn') {
    global $user;
    $values = $form_state['values'];
    $all = $form_state['values']['all'];
    $migration = array();
    foreach ($values AS $key => $value) {
      if ($all) {
        if (preg_match('/^chado_/', $key)) {
          array_push($migration, $key);
        }
      } else {
        if (preg_match('/^chado_/', $key) && $value) {
          array_push($migration, $key);
        }
      }
    }
    
    // Submit a job to migrate content
    dpm($migration);
    /* $term_id = $form_state['values']['term_id'];
    $bundle_name = 'bio-data_' . $term_id;
    $bundle = tripal_load_bundle_entity(array('name' => $bundle_name)); */

  }
}
/**
 *
 */
function tripal_chado_migrate_form_ajax_callback($form, $form_state) {
  return $form;
}

/**
 *
 */
function tripal_chado_migrate_records($bundle_name, $job_id = NULL) {

}