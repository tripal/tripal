<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal feature
 *   api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_feature_analysis_get_property($analysis_id = NULL, $feature_id = NULL, $analysisfeature_id = NULL, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_analysis_get_property',
      '%new_function' => 'chado_get_property',
    ]
  );

  // check that the incoming arguments are correct
  if (($analysis_id and !$feature_id) or
    (!$analysis_id and $feature_id)) {
    tripal_report_error('tripal_feature', TRIPAL_WARNING,
      'tripal_feature_analysis_get_property: Both an analysis ID and feature ID should be specified',
      []);
  }

  // get the analysisfeature_id if one is not provided
  if (!$analysisfeature_id) {
    $columns = ['analysisfeature_id'];
    $values = ['analysis_id' => $analysis_id, 'feature_id' => $feature_id];
    $result = chado_select_record('analysisfeature', $columns, $values);
    $analysisfeature_id = $result[0]->analysisfeature_id;
  }

  $record = [
    'table' => 'analysisfeature',
    'id' => $analysisfeature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
  ];

  // get the property.
  return chado_get_property($record, $property);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_insert_property().
 *
 * @see chado_insert_property().
 */
function tripal_feature_analysis_insert_property($analysis_id = NULL, $feature_id = NULL, $analysisfeature_id = NULL, $property, $value, $update_if_present = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_analysis_insert_property',
      '%new_function' => 'chado_insert_property',
    ]
  );

  // check that the incoming arguments are correct
  if (($analysis_id and !$feature_id) or
    (!$analysis_id and $feature_id)) {
    tripal_report_error('tripal_feature', TRIPAL_WARNING,
      'tripal_feature_analysis_insert_property: Both an analysis ID and feature ID should be specified',
      []);
  }

  // get the analysisfeature_id if one is not provided
  if (!$analysisfeature_id) {
    $columns = ['analysisfeature_id'];
    $values = ['analysis_id' => $analysis_id, 'feature_id' => $feature_id];
    $result = chado_select_record('analysisfeature', $columns, $values);
    $analysisfeature_id = $result[0]->analysisfeature_id;
  }

  $record = [
    'table' => 'analysisfeature',
    'id' => $analysisfeature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
    'value' => $value,
  ];
  $options = [
    'update_if_present' => $update_if_present,
  ];

  // insert the property.
  return chado_insert_property($record, $property, $options);

}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_feature_analysis_update_property($analysis_id = NULL, $feature_id = NULL, $analysisfeature_id = NULL, $property, $value, $insert_if_missing = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_analysis_update_property',
      '%new_function' => 'chado_update_property',
    ]
  );

  // check that the incoming arguments are correct
  if (($analysis_id and !$feature_id) or
    (!$analysis_id and $feature_id)) {
    tripal_report_error('tripal_feature', TRIPAL_WARNING,
      'tripal_feature_analysis_update_property: Both an analysis ID and feature ID should be specified',
      []);
  }

  // get the analysisfeature_id if one is not provided
  if (!$analysisfeature_id) {
    $columns = ['analysisfeature_id'];
    $values = ['analysis_id' => $analysis_id, 'feature_id' => $feature_id];
    $result = chado_select_record('analysisfeature', $columns, $values);
    $analysisfeature_id = $result[0]->analysisfeature_id;
  }

  $record = [
    'table' => 'analysisfeature',
    'id' => $analysisfeature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
    'value' => $value,
  ];
  $options = [
    'insert_if_missing' => $insert_if_missing,
  ];

  // update the property.
  return chado_update_property($record, $property, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_feature_analysis_update_property_by_id($analysisfeatureprop_id, $property, $value, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_analysis_update_property_by_id',
      '%new_function' => 'chado_update_property',
    ]
  );

  $record = [
    'table' => 'analysisfeature',
    'prop_id' => $analysisfeatureprop_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
    'value' => $value,
  ];
  // update the property.
  return chado_update_property($record, $property);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_feature_analysis_delete_property($analysis_id = NULL, $feature_id = NULL, $analysisfeature_id = NULL, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_analysis_delete_property',
      '%new_function' => 'chado_delete_property',
    ]
  );

  // check that the incoming arguments are correct
  if (($analysis_id and !$feature_id) or
    (!$analysis_id and $feature_id)) {
    tripal_report_error('tripal_feature', TRIPAL_WARNING,
      'tripal_feature_analysis_delete_property: Both an analysis ID and feature ID should be specified',
      []);
  }

  // get the analysisfeature_id if one is not provided
  if (!$analysisfeature_id) {
    $columns = ['analysisfeature_id'];
    $values = ['analysis_id' => $analysis_id, 'feature_id' => $feature_id];
    $result = chado_select_record('analysisfeature', $columns, $values);
    $analysisfeature_id = $result[0]->analysisfeature_id;
  }

  $record = [
    'table' => 'analysisfeature',
    'id' => $analysisfeature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
  ];

  // get the property.
  return chado_delete_property($record, $property);

}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_delete_record().
 *
 * @see chado_delete_record().
 */
function tripal_feature_analysis_delete_property_by_id($analysisfeatureprop_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_analysis_delete_property_by_id',
      '%new_function' => 'chado_delete_record',
    ]
  );

  // construct the array that will match the exact record to update
  $match = [
    'analysisfeatureprop_id' => $analysisfeatureprop_id,
  ];

  return chado_delete_record('analysisfeatureprop', $match);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_feature_get_property($feature_id, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_get_property',
      '%new_function' => 'chado_get_property',
    ]
  );

  $record = [
    'table' => 'feature',
    'id' => $feature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
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
function tripal_feature_insert_property($feature_id, $property, $value, $update_if_present = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_insert_property',
      '%new_function' => 'chado_insert_property',
    ]
  );

  $record = [
    'table' => 'feature',
    'id' => $feature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
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
function tripal_feature_update_property($feature_id, $property, $value, $insert_if_missing = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_update_property',
      '%new_function' => 'chado_update_property',
    ]
  );

  $record = [
    'table' => 'feature',
    'id' => $feature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
    'value' => $value,
  ];
  $options = [
    'insert_if_missing' => insert_if_missing,
  ];
  return chado_update_property($record, $property, $options);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_update_property().
 *
 * @see chado_update_property().
 */
function tripal_feature_update_property_by_id($featureprop_id, $property, $value, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_update_property_by_id',
      '%new_function' => 'chado_update_property',
    ]
  );

  $record = [
    'table' => 'feature',
    'prop_id' => $featureprop_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
    'value' => $value,
  ];
  return chado_update_property($record, $property);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_feature_delete_property($feature_id, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_delete_property',
      '%new_function' => 'chado_delete_property',
    ]
  );

  $record = [
    'table' => 'feature',
    'id' => $feature_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
  ];

  return chado_delete_property($record, $property);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_delete_record().
 *
 * @see chado_delete_record().
 */
function tripal_feature_delete_property_by_id($featureprop_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_delete_property_by_id',
      '%new_function' => 'chado_delete_record',
    ]
  );

  // construct the array that will match the exact record to update
  $match = [
    'featureprop_id' => $featureprop_id,
  ];

  return chado_delete_record('featureprop', $match);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by
 *   tripal_reverse_compliment_sequence().
 *
 * @see tripal_reverse_compliment_sequence().
 */
function tripal_feature_reverse_complement($sequence) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_reverse_complement',
      '%new_function' => 'tripal_reverse_compliment_sequence',
    ]
  );

  return tripal_reverse_compliment_sequence($sequence);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by tripal_associate_dbxref().
 *
 * @see tripal_associate_dbxref().
 */
function tripal_feature_add_dbxref($feature_id, $dbname, $accession) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_add_dbxref',
      '%new_function' => 'tripal_associate_dbxref',
    ]
  );

  return tripal_associate_dbxref(
    'feature',
    $feature_id,
    [
      'accession' => $accession,
      'db_name' => $dbname,
    ]
  );
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by tripal_associate_cvterm().
 *
 * @see tripal_associate_cvterm().
 */
function tripal_feature_add_cvterm($feature_id, $cvname, $cvterm) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_feature_add_cvterm',
      '%new_function' => 'tripal_associate_cvterm',
    ]
  );

  return tripal_associate_cvterm(
    'feature',
    $feature_id,
    [
      'name' => $cvterm,
      'cv_name' => $cvname,
    ]
  );
}
