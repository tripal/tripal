<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal analysis
 *   api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by tripal_register_analysis_child().
 *
 * @see tripal_register_analysis_child().
 */
function tripal_analysis_register_child($modulename) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_analysis_register_child',
      '%new_function' => 'tripal_register_analysis_child',
    ]
  );

  return tripal_register_analysis_child($modulename);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by
 *   tripal_unregister_analysis_child().
 *
 * @see tripal_unregister_analysis_child().
 */
function tripal_analysis_unregister_child($modulename) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_analysis_unregister_child',
      '%new_function' => 'tripal_unregister_analysis_child',
    ]
  );

  return tripal_unregister_analysis_child($modulename);
}

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_analysis_get_property($analysis_id, $property, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_analysis_get_property',
      '%new_function' => 'chado_get_property',
    ]
  );

  $record = [
    'table' => 'analysis',
    'id' => $analysis_id,
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
function tripal_analysis_insert_property($analysis_id, $property, $value, $update_if_present = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_analysis_insert_property',
      '%new_function' => 'chado_insert_property',
    ]
  );

  $record = [
    'table' => 'analysis',
    'id' => $analysis_id,
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
function tripal_analysis_update_property($analysis_id, $property, $value, $insert_if_missing = 0, $cv_name = 'tripal') {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_analysis_update_property',
      '%new_function' => 'chado_update_property',
    ]
  );

  $record = [
    'table' => 'analysis',
    'id' => $analysis_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => $cv_name,
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
 *   now. This function has been replaced by the more generic
 *   chado_delete_property().
 *
 * @see chado_delete_property().
 */
function tripal_analysis_delete_property($analysis_id, $property, $cv_name = 'tripal') {

  tripal_report_error('tripal_deprecated', TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_analysis_delete_property',
      '%new_function' => 'chado_delete_property',
    ]
  );
  $record = [
    'table' => 'analysis',
    'id' => $analysis_id,
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
 *   now. This function has been replaced by tripal_get_analysis().
 *
 * @see tripal_get_analysis().
 */
function tripal_analysis_get_node($analysis_id) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_analysis_get_node',
      '%new_function' => 'tripal_get_analysis',
    ]
  );

  return tripal_get_analysis(['analysis_id' => $analysis_id]);
}