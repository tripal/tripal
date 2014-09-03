<?php
/**
 * @file
 * Provides an application programming interface (API) to manage data withing the Chado database.
 */

require_once 'tripal_core.schema_v1.2.api.inc';
require_once 'tripal_core.schema_v1.11.api.inc';

/**
 * @defgroup tripal_chado_api Chado API
 * @ingroup tripal_core_api
 * @{
 * Provides an application programming interface (API) to manage data withing the Chado database.
 *
 * This includes functions for selecting, inserting, updating and deleting records
 * in Chado tables.  The functions will ensure proper integrity contraints are met
 * for inserts and updates.
 *
 * Also, a set of functions is provided for creating template variables.  First,
 * is the chado_generate_var which is used to select one ore more
 * records from a table and return an array with foreign key relationships fully
 * populated.  For example, if selecting a feature, the organism_id and type_id
 * would be present in the returned array as a nested array with their respective
 * foreign keys also nested.  The only fields that are not included are text
 * fields (which may be very large) or many-to-many foreign key relationships.
 * However, these fields and relationships can be expanded using the
 * chado_expand_var.
 *
 * When a row from a chado table is selected using these two functions, it provides
 * a way for users who want to cutomize Drupal template files to access all data
 * associate with a specific record.
 *
 * Finally, the property tables in Chado generally follow the same format.  Therefore
 * there is a set of functions for inserting, updating and deleting properties for
 * any table.  This provides quick lookup of properties (provided the CV term is
 * known).
 *
 * @}
 *
 */

/**
 * Set the Tripal Database
 *
 * The chado_set_active function is used to prevent namespace collisions
 * when chado and drupal are installed in the same database but in different
 * schemas.  It is also used for backwards compatibility with older versions
 * of tripal or in cases where chado is located outside of the Drupal database.
 * or when using Drupal functions such as db_table_exists()
 *
 * @ingroup tripal_chado_api
 */
function chado_set_active($dbname  = 'default') {
  global $databases, $active_db;

  if ($dbname ) {
    if ($dbname == 'chado') {
      db_query('set search_path to chado,public');
      return 'default';
    }
    else {
      db_query('set search_path to public');
      return 'chado';
    }
  }
  // if the 'chado' database is in the $db_url variable then chado is
  // not in the same Drupal database, so we don't need to set any
  // search_path and can just change the database
  elseif (array_key_exists($dbname, $databases)) {
    return db_set_active($dbname);
  }
}

/**
 * Get max rank for a given set of criteria
 * This function was developed with the many property tables in chado in mind but will
 * work for any table with a rank
 *
 * @params tablename: the name of the chado table you want to select the max rank from
 *    this table must contain a rank column of type integer
 * @params where_options: array(
 *                          <column_name> => array(
 *                            'type' => <type of column: INT/STRING>,
 *                            'value' => <the value you want to filter on>,
 *                            'exact' => <if TRUE use =; if FALSE use ~>,
 *                          )
 *        )
 *     where options should include the id and type for that table to correctly
 *     group a set of records together where the only difference are the value and rank
 * @return the maximum rank
 *
 * @ingroup tripal_chado_api
 */
function chado_get_table_max_rank($tablename, $where_options) {

  $where_clauses = array();
  $where_args = array();

  //generate the where clause from supplied options
  // the key is the column name
  $i = 0;
  $sql = "
    SELECT max(rank) as max_rank, count(rank) as count
    FROM {".$tablename."}
    WHERE
  ";
  foreach ($where_options as $key => $value) {
    $where_clauses[] = "$key = :$key";
    $where_args[":$key"] = $value;
  }
  $sql .= implode($where_clauses, ' AND ');

  $result = chado_query($sql, $where_args)->fetchObject();
  if ($result->count > 0) {
    return $result->max_rank;
  }
  else {
    return -1;
  }

}

/**
 * Retrieve a property for a given base table record.
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
 * @return
 *   An array in the same format as that generated by the function
 *   chado_generate_var().  If only one record is returned it
 *   is a single object.  If more than one record is returned then it is an array
 *   of objects
 *
 * @ingroup tripal_chado_api
 */
function chado_get_property($record, $property) {
  
  $base_table  = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id     = array_key_exists('id', $record) ? $record['id'] : '';
  $prop_id     = array_key_exists('prop_id', $record) ? $record['prop_id'] : '';
  
  $type_name   = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id     = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name     = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id       = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value       = array_key_exists('value', $property) ? $property['value'] : '';
  $rank        = array_key_exists('rank', $property) ? $property['rank'] : 0;

  
  // Build the values array for checking if the CVterm exists and for
  // inserting the term as a property.
  $type = array();
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = array(
      'name' => $cv_name,
    );
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }
  
  // Make sure the CV term exists;
  $options = array();
  $term = chado_select_record('cvterm', array('cvterm_id'), $type, $options);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_get_property: " .
        "Cannot find the term described by: %property.",
        array('%property' => print_r($property, TRUE)));
        return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_get_property: " .
        "Multiple terms found. Cannot add the property. Property was described " .
        "by: %property.",
        array('%property' => print_r($property, TRUE)));    return FALSE;
  }
  
  // get the foreign key for this property table
  $table_desc = chado_get_schema($base_table . 'prop');
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // construct the array of values to be selected
  $values = array(
    $fkcol => $base_id,
    'type_id' => $type,
  );
  
  // if we have the unique property_id make sure to add that to the values
  if ($prop_id) {
    $property_pkey = $table_desc['primary key'][0];
    $values[$property_pkey] = $prop_id;
  }
  $results = chado_generate_var($base_table . 'prop', $values);
  if ($results) {
    $results = chado_expand_var($results, 'field', $base_table . 'prop.value');
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
 *         property is inserted into featureprop
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
 *   Return TRUE if successful and FALSE otherwise
 *
 * @ingroup tripal_chado_api
 */
function chado_insert_property($record, $property, $options = array()) {

  $base_table  = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id     = array_key_exists('id', $record) ? $record['id'] : '';

  $type_name   = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id     = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name     = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id       = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value       = array_key_exists('value', $property) ? $property['value'] : '';
  $rank        = array_key_exists('rank', $property) ? $property['rank'] : 0;

  $update_if_present = array_key_exists('update_if_present', $options) ? $options['update_if_present'] : FALSE;
  $force_rank = array_key_exists('force_rank', $options) ? $options['force_rank'] : FALSE;

  // First see if the property is already assigned to the record. I
  $props = chado_get_property($record, $property);
  if (!is_array($props) and $props) {
    $props = array($props);
  }
  if (count($props) > 0) {
    // The property is already assigned, so, see if we should update it.
    if ($update_if_present) {
      return chado_update_property($record, $property);
    }
    else {
      if (!$force_rank) {
        // iterate through the properties returned and check to see if the
        // property with this value already exists if not, get the largest rank
        // and insert the same property but with this new value
        foreach ($props as $prop) {
          if ($prop->rank > $rank) {
            $rank = $prop->rank;
          }
          if (strcmp($prop->value, $value) == 0) {
            return TRUE;
          }
        }
        // now add 1 to the rank
        $rank++;
      }
      else {
        tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_insert_property: " . 
         "The property is already assigned to the record with the following " .
         "rank.  And, because the 'force_rank' option is used, the property " .
         "cannot be added: %property.",
          array('%property' => print_r($property, true)));
        return FALSE;
      }
    }
  }

  // Build the values array for checking if the CVterm exists and for
  // inserting the term as a property. 
  $values = array();
  if ($cv_id) {
    $values['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $values['cv_id'] = array(
      'name' => $cv_name,
    );
  }
  if ($type_name) {
    $values['name'] = $type_name;
  }
  if ($type_id) {
    $values['cvterm_id'] = $type_id;
  }

  // Make sure the CV term exists;
  $options = array();
  $term = chado_select_record('cvterm', array('cvterm_id'), $values, $options);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_insert_property: " .
      "Cannot find the term described by: %property.",
      array('%property' => print_r($property, TRUE)));
    return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_insert_property: " .
      "Multiple terms found. Cannot add the property. Property was described " .
      "by: %property.",
      array('%property' => print_r($property, TRUE)));    return FALSE;
  }
  
  // Get the foreign key for this property table
  $table_desc = chado_get_schema($base_table . 'prop');
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // Add the property to the record.
  $values = array(
    $fkcol => $base_id,
    'type_id' => $values,
    'value' => $value,
    'rank' => $rank,
  );
  $result = chado_insert_record($base_table . 'prop', $values);
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
 * @param $options
 *   An associative array containing the following keys:
 *     -insert_if_missing: A boolean indicating whether a record should be 
 *         inserted if one doesn't exist to update.
 *
 *
 * @return
 *   Return TRUE on Update/Insert and FALSE otherwise
 *
 * @ingroup tripal_chado_api
 */
function chado_update_property($record, $property, $options) {

  $base_table  = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id     = array_key_exists('id', $record) ? $record['id'] : '';
  $prop_id     = array_key_exists('prop_id', $record) ? $record['prop_id'] : '';
  
  $type_name   = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id     = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name     = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id       = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value       = array_key_exists('value', $property) ? $property['value'] : '';
  $rank        = array_key_exists('rank', $property) ? $property['rank'] : 0;

  $insert_if_missing = array_key_exists('insert_if_missing', $options) ? $options['insert_if_missing'] : FALSE;

  // first see if the property is missing (we can't update a missing property
  $prop = chado_get_property($record, $property);
  if (count($prop) == 0) {
    if ($insert_if_missing) {
      return chado_insert_property($record, $property);
    }
    else {
      return FALSE;
    }
  }
  
  // Build the values array for checking if the CVterm exists.
  $type = array();
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = array(
      'name' => $cv_name,
    );
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }
  
  // Make sure the CV term exists;
  $options = array();
  $term = chado_select_record('cvterm', array('cvterm_id'), $type, $options);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_update_property: " .
        "Cannot find the term described by: %property.",
        array('%property' => print_r($property, TRUE)));
    return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_update_property: " .
        "Multiple terms found. Cannot add the property. Property was described " .
        "by: %property.",
        array('%property' => print_r($property, TRUE)));    
    return FALSE;
  }
  

  // Get the foreign key for this property table
  $table_desc = chado_get_schema($base_table . 'prop');
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // construct the array that will match the exact record to update
  $match = array(
    $fkcol => $base_id,
    'type_id' => $type,
  );
  // If we have the unique property_id, make sure to use it in the match to 
  // ensure we get the exact record. Doesn't rely on there only being one 
  // property of that type.
  if ($prop_id) {
    $property_pkey = $table_desc['primary key'][0];
    $match = array(
      $property_pkey => $prop_id,
    );
  }

  // Construct the array of values to be updated.
  $values = array();
  $values['value'] = $value;
  if ($rank) {
    $values['rank'] = $rank;
  }
  
  // If we have the unique property_id then we can also update the type
  // thus add it to the values to be updated
  if ($prop_id) {
    $values['type_id'] = $type;
  }

  return chado_update_record($base_table . 'prop', $match, $values);
}

/**
 * Deletes a property for a given base table record using the property name
 * 
 * @param $record
 *   An associative array used to identify the record to which the property
 *   should be deleted.  The following keys must be used:
 *     -table: The base table for which the property should be deleted. 
 *         Thus to update a property for a feature the base_table=feature.
 *     -id: The primary key value of the base table. The property will be
 *         deleted from the record that matches this id.
 *     -prop_id: The primary key in the [table]prop table to be deleted.  If 
 *         this value is supplied then the 'table' and 'id' keys are not needed.
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
 *   Return TRUE on successful deletion and FALSE otherwise
 *
 * @ingroup tripal_chado_api
 */
function chado_delete_property($record, $property) {

  $base_table  = array_key_exists('table', $record) ? $record['table'] : '';
  $base_id     = array_key_exists('id', $record) ? $record['id'] : '';
  $prop_id     = array_key_exists('prop_id', $record) ? $record['prop_id'] : '';
  
  $type_name   = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id     = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name     = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id       = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value       = array_key_exists('value', $property) ? $property['value'] : '';
  $rank        = array_key_exists('rank', $property) ? $property['rank'] : 0;
  
  
  // Build the values array for checking if the CVterm exists
  $type = array();
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = array(
      'name' => $cv_name,
    );
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }
  
  // Make sure the CV term exists;
  $options = array();
  $term = chado_select_record('cvterm', array('cvterm_id'), $type, $options);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_delete_property: " .
        "Cannot find the term described by: %property.",
        array('%property' => print_r($property, TRUE)));
        return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_delete_property: " .
        "Multiple terms found. Cannot add the property. Property was described " .
        "by: %property.",
        array('%property' => print_r($property, TRUE)));    return FALSE;
  }
  
  // get the foreign key for this property table
  $table_desc = chado_get_schema($base_table . 'prop');
  $fkcol = key($table_desc['foreign keys'][$base_table]['columns']);

  // If we have the unique property_id, make sure to use it in the match to ensure
  // we get the exact record. Doesn't rely on there only being one property of that type
  if ($prop_id) {
    $property_pkey = $table_desc['primary key'][0];
    $match = array(
      $property_pkey => $prop_id
    );
  }
  // construct the array that will match the exact record to update
  else {
    $match = array(
      $fkcol => $record_id,
      'type_id' => $type,
    );
  }
  return chado_delete_record($base_table . 'prop', $match);
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
 * @param $options
 *   An array of options supported by chado_generate_var(). These keys
 *   are used for generating the cvterm objects returned by this function.
 *
 * @return
 *   An array of chado variables with the given property
 *
 * @ingroup tripal_chado_api
 */
function chado_get_record_with_property($record, $property, $options = array()) {
  
  $base_table  = array_key_exists('table', $record) ? $record['table'] : '';
  $base_records= array_key_exists('base_records', $record) ? $record['base_records'] : array();
  
  $type_name   = array_key_exists('type_name', $property) ? $property['type_name'] : '';
  $type_id     = array_key_exists('type_id', $property) ? $property['type_id'] : '';
  $cv_name     = array_key_exists('cv_name', $property) ? $property['cv_name'] : '';
  $cv_id       = array_key_exists('cv_id', $property) ? $property['cv_id'] : '';
  $value       = array_key_exists('value', $property) ? $property['value'] : '';
  $rank        = array_key_exists('rank', $property) ? $property['rank'] : '';

  $property_table = $base_table . 'prop';
  $foreignkey_name = $base_table . '_id';

  // Build the values array for checking if the CVterm exists and for
  // inserting the term as a property.
  $type = array();
  if ($cv_id) {
    $type['cv_id'] = $cv_id;
  }
  if ($cv_name) {
    $type['cv_id'] = array(
      'name' => $cv_name,
    );
  }
  if ($type_name) {
    $type['name'] = $type_name;
  }
  if ($type_id) {
    $type['cvterm_id'] = $type_id;
  }
  
  // Make sure the CV term exists;
  $term = chado_select_record('cvterm', array('cvterm_id'), $type);
  if (!$term or count($term) == 0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_update_property: " .
        "Cannot find the term described by: %property.",
        array('%property' => print_r($property, TRUE)));
        return FALSE;
  }
  if (count($term) > 1) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, "chado_update_property: " .
        "Multiple terms found. Cannot add the property. Property was described " .
        "by: %property.",
        array('%property' => print_r($property, TRUE)));    return FALSE;
  }

  // Build the array for identifying the property.
  $values = array();
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

  // Now select the ids of the base table that have the properties we want that match.
  $select = chado_select_record($property_table, array($foreignkey_name), $values);

  // For each of these ids, pull out the full base records
  $records = array();
  foreach ($select as $s) {
    $id = $s->{$foreignkey_name};
    $values = array($foreignkey_name => $id);
    $records[$id] = chado_generate_var($base_table, $values, $options);
  }

  return $records;
}