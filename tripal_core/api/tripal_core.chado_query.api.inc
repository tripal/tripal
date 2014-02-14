<?php
/**
 * @file
 * Provides an API for querying of chado including inserting, updating, deleting and
 * selecting from chado.
 */

/**
 * @defgroup tripal_chado_query_api Chado Query API
 * @ingroup tripal_chado_api
 * @{
 * Provides an API for querying of chado including inserting, updating, deleting and
 * selecting from specific chado tables. There is also a generic function, chado_query(),
 * to execute and SQL statement on chado. It is ideal to use these functions to interact
 * with chado in order to keep your module compatible with both local & external chado
 * databases. Furthermore, it ensures connection to the chado database is taken care
 * of for you.
 *
 * Generic Queries to a specifc chado table:
 *
 * chado_select_record( [table name], [columns to select], [specify record to select], [options*] )
 * This function allows you to select various columns from the specified chado table. Although
 * you can only select from a single table, you can specify the record to select using values
 * from related tables through use of a nested array. For example, the following code shows
 * you how to select the name and uniquename of a feature based on it's type and source
 * organism.
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => 'sinensis',
 *      ),
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = chado_select_record(
 *      'feature',                      // table to select from
 *      array('name', 'uniquename'),    // columns to select
 *      $values                         // record to select (see variable defn. above)
 *   );
 * @endcode
 *
 * chado_insert_record( [table name], [values to insert], [options*] )
 * This function allows you to insert a single record into a specific table. The values to
 * insert are specified using an associative array where the keys are the column names to
 * insert into and they point to the value to be inserted into that column. If the column
 * is a foreign key, the key will point to an array specifying the record in the foreign
 * table and then the primary key of that record will be inserted in the column. For example,
 * the following code will insert a feature and for the type_id, the cvterm.cvterm_id of
 * the cvterm record will be inserted and for the organism_id, the organism.organism_id
 * of the organism_record will be inserted.
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
 *   $result = chado_insert_record(
 *     'feature',             // table to insert into
 *     $values                // values to insert
 *   );
 * @endcode
 *
 * chado_update_record( [table name], [specify record to update], [values to change], [options*] )
 * This function allows you to update records in a specific chado table. The record(s)
 * you wish to update are specified the same as in the select function above and
 * the values to be update are specified the same as the values to be inserted were. For
 * example, the following code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be updated with a new name,
 * and the type changed from a gene to an mRNA.
 * @code
 * $umatch = array(
 *   'organism_id' => array(
 *     'genus' => 'Citrus',
 *     'species' => 'sinensis',
 *   ),
 *   'uniquename' => 'orange1.1g000034m.g7',
 *   'type_id' => array (
 *     'cv_id' => array (
 *       'name' => 'sequence',
 *     ),
 *     'name' => 'gene',
 *     'is_obsolete' => 0
 *   ),
 * );
 * $uvalues = array(
 *   'name' => 'orange1.1g000034m.g',
 *   'type_id' => array (
 *     'cv_id' => array (
 *       'name' => 'sequence',
 *     ),
 *     'name' => 'mRNA',
 *     'is_obsolete' => 0
 *   ),
 * );
 *   $result = chado_update_record('feature',$umatch,$uvalues);
 * @endcode
 *
 * chado_delete_record( [table name], [specify records to delete], [options*] )
 * This function allows you to delete records from a specific chado table. The record(s)
 * to delete are specified the same as the record to select/update was above. For example,
 * the following code will delete all genes from the organism Citrus sinensis.
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => 'sinensis',
 *      ),
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = chado_select_record(
 *      'feature',                      // table to select from
 *      $values                         // records to delete (see variable defn. above)
 *   );
 * @endcode
 *
 * Generic Queries for any SQL:
 *
 * Often it is necessary to select from more then one table in chado or to execute
 * other complex queries that cannot be handled efficiently by the above functions. It is
 * for this reason that the chado_query( [sql string], [arguments to sub-in to the sql] )
 * function was created. This function allows you to execute any SQL directly on the
 * chado database and should be used with care. If any user input will be used in the query
 * make sure to put a placeholder in your SQL string and then define the value in the
 * arguments array. This will make sure that the user input is santized and safe through
 * type-checking and escaping. The following code shows an example of how to use user input
 * resulting from a form and would be called withing the form submit function.
 * @code
 * $sql = "SELECT F.name, CVT.name as type_name, ORG.common_name
 *          FROM feature F
 *          LEFT JOIN cvterm CVT ON F.type_id = CVT.cvterm_id
 *          LEFT JOIN organism ORG ON F.organism_id = ORG.organism_id
 *          WHERE
 *            F.uniquename = :feature_uniquename";
 * $args = array( ':feature_uniquename' => $form_state['values']['uniquename'] );
 * $result = chado_query( $sql, $args );
 * foreach ($result as $r) { [Do something with the records here] }
 * @endcode
 *
 * If you are going to need more then a couple fields, you might want to use the
 * Chado Variables API (specifically chado_generate_var()) to select all
 * of the common fields needed including following foreign keys.
 *
 * Loading of Variables from chado data:
 *
 * These functions, chado_generate_var() and  chado_expand_var(), generate objects
 * containing the full details of a record(s) in chado. These should be used in all
 * theme templates.
 *
 * This differs from the objects returned by chado_select_record in so far as all foreign key
 * relationships have been followed meaning you have more complete details. Thus this function
 * should be used whenever you need a full variable and chado_select_record should be used if
 * you only case about a few columns.
 *
 * The initial variable is generated by the
 * chado_generate_var([table], [filter criteria], [optional options])
 * function. An example of how to use this function is:
 * @code
   $values = array(
     'name' => 'Medtr4g030710'
   );
   $features = chado_generate_var('feature', $values);
 * @endcode
 * This will return an object if there is only one feature with the name Medtr4g030710 or it will
 * return an array of feature objects if more than one feature has that name.
 *
 * Some tables and fields are excluded by default. To have those tables & fields added to
 * your variable you can use the
 * chado_expand_var([chado variable], [type], [what to expand], [optional options])
 * function. An example of how to use this function is:
 * @code
   // Get a chado object to be expanded
   $values = array(
     'name' => 'Medtr4g030710'
   );
   $features = chado_generate_var('feature', $values);
   // Expand the organism node
   $feature = chado_expand_var($feature, 'node', 'organism');
   // Expand the feature.residues field
   $feature = chado_expand_var($feature, 'field', 'feature.residues');
   // Expand the feature properties (featureprop table)
   $feature = chado_expand_var($feature, 'table', 'featureprop');
 * @endcode
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
 *   $result = chado_insert_record('feature',$values);
 * @endcode
 * The above code inserts a record into the feature table.  The $values array is
 * nested such that the organism is selected by way of the organism_id foreign
 * key constraint by specifying the genus and species.  The cvterm is also
 * specified using its foreign key and the cv_id for the cvterm is nested as
 * well.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_insert_record($table, $values, $options = array()) {

  $print_errors = (isset($options['print_errors'])) ? $options['print_errors'] : FALSE;

  if (!is_array($values)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass non array as values for inserting.', array(),
      array('print' => $print_errors)
    );
    return FALSE;
  }
  if (count($values)==0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass an empty array as values for inserting.',
      array(),array('print' => $print_errors)
    );
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
  $table_desc = chado_get_schema($table);
  if (empty($table_desc)) {
    tripal_report_error('tripal_core', TRIPAL_WARNING,
      'chado_insert_record; There is no table description for !table_name',
      array('!table_name' => $table), array('print' => $print_errors)
    );
  }

  // iterate through the values array and create a new 'insert_values' array
  // that has all the values needed for insert with all foreign relationsihps
  // resolved.
  foreach ($values as $field => $value) {
    // make sure the field is in the table description. If not then return an error
    // message
    if (!array_key_exists($field, $table_desc['fields'])) {
      tripal_report_error('tripal_core', TRIPAL_ERROR,
        "chado_insert_record; The field '%field' does not exist " .
          "for the table '%table'.  Cannot perform insert. Values: %array",
        array('%field' => $field, '%table' => $table, '%array' => print_r($values, 1)),
        array('print' => $print_errors)
      );
      return FALSE;
    }

    if (is_array($value)) {
      // select the value from the foreign key relationship for this value
      $results = chado_schema_get_foreign_key($table_desc, $field, $value);

      if (sizeof($results) > 1) {
        tripal_report_error('tripal_core', TRIPAL_ERROR,
          'chado_insert_record: Too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
          array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)),
          array('print' => $print_errors)
        );
      }
      elseif (sizeof($results) < 1) {
        tripal_report_error('tripal_core', TRIPAL_DEBUG,
          'chado_insert_record: no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)',
          array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)),
          array('print' => $print_errors)
        );
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
        if (chado_select_record($table, $ukselect_cols, $ukselect_vals)) {
          tripal_report_error('tripal_core', TRIPAL_ERROR,
            "chado_insert_record; Cannot insert duplicate record into $table table: !values",
            array('!values' => print_r($values, TRUE)), array('print' => $print_errors)
          );
          return FALSE;
        }
      }
    }

    // if trying to insert a field that is the primary key, make sure it also is unique
    if (array_key_exists('primary key', $table_desc)) {
      $pkey = $table_desc['primary key'][0];
      if (array_key_exists($pkey, $insert_values)) {
        $coptions = array();
        if (chado_select_record($table, array($pkey), array($pkey => $insert_values[$pkey]), $coptions)) {
          tripal_report_error('tripal_core', TRIPAL_ERROR,
            'chado_insert_record; Cannot insert duplicate primary key into !table table: !values',
            array('!table' => $table, '!values' => print_r($values, TRUE)),
            array('print' => $print_errors)
          );
          return FALSE;
        }
      }
    }

    // make sure required fields have a value
    if (!is_array($table_desc['fields'])) {
      $table_desc['fields'] = array();
      tripal_report_error('tripal_core', TRIPAL_WARNING,
        "chado_insert_record; %table missing fields: \n %schema",
        array('%table' => $table, '%schema' => print_r($table_desc, 1)),
        array('print' => $print_errors)
      );
    }
    foreach ($table_desc['fields'] as $field => $def) {
      // a field is considered missing if it cannot be NULL and there is no default
      // value for it or it is of type 'serial'
      if (array_key_exists('NOT NULL', $def) and
          !array_key_exists($field, $insert_values) and
          !array_key_exists('default', $def) and
          strcmp($def['type'], serial) != 0) {
        tripal_report_error('tripal_core', TRIPAL_ERROR,
          "chado_insert_record; Field %table.%field cannot be NULL: %values",
          array('%table' => $table, '%field' => $field, '%values' => print_r($values, 1)),
          array('print' => $print_errors)
        );
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
        $value = $results->fetchField();
        if (!$value) {
          tripal_report_error('tripal_core', TRIPAL_ERROR,
            "chado_insert_record; not able to retrieve primary key after insert: %sql",
            array('%sql' => $sql),
            array('print' => $print_errors)
          );
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
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'chado_insert_record; Cannot insert record into "%table": %values',
      array('%table' => $table, '%values' => print_r($values, 1)),
      array('print' => $print_errors)
    );
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
 *   $result = chado_update_record('feature',$umatch,$uvalues);
 * @endcode
 * The above code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be updated.
 * The organism_id is specified as a nested array that uses the organism_id
 * foreign key constraint to lookup the specified values to find the exact
 * organism_id. The same nested struture is also used for specifying the
 * values to update.  The function will find the record that matches the
 * columns specified and update the record with the avlues in the $uvalues array.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_update_record($table, $match, $values, $options = NULL) {

  $print_errors = (isset($options['print_errors'])) ? $options['print_errors'] : FALSE;

  if (!is_array($values)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass non array as values for updating.',
      array(), array('print' => $print_errors)
    );
    return FALSE;
  }
  if (count($values)==0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass an empty array as values for updating.',
      array(), array('print' => $print_errors)
    );
    return FALSE;
  }

  if (!is_array($match)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass non array as values for matching.',
      array(), array('print' => $print_errors)
    );
    return FALSE;
  }
  if (count($match)==0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass an empty array as values for matching.',
      array(), array('print' => $print_errors)
    );
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
  $table_desc = chado_get_schema($table);

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
      $options2 = array();
      $results = chado_select_record($table, $columns, $match, $options2);
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
      $results = chado_schema_get_foreign_key($table_desc, $field, $value);
      if (sizeof($results) > 1) {
        tripal_report_error('tripal_core', TRIPAL_ERROR,
          'chado_update_record: When trying to find record to update, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
          array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)),
          array('print' => $print_errors)
        );
      }
      elseif (sizeof($results) < 1) {
        tripal_report_error('tripal_core', TRIPAL_DEBUG,
          'chado_update_record: When trying to find record to update, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)',
          array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)),
          array('print' => $print_errors)
        );
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
      $results = chado_schema_get_foreign_key($table_desc, $field, $value, $foreign_options);
      if (sizeof($results) > 1) {
        tripal_report_error('tripal_core', TRIPAL_ERROR,
          'chado_update_record: When trying to find update values, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
          array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)),
          array('print' => $print_errors)
        );
      }
      elseif (sizeof($results) < 1) {
        tripal_report_error('tripal_core', TRIPAL_DEBUG,
          'chado_update_record: When trying to find update values, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)',
          array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)),
          array('print' => $print_errors)
        );
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
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      "chado_update_record: Cannot update record in %table table.  \nMatch: %match \nValues: %values",
      array('%table' => table, '%match' => print_r($match,TRUE), '%values' => print_r($values, 1)),
      array('print' => $print_errors)
    );
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
 *   $result = chado_update_record('feature', $umatch, $uvalues);
 * @endcode
 * The above code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be deleted.
 * The organism_id is specified as a nested array that uses the organism_id
 * foreign key constraint to lookup the specified values to find the exact
 * organism_id. The same nested struture is also used for specifying the
 * values to update.  The function will find all records that match the
 * columns specified and delete them.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_delete_record($table, $match, $options = NULL) {

  if (!is_array($match)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass non array as values for matching.', array());
    return FALSE;
  }
  if (count($match)==0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass an empty array as values for matching.', array());
    return FALSE;
  }

  // set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements
  if (!is_array($options)) {
    $options = array();
  }

  $delete_matches = array();  // contains the values for the where clause

  // get the table description
  $table_desc = chado_get_schema($table);
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
        $results = chado_schema_get_foreign_key($table_desc, $field, $value);
        if (sizeof($results) > 1) {
          tripal_report_error('tripal_core', TRIPAL_ERROR,
            'chado_delete_record: When trying to find record to delete, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
            array('!foreign_key' => $field, '!criteria' => print_r($value, TRUE)));
        }
        elseif (sizeof($results) < 1) {
          //tripal_report_error('tripal_core', TRIPAL_ERROR, 'chado_delete_record: When trying to find record to delete, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)));
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
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      "Cannot delete record in $table table.  Match:" . print_r($match, 1) . ". Values: " . print_r($values, 1), array());
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
 *  - is_duplicate: TRUE or FALSE.  Checks the values submited to see if
 *     they violate any of the unique constraints. If so, the record
 *     is returned, if not, FALSE is returned.
 *  - pager:  Use this option if it is desired to return only a subset of results
 *     so that they may be shown with in a Drupal-style pager. This should be
 *     an array with two keys: 'limit' and 'element'.  The value of 'limit'
 *     should specify the number of records to return and 'element' is a
 *     unique integer to differentiate between pagers when more than one
 *     appear on a page.  The 'element' should start with zero and increment by
 *     one for each pager.
 *
 * @return
 *  An array of results, FALSE if the query was not executed
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
 *     'order_by' => array(
 *        'name' => 'ASC'
 *     ),
 *   );
 *   $result = chado_select_record('feature',$columns,$values,$options);
 * @endcode
 * The above code selects a record from the feature table using the three fields
 * that uniquely identify a feature.  The $columns array simply lists the columns
 * to select. The $values array is nested such that the organism is identified by
 * way of the organism_id foreign key constraint by specifying the genus and
 * species.  The cvterm is also specified using its foreign key and the cv_id
 * for the cvterm is nested as well.  In the example above, two different species
 * are allowed to match
 *
 * @ingroup tripal_chado_query_api
 */
function chado_select_record($table, $columns, $values, $options = NULL) {

  $print_errors = (isset($options['print_errors'])) ? $options['print_errors'] : FALSE;

  if (!is_array($values)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, 'Cannot pass non array as values for selecting.',
      array(), array('print' => $print_errors)
    );
    return FALSE;
  }
  if (!is_array($columns)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, 'Cannot pass non array as columns for selecting.',
      array(), array('print' => $print_errors)
    );
    return FALSE;
  }
  if (count($columns)==0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR, 'Cannot pass an empty array as columns for selecting.',
      array(), array('print' => $print_errors)
    );
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
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'chado_select_record; the $columns argument must be an array. Columns:%columns',
      array('%columns' => print_r($columns, TRUE)),
      array('print' => $print_errors)
    );
    return FALSE;
  }
  if (!is_array($values)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'chado_select_record; the $values argument must be an array. Values:%values',
      array('%values' => print_r($values, TRUE)),
      array('print' => $print_errors)
    );
    return FALSE;
  }

  // get the table description
  $table_desc = chado_get_schema($table);
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
          tripal_report_error('tripal_core', TRIPAL_ERROR,
            'chado_select_record: There is no value for %field thus we cannot check if this record is unique',
            array('%field' => $field), array('print' => $print_errors));
          return FALSE;
        }
      }

      $results = chado_select_record($table, $new_columns, $new_values, $new_options);
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
      tripal_report_error('tripal_core', TRIPAL_ERROR,
        'chado_select_record: The field "%field" does not exist for the table "%table".  Cannot perform query. Values: %array',
        array('%field' => $field, '%table' => $table, '%array' => print_r($values, 1)),
        array('print' => $print_errors)
      );
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

        $results = chado_schema_get_foreign_key($table_desc, $field, $value, $foreign_options);
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

  // if the caller has requested the SQL rather than the results then do so
  if ($options['return_sql'] == TRUE) {
    return array('sql' => $sql, 'args' => $args);
  }
  if (array_key_exists('limit', $pager)) {
    $total_records = 0;
    $resource = chado_pager_query($sql, $args, $pager['limit'], $pager['element'], NULL, $total_records);
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
 * Example usage:
 * @code
 * $sql = "SELECT F.name, CVT.name as type_name, ORG.common_name
 *          FROM feature F
 *          LEFT JOIN cvterm CVT ON F.type_id = CVT.cvterm_id
 *          LEFT JOIN organism ORG ON F.organism_id = ORG.organism_id
 *          WHERE
 *            F.uniquename = :feature_uniquename";
 * $args = array( ':feature_uniquename' => $form_state['values']['uniquename'] );
 * $result = chado_query( $sql, $args );
 * foreach ($result as $r) { [Do something with the records here] }
 * @endcode
 *
 * @ingroup tripal_chado_query_api
 */
function chado_query($sql, $args = array()) {
  $is_local = $GLOBALS["chado_is_local"];

  // Args should be an array
  if (!is_array($args)) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'chado_query; Need to pass an array to chado_query, "%value" passed instead. Query: %query',
      array('%value' => $args, '%query' => $sql)
    );
    $args = array($args);
    return FALSE;
  }

  // if Chado is local to the database then prefix the Chado table
  // names with 'chado'.
  if ($is_local) {
    $sql = preg_replace('/\n/', '', $sql);  // remove carriage returns
    $sql = preg_replace('/\{(.*?)\}/', 'chado.$1', $sql);

    // the featureloc table has some indexes that use function that call other functions
    // and those calls do not reference a schema, therefore, any tables with featureloc
    // must automaticaly have the chado schema set as active to find
    if(preg_match('/chado.featureloc/i', $sql)) {
      $previous_db = chado_set_active('chado') ;
      $results = db_query($sql, $args);
      chado_set_active($previous_db);
    }
    // for all other tables we should have everything in scope so just run the query
    else {
      $results = db_query($sql, $args);
    }
  }
  // if Chado is not local to the Drupal database then we have to
  // switch to another database
  else {
    $previous_db = chado_set_active('chado') ;
    $results = db_query($sql, $args);
    chado_set_active($previous_db);
  }

  return $results;
}

/**
 * Use this function instead of pager_query() when selecting a
 * subset of records from a Chado table.
 *
 * @param $query
 *   The SQL statement to execute, this is followed by a variable number of args
 *   used as substitution values in the SQL statement.
 * @param $args
 *   The array of arguments for the query. They keys are the placeholders
 * @param $limit
 *   The number of query results to display per page.
 * @param $element
 *   An numeric identifier used to distinguish between multiple pagers on one page.
 * @param $count_query
 *   An SQL query used to count matching records.
 *
 * @returns
 *   A database query result resource or FALSE if the query was not
 *   executed correctly
 *
 * @ingroup tripal_chado_query_api
 */
function chado_pager_query($query, $args, $limit, $element, $count_query = '') {
  // get the page and offset for the pager
  $page = isset($_GET['page']) ? $_GET['page'] : '0';
  $offset = $limit * $page;
  $q = $_GET['q'];

  // Construct a count query if none was given.
  if (!isset($count_query)) {
    $count_query = preg_replace(array('/SELECT.*?FROM /As', '/ORDER BY .*/'),
      array('SELECT COUNT(*) FROM ', ''), $query);
  }

  // We calculate the total of pages as ceil(items / limit).
  $results = chado_query($count_query, $args);
  if (!$results) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      "chado_pager_query(): Query failed: %cq", array('%cq' => $count_query));
    return;
  }
  $total_records = $results->fetchField();

  // set a session variable for storing the total number of records
  $GLOBALS['chado_pager'][$q][$element]['total_records'] = $total_records;

  pager_default_initialize($total_records, $limit, $element);

  $query .= ' LIMIT ' . (int) $limit . ' OFFSET ' . (int) $offset;
  $results = chado_query($query, $args);
  return $results;
}
/**
 * A function to retrieve the total number of records for a pager that
 * was generated using the chado_pager_query() function
 * 
 * @param $element
 *   The $element argument that was passed to the chado_pager_query function
 *   
 * @ingroup tripal_chado_query_api
 */
function chado_pager_get_count($element) {
  $q = $_GET['q'];
  
  if (array_key_exists($q, $GLOBALS['chado_pager']) and 
      array_key_exists($element, $GLOBALS['chado_pager'][$q])) {
    return $GLOBALS['chado_pager'][$q][$element]['total_records'];
  }
  else {
    return 0;
  }
}

/**
 * Gets the value of a foreign key relationship
 *
 * This function is used by chado_select_record, chado_insert_record,
 * and chado_update_record to iterate through the associate array of
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
 *  and the value is the value of that option. These options are passed on to chado_select_record.
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
 *   $value = chado_schema_get_foreign_key('feature', 'organism_id',$values);
 *
 * @endcode
 * The above code selects a record from the feature table using the three fields
 * that uniquely identify a feature.  The $columns array simply lists the columns
 * to select. The $values array is nested such that the organism is identified by
 * way of the organism_id foreign key constraint by specifying the genus and
 * species.  The cvterm is also specified using its foreign key and the cv_id
 * for the cvterm is nested as well.
 *
 * @ingroup tripal_core
 */
function chado_schema_get_foreign_key($table_desc, $field, $values, $options = NULL) {

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
        tripal_report_error('tripal_core', $message);
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
          $result = chado_select_record($table, $select_cols, $values, $options);
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
    // @todo: what do we do if we get to this point and we have a fk
    // relationship expected but we don't have any definition for one in the
    // table schema??
    $version = $GLOBALS["chado_version"];
    $message = t("There is no foreign key relationship defined for " . $field . " .
       To define a foreign key relationship, determine the table this foreign
       key referrs to (<foreign table>) and then implement
       hook_chado_chado_schema_v<version>_<foreign table>(). See
       tripal_feature_chado_v1_2_schema_feature for an example. Chado version: $version");
    tripal_report_error('tripal_core', $message);
    drupal_set_message(check_plain($message), 'error');
  }

  return array();
}