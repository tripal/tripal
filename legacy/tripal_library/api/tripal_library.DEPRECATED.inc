<?php
/**
 * @file
 * Wrapper functions to provide backwards compatibility for the tripal library
 *   api
 */

/**
 * @deprecated Restructured API to make naming more readable and consistent.
 * Function was deprecated in Tripal 2.0 and will be removed 2 releases from
 *   now. This function has been replaced by chado_get_property().
 *
 * @see chado_get_property().
 */
function tripal_library_get_property($library_id, $property) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_library_get_property',
      '%new_function' => 'chado_get_property',
    ]
  );

  $record = [
    'table' => 'library',
    'id' => $library_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'library_property',
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
function tripal_library_insert_property($library_id, $property, $value, $update_if_present = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_library_insert_property',
      '%new_function' => 'chado_insert_property',
    ]
  );

  $record = [
    'table' => 'library',
    'id' => $library_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'library_property',
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
function tripal_library_update_property($library_id, $property, $value, $insert_if_missing = 0) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_library_update_property',
      '%new_function' => 'chado_update_property',
    ]
  );
  $record = [
    'table' => 'library',
    'id' => $library_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'library_property',
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
function tripal_library_delete_property($library_id, $property) {

  tripal_report_error(
    'tripal_deprecated',
    TRIPAL_NOTICE,
    "DEPRECATED: %old_function has been replaced with %new_function. Please update your code.",
    [
      '%old_function' => 'tripal_library_delete_property',
      '%new_function' => 'chado_delete_property',
    ]
  );
  $record = [
    'table' => 'library',
    'id' => $library_id,
  ];
  $property = [
    'type_name' => $property,
    'cv_name' => 'library_property',
  ];
  return chado_delete_property($record, $property);
}