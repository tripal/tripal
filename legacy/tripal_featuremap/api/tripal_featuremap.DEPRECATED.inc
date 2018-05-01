<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal
 *   featuremap api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_featuremap_get_property($featuremap_id, $property) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_featuremap_get_property',
      '%new_function' => 'chado_get_property',
    ]
  );
  $record = [
    'table' => 'featuremap',
    'id' => $featuremap_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'featuremap_property',
  ];
  return chado_get_property($record, $property);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_featuremap_insert_property($featuremap_id, $property, $value, $update_if_present = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_featuremap_insert_property',
      '%new_function' => 'chado_insert_property',
    ]
  );
  $record = [
    'table' => 'featuremap',
    'id' => $featuremap_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'featuremap_property',
    'value' => $value,
  ];
  $options = [
    'update_if_present' => $update_if_present,
  ];
  return chado_insert_property($record, $property, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_featuremap_update_property($featuremap_id, $property, $value, $insert_if_missing = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_featuremap_update_property',
      '%new_function' => 'chado_update_property',
    ]
  );
  $record = [
    'table' => 'featuremap',
    'id' => $featuremap_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'featuremap_property',
    'value' => $value,
  ];
  $options = [
    'insert_if_missing' => $insert_if_missing,
  ];
  return chado_update_property($record, $property, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_featuremap_delete_property($featuremap_id, $property) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_featuremap_delete_property',
      '%new_function' => 'chado_delete_property',
    ]
  );

  $record = [
    'table' => 'featuremap',
    'id' => $featuremap_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'featuremap_property',
  ];

  return chado_delete_property($record, $property);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_associate_dbxref().
 *
 * @see chado_associate_dbxref().
 */
function tripal_featuremap_add_featuremap_dbxref($featuremap_id, $featuremap_dbxref) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_featuremap_add_featuremap_dbxref',
      '%new_function' => 'chado_associate_dbxref',
    ]
  );

  // break apart the dbxref
  $dbname = '';
  $accession = '';
  if (preg_match('/^(.*?):(.*?)$/', $featuremap_dbxref, $matches)) {
    $dbname = $matches[1];
    $accession = $matches[2];
  }
  else {
    return FALSE;
  }

  // check to see if the featuremap_dbxref record already exist
  $values = [
    'dbxref_id' => [
      'accession' => $accession,
      'db_id' => [
        'name' => $dbname,
      ],
    ],
    'featuremap_id' => $featuremap_id,
  ];
  $options = ['statement_name' => 'sel_featuremapdbxref_dbpu'];
  $results = chado_select_record('featuremap_dbxref', ['*'], $values, $options);

  // if the featuremap_dbxref record  exist then we don't need to re-add it.
  if (count($results) > 0) {
    return $results[0];
  }

  // make sure our database already exists
  $db = tripal_db_add_db($dbname);

  // get the database cross-reference
  $dbxvalues = [
    'accession' => $accession,
    'db_id' => $db->db_id,
  ];
  $dbxoptions = ['statement_name' => 'sel_dbxref_acdb'];
  $results = chado_select_record('dbxref', ['dbxref_id'], $dbxvalues, $dbxoptions);
  // if the accession doesn't exist then add it
  if (count($results) == 0) {
    $dbxref = tripal_db_add_dbxref($db->db_id, $accession);
  }
  else {
    $dbxref = $results[0];
  }

  // now add the record
  $options = ['statement_name' => 'ins_featuremapdbxref_dbpu'];
  $results = chado_insert_record('featuremap_dbxref', $values, $options);
  if (!$results) {
    tripal_report_error('t_featuremap', TRIPAL_ERROR, "Cannot add map dbxref: %db:%accession.",
      ['%db' => $dbname, '%accession' => $accession]);
    return FALSE;
  }
  return $results;
}