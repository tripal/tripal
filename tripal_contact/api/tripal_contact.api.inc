<?php
/**
 * @file
 * Provides an application programming interface (API) to manage libraries
 *
 * @defgroup tripal_contact_api contact Module API
 * @ingroup tripal_api
 */

/**
 * Retrieve properties of a given type for a given contact
 *
 * @param $contact_id
 *    The contact_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 *
 * @return
 *    An contact chado variable with the specified properties expanded
 *
 * @ingroup tripal_contact_api
 */
function tripal_contact_get_property($contact_id, $property) {
  return tripal_core_get_property('contact', $contact_id, $property, 'tripal_contact');
}

/**
 * Insert a given property
 *
 * @param $contact_id
 *   The contact_id of the property to insert
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
 * @ingroup tripal_contact_api
 */
function tripal_contact_insert_property($contact_id, $property, $value, $update_if_present = 0) {
  return tripal_core_insert_property('contact', $contact_id, $property, 'tripal_contact', $value, $update_if_present);
}

/**
 * Update a given property
 *
 * @param $contact_id
 *   The contact_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 *
 * Note: The property will be identified using the unique combination of the $contact_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_contact_api
 */
function tripal_contact_update_property($contact_id, $property, $value, $insert_if_missing = 0) {
  return tripal_core_update_property('contact', $contact_id, $property, 'tripal_contact', $value, $insert_if_missing);
}
/**
 * Delete a given property
 *
 * @param $contact_id
 *   The contact_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 *
 * Note: The property will be identified using the unique combination of the $contact_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_contact_api
 */
function tripal_contact_delete_property($contact_id, $property) {
  return tripal_core_delete_property('contact', $contact_id, $property, 'tripal_contact');
}
