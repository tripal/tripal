<?php
/**
 * @file
 * API functions for Tripal Views Integration
 */

 /**
 * @defgroup tripal_views_api Tripal Views Module API
 * @ingroup tripal_api
 * @{
 * Provides functions to help extension modules add their own tripal views integrations
 * as well as functions for managing default views.
 *
 * Managing Default Views:
 *
 * When you create administrative default views (or really any default view) you really
 * have no way to ensure that the site administrator keeps these views enabled. In some
 * cases this is a good thing and provides the site administrator the ability to disable
 * views that don't apply to their particular installation or to replace your default view
 * with a custom view of their own. But in other cases, particularily for administration
 * views, you want to gaurd against accidental disabling of your views.
 *
 * One way to do this is to add a landing page using a heook_menu() definition to a custom
 * callback returning HTML. You can use this page to render the view if it is enabled but
 * provide logic to display a message if the view is disabled. Furthermore, it's often
 * helpful to provide a link allowing site administrators to enable the view from your
 * page rather than expecting them to go to the views administration UI. This is done for
 * all the administration views provided by Tripal. The following is an example of how to
 * achomplish this functionality for your own module:
 * @code
function mymodule_menu() {
  $items = array();

  // Create the landing page
  $items['admin/tripal/<PATH-TO-YOUR-LANDING-PAGE>'] = array(
    'title' => 'MyModule Administration',
    'description' => 'Administration of my module.',
    'page callback' => 'mymodule_admin_landing_page',
    'access arguments' => array('<YOUR-PERMISSION-KEY>'),
    'type' => MENU_NORMAL_ITEM,
  );

  // Create one of these for each of your default views
  $items['admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/enable'] = array(
    'title' => 'Enable <VIEW-HUMAN-READABLE-NAME>',
    'page callback' => 'tripal_enable_view',
    'page arguments' => array('<VIEW-MACHINE-NAME>', '<PATH-TO-REDIRECT-TO-AFTERWARDS>'),
    'access arguments' => array('<YOUR-PERMISSION-KEY>'),
    'type' => MENU_CALLBACK,
  );

  return $items;
}

function mymodule_admin_landing_page() {

  // Get the View Embed Code
  // This function will return FALSE if your view is not enabled
  $view_code = views_embed_view('<VIEW-MACHINE-NAME>', '<DISPLAY-MACHINE-NAME');
  // If your view is enabled then embed it in this page by returning the embed code
  if (isset($view_code)) {
    $output .= $view_code;
  }
  else {
    // Provide the landing page with links to the menu item created in hook_menu to
    // to enable your view
    $output .= '<p>The My Module module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    // NOTE: <URL-FROM-MENU-TO-ENABLE-VIEW> is
    // admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/enable
    // from above hook_menu().
    $output .= '<li>' . l('<VIEW-HUMAN-RADABLE-NAME>', '<URL-FROM-MENU-TO-ENABLE-VIEW>') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

 * @endcode
 *
 * Adding your own Tripal Views Integrations:
 *
 * One of the main ways the Tripal View API is likely to be used is if your module needs
 * to create it's own tripal views integration. You might need to do this for any number of
 * reasons but the following examples are thought to be the most common:
 *
 *   1) Your module wants to add handlers with better functionality to fields it has more
 *       knowledge of than the general integration for all tables
 * @code
   mymodule_views_data() {

    // First check that your integration has not already been added
    // When selecting a priority, do not choose -10 or -9 and make sure it is below 0
    // so that individual sites still have the ability to override it, if needed
    $table_name = 'my_chado_table';
    $priority = -5;
    if (!tripal_is_table_integrated($table_name, $priority)) {

      // If you really only need to tweak an existing integration you can clone it
      // If you want to clone the integration that is currently taking priority
      // then use tripal_get_lightest_views_integration_priority() as below
      $lightest_priority = tripal_get_lightest_views_integration_priority($table_name);
      $setup_id = tripal_clone_views_integration($table_name, $priority, $lightest_priority);

      // And then make a few changes
      // First get the definition array created via the clone above
      $defn_array = tripal_export_views_integration($setup_id);

      // Then make some changes to the array here

      // And finally save the changes to the integration
      tripal_update_views_integration($setup_id, $defn_array);

    }

  }
 * @endcode
 *   2) Your module creates a chado table that is not already integrated.
 * @code
  mymodule_views_data() {

    // First check that your integration has not already been added
    // When selecting a priority, do not choose 10 or 9 and make sure it is below 0
    // so that individual sites still have the ability to override it, if needed
    $table_name = 'my_chado_table';
    $priority = 5;
    if (!tripal_is_table_integrated($table_name, $priority)) {

      // Describe your table using a large array as specified by tripal_add_views_integration().
      $defn_array = array(
        'table' => $table_name, //tablename or materialized view name
        'name' => 'My Chado Table', // Human readable name
        'type' => 'chado', //either chado or mview depending on tablename
        'description' => 'Create a listing from my chado table.', //description seen when creating a view of this type
        'priority' => $priority, //For Base tripal modules: 10; custom modules: 9 to 0;
        'base_table' => TRUE //either TRUE or FALSE depending on whether the current table should show up in the add view list
        'fields' => array(
          'feature_id' => array(
            'name' => 'feature_id', //field name in database
            'title' => 'Feature ID', //human-readable name -seen in Views UI
            'description' => 'This is the unique identifier for features', //help/description seen in Views UI
            'type' => 'int', // the type of field
            'handlers' => array(  //possible keys are field, filter, sort, argument, relationship
              'field' => array(
                'name' => 'chado_views_handler_numeric' //name of handler
              ),
              'filter' => array( ... ),
              ...
            ),
            'join' => array( //describe a table that joins to this one via this field
              'table' => 'featureprop', //table to join to
              'field' => 'feature_id', //field in above table (featureprop)
              'handler' => 'views_handler_join_chado_aggregator', //handler to use
            ),
          )
        ),
      );
      // Actually create the entry
      tripal_add_views_integration($defn_array);

    }

  }
 * @endcode
 * @}
 */

/**
 * Programatically enable view
 *
 * This should be used in a hook_menu definition as the callback to provide a link
 * to enable the view (first example). It can also be called directly if needed (second example).
 * @code
// Create a URL that when the user navigates there, a given view will be enabled.
// You will still need to provide a link to this menu item somewhere appropriate (ie: an admin landing page).
function mymodule_menu() {
  $items = array();

  // Create one of these for each of your default views
  $items['admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/enable'] = array(
    'title' => 'Enable <VIEW-HUMAN-READABLE-NAME>',
    'page callback' => 'tripal_enable_view',
    'page arguments' => array('<VIEW-MACHINE-NAME>', '<PATH-TO-REDIRECT-TO-AFTERWARDS>'),
    'access arguments' => array('<YOUR-PERMISSION-KEY>'),
    'type' => MENU_CALLBACK,
  );

  return $items;
}

// Call this function directly to disable a view
// The example shows enabling your own default view when your module is enabled.
// This might be useful if you disable your view when your module is disabled.
function mymodule_enable() {

  $view_name = '<VIEW-MACHINE-NAME>';
  tripal_enable_view($view_name);

}
 * @endcode
 *
 * @param $view_name
 *   The machine-name of the view to be enabled
 * @param $redirect_link
 *   The path to redirect to. FALSE if no redirect needed
 *
 * @ingroup tripal_views_api
 */
function tripal_enable_view($view_name, $redirect_link = FALSE) {

  $status = variable_get('views_defaults', array());
  if (isset($status[$view_name])) {
    $status[$view_name] = FALSE;
    variable_set('views_defaults', $status);
    drupal_set_message("Successfully Enabled $view_name");
  }
  else {
    drupal_set_message("Unable to find a view by the name of '$view_name'. Unable to enable this view.",'error');
  }
  if ($redirect_link) {
    drupal_goto($redirect_link);
  }
}

/**
 * Programatically disable view.
 *
 * This should be used in a hook_menu definition as the callback to provide a link
 * to disable the view (first example). It can also be called directly if needed (second example).
 * @code
// Create a URL that when the user navigates there, a given view will be disabled.
// You will still need to provide a link to this menu item somewhere appropriate.
function mymodule_menu() {
  $items = array();

  // Create one of these for each of your default views
  $items['admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/disable'] = array(
    'title' => 'Disable <VIEW-HUMAN-READABLE-NAME>',
    'page callback' => 'tripal_disable_view',
    'page arguments' => array('<VIEW-MACHINE-NAME>', '<PATH-TO-REDIRECT-TO-AFTERWARDS>'),
    'access arguments' => array('<YOUR-PERMISSION-KEY>'),
    'type' => MENU_CALLBACK,
  );

  return $items;
}

// Call this function directly to disable a view
// The example shows disabling your own default view when your module is uninstalled
function mymodule_uninstall() {

  $view_name = '<VIEW-MACHINE-NAME>';
  tripal_disable_view($view_name);

}
 * @endcode
 *
 * @param $view_name
 *   The machine-name of the view to be enabled
 * @param $redirect_link
 *   The path to redirect to. FALSE if no redirect needed
 *
 * @ingroup tripal_views_api
 */
function tripal_disable_view($view_name, $redirect_link = FALSE) {

  $status = variable_get('views_defaults', array());
  if (isset($status[$view_name])) {
    $status[$view_name] = TRUE;
    variable_set('views_defaults', $status);
    drupal_set_message("Disabled $view_name");
  }
  else {
    drupal_set_message("Unable to find a view by the name of '$view_name'. Unable to disable this view.",'error');
  }
  if ($redirect_link) {
    drupal_goto($redirect_link);
  }
}

/**
 * Remove any drupal fields from a chado-based default view definition if chado is external.
 * This ensures compatibility with an external chado database.
 *
 * You should be calling this function in your hook_views_default_views(). This function
 * will only remove drupal tables if chado is external; thus you do not need to worry about
 * checking yourself. For example, the following is a good hook_views_default_views():
 * @code
 function mymodule_views_default_views() {
  $views = array();

  // NOTE: <VIEW-TYPE> describes the type of view:
  //     - 'admin' for views used for administration of your module
  //     - 'search' for views used to search data
  //     - 'list' for views used primarily as data listings

  // <VIEW-HUMAN-READABLE-NAME>
  $view = mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  // <VIEW-HUMAN-READABLE-NAME>
  $view = mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>();
  $view = tripal_make_view_compatible_with_external($view);
  $views[$view->name] = $view;

  return $views;
}

function mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>() {
  // PASTE VIEWS EXPORT CODE HERE
  return $view;
}

function mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>() {
  // PASTE VIEWS EXPORT CODE HERE
  return $view;
}
 * @endcode
 * Notice that the actual views export code is in a separate function. This makes your
 * hook_views_default_views() more readable.
 *
 * NOTE: Currently assumes all tables not in the tripal views integration tables are Drupal tables.
 *
 * @param $view
 *   The default view definition object
 * @return
 *   The default view with all relationships, fields, filters, sorts, arguements for
 *   Drupal tables removed.
 */
function tripal_make_view_compatible_with_external($view) {
  $remove_table = array();

  // First check that the base table for the view is a chado table
  // If it's not then don't do any filtering
  $setup_id = tripal_is_table_integrated($view->base_table);
  if (!$setup_id) {
    return $view;
  }

  // IF chado is external then remove all config relating to drupal tables
  if (!chado_is_local()) {

    // Iterate through all displays
    foreach ($view->display as $display_name => $display) {
      $display_options = $display->handler->display->display_options;

      $sections = array('fields', 'filters', 'sorts', 'relationships');
      foreach ($sections as $section) {
        $display_options[$section] = (isset($display_options[$section])) ? $display_options[$section] : array();
        foreach ($display_options[$section] as $key => $defn) {
          // If the table has not already been encountered; check if it's in tripal_views
          if (!isset($remove_table[ $defn['table'] ])) {
            // If the table is view then this is a general handler; thus keep
            if ($defn['table'] == 'views') {
              $remove_table[ $defn['table'] ] = FALSE;
            }
            // If this table is integrated then it is chado; thus keep
            $setup_id = tripal_is_table_integrated($defn['table']);
            if ($setup_id) {
              $remove_table[ $defn['table'] ] = FALSE;
            }
            else {
              $remove_table[ $defn['table'] ] = TRUE;
            }
          }

          // Based on the $remove_table array, unset this field if its from a drupal table
          if ($remove_table[ $defn['table'] ]) {
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
 *   The name of the table to retrieve the setup ID for. This can be either a materialized
 *   view or a chado table
 *
 * @return
 *   returns the lowest priority.  If the table has not been integrated, a priority of 10
 *   is returned.
 *
 * @ingroup tripal_views_api
 */
function tripal_get_lightest_views_integration_priority($table_name) {

  // D7 TODO: Check DBTNG changes work
  $sql = "SELECT priority FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
  $setup = db_query($sql, array(':table' => $table_name));
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->priority;
  }
  else {
    // default priority is 10
    return 10;
  }
}

/**
 * Retrieve the views integration setup_id with the lightest priority for a given table
 *
 * NOTE: Uses lightest priority (drupal-style) where the range is from -10 to 10
 * and -10 is of highest priority.
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a materialized
 *   view or a chado table
 *
 * @return
 *   On success, the setup_id to use for integration of this table; otherwise FALSE
 *
 * @ingroup tripal_views_api
 */
function tripal_get_lightest_views_integration_setup($table_name) {

  // D7 TODO: Check DBTNG changes work
  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
  $setup = db_query($sql, array(':table' => $table_name));
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Retrieve the views integration setup_id with the given priority/table combination.
 *
 * @param $table_name
 *   The name of the table to retrieve the setup ID for. This can be either a materialized
 *   view or a chado table
 * @param $priority
 *   The priority of the integration to retrieve the setup_id for
 *
 * @return
 *   On success, the setup_id to use for integration of this table; otherwise FALSE
 *
 * @ingroup tripal_views_api
 */
function tripal_get_views_integration_setup_id($table_name, $priority) {

  // D7 TODO: Check DBTNG changes work
  $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority ORDER BY priority ASC";
  $setup = db_query($sql, array(':table' => $table_name, ':priority' => $priority));
  $setup = $setup->fetchObject();
  if ($setup) {
    return $setup->setup_id;
  }
  else {
    return FALSE;
  }
}

/**
 * Check to see if this table already has an integration record with the given priority.
 *
 * @param $table_name
 *   The name of the table to check for integration
 * @param $priority (optional)
 *   The priority of record to check for
 *
 * @return
 *  If the table is already integrated, the setup_id of the existing integration
 *  record is returned (If priority is not specified this will be the lightest record);
 *  Otherwise the table is not already integrated and FALSE is returned.
 *
 * @ingroup tripal_views_api
 */
function tripal_is_table_integrated($table_name, $priority = NULL) {

  if ($priority) {
    // D7 TODO: Check DBTNG changes work
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority";
    $setup = db_query($sql, array(':table' => $table_name, ':priority' => $priority));
    $setup = $setup->fetchObject();
  }
  else {
    // D7 TODO: Check DBTNG changes work
    $sql = "SELECT setup_id FROM {tripal_views} WHERE table_name=:table ORDER BY priority ASC";
    $setup = db_query($sql, array(':table' => $table_name));
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
 * Checks if you are dealing with the lightest priority setup for a given table. This is a
 * good way to determine whether your modules integration is being used by views.
 *
 * @param $setup_id
 *   The ID of the setup to check (is this setup the lightest one?)
 * @param $table_name
 *   The name of the table associated with this setup
 *
 * @return TRUE is this is the lightest priority; FALSE otherwise
 *
 * @ingroup tripal_views_api
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
 * This essentially clears the cache in case you mess up the integrations in your site.
 * This should not be used during module development since it really only rebuilds the
 * integrations described by all enabled modules in the site and if $delete_first is
 * TRUE it can delete custom integrations created by site administrators which will not
 * make your module popular.
 *
 * @param $delete_first
 *   If TRUE then all integrations are first deleted.
 *
 * @ingroup tripal_views_api
 */
function tripal_rebuild_views_integrations($delete_first = FALSE) {

  if ($delete_first) {
    tripal_views_delete_all_integrations();
  }

  tripal_views_integrate_all_chado_tables();
  drupal_set_message('Successfully rebuilt default Tripal Views Integrations');
}

/**
 * Add views integration records into the tripal_views* tables.
 *
 * This is the programatic way to add your own integrations to the tripal views integrations
 * list. Keep in mind that the priority set in your $defn_array needs to be lighter than
 * any existing integrations to be used by views and that it should still be below 0
 * in order to allow site administrators to override it should they need to.
 *
 * @param $defn_array
 *   An array describing the structure and fields of the table
 *
 * @return
 *   True/False if completed successfully/not
 *
 * Example usage (in hook_install()):
 * @code
  $defn_array = array(
    'table' => 'feature', //tablename or materialized view name
    'name' => 'Sequence Features', // Human readable name
    'type' => 'chado', //either chado or mview depending on tablename
    'description' => 'Create a listing of features.', //description seen when creating a view of this type
    'priority' => 10, //For Base tripal modules: 10; custom modules: 9 to 0;
    'base_table' => TRUE //either TRUE or FALSE depending on whether the current table should show up in the add view list
    'fields' => array(
      'feature_id' => array(
        'name' => 'feature_id', //field name in database
        'title' => 'Feature ID', //human-readable name -seen in Views UI
        'description' => 'This is the unique identifier for features', //help/description seen in Views UI
        'type' => 'int', // the type of field
        'handlers' => array(  //possible keys are field, filter, sort, argument, relationship
          'field' => array(
            'name' => 'chado_views_handler_numeric' //name of handler
          ),
          'filter' => array( ... ),
          ...
        ),
        'join' => array( //describe a table that joins to this one via this field
          'table' => 'featureprop', //table to join to
          'field' => 'feature_id', //field in above table (featureprop)
          'handler' => 'views_handler_join_chado_aggregator', //handler to use
        ),
      )
    ),
  );
  tripal_add_views_integration($defn_array);
 * @endcode
 *
 * @ingroup tripal_views_api
 */
function tripal_add_views_integration($defn_array, $setup_id = FALSE) {
  $no_errors = TRUE;

  if (empty($defn_array['table'])) {
    tripal_report_error('tripal_views', TRIPAL_WARNING, 'Recieved integration with no tablename: %defn', array('%defn' => print_r($defn_array,TRUE)));
    $no_errors = FALSE;
    return $no_errors;
  }

  // First insert into tripal_views
  $view_record = array(
    'table_name' => $defn_array['table'],
    'name' => $defn_array['name'],
    'comment' => $defn_array['description'],
    'priority' => $defn_array['priority'],
    'base_table' => $defn_array['base_table'],
  );
  if ($setup_id) {
    $view_record['setup_id'] = $setup_id;
  }
  if ($defn_array['type'] == 'mview') {
    // D7 TODO: Check DBTNG changes work
    $mview = db_query("SELECT mview_id FROM {tripal_mviews} WHERE mv_table=:table", array(':table' => $defn_array['table']));
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
        array(':table' => $view_record['table_name'], ':priority' => $view_record['priority'])
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
    drupal_set_message(t('Unable to integrate "%table" table due to a missing name field.', array('%table' => $defn_array['table'])), 'error');
  }

  if ($status) {

    // Need to update the tripal_views record so base_table can be false
    // this is a fix because drupal_write_record() puts in defaults if !isset()
    // and a variable is considered not set if it's null!
    // D7 TODO: Check DBTNG changes work
    db_query(
      "UPDATE {tripal_views} SET base_table=:base WHERE table_name=:table AND priority=:priority",
      array(
        ':base' => $defn_array['base_table'],
        ':table' => $defn_array['table'],
        ':priority' => $defn_array['priority']
      )
    );

    // Insert Field Definitions
    foreach ($defn_array['fields'] as $field) {
      $field_record = array(
        'setup_id' => $view_record['setup_id'],
        'column_name' => $field['name'],
        'name' => $field['title'],
        'description' => $field['description'],
        'type' => $field['type'],
      );
      if ($view_record['setup_id'] && $field['name'] && $field['title'] && $field['description'] && $field['type']) {
        if (isset($defn_array['additional_content'])) {
          // D7 TODO: Check DBTNG changes work
          $is = db_query(
            "SELECT true as present FROM {tripal_views_field} WHERE column_name=:column AND setup_id=:setup",
            array(
              ':column' => $field_record['column_name'],
              ':setup' => $field_record['setup_id']
              )
            );
          $is = $is->fetchObject();
          if (!$is->present) {
            $status = drupal_write_record('tripal_views_field', $field_record);
          }
          else {
            $status = drupal_write_record('tripal_views_field', $field_record, array('setup_id', 'column_name'));
          }
        }
        else {
          $status = drupal_write_record('tripal_views_field', $field_record);
        }
      }
      else {
        drupal_set_message(t('Unable to integrate %name field due to missing required fields.', array('%name' => $field['name'])), 'error');
        $status = FALSE;
      }

      if ($status) {

        // Insert Handler Definitions
        foreach ($field['handlers'] as $handler_type => $handler) {
          $handler_record = array(
            'setup_id' => $view_record['setup_id'],
            'column_name' => $field['name'],
            'handler_type' => $handler_type,
            'handler_name' => $handler['name'],
            'arguments' => serialize($handler)
          );
          if ($view_record['setup_id'] && $field['name'] && $handler_type && $handler['name'] && $handler) {
            $status = drupal_write_record('tripal_views_handlers', $handler_record);
          }
          else {
            $status = FALSE;
          }
          if (!$status) {
            drupal_set_message(t('Unable to integrate %handler_type handler: %handler_name', array('%handler_type' => $handler_type, '%handler_name' => $handler['name'])), 'error');
            $no_errors = FALSE;
          }
        }

        // Insert Joins
        if (!is_array($field['joins'])) {
          $field['joins'] = array();
        }
        foreach ($field['joins'] as $join) {
          $join_record = array(
            'setup_id' => $view_record['setup_id'],
            'base_table' => $defn_array['table'],
            'base_field' => $field['name'],
            'left_table' => $join['table'],
            'left_field' => $join['field'],
          );

          $join_record['handler'] = (!empty($join['handler'])) ? $join['handler'] : 'views_join';
          $join_record['relationship_handler'] = (!empty($join['relationship_handler'])) ? $join['relationship_handler'] : 'views_handler_relationship';
          $join_record['relationship_only'] = (!empty($join['relationship_only'])) ? $join['relationship_only'] : 0;

          if ($view_record['setup_id'] && $defn_array['table'] && $field['name'] && $join['table'] && $join['field']) {
            $status = drupal_write_record('tripal_views_join', $join_record);
          }
          else {
            $status = FALSE;
          }
          if (!$status) {
            drupal_set_message(
              t(
                'Unable to join %left_table.%left_field with %table.%field',
                array(
                  '%left_table' => $join['table'],
                  '%left_field' => $join['field'],
                  '%table' => $defn_array['table'],
                  '%field' => $field['name']
                )
              ),
              'error'
            );
            $no_errors = FALSE;
          }
        }

      }
      else {
        drupal_set_message(t('Unable to integrate %field_name field', array('%field_name' => $field['name'])), 'error');
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
 * This is a great way to create your own integration since it returns an already defined
 * integration in array form that you can modify. After modifications simply set the
 * priority to something lighter (but still below 0) than any existing integrations
 * and use tripal_add_views_integration() to add it to the list of integrations.
 *
 * @param $setup_id
 *   The unique setup id of the tripal views integration
 *
 * @return
 *   A views integration definition array as used by tripal_add_views_integration()
 *
 * @ingroup tripal_views_api
 */
function tripal_export_views_integration($setup_id) {

  // Main setup details
  // D7 TODO: Check DBTNG changes work
  $r = db_query("SELECT * FROM {tripal_views} WHERE setup_id=:setup", array(':setup' => $setup_id));
  $r = $r->fetchObject();
  $defn_array = array(
    'table' => $r->table_name,
    'name' => $r->name,
    'type' => ($r->mview_id) ? 'mview' : 'chado',
    'description' => $r->comment,
    'priority' => $r->priority,
    'base_table' => $r->base_table,
    'fields' => array(),
  );

  // Add fields
  // D7 TODO: Check DBTNG changes work
  $resource = db_query("SELECT * FROM {tripal_views_field} WHERE setup_id=:setup", array(':setup' => $setup_id));
  foreach ($resource as $r) {
    $defn_array['fields'][ $r->column_name ] = array(
        'name' => $r->column_name,
        'title' => $r->name,
        'description' => $r->description,
        'type' => $r->type,
        'handlers' => array(),
        'joins' => array()
    );
  }

  // Add handlers
  // D7 TODO: Check DBTNG changes work
  $resource = db_query("SELECT * FROM {tripal_views_handlers} WHERE setup_id=:setup", array(':setup' => $setup_id));
  foreach ($resource as $r) {
    $defn_array['fields'][ $r->column_name ]['handlers'][ $r->handler_type ] = array(
      'name' => $r->handler_name
    );
  }

  // Add joins
  // D7 TODO: Check DBTNG changes work
  $resource = db_query("SELECT * FROM {tripal_views_join} WHERE setup_id=:setup", array(':setup' => $setup_id));
  foreach ($resource as $r) {
    $defn_array['fields'][ $r->base_field ]['joins'][ $r->left_table ] = array(
      'table' => $r->left_table,
      'field' => $r->left_field,
      'handler' => $r->handler,
    );
  }

  return $defn_array;
}

/**
 * Removes a View Integration Entry when you only know the table the integration was
 * created for and the priority.
 *
 * This should only be used to remove integrations created by your own module (possibly
 * on uninstall of your module). To override existing integrations simply create your own
 * integration with a lighter priority using tripal_clone_views_integration() or
 * tripal_export_views_integration() to create a template.
 *
 * @param $identifies
 *   An array of identifiers where the keys indicate what the identifier is. One of the
 *   following compinations must be present:
 *    1) table_name & priority: the name of the table & the priority to remove a views
 *       integration entry for
 *    2) setup_id: the setup_id of the entry to remove
 * @param $options
 *   An array of options, currently none are supported
 *
 * @return
 *   TRUE on Success; FALSE otherwise
 *
 * @ingroup tripal_views_api
 */
function tripal_remove_views_integration($identifiers, $options = array()) {

  // Remove the views integration using the table_name/priority combo
  if (isset($identifiers['table_name'])) {
    $table_name = $identifiers['table_name'];
    $priority = $identifiers['priority'];

    $views = db_query(
      "SELECT * FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
      array(
        ':table' => $table_name,
        ':priority' => $priority
      )
    );
    $views = $views->fetchObject();
    if ($views->setup_id) {
      $identifiers['setup_id'] = $views->setup_id;
    }

  }

  // Remove the views integration using the setup_id
  if (isset($identifiers['setup_id'])) {

    db_query('DELETE FROM {tripal_views} WHERE setup_id=:setup', array(':setup' => $setup_id));
    db_query('DELETE FROM {tripal_views_field} WHERE setup_id=:setup', array(':setup' => $setup_id));
    db_query('DELETE FROM {tripal_views_handlers} WHERE setup_id=:setup', array(':setup' => $setup_id));
    db_query('DELETE FROM {tripal_views_join} WHERE setup_id=:setup', array(':setup' => $setup_id));

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
 * @ingroup tripal_views_api
 */
function tripal_update_views_integration($setup_id, $defn_array) {

  tripal_remove_views_integration(array('setup_id' => $setup_id));

  $defn_array['additional_content'] = TRUE;
  tripal_add_views_integration($def_array, $setup_id);
}

/**
 * Clone an integration. This is often a great way to create your own module-specific
 * integration while still benifiting from an existing (or even the lightest priority)
 * integration.
 *
 * @param $table_name
 *   The table for which you'd like to clone an integration
 * @param $new_priority
 *   The priority of the clone; this is the integration which will be created.
 *   If no priority is supplied then one lighter then the $template_priority will be used.
 * @param $template_priority
 *   The priority of the template to be used for the clone; this is an existing integration.
 *   If no priority is supplied then the lightest priority will be used.
 *
 * @ingroup tripal_views_api
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

  // D7 TODO: Check DBTNG changes work
  tripal_add_views_integration($defn_array);
  $setup_id = db_query(
    "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    array(
      ':table' => $table_name,
      ':priority' => $new_priority
    )
  );
  $setup_id = $setup_id->fetchObject();

  if (empty($setup_id)) {
    tripal_report_error('tripal_views', TRIPAL_ERROR, 'Unable to clone the setup for %table in order to add the following field to the integration: %field.',
      array('%table' => $table_name, '%field' => print_r($field_array,TRUE)));
    return FALSE;
  }
  else {
    return $setup_id;
  }

}

/**
 * Adds the given field to an existing or cloned integration. In the case of a cloned
 * integration, the lightest integration is used as the template for the clone.
 *
 * NOTE: If that field already exists in the specified integration then it will first be
 * deleted and the new one added.
 *
 * @param $table_name
 *   The name of the table the integration is for
 * @param $priority
 *   The priority of the integration to use; pass NULL to use the lightest integration
 * @param $field
 *   An array describing the field ot add; uses the same format as the $defn_array
 *
 * @return
 *   TRUE if the field was added successfully; FALSE otherwise
 *
 * @ingroup tripal_views_api
 */
function tripal_add_field_to_views_integration($table_name, $priority, $field) {
  $no_errors = TRUE;

  // If no priority is supplied then add the field to the lightest integration
  if (empty($priority)) {
    $priority = tripal_get_lightest_views_integration_priority($table_name);
  }

  // First get the setup_id
  // D7 TODO: Check DBTNG changes work
  $setup_id = db_query(
    "SELECT setup_id FROM {tripal_views} WHERE table_name=:table AND priority=:priority",
    array(
      ':table' => $table_name,
      ':priority' => $priority
    )
  );
  $setup_id = $setup_id->fetchObject();

  // If there isn't an integration matching that table/priority combination
  // then clone the lightest priority integration
  if (empty($setup_id)) {
    $setup_id = tripal_clone_views_integration($table_name, $priority);
  }

  // Now delete any existing field
  db_query("DELETE FROM {tripal_views_field} WHERE setup_id=:setup AND column_name=:column",
    array(':setup' => $setup_id, 'column' => $field['name'])
  );
  db_query("DELETE FROM {tripal_views_handlers} WHERE setup_id=:setup AND column_name=:column",
    array(':setup' => $setup_id, 'column' => $field['name'])
  );
  db_query("DELETE FROM {tripal_views_join} WHERE setup_id=:setup AND base_table=:table AND base_field=:field",
    array(':setup' => $setup_id, ':table' => $table_name, ':field' => $field['name'])
  );

  // Now we need to add/update the field
  $field_record = array(
    'setup_id' => $setup_id,
    'column_name' => $field['name'],
    'name' => $field['title'],
    'description' => $field['description'],
    'type' => $field['type'],
  );
  if ($setup_id && $field['name'] && $field['title'] && $field['description'] && $field['type']) {
    if ($defn_array['additional_content']) {
      // D7 TODO: Check DBTNG changes work
      $is = db_query(
        "SELECT true as present FROM {tripal_views_field} WHERE column_name=:column AND setup_id=:setup",
        array(':column' => $field_record['column_name'], ':setup' => $field_record['setup_id'])
      );
      $is = $is->fetchObject();
      if (!$is->present) {
        $status = drupal_write_record('tripal_views_field', $field_record);
      }
      else {
        $status = drupal_write_record('tripal_views_field', $field_record, array('setup_id', 'column_name'));
      }
    }
    else {
      $status = drupal_write_record('tripal_views_field', $field_record);
    }
  }
  else {
    drupal_set_message(t('Unable to integrate %name field due to missing required fields.', array('%name' => $field['name'])), 'error');
    $status = FALSE;
  }

  if ($status) {

    // Insert Handler Definitions
    foreach ($field['handlers'] as $handler_type => $handler) {
      $handler_record = array(
        'setup_id' => $setup_id,
        'column_name' => $field['name'],
        'handler_type' => $handler_type,
        'handler_name' => $handler['name'],
        'arguments' => serialize($handler)
      );
      if ($setup_id && $field['name'] && $handler_type && $handler['name'] && $handler) {
        $status = drupal_write_record('tripal_views_handlers', $handler_record);
      }
      else {
        $status = FALSE;
      }
      if (!$status) {
        drupal_set_message(t('Unable to integrate %handler_type handler: %handler_name', array('%handler_type' => $handler_type, '%handler_name' => $handler['name'])), 'error');
        $no_errors = FALSE;
      }
    }

    // Insert Joins
    if (!is_array($field['joins'])) {
      $field['joins'] = array();
    }
    foreach ($field['joins'] as $join) {
      $join_record = array(
        'setup_id' => $setup_id,
        'base_table' => $defn_array['table'],
        'base_field' => $field['name'],
        'left_table' => $join['table'],
        'left_field' => $join['field'],
      );

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
            array(
              '%left_table' => $join['table'],
              '%left_field' => $join['field'],
              '%table' => $defn_array['table'],
              '%field' => $field['name']
            )
          ),
          'error'
        );
        $no_errors = FALSE;
      }
    }

  }
  else {
    drupal_set_message(t('Unable to integrate %field_name field', array('%field_name' => $field['name'])), 'error');
    $no_errors = FALSE;
  }

  return $no_errors;
}

/**
 * Remove a join from an integration. This is usually done after cloning an existing
 * integration using tripal_clone_views_integration().
 *
 * @param $setup_id
 *   The setup_id of the integration to delete the join from
 * @param $base_table
 *   The name of the base table involved the join
 * @param $base_field
 *   The field from the base table involved in the join
 * @param $left_table
 *   The name of the other table involved in the join
 * @param $left_field
 *   The name of the field from the other table involved in the join
 *
 * @ingroup tripal_views_api
 */
function tripal_remove_join_from_views_integration($setup_id, $base_table, $base_field, $left_table, $left_field) {
  db_query(
    "DELETE FROM {tripal_views_join} WHERE setup_id=:setup AND base_table=:base-table AND base_field=:base-field AND left_table=:left-table AND left_field=:left-field",
    array(
      ':setup' => $setup_id,
      ':base-table' => $base_table,
      ':base-field' => $base_field,
      ':left-table' => $left_table,
      ':left-field' => $left_field
    )
  );
}
