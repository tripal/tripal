<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal db api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_db().
 *
 * @see tripal_get_db().
 */
function tripal_db_get_db($select_values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db',
      '%new_function' => 'tripal_get_db'
    )
  );

  return tripal_get_db($select_values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_db().
 *
 * @see tripal_get_db().
 */
function tripal_db_get_db_by_db_id($db_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_by_db_id',
      '%new_function' => 'tripal_get_db'
    )
  );

  return tripal_get_db(array('db_id' => $db_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_db().
 *
 * @see tripal_get_db().
 */
function tripal_db_get_db_by_name($name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_by_name',
      '%new_function' => 'tripal_get_db'
    )
  );

  return tripal_get_db(array('name' => $name));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_db_select_options().
 *
 * @see tripal_get_db_select_options().
 */
function tripal_db_get_db_options() {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_options',
      '%new_function' => 'tripal_get_db_select_options'
    )
  );

  return tripal_get_db_select_options();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_db().
 *
 * @see tripal_get_dbxref().
 */
function tripal_db_get_dbxref($select_values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_dbxref',
      '%new_function' => 'tripal_get_dbxref'
    )
  );

  return tripal_get_dbxref($select_values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_get_db().
 *
 * @see tripal_get_dbxref().
 */
function tripal_db_get_dbxref_by_accession($accession, $db_id=0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_dbxref_by_accession',
      '%new_function' => 'tripal_get_dbxref'
    )
  );

  $identifiers = array(
    'accession' => $accession
  );
  if ($db_id > 0) {
    $identifiers['db'] = array(
      'db_id' => $db_id
    );
  }
  return tripal_get_dbxref($identifiers);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_insert_db().
 *
 * @see tripal_insert_db().
 */
function tripal_db_add_db($dbname, $description = '', $url = '', $urlprefix = '', $update = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_db',
      '%new_function' => 'tripal_insert_db'
    )
  );

  return tripal_insert_db(
    array(
      'name' => $dbname,
      'description' => $description,
      'url' => $url,
      'urlprefix' => $urlprefix
    ),
    array(
      'update_existing' => $update
    )
  );
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_insert_dbxref().
 *
 * @see tripal_insert_dbxref().
 */
function tripal_db_add_dbxref($db_id, $accession, $version = '', $description = '') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_dbxref',
      '%new_function' => 'tripal_insert_dbxref'
    )
  );

  return tripal_insert_dbxref(array(
    'db_id' => $db_id,
    'accession' => $accession,
    'version' => $version,
    'description' => $description
  ));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by tripal_associate_dbxref().
 *
 * @see tripal_associate_dbxref().
 */
function tripal_db_add_dbxref_link($linking_table, $dbxref_id, $foreignkey_name, $foreignkey_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_dbxref_link',
      '%new_function' => 'tripal_associate_dbxref'
    )
  );

  if (preg_match('/(\w+)_dbxref/',$linking_table,$matches)) {
    $basetable = $matches[1];

    return tripal_associate_dbxref($basetable, $foreignkey_id, array('dbxref_id' => $dbxref_id));
  }
  else {
    return FALSE;
  }
}
