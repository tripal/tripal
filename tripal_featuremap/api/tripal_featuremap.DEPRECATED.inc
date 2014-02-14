<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal featuremap api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_featuremap_get_property($featuremap_id, $property) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_featuremap_get_property',
      '%new_function' => 'chado_get_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_featuremap_insert_property($featuremap_id, $property, $value, $update_if_present = 0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_featuremap_insert_property',
      '%new_function' => 'chado_insert_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_featuremap_update_property($featuremap_id, $property, $value, $insert_if_missing = 0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_featuremap_update_property',
      '%new_function' => 'chado_update_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_featuremap_delete_property($featuremap_id, $property) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_featuremap_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_associate_dbxref().
 *
 * @see chado_associate_dbxref().
 */
function tripal_featuremap_add_featuremap_dbxref($featuremap_id, $featuremap_dbxref) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_featuremap_add_featuremap_dbxref',
      '%new_function' => 'chado_associate_dbxref'
    )
  );

  return FALSE;
}