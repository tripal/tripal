<?php

/**
 * @file
 * API to manage the chado prop table for various Tripal Node Types
 *
 * How To Use:
 * @code

  function chado_example_form($form, $form_state) {

    // When setting the defaults for your form make sure to take into account
    // that when ajax is fired via a button (which it is for the properties api)
    // you're user entered data is in $form_state['input'] instead of $form_state['values']
    // although it has not been validated. Thus make sure you handle values in $form_state['input']
    // or user input may be loast on addition of properties

    // All the form array definition particular to your node type

    // Add the properties form to your current form
    // get the chado_example properties to be used in the dropdown
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
    $ontology_name = 'nameofproptypeontology';  //you need to set this variable with the cv.name of the ontology governing your prop tables type_id
    $prop_types = chado_query($sql, array(':ontology_name' => $ontology_name));
    while ($prop = $prop_types->fetchObject()) {
      $properties[$prop->cvterm_id] = $prop->name;
    }

    $exclude = array();
    $include = array();
    $instructions = t('To add additional properties to the drop down. ' . l("Add terms to the $ontology_name vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");

    // actually create and add the form
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

  function nodetype_insert($node) {

    // if there is an example_id in the $node object then this must be a sync so
    // we can skip adding the chado_example as it is already there, although
    // we do need to proceed with the rest of the insert
    if (!property_exists($node, 'example_id')) {

      // Add record to chado example table

      // Add to any other tables needed

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

    // Add record to chado_example linking example_id to new node

  }

  function nodetype_update($node) {


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

  $d_removed = array(); // lists removed properties

  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $d_removed = $form_state['values']['removed'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $d_removed = $form_state['input']['removed'];
  }

  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t($fset_title),
    '#description' => t('You may add additional properties by
      selecting a property type from the dropdown and adding text.  You may add
      as many properties as desired by clicking the add button on the right.  To
      remove a property, click the remove button. ' . $instructions),
  );
  $form['properties']['table'] = array(
    '#type' => 'markup',
    '#value' =>  '',
    '#prefix' => '<div id="tripal-generic-edit-properties-table">',
    '#suffix' => '</div>',
  );

  // this array keeps track of all properties we have and allows the functions
  // below to select the next rank if a property is dupliated
  $ranks = array();

  // add in the properties from the Chado prop table (only pertains to existing analyses)
  if ($id) {
    tripal_core_properties_form_add_prop_table_props($prop_table, $id_field, $cv_name,
      $form, $form_state, $id, $ranks, $d_removed, $exclude, $include);
  }

  // add in any new properties that have been added by the user through an AHAH callback
  tripal_core_properties_form_add_new_props($form, $form_state, $ranks, $d_removed);

  // add an empty row of field to allow for addition of a new property
  tripal_core_properties_form_add_new_empty_props($form, $form_state, $available_props);

  $form['properties']['table']['#theme'] = 'tripal_core_properties_form';
}

/**
 * This function is responsible for adding a blank row to the properties table for
 * adding a new property.
 */
function  tripal_core_properties_form_add_new_empty_props(&$form, &$form_state, $properties_select) {

  // get the field defaults either from $form_state['values'] or $form_state['input']
  $description = '';
  $text = '';
  $id = 0;
  if (array_key_exists('values', $form_state)) {
    $id = $form_state['values']['new_id'];
    $text = $form_state['values']['new_value'];
  }
  // if we have a property ID then get it's definition to display to the user
  if($id) {
    $values = array('cvterm_id' => $id);
    $cvterm = tripal_core_chado_select('cvterm', array('definition'), $values);

    if ($cvterm[0]->definition) {
      $description = $cvterm[0]->definition;
    }
  }
  $rows = 1;

  // add one more blank set of property fields
  $form['properties']['table']['new']["new_id"] = array(
    '#type'          => 'select',
    '#options'       => $properties_select,
    '#default_value' => $id,
    '#ajax' => array(
      'callback' => "tripal_core_props_property_ajax_get_description",
      'wrapper'  => 'tripal-properties-new_value',
      'effect'   => 'fade',
      'method'   => 'replace',
    ),
  );
  $form['properties']['table']['new']["new_value"] = array(
    '#type'           => 'textarea',
    '#default_value'  => $text,
    '#cols'           => 50,
    '#rows'           => $rows,
    '#prefix'         => '<div id="tripal-properties-new_value">',
    '#description'    => '<span style="white-space: normal">' . $description . '</span>',
    '#suffix'         => '</div>',
  );

  $form['properties']['table']['new']["add"] = array(
    '#type'    => 'button',
    '#value'   => t('Add'),
    '#name'    => 'add',
    '#ajax'      => array(
      'callback' => "tripal_core_props_property_ajax_update",
      'wrapper'  => 'tripal-properties-edit-properties-table',
      'effect'   => 'fade',
      'method'   => 'replace',
      'prevent'  => 'click'
    ),
    // When this button is clicked, the form will be validated and submitted.
    // Therefore, we set custom submit and validate functions to override the
    // default form submit.  In the validate function we set the form_state
    // to rebuild the form so the submit function never actually gets called,
    // but we need it or Drupal will run the default validate anyway.
    // we also set #limit_validation_errors to empty so fields that
    // are required that don't have values won't generate warnings.
    '#submit'   => array('tripal_core_props_form_props_button_submit'),
    '#validate' => array('tripal_core_props_form_props_button_validate'),
    '#limit_validation_errors' => array(array('new_id')),
  );
}

/**
 * This function is used to rebuild the form if an ajax call is made vai a button.
 * The button causes the form to be submitted. We don't want this so we override
 * the validate and submit routines on the form button. Therefore, this function
 * only needs to tell Drupal to rebuild the form
 */
function  tripal_core_props_form_props_button_validate($form, &$form_state){
  if (array_key_exists('triggering_element', $form_state) and
  $form_state['triggering_element']['#name'] == 'add' and
  $form_state['input']['new_id'] == 0 ){
    form_set_error('new_id', "Please specify a property type");
    return;
  }
  $form_state['rebuild'] = TRUE;
}
/**
 * This function is just a dummy to override the default form submit on ajax calls for buttons
 */
function tripal_core_props_form_props_button_submit($form, &$form_state){
  // do nothing
}
/**
 * This adds
 */
function tripal_core_properties_form_add_new_props(&$form, &$form_state, &$ranks, &$d_removed) {

  // set some default values
  $j = 0;
  $num_properties = 0;

  $values = array();
  if (array_key_exists('values', $form_state)) {
    $values = $form_state['values'];
  }
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $values = $form_state['input'];
  }

  // first, add in all of the new properties that were added previously via this form
  foreach ($values as $element_name => $value) {
    if (preg_match('/new_value-(\d+)-(\d+)/', $element_name, $matches)) {
      $new_id = $matches[1];
      $rank = $matches[2];

      // skip any properties that the user requested to delete through a previous
      // ajax callback or through the current ajax callback
      if (array_key_exists("$new_id-$rank", $d_removed)) {
        continue;
      }
      if (array_key_exists('triggering_element', $form_state) and
      $form_state['triggering_element']['#name'] == 'remove-' . $new_id . '-' . $rank) {
        $d_removed["$new_id-$rank"] = 1;
        continue;
      }

      // get this new_id information
      $args = array('cvterm_id' => $new_id);
      $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), $args);

      // add it to the $ranks array
      $ranks[$new_id][$rank]['name']  = $cvterm[0]->name;
      $ranks[$new_id][$rank]['id']    = $new_id;
      $ranks[$new_id][$rank]['value'] = $value;
      $ranks[$new_id][$rank]['definition']  = $cvterm[0]->definition;
      $num_properties++;

      // determine how many rows we need in the textarea
      $rows = 1;
      $rows = strlen($value) / 80 + 1;
      if ($rows > 10) {
        $rows = 10;
      }

      // add the new fields
      $form['properties']['table']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
        '#markup' => $cvterm[0]->name
      );
      $form['properties']['table']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
        '#type'          => 'textarea',
        '#default_value' => $value,
        '#cols'          => 50,
        '#rows'          => $rows,
        '#description'   => '<span style="white-space: normal">' . $cvterm[0]->definition . '</span>',
      );

      $form['properties']['table']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
        '#type'         => 'button',
        '#value'        => t('Remove'),
        '#name'         => "remove-$new_id-$rank",
        '#ajax' => array(
          'callback' => "tripal_core_props_property_ajax_update",
          'wrapper'  => 'tripal-properties-edit-properties-table',
          'effect'   => 'fade',
          'event'    => 'mousedown',
          'method'   => 'replace',
          'prevent'  => 'click'
        ),
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default form submit.  In the validate function we set the form_state
        // to rebuild the form so the submit function never actually gets called,
        // but we need it or Drupal will run the default validate anyway.
        // we also set #limit_validation_errors to empty so fields that
        // are required that don't have values won't generate warnings.
        '#submit'   => array('tripal_core_props_form_props_button_submit'),
        '#validate' => array('tripal_core_props_form_props_button_validate'),
        '#limit_validation_errors' => array(),
      );
    }
  }

  // second add in any new properties added during this callback
  if (array_key_exists('triggering_element', $form_state) and
  $form_state['triggering_element']['#name'] == 'add' and
  $form_state['input']['new_id'] != 0) {
    $new_id    = $form_state['input']['new_id'];
    $new_value = $form_state['input']['new_value'];

    // get the rank by counting the number of entries
    $rank = count($ranks[$new_id]);

    // get this new_id information
    $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), array('cvterm_id' => $new_id));

    // add it to the $ranks array
    $ranks[$new_id][$rank]['name']  = $cvterm[0]->name;
    $ranks[$new_id][$rank]['id']    = $new_id;
    $ranks[$new_id][$rank]['value'] = $value;
    $ranks[$new_id][$rank]['definition']  = $cvterm[0]->definition;
    $num_properties++;

    // determine how many rows we need in the textarea
    $rows = 1;

    // add the new fields
    $form['properties']['table']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
      '#markup' => $cvterm[0]->name
    );
    $form['properties']['table']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $new_value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => $cvterm[0]->definition,
    );

    $form['properties']['table']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
      '#type'         => 'button',
      '#value'        => t('Remove'),
      '#name'         => "remove-$new_id-$rank",
      '#ajax' => array(
        'callback' => "tripal_core_props_property_ajax_update",
        'wrapper'  => 'tripal-properties-edit-properties-table',
        'effect'   => 'fade',
        'event'    => 'mousedown',
        'method'   => 'replace',
        'prevent'  => 'click'
      ),
      // When this button is clicked, the form will be validated and submitted.
      // Therefore, we set custom submit and validate functions to override the
      // default form submit.  In the validate function we set the form_state
      // to rebuild the form so the submit function never actually gets called,
      // but we need it or Drupal will run the default validate anyway.
      // we also set #limit_validation_errors to empty so fields that
      // are required that don't have values won't generate warnings.
      '#submit'   => array('tripal_core_props_form_props_button_submit'),
      '#validate' => array('tripal_core_props_form_props_button_validate'),
      '#limit_validation_errors' => array(),
    );

  }

  return $num_properties;
}
/**
 * This function queries the proper xxxprop table to look for existing values for the given
 * $id.  It then adds these properties to the form for editing.  It also will incorporate
 * extra properties that were specified manually by the caller.
 *
 */
function  tripal_core_properties_form_add_prop_table_props($prop_table, $id_field, $cv_name,
    &$form, $form_state, $id, &$ranks, &$d_removed, $exclude = array(), $include = array()) {

  // get the existing properties
  $num_properties = 0;

  if (!$id) {
    return;
  }

  // create an array of properties so we can merge those in the database with those provided by the caller
  $all_props = array();
  foreach ($include as $prop) {
    $all_props[] = $prop;
  }

  // now merge in properties saved in the database
  $sql = "
    SELECT CVT.cvterm_id, CVT.name, CVT.definition, PP.value, PP.rank
    FROM {" . $prop_table . "} PP
      INNER JOIN {cvterm} CVT ON CVT.cvterm_id = PP.type_id
      INNER JOIN {cv} CV      ON CVT.cv_id     = CV.cv_id
      WHERE
      PP.$id_field = :id AND
      CV.name = '$cv_name'
      ORDER BY CVT.name, PP.rank
      ";
  $props = chado_query($sql, array(':id' => $id));
  while ($prop = $props->fetchObject()) {
    $all_props[] = array('cvterm' => $prop, 'value' => $prop->value);
  }

  // iterate through the properties
  foreach ($all_props as $prop) {

    $type_id    = $prop['cvterm']->cvterm_id;
    $value      = $prop['value'];
    $name       = $prop['cvterm']->name;
    $definition = $prop['cvterm']->definition;
    $rank       = 0;
    if(array_key_exists($type_id, $ranks)) {
      $rank = count($ranks[$type_id]);
    }

    // skip any properties that the user requested to delete through a previous
    // AHAH callback or through the current AHAH callback
    if (array_key_exists("$type_id-$rank", $d_removed)) {
      continue;
    }
    // skip any properties that should be excluded
    if (count(array_intersect(array($name), $exclude)) == 1) {
      continue;
    }
    if (array_key_exists('triggering_element', $form_state) and
    $form_state['triggering_element']['#name'] == 'remove-' . $type_id . '-' . $rank) {
      $d_removed["$type_id-$rank"] = 1;
      continue;
    }

    $ranks[$type_id][$rank]['name']  = $name;
    $ranks[$type_id][$rank]['id']    = $type_id;
    $ranks[$type_id][$rank]['value'] = $value;
    $num_properties++;
    $rows = 1;
    $rows = strlen($value) / 80 + 1;
    if ($rows > 10) {
      $rows = 10;
    }


    $form['properties']['table'][$type_id][$rank]["prop_id-$type_id-$rank"] = array(
      '#markup'        => $name,
    );
    $form['properties']['table'][$type_id][$rank]["prop_value-$type_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => '<span style="white-space: normal">' . $definition . '</span>',
    );

    $form['properties']['table'][$type_id][$rank]["remove-$type_id-$rank"] = array(
      '#type'         => 'button',
      '#value'        => t('Remove'),
      '#name'         => "remove-$type_id-$rank",
      '#ajax' => array(
        'callback' => "tripal_core_props_property_ajax_update",
        'wrapper'  => 'tripal-properties-edit-properties-table',
        'effect'   => 'fade',
        'event'    => 'mousedown',
        'method'   => 'replace',
        'prevent'  => 'click'
      ),
      // When this button is clicked, the form will be validated and submitted.
      // Therefore, we set custom submit and validate functions to override the
      // default form submit.  In the validate function we set the form_state
      // to rebuild the form so the submit function never actually gets called,
      // but we need it or Drupal will run the default validate anyway.
      // we also set #limit_validation_errors to empty so fields that
      // are required that don't have values won't generate warnings.
      '#submit'   => array('tripal_core_props_form_props_button_submit'),
      '#validate' => array('tripal_core_props_form_props_button_validate'),
      '#limit_validation_errors' => array(),
    );
  }
  return $num_properties;
}

/**
 * Form AJAX callback for adding a blank property row
 *
 * We only want to return the properties as that's all we'll replace with this callback
 */
function tripal_core_props_property_ajax_update($form, $form_state) {
  $properties_html = tripal_core_props_theme_node_form_properties($form['properties']['table']);
  $form['properties']['table'] = array(
    '#markup' => $properties_html,
    '#prefix' => '<div id="tripal-properties-edit-properties-table">',
    '#suffix' => '</div>',
  );
  return $form['properties']['table'];
}
/**
 * Form AJAX callback for updating a property description. This
 * function only gets called when the property drop down is changed
 * on the bottom (empty) row of properties
 */
function tripal_core_props_property_ajax_get_description($form, $form_state) {
  return $form['properties']['table']['new']["new_value"];
}

/**
 * We need to theme the form so that the properties fields look good
 */
function theme_tripal_core_properties_form($variables) {
  $form = $variables['form'];

  $properties_table = tripal_core_props_theme_node_form_properties($form);
  $markup = $properties_table;

  $form['properties']['table'] = array(
    '#markup' => $markup,
    '#prefix' => '<div id="tripal-properties-edit-properties-table">',
    '#suffix' => '</div>',
  );
  $form['buttons']['#weight'] = 50;
  return drupal_render($form['properties']['table']);
}
/**
 *
 */
function tripal_core_props_theme_node_form_properties($form) {
  $rows = array();

  // first add in the properties derived from the prop table
  // the array tree for these properties looks like this:
  // $form['properties']['table'][$type_id][$rank]["prop_id-$type_id-$rank"]
  foreach ($form as $type_id => $elements) {
    // there are other fields in the properties array so we only
    // want the numeric ones those are our type_id
    if (is_numeric($type_id)) {
      foreach ($elements as $rank => $element) {
        if (is_numeric($rank)) {
          $rows[] = array(
            drupal_render($element["prop_id-$type_id-$rank"]),
            drupal_render($element["prop_value-$type_id-$rank"]),
            drupal_render($element["remove-$type_id-$rank"]),
          );
        }
      }
    }
  }

  // second, add in any new properties added by the user through AHAH callbacks
  // the array tree for these properties looks like this:
  // $form['properties']['table']['new'][$type_id][$rank]["new_id-$new_id-$rank"]
  foreach ($form['new'] as $type_id => $elements) {
    if (is_numeric($type_id)) {
      foreach ($elements as $rank => $element) {
        if (is_numeric($rank)) {
          $rows[] = array(
            drupal_render($element["new_id-$type_id-$rank"]),
            drupal_render($element["new_value-$type_id-$rank"]),
            drupal_render($element["remove-$type_id-$rank"]),
          );
        }
      }
    }
  }

  // finally add in a set of blank field for adding a new property
  $rows[] = array(
    drupal_render($form['new']['new_id']),
    array(
      'data' => drupal_render($form['new']['new_value']),
      'width' => '60%',
    ),
    drupal_render($form['new']['add']),
  );

  $headers = array('Property Type', 'Value', 'Actions');
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  return theme_table($table);
}

/**
 * This function is used in a hook_insert, hook_update for a node form
 * when the properties form has been added to the form.  It retrieves all of the properties
 * and returns them in an array of the format:
 *
 *   $properties[<property name>][<rank>] = <value
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

  // now add the properties
  $properties = array(); // stores all of the properties we need to add

  // get the list of properties for easy lookup (without doing lots of database queries
  $properties_list = array();
  $sql = "
      SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition
      FROM  {cvterm} CVT
        INNER JOIN {cv} ON CVT.cv_id = CV.cv_id
      WHERE
        CV.name = '$cv_name' AND
        NOT CVT.is_obsolete = 1
      ORDER BY CVT.name ASC
  ";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    $properties_list[$prop->cvterm_id] = $prop->name;
  }

  // get the properties that should be added. Properties are in one of two forms:
  //  1) prop_value-[type id]-[index]
  //  2) new_value-[type id]-[index]
  //  3) new_id, new_value

  foreach ($node as $name => $value) {
    if (preg_match('/^new_value-(\d+)-(\d+)/', $name, $matches)) {
      $type_id = $matches[1];
      $index = $matches[2];
      $name = $properties_list[$type_id];
      $properties[$name][$index] = trim($value);
    }
    if (preg_match('/^prop_value-(\d+)-(\d+)/', $name, $matches)) {
      $type_id = $matches[1];
      $index = $matches[2];
      $name = $properties_list[$type_id];
      $properties[$name][$index] = trim($value);
    }
  }
  if (property_exists($node, 'new_id') and $node->new_id and property_exists($node, 'new_value') and $node->new_value) {
    $type_id = $node->new_id;
    $name = $properties_list[$type_id];
    $index = 0;
    if (array_key_exists($name, $properties)) {
      $index = count($properties[$name]);
    }
    $properties[$name][$index] = trim($node->new_value);
  }

  return $properties;
}