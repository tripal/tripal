<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal analysis api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by pub_search_remote().
 *
 * @see pub_search_remote().
 */
function tripal_pub_get_remote_search_results($remote_db, $search_array, $num_to_retrieve, $page = 0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_remote_search_results',
      '%new_function' => 'pub_search_remote'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_remote_pub_record().
 *
 * @see chado_get_remote_pub_record().
 */
function tripal_pub_get_raw_data($dbxref) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_raw_data',
      '%new_function' => 'chado_get_remote_pub_record'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_update_multiple_publications().
 *
 * @see chado_update_multiple_publications().
 */
function tripal_pub_update_publications($do_contact = FALSE, $dbxref = NULL, $db = NULL) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_update_publications',
      '%new_function' => 'chado_update_multiple_publications'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_import_multiple_publications().
 *
 * @see chado_import_multiple_publications().
 */
function tripal_pub_import_publications_by_import_id($import_id, $job_id = NULL) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_import_publications_by_import_id',
      '%new_function' => 'chado_import_multiple_publications'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_import_multiple_publications().
 *
 * @see chado_import_multiple_publications().
 */
function tripal_pub_import_publications($report_email = FALSE, $do_update = FALSE) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_import_publications',
      '%new_function' => 'chado_import_multiple_publications'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_import_multiple_publications().
 *
 * @see chado_import_multiple_publications().
 */
function tripal_pub_import_by_dbxref($pub_dbxref, $do_contact = FALSE, $do_update) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_import_by_dbxref',
      '%new_function' => 'chado_import_multiple_publications'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_multiple_publications().
 *
 * @see chado_insert_multiple_publications().
 */
function tripal_pub_add_publications($pubs, $do_contact, $update = FALSE) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_add_publications',
      '%new_function' => 'chado_insert_multiple_publications'
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
function tripal_pub_add_pub_dbxref($pub_id, $pub_dbxref) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_add_pub_dbxref',
      '%new_function' => 'chado_associate_dbxref'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_publication().
 *
 * @see chado_get_publication().
 */
function tripal_pub_get_pubs_by_dbxref($pub_dbxref) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_pubs_by_dbxref',
      '%new_function' => 'chado_get_publication'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_publication().
 *
 * @see chado_get_publication().
 */
function tripal_pub_get_pubs_by_title_type_pyear_series($title, $type = NULL, $pyear = NULL, $series_name = NULL) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_pubs_by_title_type_pyear_series',
      '%new_function' => 'chado_get_publication'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_publication().
 *
 * @see chado_get_publication().
 */
function tripal_pub_get_pub_by_uniquename($name) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_pub_by_uniquename',
      '%new_function' => 'chado_get_publication'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_publication().
 *
 * @see chado_insert_publication().
 */
function tripal_pub_add_publication($pub_details, &$action, $do_contact = FALSE, $update_if_exists = FALSE) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_add_publication',
      '%new_function' => 'chado_insert_publication'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_insert_multiple_pubauthors().
 *
 * @see chado_insert_multiple_pubauthors().
 */
function tripal_pub_add_authors($pub_id, $authors, $do_contact) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_add_authors',
      '%new_function' => 'chado_insert_multiple_pubauthors'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_pub_get_property($pub_id, $property) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_property',
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
function tripal_pub_insert_property($pub_id, $property, $value, $update_if_present = 0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_insert_property',
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
function tripal_pub_update_property($pub_id, $property, $value, $insert_if_missing = 0) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_update_property',
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
function tripal_pub_delete_property($pub_id, $property) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_publication().
 *
 * @see chado_get_publication().
 */
function tripal_pub_get_publication_array($pub_id, $skip_existing = TRUE) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_publication_array',
      '%new_function' => 'chado_get_publication'
    )
  );

  return FALSE;
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by pub_generate_citation().
 *
 * @see pub_generate_citation().
 */
function tripal_pub_create_citation($pub) {

  tripal_report_error(
    'tripal_api',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_create_citation',
      '%new_function' => 'pub_generate_citation'
    )
  );

  return FALSE;
}