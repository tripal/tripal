<?php
/**
 * @file
 * Provides an application programming interface (API) to manage projects
 */

/**
 * @defgroup tripal_project_api Project Module API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage projects
 * @}
 */

/**
 * Retrieve properties of a given type for a given project
 *
 * @param $project_id
 *    The project_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 *
 * @return
 *    An project chado variable with the specified properties expanded
 *
 * @ingroup tripal_project_api
 */
function tripal_project_get_property($project_id, $property) {
  return chado_get_property('project', $project_id, $property, 'project_property');
}

/**
 * Insert a given property
 *
 * @param $project_id
 *   The project_id of the property to insert
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
 * @ingroup tripal_project_api
 */
function tripal_project_insert_property($project_id, $property, $value, $update_if_present = 0) {
  return chado_insert_property('project', $project_id, $property, 'project_property', $value, $update_if_present);
}

/**
 * Update a given property
 *
 * @param $project_id
 *   The project_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 *
 * Note: The property will be identified using the unique combination of the $project_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_project_api
 */
function tripal_project_update_property($project_id, $property, $value, $insert_if_missing = 0) {
  return chado_update_property('project', $project_id, $property, 'project_property', $value, $insert_if_missing);
}
/**
 * Delete a given property
 *
 * @param $project_id
 *   The project_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 *
 * Note: The property will be identified using the unique combination of the $project_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_project_api
 */
function tripal_project_delete_property($project_id, $property) {
  return chado_delete_property('project', $project_id, $property, 'project_property');
}
