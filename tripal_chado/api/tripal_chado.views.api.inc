<?php
/**
 * Removes Chado fields from a view, if Chado is external.
 *
 * Remove any drupal fields from a chado-based default view definition if
 * chado is external. This ensures compatibility with an external chado
 * database.
 *
 * You should be calling this function in your hook_views_default_views().
 * This function will only remove drupal tables if chado is external; thus you
 * do not need to worry about checking yourself. For example, the following is
 * a good hook_views_default_views():
 *
 * @code
 * function mymodule_views_default_views() {
 *   $views = array();
 *
 *   // NOTE: <VIEW-TYPE> describes the type of view:
 *   //     - 'admin' for views used for administration of your module
 *   //     - 'search' for views used to search data
 *   //     - 'list' for views used primarily as data listings
 *
 *   // <VIEW-HUMAN-READABLE-NAME>
 *   $view = mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>();
 *   $view = tripal_make_view_compatible_with_external($view);
 *   $views[$view->name] = $view;*
 *
 *   // <VIEW-HUMAN-READABLE-NAME>
 *   $view = mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>();
 *   $view = tripal_make_view_compatible_with_external($view);
 *   $views[$view->name] = $view;
 *
 *   return $views;
 * }
 *
 * function mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>() {
 *   // PASTE VIEWS EXPORT CODE HERE
 *   return $view;
 * }
 *
 * function mymodule_defaultview_<VIEW-TYPE>_<VIEW-MACHINE-NAME>() {
 *   // PASTE VIEWS EXPORT CODE HERE
 *   return $view;
 * }
 * @endcode
 *
 * Notice that the actual views export code is in a separate function. This
 * makes your hook_views_default_views() more readable.
 *
 * NOTE: Currently assumes all tables not in the tripal views integration
 * tables are Drupal tables.
 *
 * @param $view
 *   The default view definition object
 * @return
 *   The default view with all relationships, fields, filters, sorts,
 *   arguements for Drupal tables removed.
 */
function tripal_make_view_compatible_with_external($view) {
  $remove_table = array();

  // First check that the base table for the view is a chado table
  // If it's not then don't do any filtering
  $schema = chado_get_schema($view->base_table);
  if (!$schema) {
    return $view;
  }
//   $setup_id = tripal_is_table_integrated($view->base_table);
//   if (!$setup_id) {
//     return $view;
//   }

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
            //$setup_id = tripal_is_table_integrated($defn['table']);
            $schema = chado_get_schema($defn['table']);
            //if ($setup_id) {
            if ($schema) {
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
 * Programatically disable view.
 *
 * This should be used in a hook_menu definition as the callback to provide a link
 * to disable the view (first example). It can also be called directly if needed (second example).
 * @code
 * // Create a URL that when the user navigates there, a given view will be disabled.
 * // You will still need to provide a link to this menu item somewhere appropriate.
 * function mymodule_menu() {
 *   $items = array();
 *
 *   // Create one of these for each of your default views
 *   $items['admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/disable'] = array(
 *     'title' => 'Disable <VIEW-HUMAN-READABLE-NAME>',
 *     'page callback' => 'tripal_disable_view',
 *     'page arguments' => array('<VIEW-MACHINE-NAME>', '<PATH-TO-REDIRECT-TO-AFTERWARDS>'),
 *     'access arguments' => array('<YOUR-PERMISSION-KEY>'),
 *     'type' => MENU_CALLBACK,
 *   );
 *
 *   return $items;
 * }
 *
 * // Call this function directly to disable a view
 * // The example shows disabling your own default view when your module is uninstalled
 * function mymodule_uninstall() {
 *
 *   $view_name = '<VIEW-MACHINE-NAME>';
 *   tripal_disable_view($view_name);
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
 * Programatically enable view
 *
 * This should be used in a hook_menu definition as the callback to provide a link
 * to enable the view (first example). It can also be called directly if needed (second example).
 * @code
 * // Create a URL that when the user navigates there, a given view will be enabled.
 * // You will still need to provide a link to this menu item somewhere appropriate (ie: an admin landing page).
 * function mymodule_menu() {
 *   $items = array();
 *
 *   // Create one of these for each of your default views
 *   $items['admin/tripal/<PATH-TO-YOUR-ADMIN-SECTION>/views/<VIEW-MACHINE-NAME>/enable'] = array(
 *     'title' => 'Enable <VIEW-HUMAN-READABLE-NAME>',
 *     'page callback' => 'tripal_enable_view',
 *     'page arguments' => array('<VIEW-MACHINE-NAME>', '<PATH-TO-REDIRECT-TO-AFTERWARDS>'),
 *     'access arguments' => array('<YOUR-PERMISSION-KEY>'),
 *     'type' => MENU_CALLBACK,
 *   );
 *
 *   return $items;
 * }
 *
 * // Call this function directly to disable a view
 * // The example shows enabling your own default view when your module is enabled.
 * // This might be useful if you disable your view when your module is disabled.
 * function mymodule_enable() {
 *   $view_name = '<VIEW-MACHINE-NAME>';
 *   tripal_enable_view($view_name);
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