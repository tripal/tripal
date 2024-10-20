<?php
/**
 * @file
 * Provides an application programming interface (API) to manage data withing
 *   the Chado database.
 */

/**
 * @defgroup tripal_chado_prop_api Chado Properties
 * @ingroup tripal_api
 * @{
 * The Chado Properties API provides a set of functions for interacting
 * with the any chado prop table.
 * @}
 */

/**
 * Retrieve a property for a given base table record.
 *
 * @param $record
 *   An array used to identify the record to which the property is associated.
 *   The following keys must be used:
 *     -table: The base table for which the property should be updated.
 *         Thus to update a property for a feature the base_table=feature.
 *     -id: The primary key value of the base table. The property will be
 *         associated with the record that matches this id.
 *     -prop_id: The primary key in the [table]prop table.  If this value
 *         is supplied then the 'id' key is not needed.
 * @param $property
 *   An associative array used to specify the property to be selected.  It can
 *   contain the following keys. The keys must be specified to uniquely identify
 *   the term to be applied.  If the options identify more than one CV term
 *   then an error will occur.
 *     -type_name: The cvterm name to be selected.
 *     -type_id: The cvterm_id of the term to be selected.
 *     -cv_id: The cv_id of the CV that contains the term.
 *     -cv_name: The name of the CV that contains the term.
 *     -value: The specific value for the property.
 *     -rank: The specific rank for the property.
 *
 * @return
 *   An array in the same format as that generated by the function
 *   chado_generate_var().  If only one record is returned it
 *   is a single object.  If more than one record is returned then it is an
 *   array of objects
 *
 * @ingroup tripal_chado_prop_api
 */
function chado_get_property($record, $property, $schema_name = 'chado') {

  $base_table = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id = array_key_exists('id', $record) ? $record['id'] : '';
  $prop_id = array_key_exists('prop_id', $record) ? $record['prop_id'] : '';

  $type_name = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value = array_key_exists('value', $property) ? $property['value'] : '';
  $rank = array_key_exists('rank', $property) ? $property['rank'] : 0;


  // Build the values array for checking if the CVterm exists and for
  // retrieving the term as a property.
  $type = [];
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = [
      'name' => $cv_name,
    ];
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }

  // Make sure the CV term exists.
  $options = [];
  $term = chado_select_record('cvterm', ['cvterm_id'], $type, $options, $schema_name);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_get_property: " .
      "Cannot find the term described by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_get_property: " .
      "Multiple terms found. Cannot add the property. Property was described " .
      "by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }

  // Get the foreign key for this property table.
  $table_desc = chado_get_schema($base_table . 'prop', $schema_name);
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // Construct the array of values to be selected.
  $values = [
    'type_id' => $type,
  ];

  //Can supply either base_id or prop_id.
  if ($base_id) {
    $values[$fkcol] = $base_id;
  }

  // If we have the unique property_id make sure to add that to the values.
  if ($prop_id) {
    $property_pkey = $table_desc['primary key'][0];
    $values[$property_pkey] = $prop_id;
  }

  $results = chado_generate_var($base_table . 'prop', $values, [], $schema_name);
  if ($results) {
    $results = chado_expand_var($results, 'field', $base_table . 'prop.value', [], $schema_name);
  }

  return $results;
}

/**
 * Insert a property for a given base table.
 *
 * By default if the property already exists a new property is added with the
 * next available rank.  If the option 'update_if_present' is specified then
 * the record will be updated if it exists rather than adding a new property.
 *
 * @param $record
 *   An associative array used to identify the record to which the property
 *   should be assigned.  The following keys must be used:
 *     -table: The base table for which the property should be inserted.
 *         Thus to insert a property for a feature the base_table=feature and
 *         property is inserted into featureprop.
 *     -id: The primary key value of the base table. The property will be
 *         associated with the record that matches this id.
 * @param $property
 *   An associative array used to specify the property to be added.  It can
 *   contain the following keys. The keys must be specified to uniquely identify
 *   the term to be applied.  If the options identify more than one CV term
 *   then an error will occur.
 *     -type_name: The cvterm name to be selected.
 *     -type_id: The cvterm_id of the term to be selected.
 *     -cv_id: The cv_id of the CV that contains the term.
 *     -cv_name: The name of the CV that contains the term.
 *     -value: The specific value for the property.
 *     -rank: The specific rank for the property.
 * @param $options
 *   An associative array containing the following keys:
 *     -update_if_present:  A boolean indicating whether an existing record
 *        should be updated. If the property already exists and this value is
 *        not specified or is zero then a new property will be added with the
 *        next largest rank.
 *     -force_rank:  If the specified rank is already used by another property
 *        recrod for the same base_id, then set force_rank to TRUE to require
 *        that only the specified rank can be used. Otherwise, the next
 *        available rank will be used.  If 'update_if_present' is FALSE and
 *        'force_rank' is set then an error will occur.
 *
 * @return
 *   Return TRUE if successful and FALSE otherwise.
 *
 * @ingroup tripal_chado_prop_api
 */
function chado_insert_property($record, $property, $options = [], $schema_name = 'chado') {

  $base_table = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id = array_key_exists('id', $record) ? $record['id'] : '';

  $type_name = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value = array_key_exists('value', $property) ? $property['value'] : '';
  $rank = array_key_exists('rank', $property) ? $property['rank'] : 0;
  $cvalue_id = array_key_exists('cvalue_id', $property) ? $property['cvalue_id'] : '';

  $update_if_present = array_key_exists('update_if_present', $options) ? $options['update_if_present'] : FALSE;
  $force_rank = array_key_exists('force_rank', $options) ? $options['force_rank'] : FALSE;

  // First see if the property is already assigned to the record. I
  $props = chado_get_property($record, $property, $schema_name);
  if (!is_array($props)) {
    if ($props) {
      $props = [$props];
    }
    else {
      $props = [];
    }
  }
  if (count($props) > 0) {
    // The property is already assigned, so, see if we should update it.
    if ($update_if_present) {
      return chado_update_property($record, $property, [], $schema_name);
    }
    else {
      if (!$force_rank) {
        // Iterate through the properties returned and check to see if the
        // property with this value already exists if not, get the largest rank
        // and insert the same property but with this new value.
        foreach ($props as $prop) {
          if ($prop->rank > $rank) {
            $rank = $prop->rank;
          }
          if (strcmp($prop->value, $value) == 0) {
            return TRUE;
          }
        }
        // Now add 1 to the rank.
        $rank++;
      }
      else {
        tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_insert_property: " .
          "The property is already assigned to the record with the following " .
          "rank.  And, because the 'force_rank' option is used, the property " .
          "cannot be added: %property.",
          ['%property' => print_r($property, TRUE)]);
        return FALSE;
      }
    }
  }

  // Build the values array for checking if the CVterm exists and for
  // inserting the term as a property.
  $values = [];
  if ($cv_id) {
    $values['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $values['cv_id'] = [
      'name' => $cv_name,
    ];
  }
  if ($type_name) {
    $values['name'] = $type_name;
  }
  if ($type_id) {
    $values['cvterm_id'] = $type_id;
  }

  // Make sure the CV term exists.
  $options = [];
  $term = chado_select_record('cvterm', ['cvterm_id'], $values, $options, $schema_name);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_insert_property: " .
      "Cannot find the term described by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_insert_property: " .
      "Multiple terms found. Cannot add the property. Property was described " .
      "by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }


  // Check that the cvalue property exists.
  if ($cvalue_id) {
    $term = chado_select_record('cvterm', ['cvterm_id'], ['cvterm_id' => $cvalue_id], $options, $schema_name);
    if (!$term or count($term) == 0) {
      tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_insert_property: " .
        "Cannot find the term for the property value described by: %property.",
        ['%property' => print_r($property, TRUE)]);
      return FALSE;
    }
    $values['cvalue'] = $cvalue_id;
  }


  // Get the foreign key for this property table.
  $table_desc = chado_get_schema($base_table . 'prop', $schema_name);
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // Add the property to the record.
  $values = [
    $fkcol => $base_id,
    'type_id' => $values,
    'value' => $value,
    'rank' => $rank,
  ];
  $result = chado_insert_record($base_table . 'prop', $values, [], $schema_name);
  return $result;
}

/**
 * Update a property for a given base table record and property name.
 *
 * @param $record
 *   An associative array used to identify the record to which the property
 *   should be updated.  The following keys must be used:
 *     -table: The base table for which the property should be updated.
 *         Thus to update a property for a feature the base_table=feature.
 *     -id: The primary key value of the base table. The property will be
 *         associated with the record that matches this id.
 *     -prop_id: The primary key in the [table]prop table.  If this value
 *         is supplied then the 'table' and 'id' keys are not needed.
 * @param $property
 *   An associative array used to specify the property to be updated.  It can
 *   contain the following keys. The keys must be specified to uniquely identify
 *   the term to be applied.  If the options identify more than one CV term
 *   then an error will occur.
 *     -type_name: The cvterm name to be selected.
 *     -type_id: The cvterm_id of the term to be selected.
 *     -cv_id: The cv_id of the CV that contains the term.
 *     -cv_name: The name of the CV that contains the term.
 *     -value: The specific value for the property.
 *     -rank: The specific rank for the property.
 *     -cvalue_id: The cvterm_id of the value for the property.
 *      **note** cvalue_id is an anticipated column in the next Chado
 *      release (1.4).  It is included here for early adopters.
 *
 * @param $options
 *   An associative array containing the following keys:
 *     -insert_if_missing: A boolean indicating whether a record should be
 *         inserted if one doesn't exist to update.
 *
 *
 * @return
 *   Return TRUE on Update/Insert and FALSE otherwise.
 *
 * @ingroup tripal_chado_prop_api
 */
function chado_update_property($record, $property, $options = [], $schema_name = 'chado') {

  $base_table = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id = array_key_exists('id', $record) ? $record['id'] : '';
  $prop_id = array_key_exists('prop_id', $record) ? $record['prop_id'] : '';

  $type_name = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value = array_key_exists('value', $property) ? $property['value'] : '';
  $rank = array_key_exists('rank', $property) ? $property['rank'] : 0;
  $cvalue_id = array_key_exists('cvalue_id', $property) ? $property['cvalue_id'] : '';

  $insert_if_missing = array_key_exists('insert_if_missing', $options) ? $options['insert_if_missing'] : FALSE;


  // First see if the property is missing (we can't update a missing property.
  $prop = chado_get_property($record, $property, $schema_name);

  if (empty($prop)) {
    if ($insert_if_missing) {
      return chado_insert_property($record, $property, [], $schema_name);
    }
    else {
      return FALSE;
    }
  }


  // Build the values array for checking if the CVterm exists.
  $type = [];
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = [
      'name' => $cv_name,
    ];
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }


  // Make sure the CV term exists.
  $options = [];
  $term = chado_select_record('cvterm', ['cvterm_id'], $type, $options, $schema_name);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_update_property: " .
      "Cannot find the term described by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_update_property: " .
      "Multiple terms found. Cannot add the property. Property was described " .
      "by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }


  // Get the foreign key for this property table.
  $table_desc = chado_get_schema($base_table . 'prop', $schema_name);
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // Construct the array that will match the exact record to update.
  $match = [
    $fkcol => $base_id,
    'type_id' => $type,
  ];
  // If we have the unique property_id, make sure to use it in the match to
  // ensure we get the exact record. Doesn't rely on there only being one
  // property of that type.
  if ($prop_id) {
    $property_pkey = $table_desc['primary key'][0];
    $match = [
      $property_pkey => $prop_id,
    ];
  }

  // Construct the array of values to be updated.
  $values = [];
  $values['value'] = $value;
  if ($rank) {
    $values['rank'] = $rank;
  }

  // If a cvalue_id is supplied, check that it is a valid cvterm.
  if ($cvalue_id) {
    $term = chado_select_record('cvterm', ['cvterm_id'], ['cvterm_id' => $cvalue_id], $options, $schema_name);
    if (!$term or count($term) == 0) {
      tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_insert_property: " .
        "Cannot find the term for the property value described by: %property.",
        ['%property' => print_r($property, TRUE)]);
      return FALSE;
    }
    $values['cvalue_id'] = $cvalue_id;
  }

  // If we have the unique property_id then we can also update the type
  // thus add it to the values to be updated.
  if ($prop_id) {
    $values['type_id'] = $type;
  }

  return chado_update_record($base_table . 'prop', $match, $values, NULL, $schema_name);
}

/**
 * Deletes a property for a given base table record using the property name.
 *
 * @param $record
 *   An associative array used to identify the record to which the property
 *   should be deleted.  The following keys must be used:
 *     -table: The base table for which the property should be deleted.
 *         Thus to update a property for a feature the base_table=feature.
 *     -id: The primary key value of the base table. The property will be
 *         deleted from the record that matches this id.
 *     -prop_id: The primary key in the [table]prop table to be deleted.  If
 *         this value is supplied then the  'id' key is not needed.
 * @param $property
 *   An associative array used to specify the property to be updated.  It can
 *   contain the following keys. The keys must be specified to uniquely identify
 *   the term to be applied.  If the options identify more than one CV term
 *   then an error will occur.
 *     -type_name: The cvterm name to be selected.
 *     -type_id: The cvterm_id of the term to be selected.
 *     -cv_id: The cv_id of the CV that contains the term.
 *     -cv_name: The name of the CV that contains the term.
 *     -value: The specific value for the property.
 *     -rank: The specific rank for the property.
 *
 * @return
 *   Return TRUE on successful deletion and FALSE otherwise.
 *
 * @ingroup tripal_chado_prop_api
 */
function chado_delete_property($record, $property, $schema_name = 'chado') {

  $base_table = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id = array_key_exists('id', $record) ? $record['id'] : '';
  $prop_id = array_key_exists('prop_id', $record) ? $record['prop_id'] : '';

  $type_name = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value = array_key_exists('value', $property) ? $property['value'] : '';
  $rank = array_key_exists('rank', $property) ? $property['rank'] : 0;


  // Build the values array for checking if the CVterm exists.
  $type = [];
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = [
      'name' => $cv_name,
    ];
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }

  // Make sure the CV term exists.
  $options = [];
  $term = chado_select_record('cvterm', ['cvterm_id'], $type, $options, $schema_name);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_delete_property: " .
      "Cannot find the term described by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_delete_property: " .
      "Multiple terms found. Cannot add the property. Property was described " .
      "by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }

  // Get the foreign key for this property table.
  $table_desc = chado_get_schema($base_table . 'prop', $schema_name);
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // If we have the unique property_id, make sure to use it in the match to 
  // ensure we get the exact record. Doesn't rely on there only being one 
  // property of that type.
  if ($prop_id) {
    $property_pkey = $table_desc['primary key'][0];
    $match = [
      $property_pkey => $prop_id,
    ];
  }
  // Construct the array that will match the exact record to update.
  else {
    $match = [
      $fkcol => $base_id,
      'type_id' => $type,
    ];
  }
  return chado_delete_record($base_table . 'prop', $match, NULL, $schema_name);
}

/**
 * Get all records in the base table assigned one or more properties.
 *
 * The property or properties of interest are specified using the $property
 * argument.
 *
 * @param $record
 *   An associative array used to identify the table and subset of records to
 *   to be searched:
 *     -table: The base table for which the property should be updated.
 *         Thus to update a property for a feature the base_table=feature.
 *     -base_records: An array in the format accepted by the chado_select_record
 *         for specifying a subset of records in the base table.
 * @param $property
 *   An associative array used to specify the property to be selected for. It
 *   can contain the following keys. The keys must be specified to uniquely
 *   identify the term to be searched.  If the options identify more than one
 *   CV term then an error will occur.
 *     -type_name: The cvterm name to be selected.
 *     -type_id: The cvterm_id of the term to be selected.
 *     -cv_id: The cv_id of the CV that contains the term.
 *     -cv_name: The name of the CV that contains the term.
 *     -value: The specific value for the property.
 *     -rank: The specific rank for the property.
 *     -cvalue_id: The cvterm_id of the value for the property.
 *      **note** cvalue_id is an anticipated column in the next Chado
 *      release (1.4).  It is included here for early adopters.
 *
 * @param $options
 *   An array of options supported by chado_generate_var(). These keys
 *   are used for generating the cvterm objects returned by this function.
 *
 * @return
 *   An array of chado variables with the given property.
 *
 * @ingroup tripal_chado_prop_api
 */
function chado_get_record_with_property($record, $property, $options = [], $schema_name = 'chado') {

  $base_table = array_key_exists('table', $record) ? $record['table'] : '';
  $base_records = array_key_exists('base_records', $record) ? $record['base_records'] : [];

  $type_name = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value = array_key_exists('value', $property) ? $property['value'] : '';
  $rank = array_key_exists('rank', $property) ? $property['rank'] : '';

  $property_table = $base_table . 'prop';
  $foreignkey_name = $base_table . '_id';

  // Build the values array for checking if the CVterm exists and for
  // inserting the term as a property.
  $type = [];
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = [
      'name' => $cv_name,
    ];
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }

  // Make sure the CV term exists;
  $term = chado_select_record('cvterm', ['cvterm_id'], $type, NULL, $schema_name);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_update_property: " .
      "Cannot find the term described by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "chado_update_property: " .
      "Multiple terms found. Cannot add the property. Property was described " .
      "by: %property.",
      ['%property' => print_r($property, TRUE)]);
    return FALSE;
  }

  // Build the array for identifying the property.
  $values = [];
  $values['type_id'] = $type;
  if ($rank) {
    $values['rank'] = $rank;
  }
  if ($value) {
    $values['value'] = $value;
  }

  // Add the base records details to the values array.
  if (!empty($base_records)) {
    $values[$foreignkey_name] = $base_records;
  }

  // Now select the ids of the base table that have the properties we want that 
  // match.
  $select = chado_select_record($property_table, [$foreignkey_name], $values, NULL, $schema_name);

  // For each of these ids, pull out the full base records.
  $records = [];
  foreach ($select as $s) {
    $id = $s->{$foreignkey_name};
    $values = [$foreignkey_name => $id];
    $records[$id] = chado_generate_var($base_table, $values, $options, $schema_name);
  }

  return $records;
}


/**
 * Retrieves all of the property types currently availalbe in a prop table.
 *
 * @param $prop_table
 *   The name of the property table.
 *
 * @throws Exception
 *
 * @return
 *   An array of cvterm objects as created by chado_generate_var().
 *
 * @ingroup tripal_chado_prop_api
 */
function chado_get_table_property_types($prop_table, $schema_name = 'chado') {

  // Make sure this is a prop table.
  if (!preg_match('/prop$/', $prop_table)) {
    throw new Exception('Please provide a valid Chado property table');
  }
  $sql = 'SELECT DISTINCT type_id FROM {' . $prop_table . '}';
  $results = chado_query($sql, [], [], $schema_name);
  $types = [];
  foreach ($results as $result) {
    $types[] = chado_generate_var('cvterm', ['cvterm_id' => $result->type_id], [], $schema_name);
  }
  return $types;
}
