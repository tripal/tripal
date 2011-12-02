<?php
/**
 * @file
 * The Tripal Core API
 *
 * This file provides the API needed for all other Tripal and Tripal dependent
 * modules.
 */
 
/**
 * @defgroup tripal_api Tripal API
 * @{
 * Provides an application programming interface (API) for Tripal
 *
 * The Tripal API currently provides generic insert/update/select functions for all chado content as 
 * well as some module specific functions that insert/update/delete/select specific chado content. 
 *
 * This API is currently in its infancy and some necessary functions might be missing. If you find
 * a missing function that you think should be included go to the sourceforge feature request 
 * page and request it's inclusion in the API. Such feature requests with a working function 
 * definition will be given priority.
 * @}
 */

require_once "tripal_core.schema.api.inc";

/** 
 * @defgroup tripal_chado_api Core Module Chado API
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
 * @ingroup tripal_api
 */

/** 
 * @defgroup tripal_files_api Core Module Files API
 * @{
 * Provides an application programming interface (API) for managing files within 
 * the Tripal data directory structure.
 *
 * @}
 * @ingroup tripal_api
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
* 
* @return
*  On success this function returns TRUE. On failure, it returns FALSE.
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
function tripal_core_chado_insert($table,$values){
   $insert_values = array();
   
   // get the table description
   $table_desc = module_invoke_all('chado_'.$table.'_schema');

   // iterate through the values array and create a new 'insert_values' array
   // that has all the values needed for insert with all foreign relationsihps
   // resolved.
   foreach($values as $field => $value){
      if(is_array($value)){
         // select the value from the foreign key relationship for this value
         $results = tripal_core_chado_get_foreign_key($table_desc,$field,$value);
         if (sizeof($results) > 1) {
           watchdog('tripal_core', 'tripal_core_chado_insert: Too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);         
         } elseif (sizeof($results) < 1) {
           //watchdog('tripal_core', 'tripal_core_chado_insert: no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
         } else {
           $insert_values[$field] = $results[0];
         }
      }  
      else {
         $insert_values[$field] = $value;
      }
   }

   // check for violation of any unique constraints
   $ukeys = $table_desc['unique keys'];
   $ukselect_cols = array();
   $ukselect_vals = array();
   if ($ukeys) {
     foreach($ukeys as $name => $fields){
        foreach($fields as $index => $field){
           // build the arrays for performing a select that will check the contraint
           array_push($ukselect_cols,$field);
           $ukselect_vals[$field] = $insert_values[$field];
        }
        // now check the constraint
        if(tripal_core_chado_select($table,$ukselect_cols,$ukselect_vals)){
           watchdog('tripal_core',"tripal_core_chado_insert: Cannot insert duplicate record into $table table: " . print_r($values,1),array(),'WATCHDOG_ERROR');
           return false;
        }
     }
   }

   // if trying to insert a field that is the primary key, make sure it also is unique
   $pkey = $table_desc['primary key'][0];
   if($insert_values[$pkey]){
      if(tripal_core_chado_select($table,array($pkey),array($pkey => $insert_values[$pkey]))){
         watchdog('tripal_core',"tripal_core_chado_insert: Cannot insert duplicate primary key into $table table: " . print_r($values,1),array(),'WATCHDOG_ERROR');
         return false;
      }
   }

   // make sure required fields have a value
   $fields = $table_desc['fields'];
   foreach($fields as $field => $def){
      // a field is considered missing if it cannot be null and there is no default
      // value for it or it is of type 'serial'
      if($def['not null'] == 1 and !array_key_exists($field,$insert_values) and !isset($def['default']) and strcmp($def['type'],serial)!=0){
         watchdog('tripal_core',"tripal_core_chado_insert: Field $table.$field cannot be null: " . print_r($values,1),array(),'WATCHDOG_ERROR');
         return false;
      }
   }

   // Now build the insert SQL statement
   $ifields = array();
   $ivalues = array();
   $itypes = array();
   foreach ($insert_values as $field => $value){
      array_push($ifields,$field);
      array_push($ivalues,$value);
      if(strcmp($value,'__NULL__')==0){
         array_push($itypes,"NULL");
      } 
      elseif(strcmp($fields[$field]['type'],'serial')==0 or 
         strcmp($fields[$field]['type'],'int')==0){
         array_push($itypes,"%d");
      } 
      else {
         array_push($itypes,"'%s'");
      }
   }
   $sql = "INSERT INTO {$table} (" . implode(", ",$ifields) . ") VALUES (". implode(", ",$itypes) .")";

   // finally perform the insert. 
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $result = db_query($sql,$ivalues);
   tripal_db_set_active($previous_db);  // now use drupal database 
   if($result){
      // add primary keys to values before return
      $primary_key = array();
      foreach ($table_desc['primary key'] as $field) {
        $value = db_last_insert_id($table, $field);
        $values[$field] = $value;
      }
      return $values;
   } 
   else {
      watchdog('tripal_core',"tripal_core_chado_insert: Cannot insert record into $table table: " . print_r($values,1),array(),'WATCHDOG_ERROR');
      return false;
   }
   return false;
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
 *
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
 *   $result = tripal_core_chado_update('feature',$umatch,$uvalues);
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
function tripal_core_chado_delete($table,$match){
   $delete_matches = array();  // contains the values for the where clause
   
   // get the table description
   $table_desc = module_invoke_all('chado_'.$table.'_schema');

   // get the values needed for matching in the SQL statement
   foreach ($match as $field => $value){
      if(is_array($value)){
         // if the user has specified an array of values to delete rather than
         // FK relationships the keep those in our match 
         if(array_values($value) === $value){
            $delete_matches[$field] = $value;
         } else {
            $results = tripal_core_chado_get_foreign_key($table_desc,$field,$value);
            if (sizeof($results) > 1) {
              watchdog('tripal_core', 'tripal_core_chado_delete: When trying to find record to delete, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);         
            } elseif (sizeof($results) < 1) {
              //watchdog('tripal_core', 'tripal_core_chado_delete: When trying to find record to delete, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
            } else {
              $delete_matches[$field] = $results[0];
            }
         }
      }
      else {
         $delete_matches[$field] = $value;
      }
   }
   // now build the SQL statement
   $sql = "DELETE FROM {$table} WHERE ";
   $dargs = array();
   foreach($delete_matches as $field => $value){
      if (count($value) > 1) {
         $sql .= "$field IN (".db_placeholders($value,'varchar').") AND ";
         foreach ($value as $v) { $dargs[] = $v; }
      } else {
         if(strcmp($value,'__NULL__')==0){
            $sql .= " $field = NULL AND ";
         } 
         elseif(strcmp($fields[$field]['type'],'serial')==0 or 
            strcmp($fields[$field]['type'],'int')==0){
            $sql .= " $field = %d AND ";

         } 
         else {
            $sql .= " $field = '%s' AND ";
         }
         array_push($dargs,$value);
      }
   }
   $sql = substr($sql,0,-4);  // get rid of the trailing 'AND'
   
   // finally perform the delete.  If successful, return the updated record
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $result = db_query($sql,$dargs);
   tripal_db_set_active($previous_db);  // now use drupal database 
   if($result){
      return true;
   } 
   else {
      watchdog('tripal_core',"Cannot delete record in $table table.  Match:" . print_r($match,1) . ". Values: ". print_r($values,1),array(),'WATCHDOG_ERROR');
      return false;
   }
   return false;
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
*
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
function tripal_core_chado_update($table,$match,$values){
   $update_values = array();   // contains the values to be updated
   $update_matches = array();  // contains the values for the where clause
   
   // get the table description
   $table_desc = module_invoke_all('chado_'.$table.'_schema');

   // get the values needed for matching in the SQL statement
   foreach ($match as $field => $value){
      if(is_array($value)){
         $results = tripal_core_chado_get_foreign_key($table_desc,$field,$value);
         if (sizeof($results) > 1) {
           watchdog('tripal_core', 'tripal_core_chado_update: When trying to find record to update, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);         
         } elseif (sizeof($results) < 1) {
           //watchdog('tripal_core', 'tripal_core_chado_update: When trying to find record to update, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
         } else {
           $update_matches[$field] = $results[0];
         }
      }
      else {
         $update_matches[$field] = $value;
      }
   }

   // get the values used for updating
   foreach ($values as $field => $value){
      if(is_array($value)){
         $results = tripal_core_chado_get_foreign_key($table_desc,$field,$value);
         if (sizeof($results) > 1) {
           watchdog('tripal_core', 'tripal_core_chado_update: When trying to find update values, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);         
         } elseif (sizeof($results) < 1) {
           //watchdog('tripal_core', 'tripal_core_chado_update: When trying to find update values, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
         } else {
           $update_values[$field] = $results[0];
         }         
      }
      else {
         $update_values[$field] = $value;
      }
   }

   // now build the SQL statement
   $sql = "UPDATE {$table} SET ";
   $fields = $table_desc['fields'];
   $uargs = array();
   foreach($update_values as $field => $value){
      if(strcmp($value,'__NULL__')==0){
         $sql .= " $field = NULL, ";
      } 
      elseif(strcmp($fields[$field]['type'],'serial')==0 or 
         strcmp($fields[$field]['type'],'int')==0){
         $sql .= " $field = %d, ";
      } else {
         $sql .= " $field = '%s', ";
      }
      array_push($uargs,$value);
   }
   $sql = substr($sql,0,-2);  // get rid of the trailing comma & space
   $sql .= " WHERE ";
   foreach($update_matches as $field => $value){
      if(strcmp($value,'__NULL__')==0){
         $sql .= " $field = NULL AND ";
      } 
      elseif(strcmp($fields[$field]['type'],'serial')==0 or 
         strcmp($fields[$field]['type'],'int')==0){
         $sql .= " $field = %d AND ";
      } 
      else {
         $sql .= " $field = '%s' AND ";
      }
      array_push($uargs,$value);
   }
   $sql = substr($sql,0,-4);  // get rid of the trailing 'AND'
   
   // finally perform the update.  If successful, return the updated record
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $result = db_query($sql,$uargs);
   tripal_db_set_active($previous_db);  // now use drupal database 
   if($result){
      return true;
   } 
   else {
      watchdog('tripal_core',"Cannot update record in $table table.  Match:" . print_r($match,1) . ". Values: ". print_r($values,1),array(),'WATCHDOG_ERROR');
      return false;
   }
   return false;
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
*     Set this argument to 'true' to have this function return a numeric 
*     value for the number of recrods rather than the array of records.  this
*     can be useful in 'if' statements to check the presence of particula records.
*  - return_sql
*     Set this to 'true' to have this function return an array where the first 
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
* 
* @return
*  A database query result resource, FALSE if the query was not executed 
*  correctly, or the number of records in the dataset if $has_record is set.
*
* Example usage:
* @code
*   $columns = array('feature_id','name');
*   $values =  array(
*     'organism_id' => array(
*         'genus' => 'Citrus',
*         'species' => array('sinensis','clementina'),
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
*   $result = tripal_core_chado_select('feature',$columns,$values);
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
function tripal_core_chado_select($table,$columns,$values,$options = null){
    if (!is_array($options)) { $options = array(); }
    if (!$options['case_insensitive_columns']) { $options['case_insensitive_columns'] = array(); }
    if (!$options['regex_columns']) { $options['regex_columns'] = array(); }
    if (!$options['order_by']) { $options['order_by'] = array(); }
    
   if (!is_array($columns)){
      watchdog('tripal_core', 'the $columns argument for tripal_core_chado_select must be an array.');
      return false;
   }

   if (!is_array($values)){
      watchdog('tripal_core', 'the $values argument for tripal_core_chado_select must be an array.');
      return false;
   }

   // get the table description
   $table_desc = module_invoke_all('chado_'.$table.'_schema');

   $select = '';
   $from = ''; 
   $where = '';
   $args = array();
   foreach($values as $field => $value){
      $select[] = $field;
      if(is_array($value)){
         // if the user has specified multiple values for matching then this we
         // want to catch that and save them in our $where array, otherwise
         // we'll descend for a foreign key relationship
         if(array_values($value) === $value){
            $where[$field] = $value;
         } else {
            // select the value from the foreign key relationship for this value
            $foreign_options = array(
              'regex_columns' => $options['regex_columns'],
              'case_insensitive_columns' => $options['case_insensitive_columns']
            );
            $results = tripal_core_chado_get_foreign_key($table_desc,$field,$value, $foreign_options);
            if (sizeof($results) < 1) {
              // foreign key records are required
              // thus if none matched then return false and alert the admin through watchdog
              //watchdog('tripal_core', 
              // 'tripal_core_chado_select: no record in the table referenced by the foreign key (!field)   exists. tripal_core_chado_select table=!table, columns=!columns, values=!values', 
              // array('!table' => $table, 
              //   '!columns' => '<pre>' . print_r($columns, TRUE) . '</pre>', 
              //   '!values' => '<pre>' . print_r($values, TRUE) . '</pre>',
              //   '!field' => $field,
              // ), 
              // WATCHDOG_WARNING);
              return false;           
            } else {
              $where[$field] = $results;
            }
         }
      } 
      else {
        //need to catch a 0 and make int if integer field
        if ($table_desc['fields'][$field]['type'] == 'int') {
          $where[$field][] = (int) $value;
        } else {
          $where[$field][] = $value;
        }
      }
   }

   // now build the SQL select statement
   if (empty($where)) {
     // sometimes want to select everything
     $sql  = "SELECT " . implode(',',$columns) . " ";
     $sql .= "FROM {$table} ";
   } else {
     $sql  = "SELECT " . implode(',',$columns) . " ";
     $sql .= "FROM {$table} ";
     $sql .= "WHERE ";
     foreach($where as $field => $value){
       if (count($value) > 1) {
         $sql .= "$field IN (".db_placeholders($value,'varchar').") AND ";
         foreach ($value as $v) { $args[] = $v; }
       } else {
         $operator = '=';
         if (in_array($field, $options['regex_columns'])) {
           $operator = '~*';
         }
         if (in_array($field, $options['case_insensitive_columns'])) {
           $sql .= "lower($field) $operator lower('%s') AND ";
           $args[] = $value[0];
         }  else {
           $sql .= "$field $operator '%s' AND ";
           $args[] = $value[0];
         }
       }
     }
     $sql = substr($sql,0,-4);  // get rid of the trailing 'AND '
   }
   // finally add any ordering of the results to the SQL statement
   if(count($options['order_by']) > 0){
      $sql .= " ORDER BY ";
      foreach($options['order_by'] as $field => $dir){
         $sql .= "$field $dir, ";
      }
      $sql = substr($sql,0,-2);  // get rid of the trailing ', '
   }


   // if the caller has requested the SQL rather than the results...
   // which happens in the case of wanting to use the Drupal pager, then do so
   if($options['return_sql']){
      return array('sql'=> $sql, 'args' => $args);
   }

   $previous_db = tripal_db_set_active('chado');  // use chado database
   $resource = db_query($sql,$args);
   tripal_db_set_active($previous_db);  // now use drupal database   

   $results = array();
   while ($r = db_fetch_object($resource)) {
     $results[] = $r;    
   }
   
   if(!$options['has_record']){
      return $results;
   } else{
      return count($results);
   }
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
*   $value = tripal_core_chado_get_foreign_key('feature','organism_id',$values);
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
function tripal_core_chado_get_foreign_key($table_desc,$field,$values, $options = null){
    if (!is_array($options)) { $options = array(); }
    if (!$options['case_insensitive_columns']) { $options['case_insensitive_columns'] = array(); }
    if (!$options['regex_columns']) { $options['regex_columns'] = array(); }
    
   // get the list of foreign keys for this table description and
   // iterate through those until we find the one we're looking for
   $fkeys = $table_desc['foreign keys'];
   if($fkeys){
      foreach($fkeys as $name => $def){
         if (is_array($def['table'])) {
           //foreign key was described 2X
           $message = "The foreign key ".$name." was defined twice. Please check modules to determine if hook_chado_".$table_desc['table']."_schema() was implemented and defined this foreign key when it wasn't supposed to. Modules this hook was implemented in: ".implode(', ', module_implements("chado_".$table_desc['table']."_schema")).".";
           watchdog('tripal_core', $message);
           drupal_set_message($message,'error');
           continue;
         }
         $table = $def['table'];
         $columns = $def['columns'];
         // iterate through the columns of the foreign key relationship
         foreach($columns as $left => $right){
            // does the left column in the relationship match our field?
            if(strcmp($field,$left)==0){
               // the column name of the foreign key matches the field we want 
               // so this is the right relationship.  Now we want to select
               $select_cols = array($right);
               $result = tripal_core_chado_select($table,$select_cols,$values, $options);
               $fields = array();
               foreach ($result as $obj) {
                 $fields[] = $obj->$right;
               }
               return $fields;
            }
         }
      } 
   } 
   else {
      // TODO: what do we do if we get to this point and we have a fk 
      // relationship expected but we don't have any definition for one in the
      // table schema??
      $message = "There is no foreign key relationship defined for ".$field.". 
         To define a foreign key relationship, determine the table this foreign 
         key referrs to (<foreign table>) and then implement 
         hook_chado_<foreign table>_schema(). See 
         tripal_feature_chado_feature_schema for an example.";
      watchdog('tripal_core', $message);
      drupal_set_message($message,'error');      
   }
   return false;
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
 * @return
 *   Either an object (if only one record was selected from the base table) 
 *   or an array of objects (if more than one record was selected from the base table).
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
 function tripal_core_generate_chado_var($table, $values, $base_options=array()) {
  
  // get description for the current table----------------------------------------------------------
  $table_desc = module_invoke_all('chado_'.$table.'_schema');
  $table_primary_key = $table_desc['primary key'][0];
  $table_columns = array_keys($table_desc['fields']);

  // Expandable fields without value needed for criteria--------------------------------------------
  $all->expandable_fields = array();
  if ($table_desc['referring_tables']) {
    $all->expandable_tables = $table_desc['referring_tables'];
  } else {
    $all->expandable_tables = array();
  }
  $all->expandable_nodes = array();
    
  // Get fields to be removed by name.................................
  $fields_to_remove = module_invoke_all('exclude_field_from_'.$table.'_by_default');
  foreach ($fields_to_remove as $field_name => $criteria) {
    //replace &gt;field_name&lt;  with the current field name & 
    $criteria = preg_replace('/&gt;field_name&lt; /', $field_name, $criteria);

    // if field_value needed we can't deal with this field yet
    if (preg_match('/&gt;field_value&lt; /', $criteria)) { break; }

    //if criteria then remove from query
    $success = drupal_eval('<?php return '.$criteria.'; ?>');
//    watchdog('tripal_core', 
//      'Evaluating criteria (%criteria) for field %field in tripal_core_generate_chado_var for %table evaluated to %success',
//      array('%table' => $table, '%criteria'=>$criteria, '%field' => $field_name, '%success'=>$success),
//      WATCHDOG_NOTICE
//    );
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
      $criteria = preg_replace('/&gt;field_name&lt; /', $field_name, $criteria);
      
      foreach ($field_types[$field_type] as $field_name) {
        // if field_value needed we can't deal with this field yet
        if (preg_match('/&gt;field_value&lt; /', $criteria)) { 
          $fields_to_remove[$field_name] = $criteria;
          continue; 
        }

        // if field_value needed we can't deal with this field yet
        if (preg_match('/&gt;field_value&lt; /', $criteria)) { break; }

        //if criteria then remove from query
        $success = drupal_eval('<?php return '.$criteria.'; ?>');
//        watchdog('tripal_core', 
//          'Evaluating criteria (%criteria) for field %field of $type in tripal_core_generate_chado_var for %table evaluated to %success',
//          array('%table'=>$table, '%criteria'=>$criteria, '%field'=>$field_name, '%type'=>$field_type, '%success'=>$success),
//          WATCHDOG_NOTICE
//        );
        if ($success) {
          unset($table_columns[array_search($field_name, $table_columns)]);
          $all->expandable_fields[] = $table . '.' . $field_name;
        }
      } //end of foreach field of that type
    }
  } //end of foreach type to be removed
  
  // get the values for the record in the current table---------------------------------------------
  $results = tripal_core_chado_select($table, $table_columns, $values,$base_options);   
  
  if($results){
     foreach ($results as $key => $object) {
       // Add empty expandable_x arrays
       $object->expandable_fields = $all->expandable_fields;
       $object->expandable_tables = $all->expandable_tables;
       $object->expandable_nodes = $all->expandable_nodes;
       
       // add curent table
       $object->tablename = $table;
       
       // check if the current table maps to a node type-----------------------------------------------
       // if this table is connected to a node there will be a chado_tablename table in drupal
       if (db_table_exists('chado_'.$table)) {
         // that has a foreign key to this one ($table_desc['primary key'][0] 
         // and to the node table (nid)
         $sql = "SELECT %s, nid FROM chado_%s WHERE %s=%d";
         $mapping = db_fetch_object(db_query(
           $sql,
           $table_primary_key,
           $table,
           $table_primary_key, 
           $object->{$table_primary_key}
         ));
         if ($mapping->{$table_primary_key}) {
           $object->nid = $mapping->nid;
           $object->expandable_nodes[] = $table;
         }
       }

       // remove any fields where criteria need to be evalulated---------------------------------------
       foreach ($fields_to_remove as $field_name => $criteria) {
         if (!isset($object->{$field_name})) { break; }
         $criteria = preg_replace('/&gt;field_value&lt; /', $object->{$field_name}, $criteria);
         //if criteria then remove from query
         $success = drupal_eval('<?php return '.$criteria.'; ?>');
   //      watchdog('tripal_core', 
   //        'Evaluating criteria (%criteria) for field %field in tripal_core_generate_chado_var for   %table evaluated to %success',
   //        array('%table' => $table, '%criteria'=>$criteria, '%field' => $field_name, '%success'=>$success),
   //        WATCHDOG_NOTICE
   //      );
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
               break;
             }
             
             // get the record from the foreign table
             $foreign_values = array($primary_key => $object->{$foreign_key});
             $foreign_object = tripal_core_generate_chado_var($foreign_table, $foreign_values);
       
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
  
    // check only one result returned
  if (sizeof($results) == 1) {
    // add results to object
    return $results[0];
  } elseif (!empty($results)) {
    return $results;
  } else {
    // no results returned
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
 *   An array containing options for the base table.  For example, an
 *   option of 'order_by' may be used to sort results in the base table
 *   if more than one are returned.  The options must be compatible with
 *   the options accepted by the tripal_core_chado_select() function.
 * @return
 *   A chado object supplemented with the field/table/node requested to be expanded
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
function tripal_core_expand_chado_vars ($object, $type, $to_expand,$table_options = array()) {
  $base_table = $object->tablename;
  
  // check to see if they are expanding an array of objects
  if (is_array($object)) {
    foreach ($object as $index => $o) {
      $object[$index] = tripal_core_expand_chado_vars($o,$type,$to_expand);
    }
    return $object;
  }
  
  
  switch ($type) {
    case "field": //--------------------------------------------------------------------------------
      if (preg_match('/(\w+)\.(\w+)/', $to_expand, $matches)) {
        $tablename = $matches[1];
        $fieldname = $matches[2];
        $table_desc = module_invoke_all('chado_'.$tablename.'_schema');

        $values = array();
        foreach($table_desc['primary key'] as $key) {
          $values[$key] = $object->{$key};
        }
        
        if ($base_table == $tablename) {
          //get the field
          $results = tripal_core_chado_select(
            $tablename, 
            array($fieldname), 
            $values
          );
          $object->{$fieldname} = $results[0]->{$fieldname};
          $object->expanded = $to_expand;
        } else {
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
      } else {
        watchdog(
          'tripal_core',
          'tripal_core_expand_chado_vars: Field (%field) not in the right format. It should be <tablename>.<fieldname>',
          WATCHDOG_ERROR
        );
      }
    
    break;
    case "table": //--------------------------------------------------------------------------------
      $foreign_table = $to_expand;
      $foreign_table_desc = module_invoke_all('chado_'.$foreign_table.'_schema');
      
      // If it's connected to the base table
      if ($foreign_table_desc['foreign keys'][$base_table]) {
        foreach ($foreign_table_desc['foreign keys'][$base_table]['columns'] as $left => $right) {
          if (!$object->{$right}) { break; }
          
          if (is_array($values)) { 
            $values = array_merge($values, array($left => $object->{$right}) );
          } else {
            $values = array($left => $object->{$right});
          }
          $foreign_object = tripal_core_generate_chado_var(
            $foreign_table,
            array($left => $object->{$right}),
            $table_options
          );
  
          if ($foreign_object) {
            // in the case where the a foreign key relationships exists more 
            // than once with the same table we want to alter the 
            // array structure
            if(count($foreign_table_desc['foreign keys'][$base_table]['columns']) > 1){
               $object->{$foreign_table}->{$left} = $foreign_object;
               $object->expanded = $to_expand;
            } else {
               $object->{$foreign_table} = $foreign_object;
               $object->expanded = $to_expand;
            }
          }
        }
      } else {
        //We need to recurse -the table has a relationship to one of the nested objects
        foreach ((array) $object as $field_name => $field_value) {
          // if we have a nested object ->expand the table in it
          if (is_object($field_value)) {
            $object->{$field_name} = tripal_core_expand_chado_vars(
              $field_value,
              'table',
              $foreign_table
            );
          }
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
        } else {
          watchdog(
            'tripal_core',
            'tripal_core_expand_chado_vars: No node matches the nid (%nid) supplied.',
            array('%nid'=>$object->nid),
            WATCHDOG_ERROR
          );
        } //end of if node
      } else {
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
     watchdog('tripal_core',
      'tripal_core_expand_chado_vars: Unrecognized type (%type). Should be one of "field", "table", "node".',
      array('%type'=>$type),
      WATCHDOG_ERROR
    );
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
    $expandable_name = 'expandable_'.$type.'s';
    if ($object->{$expandable_name}) {
      $key_to_remove = array_search($object->expanded, $object->{$expandable_name});
      unset($object->{$expandable_name}[$key_to_remove]);
      unset($object->expanded);
    } else {
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
  return array('text' => "strlen('&gt;field_value&lt; ') > 100");
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
 *  Use this function instead of db_query() to avoid switching databases
 *  when making query to the chado database
 */
function chado_query($sql) {
	$args = func_get_args();
	array_shift($args);
	$sql = db_prefix_tables($sql);
	if (isset($args[0]) and is_array($args[0])) { // 'All arguments in one array' syntax
		$args = $args[0];
	}
	_db_query_callback($args, TRUE);
	$sql = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $sql);
	$previous_db = tripal_db_set_active('chado');
	$results = _db_query($sql);
	tripal_db_set_active($previous_db);
	return $results;
}

/**
 * Get chado id for a node. E.g, if you want to get 'analysis_id' from the 
 * 'analysis' table for a synced 'chado_analysis' node, use:
 * $analysis_id = chado_get_id_for_node ('analysis', $node)
 * Likewise,
 * $organism_id = chado_get_id_for_node ('organism', $node)
 * $feature_id = chado_get_id_for_node ('feature', $node)
 */
function chado_get_id_for_node ($table, $node) {
	 return db_result(db_query("SELECT $table"."_id FROM {chado_".$table."} WHERE nid = $node->nid"));
}

/**
 *  Get node id for a chado feature/organism/analysis. E.g, if you want to
 *  get the node id for an analysis, use:
 *  $nid = chado_get_node_id ('analysis', $analysis_id)
 *  Likewise,
 *  $nid = chado_get_node_id ('organism', $organism_id)
 *  $nid = chado_get_node_id ('feature', $feature_id) 
 */
function chado_get_node_id ($table, $id) {
	 return db_result(db_query("SELECT nid FROM {chado_".$table."} WHERE $table"."_id = $id"));
}

/**
 * Retrieve a property for a given base table record
 *
 * @param $basetable
 *   The base table for which the property should be retrieved. Thus to retrieve a property
 *   for a feature the basetable=feature and property is retrieved from featureprop
 * @param $record_id
 *   The primary key of the basetable to retrieve properties for. This should be in integer.
 * @param $property
 *   The cvterm name describing the type of properties to be retrieved
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 *
 * @return
 *   A chado variable with the specified properties expanded
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_get_property($basetable, $record_id, $property, $cv_name){

   // get the foreign key for this property table
   $table_desc = module_invoke_all('chado_'.$basetable.'prop_schema');
   $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

   // construct the array of values to be inserted  
   $values = array (
      $fkcol => $record_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => $cv_name,
         ),
         'name' => $property,
         'is_obsolete' => 0
      ),
   );
   $results = tripal_core_generate_chado_var($basetable.'prop',$values);
   $results = tripal_core_expand_chado_vars($results,'field',$basetable.'prop.value');
   return $results;
}
/**
 * Insert a property for a given basetable record
 *
 * @param $basetable
 *   The base table for which the property should be inserted. Thus to insert a property
 *   for a feature the basetable=feature and property is inserted into featureprop
 * @param $record_id
 *   The primary key of the basetable to insert a property for. This should be in integer.
 * @param $property
 *   The cvterm name describing the type of properties to be inserted
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 * @param $value
 *   The value of the property to be inserted (can be empty)
 * @param $update_if_present
 *   A boolean indicating whether an existing record should be updated or an error thrown
 *
 * @return
 *   Return True on Insert/Update and False otherwise
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_insert_property($basetable, $record_id, $property, 
   $cv_name, $value, $update_if_present = 0)
{
   // first see if the property already exists, if so we can't insert
   $prop = tripal_core_get_property($basetable,$record_id,$property,$cv_name);
   if(count($prop)>0){ 
      if($update_if_present){
        return tripal_core_update_property($basetable,$record_id,$property,$cv_name,$value) ;
      } else {
        return FALSE;
      }
   }

   // get the foreign key for this property table
   $table_desc = module_invoke_all('chado_'.$basetable.'prop_schema');
   $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

   // construct the array of values to be inserted  
   $values = array (
      $fkcol => $record_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => $cv_name,
         ),
         'name' => $property,
         'is_obsolete' => 0
      ),
      'value' => $value, 
      'rank' => 0,
   );
   return tripal_core_chado_insert($basetable.'prop',$values);
}

/**
 * Update a property for a given basetable record
 *
 * @param $basetable
 *   The base table for which the property should be updated. Thus to update a property
 *   for a feature the basetable=feature and property is updated in featureprop
 * @param $record_id
 *   The primary key of the basetable to update a property for. This should be in integer.
 * @param $property
 *   The cvterm name describing the type of property to be updated
 * @param $cv_name
 *   The name of the cv that the above cvterm is part of
 * @param $value
 *   The value of the property to be inserted (can be empty)
 * @param $insert_if_missing
 *   A boolean indicating whether a record should be inserted if one doesn't exist to update
 *
 * Note: The property to be updated is select via theu nique combination of $record_id and
 * $property and then it is updated with the supplied value
 *
 * @return
 *   Return True on Update/Insert and False otherwise
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_update_property($basetable, $record_id,$property,$cv_name,
   $value,$insert_if_missing = 0)
{

   // first see if the property is missing (we can't update a missing property
   $prop = tripal_core_get_property($basetable,$record_id,$property,$cv_name);
   if(count($prop)==0){
      if($insert_if_missing){
        return tripal_core_insert_property($basetable,$record_id,$property,$cv_name,$value);
      } else {
        return FALSE;
      }
   }

   // get the foreign key for this property table
   $table_desc = module_invoke_all('chado_'.$basetable.'prop_schema');
   $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

   // construct the array that will match the exact record to update
   $match = array (
      $fkcol => $record_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => $cv_name,
         ),
         'name' => $property,
      ),
   );
   // construct the array of values to be updated
   $values = array (      
      'value' => $value, 
   );
   return tripal_core_chado_update($basetable.'prop',$match,$values);
}

/**
 * Deletes a property for a given basetable record
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
 * Note: The property to be deleted is select via theu nique combination of $record_id and $property 
 *
 * @return
 *   Return True on Delete and False otherwise
 *
 * @ingroup tripal_chado_api
 */
function tripal_core_delete_property($basetable, $record_id,$property,$cv_name){
   // get the foreign key for this property table
   $table_desc = module_invoke_all('chado_'.$basetable.'prop_schema');
   $fkcol = key($table_desc['foreign keys'][$basetable]['columns']);

   // construct the array that will match the exact record to update
   $match = array (
      $fkcol => $record_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => $cv_name,
         ),
         'name' => $property,
      ),
   );
   return tripal_core_chado_delete($basetable.'prop',$match);
}

/**
 * This function is typically used in the '.install' file for a Tripal module
 * Each module should call this function during installation to create
 * the module data directory which is sites/default/files/tripal/[module_name] 
 * for default Drupal settings.  This directory can then be used by the module 
 * for storing files.
 *
 * @param $module_name
 *   the name of the module being installed.
 *
 * @returns
 *   nothing
 *
 * @ingroup tripal_files_api
 */
function tripal_create_moddir($module_name){
   // make the data directory for this module
   $data_dir = file_directory_path() . "/tripal/$module_name";
   if(!file_check_directory($data_dir,FILE_CREATE_DIRECTORY|FILE_MODIFY_PERMISSIONS)){
      $message = "Cannot create directory $data_dir. This module may not ".
                 "behave correctly without this directory.  Please  create ".
                 "the directory manually or fix the problem and reinstall.";
      drupal_set_message($message,'error');      
      watchdog('tripal_core',$message,array(),WATCHDOG_ERROR);
   }
}

/**
 * Each Tripal module has a unique data directory which was creatd using the
 * tripal_create_moddir function during installation.  This function 
 * retrieves the directory path.
 *
 * @param $module_name
 *   The name of the module
 *
 * @returns
 *   The path within the Drupal installation where the data directory resides
 * @ingroup tripal_files_api
 */
function tripal_get_moddir($module_name){
   $data_dir = file_directory_path() . "/tripal/$module_name";
   return $data_dir;
}
/**
 * Set the Tripal Database
 *
 * The tripal_db_set_active function is used to prevent namespace collisions
 * when chado and drupal are installed in the same database but in different
 * schemas.  It is also used for backwards compatibility with older versions
 * of tripal or in cases where chado is located outside of the Drupal database.
 *
 * @ingroup tripal_chado_api
 */
function tripal_db_set_active($dbname){
   global $db_url, $db_type;
   $chado_exists = 0;

   // only postgres can support search paths.  So if this is MysQL then
   // just run the normal tripal_db_set_active function.
   if(strcmp($db_type,'pgsql')==0){

      // if the 'chado' database is in the $db_url variable then chado is 
      // not in the same Drupal database
      if(is_array($db_url)){ 
         if(isset($db_url[$dbname])){
            return db_set_active($dbname);
         } 
      }

      // check to make sure the chado schema exists
      $sql = "select nspname from pg_catalog.pg_namespace where nspname = 'chado'";
      if(db_fetch_object(db_query($sql))){
         $chado_exists = 1;
      }

      // here we make the assumption that the default database schema is
      // 'public'.  This will most likely always be the case but if not,
      // then this code will break
      if($chado_exists && strcmp($dbname,'chado')==0){
         db_query("set search_path to %s",'chado,public');  
         return 'public,chado';
      } 
      elseif($chado_exists) {
         db_query("set search_path to %s",'public,chado');  
         return 'chado,public';
      }
      else {
         return db_set_active($dbname);
      }
   }
   else return db_set_active($dbname);
}
/**
 * Purpose: Get max rank for a given set of criteria
 *   This function was developed with the many property tables in chado in mind
 *
 * @param $tablename
 *    The name of the chado table you want to select the max rank from this table must contain a 
 *    rank column of type integer
 * @param $where_options
 *   where options should include the id and type for that table to correctly
 *     group a set of records together where the only difference are the value and rank
 * @code 
 *  array(
 *     <column_name> => array(
 *	      'type' => <type of column: INT/STRING>,
 *		   'value' => <the value you want to filter on>,
 *			'exact' => <if TRUE use =; if FALSE use ~>,
 *		)
 *	)
 * @endcode
 * @return the maximum rank
 *
 * @ingroup tripal_chado_api
 */
function tripal_get_max_chado_rank ($tablename, $where_options) {

	$where= array();
	//generate the where clause from supplied options
	// the key is the column name
	foreach ($where_options as $key => $val_array) {
		if (preg_match('/INT/', $val_array['type'])) {
			$where[] = $key."=".$val_array['value'];
		} else {
			if ($val_array['exact']) { $operator='='; }
			else { $operator='~'; }
			$where[] = $key.$operator."'".$val_array['value']."'";
		}
	}
	
  $previous_db = tripal_db_set_active('chado');
  $result = db_fetch_object(db_query(
    "SELECT max(rank) as max_rank, count(rank) as count FROM %s WHERE %s",
    $tablename,
    implode(' AND ',$where)
  ));
  tripal_db_set_active($previous_db);
	//drupal_set_message("Max Rank Query=SELECT max(rank) as max_rank, count(rank) as count FROM ".$tablename." WHERE ".implode(' AND ',$where));
	if ($result->count > 0) {
	  return $result->max_rank;
	} else {
		return -1;
	}
}
