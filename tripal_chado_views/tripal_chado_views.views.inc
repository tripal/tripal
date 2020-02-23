<?php
/**
 * @file
 * Chado Views Integration
 */

include 'views/handlers/tripal_views_handler_area_action_links.inc';

/**
 * Implements hook_views_handlers().
 *
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return
 *   An array of handler definitions
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_views_handlers() {

  return [
    'info' => [
      'path' => drupal_get_path('module', 'tripal_chado_views') . '/views/handlers',
    ],
    'handlers' => [

      // Custom Tripal Field Handlers
      /** CANT FIX UNTIL Tripal Feature is working
       * 'tripal_views_handler_field_sequence' => array(
       * 'parent' => 'views_handler_field',
       * ),
       */

      // Custom area handler
      'tripal_views_handler_area_action_links' => [
        'parent' => 'views_handler_area',
      ],

      // Custom Tripal Filter Handlers
      'tripal_views_handler_filter_no_results' => [
        'parent' => 'views_handler_filter',
      ],
      'tripal_views_handler_filter_select_cvterm' => [
        'parent' => 'tripal_views_handler_filter_select_string',
      ],
      'tripal_views_handler_filter_select_id' => [
        'parent' => 'tripal_views_handler_filter_select_string',
      ],
      'tripal_views_handler_filter_select_string' => [
        'parent' => 'views_handler_filter_string',
      ],
      'tripal_views_handler_filter_textarea' => [
        'parent' => 'views_handler_filter',
      ],
      'tripal_views_handler_filter_file_upload' => [
        'parent' => 'views_handler_filter',
      ],
      /** D7 @todo: get handlers working
       */
      /** CANT FIX UNTIL Tripal Feature is working
       * 'tripal_views_handler_filter_sequence' => array(
       * 'parent' => 'chado_views_handler_filter_string',
       * ),
       */
    ],
  ];
}

/**
 * Implements hook_views_pre_render().
 *
 * Purpose: Intercepts the view after the query has been executed
 *   All the results are stored in $view->result
 *   Looking up the NID here ensures the query is only executed once
 *   for all stocks in the table.
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_views_pre_render(&$view) {

  // We need to unset the exposed_input for the view so we can repopulate that
  // variable. This is necessary if we're using the file_upload_combo
  // custom form element which adds the file_path variable to the $_GET after the
  // view has populated the $view->exposed_input variable
  unset($view->exposed_input);

  // Add css if tripal admin tagged view
  if ($view->tag == 'tripal admin') {
    $tripal_admin_view_css = drupal_get_path('module', 'tripal_chado_views') . '/theme/css/tripal_views_admin_views.css';
    drupal_add_css($tripal_admin_view_css);
  }

}

/**
 * Implements hook_views_data().
 *
 * Generates a dynamic data array for Views
 *
 * This function is a hook used by the Views module. It populates and
 *   returns a data array that specifies for the Views module the base table,
 *   the tables it joins with and handlers.  The data array is populated
 *   using the data stored in the tripal_views tables.
 *
 * @return a data array formatted for the Views module
 *
 * D7 @todo: Add support for materialized views relationships using the new
 *   method
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_views_data() {
  $data = [];

  // Manually integrate the drupal.tripal_views* tables
  $data = tripal_chado_views_views_data_tripal_views_tables($data);

  // Determine the name of the chado schema
  $chado_schema = chado_get_schema_name('chado');

  // MAKE SURE ALL CHADO TABLES ARE INTEGRATED
  tripal_chado_views_integrate_all_chado_tables();

  // DEFINE GLOBAL FIELDS
  // Filter handler that lets the admin say:
  // "Show no results until they enter search parameters"
  $data['views']['search_results'] = [
    'title' => t('Delay Results'),
    'help' => t('Delay results until Apply/Search is clicked by the user.'),
    'filter' => [
      'handler' => 'tripal_views_handler_filter_no_results',
    ],
  ];

  $data['views']['action_links_area'] = [
    'title' => t('Action Links'),
    'help' => t('Add action links to the view.'),
    'area' => [
      'handler' => 'tripal_views_handler_area_action_links',
    ],
  ];

  $tvi_query = db_query('SELECT * FROM {tripal_views}');

  // INTEGRATE THE LIGHTEST SETUP FOR EACH TABLE
  foreach ($tvi_query as $tvi_row) {

    // check to see if this is the lightest (drupal-style) priority setup for this table
    // if not then don't use this definition
    $lightest_priority_setup = tripal_is_lightest_priority_setup($tvi_row->setup_id, $tvi_row->table_name);
    if (!$lightest_priority_setup) {
      continue;
    }

    // ids we'll use for queries
    $setup_id = $tvi_row->setup_id;
    $mview_id = $tvi_row->mview_id;

    // holds the base table name and fields
    $base_table = '';
    $base_fields = [];

    // indicated whether the current table is a base table or not
    $is_base_table = $tvi_row->base_table;

    // POPULATE THE BASE TABLE NAME AND FIELDS
    // If an $mview_id is given then get the materialized view info,
    // otherwise get the Chado table info
    $legacy_mview = FALSE;
    if ($mview_id) {

      // get the base table name from the materialized view
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT name, mv_specs FROM {tripal_mviews} WHERE mview_id = :id";
      $mview_table = db_query($sql, [':id' => $mview_id]);
      $mview_table = $mview_table->fetchObject();
      if ($mview_table) {
        $base_table = $mview_table->name;

        if (!empty($mview_table->mv_specs)) {
          $legacy_mview = TRUE;
        }
      }
      else {
        continue;
      }
    }

    if ($legacy_mview) {
      // get the columns in this materialized view.  They are separated by commas
      // where the first word is the column name and the rest is the type
      $columns = explode(",", $mview_table->mv_specs);
      foreach ($columns as $column) {
        $column = trim($column);  // trim trailing and leading spaces
        preg_match("/^(.*?)\ (.*?)$/", $column, $matches);
        $column_name = $matches[1];
        $column_type = $matches[2];
        $base_fields[$column_name] = [
          'column_name' => $column_name,
          'type' => $column_type,
        ];
      }

      // get the field name and descriptions
      // D7 TODO: Check DBTNG changes work
      $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=:setup";
      $query = db_query($sql, [':setup' => $setup_id]);
      foreach ($query as $field) {
        $base_fields[$field->column_name]['name'] = $field->name;
        $base_fields[$field->column_name]['help'] = $field->description;
      }
    }
    // if this is not a legacy materialized view
    else {
      $base_table = $tvi_row->table_name;

      // get the table description
      $table_desc = chado_get_schema($base_table);

      $fields = $table_desc['fields'];
      if (!is_array($fields)) {
        $fields = [];
      }
      foreach ($fields as $column => $attrs) {
        $base_fields[$column] = [
          'column_name' => $column,
          // Add a default for type since module developers may sometimes need to use a
          // PostgreSQL-specific type.
          'type' => (isset($attrs['type'])) ? $attrs['type'] : 'text',
        ];
        // If PostgreSQL-specifc types are needed the module developer should be given
        // a way to set the most closely matching type for views. They should also be
        // allowed to override the type for views. This can be done by adding a views_type
        // to the schema definition :-).
        if (isset($attrs['views_type'])) {
          $base_fields[$column]['type'] = $attrs['views_type'];
        }
        // Tell admin about this feature and warn them that we made an assumption for them.
        if (!isset($attrs['type']) AND !isset($attrs['views_type'])) {
          tripal_report_error(
            'tripal_views',
            TRIPAL_WARNING,
            "Unable to determine the type for %column thus we have defaulted to type 'text'.
              Tip: This may be due to setting a  postgresql-specific type. Solution: Add a
              'views_type' to your schema definition to specify what type should be used.",
            ['%column' => $column]
          );
        }
      }

      // get the field name and descriptions
      $sql = "SELECT * FROM {tripal_views_field} WHERE setup_id=:setup";
      $query = db_query($sql, [':setup' => $setup_id]);
      foreach ($query as $field) {
        $base_fields[$field->column_name]['name'] = $field->name;
        $base_fields[$field->column_name]['help'] = $field->description;
      }
    }

    // SETUP THE BASE TABLE INFO IN THE DATA ARRAY
    $data[$base_table]['table']['group'] = t("$tvi_row->name");

    if ($is_base_table) {
      $data[$base_table]['table']['base'] = [
        'group' => "$tvi_row->name",
        'title' => "$tvi_row->name",
        'help' => $tvi_row->comment,
        'search_path' => $chado_schema,
      ];
    }
    else {
      $data[$base_table]['table'] = [
        'group' => "$tvi_row->name",
        'title' => "$tvi_row->name",
        'help' => $tvi_row->comment,
        'search_path' => $chado_schema,
      ];
    }

    // ADD THE FIELDS TO THE DATA ARRAY
    foreach ($base_fields as $column_name => $base_field) {
      if (!isset($base_field['name'])) {
        $data[$base_table][$column_name] = [
          'title' => t($column_name),
          'help' => t($column_name),
          'field' => [
            'click sortable' => TRUE,
          ],
        ];

        tripal_report_error(
          'tripal_chado_views',
          TRIPAL_DEBUG,
          "The name and help were not set for %table.%column. As a consequence the column
            name has been used... You should ensure that the 'name' and 'help' keys for
            this field are set in the integration array (priority = %priority)",
          [
            '%table' => $base_table,
            '%column' => $column_name,
            '%priority' => $tvi_row->priority,
          ]
        );
      }
      else {
        $data[$base_table][$column_name] = [
          'title' => t($base_field['name']),
          'help' => t($base_field['help']),
          'field' => [
            'click sortable' => TRUE,
          ],
        ];
      }

      // now add the handlers
      $sql = "SELECT * FROM {tripal_views_handlers} WHERE setup_id = :setup AND column_name = :column";
      $handlers = db_query($sql, [
        ':setup' => $setup_id,
        ':column' => $column_name,
      ]);
      $num_handlers = 0;
      foreach ($handlers as $handler) {
        $num_handlers++;

        $data[$base_table][$column_name][$handler->handler_type]['handler'] = $handler->handler_name;

        // Add in any additional arguments
        // This should be a serialized array including (at a minimum) name => <handler name>
        if ($handler->arguments) {
          $data[$base_table][$column_name][$handler->handler_type] = array_merge($data[$base_table][$column_name][$handler->handler_type], unserialize($handler->arguments));
        }
      };

      // If there were no handlers applied to the current field then warn the administrator!
      if ($num_handlers == 0) {
        tripal_report_error(
          'tripal_chado_views',
          TRIPAL_DEBUG,
          "No handlers were registered for %table.%column. This means there is no views
            functionality for this column. To register handlers, make sure the 'handlers'
            key for this field is set in the integration array (priority = %priority).
            The supported keys include 'field', 'filter', 'sort', 'relationship'. Look
            at the tripal_add_views_integration() for additional details.",
          [
            '%table' => $base_table,
            '%column' => $column_name,
            '%priority' => $tvi_row->priority,
          ]
        );
      }
    }

    // ADD JOINS & RELATIONSHIPS TO DATA ARRAY
    // only add joins if this is a base table
    // this keeps the list of available fields manageable
    // all other tables should be added via relationships
    $sql = "SELECT * FROM {tripal_views_join} WHERE setup_id = :setup";
    $joins = db_query($sql, [':setup' => $setup_id]);
    if (!isset($joins)) {
      $joins = [];
    }
    foreach ($joins as $join) {
      $left_table = $join->left_table;
      $left_field = $join->left_field;
      $base_table = $join->base_table;
      $base_field = $join->base_field;
      $handler = $join->handler;
      $base_title = ucwords(str_replace('_', ' ', $base_table));
      $left_title = ucwords(str_replace('_', ' ', $left_table));

      // if the 'node' table is in our integrated list then
      // we want to skip it. It shouldn't be there.
      if ($left_table == 'node') {
        continue;
      }

      // add join entry
      if (!$join->relationship_only) {
        $data[$left_table]['table']['join'][$base_table] = [
          'left_field' => $base_field,
          'field' => $left_field,
        ];
        if ($handler) {
          $data[$left_table]['table']['join'][$base_table]['handler'] = $handler;
        }
        if (!empty($join->arguments)) {
          array_merge($data[$left_table]['table']['join'][$base_table], unserialize($join->arguments));
        }
      }

      // warn if deprecated method of relationship addition was used (ie: through handlers)
      if (isset($data[$base_table][$base_field]['relationship'])) {
        tripal_report_error('tripal_chado_views', TRIPAL_NOTICE,
          'DEPRECATED: Currently using tripal_views_handlers to store relationship for %base => %left when you should be using tripal_views_joins.',
          ['%base' => $base_table, '%left' => $left_table]);
      }

      // Add relationship entry.
      // NOTE: we use a fake field name to allow us to have multiple
      // relationships for the same field (ie: feature.feature_id has many
      // Many relationships but views only supports a single one).
      $fake_field = $base_field . '_to_' . $left_table;

      // Bug Fix: The old $fake_field used above doesn't take into account
      // multiple relationships to the same left table. To keep backwards
      // compatibility, this old fake_field needs to continue to be used for
      // the LAST recorded relationship. However, for all previously set
      // relationships we can use an improved fake name which takes into
      // account the left field and ensures all relationships for a single
      // left table are not condensed into a single relationship.
      if (array_key_exists($fake_field, $data[$base_table])) {

        // Again, note that we can't just change the fake_name after finding
        // there is more than one relationship because then the FIRST
        // relationship would keep the old fake_name rather than the LAST
        // which keeps backwards compatiblity since the old naming caused all
        // previous relationships be be overridden by the next one.
        // Thus we first need to determine the left field of the previous
        // join for this table combination and then use that to form our
        // improved fake field.
        $previous_left_field = $data[$base_table][$fake_field]['relationship']['base field'];
        $improved_fake_field = $base_field . '_to_' . $left_table . "." . $previous_left_field;
        $data[$base_table][$improved_fake_field] = $data[$base_table][$fake_field];
      }
      $data[$base_table][$fake_field] = [
        'title' => "$base_title.$base_field => $left_title.$left_field",
        'help' => t("Joins @base to @left", [
          '@base' => "$base_title.$base_field",
          '@left' => "$left_title.$left_field",
        ]),
        'relationship' => [
          'handler' => $join->relationship_handler,
          'title' => t("$base_field => $left_title ($left_field)"),
          'label' => t("$base_field => $left_title ($left_field)"),
          'real field' => $base_field,
          'base' => $left_table,
          'base field' => $left_field,
        ],
      ];
      if (!empty($join->arguments)) {
        array_merge($data[$base_table][$fake_field]['relationship'], unserialize($join->arguments));
      }
    }
  }

  return $data;
}

/**
 * Describes the tripal views integration tables to views for the
 * administration views
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_views_data_tripal_views_tables($data) {

  $data['tripal_views']['table']['group'] = t('Chado Views Integration');
  $data['tripal_views']['table']['base'] = [
    'field' => 'setup_id', // This is the identifier field for the view.
    'title' => t('Chado Views Integration'),
    'help' => t('Specifications on how to integrate various tables with Drupal Views'),
    'weight' => -10,
  ];

  // Implicit Join to Materialized Views
  $data['tripal_views']['table']['join'] = [
    'tripal_mviews' => [
      'left_field' => 'mview_id',
      'field' => 'mview_id',
    ],
  ];

  // setup_id
  $data['tripal_views']['setup_id'] = [
    'title' => t('Setup ID'),
    'help' => t('Primary key of the integration'),
    'field' => [
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_numeric',
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  // mview_id
  $data['tripal_views']['mview_id'] = [
    'title' => t('Materialized View ID'),
    'help' => t('The primary key of the Mview integrated.'),
    'field' => [
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_numeric',
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  // base_table
  $data['tripal_views']['base_table'] = [
    'title' => t('Base Table?'),
    'help' => t('Whether the table being integrated should be considered a base table.'),
    'field' => [
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Base Table?'),
      'type' => 'yes-no',
      'use equal' => TRUE,
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  // table_name
  $data['tripal_views']['table_name'] = [
    'title' => t('Chado Table Name'),
    'help' => t('The name of the table being integrated'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // priority
  $data['tripal_views']['priority'] = [
    'title' => t('Priority'),
    'help' => t('The priority of the integration.'),
    'field' => [
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ],
    'filter' => [
      'handler' => 'views_handler_filter_numeric',
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
  ];

  // name
  $data['tripal_views']['name'] = [
    'title' => t('Name'),
    'help' => t('The name of the integration'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  // comment
  $data['tripal_views']['comment'] = [
    'title' => t('Description'),
    'help' => t('Short description or comment about this integration.'),
    'field' => [
      'handler' => 'views_handler_field',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ],
    'sort' => [
      'handler' => 'views_handler_sort',
    ],
    'filter' => [
      'handler' => 'views_handler_filter_string',
    ],
    'argument' => [
      'handler' => 'views_handler_argument_string',
    ],
  ];

  return $data;
}

/**
 * Implements hook_views_data_alter().
 * Used to add Chado <-> Node Joins & Relationships
 *   since you need to add to the $data['node'] to do this
 *
 * @see https://api.drupal.org/api/views/views.api.php/function/hook_views_data/7.x-3.x
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_views_data_alter(&$data) {

  // ADD IN ENTITIES JOINS & RELATIONSHIPS
  // D7 @todo: Create custom handler to allow join from Entity => Base (ie: organism)
  //           with the addition of a single relationship
  // D7 @todo: Create custom handler to allow join from Base (ie: organism)
  //           with the addition of a single relationship
  // D7 @todo: Add support for Mview <-> Entity joins and relationships
  $bundle_query = db_select('chado_bundle', 'CB');
  $bundle_query->fields('CB', array('bundle_id', 'data_table'));
  $bundle_query->join('tripal_bundle', 'TB', 'CB.bundle_id = TB.id');
  $bundle_query->fields('TB', array('name', 'label'));
  $bundle_query->join('tripal_term', 'TT', 'TB.term_id = TT.id');
  $bundle_query->fields('TT', array('accession'));
  $bundle_query->join('tripal_vocab', 'TV', 'TT.vocab_id = TV.id');
  $bundle_query->fields('TV', array('vocabulary'));
  $bundles = $bundle_query->execute();
  // Look for the record ID in the appropriate chado table.
  foreach ($bundles as $bundle) {
    // Ids we'll use for queries.
    // Ex.: 'stock'.
    $base_table = $bundle->data_table;
    // Ex.: 'stock_id'.
    $base_id_field = $base_table . '_id';
    // Ex.:'Chado Stock'.
    $base_table_label = 'Chado ' . ucwords(str_replace('_', ' ', $base_table));
    // Ex.: 'chado_bio_data_21'.
    $linker_table = 'chado_' . $bundle->name;
    // Ex.: 'Germplasm Accession'
    $bundle_label = $bundle->label;
    // Ex.: 'CO_010__0000044'
    $bundle_internal_name = $bundle->vocabulary . '__' . $bundle->accession;

    // Add in joins to the tripal_entity tables if the Chado schema is local.
    $is_local = isset($GLOBALS["chado_is_local"]) && $GLOBALS["chado_is_local"];
    if ($is_local) {
      if (db_table_exists($linker_table)) {
        // Adds in a chado base table => entity relationship.
        // This allows controlled joining to multiple entities per line.
        // Use Case:  link to feature and organism entities on a feature listing.
        // D7 todo: a custom relationship handler to get from feature.organism_id => organism node
        //      without 1st needing to add relationship to organism table.
        $data[$linker_table]['table']['join'][$base_table] = array(
          'left_field' => $base_id_field,
          'field' => 'record_id',
        );
        $data[$linker_table]['table']['join']['tripal_entity'] = array(
          'left_field' => 'id',
          'field' => 'entity_id',
        );
        $data[$linker_table][$base_id_field] = array(
          'group' => $base_table_label,
          'title' => $base_table_label,
          'help' => t(
            "Links @base_table_label to its Tripal Content Type '@bundle_label'.",
            array(
              '@base_table_label' => $base_table_label,
              '@bundle_label' => $bundle_label,
            )
          ),
          'relationship' => array(
            'handler' => 'views_handler_relationship',
            'title' => t(
              "@base_table_label => @bundle_label",
              array(
                '@base_table_label' => $base_table_label,
                '@bundle_label' => $bundle_label,
              )
            ),
            'label' => t(
              "@base_table_label => @bundle_label",
              array(
                '@base_table_label' => $base_table_label,
                '@bundle_label' => $bundle_label,
              )
            ),
            'real field' => 'entity_id',
            'base' => 'tripal_entity',
            'base field' => 'id'
          ),
        );

        // Add Chado fields to a entity-based view
        // This will only be done with relationships.
        $data[$bundle_internal_name][$base_id_field] = array(
          'group' => $bundle_label,
          'title' => $bundle_label,
          'help' => t(
            "Links @bundle_label to @base_table_label.",
            array(
              '@bundle_label' => $bundle_label,
              '@base_table_label' => $base_table_label,
            )
          ),
          'relationship' => array(
            'handler' => 'views_handler_relationship',
            'title' => t(
              "@bundle_label => @base_table_label",
              array(
                '@bundle_label' => $bundle_label,
                '@base_table_label' => $base_table_label,
              )
            ),
            'label' => t(
              "@bundle_label => @base_table_label",
              array(
                '@bundle_label' => $bundle_label,
                '@base_table_label' => $base_table_label,
              )
            ),
            'real field' => 'id',
            'base' => $base_table,
            'base field' => $base_id_field
          ),
        );

      }
    }
  }

  return $data;
}

/**
 * Implementation of hook_views_pre_view().
 *
 * Merge the $_GET and $_POST into the $_GET. This is because
 * Views and Views Data Export modules only uses the $_GET variable but
 * file uploads require $_POST. We need to make sure these two modules
 * have access to everything needed for this view to work properly.
 *
 * @ingroup tripal_chado_views
 */
function tripal_chado_views_views_pre_view(&$view, &$display_id, &$args) {
  $_GET = array_merge($_GET, $_POST);
}
