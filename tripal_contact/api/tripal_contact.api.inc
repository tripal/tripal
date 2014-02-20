<?php
/**
 * @file
 * Functions to interact with contacts.
 *
 * @ingroup tripal_contact
 */

/**
 * @defgroup tripal_contact_api Contact Module API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage chado contacts
 * @}
 */

/**
 * Adds a contact to the Chado contact table
 *
 * @param $values
 *   An array of values to be inserted. Valid keys include:
 *   - name: The name of the contact
 *   - description: Text describing the contact
 *   - type_name: The type of contact.  Must be a term in the tripal_contact vocabulary
 *   - properties: An associative array containing a list of key value pairs for the properites.
 *     The key's must be valid terms in the tripal_contact vocabulary (e.g. Affiliation,
 *     Address, etc).
 *
 * @return
 *   On success, an array is returned containing the fields of the contact
 *   record including the newly added contact_id. On failure, FALSE is
 *   returned
 *
 * @ingroup tripal_contact_api
 */
function tripal_insert_contact($values) {

  $name = $values['name'];
  $description = $values['description'];
  $type = $values['type_name'];
  $properties = $values['properties'];

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
