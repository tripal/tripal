<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal cv api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_cv().
 *
 * @see chado_get_cv().
 */
function tripal_cv_get_cv($select_values) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv',
      '%new_function' => 'chado_get_cv'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_cv().
 *
 * @see chado_get_cv().
 */
function tripal_cv_get_cv_by_name($name) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_by_name',
      '%new_function' => 'chado_get_cv'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_cv().
 *
 * @see chado_get_cv().
 */
function tripal_cv_get_cv_by_id($cv_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_by_id',
      '%new_function' => 'chado_get_cv'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_cv().
 *
 * @see chado_get_cv().
 */
function tripal_cv_get_cv_id($cv_name) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_id',
      '%new_function' => 'chado_get_cv'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cv_get_select_options().
 *
 * @see cv_get_select_options().
 */
function tripal_cv_get_cv_options() {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_options',
      '%new_function' => 'cv_get_select_options'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_cvterm().
 *
 * @see chado_get_cvterm().
 */
function tripal_cv_get_cvterm_by_id($cvterm_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_id',
      '%new_function' => 'chado_get_cvterm'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_cvterm().
 *
 * @see chado_get_cvterm().
 */
function tripal_cv_get_cvterm_by_name($name, $cv_id = NULL, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_name',
      '%new_function' => 'chado_get_cvterm'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_cvterm().
 *
 * @see chado_get_cvterm().
 */
function tripal_cv_get_cvterm_by_synonym($synonym, $cv_id = NULL, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_synonym',
      '%new_function' => 'chado_get_cvterm'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cvterm_get_select_options().
 *
 * @see cvterm_get_select_options().
 */
function tripal_cv_get_cvterm_options($cv_id = 0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_options',
      '%new_function' => 'cvterm_get_select_options'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_cvtermpath().
 *
 * @see chado_update_cvtermpath().
 */
function tripal_cv_update_cvtermpath($cvid, $job_id = NULL) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_update_cvtermpath',
      '%new_function' => 'chado_update_cvtermpath'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_cv().
 *
 * @see chado_insert_cv().
 */
function tripal_cv_add_cv($name, $definition) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_cv',
      '%new_function' => 'chado_insert_cv'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_cvterm().
 *
 * @see chado_insert_cvterm().
 */
function tripal_cv_add_cvterm($term, $defaultcv = '_global', $is_relationship = 0, $update = 1, $dbname = 'internal') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_cvterm',
      '%new_function' => 'chado_insert_cvterm'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_submit_obo_job().
 *
 * @see tripal_submit_obo_job().
 */
function tripal_cv_submit_obo_job($obo_id = NULL, $obo_name = NULL, $obo_url = NULL, $obo_file = NULL) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_submit_obo_job',
      '%new_function' => 'tripal_submit_obo_job'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_obo().
 *
 * @see chado_insert_obo().
 */
function tripal_cv_add_obo_ref($name, $path) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_obo_ref',
      '%new_function' => 'chado_insert_obo'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_cvterm_autocomplete().
 *
 * @see chado_cvterm_autocomplete().
 */
function tripal_cv_cvterm_name_autocomplete($cv_id, $string = '') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_cvterm_name_autocomplete',
      '%new_function' => 'chado_cvterm_autocomplete'
    )
  );

  return FALSE;
}
