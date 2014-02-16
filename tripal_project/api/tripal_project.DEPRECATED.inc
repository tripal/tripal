<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal project api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_project_get_property($project_id, $property) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_project_get_property',
      '%new_function' => 'chado_get_property'
    )
  );

  return chado_get_property('project', $project_id, $property, 'project_property');
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_project_insert_property($project_id, $property, $value, $update_if_present = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_project_insert_property',
      '%new_function' => 'chado_insert_property'
    )
  );

  return chado_insert_property('project', $project_id, $property, 'project_property', $value, $update_if_present);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_project_update_property($project_id, $property, $value, $insert_if_missing = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_project_update_property',
      '%new_function' => 'chado_update_property'
    )
  );

  return chado_update_property('project', $project_id, $property, 'project_property', $value, $insert_if_missing);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_project_delete_property($project_id, $property) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_project_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  return chado_delete_property('project', $project_id, $property, 'project_property');
}