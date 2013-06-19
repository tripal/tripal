<?php

require_once "tripal_core.schema_v1.2.api.inc";
require_once "tripal_core.schema_v1.11.api.inc";

/**
 * @file
 * The Tripal Core API
 *
 * This file provides the API needed for all other Tripal and Tripal dependent
 * modules.
 *
 *
 * @defgroup tripal_chado_api Chado API
 * @ingroup tripal_core_api
 * @{
 * Provides an application programming interface (API) to manage data withing the Chado database.
 * This includes functions for selecting, inserting, updating and deleting records
 * in Chado tables.  The functions will ensure proper integrity contraints are met
 * for inserts and updates.
 *
 * Also, a set of functions is provided for creating template variables.  First,
 * is the tripal_core_generate_chado_vars which is used to select one ore more
 * records from a table and return an array with foreign key relationships fully
 * populated.  For example, if selecting a feature, the organism_id and type_id
 * would be present in the returned array as a nested array with their respective
 * foreign keys also nested.  The only fields that are not included are text
 * fields (which may be very large) or many-to-many foreign key relationships.
 * However, these fields and relationships can be expanded using the
 * tripal_core_expand_chado_vars.
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
 * Provides a generic routine for inserting into any Chado table
 *
 * Use this function to insert a record into any Chado table.  The first
 * argument specifies the table for inserting and the second is an array
 * of values to be inserted.  The array is mutli-dimensional such that
 * foreign key lookup values can be specified.
 *
 * @param $table
 *  The name of the chado table for inserting
 * @param $values
 *  An associative array containing the values for inserting.
 * @param $options
 *  An array of options such as:
 *  - skip_validation: TRUE or FALSE. If TRUE will skip all the validation steps and
 *     just try to insert as is. This is much faster but results in unhandled
 *     non user-friendly errors if the insert fails.
 *  - return_record: by default, the function will return the record but with
 *     the primary keys added after insertion.  To simply return TRUE on success
 *     set this option to FALSE
 *
 * @return
 *  On success this function returns the inserted record with the new primary keys
 *  added to the returned array. On failure, it returns FALSE.
 *
 * Example usage:
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => 'sinensis',
 *      ),
 *     'name' => 'orange1.1g000034m.g',
 *     'uniquename' => 'orange1.1g000034m.g',
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = tripal_core_chado_insert('feature',$values);
 * @endcode
 * The above code inserts a record into the feature table.  The $values array is
 * nested such that the organism is selected by way of the organism_id foreign
 * key constraint by specifying the genus and species.  The cvterm is also
 * specified using its foreign key and the cv_id for the cvterm is nested as
 * well.
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_chado_insert($table, $values, $options = array()) {

  if (!is_array($values)) {
    watchdog('tripal_core', 'Cannot pass non array as values for inserting.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }
  if (count($values)==0) {
    watchdog('tripal_core', 'Cannot pass an empty array as values for inserting.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }

  // set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements
  if (!is_array($options)) {
    $options = array();
  }

  if (!array_key_exists('skip_validation', $options)) {
    $options['skip_validation'] = FALSE;
  }
  if (!array_key_exists('return_record', $options)) {
    $options['return_record'] = TRUE;
  }

  $insert_values = array();

  if (array_key_exists('skip_validation', $options)) {
    $validate = !$options['skip_validation'];
  }
  else {
    $validate = TRUE;
  }

  // get the table description
  $table_desc = tripal_core_get_chado_table_schema($table);
  if (empty($table_desc)) {
    watchdog('tripal_core', 'tripal_core_chado_insert: There is no table description for !table_name', array('!table_name' => $table), WATCHDOG_WARNING);
  }

  // iterate through the values array and create a new 'insert_values' array
  // that has all the values needed for insert with all foreign relationsihps
  // resolved.
  foreach ($values as $field => $value) {
    // make sure the field is in the table description. If not then return an error
    // message
    if (!array_key_exists($field, $table_desc['fields'])) {
      watchdog('tripal_core', "tripal_core_chado_insert: The field '%field' does not exist " .
        "for the table '%table'.  Cannot perform insert. Values: %array",
        array('%field' => $field, '%table' => $table, '%array' => print_r($values, 1)), WATCHDOG_ERROR);
      return FALSE;
    }

    if (is_array($value)) {
      // select the value from the foreign key relationship for this value
      $results = tripal_core_chado_get_foreign_key($table_desc, $field, $value, $foreign_options);

      if (sizeof($results) > 1) {
        watchdog('tripal_core', 'tripal_core_chado_insert: Too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)), WATCHDOG_ERROR);
      }
      elseif (sizeof($results) < 1) {
        //watchdog('tripal_core', 'tripal_core_chado_insert: no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)), WATCHDOG_ERROR);
      }
      else {
        $insert_values[$field] = $results[0];
      }
    }
    else {
      $insert_values[$field] = $value;
    }
  }

  if ($validate) {

    // check for violation of any unique constraints
    $ukeys = array();
    if (array_key_exists('unique keys', $table_desc)) {
      $ukeys = $table_desc['unique keys'];
    }
    $ukselect_cols = array();
    $ukselect_vals = array();
    if ($ukeys) {
      foreach ($ukeys as $name => $fields) {
        foreach ($fields as $index => $field) {
          // build the arrays for performing a select that will check the contraint
          $ukselect_cols[] = $field;
          if (!array_key_exists($field, $insert_values)) {
            if (array_key_exists('default', $table_desc['fields'][$field])) {
              $ukselect_vals[$field] = $table_desc['fields'][$field]['default'];
            }
          }
          else {
            $ukselect_vals[$field] = $insert_values[$field];
          }
        }
        // now check the constraint        
        if (tripal_core_chado_select($table, $ukselect_cols, $ukselect_vals)) {
          watchdog('tripal_core', "tripal_core_chado_insert: Cannot insert duplicate record into $table table: " .
            print_r($values, 1), array(), 'WATCHDOG_ERROR');
          return FALSE;
        }
      }
    }

    // if trying to insert a field that is the primary key, make sure it also is unique
    if (array_key_exists('primary key', $table_desc)) {
      $pkey = $table_desc['primary key'][0];
      if (array_key_exists($pkey, $insert_values)) {
        $coptions = array('statement_name' => 'pqsel_' . $table . '_' . $pkey);
        if (tripal_core_chado_select($table, array($pkey), array($pkey => $insert_values[$pkey]), $coptions)) {
          watchdog('tripal_core', "tripal_core_chado_insert: Cannot insert duplicate primary key into $table table: " . print_r($values, 1), array(), 'WATCHDOG_ERROR');
          return FALSE;
        }
      }
    }

    // make sure required fields have a value
    if (!is_array($table_desc['fields'])) {
      $table_desc['fields'] = array();
      watchdog('tripal_core', "tripal_core_chado_insert: %table missing fields: \n %schema",
        array('%table' => $table, '%schema' => print_r($table_desc, 1)), WATCHDOG_WARNING);

    }
    foreach ($table_desc['fields'] as $field => $def) {
      // a field is considered missing if it cannot be NULL and there is no default
      // value for it or it is of type 'serial'
      if (array_key_exists('NOT NULL', $def) and
          !array_key_exists($field, $insert_values) and
          !array_key_exists('default', $def) and
          strcmp($def['type'], serial) != 0) {
        watchdog('tripal_core', "tripal_core_chado_insert: Field $table.$field cannot be NULL: " .
          print_r($values, 1), array(), 'WATCHDOG_ERROR');
        return FALSE;
      }
    }
  } //end of validation

  // Now build the insert SQL statement
  $ifields = array();       // contains the names of the fields
  $itypes  = array();       // contains placeholders for the sql query
  $ivalues = array();       // contains the values of the fields
  $i = 1;
  foreach ($insert_values as $field => $value) {
    $ifields[] = $field;
    $ivalues[":$field"] = $value;
    $i++;
    if (strcmp($value, '__NULL__')==0) {
      $itypes[] = "NULL";
    }
    else {
      $itypes[] = ":$field";
    }
  }

  // create the SQL
  $sql = 'INSERT INTO {' . $table . '} (' . implode(", ", $ifields) . ") VALUES (" . implode(", ", $itypes) . ")";
  $result = chado_query($sql, $ivalues);

  // if we have a result then add primary keys to return array
  if ($options['return_record'] == TRUE and $result) {
    if (array_key_exists('primary key', $table_desc) and is_array($table_desc['primary key'])) {
      foreach ($table_desc['primary key'] as $field) {
        $sql = "SELECT CURRVAL('{" . $table . "_" . $field . "_seq}')";
        $results = chado_query($sql);
        $value = $results->fetchObject();
        if (!$value) {
          watchdog('tripal_core', "tripal_core_chado_insert: not able to retrieve primary key after insert: %sql",
            array('%sql' => $sql), WATCHDOG_ERROR);
          return FALSE;
        }
        $values[$field] = $value;
      }
    }
    return $values;
  }
  elseif ($options['return_record'] == FALSE and $result) {
    return TRUE;
  }
  else {
    watchdog('tripal_core', "tripal_core_chado_insert: Cannot insert record into '%table': " . print_r($values, 1), 
      array('%table' => $table), 'WATCHDOG_ERROR');
    return FALSE;
  }

  return FALSE;

}
/**
 * Provides a generic routine for updating into any Chado table
 *
 * Use this function to update a record in any Chado table.  The first
 * argument specifies the table for inserting, the second is an array
 * of values to matched for locating the record for updating, and the third
 * argument give the values to update.  The arrays are mutli-dimensional such
 * that foreign key lookup values can be specified.
 *
 * @param $table
 *  The name of the chado table for inserting
 * @param $match
 *  An associative array containing the values for locating a record to update.
 * @param $values
 *  An associative array containing the values for updating.
 * @param $options
 *  An array of options such as:
 *  - return_record: by default, the function will return the TRUE if the record
 *     was succesfully updated.  However, set this option to TRUE to return the
 *     record that was updated.  The returned record will have the fields provided
 *     but the primary key (if available for the table) will be added to the record.
 * @return
 *  On success this function returns TRUE. On failure, it returns FALSE.
 *
 * Example usage:
 * @code
 $umatch = array(
   'organism_id' => array(
     'genus' => 'Citrus',
     'species' => 'sinensis',
   ),
   'uniquename' => 'orange1.1g000034m.g7',
   'type_id' => array (
     'cv_id' => array (
       'name' => 'sequence',
     ),
     'name' => 'gene',
     'is_obsolete' => 0
   ),
 );
 $uvalues = array(
   'name' => 'orange1.1g000034m.g',
   'type_id' => array (
     'cv_id' => array (
       'name' => 'sequence',
     ),
     'name' => 'mRNA',
     'is_obsolete' => 0
   ),
 );
 *   $result = tripal_core_chado_update('feature',$umatch,$uvalues);
 * @endcode
 * The above code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be updated.
 * The organism_id is specified as a nested array that uses the organism_id
 * foreign key constraint to lookup the specified values to find the exact
 * organism_id. The same nested struture is also used for specifying the
 * values to update.  The function will find the record that matches the
 * columns specified and update the record with the avlues in the $uvalues array.
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_chado_update($table, $match, $values, $options = NULL) {

  if (!is_array($values)) {
    watchdog('tripal_core', 'Cannot pass non array as values for updating.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }
  if (count($values)==0) {
    watchdog('tripal_core', 'Cannot pass an empty array as values for updating.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }

  if (!is_array($match)) {
    watchdog('tripal_core', 'Cannot pass non array as values for matching.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }
  if (count($match)==0) {
    watchdog('tripal_core', 'Cannot pass an empty array as values for matching.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }

  // set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements
  if (!is_array($options)) {
    $options = array();
  }

  if (!array_key_exists('return_record', $options)) {
    $options['return_record'] = FALSE;
  }

  $update_values = array();   // contains the values to be updated
  $update_matches = array();  // contains the values for the where clause


  // get the table description
  $table_desc = tripal_core_get_chado_table_schema($table);

  // if the user wants us to return the record then we need to get the
  // unique primary key if one exists.  That way we can add it to the
  // values that get returned at the end of the function
  $pkeys = array();
  if ($options['return_record'] == TRUE) {
    if (array_key_exists('primary key', $table_desc) and is_array($table_desc['primary key'])) {
      $columns = array();
      $stmt_suffix = '';
      foreach ($table_desc['primary key'] as $field) {
        $columns[] = $field;
        $stmt_suffix .= substr($field, 0, 2);
      }
      $options2 = array('statement_name' => 'sel_' . $table . '_' . $stmt_suffix);
      $results = tripal_core_chado_select($table, $columns, $match, $options2);
      if (count($results) > 0) {
        foreach ($results as $index => $pkey) {
          $pkeys[] = $pkey;
        }
      }
    }
  }

  // get the values needed for matching in the SQL statement
  foreach ($match as $field => $value) {
    if (is_array($value)) {   
      $results = tripal_core_chado_get_foreign_key($table_desc, $field, $value);
      if (sizeof($results) > 1) {
        watchdog('tripal_core', 'tripal_core_chado_update: When trying to find record to update, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)), WATCHDOG_ERROR);
      }
      elseif (sizeof($results) < 1) {
        //watchdog('tripal_core', 'tripal_core_chado_update: When trying to find record to update, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)), WATCHDOG_ERROR);
      }
      else {
        $update_matches[$field] = $results[0];
      }
    }
    else {
      $update_matches[$field] = $value;
    }
  }

  // get the values used for updating
  foreach ($values as $field => $value) {
    if (is_array($value)) {
      $foreign_options = array();
      // select the value from the foreign key relationship for this value   
      $results = tripal_core_chado_get_foreign_key($table_desc, $field, $value, $foreign_options);
      if (sizeof($results) > 1) {
        watchdog('tripal_core', 'tripal_core_chado_update: When trying to find update values, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)), WATCHDOG_ERROR);
      }
      elseif (sizeof($results) < 1) {
        //watchdog('tripal_core', 'tripal_core_chado_update: When trying to find update values, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
      }
      else {
        $update_values[$field] = $results[0];
      }
    }
    else {
      $update_values[$field] = $value;
    }
  }

  // now build the SQL statement
  $sql  = 'UPDATE {' . $table . '} SET ';
  $args = array();        // arguments passed to chado_query
  foreach ($update_values as $field => $value) {    
    if (strcmp($value, '__NULL__') == 0) {
      $sql .= " $field = NULL, ";
    }
    else {
      $sql .= " $field = :$field, ";
      $args[":$field"] = $value;
    }    
  }
  $sql = drupal_substr($sql, 0, -2);  // get rid of the trailing comma & space

  $sql .= " WHERE ";
  foreach ($update_matches as $field => $value) {
    if (strcmp($value, '__NULL__')==0) {
      $sql .= " $field = NULL AND ";
    }
    else {
      $sql .= " $field = :$field AND ";
      $args[":$field"] = $value;
    }    
  }
  $sql = drupal_substr($sql, 0, -4);  // get rid of the trailing 'AND'

  $result = chado_query($sql, $args);

  // if we have a result then add primary keys to return array
  if ($options['return_record'] == TRUE and $result) {
    // only if we have a single result do we want to add the primary keys to the values
    // array.  If the update matched many records we can't add the pkeys

    if (count($pkeys) == 1) {
      foreach ($pkeys as $index => $pkey) {
        foreach ($pkey as $field => $fvalue) {
          $values[$field] = $fvalue;
        }
      }
    }
    return $values;
  }
  elseif ($options['return_record'] == FALSE and $result) {
    return TRUE;
  }
  else {
    watchdog('tripal_core', "Cannot update record in $table table.  \nMatch:" . print_r($match, 1) . "\nValues: " . print_r($values, 1), array(), 'WATCHDOG_ERROR');
    return FALSE;
  }

  return FALSE;
}
/**
 * Provides a generic function for deleting a record(s) from any chado table
 *
 * Use this function to delete a record(s) in any Chado table.  The first
 * argument specifies the table to delete from and the second is an array
 * of values to match for locating the record(s) to be deleted.  The arrays
 * are mutli-dimensional such that foreign key lookup values can be specified.
 *
 * @param $table
 *  The name of the chado table for inserting
 * @param $match
 *  An associative array containing the values for locating a record to update.
 * @param $options
 *  Currently there are no options
 * @return
 *   On success this function returns TRUE. On failure, it returns FALSE.
 *
 * Example usage:
 * @code
 $umatch = array(
   'organism_id' => array(
     'genus' => 'Citrus',
     'species' => 'sinensis',
   ),
   'uniquename' => 'orange1.1g000034m.g7',
   'type_id' => array (
     'cv_id' => array (
       'name' => 'sequence',
     ),
     'name' => 'gene',
     'is_obsolete' => 0
   ),
 );
 $uvalues = array(
   'name' => 'orange1.1g000034m.g',
   'type_id' => array (
     'cv_id' => array (
       'name' => 'sequence',
     ),
     'name' => 'mRNA',
     'is_obsolete' => 0
   ),
 );
 *   $result = tripal_core_chado_update('feature', $umatch, $uvalues);
 * @endcode
 * The above code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be deleted.
 * The organism_id is specified as a nested array that uses the organism_id
 * foreign key constraint to lookup the specified values to find the exact
 * organism_id. The same nested struture is also used for specifying the
 * values to update.  The function will find all records that match the
 * columns specified and delete them.
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_chado_delete($table, $match, $options = NULL) {

  if (!is_array($match)) {
    watchdog('tripal_core', 'Cannot pass non array as values for matching.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }
  if (count($match)==0) {
    watchdog('tripal_core', 'Cannot pass an empty array as values for matching.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }

  // set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements
  if (!is_array($options)) {
    $options = array();
  }

  $delete_matches = array();  // contains the values for the where clause

  // get the table description
  $table_desc = tripal_core_get_chado_table_schema($table);
  $fields = $table_desc['fields'];

  // get the values needed for matching in the SQL statement
  foreach ($match as $field => $value) {
    if (is_array($value)) {
      // if the user has specified an array of values to delete rather than
      // FK relationships the keep those in our match
      if (array_values($value) === $value) {
        $delete_matches[$field] = $value;
      }
      else {
        $results = tripal_core_chado_get_foreign_key($table_desc, $field, $value);
        if (sizeof($results) > 1) {
          watchdog('tripal_core', 'tripal_core_chado_delete: When trying to find record to delete, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)), WATCHDOG_ERROR);
        }
        elseif (sizeof($results) < 1) {
          //watchdog('tripal_core', 'tripal_core_chado_delete: When trying to find record to delete, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
        }
        else {
          $delete_matches[$field] = $results[0];
        }
      }
    }
    else {
      $delete_matches[$field] = $value;
    }
  }

  // now build the SQL statement
  $sql = 'DELETE FROM {' . $table . '} WHERE ';
  $args = array();
  foreach ($delete_matches as $field => $value) {
    // if we have an array values then this is an "IN" clasue.
    // we cannot use prepared statements with these
    
    if (count($value) > 1) {
      $sql .= "$field IN (";
      $index = 0;
      foreach ($value as $v) {
      	$sql .= ":$field" . $index . ", ";
        $args[":$field" . $index] = $v;
        $index++;
      }
      $sql = drupal_substr($sql, 0, -2); // get rid of trailing ', '
      $sql .= ") AND ";
    }
    else {
      if (strcmp($value, '__NULL__') == 0) {
        $sql .= " $field = NULL AND ";
      }
      else {
        $sql .= " $field = :$field AND ";
        $args[":$field"] = $value;
      }
    }
  }
  $sql = drupal_substr($sql, 0, -4);  // get rid of the trailing 'AND'

  // finally perform the delete.  If successful, return the updated record
  $result = chado_query($sql, $args);
  if ($result) {
    return TRUE;
  }
  else {
    watchdog('tripal_core', "Cannot delete record in $table table.  Match:" . print_r($match, 1) . ". Values: " . print_r($values, 1), array(), 'WATCHDOG_ERROR');
    return FALSE;
  }
  return FALSE;
}



/**
 * Provides a generic routine for selecting data from a Chado table
 *
 * Use this function to perform a simple select from any Chado table.
 *
 * @param $table
 *  The name of the chado table for inserting
 * @param $columns
 *  An array of column names
 * @param $values
 *  An associative array containing the values for filtering the results. In the
 *  case where multiple values for the same time are to be selected an additional
 *  entry for the field should appear for each value
 * @param $options
 *  An associative array of additional options where the key is the option
 *  and the value is the value of that option.
 *
 * Additional Options Include:
 *  - has_record
 *     Set this argument to 'TRUE' to have this function return a numeric
 *     value for the number of recrods rather than the array of records.  this
 *     can be useful in 'if' statements to check the presence of particula records.
 *  - return_sql
 *     Set this to 'TRUE' to have this function return an array where the first
 *     element is the sql that would have been run and the second is an array of
 *     arguments.
 *  - case_insensitive_columns
 *     An array of columns to do a case insensitive search on.
 *  - regex_columns
 *     An array of columns where the value passed in should be treated as a regular expression
 *  - order_by
 *     An associative array containing the column names of the table as keys
 *     and the type of sort (i.e. ASC, DESC) as the values.  The results in the
 *     query will be sorted by the key values in the direction listed by the value
 *  - statement_name: the name of the prepared statement to use. If the statement
 *     has not yet been prepared it will be prepared automatically. On subsequent
 *     calls with the same statement_name only an execute on the previously
 *     prepared statement will occur.
 *  - is_prepared: TRUE or FALSE. Whether or not the statement is prepared. By
 *     default if the statement is not prepared it will be automatically.
 *     However to avoid this check, which requires a database query you can
 *     set this value to true and the check will not be performed.
 *  - is_duplicate: TRUE or FALSE.  Checks the values submited to see if
 *     they violate any of the unique constraints. If so, the record
 *     is returned, if not, FALSE is returned.
 *  - pager:  Use this option if it is desired to return only a subset of results
 *     so that they may be shown with in a Drupal-style pager. This should be
 *     an array with two keys: 'limit' and 'element'.  The value of 'limit'
 *     should specify the number of records to return and 'element' is a
 *     unique integer to differentiate between pagers when more than one
 *     appear on a page.  The 'element' should start with zero and increment by
 *     one for each pager.  The pager currently does not work with prepared queries
 *     (when using the -statement_name option).
 *
 * @return
 *  A database query result resource, FALSE if the query was not executed
 *  correctly, an empty array if no records were matched, or the number of records
 *  in the dataset if $has_record is set.
 *  If the option 'is_duplicate' is provided and the record is a duplicate it
 *  will return the duplicated record.  If the 'has_record' option is provided
 *  a value of TRUE will be returned if a record exists and FALSE will bee
 *  returned if there are not records.
 *
 * Example usage:
 * @code
 *   $columns = array('feature_id', 'name');
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => array('sinensis', 'clementina'),
 *      ),
 *     'uniquename' => 'orange1.1g000034m.g',
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $options = array(
 *     'statement_name' => 'sel_feature_genus_species_cvname'
 *     'order_by' => array(
 *        'name' => 'ASC'
 *     ),
 *   );
 *   $result = tripal_core_chado_select('feature',$columns,$values,$options);
 * @endcode
 * The above code selects a record from the feature table using the three fields
 * that uniquely identify a feature.  The $columns array simply lists the columns
 * to select. The $values array is nested such that the organism is identified by
 * way of the organism_id foreign key constraint by specifying the genus and
 * species.  The cvterm is also specified using its foreign key and the cv_id
 * for the cvterm is nested as well.  In the example above, two different species
 * are allowed to match
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_chado_select($table, $columns, $values, $options = NULL) {

  if (!is_array($values)) {
    watchdog('tripal_core', 'Cannot pass non array as values for selecting.', array(),
      WATCHDOG_ERROR);
      return FALSE;
  }
  if (!is_array($columns)) {
    watchdog('tripal_core', 'Cannot pass non array as columns for selecting.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }
  if (count($columns)==0) {
    watchdog('tripal_core', 'Cannot pass an empty array as columns for selecting.', array(),
      WATCHDOG_ERROR);
    return FALSE;
  }

  // set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements
  if (!is_array($options)) {
    $options = array();
  }
  if (!array_key_exists('case_insensitive_columns', $options)) {
    $options['case_insensitive_columns'] = array();
  }
  if (!array_key_exists('regex_columns', $options)) {
    $options['regex_columns'] = array();
  }
  if (!array_key_exists('order_by', $options)) {
    $options['order_by'] = array();
  }
  if (!array_key_exists('return_sql', $options)) {
    $options['return_sql'] = FALSE;
  }
  if (!array_key_exists('has_record', $options)) {
    $options['has_record'] = FALSE;
  }
  if (!array_key_exists('is_duplicate', $options)) {
    $options['is_duplicate'] = FALSE;
  }
  $pager = array();
  if (array_key_exists('pager', $options)) {
    $pager = $options['pager'];
  }

  // check that our columns and values arguments are proper arrays
  if (!is_array($columns)) {
    watchdog('tripal_core', 'the $columns argument for tripal_core_chado_select must be an array.');
    return FALSE;
  }
  if (!is_array($values)) {
    watchdog('tripal_core', 'the $values argument for tripal_core_chado_select must be an array.');
    return FALSE;
  }

  // get the table description
  $table_desc = tripal_core_get_chado_table_schema($table);
  $select = '';
  $from = '';
  $where = array();
  $args = array();

  // if the 'use_unique' option is turned on then we want
  // to remove all but unique keys
  if ($options['is_duplicate'] and array_key_exists('unique keys', $table_desc)) {
    $ukeys = $table_desc['unique keys'];
    $has_results = 0;

    // iterate through the unique constraints and reset the values and columns
    // arrays to only include these fields
    foreach ($ukeys as $cname => $fields) {
      if ($has_results) {
         continue;
      }
      $new_values = array();
      $new_columns = array();
      $new_options = array();
      $uq_sname = "uq_" . $table . "_";
      $has_pkey = 0;


      // include the primary key in the results returned
      if (array_key_exists('primary key', $table_desc)) {
        $has_pkey = 1;
        $pkeys = $table_desc['primary key'];
        foreach ($pkeys as $index => $key) {
          array_push($new_columns, $key);
        }
      }

      // recreate the $values and $columns arrays
      foreach ($fields as $field) {
        if (array_key_exists($field, $values)) {
          $new_values[$field] = $values[$field];
          $uq_sname .= substr($field, 0, 2);
          // if there is no primary key then use the unique contraint fields
          if (!$has_pkey) {
            array_push($new_columns, $field);
          }
        }
        // if the field doesn't exist in the values array then
        // substitute any default values
        elseif (array_key_exists('default', $table_desc['fields'][$field])) {
          $new_values[$field] = $table_desc['fields'][$field]['default'];
          $uq_sname .= substr($field, 0, 2);
          if (!$has_pkey) {
            array_push($new_columns, $field);
          }
        }
        // if there is no value (default or otherwise) check if this field is
        // allowed to be null
        elseif (!$table_desc['fields'][$field]['not null']) {
          $new_values[$field] = NULL;
          $uq_sname .= "n" . substr($field, 0, 2);
          if (!$has_pkey) {
            array_push($new_columns, $field);
          }
        }
        // if the array key doesn't exist in the values given by the caller
        // and there is no default value then we cannot check if the record
        // is a duplicate so return FALSE
        else {
          watchdog('tripal_core', "tripal_core_chado_select: There is no value for %field"
            . " thus we cannot check if this record is unique",
            array('%field' => $field), WATCHDOG_ERROR);
          return FALSE;
        }
      }
      $new_options['statement_name'] = $uq_sname;

      $results = tripal_core_chado_select($table, $new_columns, $new_values, $new_options);
      // if we have a duplicate record then return the results
      if (count($results) > 0) {
        $has_results = 1;
      }
      unset($new_columns);
      unset($new_values);
      unset($new_options);
    }
    if ($options['has_record'] and $has_results) {
      return TRUE;
    }
    else {
      return $results;
    }
  }

  foreach ($values as $field => $value) {
    // make sure the field is in the table description. If not then return an error
    // message
    if (!array_key_exists($field, $table_desc['fields'])) {
      watchdog('tripal_core', "tripal_core_chado_select: The field '%field' does not exist " .
        "for the table '%table'.  Cannot perform query. Values: %array",
        array('%field' => $field, '%table' => $table, '%array' => print_r($values, 1)), WATCHDOG_ERROR);
      return array();
    }

    $select[] = $field;
    if (is_array($value)) {
      // if the user has specified multiple values for matching then this we
      // want to catch that and save them in our $where array, otherwise
      // we'll descend for a foreign key relationship
      if (array_values($value) === $value) {
        $where[$field] = $value;
      }
      else {
        // select the value from the foreign key relationship for this value
        $foreign_options = array(
          'regex_columns' => $options['regex_columns'],
        );

        $results = tripal_core_chado_get_foreign_key($table_desc, $field, $value, $foreign_options);
        if (!$results or count($results)==0) {
          return array();
        }
        else {
          $where[$field] = $results;
        }
      }
    }
    else {
      // need to catch a 0 and make int if integer field
      // but we don't want to catch a NULL
      if ($value === NULL) {
        $where[$field] = NULL;
      }
      elseif ($table_desc['fields'][$field]['type'] == 'int') {
        $where[$field][] = (int) $value;
      }
      else {
        $where[$field][] = $value;
      }
    }
  }

  
  // now build the SQL and prepared SQL statements. We may not use
  // the prepared statement if it wasn't requested in the options or if the
  // argument in a where statement has multiple values.
  if (empty($where)) {
    // sometimes want to select everything
    $sql  = "SELECT " . implode(', ', $columns) . " ";
    $sql .= 'FROM {' . $table . '} ';
    // we don't prepare a statement if there is no where clause
    $prepared = FALSE;
  }
  else {
    $sql  = "SELECT " . implode(', ', $columns) . " ";
    $sql .= 'FROM {' . $table . '} ';

    // if $values is empty then we want all results so no where clause
    if (!empty($values)) {
      $sql .= "WHERE ";
    }
    foreach ($where as $field => $value) {

      // if we have multiple values returned then we need an 'IN' statement
      // in our where statement
      if (count($value) > 1) {
        $sql .= "$field IN (";
        $index = 0;
        foreach ($value as $v) {
        	$sql .= ":$field" . $index . ', ';
          $args[":$field" . $index] = $v;
          $index++;
        }
        $sql = drupal_substr($sql, 0, -2); // remove trailing ', '
        $sql .= ") AND ";
      }
      // if we have a null value then we need an IS NULL in our where statement
      elseif ($value === NULL) {
        $sql .= "$field IS NULL AND ";
        // Need to remove one from the argument count b/c nulls don't add an argument
      }
      // if we have a single value then we need an = in our where statement
      else {
        $operator = '=';
        if (in_array($field, $options['regex_columns'])) {
          $operator = '~*';
        }        
        if (in_array($field, $options['case_insensitive_columns'])) {
          $sql .= "lower($field) $operator lower(:$field) AND ";
          $args[":$field"] = $value[0];
        }
        else {
	        $sql .= "$field $operator :$field AND ";
	        $args[":$field"] = $value[0];        	
        }
      }
    } // end foreach item in where clause
    $sql = drupal_substr($sql, 0, -4);  // get rid of the trailing 'AND '
  } // end if (empty($where)){ } else {

  // finally add any ordering of the results to the SQL statement
  if (count($options['order_by']) > 0) {
    $sql .= " ORDER BY ";
    foreach ($options['order_by'] as $field => $dir) {
      $sql .= "$field $dir, ";
    }
    $sql = drupal_substr($sql, 0, -2);  // get rid of the trailing ', '
  }
  
  // if the caller has requested the SQL rather than the results...
  // which happens in the case of wanting to use the Drupal pager, then do so
  if ($options['return_sql'] == TRUE) {
    return array('sql' => $sql, 'args' => $args);
  }
  if (array_key_exists('limit', $pager)) {
    $resource = chado_pager_query($sql, $pager['limit'], $pager['element'], NULL, $args);
  }
  else {
    $resource = chado_query($sql, $args);
  }

  // format results into an array
  $results = array();
  foreach ($resource as $r) {
    $results[] = $r;
  }
  if ($options['has_record']) {
    return count($results);
  }
  return $results;
}

/**
 * Gets the value of a foreign key relationship
 *
 * This function is used by tripal_core_chado_select, tripal_core_chado_insert,
 * and tripal_core_chado_update to iterate through the associate array of
 * values that gets passed to each of those routines.  The values array
 * is nested where foreign key contraints are used to specify a value that.  See
 * documentation for any of those functions for further information.
 *
 * @param $table_desc
 *  A table description for the table with the foreign key relationship to be identified generated by
 *  hook_chado_<table name>_schema()
 * @param $field
 *  The field in the table that is the foreign key.
 * @param $values
 *  An associative array containing the values
 * @param $options
 *  An associative array of additional options where the key is the option
 *  and the value is the value of that option. These options are passed on to tripal_core_chado_select.
 *
 * Additional Options Include:
 *  - case_insensitive_columns
 *     An array of columns to do a case insensitive search on.
 *  - regex_columns
 *     An array of columns where the value passed in should be treated as a regular expression
 *
 * @return
 *  A string containg the results of the foreign key lookup, or FALSE if failed.
 *
 * Example usage:
 * @code
 *
 *   $values = array(
 *     'genus' => 'Citrus',
 *     'species' => 'sinensis',
 *   );
 *   $value = tripal_core_chado_get_foreign_key('feature', 'organism_id',$values);
 *
 * @endcode
 * The above code selects a record from the feature table using the three fields
 * that uniquely identify a feature.  The $columns array simply lists the columns
 * to select. The $values array is nested such that the organism is identified by
 * way of the organism_id foreign key constraint by specifying the genus and
 * species.  The cvterm is also specified using its foreign key and the cv_id
 * for the cvterm is nested as well.
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_chado_get_foreign_key($table_desc, $field, $values, $options = NULL) {

  // set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements
  if (!is_array($options)) {
    $options = array();
  }
  if (!array_key_exists('case_insensitive_columns', $options)) {
    $options['case_insensitive_columns'] = array();
  }
  if (!array_key_exists('regex_columns', $options)) {
    $options['regex_columns'] = array();
  }

  // get the list of foreign keys for this table description and
  // iterate through those until we find the one we're looking for
  $fkeys = '';
  if (array_key_exists('foreign keys', $table_desc)) {
    $fkeys = $table_desc['foreign keys'];
  }
  if ($fkeys) {
    foreach ($fkeys as $name => $def) {
      if (is_array($def['table'])) {
        //foreign key was described 2X
        $message = "The foreign key " . $name . " was defined twice. Please check modules "
          . "to determine if hook_chado_schema_<version>_" . $table_desc['table'] . "() was "
          . "implemented and defined this foreign key when it wasn't supposed to. Modules "
          . "this hook was implemented in: " . implode(', ',
        module_implements("chado_" . $table_desc['table'] . "_schema")) . ".";
        watchdog('tripal_core', $message);
        drupal_set_message(check_plain($message), 'error');
        continue;
      }
      $table = $def['table'];
      $columns = $def['columns'];

      // iterate through the columns of the foreign key relationship
      foreach ($columns as $left => $right) {
        // does the left column in the relationship match our field?
        if (strcmp($field, $left) == 0) {
          // the column name of the foreign key matches the field we want
          // so this is the right relationship.  Now we want to select
          $select_cols = array($right);
          $result = tripal_core_chado_select($table, $select_cols, $values, $options);
          $fields = array();
          if ($result and count($result) > 0) {
            foreach ($result as $obj) {
              $fields[] = $obj->$right;
            }
            return $fields;
          }
        }
      }
    }
  }
  else {
    // TODO: what do we do if we get to this point and we have a fk
    // relationship expected but we don't have any definition for one in the
    // table schema??
    $version = tripal_core_get_chado_version(TRUE);
    $message = t("There is no foreign key relationship defined for " . $field . " .
       To define a foreign key relationship, determine the table this foreign
       key referrs to (<foreign table>) and then implement
       hook_chado_chado_schema_v<version>_<foreign table>(). See
       tripal_feature_chado_v1_2_schema_feature for an example. Chado version: $version");
    watchdog('tripal_core', $message);
    drupal_set_message(check_plain($message), 'error');
  }

  return array();
}

/**
 * Generates an object containing the full details of a record(s) in chado.
 *
 * This differs from the objects returned by tripal_core_chado_select in so far as all foreign key
 * relationships have been followed meaning you have more complete details. Thus this function
 * should be used whenever you need a full variable and tripal_core_chado_select should be used if
 * you only case about a few columns.
 *
 * @param $table
 *   The name of the base table to generate a variable for
 * @param $values
 *   A select values array that selects the records you want from the base table
 *   (this has the same form as tripal_core_chado_select)
 * @param $base_options
 *   An array containing options for the base table.  For example, an
 *   option of 'order_by' may be used to sort results in the base table
 *   if more than one are returned.  The options must be compatible with
 *   the options accepted by the tripal_core_chado_select() function.
 *   Additionally,  These options are available for this function:
 *   -return_array:
 *     can be provided to force the function to always return an array. Default
 *     behavior is to return a single record if only one record exists or to return
 *     an array if multiple records exist.
 *  - include_fk:
 *     an array of FK relationships to follow. By default, the
 *     tripal_core_chado_select function will follow all FK relationships but this
 *     may generate more queries then is desired slowing down this function call when
 *     there are lots of FK relationships to follow.  Provide an array specifying the
 *     fields to include.  For example, if expanding a property table (e.g. featureprop)
 *     and you want the CV and accession but do not want the DB the following
 *     array would work:
 *
 *        $table_options =  array(
 *          'include_fk' => array(
 *            'type_id' => array(
 *              'cv_id' => 1,
 *              'dbxref_id' => 1,
 *            )
 *          )
 *        );
 *
 *     The above array will expand the 'type_id' of the property table but only
 *     further expand the cv_id and the dbxref_id and will go no further.
 *   - pager:
 *     Use this option if it is desired to return only a subset of results
 *     so that they may be shown within a Drupal-style pager. This should be
 *     an array with two keys: 'limit' and 'element'.  The value of 'limit'
 *     should specify the number of records to return and 'element' is a
 *     unique integer to differentiate between pagers when more than one
 *     appear on a page.  The 'element' should start with zero and increment by
 *     one for each pager.  This only works when type is a 'table'.
 * @return
 *   Either an object (if only one record was selected from the base table)
 *   or an array of objects (if more than one record was selected from the base table).
 *   If the option 'return_array' is provided the function always returns an array.
 *
 * Example Usage:
 * @code
   $values = array(
     'name' => 'Medtr4g030710'
   );
   $features = tripal_core_generate_chado_var('feature', $values);
 * @endcode
 * This will return an object if there is only one feature with the name Medtr4g030710 or it will
 * return an array of feature objects if more than one feature has that name.
 *
 * Note to Module Designers: Fields can be excluded by default from these objects by implementing
 * one of the following hooks:
 *  - hook_exclude_field_from_tablename_by_default (where tablename is the name of the table):
 *      This hook allows you to add fields to be excluded on a per table basis. Simply implement
 *      this hook to return an array of fields to be excluded. For example:
 * @code
   mymodule_exclude_field_from_feature_by_default() {
     return array('residues' => TRUE);
   }
 * @endcode
 *      will ensure that feature.residues is ecluded from a feature object by default.
 *  - hook_exclude_type_by_default:
 *      This hook allows you to exclude fields from all tables that are of a given postgresql field
 *      type. Simply implement this hook to return an array of postgresql types mapped to criteria.
 *      Then all fields of that type where the criteria supplied returns TRUE will be excluded from
 *      any table. Tokens available in criteria are &gt;field_value&lt;  and &gt;field_name&lt; . For example:
 * @code
   mymodule_exclude_type_by_default() {
     return array('text' => 'length(&gt;field_value&lt; ) > 50');
   }
 * @endcode
 *      will exclude all text fields with a length > 50. Thus if $feature.residues is longer than 50 *      it will be excluded, otherwise it will be added.
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_generate_chado_var($table, $values, $base_options = array()) {
  $all = new stdClass();

  $return_array = 0;
  if (array_key_exists('return_array', $base_options)) {
    $return_array = 1;
  }
  $include_fk = 0;
  if (array_key_exists('include_fk', $base_options)) {
    $include_fk = $base_options['include_fk'];
  }
  $pager = array();
  if (array_key_exists('pager', $base_options)) {
    $pager = $base_options['pager'];
  }
  // get description for the current table----------------------------------------------------------
  $table_desc = tripal_core_get_chado_table_schema($table);
  if (!$table_desc or count($table_desc) == 0) {
    watchdog('tripal_core', "tripal_core_generate_chado_var: The table '%table' has not been defined. " .
             "and cannot be expanded. If this is a custom table, please add it using the Tripal " .
             "custom table interface.", array('%table' => $table), WATCHDOG_ERROR);
    if ($return_array) {
      return array();
    }
    return FALSE;
  }
  $table_primary_key = $table_desc['primary key'][0];
  $table_columns = array_keys($table_desc['fields']);

  // Expandable fields without value needed for criteria--------------------------------------------
  $all->expandable_fields = array();
  if ($table_desc['referring_tables']) {
    $all->expandable_tables = $table_desc['referring_tables'];
  }
  else {
    $all->expandable_tables = array();
  }
  $all->expandable_nodes = array();

  /*
  // Get fields to be removed by name.................................
  $fields_to_remove = module_invoke_all('exclude_field_from_' . $table . '_by_default');
  foreach ($fields_to_remove as $field_name => $criteria) {
    //replace &gt;field_name&lt;  with the current field name &
    $criteria = preg_replace('/&gt;field_name&lt; /', addslashes($field_name), $criteria);
    // if field_value needed we can't deal with this field yet
    if (preg_match('/&gt;field_value&lt; /', $criteria)) {
      break;
    }

    //if criteria then remove from query
    // @coder-ignore: only module designers can populate $criteria -not security risk
    $success = php_eval('<?php return ' . $criteria . '; ?>');
    if ($success) {
      unset($table_columns[array_search($field_name, $table_columns)]);
      unset($fields_to_remove[$field_name]);
      $all->expandable_fields[] = $table . '.' . $field_name;
    }
  }

  //Get fields to be removed by type................................
  $types_to_remove = module_invoke_all('exclude_type_by_default');
  $field_types = array();
  foreach ($table_desc['fields'] as $field_name => $field_array) {
    $field_types[$field_array['type']][] = $field_name;
  }
  foreach ($types_to_remove as $field_type => $criteria) {
    // if there are fields of that type to remove
    if (is_array($field_types[$field_type])) {
      //replace &gt;field_name&lt;  with the current field name &
      $criteria = preg_replace('/&gt;field_name&lt; /', addslashes($field_name), $criteria);
      foreach ($field_types[$field_type] as $field_name) {
        // if field_value needed we can't deal with this field yet
        if (preg_match('/&gt;field_value&lt; /', $criteria)) {
          $fields_to_remove[$field_name] = $criteria;
          continue;
        }
        // if field_value needed we can't deal with this field yet
        if (preg_match('/&gt;field_value&lt; /', $criteria)) {
          break;
        }
        //if criteria then remove from query
        // @coder-ignore: only module designers can populate $criteria -not security risk
        $success = php_eval('<?php return ' . $criteria . '; ?>');
        if ($success) {
          unset($table_columns[array_search($field_name, $table_columns)]);
          $all->expandable_fields[] = $table . '.' . $field_name;
        }
      } //end of foreach field of that type
    }
  } //end of foreach type to be removed
*/
  // get the values for the record in the current table---------------------------------------------
  $results = tripal_core_chado_select($table, $table_columns, $values, $base_options);
  
  if ($results) {
    foreach ($results as $key => $object) {
      // Add empty expandable_x arrays
      $object->expandable_fields = $all->expandable_fields;
      $object->expandable_tables = $all->expandable_tables;
      $object->expandable_nodes = $all->expandable_nodes;
      // add curent table
      $object->tablename = $table;

      // check if the current table maps to a node type-----------------------------------------------
      // if this table is connected to a node there will be a chado_tablename table in drupal
      if (db_table_exists('chado_' . $table)) {
        // that has a foreign key to this one ($table_desc['primary key'][0]
        // and to the node table (nid)
        $sql = "
          SELECT $table_primary_key, nid 
          FROM {chado_$table} 
          WHERE $table_primary_key = :$table_primary_key";
        $mapping = db_query($sql, array(":$table_primary_key" => $object->{$table_primary_key}))->fetchObject();
        if ($mapping->{$table_primary_key}) {
          $object->nid = $mapping->nid;
          $object->expandable_nodes[] = $table;
        }
      }

      // remove any fields where criteria needs to be evalulated---------------------------------------
/*      foreach ($fields_to_remove as $field_name => $criteria) {
        if (!isset($object->{$field_name})) {
          break;
        }
        $criteria = preg_replace('/&gt;field_value&lt; /', addslashes($object->{$field_name}), $criteria);
        $success = php_eval('<?php return ' . $criteria . '; ?>');
        if ($success) {
          unset($object->{$field_name});
          $object->expandable_fields[] = $table . '.' . $field_name;
        }
      }
*/
      // recursively follow foreign key relationships nesting objects as we go------------------------
      if ($table_desc['foreign keys']) {
        foreach ($table_desc['foreign keys'] as $foreign_key_array) {
          $foreign_table = $foreign_key_array['table'];
          foreach ($foreign_key_array['columns'] as $foreign_key => $primary_key) {
            // Note: Foreign key is the field in the current table whereas primary_key is the field in
            // the table referenced by the foreign key
            //Dont do anything if the foreign key is empty
            if (empty($object->{$foreign_key})) {
              continue;
            }

            if ($include_fk) {
              // don't recurse if the callee has supplied an $fk_include list and this
              // FK table is not in the list.
              if (is_array($include_fk) and !array_key_exists($foreign_key, $include_fk)) {
                continue;
              }
              // if we have the option but it is not an array then we don't recurse any furutehr
              if (!is_array($include_fk)) {
                continue;
              }
            }
            // get the record from the foreign table
            $foreign_values = array($primary_key => $object->{$foreign_key});
            $options = array();
            if (is_array($include_fk)) {
              $options['include_fk'] = $include_fk[$foreign_key];
            }
            $foreign_object = tripal_core_generate_chado_var($foreign_table, $foreign_values, $options);
            // add the foreign record to the current object in a nested manner
            $object->{$foreign_key} = $foreign_object;
            // Flatten expandable_x arrays so only in the bottom object
            if (is_array($object->{$foreign_key}->expandable_fields)) {
              $object->expandable_fields = array_merge(
              $object->expandable_fields,
              $object->{$foreign_key}->expandable_fields
              );
              unset($object->{$foreign_key}->expandable_fields);
            }
            if (is_array($object->{$foreign_key}->expandable_tables)) {
              $object->expandable_tables = array_merge(
              $object->expandable_tables,
              $object->{$foreign_key}->expandable_tables
              );
              unset($object->{$foreign_key}->expandable_tables);
            }
            if (is_array($object->{$foreign_key}->expandable_nodes)) {
              $object->expandable_nodes = array_merge(
              $object->expandable_nodes,
              $object->{$foreign_key}->expandable_nodes
              );
              unset($object->{$foreign_key}->expandable_nodes);
            }
          }
        }
        $results[$key] = $object;
      }
    }
  }
  
  // convert the results into an array
  $results_arr = array(); 
  foreach ($results as $record) {
  	$results_arr[] = $record;
  }
  // check only one result returned
  if (!$return_array) {
    if (sizeof($results_arr) == 1) {
      // add results to object
      return $results_arr[0];
    }
    elseif (!empty($results_arr)) {
      return $results_arr;
    }
    else {
      // no results returned
    }
  }
  // the caller has requested results are always returned as
  // an array
  else {
    if (!$results_arr) {
      return array();
    }
    else {
      return $results_arr;
    }
  }
}

/**
 * Retrieves fields/tables/nodes that were excluded by default from a variable and adds them
 *
 * This function exists to allow tripal_core_generate_chado_var() to excldue some
 * fields/tables/nodes from the default form of a variable without making it extremely difficult for
 * the tripal admin to get at these variables if he/she wants them.
 *
 * @param $object
 *   This must be an object generated using tripal_core_generate_chado_var()
 * @param $type
 *   Must be one of 'field', 'table', 'node'. Indicates what is being expanded.
 * @param $to_expand
 *   The name of the field/table/node to be expanded
 * @param $table_options
 *   - order_by:
 *     An array containing options for the base table.  For example, an
 *     option of 'order_by' may be used to sort results in the base table
 *     if more than one are returned.  The options must be compatible with
 *     the options accepted by the tripal_core_chado_select() function.
 *   - return_array:
 *     Additionally,  The option 'return_array' can be provided to force
 *     the function to expand tables as an array. Default behavior is to expand
 *     a table as single record if only one record exists or to expand as an array if
 *     multiple records exist.
 *   - include_fk:
 *     an array of FK relationships to follow. By default, the
 *     tripal_core_chado_select function will follow all FK relationships but this
 *     may generate more queries then is desired slowing down this function call when
 *     there are lots of FK relationships to follow.  Provide an array specifying the
 *     fields to include.  For example, if expanding a property table (e.g. featureprop)
 *     and you want the CV and accession but do not want the DB the following
 *     array would work:
 *        $table_options =  array(
 *          'include_fk' => array(
 *            'type_id' => array(
 *              'cv_id' => 1,
 *              'dbxref_id' => 1,
 *            )
 *          )
 *        );
 *
 *     The above array will expand the 'type_id' of the property table but only
 *     further expand the cv_id and the dbxref_id and will go no further.
 *   - pager:
 *     Use this option if it is desired to return only a subset of results
 *     so that they may be shown within a Drupal-style pager. This should be
 *     an array with two keys: 'limit' and 'element'.  The value of 'limit'
 *     should specify the number of records to return and 'element' is a
 *     unique integer to differentiate between pagers when more than one
 *     appear on a page.  The 'element' should start with zero and increment by
 *     one for each pager.  This only works when type is a 'table'.
 * @return
 *   A chado object supplemented with the field/table/node requested to be expanded.
 *   If the type is a table and it has already been expanded no changes is made to the
 *   returned object
 *
 * Example Usage:
 * @code
   // Get a chado object to be expanded
   $values = array(
     'name' => 'Medtr4g030710'
   );
   $features = tripal_core_generate_chado_var('feature', $values);
   // Expand the organism node
   $feature = tripal_core_expand_chado_vars($feature, 'node', 'organism');
   // Expand the feature.residues field
   $feature = tripal_core_expand_chado_vars($feature, 'field', 'feature.residues');
   // Expand the feature properties (featureprop table)
   $feature = tripal_core_expand_chado_vars($feature, 'table', 'featureprop');
 * @endcode
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_expand_chado_vars($object, $type, $to_expand, $table_options = array()) {

  // make sure we have a value
  if (!$object) {
    watchdog('tripal_core', 'Cannot pass non array as arugment, $object, to tripal_core_expand_chado_vars function.', array(), WATCHDOG_ERROR);
    return $object;
  }

  // check to see if we are expanding an array of objects
  if (is_array($object)) {
    foreach ($object as $index => $o) {
      $object[$index] = tripal_core_expand_chado_vars($o, $type, $to_expand);
    }
    return $object;
  }

  // get the base table name
  $base_table = $object->tablename;

  switch ($type) {
    case "field": //--------------------------------------------------------------------------------
      if (preg_match('/(\w+)\.(\w+)/', $to_expand, $matches)) {
        $tablename = $matches[1];
        $fieldname = $matches[2];
        $table_desc = tripal_core_get_chado_table_schema($tablename);
        $values = array();
        foreach ($table_desc['primary key'] as $key) {
          $values[$key] = $object->{$key};
        }
        if ($base_table == $tablename) {
          //get the field
          $results = tripal_core_chado_select($tablename, array($fieldname), $values);
          $object->{$fieldname} = $results[0]->{$fieldname};
          $object->expanded = $to_expand;
        }
        else {
          //We need to recurse -the field is in a nested object
          foreach ((array) $object as $field_name => $field_value) {
            if (is_object($field_value)) {
              $object->{$field_name} = tripal_core_expand_chado_vars(
              $field_value,
                'field',
              $to_expand
              );
            }
          } //end of for each field in the current object
        }
      }
      else {
        watchdog('tripal_core', 'tripal_core_expand_chado_vars: Field (%field) not in the right format. " .
          "It should be <tablename>.<fieldname>', WATCHDOG_ERROR);
      }
      break;
    case "table": //--------------------------------------------------------------------------------
      $foreign_table = $to_expand;

      // don't expand the table it already is expanded
      if (array_key_exists($foreign_table, $object)) {
        return $object;
      }
      $foreign_table_desc = tripal_core_get_chado_table_schema($foreign_table);

      // If it's connected to the base table via a FK constraint
      if ($foreign_table_desc['foreign keys'][$base_table]) {
        foreach ($foreign_table_desc['foreign keys'][$base_table]['columns'] as $left => $right) {
          // if the FK value in the base table is not there then we can't expand it, so just skip it.
          if (!$object->{$right}) {
            continue;
          }

          // generate a new object for this table using the FK values in the base table.
          // if a prepared statement is provided generate a new statement_name so that
          // we don't conflict when we recurse.
          $new_options = $table_options;
/*          
          if (array_key_exists('statement_name', $table_options)) {
             $new_options['statement_name'] = "exp_" . $foreign_table . "_" . substr($left, 0, 2) . substr($right, 0, 2);
          }
*/          
          $foreign_object = tripal_core_generate_chado_var($foreign_table, array($left => $object->{$right}), $new_options);

          // if the generation of the object was successful, update the base object to include it.
          if ($foreign_object) {
            // in the case where the foreign key relationships exists more
            // than once with the same table we want to alter the array structure. rather than
            // add the object with a key of the table name, we will add the FK field name in between
            if (count($foreign_table_desc['foreign keys'][$base_table]['columns']) > 1) {
              if (!is_object($object->{$foreign_table})) {
                $object->{$foreign_table} = new stdClass();
              }
              $object->{$foreign_table}->{$left} = $foreign_object;
              $object->expanded = $to_expand;
            }
            else {
              $object->{$foreign_table} = $foreign_object;
              $object->expanded = $to_expand;
            }
          }
          // if the object returned is NULL then handle that
          else {
            if (count($foreign_table_desc['foreign keys'][$base_table]['columns']) > 1) {
              $object->{$foreign_table}->{$left} = NULL;
            }
            else {
              $object->{$foreign_table} = NULL;
            }
          }
        }
      }
      // if the foreign table is not connected to the base table through a FK constraint
      else {
        // We need to recurse -the table has a relationship to one of the nested objects
        $did_expansion = 0;
        foreach ((array) $object as $field_name => $field_value) {
          // if we have a nested object ->expand the table in it
          if (is_object($field_value)) {
            $did_expansion = 1;
            $object->{$field_name} = tripal_core_expand_chado_vars($field_value, 'table', $foreign_table);
          }
        }
        // if we did not expand this table we should return a message that the foreign table
        // could not be expanded
        if (!$did_expansion) {
          watchdog('tripal_core', 'tripal_core_expand_chado_vars: Could not expand table, %table. It is ',
            'not in a foreign key relationship with the base object nor with any other expanded table. ' .
            'Check the table definition to ensure that a proper foreign key relationship is present.',
            array('%table' => $foreign_table), WATCHDOG_ERROR);
        }
      }
      break;
    case "node": //---------------------------------------------------------------------------------
      //if the node to be expanded is for our base table, then just expand it
      if ($object->tablename == $to_expand) {
        $node = node_load($object->nid);
        if ($node) {
          $object->expanded = $to_expand;
          $node->expandable_fields = $object->expandable_fields;
          unset($object->expandable_fields);
          $node->expandable_tables = $object->expandable_tables;
          unset($object->expandable_tables);
          $node->expandable_nodes = $object->expandable_nodes;
          unset($object->expandable_nodes);
          $node->{$base_table} = $object;
          $object = $node;
        }
        else {
          watchdog('tripal_core', 'tripal_core_expand_chado_vars: No node matches the nid (%nid) supplied.',
            array('%nid' => $object->nid), WATCHDOG_ERROR);
        } //end of if node
      }
      else {
        //We need to recurse -the node to expand is one of the nested objects
        foreach ((array) $object as $field_name => $field_value) {
          if (is_object($field_value)) {
            $object->{$field_name} = tripal_core_expand_chado_vars(
            $field_value,
              'node',
            $to_expand
            );
          }
        } //end of for each field in the current object
      }
      break;
    default:
      watchdog('tripal_core', 'tripal_core_expand_chado_vars: Unrecognized type (%type). Should be one of "field", "table", "node".',
        array('%type' => $type), WATCHDOG_ERROR);
      return FALSE;
  }

  //move extended array downwards-------------------------------------------------------------------
  if (!$object->expanded) {
    //if there's no extended field then go hunting for it
    foreach ( (array)$object as $field_name => $field_value) {
      if (is_object($field_value)) {
        if (isset($field_value->expanded)) {
          $object->expanded = $field_value->expanded;
          unset($field_value->expanded);
        }
      }
    }
  }
  //try again becasue now we might have moved it down
  if ($object->expanded) {
    $expandable_name = 'expandable_' . $type . 's';
    if ($object->{$expandable_name}) {
      $key_to_remove = array_search($object->expanded, $object->{$expandable_name});
      unset($object->{$expandable_name}[$key_to_remove]);
      unset($object->expanded);
    }
    else {
      // if there is an expandable array then we've reached the base object
      // if we get here and don't have anything expanded then something went wrong
      //      watchdog(
      //        'tripal_core',
      //        'tripal_core_expand_chado_vars: Unable to expand the %type %to_expand',
      //        array('%type'=>$type, '%to_expand'=>$to_expand),
      //        WATCHDOG_ERROR
      //      );
    } //end of it we've reached the base object
  }

  return $object;
}

/**
 * Implements hook_exclude_type_by_default()
 *
 * This hooks allows fields of a specified type that match a specified criteria to be excluded by
 * default from any table when tripal_core_generate_chado_var() is called. Keep in mind that if
 * fields are excluded by default they can always be expanded at a later date using
 * tripal_core_expand_chado_vars().
 *
 * Criteria are php strings that evaluate to either TRUE or FALSE. These strings are evaluated using
 * drupal_eval() which suppresses syntax errors and throws watchdog entries of type php. There are
 * also watchdog entries of type tripal_core stating the exact criteria evaluated. Criteria can
 * contain the following tokens:
 *   - &gt;field_name&lt;
 *       Replaced by the name of the field to be excluded
 *   - &gt;field_value&lt;
 *       Replaced by the value of the field in the current record
 * Also keep in mind that if your criteria doesn't contain the &gt;field_value&lt;  token then it will be
 * evaluated before the query is executed and if the field is excluded it won't be included in the
 * query.
 *
 * @return
 *   An array of type => criteria where the type is excluded if the criteria evaluates to TRUE
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_exclude_type_by_default() {
  return array('text' => 'strlen("&gt;field_value&lt; ") > 100');
}

/**
 * Implements hook_exclude_field_from_<tablename>_by_default()
 *
 * This hooks allows fields from a specified table that match a specified criteria to be excluded by
 * default from any table when tripal_core_generate_chado_var() is called. Keep in mind that if
 * fields are excluded by default they can always be expanded at a later date using
 * tripal_core_expand_chado_vars().
 *
 * Criteria are php strings that evaluate to either TRUE or FALSE. These strings are evaluated using
 * drupal_eval() which suppresses syntax errors and throws watchdog entries of type php. There are
 * also watchdog entries of type tripal_core stating the exact criteria evaluated. Criteria can
 * contain the following tokens:
 *   - &gt;field_name&lt;
 *       Replaced by the name of the field to be excluded
 *   - &gt;field_value&lt;
 *       Replaced by the value of the field in the current record
 * Also keep in mind that if your criteria doesn't contain the &gt;field_value&lt;  token then it will be
 * evaluated before the query is executed and if the field is excluded it won't be included in the
 * query.
 *
 * @return
 *   An array of type => criteria where the type is excluded if the criteria evaluates to TRUE
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_exclude_field_from_feature_by_default() {
  return array();
}

/**
 * Use this function instead of pager_query() when selecting a
 * subset of records from a Chado table.
 *
 * @param $query
 *   The SQL statement to execute, this is followed by a variable number of args
 *   used as substitution values in the SQL statement.
 * @param $limit
 *   The number of query results to display per page.
 * @param $element
 *   An optional integer to distinguish between multiple pagers on one page.
 * @param $count_query
 *   An SQL query used to count matching records.
 *
 * @returns
 *   A database query result resource or FALSE if the query was not
 *   executed correctly
 *
 * @ingroup tripal_chado_api
 */
function chado_pager_query($query, $limit, $element, $count_query) {

  // The following code is almost an exact duplicate of the
  // Drupal pager_query function. However, substitions have
  // been made to call chado_query rather than db_query

  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : '';

  // get the SQL query arguments that get substituted into modifiers later.
  $args = func_get_args();
  $args = array_slice($args, 4);
  // Alternative syntax for '...'
  if (isset($args[0]) && is_array($args[0])) {
    $args = $args[0];
  }

  // Construct a count query if none was given.
  if (!isset($count_query)) {
    $count_query = preg_replace(array('/SELECT.*?FROM /As', '/ORDER BY .*/'), array('SELECT COUNT(*) FROM ', ''), $query);
  }

  // Convert comma-separated $page to an array, used by other functions.
  $pager_page_array = explode(',', $page);

  // We calculate the total of pages as ceil(items / limit).
  $pager_total_items[$element] = db_result(chado_query($count_query, $args));
  $pager_total[$element] = ceil($pager_total_items[$element] / $limit);
  $pager_page_array[$element] = max(0, min((int) $pager_page_array[$element], ((int) $pager_total[$element]) - 1));

  return chado_query_range($query, $args, $pager_page_array[$element] * $limit, $limit);
}

/**
 * Use this function instead of db_query_range().
 *
 * @param $sql
 *   The SQL statement to execute, this is followed by a variable number of args
 *   used as substitution values in the SQL statement.
 * @param $args
 *   The SQL arguments
 * @param $from
 *   The first result row to return..
 * @param $count
 *   The maximum number of result rows to return.
 *
 * @returns
 *   A database query result resource or FALSE if the query was not
 *   executed correctly
 *
 * @ingroup tripal_chado_api
 */
function chado_query_range($query, $args, $from, $count) {

  $query .= ' LIMIT ' . (int) $count . ' OFFSET ' . (int) $from;

  return chado_query($query, $args);
}
/**
 * Use this function instead of db_query() to avoid switching databases
 * when making query to the chado database
 *
 * Will use a chado persistent connection if it already exists
 *
 * @param $sql
 *   The sql statement to execute
 *   
 * @param $args
 *   The array of arguments, with the same structure as passed to 
 *   the db_query() function of Drupal.
 *
 * @return
 *   DatabaseStatementInterface A prepared statement object, already executed.
 *
 * @ingroup tripal_chado_api
 */
function chado_query($sql, $args = array()) {
	
  $is_local = tripal_core_is_chado_local();
    
  // if Chado is local to the database then prefix the Chado table
  // names with 'chado'.
  if ($is_local) {
  	$sql = preg_replace('/\n/', '', $sql);  // remove carriage returns
    $sql = preg_replace('/\{(.*?)\}/', 'chado.$1', $sql);
    $results = db_query($sql, $args);
  }
  // if Chado is not local to the Drupal database then we have to 
  // switch to another database
  else {
    $previous_db = tripal_db_set_active('chado') ;
    $results = db_query($sql);
    tripal_db_set_active($previous_db);
  }
  
  return $results;
}

/**
 * Get chado id for a node. E.g, if you want to get 'analysis_id' from the
 * 'analysis' table for a synced 'chado_analysis' node, (the same for
 * organisms and features):
 * $analysis_id = chado_get_id_for_node ('analysis', $node->nid)
 * $organism_id = chado_get_id_for_node ('organism', $node->nid)
 * $feature_id  = chado_get_id_for_node ('feature', $node->nid)
 *
 * @param $table
 * @param $nid
 * 
 * @ingroup tripal_chado_api
 */
function chado_get_id_for_node($table, $nid) {
	$sql = "SELECT " . $table . "_id as id FROM {chado_$table} WHERE nid = :nid";
	$result = db_query($sql, array(':nid' => $nid))->fetchObject();
	
	return $result->id;
}

/**
 *  Get node id for a chado feature/organism/analysis. E.g, if you want to
 *  get the node id for an analysis, use:
 *  $nid = chado_get_node_id ('analysis', $analysis_id)
 *  Likewise,
 *  $nid = chado_get_node_id ('organism', $organism_id)
 *  $nid = chado_get_node_id ('feature', $feature_id)
 *
 *  @ingroup tripal_chado_api
 */
function chado_get_node_id($table, $id) {
	$sql = "SELECT nid FROM {chado_$table} WHERE " . $table . "_id = :" . $table . "_id";
  $result = db_query($sql, array(":" . $table . "_id" => $id))->fetchObject();
  return $results->nid;
}

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
 * @ingroup tripal_chado_api
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
 * @ingroup tripal_chado_api
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
  
  $options = array('statement_name' => 'sel_cvterm_cv');
  $term = tripal_core_chado_select('cvterm', array('cvterm_id'), $values, $options);  
  if (!$term or count($term) == 0) {
    watchdog('tripal_core', "Cannot find property '%prop_name'.",
      array('%prop_name' => $property), WATCHDOG_ERROR);
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

  $options = array('statement_name' => 'ins_' . $basetable . 'prop_' . substr($fkcol, 0, 2) . 'tyvara');
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
 * @ingroup tripal_chado_api
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
 * @ingroup tripal_chado_api
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
 * @ingroup tripal_chado_api
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
 * @ingroup tripal_chado_api
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
 * Start a transaction block. Ensures the use of a persistent chado connection
 */
function tripal_db_start_transaction() {
/*  $connection = tripal_db_persistent_chado();
  if ($connection) {
    chado_query("BEGIN");
    return $connection;
  }
  return FALSE;
*/
}

/**
 * Set a savepoint to roll the current transaction back to if an error is encountered
 */
function tripal_db_set_savepoint_transaction($savepoint, $release = FALSE) {
  // Postgresql requires a savepoint of the same name to be unset before re-use
  if ($release) {
    chado_query("RELEASE SAVEPOINT :savepoint", array(':savepoint' => $savepoint));
  }
  chado_query("SAVEPOINT :savepoint", array(':savepoint' => $savepoint));
}

/**
 * A simple function to commit a database transaction
 *
 * @return nothing
 */
function tripal_db_commit_transaction() {
  chado_query("COMMIT");
}

/**
 * Rollback changes.
 *
 * If $savepoint is NULL then rollback to the beginning of the transaction,
 * Otherwise, rollback to the point at which the named $savepoint was created
 *
 * @param $savepoint
 *   The name of the saved point in the transaction to rollback to
 *
 * @param $commit
 *   The transcation will only be committed if this value is TRUE. The
 *   default is TRUE.
 *
 * @return nothing
 */
function tripal_db_rollback_transaction($savepoint = NULL, $commit = TRUE) {

  if ($savepoint) {
    chado_query("ROLLBACK TO SAVEPOINT :savepoint", array(':savepoint' => $savepoint));
  }
  else {
    chado_query("ROLLBACK");
  }

  if ($commit) {
    tripal_db_commit_transaction();
  }

}

/**
 * Retrieves the schema in an array for the specified custom table.
 *
 * @param $table
 *   The name of the table to create.
 *
 * @return
 *   A Drupal-style Schema API array definition of the table. Returns
 *   FALSE on failure.
 *
 * @ingroup tripal_core_api
 */
function tripal_get_chado_custom_schema($table) {

  $sql = "SELECT schema FROM {tripal_custom_tables} WHERE table_name = :table_name";
  $results = db_query($sql, array(':table_name' => $table));
  $custom = $results->fetchObject();
  if (!$custom) {
    return FALSE;
  }
  else {
    return unserialize($custom->schema);
  }
}

/**
 * Check that any given Chado table exists.  This function
 * is necessary because Drupa's db_table_exists function
 * hardcodes the 'public'
 *
 * @return
 *   TRUE/FALSE depending upon whether it exists
 */
function chado_table_exists($table) {
	global $databases;
	
	$default_db = $databases['default']['default']['database'];

  $sql = "
    SELECT 1
    FROM information_schema.tables 
    WHERE 
      table_name = :table_name AND 
      table_schema = 'chado' AND 
      table_catalog = '$default_db' 
  ";
  $results = db_query($sql, array(':table_name' => $table));
  $exists = $results->fetchObject();
  if (!$exists) {
  	return FALSE;
  }
  return TRUE;
}
/**
 * Check that the Chado schema exists within the local database
 *
 * @return
 *   TRUE/FALSE depending upon whether it exists
 */
function tripal_core_chado_schema_exists() {

  $exists = variable_get('chado_schema_exists', FALSE);
  
  if (!$exists) {
    // This is postgresql-specific code to check the existence of the chado schema
    // @coder-ignore: acting on pg_catalog schema rather then drupal schema therefore, table prefixing does not apply
    $sql = "
      SELECT nspname 
      FROM pg_catalog.pg_namespace 
      WHERE nspname = 'chado'
    ";
    $results = db_query($sql);
    $name = $results->fetchObject();
    if ($name) {
      variable_set('chado_schema_exists', TRUE);
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
  return TRUE;
}
/**
 * Check that any given schema exists
 *
 * @param $schema
 *   The name of the schema to check the existence of
 *
 * @return
 *   TRUE/FALSE depending upon whether or not the schema exists
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_schema_exists($schema) {

  // check that the chado schema now exists
  $sql = "
    SELECT nspname
    FROM pg_namespace
    WHERE 
      has_schema_privilege(nspname, 'USAGE') AND 
      nspname = :nspname
    ORDER BY nspname
  ";
  $results = db_query($sql, array(':nspname' => $schema));
  $name = $results->fetchObject();
  if (strcmp($name->nspname, $schema) != 0) {
    return FALSE;
  }

  return TRUE;
}

/**
 * Retrieves the list tables in the Chado schema.  By default it only retursn
 * the default Chado tables, but may also return custom tables added to the
 * Chado schema as well.
 *
 * @param $include_custom
 *   Optional.  Set as TRUE to include any custom tables created in the
 *   Chado schema. Custom tables are added to Chado using the
 *   tripal_core_chado_create_table() function.
 *
 * @returns
 *   An associative array where the key and value pairs are the Chado table names.
 *
 * @ingroup tripal_core_api
 */
function tripal_core_get_chado_tables($include_custom = NULL) {


  // first get the chado version that is installed
  $v = tripal_core_get_chado_version();

  $tables = array();
  if ($v == '1.2') {
    $tables_v1_2 = tripal_core_chado_get_v1_2_tables();
    foreach ($tables_v1_2 as $table) {
      $tables[$table] = $table;
    }
  }
  if ($v == '1.11' or $v == '1.11 or older') {
    $tables_v1_11 = tripal_core_chado_get_v1_11_tables();
    foreach ($tables_v1_11 as $table) {
      $tables[$table] = $table;
    }
  }

  // now add in the custom tables too if requested
  if ($include_custom) {
    $sql = "SELECT table_name FROM {tripal_custom_tables}";
    $resource = db_query($sql);

    foreach ($resource as $r) {
      $tables[$r->table_name] = $r->table_name;
    }
  }

  asort($tables);
  return $tables;
}
/**
 * Sets a Drupal variable with the current version of Chado.  This variable
 * can then be queried later using the tripal_core_get_chado_Version
 *
 * @returns
 *   The version of Chado
 *
 * @ingroup tripal_core_api
 */
function tripal_core_set_chado_version() {
  global $databases;
  $is_local = 0;  

  // check that Chado is installed if not return 'uninstalled as the version'
  $chado_exists = tripal_core_chado_schema_exists();
  if (!$chado_exists) {  	
    // if it's not in the drupal database check to see if it's specified in the $db_url
    // in the settings.php    
    if (!array_key_exists('chado', $databases)) {    	
      // if it's not in the drupal database or specified in the $db_url then
      // return uninstalled as the version
      return 'not installed';
    }
    $previous_db = tripal_db_set_active('chado');
    $prop_exists = db_table_exists('chadoprop');
    tripal_db_set_active($previous_db);
  }
  else {
  	$is_local = 1;
  	$prop_exists = db_table_exists('chado.chadoprop');
  }
  
  // if the table doesn't exist then we don't know what version but we know
  // it must be 1.11 or older.
  if (!$prop_exists) {
    return "1.11 or older";
  }

  $sql = "
    SELECT value 
    FROM {chadoprop} CP 
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = CP.type_id 
      INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id 
    WHERE CV.name = 'chado_properties' and CVT.name = 'version'
  ";
  if (!$is_local) {
	  $previous_db = tripal_db_set_active('chado');
	  $results = db_query($sql);
	  tripal_db_set_active($previous_db);
  }
  else {
    $results = chado_query($sql);
  }
  $v = $results->fetchObject();

  // if we don't have a version in the chadoprop table then it must be
  // v1.11 or older
  if (!$v) {
    variable_set('chado_version', "1.11 or older");
    return "1.11 or older";
  }

  variable_set('chado_version', $v->value);
  return $v->value;
}
/**
 * Returns the version number of the currently installed Chado instance.
 * It can return the real or effective version.
 *
 * @param $exact
 *   Set this argument to 1 to retrieve the exact version that is installed.
 *   Otherwise, this function will set the version to the nearest 'tenth'.
 *   Chado versioning numbers in the hundreds represent changes to the
 *   software and not the schema.  Changes in the tenth's represent changes
 *   in the schema.
 *
 * @param $warn_if_unsupported
 *   If the currently installed version of Chado is not supported by Tripal
 *   the generatea a Drupal warning.
 *
 * @returns
 *   The version of Chado
 *
 * @ingroup tripal_core_api
 */
function tripal_core_get_chado_version($exact = FALSE, $warn_if_unsupported = FALSE) {
  // first get the chado version that is installed
  $exact_version = variable_get('chado_version', '');
  if (!$exact_version) {
    $exact_version = tripal_core_set_chado_version();
  }

  // Tripal only supports v1.11 or newer.. really this is the same as v1.1
  // but at the time the v1.11 schema API was written we didn't know that so
  // we'll return the version 1.11 so the schema API will work.
  if (strcmp($exact_version, '1.11 or older') == 0) {
    $exact_version = "1.11";
    if ($warn_if_unsupported) {
      drupal_set_message(t("WARNING: Tripal does not fully support Chado version less than v1.11.  If you are certain this is v1.11
         or if Chado was installed using an earlier version of Tripal then all is well. If not please upgrade to v1.11 or later"),
         'warning');
    }
  }

  // if not returing an exact version, return the version to the nearest 10th.
  // return 1.2 for all versions of 1.2x
  $effective_version = $exact_version;
  if (preg_match('/^1\.2\d+$/', $effective_version)) {
    $effective_version = "1.2";
  }
  if ($warn_if_unsupported and ($effective_version != 1.11 and $effective_version != 1.2 and $effective_version != 'not installed')) {
    drupal_set_message(t("WARNING: The currently installed version of Chado, v$exact_version, is not fully compatible with Tripal."), 'warning');
  }
  // if the callee has requested the exact version then return it
  if ($exact) {
    return $exact_version;
  }

  return $effective_version;
}
/**
 * Retrieves the chado tables Schema API array.
 *
 * @param $table
 *   The name of the table to retrieve.  The function will use the appopriate
 *   Tripal chado schema API hooks (e.g. v1.11 or v1.2).
 *
 * @returns
 *   A Drupal Schema API array defining the table.
 *
 * @ingroup tripal_core_api
 */
function tripal_core_get_chado_table_schema($table) {

  // first get the chado version that is installed
  $v = tripal_core_get_chado_version();

  // get the table array from the proper chado schema
  $v = preg_replace("/\./", "_", $v); // reformat version for hook name
  $table_arr = module_invoke_all("chado_schema_v" . $v . "_" . $table);

  // if the table_arr is empty then maybe this is a custom table
  if (!is_array($table_arr) or count($table_arr) == 0) {
    $table_arr = tripal_get_chado_custom_schema($table);
  }

  return $table_arr;
}
/**
 * This function will delete Drupal nodes for any sync'ed table (e.g.
 * feature, organism, analysis, stock, library) if the chado record has been
 * deleted or the entry in the chado_[table] table has been removed.
 *
 * @param $table
 *   The name of the table that corresonds to the node type we want to clean up.
 * @param $job_id
 *   This should be the job id from the Tripal jobs system.  This function
 *   will update the job status using the provided job ID.
 *
 * @ingroup tripal_core_api
 */
function tripal_core_clean_orphaned_nodes($table, $job_id) {
  $count = 0;

  // build the SQL statments needed to check if nodes point to valid analyses
  $dsql = "SELECT * FROM {node} WHERE type = 'chado_" . $table . "' order by nid";
  $nsql = "SELECT * FROM {node} WHERE nid = :nid";
  $csql = "SELECT * FROM {chado_" . $table . "} WHERE nid = :nid ";
  $clsql= "SELECT * FROM {chado_" . $table . "}";
  $lsql = "SELECT * FROM {" . $table . "} where " . $table . "_id = :" . $table . "_id ";

  // load into nodes array
  print "Getting nodes\n";
  $nodes = array();
  $res = db_query($dsql);
  foreach ($res as $node) {  
    $nodes[$count] = $node;
    $count++;
  }

  // load the chado_$table into an array
  print "Getting chado_$table\n";
  $cnodes = array();
  $res = db_query($clsql);
  foreach ($res as $node) {
    $cnodes[$count] = $node;
    $count++;
  }
  $interval = intval($count * 0.01);
  if ($interval < 1) {
    $interval = 1;
  }

  // iterate through all of the chado_$table entries and remove those
  // that don't have a node or don't have a $table record in chado.libary
  print "Verifying all chado_$table Entries\n";
  $deleted = 0;
  foreach ($cnodes as $nid) {

    // update the job status every 1% analyses
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id, intval(($i / $count) * 100));
    }

    // see if the node exits, if not remove the entry from the chado_$table table
    $results = db_query($nsql, array(':nid' => $nid->nid));
    $node = $results->fetchObject();
    if (!$node) {
      $deleted++;
      db_query("DELETE FROM {chado_" . $table . "} WHERE nid = :nid", array(':nid' => $nid->nid));
      $message = "chado_$table missing node.... DELETING: $nid->nid";
      watchdog('tripal_core', $message, array(), WATCHDOG_WARNING);
    }

    // see if the record in chado exist, if not remove the entry from the chado_$table
    $table_id = $table . "_id";
    $results = chado_query($lsql, array(":" . $table . "_id " => $nid->$table_id));
    $record = $results->fetchObject();
    if (!$record) {
      $deleted++;
      $sql = "DELETE FROM {chado_" . $table . "} WHERE " . $table . "_id = :" . $table . "_id";
      db_query($sql, array(":" . $table . "_id" => $nid->$table_id));
      $message = "chado_$table missing $table.... DELETING entry.";
      watchdog('tripal_core', $message, array(), WATCHDOG_WARNING);
    }
    $i++;
  }
  print "\t$deleted chado_$table entries missing either a node or chado entry.\n";

  // iterate through all of the nodes and delete those that don't
  // have a corresponding entry in chado_$table
  $deleted = 0;
  foreach ($nodes as $node) {

    // update the job status every 1% libraries
    if ($job_id and $i % $interval == 0) {
      tripal_job_set_progress($job_id, intval(($i / $count) * 100));
    }

    // check to see if the node has a corresponding entry
    // in the chado_$table table. If not then delete the node.
    $results = db_query($csql, array(":nid" => $node->nid));
    $link = $results->fetchObject();
    if (!$link) {
      if (node_access('delete', $node)) {
        $deleted++;
        $message = "Node missing in chado_$table table.... DELETING node $node->nid";
        watchdog("tripal_core", $message, array(), WATCHDOG_WARNING);
        node_delete($node->nid);
      }
      else {
        $message = "Node missing in chado_$table table.... but cannot delete due to improper permissions (node $node->nid)";
        watchdog("tripal_core", $message, array(), WATCHDOG_WARNING);
      }
    }
    $i++;
  }
  print "\t$deleted nodes did not have corresponding chado_$table entries.\n";

  return '';
}

/**
 * Check whether chado is installed (either in the same or a different database)
 *
 * @return
 *   TRUE/FALSE depending upon whether chado is installed.
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_is_chado_installed() {
  global $databases;

  // first check if chado is in the $databases variable of the settings.php file
  if (array_key_exists('chado', $databases)) {
    return TRUE;
  }

  // check to make sure the chado schema exists
  return tripal_core_chado_schema_exists();
}

/**
 * Check whether chado is installed local to the Drupal database
 * in its own Chado schema.
 *
 * @return
 *   TRUE/FALSE depending upon whether chado is local.
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_is_chado_local() {
  global $db_url, $db_type;

  $is_installed = tripal_core_is_chado_installed();
  if ($is_installed) {
    if (is_array($db_url)) {
      if (isset($db_url['chado'])) {
        return FALSE;
      }
    }
    return TRUE;
  }
  return FALSE;
}

/**
 * Determine whether a given chado table is directly linked to a node
 *
 * @param $chado_table
 *   The name of a chado table to check (ie: feature)
 * @return
 *   TRUE if it is linked to a node and FALSE otherwise
 */
function tripal_core_is_tripal_node_type($chado_table) {
  $linking_table = 'chado_' . $chado_table;
  if (db_table_exists($linking_table)) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Set the Tripal Database
 *
 * The tripal_db_set_active function is used to prevent namespace collisions
 * when chado and drupal are installed in the same database but in different
 * schemas.  It is also used for backwards compatibility with older versions
 * of tripal or in cases where chado is located outside of the Drupal database.
 * or when using Drupal functions such as db_table_exists()
 *
 * @ingroup tripal_chado_api
 */
function tripal_db_set_active($dbname  = 'default') {
  global $databases, $active_db;
  
  $chado_exists = variable_get('chado_schema_exists', FALSE);
  if ($chado_exists) {
    if($dbname == 'chado') {
      db_query('set search_path to chado');
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