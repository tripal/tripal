<?php
/**
 * @file
 * Provides API functions that support direct integration of Chado tables with
 * Drupal Views.
 */
/**
 * @defgroup tripal_chado_views_api Chado Views Integration
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions that support direct integration of Chado tables with
 * Drupal Views. This is different from the entity and field integration that
 * Tripal v3 provides.  Here Chado tables are directly integrated.  Tripal
 * provides a web interface that allows site developers to customize how a
 * Chado table is integrate with Views.  However, these functions provide
 * programmatic access to the same functionality.
 * @}
 */

/**
 * Programatically enable view
 *
 * This should be used in a hook_menu definition as the callback to provide a
 * link to enable the view (first example). It can also be called directly if
 * needed (second example).
 *
 * @code
 *  Create a URL that when the user navigates there, a given view will be
 *  enabled.
 *  You will still need to provide a link to this menu item somewhere
 *  appropriate (ie: an admin landing page).
 *
 * function mymodule_menu () {
 * $items = array();
 *
 *  //Create one of these for each of your default views
 *  $items['admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/enable']
 *   = array(
 *  'title' => 'Enable <VIEW-HUMAN-READABLE-NAME>',
 *  'page callback' => 'tripal_enable_view',
 *  'page arguments' => array('<VIEW-MACHINE-NAME>',
 *   '<PATH-TO-REDIRECT-TO-AFTERWARDS>'),
 *  'access arguments' => array('<YOUR-PERMISSION-KEY>'),
 * 'type' => MENU_CALLBACK,
 *  );
 *
 *  return $items;
 * }
 *
 *  Call this function directly to disable a view
 *  The example shows enabling your own default view when your module is
 *   enabled. This might be useful if you disable your view when your module is
 *   disabled. function mymodule_enable() {
 *
 *  $view_name = '<VIEW-MACHINE-NAME>';
 * tripal_enable_view($view_name);
 *
 *  }
 * @endcode
 *
 * @param $view_name
 *   The machine-name of the view to be enabled
 * @param $redirect_link
 *   The path to redirect to. FALSE if no redirect needed
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_enable_view($view_name, $redirect_link = FALSE) {

  $status = variable_get('views_defaults', []);
  if (isset($status[$view_name])) {
    $status[$view_name] = FALSE;
    variable_set('views_defaults', $status);
    drupal_set_message("Successfully Enabled $view_name");
  }
  else {
    drupal_set_message("Unable to find a view by the name of '$view_name'. Unable to enable this view.", 'notice');
  }
  if ($redirect_link) {
    drupal_goto($redirect_link);
  }
}

/**
 * Programatically disable view.
 *
 * This should be used in a hook_menu definition as the callback to provide a
 * link to disable the view (first example). It can also be called directly if
 * needed (second example).
 *
 * @code
 *  //Create a URL that when the user navigates there, a given view will be
 *  //disabled.
 *  //You will still need to provide a link to this menu item somewhere
 *  //appropriate.
 *
 * function mymodule_menu() {
 * $items = array();
 *
 *   //Create one of these for each of your default views
 * $items['admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/disable']
 *   = array(
 * 'title' => 'Disable <VIEW-HUMAN-READABLE-NAME>',
 * 'page callback' => 'tripal_disable_view',
 * 'page arguments' => array('<VIEW-MACHINE-NAME>',
 *   '<PATH-TO-REDIRECT-TO-AFTERWARDS>'),
 * 'access arguments' => array('<YOUR-PERMISSION-KEY>'),
 * 'type' => MENU_CALLBACK,
 * );
 *
 * return $items;
 * }
 *
 *    //Call this function directly to disable a view
 *    //The example shows disabling your own default view when your module is
 *    //uninstalled
 *  function mymodule_uninstall() {
 *
 * $view_name = '<VIEW-MACHINE-NAME>';
 *  tripal_disable_view($view_name);
 *
 * }
 * @endcode
 *
 * @param $view_name
 *   The machine-name of the view to be enabled
 * @param $redirect_link
 *   The path to redirect to. FALSE if no redirect needed
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_disable_view($view_name, $redirect_link = FALSE) {

  $status = variable_get('views_defaults', []);
  if (isset($status[$view_name])) {
    $status[$view_name] = TRUE;
    variable_set('views_defaults', $status);
    drupal_set_message("Disabled $view_name");
  }
  else {
    drupal_set_message("Unable to find a view by the name of '$view_name'. Unable to disable this view.", 'notice');
  }
  if ($redirect_link) {
    drupal_goto($redirect_link);
  }
}

/**
 * Remove any drupal fields from a chado-based default view definition if chado
 * is external.
 * This ensures compatibility with an external chado database.
 *
 * You should be calling this function in your hook_views_default_views().
 * This function will only remove drupal tables if chado is external; thus you
 * do not need to worry about checking yourself. For example, the following is
 * a good  hook_views_default_views():
 *
 * @code
 * function mymodule_views_default_views() {
 * $views = array();
 *
 *   // NOTE: <VIEW-TYPE> describes the type of view:
 *   //      - 'admin' for views used for administration of your module
 *   //      - 'search' for views used to search data
 *   //     - 'list' for views used primarily as data listings
 *   //
 *   //<VIEW-HUMAN-READABLE-NAME>
 * $view = mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>();
 * $view = tripal_make_view_compatible_with_external($view);
 * $views[$view->name] = $view;
 *
 *    //<VIEW-HUMAN-READABLE-NAME>
 * $view = mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>();
 * $view = tripal_make_view_compatible_with_external($view);
 * $views[$view->name] = $view;
 *
 * return $views;
 * }
 * function mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>() {
 *     //PASTE VIEWS EXPORT CODE HERE
 * return $view;
 * }
 *
 * function mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>() {
 *    //PASTE VIEWS EXPORT CODE HERE
 * return $view;
 * }
 * @endcode
 * Notice that the actual views export code is in a separate function.
 * This makes your hook_views_default_views() more readable.
 *
 * NOTE: Currently assumes all tables not in the tripal views integration
 * tables are Drupal tables.
 *
 * @param $view
 *   The default view definition object
 *
 * @return
 *   The default view with all relationships, fields, filters, sorts, arguments
 *   for Drupal tables removed.
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_make_view_compatible_with_external($view) {
  $remove_table = [];

  // First check that the base table for the view is a chado table
  // If it's not then don't do any filtering.
  $setup_id = tripal_is_table_integrated($view->base_table);
  if (!$setup_id) {
    return $view;
  }

  // IF chado is external then remove all config relating to drupal tables.
  $is_local = isset($GLOBALS["chado_is_local"]) && $GLOBALS["chado_is_local"];
  if (!$is_local) {

    // Iterate through all displays.
    foreach ($view->display as $display_name => $display) {
      $display_options = $display->handler->display->display_options;

      $sections = ['fields', 'filters', 'sorts', 'relationships'];
      foreach ($sections as $section) {
        $display_options[$section] = (isset($display_options[$section])) ? $display_options[$section] : [];
        foreach ($display_options[$section] as $key => $defn) {
          // If the table has not already been encountered; check if it's in 
          // tripal_views.
          if (!isset($remove_table[$defn['table']])) {
            // If the table is view then this is a general handler; thus keep.
            if ($defn['table'] == 'views') {
              $remove_table[$defn['table']] = FALSE;
            }
            // If this table is integrated then it is chado; thus keep.
            $setup_id = tripal_is_table_integrated($defn['table']);
            if ($setup_id) {
              $remove_table[$defn['table']] = FALSE;
            }
            else {
              $remove_table[$defn['table']] = TRUE;
            }
          }

          // Based on the $remove_table array, unset this field if its from a 
          // drupal table.
          if ($remove_table[$defn['table']]) {
            unset($view->display[$display_name]->handler->display->display_options[$section][$key]);
          }
        }
      }

    }
  }

  return $view;
}

/**
 * Retrieve the priority of the lightest priority for a given table.
 *
 * NOTE: Uses lightest priority (drupal-style) where the range is from -10 to 10
 * and -10 is of highest priority.
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a
 *   materialized view or a chado table
 *
 * @return
 *   returns the lowest priority.  If the table has not been integrated, a
 *   priority of 10 is returned.
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_get_lightest_views_integration_priority($table_name) {

  // D7 TODO: Check DBTNG changes work.
  $sql = "SELECT priority FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
  $setup = db_query($sql, [':table' => $table_name]);
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->priority;
  }
  else {
    // Default priority is 10.
    return 10;
  }
}

/**
 * Retrieve the views integration setup_id with the lightest priority for a
 * given table
 *
 * NOTE: Uses lightest priority (drupal-style) where the range is from -10 to 10
 * and -10 is of highest priority.
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a
 *   materialized view or a chado table
 *
 * @return
 *   On success, the setup_id to use for integration of this table; otherwise
 *   FALSE
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_get_lightest_views_integration_setup($table_name) {

  // D7 TODO: Check DBTNG changes work.
  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
  $setup = db_query($sql, [':table' => $table_name]);
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Retrieve the views integration setup_id with the given priority/table
 * combination.
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a
 *   materialized view or a chado table
 * @param $priority
 *   The priority of the integration to retrieve the setup_id for
 *
 * @return
 *   On success, the setup_id to use for integration of this table; otherwise
 *   FALSE
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_get_views_integration_setup_id($table_name, $priority) {

  // D7 TODO: Check DBTNG changes work
  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority ORDER BY priority ASC";
  $setup = db_query($sql, [':table' => $table_name, ':priority' => $priority]);
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Check to see if this table already has an integration record with the given
 * priority.
 *
 * @param $table_name
 *   The name of the table to check for integration
 * @param $priority (optional)
 *   The priority of record to check for
 *
 * @return
 *  If the table is already integrated, the setup_id of the existing integration
 *  record is returned (If priority is not specified this will be the lightest
 *  record);  Otherwise the table is not already integrated and FALSE is
 *  returned.
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_is_table_integrated($table_name, $priority = NULL) {

  if ($priority) {
    // D7 TODO: Check DBTNG changes work.
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority";
    $setup = db_query($sql, [
      ':table' => $table_name,
      ':priority' => $priority,
    ]);
    $setup = $setup->fetchObject();
  }
  else {
    // D7 TODO: Check DBTNG changes work.
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
    $setup = db_query($sql, [':table' => $table_name]);
    $setup = $setup->fetchObject();
  }
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Checks if you are dealing with the lightest priority setup for a given table.
 * This is a good way to determine whether your modules integration is being
 * used by views.
 *
 * @param $setup_id
 *   The ID of the setup to check (is this setup the lightest one?)
 * @param $table_name
 *   The name of the table associated with this setup
 *
 * @return TRUE is this is the lightest priority; FALSE otherwise
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_is_lightest_priority_setup($setup_id, $table_name) {

  $lightest_priority_setup_id = tripal_get_lightest_views_integration_setup($table_name);
  if ($lightest_priority_setup_id == $setup_id) {
    return TRUE;
  }
  else {
    return FALSE;
  }

}

/**
 * Rebuilds all the default integrations.
 *
 * This essentially clears the cache in case you mess up the integrations in
 * your site. This should not be used during module development since it
 * really only rebuilds the integrations described by all enabled modules
 * in the site and if $delete_first is TRUE it can delete custom integrations
 * created by site administrators which will not make your module popular.
 *
 * @param $delete_first
 *   If TRUE then all integrations are first deleted.
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_rebuild_views_integrations($delete_first = FALSE) {

  if ($delete_first) {
    tripal_chado_views_delete_all_integrations();
  }

  tripal_chado_views_integrate_all_chado_tables();

  // TODO: the function above should have a return value from which we can
  // determine if the message below is approprite.
  drupal_set_message('Successfully rebuilt default Chado Views Integrations');
}

/**
 * Add views integration records into the tripal_views* tables.
 *
 * This is the programatic way to add your own integrations to the tripal views
 * integrations list. Keep in mind that the priority set in your $defn_array
 * needs to be lighter than any existing integrations to be used by views and
 * that it should still be below 0 in order to allow site administrators to
 * override it should they need to.
 *
 * @param $defn_array
 *   An array describing the structure and fields of the table.
 *
 * @return
 *   True/False if completed successfully/not.
 *
 * Example usage (in hook_install()):
 * @code
 *  $defn_array = array(
 *    'table' => 'feature', //tablename or materialized view name
 *    'name' => 'Sequence Features', // Human readable name
 *    'type' => 'chado', //either chado or mview depending on tablename
 *    'description' => 'Create a listing of features.', //description seen when
 *   creating a view of this type
 *    'priority' => 10, //For Base tripal modules: 10; custom modules: 9 to 0;
 *    'base_table' => TRUE //either TRUE or FALSE depending on whether the
 *   current table should show up in the add view list
 *    'fields' => array(
 *      'feature_id' => array(
 *        'name' => 'feature_id', //field name in database
 *        'title' => 'Feature ID', //human-readable name -seen in Views UI
 *         'description' => 'This is the unique identifier for features',
 *   //help/description seen in Views UI
 *         'type' => 'int', // the type of field
 *        'handlers' => array(  //possible keys are field, filter, sort,
 *   argument, relationship
 *           'field' => array(
 *             'name' => 'chado_views_handler_numeric' //name of handler
 *           ),
 *           'filter' => array( ... ),
 *           ...
 *         ),
 *        // Describe any joins involving this field.
 *        // Note: you can include both foreign keys (feature.type_id =>
 *   cvterm.cvterm_id)
 *        // and referring tables (ie: feature.feature_id <=
 *   feature_relationship.subject_id)
 *         'joins' => array(
 *          'feature_relationship' => array( //table to join to.
 *            'subject_id' => array( //field in above table
 *   (feature_relationship)
 *              'table' => 'featureprop', //table to join to
 *              'field' => 'feature_id', //field in above table
 *   (feature_relationship)
 *              'handler' => 'views_join', //handler to use for joining
 *              'relationship_handler' => 'views_handler_relationship',
 *   //handler to use when a relationship is added.
 *              'relationship_only' => FALSE, //whether to join automatically
 *   (FALSE) or not (TRUE)
 *            ),
 *            ...
 *          ),
 *          ...
 *         ),
 *      )
 *    ),
 *  );
 *  tripal_add_views_integration($defn_array);
 * @endcode
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_add_views_integration($defn_array, $setup_id = FALSE) {
  $no_errors = TRUE;

  if (empty($defn_array['table'])) {
    tripal_report_error('tripal_views', TRIPAL_WARNING, 'Recieved integration with no tablename: %defn', ['%defn' => print_r($defn_array, TRUE)]);
    $no_errors = FALSE;
    return $no_errors;
  }

  // First insert into tripal_views.
  $view_record = [
    'table_name' => $defn_array['table'],
    'name' => $defn_array['name'],
    'comment' => $defn_array['description'],
    'priority' => $defn_array['priority'],
    'base_table' => $defn_array['base_table'],
  ];
  if ($setup_id) {
    $view_record['setup_id'] = $setup_id;
  }
  if ($defn_array['type'] == 'mview') {
    $mview = db_query("SELECT mview_id FROM {tripal_mviews} WHERE mv_table=:table", [':table' => $defn_array['table']]);
    $mview = $mview->fetchObject();
    $view_record['mview_id'] = $mview->mview_id;
    if (!$mview->mview_id) {
      return FALSE;
    }
  }
  if ($view_record['name']) { // && $view_record['comment']) {  # SPF: commented out 9/24/2012 .. It's not required on the form
    if (isset($defn_array['additional_content'])) {
      // D7 TODO: Check DBTNG changes work
      $setup = db_query(
        "SELECT * FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
        [
          ':table' => $view_record['table_name'],
          ':priority' => $view_record['priority'],
        ]
      );
      $setup = $setup->fetchObject();
      if (empty($setup->setup_id)) {
        $status = drupal_write_record('tripal_views', $view_record);
      }
      else {
        $view_record['setup_id'] = $setup->setup_id;
        $status = drupal_write_record('tripal_views', $view_record, 'setup_id');
      }
    }
    else {
      $status = drupal_write_record('tripal_views', $view_record);
    }
  }
  else {
    $status = FALSE;
    drupal_set_message(t('Unable to integrate "%table" table due to a missing name field.', ['%table' => $defn_array['table']]), 'error');
  }

  if ($status) {

    // Need to update the tripal_views record so base_table can be false
    // this is a fix because drupal_write_record() puts in defaults if !isset()
    // and a variable is considered not set if it's null!
    // D7 TODO: Check DBTNG changes work.
    db_query(
      "UPDATE {tripal_views} SET base_table=:base WHERE table_name=:table AND priority=:priority",
      [
        ':base' => $defn_array['base_table'],
        ':table' => $defn_array['table'],
        ':priority' => $defn_array['priority'],
      ]
    );

    // Insert Field Definitions.
    foreach ($defn_array['fields'] as $key => $field) {
      // Set some defaults.
      $field['name'] = (isset($field['name'])) ? $field['name'] : $key;
      $field['title'] = (isset($field['title'])) ? $field['title'] : $field['name'];
      $field['type'] = (isset($field['type'])) ? $field['type'] : 'text';
      $field['description'] = (isset($field['description'])) ? $field['description'] : $field['name'];
      $field['handlers'] = (isset($field['handlers'])) ? $field['handlers'] : ['field' => ['name' => 'views_handler_field']];

      // Build the field record.
      $field_record = [
        'setup_id' => $view_record['setup_id'],
        'column_name' => $field['name'],
        'name' => $field['title'],
        'description' => $field['description'],
        'type' => $field['type'],
      ];
      if ($view_record['setup_id'] && $field['name'] && $field['title'] && $field['description'] && $field['type']) {
        if (isset($defn_array['additional_content'])) {
          // D7 TODO: Check DBTNG changes work.
          $is_present = db_query(
            "SELECT true as present FROM {tripal_views_field} WHERE column_name=:column AND setup_id=:setup",
            [
              ':column' => $field_record['column_name'],
              ':setup' => $field_record['setup_id'],
            ]
          );
          $is_present = $is_present->fetchField();
          if (!$is_present) {
            $status = drupal_write_record('tripal_views_field', $field_record);
          }
          else {
            $status = drupal_write_record('tripal_views_field', $field_record, [
              'setup_id',
              'column_name',
            ]);
          }
        }
        else {
          $status = drupal_write_record('tripal_views_field', $field_record);
        }
      }
      else {
        drupal_set_message(t('Unable to integrate %name field due to missing required fields.', ['%name' => $field['name']]), 'error');
        $status = FALSE;
      }

      if ($status) {

        // Insert Handler Definitions.
        foreach ($field['handlers'] as $handler_type => $handler) {
          $handler_record = [
            'setup_id' => $view_record['setup_id'],
            'column_name' => $field['name'],
            'handler_type' => $handler_type,
            'handler_name' => $handler['name'],
            'arguments' => serialize($handler),
          ];
          if ($view_record['setup_id'] && $field['name'] && $handler_type && $handler['name'] && $handler) {
            $status = drupal_write_record('tripal_views_handlers', $handler_record);
          }
          else {
            $status = FALSE;
          }
          if (!$status) {
            drupal_set_message(t('Unable to integrate %handler_type handler: %handler_name', [
              '%handler_type' => $handler_type,
              '%handler_name' => $handler['name'],
            ]), 'error');
            $no_errors = FALSE;
          }
        }

        // Insert Joins
        // Note: The new defn_array structure accounts for 1+ joins to the same
        // table (ie: feature_relationship => feature : subject_id & object_id)
        // by making $field['joins'] an array of left_field keys.
        if (!is_array($field['joins'])) {
          $field['joins'] = [];
        }
        foreach ($field['joins'] as $joins) {
          // To keep backwards compatibility with the old defn_array which just
          // jumped right into the table definition allowing only a single join,
          // we need to check for old defn_arrays and transform them into the
          // new format.
          if (isset($joins['table'])) {
            $left_field = $joins['field'];
            $joins = [
              $left_field => $joins,
            ];
          }

          // Loop on left fields.
          foreach ($joins as $left_field => $join) {
            $join_record = [
              'setup_id' => $view_record['setup_id'],
              'base_table' => $defn_array['table'],
              'base_field' => $field['name'],
              'left_table' => $join['table'],
              'left_field' => $left_field,
            ];

            $join_record['handler'] = (!empty($join['handler'])) ? $join['handler'] : 'views_join';
            $join_record['relationship_handler'] = (!empty($join['relationship_handler'])) ? $join['relationship_handler'] : 'views_handler_relationship';
            $join_record['relationship_only'] = (!empty($join['relationship_only'])) ? $join['relationship_only'] : 0;

            if ($view_record['setup_id'] && $defn_array['table'] && $field['name'] && $join['table'] && $left_field) {
              $status = drupal_write_record('tripal_views_join', $join_record);
            }
            else {
              $status = FALSE;
            }
            if (!$status) {
              drupal_set_message(
                t(
                  'Unable to join %left_table.%left_field with %table.%field',
                  [
                    '%left_table' => $join['table'],
                    '%left_field' => $left_field,
                    '%table' => $defn_array['table'],
                    '%field' => $field['name'],
                  ]
                ),
                'error'
              );
              $no_errors = FALSE;
            }
          }
        }

      }
      else {
        drupal_set_message(t('Unable to integrate %field_name field', ['%field_name' => $field['name']]), 'error');
        $no_errors = FALSE;
      }
    }

  }
  else {
    drupal_set_message(t('Unable to set default tripal views integration'), 'error');
    $no_errors = FALSE;
  }

  return $no_errors;
}

/**
 * Export Views integration records.
 *
 * This is a great way to create your own integration since it returns an
 * already defined integration in array form that you can modify. After
 * modifications simply set the priority to something lighter (but still
 * below 0) than any existing integrations and use
 * tripal_add_views_integration() to add it to the list of integrations.
 *
 * @param $setup_id
 *   The unique setup id of the tripal views integration
 *
 * @return
 *   A views integration definition array as used by
 *   tripal_add_views_integration()
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_export_views_integration($setup_id) {

  // Main setup details
  // D7 TODO: Check DBTNG changes work
  $r = db_query("SELECT * FROM {tripal_views} WHERE setup_id=:setup", [':setup' => $setup_id]);
  $r = $r->fetchObject();
  $defn_array = [
    'table' => $r->table_name,
    'name' => $r->name,
    'type' => ($r->mview_id) ? 'mview' : 'chado',
    'description' => $r->comment,
    'priority' => $r->priority,
    'base_table' => $r->base_table,
    'fields' => [],
  ];

  // Add fields.
  $resource = db_query("SELECT * FROM {tripal_views_field} WHERE setup_id=:setup", [':setup' => $setup_id]);
  foreach ($resource as $r) {
    $defn_array['fields'][$r->column_name] = [
      'name' => $r->column_name,
      'title' => $r->name,
      'description' => $r->description,
      'type' => $r->type,
      'handlers' => [],
      'joins' => [],
    ];
  }

  // Add handlers.
  $resource = db_query("SELECT * FROM {tripal_views_handlers} WHERE setup_id=:setup", [':setup' => $setup_id]);
  foreach ($resource as $r) {
    $defn_array['fields'][$r->column_name]['handlers'][$r->handler_type] = [
      'name' => $r->handler_name,
    ];
  }

  // Add joins.
  $resource = db_query("SELECT * FROM {tripal_views_join} WHERE setup_id=:setup", [':setup' => $setup_id]);
  foreach ($resource as $r) {
    $defn_array['fields'][$r->base_field]['joins'][$r->left_table][$r->left_field] = [
      'table' => $r->left_table,
      'field' => $r->left_field,
      'handler' => $r->handler,
    ];
  }

  return $defn_array;
}

/**
 * Removes a View Integration Entry when you only know the table the integration
 * was created for and the priority.
 *
 * This should only be used to remove integrations created by your own module
 * (possibly on uninstall of your module). To override existing integrations
 * simply create your own integration with a lighter priority using
 * tripal_clone_views_integration() or tripal_export_views_integration() to
 * create a template.
 *
 * @param $identifies
 *   An array of identifiers where the keys indicate what the identifier is.
 *   One of the following compinations must be present:
 *    1) table_name & priority: the name of the table & the priority to remove
 *        a views integration entry for.
 *    2) setup_id: the setup_id of the entry to remove.
 * @param $options
 *   An array of options, currently none are supported.
 *
 * @return
 *   TRUE on Success; FALSE otherwise.
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_remove_views_integration($identifiers, $options = []) {

  // Remove the views integration using the table_name/priority combo.
  if (isset($identifiers['table_name'])) {
    $table_name = $identifiers['table_name'];
    $priority = $identifiers['priority'];

    $views = db_query(
      "SELECT * FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
      [
        ':table' => $table_name,
        ':priority' => $priority,
      ]
    );
    $views = $views->fetchObject();
    if ($views->setup_id) {
      $identifiers['setup_id'] = $views->setup_id;
    }

  }

  // Remove the views integration using the setup_id.
  if (isset($identifiers['setup_id'])) {

    db_query('DELETE FROM {tripal_views} WHERE setup_id=:setup', [':setup' => $identifiers['setup_id']]);
    db_query('DELETE FROM {tripal_views_field} WHERE setup_id=:setup', [':setup' => $identifiers['setup_id']]);
    db_query('DELETE FROM {tripal_views_handlers} WHERE setup_id=:setup', [':setup' => $identifiers['setup_id']]);
    db_query('DELETE FROM {tripal_views_join} WHERE setup_id=:setup', [':setup' => $identifiers['setup_id']]);

    return TRUE;
  }

  return FALSE;
}

/**
 * Update an existing Views Intregration Entry.
 * This essentially removes and then re-adds the integration.
 *
 * @param $setup_id
 *   The setup ID of the views integration entry to update
 * @param $defn_array
 *   An array describing the structure and fields of the table as is used in
 *   tripal_add_views_integration().
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_update_views_integration($setup_id, $defn_array) {

  tripal_remove_views_integration(['setup_id' => $setup_id]);

  $defn_array['additional_content'] = TRUE;
  tripal_add_views_integration($defn_array, $setup_id);
}

/**
 * Clone an integration. This is often a great way to create your own
 * module-specific integration while still benifiting from an existing
 * (or even the lightest priority) integration.
 *
 * @param $table_name
 *   The table for which you'd like to clone an integration
 * @param $new_priority
 *   The priority of the clone; this is the integration which will be created.
 *   If no priority is supplied then one lighter then the $template_priority
 *   will be used.
 * @param $template_priority
 *   The priority of the template to be used for the clone; this is an existing
 *   integration. If no priority is supplied then the lightest priority will be
 *   used.
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_clone_views_integration($table_name, $new_priority = NULL, $template_priority = NULL) {

  if (empty($template_priority)) {
    $template_setup_id = tripal_get_lightest_views_integration_setup($table_name);
  }
  else {
    $template_setup_id = tripal_get_views_integration_setup_id($table_name, $template_priority);
  }

  $defn_array = tripal_export_views_integration($template_setup_id);

  if (empty($new_priority)) {
    $defn_array['priority'] = $new_priority;
  }
  else {
    $new_priority = $defn_array['priority'] - 1;
    $defn_array['priority'] = $defn_array['priority'] - 1;
  }

  tripal_add_views_integration($defn_array);
  $setup_id = db_query(
    "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    [
      ':table' => $table_name,
      ':priority' => $new_priority,
    ]
  );
  $setup_id = $setup_id->fetchField();

  if (empty($setup_id)) {
    tripal_report_error('tripal_views', TRIPAL_ERROR, 'Unable to clone the setup for %table in order to add the following field to the integration: %field.',
      ['%table' => $table_name, '%field' => print_r($field_array, TRUE)]);
    return FALSE;
  }
  else {
    return $setup_id;
  }

}

/**
 * Adds the given field to an existing or cloned integration. In the case of a
 * cloned integration, the lightest integration is used as the template for the
 * clone.
 *
 * NOTE: If that field already exists in the specified integration then it will
 * first be deleted and the new one added.
 *
 * @param $table_name
 *   The name of the table the integration is for
 * @param $priority
 *   The priority of the integration to use; pass NULL to use the lightest
 *   integration
 * @param $field
 *   An array describing the field ot add; uses the same format as the
 *   $defn_array
 *
 * @return
 *   TRUE if the field was added successfully; FALSE otherwise
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_add_field_to_views_integration($table_name, $priority, $field) {
  $no_errors = TRUE;

  // If no priority is supplied then add the field to the lightest integration.
  if (empty($priority)) {
    $priority = tripal_get_lightest_views_integration_priority($table_name);
  }

  // First get the setup_id
  // D7 TODO: Check DBTNG changes work
  $setup_id = db_query(
    "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    [
      ':table' => $table_name,
      ':priority' => $priority,
    ]
  );
  $setup_id = $setup_id->fetchObject();

  // If there isn't an integration matching that table/priority combination
  // then clone the lightest priority integration.
  if (empty($setup_id)) {
    $setup_id = tripal_clone_views_integration($table_name, $priority);
  }

  // Now delete any existing field
  db_query("DELETE FROM {tripal_views_field} WHERE setup_id=:setup AND column_name=:column",
    [':setup' => $setup_id, 'column' => $field['name']]
  );
  db_query("DELETE FROM {tripal_views_handlers} WHERE setup_id=:setup AND column_name=:column",
    [':setup' => $setup_id, 'column' => $field['name']]
  );
  db_query("DELETE FROM {tripal_views_join} WHERE setup_id=:setup AND base_table=:table AND base_field=:field",
    [':setup' => $setup_id, ':table' => $table_name, ':field' => $field['name']]
  );

  // Now we need to add/update the field.
  $field_record = [
    'setup_id' => $setup_id,
    'column_name' => $field['name'],
    'name' => $field['title'],
    'description' => $field['description'],
    'type' => $field['type'],
  ];
  if ($setup_id && $field['name'] && $field['title'] && $field['description'] && $field['type']) {
    if ($defn_array['additional_content']) {
      // D7 TODO: Check DBTNG changes work.
      $is = db_query(
        "SELECT true as present FROM {tripal_views_field} WHERE column_name=:column AND setup_id=:setup",
        [
          ':column' => $field_record['column_name'],
          ':setup' => $field_record['setup_id'],
        ]
      );
      $is = $is->fetchObject();
      if (!$is->present) {
        $status = drupal_write_record('tripal_views_field', $field_record);
      }
      else {
        $status = drupal_write_record('tripal_views_field', $field_record, [
          'setup_id',
          'column_name',
        ]);
      }
    }
    else {
      $status = drupal_write_record('tripal_views_field', $field_record);
    }
  }
  else {
    drupal_set_message(t('Unable to integrate %name field due to missing required fields.', ['%name' => $field['name']]), 'error');
    $status = FALSE;
  }

  if ($status) {

    // Insert Handler Definitions.
    foreach ($field['handlers'] as $handler_type => $handler) {
      $handler_record = [
        'setup_id' => $setup_id,
        'column_name' => $field['name'],
        'handler_type' => $handler_type,
        'handler_name' => $handler['name'],
        'arguments' => serialize($handler),
      ];
      if ($setup_id && $field['name'] && $handler_type && $handler['name'] && $handler) {
        $status = drupal_write_record('tripal_views_handlers', $handler_record);
      }
      else {
        $status = FALSE;
      }
      if (!$status) {
        drupal_set_message(t('Unable to integrate %handler_type handler: %handler_name', [
          '%handler_type' => $handler_type,
          '%handler_name' => $handler['name'],
        ]), 'error');
        $no_errors = FALSE;
      }
    }

    // Insert Joins
    // Note: The new defn_array structure accounts for 1+ joins to the same
    // table (ie: feature_relationship => feature : subject_id & object_id)
    // by making $field['joins'] an array of left_field keys.
    if (!is_array($field['joins'])) {
      $field['joins'] = [];
    }
    foreach ($field['joins'] as $joins) {
      // To keep backwards compatibility with the old defn_array which just
      // jumped right into the table definition allowing only a single join,
      // we need to check for old defn_arrays and transform them into the
      // new format.
      if (isset($joins['table'])) {
        $left_field = $joins['field'];
        $joins = [
          $left_field => $joins,
        ];
      }

      // Loop on left fields.
      foreach ($joins as $left_field => $join) {
        $join_record = [
          'setup_id' => $setup_id,
          'base_table' => $defn_array['table'],
          'base_field' => $field['name'],
          'left_table' => $join['table'],
          'left_field' => $join['field'],
        ];

        if (!empty($join['handler'])) {
          $join_record['handler'] = $join['handler'];
        }
        else {
          $join_record['handler'] = 'views_join';
        }

        if ($setup_id && $defn_array['table'] && $field['name'] && $join['table'] && $join['field']) {
          $status = drupal_write_record('tripal_views_join', $join_record);
        }
        else {
          $status = FALSE;
        }
        if (!$status) {
          drupal_set_message(
            t(
              'Unable to join %left_table.%left_field with %table.%field',
              [
                '%left_table' => $join['table'],
                '%left_field' => $join['field'],
                '%table' => $defn_array['table'],
                '%field' => $field['name'],
              ]
            ),
            'error'
          );
          $no_errors = FALSE;
        }
      }
    }

  }
  else {
    drupal_set_message(t('Unable to integrate %field_name field', ['%field_name' => $field['name']]), 'error');
    $no_errors = FALSE;
  }

  return $no_errors;
}

/**
 * Adds the given field to an existing or cloned integration. In the case of a
 * cloned integration, the lightest integration is used as the template for the
 * clone.
 *
 * NOTE: If that field already exists in the specified integration then it will
 * first be deleted and the new one added.
 *
 * @param $table_name
 *   The name of the table the integration is for
 * @param $priority
 *   The priority of the integration to use; pass NULL to use the lightest
 *   integration
 * @param $field
 *   An array describing the join to add. it should contain the following keys:
 *   base_table, base_field, left_table, left_field, handler,
 *   relationship_handler, relationship_only
 *
 * @return
 *   TRUE if the field was added successfully; FALSE otherwise
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_add_join_to_views_integration($table_name, $priority, $join) {
  $no_errors = TRUE;

  // If no priority is supplied then add the field to the lightest integration.
  if (empty($priority)) {
    $priority = tripal_get_lightest_views_integration_priority($table_name);
  }

  // First get the setup_id.
  $setup_id = db_query(
    "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    [
      ':table' => $table_name,
      ':priority' => $priority,
    ]
  );
  $setup_id = $setup_id->fetchField();

  // If there isn't an integration matching that table/priority combination
  // then clone the lightest priority integration.
  if (empty($setup_id)) {
    $setup_id = tripal_clone_views_integration($table_name, $priority);
  }

  // Add the setup_id to the join record passed in
  $join['setup_id'] = $setup_id;

  drupal_write_record('tripal_views_join', $join);
}

/**
 * Remove a join from an integration. This is usually done after cloning an
 * existing integration using tripal_clone_views_integration().
 *
 * @param $setup_id
 *   The setup_id of the integration to delete the join from.
 * @param $base_table
 *   The name of the base table involved the join.
 * @param $base_field
 *   The field from the base table involved in the join.
 * @param $left_table
 *   The name of the other table involved in the join
 * @param $left_field
 *   The name of the field from the other table involved in the join.
 *
 * @ingroup tripal_chado_views_api
 */
function tripal_remove_join_from_views_integration($setup_id, $base_table, $base_field, $left_table, $left_field) {
  db_query(
    "DELETE FROM {tripal_views_join} WHERE setup_id=:setup AND base_table=:base-table AND base_field=:base-field AND left_table=:left-table AND left_field=:left-field",
    [
      ':setup' => $setup_id,
      ':base-table' => $base_table,
      ':base-field' => $base_field,
      ':left-table' => $left_table,
      ':left-field' => $left_field,
    ]
  );
}
