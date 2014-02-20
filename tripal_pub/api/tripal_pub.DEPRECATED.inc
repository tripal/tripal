<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal analysis api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_search_publications().
 *
 * @see tripal_search_publications().
 */
function pub_search($search_array, $offset, $limit, &$total_records) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'pub_search',
      '%new_function' => 'tripal_search_pubs'
    )
  );

  return tripal_search_publications($search_array, $offset, $limit, $total_records);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_publication().
 *
 * @see tripal_get_publication().
 */
function chado_get_publication($identifiers, $options = array()) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_get_publication',
      '%new_function' => 'tripal_get_publication'
    )
  );

  return tripal_get_publication($identifiers, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_publication_exists().
 *
 * @see tripal_publication_exists().
 */
function chado_does_pub_exist($pub_details) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_does_pub_exist',
      '%new_function' => 'tripal_publication_exists'
    )
  );

  return tripal_publication_exists($pub_details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_reimport_publications().
 *
 * @see tripal_reimport_publications().
 */
function chado_reimport_publications($do_contact = FALSE, $dbxref = NULL, $db = NULL) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'chado_reimport_publications',
      '%new_function' => 'tripal_reimport_publications'
    )
  );

  return tripal_reimport_publications($do_contact, $dbxref, $db);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_remote_pubs().
 *
 * @see tripal_get_remote_pubs().
 */
function tripal_pub_get_remote_search_results($remote_db, $search_array, $num_to_retrieve, $page = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_remote_search_results',
      '%new_function' => 'tripal_get_remote_pubs'
    )
  );

  return tripal_get_remote_pubs($remote_db, $search_array, $num_to_retrieve, $page);;
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_raw_data',
      '%new_function' => 'chado_get_remote_pub_record'
    )
  );

  return tripal_get_remote_pub($dbxref);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_update_publications',
      '%new_function' => 'chado_reimport_publications'
    )
  );

  return chado_reimport_publications($do_contact, $dbxref, $db);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_import_publications_by_import_id',
      '%new_function' => 'tripal_execute_pub_importer'
    )
  );

  return tripal_execute_pub_importer($import_id, $job_id);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_import_publications',
      '%new_function' => 'tripal_execute_active_pub_importers'
    )
  );

  return tripal_execute_active_pub_importers($report_email, $do_update);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_import_by_dbxref',
      '%new_function' => 'chado_import_multiple_publications'
    )
  );

  return tripal_import_pub_by_dbxref($pub_dbxref, $do_contact, $do_udpate);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_associate_dbxref().
 *
 * @see tripal_associate_dbxref().
 */
function tripal_pub_add_pub_dbxref($pub_id, $pub_dbxref) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_add_pub_dbxref',
      '%new_function' => 'tripal_associate_dbxref'
    )
  );

  $dbxref = array();
  if(preg_match('/^(.*?):(.*?)$/', trim($pub_dbxref), $matches)) {
    $dbxref['db_name']   = $matches[1];
    $dbxref['accession'] = $matches[2];
  }
  return tripal_associate_dbxref('pub', $pub_id, $dbxref);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_pubs_by_dbxref',
      '%new_function' => 'chado_get_publication'
    )
  );

  $pub = chado_get_publication(array('dbxref' => $pub_dbxref));
  if ($pub) {
    // the original function returned an array of pub_ids
    return array($pub->pub_id);
  }
  else {
    return array();
  }

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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_pubs_by_title_type_pyear_series',
      '%new_function' => 'chado_get_publication'
    )
  );

  $pub_details = array(
    'Title' => $title,
    'Year' => $pyear,
    'Series Name' => $series_name,
    'Publication Type' => $type,
  );
  return chado_does_pub_exist($pub_details);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_get_publication().
 *
 * @see chado_get_publication().
 */
function tripal_pub_get_pub_by_uniquename($uniquenname) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_pub_by_uniquename',
      '%new_function' => 'chado_get_publication'
    )
  );

  $pub = chado_get_publication(array('uniquename' => $uniquenname));
  if ($pub) {
    // the original version of this function returned an array of matching pub_ids
    return array($pub->pub_id);
  }
  return array();
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_get_property',
      '%new_function' => 'chado_get_property'
    )
  );

  return chado_get_property('pub', $pub_id, $property, 'tripal_pub');
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_insert_property',
      '%new_function' => 'chado_insert_property'
    )
  );

  return chado_insert_property('pub', $pub_id, $property, 'tripal_pub', $value, $update_if_present);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_update_property',
      '%new_function' => 'chado_update_property'
    )
  );

  return chado_update_property('pub', $pub_id, $property, 'tripal_pub', $value, $insert_if_missing);
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_pub_delete_property',
      '%new_function' => 'chado_delete_property'
    )
  );

  return chado_delete_property('pub', $pub_id, $property, 'tripal_pub');
}
