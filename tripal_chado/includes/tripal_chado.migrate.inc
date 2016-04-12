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

  $form['overview_vert_tabs'] = array(
    '#type' => 'vertical_tabs'
  );
  $form['modes']['#group'] = 'overview_vert_tabs';
  $form['modes']['#weight'] = 1000;
  
  // Step 1
  $form['step1'] = array(
    '#type' => 'fieldset',
    '#title' => 'Step1',
    '#description' => '<b>Enable Legacy Support</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs'
  );
  global $base_url;
  $mod_url = $base_url . '/admin/modules';
  $form['step1']['step1_content'] = array(
    '#markup' => 'Tripal legacy modules are needed to support the display of Tripal v2 
                      content types. Review and <a href="'. $mod_url .'">enable modules</a>
                      in the \'Tripal v2 Legacy\' category for legacy content support'
  );
  
  // Step 2
  $form['step2'] = array(
    '#type' => 'fieldset',
    '#title' => 'Step2',
    '#description' => '<b>Migrate Content</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs'
  );
  $form['step2']['step2_container'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#prefix' => '<div id="tripal-chado-migrate-form-step2">',
    '#suffix' => '</div>'
  );
  
  $tv2_content_type = 'all';
  if (array_key_exists('values', $form_state)) {
    $tv2_content_type = $form_state['values']['tv2_content_type'];
  }

  $tv2_options = tripal_chado_get_tripal_v2_content_type_options(TRUE);
  $form['step2']['step2_container']['tv2_content_type'] = array(
    '#type' => 'select',
    '#title' => 'Tripal v2 Content Type',
    '#description' => t('Select the Tripal v2 content type to migrate.'),
    '#options' => $tv2_options,
    '#default_value' => $tv2_content_type,
    '#ajax' => array(
      'callback' => "tripal_chado_migrate_form_step2_ajax_callback",
      'wrapper' => "tripal-chado-migrate-form-step2",
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

    $form['step2']['step2_container']['tv3_content_type'] = array(
      '#type' => 'fieldset',
      '#title' => 'Tripal v3 Content Type',
      '#description' => "Click the 'Get Tripal v3 Types' button to retrieve a list of Tripal v3
        content types to which this Tripal v2 type can be converted. This may take a while
        depending on the size of your database.  The number of items to be converted is
        shown beside the type."
    );
    $form['step2']['step2_container']['tv3_content_type']['get_v3_type_btn'] = array(
      '#type' => 'button',
      '#name' => 'get_v3_type_btn',
      '#value' => "Get Tripal v3 Types",
      '#ajax' => array(
        'callback' => "tripal_chado_migrate_form_step2_ajax_callback",
        'wrapper' => "tripal-chado-migrate-form-step2",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );

    $no_data = TRUE;
    if ($form_state['clicked_button']['#name'] == 'get_v3_type_btn') {

      // Migrate all      
      $form['step2']['step2_container']['tv3_content_type']['tv3_migrate_all'] = array(
        '#type' => 'checkbox',
        '#title' => 'Migrate All'
      );
      // Migrate selection only
      if (key_exists('cvterm', $fkeys) && key_exists('type_id', $fkeys['cvterm']['columns'])) {
        // Get all Tripal v2 node types from the chad_* linking table
        $sql =
          "SELECT V.name AS type, X.accession, db.name AS namespace , count(*) AS num
              FROM {" . $table . "} T
              INNER JOIN public.$tv2_content_type CT ON T.$pkey = CT.$pkey
              INNER JOIN {cvterm} V ON V.cvterm_id = T.type_id
              INNER JOIN {dbxref} X ON X.dbxref_id = V.dbxref_id
              INNER JOIN {db} ON db.db_id = X.db_id
              LEFT JOIN public.chado_entity CE ON CE.record_id = T.$pkey
              AND CE.data_table = '$table'
              WHERE CE.record_id IS NULL
              GROUP BY V.name, X.accession, db.name";
        $tv3_content_types = chado_query($sql);
        while($tv3_content_type = $tv3_content_types->fetchObject()) {
          // We need to store namespace/accession/type for each checkbox in the key becuase
          // the value only allows 1 or 0
          $key = urlencode(
            'tv3_content_type--' . 
            $tv3_content_type->namespace . '--' . 
            $tv3_content_type->accession . '--' . 
            $tv3_content_type->type);
          $form['step2']['step2_container']['tv3_content_type'][$key] = array(
            '#type' => 'checkbox',
            '#title' => $tv3_content_type->type . ' (' . $tv3_content_type->num . ')',
          );
          $no_data = FALSE;
        }
      }
      else if ($table == 'organism') {
        $sql =
          "SELECT count(*)
              FROM {organism} O
              INNER JOIN public.chado_organism CO ON O.organism_id = CO.organism_id
              LEFT JOIN public.chado_entity CE ON CE.record_id = O.organism_id 
                AND CE.data_table = 'organism'
              WHERE CE.record_id IS NULL";
        $org_count = chado_query($sql)->fetchField();
        if ($org_count > 0) {
          $key = urldecode('tv3_content_type--local--organism--organism');
          $form['step2']['step2_container']['tv3_content_type'][$key] = array(
            '#type' => 'checkbox',
            '#title' => 'Organism (' . $org_count . ')',
          );
          $no_data = FALSE;
        }
      }
      else if ($table == 'analysis') {
        $sql =
        "SELECT count(*)
              FROM {analysis} A
              INNER JOIN public.chado_analysis CA ON A.analysis_id = CA.analysis_id
              LEFT JOIN public.chado_entity CE ON CE.record_id = A.analysis_id 
                AND CE.data_table = 'analysis'
              WHERE CE.record_id IS NULL";
        $ana_count = chado_query($sql)->fetchField();
        if ($ana_count > 0) {
          $key = urlencode('tv3_content_type--local--analysis--analysis');
          $form['step2']['step2_container']['tv3_content_type'][$key] = array(
            '#type' => 'checkbox',
            '#title' => 'Analysis (' . $ana_count . ')',
          );
          $no_data = FALSE;
        }
      }
      if ($no_data) {
        unset($form['step2']['step2_container']['tv3_content_type']['tv3_migrate_all']);
        drupal_set_message('No data for migration or all have been migrated.', 'warning');
      }
    }
  }
  // Migrate button
  if ($tv2_content_type == 'all' || key_exists('tv3_migrate_all', $form['step2']['step2_container']['tv3_content_type'])) {
    $form['step2']['step2_container']['migrate_btn'] = array(
      '#type' => 'submit',
      '#name' => 'migrate_btn',
      '#value' => "Migrate $tv2_options[$tv2_content_type]",
    );
  }
  
  // Step 3
  $form['step3'] = array(
    '#type' => 'fieldset',
    '#title' => 'Step3',
    '#description' => '<b>Use Legacy Templates (optional)</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs'
  );
  // Get a list of enabled legacy modules with tv2 templates
  $mod_enabled = tripal_chado_migrate_get_enabled_legacy_modules(TRUE);
  $enabled_templates = variable_get('tripal_chado_enabled_legacy_templates', array());
  foreach ($mod_enabled AS $mod_name => $mod_path) {
    $form ['step3']['legacy_template--' . $mod_name] = array (
      '#type' => 'checkbox',
      '#title' => ucwords(str_replace(array('tripal', '_'), array('chado', ' '), $mod_name)),
      '#default_value' => key_exists('legacy_template--' . $mod_name, $enabled_templates) ? $enabled_templates['legacy_template--' . $mod_name] : 0,
    );
  }
  $form['step3']['save_btn'] = array(
    '#type' => 'button',
    '#name' => 'save_enabled_template_btn',
    '#value' => "Save",
  );
  
  // Step 4
  $form['step4'] = array(
    '#type' => 'fieldset',
    '#title' => 'Step4',
    '#description' => '<b>Complete Migration</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs'
  );
  $opt_complete_migration = array (
    'cp_title' => 'Copy Title over to Tripal v3 Content',
    'mv_url' => 'Migrate URL Alias to Tripal v3 Content',
    'unpublish' => 'Unpublish Tripal v2 Content',
    'delete' => 'Delete Tripal v2 Content',
  );
  if (count($tv2_options) == 1 && key_exists('all', $tv2_options)) {
    $form['step4']['compltete'] = array(
      '#markup' => 'Migration completed. All content have been migrated.'
    );
  }
  else {
    foreach ($tv2_options AS $opt_key => $opt) {
      $form['step4'][$opt_key . '_title'] = array(
        '#markup' => "<b>$opt</b>"
      );
      $form['step4']['complete_migration--' . $opt_key] = array(
        '#type' => 'checkboxes',
        '#options' => $opt_complete_migration,
      );
    }
    $form['step4']['submit_btn'] = array(
      '#type' => 'button',
      '#name' => 'complete_migration_btn',
      '#value' => "Submit",
    );
  }
  return $form;
}

/**
 * Implements hook_validate()
 *
 * @param $form
 * @param $form_state
 */
function tripal_chado_migrate_form_validate($form, &$form_state) {
  // Store the legacy template setting in a Drupal variable
  if ($form_state['clicked_button']['#name'] == 'save_enabled_template_btn') {
    $values = $form_state['values'];
    $enabled_templates = array();
    foreach ($values AS $key => $value) {
      if (preg_match('/^legacy_template--/', $key)) {
        $enabled_templates[$key] = $value;
      }
    }
    variable_set('tripal_chado_enabled_legacy_templates', $enabled_templates);
    drupal_theme_rebuild();
  }
  // Complete migration
  else if ($form_state['clicked_button']['#name'] == 'complete_migration_btn') {
    $values = $form_state['values'];
    $config = array (
      'delete' => array(),
      'unpublish' => array(),
      'cp_title' => array(),
      'mv_url' => array()
    );
    $all = array();
    $all_types = array();
    // Gather checked checkboxes and store in the $config array
    foreach ($values AS $key => $value) {
      if (preg_match('/^complete_migration--/', $key)) {
        $type = str_replace('complete_migration--', '', $key);
        if ($type != 'all') {
          array_push($all_types, $type);
        }        
        foreach ($value AS $key_op => $op) {
          if ($type == 'all') {
            if ($op) {
              array_push($all, $key_op);
            }
          }
          else {
            if ($op) {
              array_push($config[$key_op], $type);
            }
          }
        }
      }
    }
    foreach($all AS $a) {
      $config[$a] = $all_types;
    }
    // Submit jobs to complete the migration
    global $user;
    $includes = array(
      module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.migrate'),
    );
    
    if (count($config['cp_title']) > 0) {
      $args = array($config['cp_title']);
      tripal_add_job("Copy Title over to Tripal v3 Content", 'tripal_chado', 
        'tripal_chado_copy_title_for_selected_types', $args, $user->uid, 10, $includes);
    }
    if (count($config['mv_url']) > 0) {
      $args = array($config['mv_url']);
      tripal_add_job(" Migrate URL Alias to Tripal v3 Content ", 'tripal_chado',
        'tripal_chado_migrate_url_alias_for_selected_types', $args, $user->uid, 10, $includes);
    }
    if (count($config['unpublish']) > 0) {
      $args = array($config['unpublish']);
      tripal_add_job(" Unpublish Tripal v2 Content ", 'tripal_chado',
        'tripal_chado_unpublish_selected_types', $args, $user->uid, 10, $includes);
    }
    if (count($config['delete']) > 0) {
      $args = array($config['delete']);
      tripal_add_job("Delete Tripal v2 Content ", 'tripal_chado',
        'tripal_chado_delete_selected_types', $args, $user->uid, 10, $includes);
    }
    return;
  }
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
        $key = urldecode($key);
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
      return drupal_set_message('Nothing to do. All data have been migrated or no data for migration.');
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
function tripal_chado_migrate_form_step2_ajax_callback(&$form, &$form_state) {
  return $form['step2']['step2_container'];
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
 * 
 * Gather all available Tripal v2 content types and store the result in an associated array with 
 * values of namespace, accession, term_name. The array is then pass to the function 
 * tripal_chado_migrate_selected_types() that handles the migration
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
        FROM {" . $table . "} T
          INNER JOIN public.$tv2_content_type CT ON T.$pkey = CT.$pkey
          INNER JOIN {cvterm} V ON V.cvterm_id = T.type_id
          INNER JOIN {dbxref} X ON X.dbxref_id = V.dbxref_id
          INNER JOIN {db} ON db.db_id = X.db_id
        GROUP BY V.name, X.accession, db.name
      ";
      $tv3_content_types = chado_query($sql);
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
    $bundle_name = 'bio_data_' . $term->id;

    // Publish records for the bundle
    $value = array(
      'sync_node' => 1,
      'bundle_name' => $bundle_name
    );
    tripal_chado_publish_records ($value);
  }
}

/**
 * Get a list of enabled legacy modules
 * 
 * return an associated array with value of module directory and keyed by the module name
 */
function tripal_chado_migrate_get_enabled_legacy_modules ($has_base_template = FALSE) {
  $mod_enabled = module_list();
  $legacy_mod = array ();
  foreach ($mod_enabled AS $mod) {
    if (preg_match('/^tripal_/', $mod)) {
      $mod_dir = drupal_get_path('module', $mod);
      if (preg_match('/\/legacy\//', $mod_dir) ) {
        if ($has_base_template) {
          if (file_exists($mod_dir . '/theme/templates/' . $mod . '_base.tpl.php')) {
            $legacy_mod[$mod] = $mod_dir;
          }
        } else {
          $legacy_mod[$mod] = $mod_dir;
        }
      }
    }
  }
  return $legacy_mod;
}

/**
 * Delete selected Tripal v2 content
 *
 * Delete all records from chado_* table then call the cleanup orphan nodes function
 * 
 * @param unknown $tv2_content_types
 */
function tripal_chado_delete_selected_types($tv2_content_types = array()) {
  foreach ($tv2_content_types AS $type) {
    $sql = "DELETE FROM $type";
    db_query($sql);
    chado_cleanup_orphaned_nodes(str_replace('chado_', '', $type));
  }
}

/**
 * Unpublish selected Tripal v2 content
 *
 * Set status = 0 (unpublished) for all nodes of selected Tripal v2 content types
 *
 * @param unknown $tv2_content_types
 */
function tripal_chado_unpublish_selected_types($tv2_content_types = array()) {
  foreach ($tv2_content_types AS $type) {
    $sql = "UPDATE node SET status = 0 WHERE nid IN (SELECT nid FROM $type)";
    db_query($sql);
    $sql = "UPDATE node_revision SET status = 0 WHERE nid IN (SELECT nid FROM $type)";
    db_query($sql);
  }
}

/**
 * Copy titles for selected Tripal v2 content
 *
 * Copy tiltles for all nodes of selected Tripal v2 content types
 *
 * @param unknown $tv2_content_types
 */
function tripal_chado_copy_title_for_selected_types($tv2_content_types = array()) {
  foreach ($tv2_content_types AS $type) {
    $sql = "SELECT nid, entity_id FROM chado_entity WHERE nid IN (SELECT nid FROM $type)";
    $result = db_query($sql);
    while ($entity = $result->fetchObject()) {
      $usql = "
          UPDATE tripal_entity 
          SET title = (SELECT title FROM node WHERE nid = :nid)
          WHERE id = :entity_id";
      db_query($usql, array(
        ':nid' => $entity->nid, 
        ':entity_id' => $entity->entity_id)
      );
    }
  }
}

/**
 * Migrate URL alias for selected Tripal v2 content
 *
 * Migrate URL alias for all nodes of selected Tripal v2 content types
 *
 * @param unknown $tv2_content_types
 */
function tripal_chado_migrate_url_alias_for_selected_types($tv2_content_types = array()) {
  foreach ($tv2_content_types AS $type) {
    $sql = "SELECT nid, entity_id FROM chado_entity WHERE nid IN (SELECT nid FROM $type)";
    $result = db_query($sql);
    while ($entity = $result->fetchObject()) {
      $usql = "
          UPDATE url_alias
          SET source = 'bio_data/" . $entity->entity_id .
          "' WHERE source = 'node/" . $entity->nid . "'";
      db_query($usql);
    }
  }
}