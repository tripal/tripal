<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal organism api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by organism_retrieve().
 *
 * @see organism_retrieve().
 */
function tripal_organism_get_organism_by_nid($nid) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_organism_by_nid',
      '%new_function' => 'organism_retrieve'
    )
  );

  return organism_retrieve(array('nid' => $nid));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by organism_retrieve().
 *
 * @see organism_retrieve().
 */
function tripal_organism_get_organism_by_organism_id($organism_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_organism_by_organism_id',
      '%new_function' => 'organism_retrieve'
    )
  );

  return organism_retrieve(array('organism_id' => $organism_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by organism_get_select_options().
 *
 * @see organism_get_select_options().
 */
function tripal_organism_get_synced() {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_synced',
      '%new_function' => 'organism_get_select_options'
    )
  );

  return organism_get_select_options();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by organism_get_image().
 *
 * @see organism_get_image().
 */
function tripal_organism_get_image_url($organism, $nid = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_organism_get_image_url',
      '%new_function' => 'organism_get_image'
    )
  );

  return organism_get_image($organism, $nid);
}