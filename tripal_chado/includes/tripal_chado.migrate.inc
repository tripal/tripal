<?php

/**
 * Implements hook_form()
 *
 * Provide a form to select Tripal v2 content types for migration
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_migrate_form($form, &$form_state) {

  $tv2_content_type = 'all';
  if (array_key_exists('values', $form_state)) {
    $tv2_content_type = $form_state['values']['tv2_content_type'];
  }

  $options = tripal_chado_get_tripal_v2_content_type_options(TRUE);
  $form['tv2_content_type'] = array(
    '#type' => 'select',
    '#title' => 'Tripal v2 Content Type',
    '#description' => t('Select the Tripal v2 content type to migrate.'),
    '#options' => $options,
    '#default_value' => $tv2_content_type,
    '#ajax' => array(
      'callback' => "tripal_chado_migrate_form_ajax_callback",
      'wrapper' => "tripal-chado-migrate-form",
      'effect' => 'fade',
      'method' => 'replace'
    ),
  );

  // Add a review button that allows reviewing migratable content types
  if ($tv2_content_type != 'all') {

    $table = str_replace('chado_', '', $tv2_content_type);
    $schema = chado_get_schema($table);
    $pkey = $schema['primary key'][0];
    $fkeys = $schema['foreign keys'];

    $form['tv3_content_type'] = array(
      '#type' => 'fieldset',
      '#title' => 'Tripal v3 Content Type',
      '#description' => "Click the 'Get Tripal v3 Types' button to retrieve a list of Tripal v3
        content types to which this Tripal v2 type can be converted. This may take a while
        depending on the size of your database.  The number of items to be converted is
        shown beside the type."
    );
    $form['tv3_content_type']['get_v3_type_btn'] = array(
      '#type' => 'button',
      '#name' => 'get_v3_type_btn',
      '#value' => "Get Tripal v3 Types",
      '#ajax' => array(
        'callback' => "tripal_chado_migrate_form_ajax_callback",
        'wrapper' => "tripal-chado-migrate-form",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );

    if ($form_state['clicked_button']['#name'] == 'get_v3_type_btn') {

      // Migrate all
      $form['tv3_content_type']['tv3_migrate_all'] = array(
        '#type' => 'checkbox',
        '#title' => 'Migrate All'
      );
      // Migrate selection only
      if (key_exists('cvterm', $fkeys) && key_exists('type_id', $fkeys['cvterm']['columns'])) {
        // Get all Tripal v2 node types from the chad_* linking table
        $sql =
          "SELECT V.name AS type, X.accession, db.name AS namespace , count(*) AS num
              FROM chado.$table T
              INNER JOIN $tv2_content_type CT ON T.$pkey = CT.$pkey
              INNER JOIN chado.cvterm V ON V.cvterm_id = T.type_id
              INNER JOIN chado.dbxref X ON X.dbxref_id = V.dbxref_id
              INNER JOIN chado.db ON db.db_id = X.db_id
              GROUP BY V.name, X.accession, db.name";
        $tv3_content_types = db_query($sql);
        while($tv3_content_type = $tv3_content_types->fetchObject()) {
          $form['tv3_content_type']['tv3_content_type--' . $tv3_content_type->namespace .
              '--' . $tv3_content_type->accession . '--' . $tv3_content_type->type] = array(
            '#type' => 'checkbox',
            '#title' => $tv3_content_type->type . ' (' . $tv3_content_type->num . ')',
          );
        }
      }
      else if ($table == 'organism') {
        $sql =
          "SELECT count(*)
              FROM chado.organism O
              INNER JOIN chado_organism CO ON O.organism_id = CO.organism_id";
        $org_count = db_query($sql)->fetchField();
        $form['tv3_content_type']['tv3_content_type--local--organism--organism'] = array(
          '#type' => 'checkbox',
          '#title' => 'Organism (' . $org_count . ')',
        );
      }
      else if ($table == 'analysis') {
        $sql =
        "SELECT count(*)
              FROM chado.analysis A
              INNER JOIN chado_analysis CA ON A.analysis_id = CA.analysis_id";
        $ana_count = db_query($sql)->fetchField();
        $form['tv3_content_type']['tv3_content_type--local--analysis--analysis'] = array(
          '#type' => 'checkbox',
          '#title' => 'Analysis (' . $ana_count . ')',
        );
      }
    }
  }

  // Submit button
  if ($tv2_content_type == 'all' || key_exists('tv3_migrate_all', $form['tv3_content_type'])) {
    $form['migrate_btn'] = array(
      '#type' => 'submit',
      '#name' => 'migrate_btn',
      '#value' => "Migrate $options[$tv2_content_type]",
    );
  }

  $form['#prefix'] = '<div id="tripal-chado-migrate-form">';
  $form['#suffix'] = '</div>';
  return $form;
}

/**
 * Implements hook_validate()
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_migrate_form_validate($form, &$form_state) {

}

/**
 * Implements hook_submit()
 *
 * By submiting the form, a Tripal job to migrate Tripal v2 content is submitted
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_migrate_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'migrate_btn') {
    global $user;
    $values = $form_state['values'];
    $tv2_content_type = $form_state['values']['tv2_content_type'];
    $tv3_content_type = array();
    foreach ($values AS $key => $value) {
      if ($tv2_content_type != 'all') {
        if (preg_match('/^tv3_content_type--(.+)--(.+)--(.+)/', $key, $matches) &&
            ($value == 1 || $values['tv3_migrate_all'] == 1)) {
          $namespace = $matches[1];
          $accession = $matches[2];
          $type = $matches[3];
          $tv3_content_type [] = array(
            'namespace' => $namespace,
            'accession' => $accession,
            'term_name' => $type
          );
        }
      }
    }

    // Submit a job to migrate content
    global $user;
    $args = array(
      array(
        'tv2_content_type' => $tv2_content_type,
        'tv3_content_type' => $tv3_content_type
      )
    );
    $includes = array(
      module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.migrate'),
    );
    if ($tv2_content_type == 'all' || count($tv3_content_type) != 0) {
      return tripal_add_job("Migrate $tv2_content_type Tripal v2 content.",
        'tripal_chado', 'tripal_chado_migrate_records', $args, $user->uid, 10, $includes);
    }
    else {
      return drupal_set_message('Nothing to do. No data for migration.');
    }
  }
}

/**
 * Ajax call back that returns the entire form
 *
 * The callback is triggered by ajax elements on the form which leads to the update of
 * entire form according to the values set on the form
 *
 * @param $form
 * @param $form_state
 * @return $form
 */
function tripal_chado_migrate_form_ajax_callback($form, $form_state) {
  return $form;
}

/**
 * Get available Tripal v2 content types
 *
 * @param boolean $all_option
 *   Include an 'all' option in the returned array
 * @return string[]
 *  Return a string array keyed by the node type
 */
function tripal_chado_get_tripal_v2_content_type_options($all_option = FALSE) {
  // Get all available Tripal v2 chado tables
  $sql =
  "SELECT table_name
      FROM information_schema.tables
      WHERE table_schema = 'public' AND table_name LIKE 'chado_%'";
  $result = db_query($sql);
  $tables = array();
  while ($field = $result->fetchField()) {
    $count = db_query("SELECT count(*) FROM $field")->fetchField();
    if ($count != 0) {
      array_push($tables, $field);
    }
  }

  // List all available Tripal v2 content types
  $result = db_select('node_type', 'nt')
    ->fields('nt', array('type', 'name', 'description'))
    ->condition('type', 'chado_%', 'LIKE')
    ->execute();
  $options = array();
  if ($all_option) {
    $options['all'] = 'All';
  }
  while ($obj = $result->fetchObject()) {
    if (in_array($obj->type, $tables)) {
      $options[$obj->type] = $obj->name;
    }
  }
  return $options;
}

/**
 * Tripal job callback to migrate Tripal v2 content into Tripal v3 content
 *
 * @param $migration
 * @param $job_id
 */
 function tripal_chado_migrate_records($migration, $job_id = NULL) {
  $tv2_content_type = $migration['tv2_content_type'];
  $tv3_content_type = $migration['tv3_content_type'];

  // If tv2_content_type is 'all', migrate all existing Tripal v2 content
  if ($tv2_content_type == 'all') {
    print "Migrating all Tripal v2 content...\n";
    tripal_chado_migrate_all_types();
  }
  // Otherwise, migrate only selected Tripal v2 content
  else {
    print "Migrating selected Tripal v2 content...\n";
    tripal_chado_migrate_selected_types($tv3_content_type);
  }
}

/**
 * Migrate all Tripal v2 content types
 */
function tripal_chado_migrate_all_types() {
  // Get all available Tripal v2 content types
  $tv2_content_types = tripal_chado_get_tripal_v2_content_type_options();

  $types = array();
  foreach($tv2_content_types AS $tv2_content_type => $value) {
    $table = str_replace('chado_', '', $tv2_content_type);
    $schema = chado_get_schema($table);
    $pkey = $schema['primary key'][0];
    $fkeys = $schema['foreign keys'];

    if (key_exists('cvterm', $fkeys) && key_exists('type_id', $fkeys['cvterm']['columns'])) {
      // Get all Tripal v2 node types from the chad_* linking table
      $sql = "
        SELECT V.name AS type, X.accession, db.name AS namespace
        FROM chado.$table T
          INNER JOIN $tv2_content_type CT ON T.$pkey = CT.$pkey
          INNER JOIN chado.cvterm V ON V.cvterm_id = T.type_id
          INNER JOIN chado.dbxref X ON X.dbxref_id = V.dbxref_id
          INNER JOIN chado.db ON db.db_id = X.db_id
        GROUP BY V.name, X.accession, db.name
      ";
      $tv3_content_types = db_query($sql);
      while($tv3_content_type = $tv3_content_types->fetchObject()) {
        array_push($types, array(
          'namespace' => $tv3_content_type->namespace,
          'accession' => $tv3_content_type->accession,
          'term_name' => $tv3_content_type->type
        ));
      }
    }
    else if ($table == 'organism') {
      array_push($types, array(
        'namespace' => 'local',
        'accession' => 'organism',
        'term_name' => 'organism'
      ));
    }
    else if ($table == 'analysis') {
      array_push($types, array(
        'namespace' => 'local',
        'accession' => 'analysis',
        'term_name' => 'analysis'
      ));
    }
  }

  tripal_chado_migrate_selected_types($types);
}

/**
 * Migrate only selected Tripal v2 content types
 *
 * @param unknown $tv3_content_type
 */
function tripal_chado_migrate_selected_types($tv3_content_types) {

  foreach ($tv3_content_types AS $tv3_content_type) {
    // Check if the term already exists
    $term = tripal_load_term_entity($tv3_content_type);

    // If term doesn't exist, create a new bundle for this term
    if (!$term) {
      print("Creating bundle for term '" . $tv3_content_type['term_name'] . "'...\n");
      $success = tripal_create_bundle($tv3_content_type['namespace'], $tv3_content_type['accession'], $tv3_content_type['term_name']);
      $term = tripal_load_term_entity($tv3_content_type);
    }
    // Create bundle name
    $bundle_name = 'bio-data_' . $term->id;

    // Publish records for the bundle
    tripal_chado_publish_records (array('bundle_name' => $bundle_name));
  }
}