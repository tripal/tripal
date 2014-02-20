<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal analysis api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_register_analysis_child().
 *
 * @see tripal_register_analysis_child().
 */
function tripal_analysis_register_child($modulename) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_analysis_register_child',
      '%new_function' => 'tripal_register_analysis_child'
    )
  );

  return tripal_register_analysis_child($modulename);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_unregister_analysis_child().
 *
 * @see tripal_unregister_analysis_child().
 */
function tripal_analysis_unregister_child($modulename) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_analysis_unregister_child',
      '%new_function' => 'tripal_unregister_analysis_child'
    )
  );

  return tripal_unregister_analysis_child($modulename);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_analysis_get_property($analysis_id, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_analysis_get_property',
      '%new_function' => 'chado_get_property'
    )
  );

  return chado_get_property('analysis', $analysis_id, $property, $cv_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_analysis_insert_property($analysis_id, $property, $value, $update_if_present = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_analysis_insert_property',
      '%new_function' => 'chado_insert_property'
    )
  );

  return chado_insert_property('analysis', $analysis_id, $property, $cv_name, $value, $update_if_present);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_analysis_update_property($analysis_id, $property, $value, $insert_if_missing = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_analysis_update_property',
      '%new_function' => 'chado_update_property'
    )
  );

  return chado_update_property('analysis', $analysis_id, $property, $cv_name, $value, $insert_if_missing);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by the more generic chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_analysis_delete_property($analysis_id, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_analysis_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  return chado_delete_property('analysis', $analysis_id, $property, $cv_name);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_analysis().
 *
 * @see tripal_get_analysis().
 */
function tripal_analysis_get_node($analysis_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_analysis_get_node',
      '%new_function' => 'tripal_get_analysis'
    )
  );

  return tripal_get_analysis(array('analysis_id' => $analysis_id));
}