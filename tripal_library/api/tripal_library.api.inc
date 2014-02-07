<?php
/**
 * @defgroup tripal_library_api Library API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage libraries
 * @}
 */

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
  return chado_get_property('library', $library_id, $property, 'library_property');
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
  return chado_insert_property('library', $library_id, $property, 'library_property', $value, $update_if_present);
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
  return chado_update_property('library', $library_id, $property, 'library_property', $value, $insert_if_missing);
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
  return chado_delete_property('library', $library_id, $property, 'library_property');
}
