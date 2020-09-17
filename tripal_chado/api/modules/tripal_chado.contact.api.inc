<?php
/**
 * @file
 * Provides API functions specifically for managing contact records in Chado.
 *
 * @ingroup tripal_chado
 */

/**
 * @defgroup tripal_contact_api Chado Contact
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions for working with chado records in Chado that
 * go beyond the generic Chado API functions.
 * @}
 */

/**
 * Adds a contact to the Chado contact table.
 *
 * @param $values
 *   An array of values to be inserted. Valid keys include:
 *   - name: The name of the contact.
 *   - description: Text describing the contact.
 *   - type_name: The type of contact.  Must be a term in the tripal_contact
 *     vocabulary.
 *   - properties: An associative array containing a list of key value pairs for
 *     the properites. The key's must be valid terms in the tripal_contact
 *     vocabulary (e.g. Affiliation, Address, etc).
 *
 * @return
 *   On success, an array is returned containing the fields of the contact
 *   record including the newly added contact_id. On failure, FALSE is
 *   returned.
 *
 * @ingroup tripal_contact_api
 */
function chado_insert_contact($values) {

  $name = $values['name'];
  $description = $values['description'];
  $type = $values['type_name'];
  $properties = $values['properties'];

  // check to see if this contact name already exists.
  $values = ['name' => $name];
  $options = ['statement_name' => 'sel_contact_na'];
  $contact = chado_select_record('contact', ['contact_id'], $values, $options);

  if (count($contact) == 0) {
    $cvterm = chado_get_cvterm([
      'name' => $type,
      'cv_id' => ['name' => 'tripal_contact'],
    ]);
    if (!$cvterm) {
      tripal_report_error('tripal_contact', TRIPAL_ERROR, "Cannot find contact type '%type'",
        ['%type' => $type]);
      return FALSE;
    }
    $values = [
      'name' => $name,
      'description' => '',
      'type_id' => $cvterm->cvterm_id,
    ];
    $options = ['statement_name' => 'ins_contact_nadety'];
    $contact = chado_insert_record('contact', $values, $options);
    if (!$contact) {
      tripal_report_error('tripal_contact', TRIPAL_ERROR, 'Could not add the contact', []);
      return FALSE;
    }
  }
  else {
    $contact = (array) $contact[0];
  }

  // add the description property. We don't store this in the contact.description
  // field because it is only 255 characters long and may not be enough
  if ($description) {
    chado_insert_property(
      [
        'table' => 'contact',
        'id' => $contact['contact_id'],
      ],
      [
        'type_name' => 'contact_description',
        'cv_name' => 'tripal_contact',
        'value' => $description,
      ],
      [
        'update_if_present' => TRUE,
      ]
    );
  }

  // add in the other properties provided
  foreach ($properties as $key => $value) {
    $success = chado_insert_property(
      ['table' => 'contact', 'id' => $contact['contact_id']],
      [
        'type_name' => $key,
        'cv_name' => 'tripal_contact',
        'value' => $value,
      ],
      ['update_if_present' => TRUE]
    );

    if (!$success) {
      tripal_report_error('tripal_contact', TRIPAL_ERROR,
        "Could not add the contact property '%prop'", ['%prop' => $key]);
      return FALSE;
    }
  }
  return $contact;
}


/**
 * This function is intended to be used in autocomplete forms for contacts.
 *
 * @param $text
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_contact_api
 */
function chado_autocomplete_contact($text) {
  $matches = [];

  $sql = "SELECT * FROM {contact} WHERE lower(name) like lower(:name) ";
  $args = [];
  $args[':name'] = $text . '%';
  $sql .= "ORDER BY name ";
  $sql .= "LIMIT 25 OFFSET 0 ";
  $results = chado_query($sql, $args);
  $items = [];
  foreach ($results as $contact) {
    // Don't include the null contact
    if ($contact->name == 'null') {
      continue;
    }
    $items[$contact->name] = $contact->name;
  }
  drupal_json_output($items);
}
