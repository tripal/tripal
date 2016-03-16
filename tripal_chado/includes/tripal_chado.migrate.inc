<?php

function tripal_chado_migrate_form($form, &$form_state) {

  $chado_content = 'all';
  if (array_key_exists('values', $form_state)) {
    $chado_content = $form_state['values']['chado_content'];
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
  
  $form['chado_content'] = array(
    '#type' => 'select',
    '#title' => 'Chado Content',
    '#description' => t('Select the chado content to migrate.'),
    '#options' => $options,
    '#default_value' => $chado_content,
    '#ajax' => array(
      'callback' => "tripal_chado_migrate_form_ajax_callback",
      'wrapper' => "tripal-chado-migrate-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );
  
  // Add a review button that allows reviewing migratable content types
  if ($chado_content != 'all') {
    
    $table = str_replace('chado_', '', $chado_content);
    $schema = chado_get_schema($table);
    $pkey = $schema['primary key'][0];
    $fkeys = $schema['foreign keys'];
    
    if (key_exists('cvterm', $fkeys) && key_exists('type_id', $fkeys['cvterm']['columns'])) {
      $form['review'] = array(
        '#type' => 'fieldset',
        '#title' => 'Review Content Type',
        '#description' => 'By clicking on the Review button, Tripal will gather information from
        existing database and list content types which can be mapped to a Tripal v3 content 
        type. (may take a while depending on the size of your database.). This allows you 
        to migrate only the types of your selection.'
      );
      $form['review']['review_btn'] = array(
        '#type' => 'button',
        '#name' => 'review_btn',
        '#value' => "Review",
        '#ajax' => array(
          'callback' => "tripal_chado_migrate_form_ajax_callback",
          'wrapper' => "tripal-chado-migrate-form",
          'effect' => 'fade',
          'method' => 'replace'
        ),
      );
      $counter = 0;
      if ($form_state['clicked_button']['#name'] == 'review_btn') {
        // Get all Tripal v2 node types from the chad_* linking table
        $sql = 
          "SELECT DISTINCT V.name AS type, X.accession, db.name AS namespace 
              FROM chado.$table T
              INNER JOIN $chado_content CT ON T.$pkey = CT.$pkey
              INNER JOIN chado.cvterm V ON V.cvterm_id = T.type_id
              INNER JOIN chado.dbxref X ON X.dbxref_id = V.dbxref_id
              INNER JOIN chado.db ON db.db_id = X.db_id";
        $subtypes = db_query($sql);
        while($subtype = $subtypes->fetchObject()) {
          $form['review']['chado_content_type--' . $subtype->namespace . '--' . $subtype->type] = array(
            '#type' => 'checkbox',
            '#title' => $subtype->type,
          );
            $counter ++;
        }
      
        // No subtype exists, migrate all
        if ($counter == 0) {
          $form['review']['nosubtype'] = array(
            '#markup' => t("<br>Type not found. Migrate all $options[$chado_content]."),
          );
        }
      }
    }
  }
  
  // Submit button
  $form['migrate_btn'] = array(
    '#type' => 'submit',
    '#name' => 'migrate_btn',
    '#value' => "Migrate $options[$chado_content]",
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
    $chado_contents = $form_state['values']['chado_content'];
    $subtypes = array();
    foreach ($values AS $key => $value) {
      if ($chado_contents != 'all') {
        if (preg_match('/^chado_content_type--(.+)--(.+)/', $key, $matches) && $value == 1) {
          $namespace = $matches[1];
          $accession = $matches[2];          
          $subtypes [$namespace] = $accession;
        }
      }
    }
    
    // Submit a job to migrate content
    global $user;
    $args = array(
      array(
        'chado_content' => $chado_contents,
        'subtypes' => $subtypes
      ),
    );
    $includes = array();
    return tripal_add_job("Migrate $chado_contents.", 'tripal_chado', 'tripal_chado_migrate_records', $args, $user->uid, 10, $includes);
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