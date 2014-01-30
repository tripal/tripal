<?php

/**
 * @file
 * API to manage the chado prop table for various Tripal Node Types
 *
 * How To Use:
 * @code

  function chado_example_form($form, $form_state) {

    // Default values for form elements can come in the following ways:
    //
    // 1) as elements of the $node object.  This occurs when editing an existing node
    // 2) in the $form_state['values'] array which occurs on a failed validation or
    //    ajax callbacks when the ajax call originates from non-submit fields other
    //    than button
    // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
    //    form elements (e.g. buttons) and the form is being rebuilt but has not yet
    //    been validated
    //
    // The properties elements added by this function do use AJAX calls from buttons,
    // therefore, it is important to check for form values in the $form_state['values']
    // for case #2 above, and in the $form_state['input'] for case #3.
    // See the chado analysis node form for an example.


    // Next, add in all the form array definition particular to your node type

    // To add in the properties form elements, you first need to prepare the arguments
    // for the function call.  One inportant argument is a list of properties that
    // will be made avaialble for the user to select from.  You should query the
    // database to retrieve the applicable terms from the cvterm table and store them in an array
    // of where the cvterm.cvterm_id field is the key and the cvterm.name is the value.
    // these terms should all be from the same vocabulary.
    $properties = array();
    $properties[] = 'Select a Property';
    $sql = "
      SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition
      FROM  {cvterm} CVT
        INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
      WHERE
        CV.name = :ontology_name AND
        NOT CVT.is_obsolete = 1
      ORDER BY CVT.name ASC
    ";

    $ontology_name = 'name_of_proptype_ontology';  //you need to set this variable with the cv.name of the ontology governing your prop tables type_id
    $prop_types = chado_query($sql, array(':ontology_name' => $ontology_name));
    while ($prop = $prop_types->fetchObject()) {
      $properties[$prop->cvterm_id] = $prop->name;
    }

    // the properties form will add a select dropdown of terms containing the items in the $properties array
    // constructed above, but it will also pre-populate rows of properties that already are associated
    // with the object.  If you would like to pre-populated properties regardless if they exist in the database
    // or not, you can create an $include array which has the following format:
    //     array(
    //       array('cvterm' => $obj1, 'value' => $val1),
    //       array('cvterm' => $obj2, 'value' => $val2),
    //       ... etc
    //     );
    //   The 'cvterm' key should have as a value an object with these properties: 'name', 'cvterm_id', 'definition'.
    $include = array();

    // sometimes a property exists in the database and is properly associated with the object, but we do
    // not want it to appear in the list of properties that are pre-populated. It may be handled in some
    // other way.  For example, for contacts, the description field is stored as a property because
    // the actual contact.description field is only 255 characters. The 'contact_description' property should
    // not be shown in the list of properties, even if present, because it is handled by
    // a different form element.  This array holds the value of the cvterm.name column of the cvterms
    // to exclude
    $exclude = array();

    // the instructions argument provides additional instructions to the user beyond the default instructions.
    $instructions = t('To add additional properties to the drop down. ' . l("Add terms to the $ontology_name vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");

    // Finally, and add the properties form elements to the form
    tripal_core_properties_form(
      $form, $form_state,     // form and form_state of the current form
      'exampleprop',          // properties table name
      'example_id',           // key to link to the chado content created by this node
      $ontology_name,         // name of ontology governing your prop table type_id column
      $properties,            // an array of properties to use in the drop-down
      $example_id,            // the value of the above key
      $exclude,               // elements from the ontology you don't want to be available as property types
      $include,               // additional elements not in the ontology that you do what in the drop-down
      $instructions,          // form specific instructions
      'Properties'            // name of the fieldset
    );

    return $form;
  }

  function chado_example_insert($node) {

    // if there is an example_id in the $node object then this must be a sync so
    // we can skip adding the chado_example as it is already there, although
    // we do need to proceed with the rest of the insert
    if (!property_exists($node, 'example_id')) {

      // Add record to chado example table

      // Add to any other tables needed

      // Add each property (exampleprop table). The tripal_core_properties_form_retrieve()
      // function retrieves all of the properties and returns them in an array of the format:
      //
      // $properties[<property name>][<rank>] = <value
      //
      // This array can then be used for inserting or updating properties using the API call
      //   tripal_hook_insert_property()
      //
      // example_property = controlled vocab name for exampleprop.type_id
      $properties = tripal_core_properties_form_retreive($node, 'example_property');
      foreach ($properties as $property => $elements) {
        foreach ($elements as $rank => $value) {

          $success = tripal_core_insert_property(
            'example',        //base table name
            $example_id,      // key to link to the chado content created by this node
            $property,        // cvterm.name of the property to be added
            $ontology_name,   // name of the ontology the cvterm is from
            $value            // the value o the property
          );

          if (!$success) {
            watchdog(
              'tripal_example',
              'Example Update: Unable to insert property %cvterm %value.',
              array('%cvterm' => $property, '%value' => $value),
              WATCHDOG_ERROR
            );
          }
        }
      }
    }

    // Add record to chado_example linking example_id to new node

  }

  function chado_example_update($node) {


      // Update record in chado example table

      // Update any other tables needed

      // First delete any existing properties
      tripal_core_chado_delete(
        'exampleprop',                        // the name of the prop table
        array('example_id' => $example_id)    // name of your key and the current value used to determine which to delete
      );

      // Add each property (exampleprop table)
      // example_property = controlled vocab name for exampleprop.type_id
      $properties = tripal_core_properties_form_retreive($node, 'example_property');
      foreach ($properties as $property => $elements) {
        foreach ($elements as $rank => $value) {

          $success = tripal_core_insert_property(
            'example',        //base table name
            $example_id,      // key to link to the chado content created by this node
            $property,        // cvterm.name of the property to be added
            $ontology_name,   // name of the ontology the cvterm is from
            $value            // the value o the property
          );

          if (!$success) {
            watchdog(
              'tripal_example',
              'Example Update: Unable to insert property %cvterm %value.',
              array('%cvterm' => $property, '%value' => $value),
              WATCHDOG_ERROR
            );
          }
        }
      }
    }

    // Don't need to update chado_example linking table since niether example_id or nid can be changed in update

  }

 * @endcode
 */

/**
 * Retrieve a property for a given base table record
 *
 * @param $basetable
 *   The base table for which the property should be retrieved. Thus to retrieve a property
 *   for a feature the basetable=feature and property is retrieved from featureprop
 * @param $record_id
 *   The foriegn key field of the base table. This should be in integer.
 * @param $property
 *   The cvterm name describing the type of properties to be retrieved
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 *
 * @return
 *   An array in the same format as that generated by the function
 *   tripal_core_generate_chado_var().  If only one record is returned it
 *   is a single object.  If more than one record is returned then it is an array
 *   of objects
 *
 * @ingroup tripal_properties_api
 */
function tripal_core_get_property($basetable, $record_id, $property, $cv_name) {
  // get the foreign key for this property table
  $table_desc = tripal_core_get_chado_table_schema($basetable . 'prop');
  $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

  // construct the array of values to be selected
  $values = array(
    $fkcol => $record_id,
    'type_id' => array(
      'cv_id' => array(
        'name' => $cv_name,
      ),
      'name' => $property,
      'is_obsolete' => 0
    ),
  );
  $results = tripal_core_generate_chado_var($basetable . 'prop', $values);
  if ($results) {
    $results = tripal_core_expand_chado_vars($results, 'field', $basetable . 'prop.value');
  }

  return $results;
}

/**
 * Insert a property for a given base table.  By default if the property already
 * exists a new property is added with the next available rank.  If
 * $update_if_present argument is specified then the record will be updated if it
 * exists rather than adding a new property.
 *
 * @param $basetable
 *   The base table for which the property should be inserted. Thus to insert a property
 *   for a feature the basetable=feature and property is inserted into featureprop
 * @param $record_id
 *   The foriegn key value of the base table. This should be in integer.
 * @param $property
 *   The cvterm name describing the type of properties to be inserted
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 * @param $value
 *   The value of the property to be inserted (can be empty)
 * @param $update_if_present
 *   A boolean indicating whether an existing record should be updated. If the
 *   property already exists and this value is not specified or is zero then
 *   a new property will be added with the next largest rank.
 *
 * @return
 *   Return True on Insert/Update and False otherwise
 *
 * @ingroup tripal_properties_api
 */
function tripal_core_insert_property($basetable, $record_id, $property,
$cv_name, $value, $update_if_present = 0) {

  // first see if the property already exists, if the user want's to update
  // then we can do that, but otherwise we want to increment the rank and
  // insert
  $props = tripal_core_get_property($basetable, $record_id, $property, $cv_name);
  if (!is_array($props) and $props) {
    $props = array($props);
  }

  $rank = 0;
  if (count($props) > 0) {
    if ($update_if_present) {
      return tripal_core_update_property($basetable, $record_id, $property, $cv_name, $value);
    }
    else {
      // iterate through the properties returned and check to see if the
      // property with this value already exists if not, get the largest rank
      // and insert the same property but with this new value
      foreach ($props as $p) {
        if ($p->rank > $rank) {
          $rank = $p->rank;
        }
        if (strcmp($p->value, $value) == 0) {
          return TRUE;
        }
      }
      // now add 1 to the rank
      $rank++;
    }
  }

  // make sure the cvterm exists.  Otherwise we'll get an error with
  // prepared statements not matching
  $values = array(
    'cv_id' => array(
      'name' => $cv_name,
    ),
    'name' => $property,
  );

  $options = array();
  $term = tripal_core_chado_select('cvterm', array('cvterm_id'), $values, $options);
  if (!$term or count($term) == 0) {
    watchdog('tripal_core', "Cannot find property '%prop_name' in vocabulary '%cvname'.",
    array('%prop_name' => $property, '%cvname' => $cv_name), WATCHDOG_ERROR);
    return FALSE;
  }

  // get the foreign key for this property table
  $table_desc = tripal_core_get_chado_table_schema($basetable . 'prop');
  $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

  // construct the array of values to be inserted
  $values = array(
    $fkcol => $record_id,
    'type_id' => array(
      'cv_id' => array(
        'name' => $cv_name,
      ),
      'name' => $property,
    ),
    'value' => $value,
    'rank' => $rank,
  );

  $options = array();
  $result = tripal_core_chado_insert($basetable . 'prop', $values, $options);
  return $result;
}

/**
 * Update a property for a given base table record and property name.  This
 * function should be used only if one record of the property will be present.
 * If the property name can have multiple entries (with increasing rank) then
 * use the function named tripal_core_update_property_by_id
 *
 * @param $basetable
 *   The base table for which the property should be updated. The property table
 *   is constructed using  a combination of the base table name and the suffix
 *   'prop' (e.g. basetable = feature then property tabie is featureprop).
 * @param $record_id
 *   The foreign key of the basetable to update a property for. This should be in integer.
 *   For example, if the basetable is 'feature' then the $record_id should be the feature_id
 * @param $property
 *   The cvterm name of property to be updated
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 * @param $value
 *   The value of the property to be inserted (can be empty)
 * @param $insert_if_missing
 *   A boolean indicating whether a record should be inserted if one doesn't exist to update
 *
 * Note: The property to be updated is select via the unique combination of $record_id and
 * $property and then it is updated with the supplied value
 *
 * @return
 *   Return True on Update/Insert and False otherwise
 *
 * @ingroup tripal_properties_api
 */
function tripal_core_update_property($basetable, $record_id, $property,
$cv_name, $value, $insert_if_missing = 0) {

  // first see if the property is missing (we can't update a missing property
  $prop = tripal_core_get_property($basetable, $record_id, $property, $cv_name);
  if (count($prop)==0) {
    if ($insert_if_missing) {
      return tripal_core_insert_property($basetable, $record_id, $property, $cv_name, $value);
    }
    else {
      return FALSE;
    }
  }

  // get the foreign key for this property table
  $table_desc = tripal_core_get_chado_table_schema($basetable . 'prop');
  $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

  // construct the array that will match the exact record to update
  $match = array(
    $fkcol => $record_id,
    'type_id' => array(
      'cv_id' => array(
        'name' => $cv_name,
      ),
      'name' => $property,
    ),
  );

  // construct the array of values to be updated
  $values = array(
    'value' => $value,
  );

  return tripal_core_chado_update($basetable . 'prop', $match, $values);
}

/**
 * Update a property for a given base table record.  This function should be
 * used if multiple records of the same property will be present. Also, use this
 * function to change the property name of an existing property.
 *
 * @param $basetable
 *   The base table for which the property should be updated. The property table
 *   is constructed using  a combination of the base table name and the suffix
 *   'prop' (e.g. basetable = feature then property tabie is featureprop).
 * @param $record_id
 *   The primary key of the base table. This should be in integer.
 *   For example, if the basetable is 'feature' then the $record_id should be the featureprop_id
 * @param $property
 *   The cvterm name of property to be updated
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 * @param $value
 *   The value of the property to be inserted (can be empty)
 *
 * @return
 *   Return True on Update/Insert and False otherwise
 *
 * @ingroup tripal_properties_api
 */
function tripal_core_update_property_by_id($basetable, $record_id, $property,
$cv_name, $value) {

  // get the primary key for this property table
  $table_desc = tripal_core_get_chado_table_schema($basetable . 'prop');
  $pkcol = $table_desc['primary key'][0];

  // construct the array that will match the exact record to update
  $match = array(
    $pkcol => $record_id,
  );

  // construct the array of values to be updated
  $values = array(
    'type_id' => array(
      'cv_id' => array(
        'name' => $cv_name,
      ),
      'name' => $property,
    ),
    'value' => $value,
  );

  return tripal_core_chado_update($basetable . 'prop', $match, $values);
}

/**
 * Deletes a property for a given base table record using the property name
 *
 * @param $basetable
 *   The base table for which the property should be deleted. Thus to deleted a property
 *   for a feature the basetable=feature and property is deleted from featureprop
 * @param $record_id
 *   The primary key of the basetable to delete a property for. This should be in integer.
 * @param $property
 *   The cvterm name describing the type of property to be deleted
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 *
 * Note: The property to be deleted is select via the unique combination of $record_id and $property
 *
 * @return
 *   Return True on Delete and False otherwise
 *
 * @ingroup tripal_properties_api
 */
function tripal_core_delete_property($basetable, $record_id, $property, $cv_name) {

  // get the foreign key for this property table
  $table_desc = tripal_core_get_chado_table_schema($basetable . 'prop');
  $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

  // construct the array that will match the exact record to update
  $match = array(
    $fkcol => $record_id,
    'type_id' => array(
      'cv_id' => array(
        'name' => $cv_name,
      ),
      'name' => $property,
    ),
  );

  return tripal_core_chado_delete($basetable . 'prop', $match);
}

/**
 * Deletes a property using the property ID
 *
 * @param $basetable
 *   The base table for which the property should be deleted. Thus to deleted a property
 *   for a feature the basetable=feature and property is deleted from featureprop
 * @param $record_id
 *   The primary key of the basetable to delete a property for. This should be in integer.
 *
 * @return
 *   Return True on Delete and False otherwise
 *
 * @ingroup tripal_properties_api
 */
function tripal_core_delete_property_by_id($basetable, $record_id) {

  // get the foreign key for this property table
  $table_desc = tripal_core_get_chado_table_schema($basetable . 'prop');
  $pkcol = $table_desc['primary key'][0];

  // construct the array that will match the exact record to update
  $match = array(
    $pkcol => $record_id,
  );

  return tripal_core_chado_delete($basetable . 'prop', $match);
}
