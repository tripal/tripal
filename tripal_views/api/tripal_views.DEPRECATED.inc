<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal analysis api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_enable_view().
 *
 * @see tripal_enable_view().
 */
function tripal_views_admin_enable_view($view_name, $redirect_link) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_admin_enable_view',
      '%new_function' => 'tripal_enable_view'
    )
  );

  return tripal_enable_view($view_name, $redirect_link);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_disable_view().
 *
 * @see tripal_disable_view().
 */
function tripal_views_admin_disable_view($view_name, $redirect_link) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_admin_disable_view',
      '%new_function' => 'tripal_disable_view'
    )
  );

  return tripal_disable_view($view_name, $redirect_link);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_make_view_compatible_with_external().
 *
 * @see tripal_make_view_compatible_with_external().
 */
function tripal_views_make_view_compatible_with_external($view) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_make_view_compatible_with_external',
      '%new_function' => 'tripal_make_view_compatible_with_external'
    )
  );

  return tripal_make_view_compatible_with_external($view);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_lightest_views_integration_priority().
 *
 * @see tripal_get_lightest_views_integration_priority().
 */
function tripal_views_get_table_lightest_priority($table_name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_get_table_lightest_priority',
      '%new_function' => 'tripal_get_lightest_views_integration_priority'
    )
  );

  return tripal_get_lightest_views_integration_priority($table_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_lightest_views_integration_setup().
 *
 * @see tripal_get_lightest_views_integration_setup().
 */
function tripal_views_get_lightest_priority_setup($table_name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_get_lightest_priority_setup',
      '%new_function' => 'tripal_get_lightest_views_integration_setup'
    )
  );

  return tripal_get_lightest_views_integration_setup($table_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_views_integration_setup_id().
 *
 * @see tripal_get_views_integration_setup_id().
 */
function tripal_views_get_setup_id($table_name, $priority) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_get_setup_id',
      '%new_function' => 'tripal_get_views_integration_setup_id'
    )
  );

  return tripal_get_views_integration_setup_id($table_name, $priority);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_is_table_integrated().
 *
 * @see tripal_is_table_integrated().
 */
function tripal_views_is_integrated($table_name, $priority = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_is_integrated',
      '%new_function' => 'tripal_is_table_integrated'
    )
  );

  return tripal_is_table_integrated($table_name, $priority);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_is_lightest_priority_setup().
 *
 * @see tripal_is_lightest_priority_setup().
 */
function tripal_views_is_lightest_priority_setup($setup_id, $table_name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_is_lightest_priority_setup',
      '%new_function' => 'tripal_is_lightest_priority_setup'
    )
  );

  return tripal_is_lightest_priority_setup($setup_id, $table_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_rebuild_views_integrations().
 *
 * @see tripal_rebuild_views_integrations().
 */
function tripal_views_rebuild_views_integrations($delete_first = FALSE) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_rebuild_views_integrations',
      '%new_function' => 'tripal_rebuild_views_integrations'
    )
  );

  return tripal_rebuild_views_integrations($delete_first);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_add_views_integration().
 *
 * @see tripal_add_views_integration().
 */
function tripal_views_integration_add_entry($defn_array, $setup_id = FALSE) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_integration_add_entry',
      '%new_function' => 'tripal_add_views_integration'
    )
  );

  return tripal_add_views_integration($defn_array, $setup_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_export_views_integration().
 *
 * @see tripal_export_views_integration().
 */
function tripal_views_integration_export_entry($setup_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_integration_export_entry',
      '%new_function' => 'tripal_export_views_integration'
    )
  );

  return tripal_export_views_integration($setup_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_remove_views_integration().
 *
 * @see tripal_remove_views_integration().
 */
function tripal_views_integration_remove_entry_by_table_name($table_name, $priority) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_integration_remove_entry_by_table_name',
      '%new_function' => 'tripal_remove_views_integration'
    )
  );

  return tripal_remove_views_integration(array('table_name' => $table_name, 'priority' => $priority));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_remove_views_integration().
 *
 * @see tripal_remove_views_integration().
 */
function tripal_views_integration_remove_entry_by_setup_id($setup_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_integration_remove_entry_by_setup_id',
      '%new_function' => 'tripal_remove_views_integration'
    )
  );

  return tripal_remove_views_integration(array('setup_id' => $setup_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_update_views_integration().
 *
 * @see tripal_update_views_integration().
 */
function tripal_views_integration_update_entry($setup_id, $defn_array) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_integration_update_entry',
      '%new_function' => 'tripal_update_views_integration'
    )
  );

  return tripal_update_views_integration($setup_id, $defn_array);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_clone_views_integration().
 *
 * @see tripal_clone_views_integration().
 */
function tripal_views_clone_integration($table_name, $new_priority = NULL, $template_priority = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_clone_integration',
      '%new_function' => 'tripal_clone_views_integration'
    )
  );

  return tripal_clone_views_integration($table_name, $new_priority, $template_priority);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_add_field_to_views_integration().
 *
 * @see tripal_add_field_to_views_integration().
 */
function tripal_views_add_field_to_integration($table_name, $priority, $field) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_add_field_to_integration',
      '%new_function' => 'tripal_add_field_to_views_integration'
    )
  );

  return tripal_add_field_to_views_integration($table_name, $priority, $field);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_remove_join_from_views_integration().
 *
 * @see tripal_remove_join_from_views_integration().
 */
function tripal_views_remove_join_from_integration($setup_id, $base_table, $base_field, $left_table, $left_field) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_views_remove_join_from_integration',
      '%new_function' => 'tripal_remove_join_from_views_integration'
    )
  );

  return tripal_remove_join_from_views_integration($setup_id, $base_table, $base_field, $left_table, $left_field);
}
