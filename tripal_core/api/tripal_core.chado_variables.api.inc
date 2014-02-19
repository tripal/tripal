<?php
/**
 * @file
 * This API generates objects containing the full details of a record(s) in chado.
 */

/**
 * Implements hook_exclude_type_by_default()
 *
 * This hooks allows fields of a specified type that match a specified criteria to be excluded by
 * default from any table when chado_generate_var() is called. Keep in mind that if
 * fields are excluded by default they can always be expanded at a later date using
 * chado_expand_var().
 *
 * Criteria are php strings that evaluate to either TRUE or FALSE. These strings are evaluated using
 * drupal_eval() which suppresses syntax errors and throws watchdog entries of type php. There are
 * also watchdog entries of type tripal_core stating the exact criteria evaluated. Criteria can
 * contain the following tokens:
 *   - <field_name>
 *       Replaced by the name of the field to be excluded
 *   - <field_value>
 *       Replaced by the value of the field in the current record
 * Also keep in mind that if your criteria doesn't contain the &gt;field_value&lt;  token then it will be
 * evaluated before the query is executed and if the field is excluded it won't be included in the
 * query.
 *
 * @return
 *   An array of type => criteria where the type is excluded if the criteria evaluates to TRUE
 *
 * @ingroup tripal_chado_query_api
 */
function tripal_core_exclude_type_by_default() {
  return array('text' => 'strlen("<field_value> ") > 250');
}

/**
 * Implements hook_exclude_field_from_<tablename>_by_default()
 *
 * This hooks allows fields from a specified table that match a specified criteria to be excluded by
 * default from any table when chado_generate_var() is called. Keep in mind that if
 * fields are excluded by default they can always be expanded at a later date using
 * chado_expand_var().
 *
 * Criteria are php strings that evaluate to either TRUE or FALSE. These strings are evaluated using
 * drupal_eval() which suppresses syntax errors and throws watchdog entries of type php. There are
 * also watchdog entries of type tripal_core stating the exact criteria evaluated. Criteria can
 * contain the following tokens:
 *   - <field_name>
 *       Replaced by the name of the field to be excluded
 *   - <field_value>
 *       Replaced by the value of the field in the current record
 * Also keep in mind that if your criteria doesn't contain the <field_value>  token then it will be
 * evaluated before the query is executed and if the field is excluded it won't be included in the
 * query.
 *
 * @return
 *   An array of field => criteria where the type is excluded if the criteria evaluates to TRUE
 *
 * @ingroup tripal_chado_query_api
 */
function tripal_core_exclude_field_from_feature_by_default() {
  return array('residues' => 'TRUE');
}

/**
 * Generates an array containing the full details of a record(s) in chado. The 
 * returned array differs from the array returned by chado_select_record as all foreign key
 * relationships have been followed and those data are also included. The array
 * returned by this function can be used with chado_expand_var function to add
 * additional FK relationships that were not included because they were not
 * a one-to-one mapping or for fields that were excluded such as large text fields.
 *  
 *
 * @param $table
 *   The name of the base table to generate a variable for
 * @param $values
 *   A select values array that selects the records you want from the base table
 *   (this has the same form as chado_select_record)
 * @param $base_options
 *   An array containing options for the base table.  For example, an
 *   option of 'order_by' may be used to sort results in the base table
 *   if more than one are returned.  The options must be compatible with
 *   the options accepted by the chado_select_record() function.
 *   Additionally,  These options are available for this function:
 *   -return_array:
 *     can be provided to force the function to always return an array. Default
 *     behavior is to return a single record if only one record exists or to return
 *     an array if multiple records exist.
 *  - include_fk:
 *     an array of FK relationships to follow. By default, the
 *     chado_select_record function will follow all FK relationships but this
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
   $features = chado_generate_var('feature', $values);
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
 * @ingroup tripal_chado_query_api
 */
function chado_generate_var($table, $values, $base_options = array()) {
  $all = new stdClass();

  $return_array = 0;
  if (array_key_exists('return_array', $base_options)) {
    $return_array = 1;
  }
  $include_fk = FALSE;
  if (array_key_exists('include_fk', $base_options)) {
    $include_fk = $base_options['include_fk'];
  }
  $pager = array();
  if (array_key_exists('pager', $base_options)) {
    $pager = $base_options['pager'];
  }
  // get description for the current table----------------------------------------------------------
  $table_desc = chado_get_schema($table);
  if (!$table_desc or count($table_desc) == 0) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      "chado_generate_var: The table '%table' has not been defined. " .
      "and cannot be expanded. If this is a custom table, please add it using the Tripal " .
      "custom table interface.", array('%table' => $table));
    if ($return_array) {
      return array();
    }
    return FALSE;
  }
  $table_primary_key = $table_desc['primary key'][0];
  $table_columns = array_keys($table_desc['fields']);

  // Expandable fields without value needed for criteria--------------------------------------------
  // Add in the default expandable arrays
  // These are used for later expanding fields, tables, foreign keys and nodes
  $all->expandable_fields = array();
  $all->expandable_foreign_keys = array();
  if (array_key_exists('referring_tables', $table_desc) and $table_desc['referring_tables']) {
    $all->expandable_tables = $table_desc['referring_tables'];
  }
  else {
    $all->expandable_tables = array();
  }
  $all->expandable_nodes = array();


  // Get fields to be removed by name.................................
  // This gets all implementations of hook_exclude_field_from_<table>_by_default()
  // where <table> is the current table a variable is being created for.

  // This allows modules to specify that some fields should be excluded by default
  // For example, tripal core provides a tripal_core_exclude_field_from_feature_by_default()
  // which says that we usually don't want to include the residues field by default since
  // it can be very large and cause performance issues.

  // If a field is excluded by default it can always be expanded at a later point by calling
  // chado_expand_var($chado_var, 'field', <field name as shown in expandable_fields array>);

  // First get an array of all the fields to be removed for the current table
  // module_invoke_all() is drupal's way of invoking all implementations of the specified
  // hook and merging all of the results.

  // $fields_to_remove should be an array with the keys matching field names
  // and the values being strings to be executed using php_eval() to determine whether
  // to exclude the field (evaluates to TRUE) or not (evaluates to FALSE)
  $fields_to_remove = module_invoke_all('exclude_field_from_' . $table . '_by_default');

  // Now, for each field to be removed
  foreach ($fields_to_remove as $field_name => $criteria) {

    //replace <field_name> with the current field name
    $criteria = preg_replace('/<field_name> /', addslashes($field_name), $criteria);
    // if field_value needed we can't deal with this field yet
    if (preg_match('/<field_value> /', $criteria)) {
      break;
    }

    //if criteria then remove from query
    // @coder-ignore: only module designers can populate $criteria -not a security risk
    $success = php_eval('<?php return ' . $criteria . '; ?>');
    if ($success) {
      unset($table_columns[array_search($field_name, $table_columns)]);
      unset($fields_to_remove[$field_name]);
      $all->expandable_fields[] = $table . '.' . $field_name;
    }
  }

  // Get fields to be removed by type................................
  // This gets all implementations of hook_exclude_type_by_default().

  // This allows modules to specify that some types of fields should be excluded by default
  // For example, tripal core provides a tripal_core_exclude_type_by_default() which says
  // that text fields are often very large and if they are longer than 250 characters then
  // we want to exclude them by default

  // If a field is excluded by default it can always be expanded at a later point by calling
  // chado_expand_var($chado_var, 'field', <field name as shown in expandable_fields array>);

  // First get an array of all the types of fields to be removed for the current table
  // module_invoke_all() is drupal's way of invoking all implementations of the specified
  // hook and merging all of the results.

  // $types_to_remove should be an array with the keys matching field names
  // and the values being strings to be executed using php_eval() to determine whether
  // to exclude the field (evaluates to TRUE) or not (evaluates to FALSE)
  // (ie: array('text' => 'strlen("<field_value> ") > 100');
  $types_to_remove = module_invoke_all('exclude_type_by_default');

  // Get a list of all the types of fields
  // the key is the type of field and the value is an array of fields of this type
  $field_types = array();
  foreach ($table_desc['fields'] as $field_name => $field_array) {
    $field_types[$field_array['type']][] = $field_name;
  }

  // We want to use the types to remove in conjunction with our table field descriptions
  // to determine which fields might need to be removed
  foreach ($types_to_remove as $field_type => $criteria) {

    // if there are fields of that type to remove
    if (isset($field_types[$field_type])) {

      // Do any processing needed on the php criteria
      //replace <field_name>  with the current field name
      $criteria = preg_replace('/<field_name> /', addslashes($field_name), $criteria);
      foreach ($field_types[$field_type] as $field_name) {
        // if field_value needed we can't deal with this field yet
        if (preg_match('/<field_value>/', $criteria)) {
          $fields_to_remove[$field_name] = $criteria;
          continue;
        }

        // if criteria then remove from query
        // (as long as <field_value> is not needed for the criteria to be evaluated)
        // @coder-ignore: only module designers can populate $criteria -not a security risk
        $success = php_eval('<?php return ' . $criteria . '; ?>');
        if ($success) {
          unset($table_columns[array_search($field_name, $table_columns)]);
          $all->expandable_fields[] = $table . '.' . $field_name;
        }
      } //end of foreach field of that type
    }
  } //end of foreach type to be removed

  // get the values for the record in the current table---------------------------------------------
  $results = chado_select_record($table, $table_columns, $values, $base_options);

  if ($results) {
    foreach ($results as $key => $object) {
      // Add empty expandable_x arrays
      $object->expandable_fields = $all->expandable_fields;
      $object->expandable_foreign_keys = $all->expandable_foreign_keys;
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
          WHERE $table_primary_key = :$table_primary_key
        ";
        $mapping = db_query($sql, array(":$table_primary_key" => $object->{$table_primary_key}))->fetchObject();
        if ($mapping and $mapping->$table_primary_key) {
          $object->nid = $mapping->nid;
          $object->expandable_nodes[] = $table;
        }
      }

      // remove any fields where criteria needs to be evalulated---------------------------------------
      // The fields to be removed can be populated by implementing either
      // hook_exclude_field_from_<table>_by_default() where <table> is the current table
      // OR hook_exclude_type_by_default() where there are fields of the specified type in the current table
      // It only reaches this point if the criteria specified for whether or not to
      // exclude the field includes <field_value> which means it has to be evaluated after
      // the query has been executed
      foreach ($fields_to_remove as $field_name => $criteria) {

        // If the field is an object then we don't support exclusion of it
        // For example, if the field is a foreign key
        if (!isset($object->{$field_name})) {
          break;
        }

        // replace <field_value> with the actual value of the field from the query
        $criteria = preg_replace('/<field_value>/', addslashes($object->{$field_name}), $criteria);

        // evaluate the criteria, if TRUE is returned then exclude the field
        // excluded fields can be expanded later by calling
        // chado_expand_var($var, 'field', <field name as shown in expandable_fields array>);
        $success = php_eval('<?php return ' . $criteria . '; ?>');
        if ($success) {
          unset($object->{$field_name});
          $object->expandable_fields[] = $table . '.' . $field_name;
        }
      }

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

            if (is_array($include_fk)) {
              // don't recurse if the callee has supplied an $fk_include list and this
              // FK table is not in the list.
              if (is_array($include_fk) and !array_key_exists($foreign_key, $include_fk)) {
                $object->expandable_foreign_keys[] = $table . '.' . $foreign_key . ' => ' . $foreign_table;
                continue;
              }
            }
            // if we have the option but it is not an array then we don't recurse any furutehr
            if ($include_fk === TRUE) {
              $object->expandable_foreign_keys[] = $table . '.' . $foreign_key . ' => ' . $foreign_table;
              continue;
            }

            // get the record from the foreign table
            $foreign_values = array($primary_key => $object->{$foreign_key});
            $options = array();
            if (is_array($include_fk)) {
              $options['include_fk'] = $include_fk[$foreign_key];
            }

            $foreign_object = chado_generate_var($foreign_table, $foreign_values, $options);

            // add the foreign record to the current object in a nested manner
            $object->{$foreign_key} = $foreign_object;
            // Flatten expandable_x arrays so only in the bottom object
            if (property_exists($object->{$foreign_key}, 'expandable_fields') and
                is_array($object->{$foreign_key}->expandable_fields)) {
              $object->expandable_fields = array_merge(
                $object->expandable_fields,
                $object->{$foreign_key}->expandable_fields
              );
              unset($object->{$foreign_key}->expandable_fields);
            }
            if (property_exists($object->{$foreign_key}, 'expandable_foreign_keys') and
                is_array($object->{$foreign_key}->expandable_foreign_keys)) {
              $object->expandable_foreign_keys = array_merge(
                $object->expandable_foreign_keys,
                $object->{$foreign_key}->expandable_foreign_keys
              );
              unset($object->{$foreign_key}->expandable_foreign_keys);
            }
            if (property_exists($object->{$foreign_key}, 'expandable_tables') and
                is_array($object->{$foreign_key}->expandable_tables)) {
              $object->expandable_tables = array_merge(
                $object->expandable_tables,
                $object->{$foreign_key}->expandable_tables
              );
              unset($object->{$foreign_key}->expandable_tables);
            }
            if (property_exists($object->{$foreign_key}, 'expandable_nodes') and
                is_array($object->{$foreign_key}->expandable_nodes)) {
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
 * This function exists to allow chado_generate_var() to excldue some
 * fields/tables/nodes from the default form of a variable without making it extremely difficult for
 * the tripal admin to get at these variables if he/she wants them.
 *
 * @param $object
 *   This must be an object generated using chado_generate_var()
 * @param $type
 *   Must be one of 'field', 'table', 'node'. Indicates what is being expanded.
 * @param $to_expand
 *   The name of the field/table/node to be expanded
 * @param $table_options
 *   - order_by:
 *     An array containing options for the base table.  For example, an
 *     option of 'order_by' may be used to sort results in the base table
 *     if more than one are returned.  The options must be compatible with
 *     the options accepted by the chado_select_record() function.
 *   - return_array:
 *     Additionally,  The option 'return_array' can be provided to force
 *     the function to expand tables as an array. Default behavior is to expand
 *     a table as single record if only one record exists or to expand as an array if
 *     multiple records exist.
 *   - include_fk:
 *     an array of FK relationships to follow. By default, the
 *     chado_expand_var function will follow all FK relationships but this
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
   $features = chado_generate_var('feature', $values);
   // Expand the organism node
   $feature = chado_expand_var($feature, 'node', 'organism');
   // Expand the feature.residues field
   $feature = chado_expand_var($feature, 'field', 'feature.residues');
   // Expand the feature properties (featureprop table)
   $feature = chado_expand_var($feature, 'table', 'featureprop');
 * @endcode
 *
 * @ingroup tripal_chado_query_api
 */
function chado_expand_var($object, $type, $to_expand, $table_options = array()) {

  // make sure we have a value
  if (!$object) {
    tripal_report_error('tripal_core', TRIPAL_ERROR,
      'Cannot pass non array as argument, $object, to chado_expand_var function.', array());
    return $object;
  }

  // check to see if we are expanding an array of objects
  if (is_array($object)) {
    foreach ($object as $index => $o) {
      $object[$index] = chado_expand_var($o, $type, $to_expand);
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
        $table_desc = chado_get_schema($tablename);

        // BASE CASE: the field is from the current table
        if ($base_table == $tablename) {
          // Use the table description to fully describe the current object
          // in a $values array to be used to select the field from chado
          $values = array();
          foreach ($table_desc['primary key'] as $key) {
            if(property_exists($object, $key)) {
              $values[$key] = $object->{$key};
            }
          }

          // Retrieve the field from Chado
          $results = chado_select_record($tablename, array($fieldname), $values);

          // Check that the field was retrieved correctly
          if (isset($results[0])) {
            $object->{$fieldname} = $results[0]->{$fieldname};
            $object->expanded = $to_expand;
          }
          // If it wasn't retrieved correctly, we need to warn the administrator

        }
        // RECURSIVE CASE: the field is in a nested object
        else {
          // We want to look at each field and if it's an object then we want to
          // attempt to expand the field in it via recursion
          foreach ((array) $object as $field_name => $field_value) {
            if (is_object($field_value)) {
              $object->{$field_name} = chado_expand_var(
              $field_value,
                'field',
              $to_expand
              );
            }
          } //end of for each field in the current object
        }
      }
      // Otherwise we weren't able to extract the parts of the field to expand
      // Thus we will warn the administrator
      else {
        tripal_report_error('tripal_core', TRIPAL_ERROR,
          'chado_expand_var: Field (%field) not in the right format. " .
          "It should be <tablename>.<fieldname>', array('%field' => $to_expand));
      }
      break;

    case "foreign_key": //--------------------------------------------------------------------------
      if (preg_match('/(\w+)\.(\w+) => (\w+)/', $to_expand, $matches)) {
        $table_name = $matches[1];
        $field_name = $matches[2];
        $foreign_table = $matches[3];
        $table_desc = chado_get_schema($table_name);

        // BASE CASE: The foreign key is from the current table
        if ($base_table == $table_name) {

          // Get the value of the foreign key from the object
          $field_value = $object->{$field_name};

          // Get the name of the field in the foreign table using the table description
          // For example, with the feature.type_id => cvterm.cvterm_id we need cvterm_id
          $foreign_field_name = FALSE;
          foreach ($table_desc['foreign keys'][$foreign_table]['columns'] as $left => $right) {
            if ($right == $field_name) {
              $foreign_field_name = $left;
            }
          }

          // Check that we were able to determine the field name in the foreign table
          if ($foreign_field_name) {

            // Generate a chado variable of the foreign key
            // For example, if the foreign key to expand is feature.type_id
            // then we want to generate a chado cvterm variable that matches the feature.type_id
            $foreign_var = chado_generate_var(
              $foreign_table, // thus in the example above, generate a cvterm var
              array($foreign_field_name => $field_value), // where the cvterm.cvterm_id = feature.type_id value
              $table_options //pass in the same options given to this function
            );

            // Check that the foreign object was returned
            if ($foreign_var) {

              // It was so now we can add this chado variable to our current object
              // in place of the key value
              $object->{$field_name} = $foreign_var;
              $object->expanded = $to_expand;

            }
            // Otherwise we weren't able to expand the foreign key
            else {
              tripal_report_error('tripal_core', TRIPAL_ERROR,
                'chado_expand_var: unable to retrieve the object desribed by the foreign key
                while trying to expand %fk.',
                array('%fk' => $to_expand));
            }
          }
          // Else we were unable to determine the field name in the foreign table
          else {
            tripal_report_error('tripal_core', TRIPAL_ERROR,
              'chado_expand_var: unable to determine the field name in the table the foreign
              key points to while trying to expand %fk.',
              array('%fk' => $to_expand));
          }

        }
        // RECURSIVE CASE: Check any nested objects
        else {

          foreach ((array) $object as $field_name => $field_value) {
            if (is_object($field_value)) {
              $object->{$field_name} = chado_expand_var(
              $field_value,
              'foreign_key',
              $to_expand
              );
            }
          } //end of for each field in the current object

        }
      }
      // Otherwise we weren't able to extract the parts of the foreign key to expand
      // Thus we will warn the administrator
      else {
        tripal_report_error('tripal_core', TRIPAL_ERROR,
          'chado_expand_var: foreign_key (%fk) not in the right format. " .
          "It should be <tablename>.<fieldname>', array('%fk' => $to_expand));
      }
      break;

    case "table": //--------------------------------------------------------------------------------
      $foreign_table = $to_expand;

      // RECURSIVE BASE CASE: don't expand the table it already is expanded
      if (array_key_exists($foreign_table, $object)) {
        return $object;
      }
      $foreign_table_desc = chado_get_schema($foreign_table);

      // BASE CASE: If it's connected to the base table via a FK constraint
      // then we have all the information needed to expand it now
      if (array_key_exists($base_table, $foreign_table_desc['foreign keys'])) {
        foreach ($foreign_table_desc['foreign keys'][$base_table]['columns'] as $left => $right) {
          // if the FK value in the base table is not there then we can't expand it, so just skip it.
          if (!$object->{$right}) {
            continue;
          }

          // generate a new object for this table using the FK values in the base table.
          $new_options = $table_options;
          $foreign_object = chado_generate_var($foreign_table, array($left => $object->{$right}), $new_options);

          // if the generation of the object was successful, update the base object to include it.
          if ($foreign_object) {
            // in the case where the foreign key relationship exists more
            // than once with the same table we want to alter the array structure to
            // include the field name.
            if (count($foreign_table_desc['foreign keys'][$base_table]['columns']) > 1) {
              if (!property_exists($object, $foreign_table)) {
                $object->{$foreign_table} = new stdClass();
              }
              $object->{$foreign_table}->{$left} = $foreign_object;
              $object->expanded = $to_expand;

            }
            else {
              if (!property_exists($object, $foreign_table)) {
                $object->{$foreign_table} = new stdClass();
              }
              $object->{$foreign_table} = $foreign_object;
              $object->expanded = $to_expand;
            }
          }
          // if the object returned is NULL then handle that
          else {
            // in the case where the foreign key relationship exists more
            // than once with the same table we want to alter the array structure to
            // include the field name.
            if (count($foreign_table_desc['foreign keys'][$base_table]['columns']) > 1) {
              if (!property_exists($object, $foreign_table)) {
                $object->{$foreign_table} = new stdClass();
              }
              $object->{$foreign_table}->{$left} = NULL;
            }
            else {
              $object->{$foreign_table} = NULL;
            }
          }
        }
      }
      // RECURSIVE CASE: if the table is not connected directly to the current base table
      // through a foreign key relationship, then maybe it has a relationship to
      // one of the nested objects.
      else {

        // We need to recurse -the table has a relationship to one of the nested objects
        // We assume it's a nested object if the value of the field is an object
        $did_expansion = 0;
        foreach ((array) $object as $field_name => $field_value) {

          // if we have a nested object ->expand the table in it
          // check to see if the $field_name is a valid chado table, we don't need
          // to call chado_expand_var on fields that aren't tables
          $check = chado_get_schema($field_name);
          if ($check) {
            $did_expansion = 1;
            $object->{$field_name} = chado_expand_var($field_value, 'table', $foreign_table);
          }
        }

        // if we did not expand this table we should return a message that the foreign table
        // could not be expanded
        if (!$did_expansion) {
          tripal_report_error('tripal_core', TRIPAL_ERROR, 'chado_expand_var: Could not expand table, %table. It is ' .
            'not in a foreign key relationship with the base object nor with any other expanded table. ' .
            'Check the table definition to ensure that a proper foreign key relationship is present.',
            array('%table' => $foreign_table));
        }
      }
      break;

    case "node": //---------------------------------------------------------------------------------

      // BASE CASE: if the node to be expanded is for our base table, then just expand it
      if ($object->tablename == $to_expand) {

        // Load the node based on the current objects nid (node primary key)
        $node = node_load($object->nid);

        // If we have successfully loaded the node...
        if ($node) {

          // Move expandable arrays from the object into the node
          $object->expanded = $to_expand;
          $node->expandable_fields = $object->expandable_fields;
          unset($object->expandable_fields);
          $node->expandable_tables = $object->expandable_tables;
          unset($object->expandable_tables);
          $node->expandable_nodes = $object->expandable_nodes;
          unset($object->expandable_nodes);

          // The node becomes the base object with the obejct added to it.
          // For example, we may start with a feature object with a name, uniquename , type, etc.
          // After expanding we will return the node and at $node->feature you will find the original object
          $node->{$base_table} = $object;
          $object = $node;

        }
        // Else we were unable to load the node
        else {

          // Warn the administrator
          tripal_report_error('tripal_core', TRIPAL_ERROR, 'chado_expand_var: No node matches the nid (%nid) supplied.',
            array('%nid' => $object->nid));
        } //end of if node

      }
      // RECURSIVE CASE: check to see if the node to be expanded associates with a
      // chado table within one of the nested objects.
      else {

        // We need to recurse -the node to expand is one of the nested objects
        // We assume it's a nested object if the field value is an object
        foreach ((array) $object as $field_name => $field_value) {
          if (is_object($field_value)) {
            $object->{$field_name} = chado_expand_var(
            $field_value,
              'node',
            $to_expand
            );
          }
        } //end of for each field in the current object
      }
      break;

    // The $type to be expanded is not yet supported
    default:
      tripal_report_error('tripal_core', TRIPAL_ERROR, 'chado_expand_var: Unrecognized type (%type). Should be one of "field", "table", "node".',
        array('%type' => $type));
      return FALSE;
  }

  // Move expandable arrays downwards -------------------------------
  // If the type was either table or foreign key then a new chado variable was generated
  // this variable will have it's own expandable array's which need to be moved down
  // and merged with the base objects expandable arrays

  // Thus, check all nested objects for expandable arrays
  // and if they have them, move them downwards
  foreach ( (array)$object as $field_name => $field_value) {
    if (is_object($field_value)) {

      // The current nested object has expandable arrays
      if (isset($field_value->expandable_fields)) {

        // Move expandable fields downwards
        if (isset($field_value->expandable_fields) and is_array($field_value->expandable_fields)) {

          // If the current object has it's own expandable fields then merge them
          if (isset($object->expandable_fields)) {
            $object->expandable_fields = array_merge(
              $object->expandable_fields,
              $object->{$field_name}->expandable_fields
            );
            unset($object->{$field_name}->expandable_fields);

          }
          // Otherwise, just move the expandable fields downwards
          else {
            $object->expandable_fields = $object->{$field_name}->expandable_fields;
            unset($object->{$field_name}->expandable_fields);
          }

        }

        // Move expandable foreign keys downwards
        if (isset($field_value->expandable_foreign_keys) and is_array($field_value->expandable_foreign_keys)) {

          // If the current object has it's own expandable foreign keys then merge them
          if (isset($object->expandable_foreign_keys)) {
            $object->expandable_foreign_keys = array_merge(
              $object->expandable_foreign_keys,
              $object->{$field_name}->expandable_foreign_keys
            );
            unset($object->{$field_name}->expandable_foreign_keys);

          }
          // Otherwise, just move the expandable foreign keys downwards
          else {
            $object->expandable_foreign_keys = $object->{$field_name}->expandable_foreign_keys;
            unset($object->{$field_name}->expandable_foreign_keys);
          }
        }

        // Move expandable tables downwards
        if (isset($field_value->expandable_tables) and is_array($field_value->expandable_tables)) {

          // If the current object has it's own expandable tables then merge them
          if (isset($object->expandable_tables)) {
            $object->expandable_tables = array_merge(
              $object->expandable_tables,
              $object->{$field_name}->expandable_tables
            );
            unset($object->{$field_name}->expandable_tables);

          }
          // Otherwise, just move the expandable tables downwards
          else {
            $object->expandable_tables = $object->{$field_name}->expandable_tables;
            unset($object->{$field_name}->expandable_tables);
          }
        }

        // Move expandable nodes downwards
        if (isset($field_value->expandable_nodes) and is_array($field_value->expandable_nodes)) {

          // If the current object has it's own expandable tables then merge them
          if (isset($object->expandable_nodes)) {
            $object->expandable_nodes = array_merge(
              $object->expandable_nodes,
              $object->{$field_name}->expandable_nodes
            );
            unset($object->{$field_name}->expandable_nodes);

          }
          // Otherwise, just move the expandable tables downwards
          else {
            $object->expandable_nodes = $object->{$field_name}->expandable_nodes;
            unset($object->{$field_name}->expandable_nodes);
          }
        }
      }
    }
  }

  // Move extended array downwards ----------------------------------
  // This tells us what we have expanded (ie: that we succeeded)
  // and is needed to remove the entry from the expandable array

  // If there is no expanded field in the current object then check any of the nested objects
  // and move it down
  if (!property_exists($object, 'expanded')) {

    // It's a nested object if the value is an object
    foreach ( (array)$object as $field_name => $field_value) {
      if (is_object($field_value)) {

        // Check if the current nested object has an expanded array
        if (isset($field_value->expanded)) {

          // If so, then move it downwards
          $object->expanded = $field_value->expanded;
          unset($field_value->expanded);
        }
      }
    }
  }

  // Check again if there is an expanded field in the current object
  // We check again because it might have been moved downwards above
  if (property_exists($object, 'expanded')) {

    // If so, then remove the expanded identifier from the correct expandable array
    $expandable_name = 'expandable_' . $type . 's';
    if (property_exists($object, $expandable_name) and $object->{$expandable_name}) {
      $key_to_remove = array_search($object->expanded, $object->{$expandable_name});
      unset($object->{$expandable_name}[$key_to_remove]);
      unset($object->expanded);

    }
  }

  // Finally, Return the object!
  return $object;
}
