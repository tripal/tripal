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
  $form['overview_vert_tabs'] = [
    '#type' => 'vertical_tabs',
  ];

  $form['overview_vert_tabs']['#default_tab'] = key_exists('tripal_migrate_current_tab', $_SESSION) ? $_SESSION['tripal_migrate_current_tab'] : 'edit-step1';

  $form['instructions'] = [
    '#type' => 'item',
    '#markup' => t('Here you may migrate Tripal v2 content types to Tripal v3
        content types. The migration process is divided into four steps that
        allow you to migrate your site as your own pace. Please click each
        step below for details as to the purpose of the step. When all steps
        are completed your site will be fully Tripal v3 compatible.'),
    '#weight' => -100,
  ];

  // Step 1
  $form['step1'] = [
    '#type' => 'fieldset',
    '#title' => 'Step1',
    '#description' => '<b>Enable Legacy Support</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs',
  ];
  global $base_url;
  $mod_url = '/admin/modules';
  $form['step1']['step1_content'] = [
    '#markup' => 'Tripal legacy modules are needed to support the display of Tripal v2
      content types. Review and ' . l('enable modules', $mod_url) . ' in the \'Tripal v2 Legacy\' category
      for legacy content support. Only content types for enabled legacy
      moodules can be migrated',
  ];

  // Step 2
  $form['step2'] = [
    '#type' => 'fieldset',
    '#title' => 'Step2',
    '#description' => '<b>Migrate Content</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs',
  ];
  $form['step2']['step2_container'] = [
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#prefix' => '<div id="tripal-chado-migrate-form-step2">',
    '#suffix' => '</div>',
  ];
  $form['step2']['step2_container']['instructions'] = [
    '#type' => 'item',
    '#markup' => t('Here you can migrate Tripal v2 content types to Tripal v3
        content types.  This will not destroy or remove existing Tripal v2 pages
        but will creatte new Tripal v3 pages.  This allows you to keep existing
        pages while reviewing and customizing the Tripal v3 content types. Site
        visitors can continue to visit the Tripal v2 pages. Tripal
        v3 content types may remain private while customization is underway.
        Once customization is completed a subsequent step will allow you to
        swap out Tripal v2 pages for the newer Tripal v3 pages. If you would like to
        use Trial v3 web services you must migrate content types.'),
  ];

  $tv2_content_type = 'all';
  if (array_key_exists('values', $form_state)) {
    $tv2_content_type = $form_state['values']['tv2_content_type'];
  }

  $tv2_options = tripal_chado_get_tripal_v2_content_type_options(TRUE);
  $form['step2']['step2_container']['tv2_content_type'] = [
    '#type' => 'select',
    '#title' => 'Tripal v2 Content Type',
    '#description' => t('Select the Tripal v2 content type to migrate.'),
    '#options' => $tv2_options,
    '#default_value' => $tv2_content_type,
    '#ajax' => [
      'callback' => "tripal_chado_migrate_form_step2_ajax_callback",
      'wrapper' => "tripal-chado-migrate-form-step2",
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];

  // Add a review button that allows reviewing migratable content types
  if ($tv2_content_type != 'all') {
    $table = str_replace('chado_', '', $tv2_content_type);
    $schema = chado_get_schema($table);
    $pkey = $schema['primary key'][0];
    $fkeys = $schema['foreign keys'];

    $form['step2']['step2_container']['tv3_content_type'] = [
      '#type' => 'fieldset',
      '#title' => 'Tripal v3 Content Type',
      '#description' => "Click the 'Get Tripal v3 Types' button to retrieve a list of Tripal v3
        content types to which this Tripal v2 type can be converted. This may take a while
        depending on the size of your database.  The number of items to be converted is
        shown beside the type.",
    ];
    $form['step2']['step2_container']['tv3_content_type']['get_v3_type_btn'] = [
      '#type' => 'button',
      '#name' => 'get_v3_type_btn',
      '#value' => "Get Tripal v3 Types",
      '#ajax' => [
        'callback' => "tripal_chado_migrate_form_step2_ajax_callback",
        'wrapper' => "tripal-chado-migrate-form-step2",
        'effect' => 'fade',
        'method' => 'replace',
      ],
    ];

    $no_data = TRUE;
    if ($form_state['clicked_button']['#name'] == 'get_v3_type_btn') {

      // Migrate all
      $form['step2']['step2_container']['tv3_content_type']['tv3_migrate_all'] = [
        '#type' => 'checkbox',
        '#title' => 'Migrate All',
      ];
      // Migrate selection only
      if ($table == 'organism') {
        $sql =
          "SELECT count(*)
            FROM {organism} O
            INNER JOIN [chado_organism] CO ON O.organism_id = CO.organism_id
          ";
        $org_count = chado_query($sql)->fetchField();
        if ($org_count > 0) {
          $key = urldecode('tv3_content_type--OBI--0100026--organism');
          $form['step2']['step2_container']['tv3_content_type'][$key] = [
            '#type' => 'checkbox',
            '#title' => 'Organism (' . $org_count . ')',
          ];
          $no_data = FALSE;
        }
      }
      else {
        if ($table == 'analysis') {
          $sql =
            "SELECT count(*)
          FROM {analysis} A
          INNER JOIN [chado_analysis] CA ON A.analysis_id = CA.analysis_id
         ";
          $ana_count = chado_query($sql)->fetchField();
          if ($ana_count > 0) {
            $key = urlencode('tv3_content_type--operation--2945--analysis');
            $form['step2']['step2_container']['tv3_content_type'][$key] = [
              '#type' => 'checkbox',
              '#title' => 'Analysis (' . $ana_count . ')',
            ];
            $no_data = FALSE;
          }
        }
        else {
          if ($table == 'project') {
            $sql =
              "SELECT count(*)
           FROM {project} P
           INNER JOIN [chado_project] CP ON P.project_id = CP.project_id
          ";
            $proj_count = chado_query($sql)->fetchField();
            if ($proj_count > 0) {
              $key = urlencode('tv3_content_type--local--project--project');
              $form['step2']['step2_container']['tv3_content_type'][$key] = [
                '#type' => 'checkbox',
                '#title' => 'Project (' . $proj_count . ')',
              ];
              $no_data = FALSE;
            }
          }
          else {
            if ($table == 'featuremap') {
              $sql =
                "SELECT count(*)
            FROM {featuremap} M
            INNER JOIN [chado_featuremap] CM ON M.featuremap_id = CM.featuremap_id
          ";
              $map_count = chado_query($sql)->fetchField();
              if ($map_count > 0) {
                $key = urlencode('tv3_content_type--data--1274--map');
                $form['step2']['step2_container']['tv3_content_type'][$key] = [
                  '#type' => 'checkbox',
                  '#title' => 'Map (' . $map_count . ')',
                ];
                $no_data = FALSE;
              }
            }
            else {
              if ($table == 'pub') {
                $sql =
                  "SELECT count(*)
           FROM {pub} P
           INNER JOIN [chado_pub] CP ON P.pub_id = CP.pub_id
         ";
                $proj_count = chado_query($sql)->fetchField();
                if ($proj_count > 0) {
                  $key = urlencode('tv3_content_type--TPUB--0000002--Publication');
                  $form['step2']['step2_container']['tv3_content_type'][$key] = [
                    '#type' => 'checkbox',
                    '#title' => 'Publication (' . $proj_count . ')',
                  ];
                  $no_data = FALSE;
                }
              }
              else {
                if ($table == 'biomaterial') {
                  $sql =
                    "SELECT count(*)
           FROM {biomaterial} B
           INNER JOIN [chado_biomaterial] CB ON B.biomaterial_id = CB.biomaterial_id
          ";
                  $bm_count = chado_query($sql)->fetchField();
                  if ($bm_count > 0) {
                    $key = urlencode('tv3_content_type--sep--1095--biological sample');
                    $form['step2']['step2_container']['tv3_content_type'][$key] = [
                      '#type' => 'checkbox',
                      '#title' => 'Biomaterial (' . $bm_count . ')',
                    ];
                    $no_data = FALSE;
                  }
                }
                else {
                  if (key_exists('cvterm', $fkeys) && key_exists('type_id', $fkeys['cvterm']['columns'])) {
                    // Get all Tripal v2 node types from the chad_* linking table
                    $sql =
                      "SELECT V.name AS type, X.accession, db.name AS vocabulary , count(*) AS num
              FROM {" . $table . "} T
              INNER JOIN [" . $tv2_content_type . "] CT ON T.$pkey = CT.$pkey
              INNER JOIN {cvterm} V ON V.cvterm_id = T.type_id
              INNER JOIN {dbxref} X ON X.dbxref_id = V.dbxref_id
              INNER JOIN {db} ON db.db_id = X.db_id
              GROUP BY V.name, X.accession, db.name";
                    $tv3_content_types = chado_query($sql);
                    while ($tv3_content_type = $tv3_content_types->fetchObject()) {
                      // We need to store vocabulary/accession/type for each checkbox in the key becuase
                      // the value only allows 1 or 0
                      $key = urlencode(
                        'tv3_content_type--' .
                        $tv3_content_type->vocabulary . '--' .
                        $tv3_content_type->accession . '--' .
                        $tv3_content_type->type);
                      $form['step2']['step2_container']['tv3_content_type'][$key] = [
                        '#type' => 'checkbox',
                        '#title' => $tv3_content_type->type . ' (' . $tv3_content_type->num . ')',
                      ];
                      $no_data = FALSE;
                    }
                  }
                }
              }
            }
          }
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
    $form['step2']['step2_container']['migrate_btn'] = [
      '#type' => 'submit',
      '#name' => 'migrate_btn',
      '#value' => "Migrate $tv2_options[$tv2_content_type]",
    ];
    // Disable the migration if all have been done
    if (count($tv2_options) == 1 && key_exists('all', $tv2_options)) {
      $form['step2']['step2_container']['migrate_btn']['#disabled'] = 1;
    }
  }

  // Step 3
  $form['step3'] = [
    '#type' => 'fieldset',
    '#title' => 'Step3',
    '#description' => '<b>Use Legacy Templates (optional)</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs',
  ];
  $form['step3']['instructions'] = [
    '#type' => 'item',
    '#markup' => t('Once content types have been migrated you have the option
        to use the Tripal v2 display templates.  Tripal v3 allows you to
        customize the page layout using a web interface whereas Tripal v2
        required programatic changes to template files. If your site has
        considerable Tripal v2 customizations that you do not
        want to lose you can use the legacy templates by checking the box
        for the desired content types below.'),
  ];
  // Get a list of enabled legacy modules with tv2 templates
  $enabled_templates = variable_get('tripal_chado_enabled_legacy_templates', []);
  $info = module_invoke_all('node_info');
  $options = tripal_chado_get_tripal_v2_content_type_options(FALSE, TRUE);
  foreach ($options AS $type_name => $type_label) {
    $title = $type_label;

    if ($type_name != 'all' and !key_exists($type_name, $info)) {
      $title .= " <strong>(please enable the $type_name legacy module).</strong>";
    }
    $form ['step3']['legacy_template--' . $type_name] = [
      '#type' => 'checkbox',
      '#title' => $title,
      '#default_value' => key_exists('legacy_template--' . $type_name, $enabled_templates) ? $enabled_templates['legacy_template--' . $type_name] : 0,
    ];
  }
  $form['step3']['save_btn'] = [
    '#type' => 'submit',
    '#name' => 'save_enabled_template_btn',
    '#value' => "Save",
  ];

  // Step 4
  $form['step4'] = [
    '#type' => 'fieldset',
    '#title' => 'Step4',
    '#description' => '<b>Complete Migration</b>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'overview_vert_tabs',
  ];
  $form['step4']['instructions'] = [
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
  ];
  $form['step4']['warning'] = [
    '#type' => 'item',
    '#markup' => tripal_set_message('Any of the following options cannot be
        undone. Also, please be sure you have migrated all
        desired content types in Step 2 prior to deleting the Tripal v2
        contents.', TRIPAL_WARNING, ['return_html' => TRUE]),
  ];
  $opt_complete_migration = [
    'cp_title' => 'Copy Title over to Tripal v3 Content',
    'mv_url' => 'Migrate URL Alias to Tripal v3 Content',
    'unpublish' => 'Unpublish Tripal v2 Content',
    'delete' => 'Delete Tripal v2 Content',
  ];
  if (count($tv2_options) == 1 && key_exists('all', $tv2_options)) {
    $form['step4']['complete'] = [
      '#markup' => 'Migration completed. All content have been migrated.',
    ];
  }
  else {
    foreach ($tv2_options AS $opt_key => $opt) {
      $form['step4'][$opt_key . '_title'] = [
        '#markup' => "<b>$opt</b>",
      ];
      $form['step4']['complete_migration--' . $opt_key] = [
        '#type' => 'checkboxes',
        '#options' => $opt_complete_migration,
      ];
    }
    $form['step4']['submit_btn'] = [
      '#type' => 'submit',
      '#name' => 'complete_migration_btn',
      '#value' => "Submit",
    ];
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
    $enabled_templates = [];
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
  else {
    if ($form_state['clicked_button']['#name'] == 'complete_migration_btn') {
      $values = $form_state['values'];
      $config = [
        'delete' => [],
        'unpublish' => [],
        'cp_title' => [],
        'mv_url' => [],
      ];
      $all = [];
      $all_types = [];
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
      foreach ($all AS $a) {
        $config[$a] = $all_types;
      }
      // Submit jobs to complete the migration
      global $user;
      $includes = [
        module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.migrate'),
      ];

      if (count($config['cp_title']) > 0) {
        $args = [$config['cp_title']];
        tripal_add_job("Copy Title over to Tripal v3 Content", 'tripal_chado',
          'tripal_chado_copy_title_for_selected_types', $args, $user->uid, 10, $includes);
      }
      if (count($config['mv_url']) > 0) {
        $args = [$config['mv_url']];
        tripal_add_job(" Migrate URL Alias to Tripal v3 Content ", 'tripal_chado',
          'tripal_chado_migrate_url_alias_for_selected_types', $args, $user->uid, 10, $includes);
      }
      if (count($config['unpublish']) > 0) {
        $args = [$config['unpublish']];
        tripal_add_job(" Unpublish Tripal v2 Content ", 'tripal_chado',
          'tripal_chado_unpublish_selected_types', $args, $user->uid, 10, $includes);
      }
      if (count($config['delete']) > 0) {
        $args = [$config['delete']];
        tripal_add_job("Delete Tripal v2 Content ", 'tripal_chado',
          'tripal_chado_delete_selected_types', $args, $user->uid, 10, $includes);
      }
      $_SESSION['tripal_migrate_current_tab'] = 'edit-step4';
      return;
    }
    // Migrate tv2 content to tv3 content
    else {
      if ($form_state['clicked_button']['#name'] == 'migrate_btn') {
        global $user;
        $values = $form_state['values'];
        $tv2_content_type = $form_state['values']['tv2_content_type'];
        $tv3_content_type = [];
        foreach ($values AS $key => $value) {
          if ($tv2_content_type != 'all') {
            $key = urldecode($key);
            if (preg_match('/^tv3_content_type--(.+)--(.+)--(.+)/', $key, $matches) &&
              ($value == 1 || $values['tv3_migrate_all'] == 1)) {
              $vocabulary = $matches[1];
              $accession = $matches[2];
              $type = $matches[3];
              $table = str_replace('chado_', '', $tv2_content_type);
              $tv3_content_type [] = [
                'vocabulary' => $vocabulary,
                'accession' => $accession,
                'term_name' => $type,
                'storage_args' => [
                  'data_table' => $table,
                  'type_column' => db_field_exists('chado.' . $table, 'type_id') ? 'type_id' : '',
                ],
              ];
            }
          }
        }
        // Submit a job to migrate content
        global $user;
        $args = [
          [
            'tv2_content_type' => $tv2_content_type,
            'tv3_content_type' => $tv3_content_type,
          ],
        ];
        $includes = [
          module_load_include('inc', 'tripal_chado', 'includes/tripal_chado.migrate'),
        ];
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
  }
}

/**
 * Ajax call back that returns the entire form
 *
 * The callback is triggered by ajax elements on the form which leads to the
 * update of entire form according to the values set on the form
 *
 * @param $form
 * @param $form_state
 *
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
 * @param boolean $has_template
 *   Return TV2 content types only if it has a base template. This will exclude
 *   TV2 content types such as Blast/Kegg/Interpro/Unigene which are all
 *   converted into the TV3
 *   'Analysis' entity type
 *
 * @return string[]
 *  Return a string array keyed by the node type
 */
function tripal_chado_get_tripal_v2_content_type_options($all_option = FALSE, $has_template = FALSE) {
  // Get all available Tripal v2 chado tables
  $sql = "
    SELECT table_name
    FROM information_schema.tables
    WHERE table_schema = 'public' AND table_name LIKE 'chado_%'
  ";
  $result = db_query($sql);
  // Store 'chado_*' tables that has at least one node
  $tables = [];
  while ($field = $result->fetchField()) {
    $count = db_query("SELECT count(*) FROM $field LIMIT 1")->fetchField();
    if ($count != 0) {
      array_push($tables, $field);
    }
  }

  // List all available Tripal v2 content types
  $result = db_select('node_type', 'nt')
    ->fields('nt', ['type', 'name', 'description'])
    ->condition('type', 'chado_%', 'LIKE')
    ->execute();
  $options = [];
  if ($all_option) {
    $options['all'] = 'All';
  }
  while ($obj = $result->fetchObject()) {
    // Ignore analysis sub modules
    if ($has_template && !preg_match('/^chado_analysis_.+$/', $obj->type)) {
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
  $types = [];
  foreach ($tv2_content_types AS $tv2_content_type => $value) {
    $table = str_replace('chado_', '', $tv2_content_type);
    $schema = chado_get_schema($table);
    $pkey = $schema['primary key'][0];
    $fkeys = $schema['foreign keys'];

    if ($table == 'organism') {
      array_push($types, [
        'vocabulary' => 'OBI',
        'accession' => '0100026',
        'term_name' => 'organism',
        'storage_args' => [
          'data_table' => $table,
        ],
      ]);
    }
    else {
      if ($table == 'analysis') {
        array_push($types, [
          'vocabulary' => 'operation',
          'accession' => '2945',
          'term_name' => 'Analysis',
          'storage_args' => [
            'data_table' => $table,
          ],
        ]);
      }
      else {
        if ($table == 'project') {
          array_push($types, [
            'vocabulary' => 'local',
            'accession' => 'project',
            'term_name' => 'project',
            'storage_args' => [
              'data_table' => $table,
            ],
          ]);
        }
        else {
          if ($table == 'featuremap') {
            array_push($types, [
              'vocabulary' => 'data',
              'accession' => '1274',
              'term_name' => 'map',
              'storage_args' => [
                'data_table' => $table,
              ],
            ]);
          }
          else {
            if ($table == 'pub') {
              array_push($types, [
                'vocabulary' => 'TPUB',
                'accession' => '0000002',
                'term_name' => 'Publication',
                'storage_args' => [
                  'data_table' => $table,
                ],
              ]);
            }
            else {
              if ($table == 'biomaterial') {
                array_push($types, [
                  'vocabulary' => 'sep',
                  'accession' => '1095',
                  'term_name' => 'biological sample',
                  'storage_args' => [
                    'data_table' => $table,
                  ],
                ]);
              }
              else {
                if (key_exists('cvterm', $fkeys) && key_exists('type_id', $fkeys['cvterm']['columns'])) {
                  // Get all Tripal v2 node types from the chad_* linking table
                  $sql = "
        SELECT V.name AS type, X.accession, db.name AS vocabulary
        FROM {" . $table . "} T
          INNER JOIN [" . $tv2_content_type . "] CT ON T.$pkey = CT.$pkey
          INNER JOIN {cvterm} V ON V.cvterm_id = T.type_id
          INNER JOIN {dbxref} X ON X.dbxref_id = V.dbxref_id
          INNER JOIN {db} ON db.db_id = X.db_id
          GROUP BY V.name, X.accession, db.name
          ";
                  $tv3_content_types = chado_query($sql);
                  while ($tv3_content_type = $tv3_content_types->fetchObject()) {
                    array_push($types, [
                      'vocabulary' => $tv3_content_type->vocabulary,
                      'accession' => $tv3_content_type->accession,
                      'term_name' => $tv3_content_type->type,
                      'storage_args' => [
                        'data_table' => $table,
                        'type_column' => db_field_exists('chado.' . $table, 'type_id') ? 'type_id' : '',
                      ],
                    ]);
                  }
                }
              }
            }
          }
        }
      }
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

  // Initialize the population of the chado_cvterm_mapping table before migration.
  tripal_chado_map_cvterms();

  foreach ($tv3_content_types AS $tv3_content_type) {
    // Check if the term already exists
    $term = tripal_load_term_entity($tv3_content_type);
    // If term doesn't exist, create a new bundle for this term
    if (!$term) {
      print("Creating bundle for term '" . $tv3_content_type['term_name'] . "'...\n");
      $success = tripal_create_bundle($tv3_content_type);
      $term = tripal_load_term_entity($tv3_content_type);
    }
    // Create bundle name
    $bundle_name = 'bio_data_' . $term->id;

    // Publish records for the bundle
    $value = [
      'sync_node' => 1,
      'bundle_name' => $bundle_name,
    ];
    chado_publish_records($value);

    // Migrate Resource Titles/Blocks or Resource Links if available
    tripal_chado_migrate_resource_blocks($bundle_name);
    tripal_chado_migrate_resource_links($bundle_name);

    // Migrate organism images
    if ($term->name == 'organism') {
      tripal_chado_migrate_organism_images($bundle_name);
    }
  }
}

/**
 * Delete selected Tripal v2 content
 *
 * Delete all records from chado_* table then call the cleanup orphan nodes
 * function
 *
 * @param unknown $tv2_content_types
 */
function tripal_chado_delete_selected_types($tv2_content_types = [], $job_id) {
  foreach ($tv2_content_types AS $type) {
    $sql = "DELETE FROM $type";
    db_query($sql);
    chado_cleanup_orphaned_nodes(str_replace('chado_', '', $type), 25000, NULL, NULL, $job_id);
  }
}

/**
 * Unpublish selected Tripal v2 content
 *
 * Set status = 0 (unpublished) for all nodes of selected Tripal v2 content
 * types
 *
 * @param unknown $tv2_content_types
 */
function tripal_chado_unpublish_selected_types($tv2_content_types = [], $job_id) {
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
function tripal_chado_copy_title_for_selected_types($tv2_content_types = [], $job_id) {
  foreach ($tv2_content_types AS $type) {
    $chado_table = str_replace('chado_', '', $type);
    $bio_data_tables = tripal_chado_migrate_get_biodata_tables($chado_table);
    foreach ($bio_data_tables AS $bio_data_table) {
      $sql = "SELECT nid, entity_id FROM $bio_data_table WHERE nid IN (SELECT nid FROM $type)";
      $result = db_query($sql);
      while ($entity = $result->fetchObject()) {
        $usql = "
            UPDATE tripal_entity
            SET title = (SELECT title FROM node WHERE nid = :nid)
            WHERE id = :entity_id";
        db_query($usql, [
            ':nid' => $entity->nid,
            ':entity_id' => $entity->entity_id,
          ]
        );
      }
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
function tripal_chado_migrate_url_alias_for_selected_types($tv2_content_types = [], $job_id) {
  foreach ($tv2_content_types AS $type) {
    $chado_table = str_replace('chado_', '', $type);
    $bio_data_tables = tripal_chado_migrate_get_biodata_tables($chado_table);
    foreach ($bio_data_tables AS $bio_data_table) {
      $sql = "SELECT nid, entity_id FROM $bio_data_table WHERE nid IN (SELECT nid FROM $type)";
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
}

/**
 * Migrate images for Tripal v2 organism content
 *
 * Migrate images for all chado_organism
 *
 */
function tripal_chado_migrate_organism_images($bundle_name) {
  // Get all organism entities
  $bio_data_table = array_pop(tripal_chado_migrate_get_biodata_tables('organism'));
  $results =
    db_select($bio_data_table, 'ce')
      ->fields('ce', ['entity_id', 'record_id', 'nid'])
      ->execute();

  // Iterate through each organism entity
  while ($organism = $results->fetchObject()) {
    $nid = $organism->nid;
    $entity_id = $organism->entity_id;

    // check if there is a file record for the organism node
    $fid = db_select('file_usage', 'fu')
      ->fields('fu', ['fid'])
      ->condition('module', 'tripal_organism')
      ->condition('type', 'organism_image')
      ->condition('id', $nid)
      ->execute()
      ->fetchField();
    // check if the image was added using the old interface.
    if (!$fid) {
      $sql =
        "SELECT genus,species,nid
          FROM {organism} O
          INNER JOIN chado_organism CO ON O.organism_id = CO.organism_id
          WHERE O.organism_id = :organism_id";
      $chado_org = chado_query($sql, [':organism_id' => $organism->record_id])->fetchObject();

      if ($chado_org) {
        $base_path = realpath('.');
        $image_dir = tripal_get_files_dir('tripal_organism') . "/images";
        $image_name = $chado_org->genus . "_" . $chado_org->species . ".jpg";
        $image_path = "$base_path/$image_dir/$image_name";
        // image files are stored as 'genus_species.jpg'
        $file = NULL;
        if (file_exists($image_path)) {
          $handle = fopen($image_path, 'r');
          $file = file_save_data($handle, "public://$image_name");
          fclose($handle);
        }
        // last possible case: image files are stored as 'organism_id.jpg'
        else {
          $image_name = $chado_org->nid . ".jpg";
          $image_path = "$base_path/$image_dir/$image_name";
          if (file_exists($image_path)) {
            $handle = fopen($image_path, 'r');
            $file = file_save_data($handle, "public://$image_name");
            fclose($handle);
          }
        }
        if ($file) {
          tripal_chado_migrate_organism_image_add_file($file->fid, $entity_id, $bundle_name);
        }
      }
    }
    else {
      // If there is an image, add it to the organism entity
      tripal_chado_migrate_organism_image_add_file($fid, $entity_id, $bundle_name);
    }

  }
}

/**
 *
 * Add image file for the organism entity
 *
 * @param unknown $fid
 */
function tripal_chado_migrate_organism_image_add_file($fid, $entity_id, $bundle_name) {
  if ($fid && $entity_id) {
    $file = file_load($fid);
    // Add a record to the file_usage table
    file_usage_add($file, 'file', 'TripalEntity', $entity_id);
    $image_file = (array) $file;
    // Attached it to the entity
    /*
    $entities = entity_load('TripalEntity', array($entity_id));
    $entity = $entities[$entity_id];
    $image = array(
      'und' => array(
        0 =>$image_file
      )
    );
    $entity->data__image = $image;
    field_attach_update('TripalEntity', $entity);
    entity_get_controller('TripalEntity')->resetCache(array($entity_id));
    */
    // Add a record to the field_data_data__image table
    $values = [
      'entity_type' => 'TripalEntity',
      'bundle' => $bundle_name,
      'entity_id' => $entity_id,
      'revision_id' => $entity_id,
      'language' => 'und',
      'delta' => 0,
      'data__image_fid' => $fid,
    ];
    drupal_write_record('field_data_data__image', $values);
  }
}

/**
 * Retrieve chado_biodata_* table name
 *
 * @param string $chado_table
 *   the chado_table where the record is stored
 *
 * @return
 *   Return all chado_biodata_* table names mapping to a chado_table
 */
function tripal_chado_migrate_get_biodata_tables($chado_table) {
  // To find the bundle_table, check if type_column is used for the chado_table
  $query = db_select('chado_bundle', 'cb');
  $query->join('tripal_bundle', 'tb', 'tb.id = cb.bundle_id');
  $query->fields('cb', ['data_table']);
  $query->fields('tb', ['name']);
  $query->condition('cb.data_table', $chado_table);
  $bundles = $query->execute();
  $tables = [];
  while ($bundle = $bundles->fetchObject()) {
    array_push($tables, 'chado_' . $bundle->name);
  }
  return $tables;
}

function tripal_chado_migrate_resource_blocks($bundle_name) {
  // Do not try to migrate resouce blocks and titles if they weren't set up
  if (!db_table_exists('field_revision_field_resource_titles') || !db_table_exists('field_revision_field_resource_blocks')) {
    return;
  }
  $entites =
    db_select('chado_' . $bundle_name, 'B')
      ->fields('B', ['nid'])
      ->execute();
  while ($nid = $entites->fetchField()) {
    // Only the latest revision is migrated
    $sql = "
      SELECT
        entity_id,
        max(revision_id) AS vid,
        delta,
        (SELECT field_resource_titles_value
         FROM field_revision_field_resource_titles
         WHERE entity_id = RT.entity_id
         AND revision_id = max(RT.revision_id)
         AND delta = RT.delta
        ),
        (SELECT field_resource_blocks_value
         FROM field_revision_field_resource_blocks
         WHERE entity_id = RT.entity_id
         AND revision_id = max(RT.revision_id)
         AND delta = RT.delta
        )
      FROM field_revision_field_resource_titles RT
      WHERE RT.entity_id = :nid
      GROUP BY entity_id, delta
      ORDER BY RT.delta
    ";
    $results = db_query($sql, [':nid' => $nid]);
    while ($resource = $results->fetchObject()) {
      $title = $resource->field_resource_titles_value;
      $content = $resource->field_resource_blocks_value;
      $delta = $resource->delta;
      $nid = $resource->entity_id;
      $entity_id =
        db_select('chado_' . $bundle_name, 'B')
          ->fields('B', ['entity_id'])
          ->condition('nid', $nid)
          ->execute()
          ->fetchField();
      // field name: (can not be longer than 32 chars)
      // bio_data_<i>_resource_<title to lower case/space replaced with _/first 10 chars>
      $field_name = $bundle_name . '_rsc_' . substr(preg_replace('/\s+/', '_', strtolower($title)), 0, 15);
      // Create a field if it does not exist
      if (!field_info_field($field_name)) {
        field_create_field([
          'field_name' => $field_name,
          'type' => 'text',
          'cardinality' => 1,
          'locked' => FALSE,
          'storage' => [
            'type' => 'field_sql_storage',
          ],
          'settings' => [
            'max_length' => 10485760,
            'text_processing' => 1,
          ],
        ]);
      }
      // Create field instance for the bundle if it does not exist
      if (!field_info_instance('TripalEntity', $field_name, $bundle_name)) {
        field_create_instance([
          'field_name' => $field_name,
          'entity_type' => 'TripalEntity',
          'bundle' => $bundle_name,
          'label' => $title,
          'widget' => [
            'type' => 'text_textarea',
          ],
          'display' => [
            'default' => [
              'label' => 'hidden',
            ],
          ],
          'settings' => [
            'text_processing' => 1,
            'format' => 'full_html',
            'term_vocabulary' => 'schema',
            'term_name' => 'comment',
            'term_accession' => 'comment',
          ],
        ]);
      }
      // Migrate the field content
      $ftable = 'field_data_' . $field_name;
      $frtable = 'field_revision_' . $field_name;
      $fvalue = $field_name . '_value';
      $fformat = $field_name . '_format';
      try {
        $sql = "
          INSERT INTO $ftable (entity_type, bundle, entity_id, revision_id, language, delta, $fvalue, $fformat)
          VALUES (:entity_type, :bundle, :entity_id, :revision_id, :language, :delta, :value, :format)
        ";
        db_query($sql,
          [
            ':entity_type' => 'TripalEntity',
            ':bundle' => $bundle_name,
            ':entity_id' => $entity_id,
            'revision_id' => $entity_id,
            ':language' => 'und',
            ':delta' => 0,
            ':value' => $content,
            ':format' => 'full_html',
          ]
        );
        $rsql = "
        INSERT INTO $frtable (entity_type, bundle, entity_id, revision_id, language, delta, $fvalue, $fformat)
        VALUES (:entity_type, :bundle, :entity_id, :revision_id, :language, :delta, :value, :format)
        ";
        db_query($rsql,
          [
            ':entity_type' => 'TripalEntity',
            ':bundle' => $bundle_name,
            ':entity_id' => $entity_id,
            'revision_id' => $entity_id,
            ':language' => 'und',
            ':delta' => 0,
            ':value' => $content,
            ':format' => 'full_html',
          ]
        );
      } catch (\PDOException $e) {
        $error = $e->getMessage();
        watchdog_exception('tripal_chado', $e);
      }
    }
  }
}

function tripal_chado_migrate_resource_links($bundle_name) {
  // Do not try to migrate resouce blocks and titles if they weren't set up
  if (!db_table_exists('field_revision_field_resource_links')) {
    return;
  }
  $entites =
    db_select('chado_' . $bundle_name, 'B')
      ->fields('B', ['nid'])
      ->execute();
  while ($nid = $entites->fetchField()) {
    // Only the latest revision is migrated
    $sql = "
      SELECT
        entity_id,
        max(revision_id) AS vid,
        delta,
        (SELECT field_resource_links_value
         FROM field_revision_field_resource_links
         WHERE entity_id = RT.entity_id
         AND revision_id = max(RT.revision_id)
         AND delta = RT.delta
        )
      FROM field_revision_field_resource_links RT
      WHERE RT.entity_id = :nid
      GROUP BY entity_id, delta
      ORDER BY RT.delta
    ";
    $results = db_query($sql, [':nid' => $nid]);
    while ($resource = $results->fetchObject()) {
      $values = explode('|', $resource->field_resource_links_value);
      $title = $values[0];
      $link = $values[1];
      $delta = $resource->delta;
      $nid = $resource->entity_id;
      $entity_id =
        db_select('chado_' . $bundle_name, 'B')
          ->fields('B', ['entity_id'])
          ->condition('nid', $nid)
          ->execute()
          ->fetchField();

      // field name: (can not be longer than 32 chars)
      // bio_data_<i>_resource_<title to lower case/space replaced with _/first 10 chars>
      $field_name = $bundle_name . '_resource_links';
      // Create a field if it does not exist
      if (!field_info_field($field_name)) {
        field_create_field([
          'field_name' => $field_name,
          'type' => 'link_field',
          'cardinality' => FIELD_CARDINALITY_UNLIMITED,
          'locked' => FALSE,
          'storage' => [
            'type' => 'field_sql_storage',
          ],
          'settings' => [
          ],
        ]);
      }
      // Create field instance for the bundle if it does not exist.
      if (!field_info_instance('TripalEntity', $field_name, $bundle_name)) {
        field_create_instance([
          'field_name' => $field_name,
          'entity_type' => 'TripalEntity',
          'bundle' => $bundle_name,
          'label' => 'Links',
          'widget' => [
            'type' => 'link_field',
          ],
          'display' => [
            'default' => [
              'label' => 'hidden',
            ],
          ],
          'settings' => [
            'term_vocabulary' => 'schema',
            'term_name' => 'url',
            'term_accession' => 'url',
            'absolute_url' => 0,
            'validate_url' => 0,
          ],
        ]);
      }
      // Migrate the field content
      $ftable = 'field_data_' . $field_name;
      $frtable = 'field_revision_' . $field_name;
      $furl = $field_name . '_url';
      $ftitle = $field_name . '_title';
      try {
        $sql = "
        INSERT INTO $ftable (entity_type, bundle, entity_id, revision_id, language, delta, $furl, $ftitle)
        VALUES (:entity_type, :bundle, :entity_id, :revision_id, :language, :delta, :url, :title)
        ";
        db_query($sql,
          [
            ':entity_type' => 'TripalEntity',
            ':bundle' => $bundle_name,
            ':entity_id' => $entity_id,
            'revision_id' => $entity_id,
            ':language' => 'und',
            ':delta' => $delta,
            ':url' => $link,
            ':title' => $title,
          ]
        );
        $rsql = "
        INSERT INTO $frtable (entity_type, bundle, entity_id, revision_id, language, delta, $furl, $ftitle)
        VALUES (:entity_type, :bundle, :entity_id, :revision_id, :language, :delta, :url, :title)
        ";
        db_query($rsql,
          [
            ':entity_type' => 'TripalEntity',
            ':bundle' => $bundle_name,
            ':entity_id' => $entity_id,
            'revision_id' => $entity_id,
            ':language' => 'und',
            ':delta' => $delta,
            ':url' => $link,
            ':title' => $title,
          ]
        );
      } catch (\PDOException $e) {
        $error = $e->getMessage();
        watchdog_exception('tripal_chado', $e);
      }
    }
  }
}
