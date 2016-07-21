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

  $form['overview_vert_tabs']['#default_tab'] = key_exists('tripal_migrate_current_tab', $_SESSION) ? $_SESSION['tripal_migrate_current_tab'] : 'edit-step1';

  $form['instructions'] = array(
    '#type' => 'item',
    '#markup' => t('Here you may migrate Tripal v2 content types to Tripal v3
        content types. The migration process is divided into four steps that
        allow you to migrate your site as your own pace. Please click each
        step below for details as to the purpose of the step. When all steps
        are completed your site will be fully Tripal v3 compatible.'),
    '#weight' => -100
  );

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
  $mod_url = '/admin/modules';
  $form['step1']['step1_content'] = array(
    '#markup' => 'Tripal legacy modules are needed to support the display of Tripal v2
      content types. Review and ' . l('enable modules', $mod_url) . ' in the \'Tripal v2 Legacy\' category
      for legacy content support. Only content types for enabled legacy
      moodules can be migrated'
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
  $form['step2']['step2_container']['instructions'] = array(
    '#type' => 'item',
    '#markup' => t('Here you can migrate Tripal v2 content types to Tripal v3
        content types.  This will not destroy or remove existing Tripal v2 pages
        but will creatte new Tripal v3 pages.  This allows you to keep existing
        pages while reviewing and customizing the Tripal v3 content types. Site
        visitors can continue to visit the Tripal v2 pages. Tripal
        v3 content types may remain private while customization is underway.
        Once customization is completed a subsequent step will allow you to
        swap out Tripal v2 pages for the newer Tripal v3 pages.') .
      t('If you would like to use Trial v3 web services you must migrate
         content types.'),
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
          "SELECT V.name AS type, X.accession, db.name AS vocabulary , count(*) AS num
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
          // We need to store vocabulary/accession/type for each checkbox in the key becuase
          // the value only allows 1 or 0
          $key = urlencode(
            'tv3_content_type--' .
            $tv3_content_type->vocabulary . '--' .
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
  $form['step3']['instructions'] = array(
    '#type' => 'item',
    '#markup' => t('Once content types have been migrated you have the option
        to use the Tripal v2 display templates.  Tripal v3 allows you to
        customize the page layout using a web interface whereas Tripal v2
        required programatic changes to template files. If your site has
        considerable Tripal v2 customizations that you do not
        want to lose you can use the legacy templates by checking the box
        for the desired content types below.'),
  );
  // Get a list of enabled legacy modules with tv2 templates
  $enabled_templates = variable_get('tripal_chado_enabled_legacy_templates', array());
  $info = module_invoke_all('node_info');
  $options = tripal_chado_get_tripal_v2_content_type_options(FALSE, TRUE);
  foreach ($options AS $type_name => $type_label) {
    $title = $type_label;

    if ($type_name != 'all' and !key_exists($type_name, $info)) {
      $title .= " <strong>(please enable the $type_name legacy module).</strong>";
    }
    $form ['step3']['legacy_template--' . $type_name] = array (
      '#type' => 'checkbox',
      '#title' => $title,
      '#default_value' => key_exists('legacy_template--' . $type_name, $enabled_templates) ? $enabled_templates['legacy_template--' . $type_name] : 0,
    );
  }
  $form['step3']['save_btn'] = array(
    '#type' => 'submit',
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
  $form['step4']['instructions'] = array(
    '#type' => 'item',
    '#markup' => t('This step allows you to fully switch to Tripal v3 pages.
        You can move URLs and titles from Tripal v2 pages to their
        corresponding Tripal v3 pages. This ensures user bookmarks and external
        links to your site are not broken.  Here you can also unpublish Tripal
        v2 content types or completely remove them if desired.  You can
        perform these action in stages such as first moving titles and URLs,
        then unpublishing Tripal v2 pages and once the migration has been
        verified you can finally delete the Tripal v2 pages to free space.
        Deleting the Tripal v2 content will not delete the data in Chado.
        The page is simply removed from the site.'),
  );
  $form['step4']['warning'] = array(
    '#type' => 'item',
    '#markup' => tripal_set_message('Any of the following options cannot be
        undone. Also, please be sure you have migrated all
        desired content types in Step 2 prior to deleting the Tripal v2
        contents.', TRIPAL_WARNING, array('return_html' => TRUE)),
  );
  $opt_complete_migration = array (
    'cp_title' => 'Copy Title over to Tripal v3 Content',
    'mv_url' => 'Migrate URL Alias to Tripal v3 Content',
    'unpublish' => 'Unpublish Tripal v2 Content',
    'delete' => 'Delete Tripal v2 Content',
  );
  if (count($tv2_options) == 1 && key_exists('all', $tv2_options)) {
    $form['step4']['complete'] = array(
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
      '#type' => 'submit',
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
  if ($form_state['clicked_button']['#name'] == 'migrate_btn') {
    $selected_type = $form_state['values']['tv2_content_type'];
    $tv3_content_types = array();
    $values = $form_state['values'];
    if ($selected_type == 'all') {
      $tv2_content_types = tripal_chado_get_tripal_v2_content_type_options();
      $tv3_content_types = tripal_chado_migrate_map_types($tv2_content_types);
    }
    else {
      foreach ($values AS $key => $value) {
        if ($selected_type != 'all') {
          $key = urldecode($key);
          if (preg_match('/^tv3_content_type--(.+)--(.+)--(.+)/', $key, $matches) &&
              ($value == 1 || $values['tv3_migrate_all'] == 1)) {
            $vocabulary = $matches[1];
            $accession = $matches[2];
            $type = $matches[3];
            $tv3_content_types [] = array(
              'vocabulary' => $vocabulary,
              'accession' => $accession,
              'term_name' => $type
            );
          }
        }
      }
    }
    // Check if CV exists in the tripal_cv_defaults table. If not, do not proceed to migration
    foreach ($tv3_content_types AS $type) {
      $voc = $type['vocabulary'];
      $acc = $type['accession'];
      $term_name =  $type['term_name'];
      $match = array(
        'dbxref_id' => array(
          'db_id' => array(
            'name' => $voc,
          ),
          'accession' => $acc
        ),
      );
      $cvterm = chado_generate_var('cvterm', $match);
      if ($voc == 'local' || !$cvterm) {
        // Ignore local terms
        continue;
      }
      $default = tripal_get_default_cv_table($cvterm->cv_id->cv_id);
      if (!$default) {
        form_set_error('', 
            t("'@voc' is not a ". l('Default Vocabulary', 'admin/tripal/storage/chado/vocab') . ". '@cvterm' can not be migrated.", 
              array(
                '@voc' => $voc,
                '@cvterm' => $cvterm->name
              )
            )
        );
      }
    }
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
    $_SESSION['tripal_migrate_current_tab'] = 'edit-step3';
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
    $_SESSION['tripal_migrate_current_tab'] = 'edit-step4';
    return;
  }
  // Migrate tv2 content to tv3 content
  else if ($form_state['clicked_button']['#name'] == 'migrate_btn') {
    global $user;
    $values = $form_state['values'];
    $tv2_content_type = $form_state['values']['tv2_content_type'];
    $tv3_content_type = array();
    foreach ($values AS $key => $value) {
      if ($tv2_content_type != 'all') {
        $key = urldecode($key);
        if (preg_match('/^tv3_content_type--(.+)--(.+)--(.+)/', $key, $matches) &&
            ($value == 1 || $values['tv3_migrate_all'] == 1)) {
          $vocabulary = $matches[1];
          $accession = $matches[2];
          $type = $matches[3];
          $tv3_content_type [] = array(
            'vocabulary' => $vocabulary,
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
    $_SESSION['tripal_migrate_current_tab'] = 'edit-step2';
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
function tripal_chado_get_tripal_v2_content_type_options($all_option = FALSE, $has_template = FALSE) {
  // Get all available Tripal v2 chado tables
  $sql =
  "SELECT table_name
      FROM information_schema.tables
      WHERE table_schema = 'public' AND table_name LIKE 'chado_%'";
  $result = db_query($sql);
  // Store 'chado_*' tables that has at least one node
  $tables = array();
  while ($field = $result->fetchField()) {
    $count = db_query("SELECT count(*) FROM $field LIMIT 1")->fetchField();
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
    if ($has_template) {
      $mod = str_replace('chado_', 'tripal_', $obj->type);
      $mod_dir = drupal_get_path('module', $mod);
      if (file_exists($mod_dir . '/theme/templates/' . $mod . '_base.tpl.php')) {
        $options[$obj->type] = $obj->name;
      }
    }
    else {
      if (in_array($obj->type, $tables)) {
        $options[$obj->type] = $obj->name;
      }
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
 * Gather all available Tripal v2 content types and store the result in an
 * associated array with values of vocabulary, accession, term_name. The array
 * is then pass to the function tripal_chado_migrate_selected_types() that
 * handles the migration.
 */
function tripal_chado_migrate_all_types() {
  // Get all available Tripal v2 content types
  $tv2_content_types = tripal_chado_get_tripal_v2_content_type_options();
  $types = tripal_chado_migrate_map_types($tv2_content_types);
  tripal_chado_migrate_selected_types($types);
}
/**
 * Map all tv2_content_type to tv3_content_type
 *
 * @param unknown $tv2_content_type
 * return $tv3_content_type
 */
function tripal_chado_migrate_map_types($tv2_content_types) {
  $types = array();
  foreach($tv2_content_types AS $tv2_content_type => $value) {
    $table = str_replace('chado_', '', $tv2_content_type);
    $schema = chado_get_schema($table);
    $pkey = $schema['primary key'][0];
    $fkeys = $schema['foreign keys'];
  
    if (key_exists('cvterm', $fkeys) && key_exists('type_id', $fkeys['cvterm']['columns'])) {
      // Get all Tripal v2 node types from the chad_* linking table
      $sql = "
        SELECT V.name AS type, X.accession, db.name AS vocabulary
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
          'vocabulary' => $tv3_content_type->vocabulary,
          'accession' => $tv3_content_type->accession,
          'term_name' => $tv3_content_type->type
        ));
      }
    }
    else if ($table == 'organism') {
      array_push($types, array(
        'vocabulary' => 'local',
        'accession' => 'organism',
        'term_name' => 'organism'
      ));
    }
    else if ($table == 'analysis') {
      array_push($types, array(
        'vocabulary' => 'local',
        'accession' => 'analysis',
        'term_name' => 'analysis'
      ));
    }
  }
  return $types;
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
      $success = tripal_create_bundle($tv3_content_type['vocabulary'],
          $tv3_content_type['accession'], $tv3_content_type['term_name']);
      $term = tripal_load_term_entity($tv3_content_type);
    }
    // Create bundle name
    $bundle_name = 'bio_data_' . $term->id;

    // Publish records for the bundle
    $value = array(
      'sync_node' => 1,
      'bundle_name' => $bundle_name
    );
    tripal_chado_publish_records($value);
  }
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