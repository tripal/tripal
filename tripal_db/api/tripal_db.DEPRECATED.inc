<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal db api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_db().
 *
 * @see chado_get_db().
 */
function tripal_db_get_db($select_values) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db',
      '%new_function' => 'chado_get_db'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_db().
 *
 * @see chado_get_db().
 */
function tripal_db_get_db_by_db_id($db_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_by_db_id',
      '%new_function' => 'chado_get_db'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_db().
 *
 * @see chado_get_db().
 */
function tripal_db_get_db_by_name($name) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_by_name',
      '%new_function' => 'chado_get_db'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by db_get_select_options().
 *
 * @see db_get_select_options().
 */
function tripal_db_get_db_options() {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_options',
      '%new_function' => 'db_get_select_options'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_db().
 *
 * @see chado_get_dbxref().
 */
function tripal_db_get_dbxref($select_values) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_dbxref',
      '%new_function' => 'chado_get_dbxref'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_db().
 *
 * @see chado_get_dbxref().
 */
function tripal_db_get_dbxref_by_accession($accession, $db_id=0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_dbxref_by_accession',
      '%new_function' => 'chado_get_dbxref'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_db().
 *
 * @see chado_insert_db().
 */
function tripal_db_add_db($dbname, $description = '', $url = '', $urlprefix = '', $update = 0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_db',
      '%new_function' => 'chado_insert_db'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_dbxref().
 *
 * @see chado_insert_dbxref().
 */
function tripal_db_add_dbxref($db_id, $accession, $version = '', $description = '') {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_dbxref',
      '%new_function' => 'chado_insert_dbxref'
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
function tripal_db_add_dbxref_link($linking_table, $dbxref_id, $foreignkey_name, $foreignkey_id) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_dbxref_link',
      '%new_function' => 'chado_associate_dbxref'
    )
  );

  return FALSE;
}
