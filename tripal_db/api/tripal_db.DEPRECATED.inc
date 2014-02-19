<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal db api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by db_retrieve().
 *
 * @see db_retrieve().
 */
function tripal_db_get_db($select_values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db',
      '%new_function' => 'db_retrieve'
    )
  );

  return db_retrieve($select_values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by db_retrieve().
 *
 * @see db_retrieve().
 */
function tripal_db_get_db_by_db_id($db_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_by_db_id',
      '%new_function' => 'db_retrieve'
    )
  );

  return db_retrieve(array('db_id' => $db_id));
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by db_retrieve().
 *
 * @see db_retrieve().
 */
function tripal_db_get_db_by_name($name) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_by_name',
      '%new_function' => 'db_retrieve'
    )
  );

  return db_retrieve(array('name' => $name));
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_db_options',
      '%new_function' => 'db_get_select_options'
    )
  );

  return db_get_select_options();
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by db_retrieve().
 *
 * @see dbxref_retrieve().
 */
function tripal_db_get_dbxref($select_values) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_dbxref',
      '%new_function' => 'dbxref_retrieve'
    )
  );

  return dbxref_retrieve($select_values);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by db_retrieve().
 *
 * @see dbxref_retrieve().
 */
function tripal_db_get_dbxref_by_accession($accession, $db_id=0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_get_dbxref_by_accession',
      '%new_function' => 'dbxref_retrieve'
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
  return dbxref_retrieve($identifiers);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from now.
 * This function has been replaced by chado_db_insert_record().
 *
 * @see chado_db_insert_record().
 */
function tripal_db_add_db($dbname, $description = '', $url = '', $urlprefix = '', $update = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_db',
      '%new_function' => 'chado_db_insert_record'
    )
  );

  return chado_db_insert_record(
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
 * This function has been replaced by chado_dbxref_insert_record().
 *
 * @see chado_dbxref_insert_record().
 */
function tripal_db_add_dbxref($db_id, $accession, $version = '', $description = '') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_dbxref',
      '%new_function' => 'chado_dbxref_insert_record'
    )
  );

  return chado_dbxref_insert_record(array(
    'db_id' => $db_id,
    'accession' => $accession,
    'version' => $version,
    'description' => $description
  ));
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
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    array(
      '%old_function'=>'tripal_db_add_dbxref_link',
      '%new_function' => 'chado_associate_dbxref'
    )
  );

  if (preg_match('/(\w+)_dbxref/',$linking_table,$matches)) {
    $basetable = $matches[1];

    return chado_associate_dbxref($basetable, $foreignkey_id, array('dbxref_id' => $dbxref_id));
  }
  else {
    return FALSE;
  }
}
