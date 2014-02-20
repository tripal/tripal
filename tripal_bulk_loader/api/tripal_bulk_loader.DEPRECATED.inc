<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal analysis api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_is_bulk_loader_record_name_unique().
 *
 * @see tripal_is_bulk_loader_record_name_unique().
 */
function tripal_bulk_loader_is_record_name_unique($new_record_name, $template_id, $template_array = NULL, $current_priority = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_bulk_loader_is_record_name_unique',
      '%new_function' => 'tripal_is_bulk_loader_record_name_unique'
    )
  );

  return tripal_is_bulk_loader_record_name_unique($new_record_name, $template_id, $template_array, $current_priority);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_delete_bulk_loader_record().
 *
 * @see tripal_delete_bulk_loader_record().
 */
function tripal_bulk_loader_delete_record($delete_priority, $template_array) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_bulk_loader_delete_record',
      '%new_function' => 'tripal_delete_bulk_loader_record'
    )
  );

  return tripal_delete_bulk_loader_record($delete_priority, $template_array);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_delete_bulk_loader_field().
 *
 * @see tripal_delete_bulk_loader_field().
 */
function tripal_bulk_loader_delete_field($priority, $delete_field_index, $template_array) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_bulk_loader_delete_field',
      '%new_function' => 'tripal_delete_bulk_loader_field'
    )
  );

  return tripal_delete_bulk_loader_field($priority, $delete_field_index, $template_array);
}