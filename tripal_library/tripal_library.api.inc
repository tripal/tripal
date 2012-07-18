<?php
/**
 * @file
 * @todo Add file header description
 */

/**
 * @defgroup tripal_library_api Library Module API
 * @ingroup tripal_api
 * @ingroup tripal_library
 */

/**
 * Implements hook_chado_schema_v1_11_table()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the library table
 *
 * @ingroup tripal_schema_api
 */
function tripal_library_chado_schema_v1_11_library() {
  $description = array();
  $referring_tables = array(
      'library_cvterm',
      'library_feature',
      'library_pub',
      'library_synonym',
      'libraryprop'
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

/**
 * Implements hook_chado_schema_v1_2_table()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the library table
 *
 * @ingroup tripal_schema_api
 */
function tripal_library_chado_schema_v1_2_library() {
  $description = array();
  $referring_tables = array(
      'library_cvterm',
      'library_feature',
      'library_pub',
      'library_synonym',
      'libraryprop'
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}



/**
 * Retrieve properties of a given type for a given library
 *
 * @param $library_id
 *    The library_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 *
 * @return
 *    An library chado variable with the specified properties expanded
 *
 * @ingroup tripal_library_api
 */
function tripal_library_get_property($library_id, $property) {
  return tripal_core_get_property('library', $library_id, $property, 'tripal');
}

/**
 * Insert a given property
 *
 * @param $library_id
 *   The library_id of the property to insert
 * @param $property
 *   The cvterm name of the property to insert
 * @param $value
 *   The value of the property to insert
 * @param $update_if_present
 *   A boolean indicated whether to update the record if it's already present
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_library_api
 */
function tripal_library_insert_property($library_id, $property, $value, $update_if_present = 0) {
  return tripal_core_insert_property('library', $library_id, $property, 'tripal', $value, $update_if_present);
}

/**
 * Update a given property
 *
 * @param $library_id
 *   The library_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 *
 * Note: The property will be identified using the unique combination of the $library_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_library_api
 */
function tripal_library_update_property($library_id, $property, $value, $insert_if_missing = 0) {
  return tripal_core_update_property('library', $library_id, $property, 'tripal', $value, $insert_if_missing);
}
/**
 * Delete a given property
 *
 * @param $library_id
 *   The library_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 *
 * Note: The property will be identified using the unique combination of the $library_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_library_api
 */
function tripal_library_delete_property($library_id, $property) {
  return tripal_core_delete_property('library', $library_id, $property, 'tripal');
}
