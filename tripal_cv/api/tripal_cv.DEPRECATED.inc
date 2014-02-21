<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal cv api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_cv().
 *
 * @see tripal_get_cv().
 */
function tripal_cv_get_cv($select_values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv',
      '%new_function' => 'tripal_get_cv'
    )
  );

  return tripal_get_cv($select_values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_cv().
 *
 * @see tripal_get_cv().
 */
function tripal_cv_get_cv_by_name($name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_by_name',
      '%new_function' => 'tripal_get_cv'
    )
  );

  return tripal_get_cv(array('name' => $name));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_cv().
 *
 * @see tripal_get_cv().
 */
function tripal_cv_get_cv_by_id($cv_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_by_id',
      '%new_function' => 'tripal_get_cv'
    )
  );

  return tripal_get_cv(array('cv_id' => $id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_cv().
 *
 * @see tripal_get_cv().
 */
function tripal_cv_get_cv_id($cv_name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_id',
      '%new_function' => 'tripal_get_cv'
    )
  );

  $cv = tripal_get_cv(array('name' => $cv_name));
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
 * This function has been replaced by tripal_get_cv_select_options().
 *
 * @see tripal_get_cv_select_options().
 */
function tripal_cv_get_cv_options() {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cv_options',
      '%new_function' => 'tripal_get_cv_select_options'
    )
  );

  return tripal_get_cv_select_options();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_cvterm().
 *
 * @see tripal_get_cvterm().
 */
function tripal_cv_get_cvterm_by_id($cvterm_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_id',
      '%new_function' => 'tripal_get_cvterm'
    )
  );

  return tripal_get_cvterm(array('cvterm_id' => $cvterm_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_cvterm().
 *
 * @see tripal_get_cvterm().
 */
function tripal_cv_get_cvterm_by_name($name, $cv_id = NULL, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_name',
      '%new_function' => 'tripal_get_cvterm'
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

  return tripal_get_cvterm($identifiers);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_cvterm().
 *
 * @see tripal_get_cvterm().
 */
function tripal_cv_get_cvterm_by_synonym($synonym, $cv_id = NULL, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_by_synonym',
      '%new_function' => 'tripal_get_cvterm'
    )
  );

  return tripal_get_cvterm(array(
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
 * This function has been replaced by tripal_get_cvterm_select_options().
 *
 * @see tripal_get_cvterm_select_options().
 */
function tripal_cv_get_cvterm_options($cv_id = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_get_cvterm_options',
      '%new_function' => 'tripal_get_cvterm_select_options'
    )
  );

  return tripal_get_cvterm_select_options($cv_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_update_cvtermpath().
 *
 * @see tripal_update_cvtermpath().
 */
function tripal_cv_update_cvtermpath($cvid, $job_id = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_update_cvtermpath',
      '%new_function' => 'tripal_update_cvtermpath'
    )
  );

  return tripal_update_cvtermpath($cvid, $job_id);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_insert_cv().
 *
 * @see tripal_insert_cv().
 */
function tripal_cv_add_cv($name, $definition) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_cv',
      '%new_function' => 'tripal_insert_cv'
    )
  );

  return tripal_insert_cv($name, $definition);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_insert_cvterm().
 *
 * @see tripal_insert_cvterm().
 */
function tripal_cv_add_cvterm($term, $defaultcv = '_global', $is_relationship = 0, $update = 1, $dbname = 'internal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_add_cvterm',
      '%new_function' => 'tripal_insert_cvterm'
    )
  );

  $term['cv_name'] = $defaultcv;
  $term['db_name'] = $dbname;
  $term['is_relationship'] = $is_relationship;

  if (isset($term['def'])) {
    $term['definition'] = $term['def'];
    unset($term['def']);
  }

  return tripal_insert_cvterm(
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

  return tripal_insert_obo($name, $path);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_autocomplete_cvterm().
 *
 * @see tripal_autocomplete_cvterm().
 */
function tripal_cv_cvterm_name_autocomplete($cv_id, $string = '') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_cv_cvterm_name_autocomplete',
      '%new_function' => 'tripal_autocomplete_cvterm'
    )
  );

  return tripal_autocomplete_cvterm($cv_id, $string);
}
