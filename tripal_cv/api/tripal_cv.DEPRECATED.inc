<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal cv api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cv_retrieve().
 *
 * @see cv_retrieve().
 */
function tripal_cv_get_cv($select_values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv',
      '%new_function' => 'cv_retrieve'
    )
  );

  return cv_retrieve($select_values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cv_retrieve().
 *
 * @see cv_retrieve().
 */
function tripal_cv_get_cv_by_name($name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_by_name',
      '%new_function' => 'cv_retrieve'
    )
  );

  return cv_retrieve(array('name' => $name));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cv_retrieve().
 *
 * @see cv_retrieve().
 */
function tripal_cv_get_cv_by_id($cv_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_by_id',
      '%new_function' => 'cv_retrieve'
    )
  );

  return cv_retrieve(array('cv_id' => $id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cv_retrieve().
 *
 * @see cv_retrieve().
 */
function tripal_cv_get_cv_id($cv_name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_id',
      '%new_function' => 'cv_retrieve'
    )
  );

  $cv = cv_retrieve(array('name' => $cv_name));
  if (isset($cv->cv_id)) {
    return $cv->cv_id;
  }
  else {
    return FALSE;
  }
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_options',
      '%new_function' => 'cv_get_select_options'
    )
  );

  return cv_get_select_options();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cvterm_retrieve().
 *
 * @see cvterm_retrieve().
 */
function tripal_cv_get_cvterm_by_id($cvterm_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_id',
      '%new_function' => 'cvterm_retrieve'
    )
  );

  return cvterm_retrieve(array('cvterm_id' => $cvterm_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cvterm_retrieve().
 *
 * @see cvterm_retrieve().
 */
function tripal_cv_get_cvterm_by_name($name, $cv_id = NULL, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_name',
      '%new_function' => 'cvterm_retrieve'
    )
  );

  $identifiers = array('name' => $name);
  if (isset($cv_id)) {
    $identifiers['cv_id'] = $cv_id;
  }
  if (isset($cv_name)) {
    $identifiers['cv_id'] = array(
      'name' => $cv_name
    );
  }

  return cvterm_retrieve($identifiers);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cvterm_retrieve().
 *
 * @see cvterm_retrieve().
 */
function tripal_cv_get_cvterm_by_synonym($synonym, $cv_id = NULL, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_synonym',
      '%new_function' => 'cvterm_retrieve'
    )
  );

  return cvterm_retrieve(array(
    'synonym' => array(
      'name' => $synonym,
      'cv_id' => $cv_id,
      'cv_name' => $cv_name
    )
  ));
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_options',
      '%new_function' => 'cvterm_get_select_options'
    )
  );

  return cvterm_get_select_options($cv_id);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_update_cvtermpath',
      '%new_function' => 'chado_update_cvtermpath'
    )
  );

  return chado_update_cvtermpath($cvid, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cv_insert().
 *
 * @see cv_insert().
 */
function tripal_cv_add_cv($name, $definition) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_cv',
      '%new_function' => 'cv_insert'
    )
  );

  return cv_insert($name, $definition);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by cvterm_insert().
 *
 * @see cvterm_insert().
 */
function tripal_cv_add_cvterm($term, $defaultcv = '_global', $is_relationship = 0, $update = 1, $dbname = 'internal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_cvterm',
      '%new_function' => 'cvterm_insert'
    )
  );

  $term['cv_name'] = $defaultcv;
  $term['db_name'] = $dbname;
  $term['is_relationship'] = $is_relationship;

  if (isset($term['def'])) {
    $term['definition'] = $term['def'];
    unset($term['def']);
  }

  return cvterm_insert(
    $term,
    array(
      'update_existing' => $update
    )
  );
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_submit_obo_job',
      '%new_function' => 'tripal_submit_obo_job'
    )
  );

  return tripal_submit_obo_job(
    array(
      'obo_id' => $obo_id,
      'name' => $obo_name,
      'url' => $obo_url,
      'file' => $obo_file
    )
  );
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_insert_obo().
 *
 * @see tripal_insert_obo().
 */
function tripal_cv_add_obo_ref($name, $path) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_obo_ref',
      '%new_function' => 'chado_insert_obo'
    )
  );

  return tripal_insert_obo($name, $ref);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_cvterm_name_autocomplete',
      '%new_function' => 'chado_cvterm_autocomplete'
    )
  );

  return chado_cvterm_autocomplete($cv_id, $string);
}
