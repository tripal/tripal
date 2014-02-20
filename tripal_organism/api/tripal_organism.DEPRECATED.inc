<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal organism api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_organism().
 *
 * @see tripal_get_organism().
 */
function tripal_organism_get_organism_by_nid($nid) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_organism_by_nid',
      '%new_function' => 'tripal_get_organism'
    )
  );

  return tripal_get_organism(array('nid' => $nid));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_organism().
 *
 * @see tripal_get_organism().
 */
function tripal_organism_get_organism_by_organism_id($organism_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_organism_by_organism_id',
      '%new_function' => 'tripal_get_organism'
    )
  );

  return tripal_get_organism(array('organism_id' => $organism_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_organism_select_options().
 *
 * @see tripal_get_organism_select_options().
 */
function tripal_organism_get_synced() {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_synced',
      '%new_function' => 'tripal_get_organism_select_options'
    )
  );

  return tripal_get_organism_select_options();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_organism_image().
 *
 * @see tripal_get_organism_image().
 */
function tripal_organism_get_image_url($organism, $nid = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_image_url',
      '%new_function' => 'tripal_get_organism_image'
    )
  );

  return tripal_get_organism_image($organism, $nid);
}