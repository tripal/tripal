<?php
/**
 * @defgroup tripal_contact_api Contact Module API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage chado contacts
 * @}
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
  return chado_get_property('contact', $contact_id, $property, 'tripal_contact');
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
  return chado_insert_property('contact', $contact_id, $property, 'tripal_contact', $value, $update_if_present);
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
  return chado_update_property('contact', $contact_id, $property, 'tripal_contact', $value, $insert_if_missing);
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
  return chado_delete_property('contact', $contact_id, $property, 'tripal_contact');
}

/**
 * Adds a contact to the Chado contact table
 *
 * @param $name
 *   The name of the contact
 * @param $description
 *   Text describing the contact
 * @param $type
 *   The type of contact.  Must be a term in the tripal_contact vocabulary
 * @param $properties
 *   An associative array containing a list of key value pairs for the properites.
 *   The key's must be valid terms in the tripal_contact vocabulary (e.g. Affiliation,
 *   Address, etc).
 *
 * @return
 *   On success, an array is returned containing the fields of the contact
 *   record including the newly added contact_id. On failure, FALSE is
 *   returned
 *
 * @ingroup tripal_contact_api
 *
 */
function tripal_contact_add_contact($name, $description, $type, $properties) {

  // check to see if this contact name already exists.
  $values =  array('name' => $name);
  $options = array('statement_name' => 'sel_contact_na');
  $contact = chado_select_record('contact', array('contact_id'), $values, $options);

  if (count($contact) == 0) {
    $cvterm = tripal_cv_get_cvterm_by_name($type, NULL, 'tripal_contact');
    if (!$cvterm) {
      tripal_report_error('tripal_contact', TRIPAL_ERROR, "Cannot find contact type '%type'",
        array('%type' => $type));
      return FALSE;
    }
    $values =  array(
      'name' => $name,
      'description' => '',
      'type_id' => $cvterm->cvterm_id,
    );
    $options = array('statement_name' => 'ins_contact_nadety');
    $contact = chado_insert_record('contact', $values, $options);
    if (!$contact) {
      tripal_report_error('tripal_contact', TRIPAL_ERROR, 'Could not add the contact', array());
      return FALSE;
    }
  }
  else {
    $contact = (array) $contact[0];
  }

  // add the description property. We don't store this in the contact.description
  // field because it is only 255 characters long and may not be enough
  if ($description) {
    tripal_contact_insert_property($contact['contact_id'], 'contact_description', $description, TRUE);
  }

  // add in the other properties provided
  foreach ($properties as $key => $value) {
    $success = tripal_contact_insert_property($contact['contact_id'], $key,$value, TRUE);
    if (!$success) {
      tripal_report_error('tripal_contact', TRIPAL_ERROR,
        "Could not add the contact property '%prop'", array('%prop' => $key));
      return FALSE;
    }
  }
  return $contact;
}