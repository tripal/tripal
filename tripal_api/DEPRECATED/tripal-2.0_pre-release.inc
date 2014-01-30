<?php

/**
 * @file
 * Wrappers for functions created for Tripal 2 whose names were changed before release
 */

/**
 * This function is a wrapper for adding fields to an existing form for managing properties.
 * Many of the chado tables have a corresponding 'prop' table (e.g. analysisprop, contactprop,
 * organismprop, etc) and those prop tables all have the same schema.  Use this function
 * to add all the necessary components to a form for allowing the user to add/edit properties to
 * a given record.  To retreive properties in hook_insert or hook_update of a node form use
 * use the function tripal_core_properties_form_retreive().
 *
 * @param $form
 *   The Drupal form array into which the properties elements will be added
 * @param $form_state
 *   The corresponding form_state array for the form
 * @param $prop_table
 *   The name of the property table (e.g. anlaysisprop, contactprop)
 * @param $id_field
 *   The name of the ID field in the property table (e.g. analysis_id, contact_id)
 * @param $cv_name
 *   The name of the controlled vocabulary that these properties are derived from
 * @param $available_props
 *   An array of properties to inclde in the properties drop down.  This array should
 *   have cvterm_id's as the key and the cvterm name as the value
 * @param $id
 *   The current base table ID.  For example, if the property table is analysisprop then the
 *   value should be that of the analysis_id.  If the property table is contactprop then the
 *   value should be that of the contact_id.  This is the record from which currently assigned
 *   properties will be retrieved.
 * @param $exclude
 *   An optional array of cvterms to exclude when retreiving terms already saved in the database.
 *   Use this array when properties are present but should be handled elsewhere.
 *   For example, for contacts, the description field is stored as a property because
 *   the actual field is only 255 characters. The 'contact_description' therefore should
 *   not be shown in the list of properties, even if present, because it is handled by
 *   a different form element.
 * @param $include
 *   An optional array of terms to pre-populate in the form.  This argument can be used to
 *   add a default set of pre-populated properties regardless if they exist in the database
 *   or not.  The array should be of the following form:
 *     array(
 *       array('cvterm' => $obj1, 'value' => $val1),
 *       array('cvterm' => $obj2, 'value' => $val2),
 *       ... etc
 *     );
 *   The 'cvterm' key should have as a value an object with these properties: 'name', 'cvterm_id', 'definition'.
 * @param $instructions
 *   An optional additional set of instructions for the form properties.
 * @param $fset_title
 *   A title for the property field set.  The default is 'Additional Details'.
 * @ingroup tripal_properties_api
 */
function tripal_core_properties_form(&$form, &$form_state, $prop_table, $id_field, $cv_name,
    $available_props, $id = NULL, $exclude = array(), $include = array(), $instructions = '',
    $fset_title = 'Additional Details') {

    // $available_props is now created by the form based on the cv
    // $exclude and $include are not yet supported

    $details = array(
    'property_table' => $prop_table,
    'base_foreign_key' => $id_field,
    'base_key_value' => $id,
    'cv_name' => $cv_name,
    'fieldset_title' => $fset_title,
    'additional_instructions' => $instructions
  );
  tripal_api_chado_node_properties_form($form, $form_state, $details);

}

/**
 * This function is used in a hook_insert, hook_update for a node form
 * when the properties form has been added to the form.  It retrieves all of the properties
 * and returns them in an array of the format:
 *
 *   $properties[<property name>][<rank>] = <value>
 *
 * This array can then be used for inserting or updating properties using the API call
 * tripal_hook_insert_property()
 *
 * @param $node
 * @param $cvname
 *   The name of the controlled vocabulary that the properties belong to
 *
 * @return
 *   A properties array
 *
 * @ingroup tripal_properties_api
 */
function tripal_core_properties_form_retreive($node, $cv_name) {

  return tripal_api_chado_node_properties_form_retreive($node);
}