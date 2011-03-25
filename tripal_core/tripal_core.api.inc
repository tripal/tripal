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
 * Provides an application programming interface (API) for the tripal package
 *
 * The Tripal API currently provides generic insert/update/select functions for all chado content as 
 * well as some module specific functions that insert/update/delete/select specific chado content. 
 *
 * This API is currently in it's infancy so some necessary functions might be missing. If you come 
 * across a missing function that you think should be included go to the sourceforge feature request * page and request it's inclusion in the API. Such feature requests with a working function * definition will be given priority.
 * @}
 */

/** 
 * @defgroup tripal_schema_api Tripal Schema API
 * @{
 * Provides an application programming interface (API) used to describe chado tables to 
 * drupal/tripal
 * @}
 * @ingroup tripal_api
 */
require_once "tripal_core.schema.api.inc";

// just a temporary function for debugging
function tripal_core_chado_insert_test(){
   $ivalues =  array(
     'organism_id' => array(
         'genus' => 'Lens',
         'species' => 'culinaris',
      ),
     'name' => 'orange1.1g000034m.g',
     'uniquename' => 'orange1.1g000034m.g9',
     'type_id' => array (
         'cv_id' => array (
            'name' => 'sequence',
         ),
         'name' => 'gene',
         'is_obsolete' => 0
      ),
   );
   $umatch = array(
     'organism_id' => array(
         'genus' => 'Lens',
         'species' => 'culinaris',
      ),
     'uniquename' => 'orange1.1g000034m.g9',
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
   $select_multiple = array(
     'dbxref_id' => array(
       'db_id' => 2,
     )
   );
   //$result = tripal_core_chado_insert('feature',$ivalues);
   //return "$result->feature_id";
   $result = tripal_core_chado_update('feature',$umatch,$uvalues);
   //$result = tripal_core_chado_select('feature',array('type_id', 'uniquename'),$select_multiple);
   return $result;

}
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
* @ingroup tripal_api
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
           watchdog('tripal_core', 'tripal_core_chado_insert: no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
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
      if($def['not null'] == 1 and !array_key_exists($field,$insert_values) and !$def['default'] and strcmp($def['type'],serial)!=0){
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
      if(strcmp($fields[$field]['type'],'serial')==0 or 
         strcmp($fields[$field]['type'],'int')==0){
         array_push($itypes,"%d");
      } else {
         array_push($itypes,"'%s'");
      }
   }
   $sql = "INSERT INTO {$table} (" . implode(", ",$ifields) . ") VALUES (". implode(", ",$itypes) .")";

   // finally perform the insert. 
   $previous_db = tripal_db_set_active('chado');  // use chado database
   $result = db_query($sql,$ivalues);
   tripal_db_set_active($previous_db);  // now use drupal database 
   if($result){
      return true;
   } 
   else {
      watchdog('tripal_core',"tripal_core_chado_insert: Cannot insert record into $table table: " . print_r($values,1),array(),'WATCHDOG_ERROR');
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
* @ingroup tripal_api
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
           watchdog('tripal_core', 'tripal_core_chado_update: When trying to find record to update, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
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
           watchdog('tripal_core', 'tripal_core_chado_update: When trying to find update values, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)), WATCHDOG_ERROR);
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
      if(strcmp($fields[$field]['type'],'serial')==0 or 
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
      if(strcmp($fields[$field]['type'],'serial')==0 or 
         strcmp($fields[$field]['type'],'int')==0){
         $sql .= " $field = %d AND ";
      } else {
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
*  An associative array containing the values for filtering the results.
* @param $has_record
*  Set this argument to 'true' to have this function return a numeric 
*  value for the number of recrods rather than the array of records.  this
*  can be useful in 'if' statements to check the presence of particula records.
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
*         'species' => 'sinensis',
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
* for the cvterm is nested as well.
*
* @ingroup tripal_api
*/
function tripal_core_chado_select($table,$columns,$values,$has_record = 0){

   if (!is_array($columns)){
      watchdog('tripal_feature', 'the $columns argument for tripal_core_chado_select must be an array.');
      return false;
   }

   if (!is_array($values)){
      watchdog('tripal_feature', 'the $values argument for tripal_core_chado_select must be an array.');
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
         // select the value from the foreign key relationship for this value
         $results = tripal_core_chado_get_foreign_key($table_desc,$field,$value);
         if (sizeof($results) < 1) {
           // foreign key records are required
           // thus if none matched then return false and alert the admin through watchdog
           watchdog('tripal_core', 
            'tripal_core_chado_select: no record in the table referenced by the foreign key (!field)   exists. tripal_core_chado_select table=!table, columns=!columns, values=!values', 
            array('!table' => $table, 
              '!columns' => '<pre>' . print_r($columns, TRUE) . '</pre>', 
              '!values' => '<pre>' . print_r($values, TRUE) . '</pre>',
              '!field' => $field,
            ), 
            WATCHDOG_WARNING);
           return false;
           
         } elseif (sizeof($results) == 1) {
           $where[$field] = $results[0];
         } else {
           $where[$field] = $results;
         }
      } 
      else {
         $where[$field] = $value;
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
       if (is_array($value)) {
         $sql .= "$field IN (".db_placeholders($value,'varchar').") AND ";
         foreach ($value as $v) { $args[] = $v; }
       } else {
         $sql .= "$field = '%s' AND ";
         $args[] = $value;
       }
     }
     $sql = substr($sql,0,-4);  // get rid of the trailing 'AND'
   }

   $previous_db = tripal_db_set_active('chado');  // use chado database
   $resource = db_query($sql,$args);
   tripal_db_set_active($previous_db);  // now use drupal database   

   $results = array();
   while ($r = db_fetch_object($resource)) {
     $results[] = $r;    
   }
   
   if(!$has_record){
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
* @ingroup tripal_api
*/
function tripal_core_chado_get_foreign_key($table_desc,$field,$values){

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
               $result = tripal_core_chado_select($table,$select_cols,$values);
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
      $message = "There is no foreign key relationship defined for ".$field.". To define a foreign key relationship, determine the table this foreign key referrs to (<foreign table>) and then implement hook_chado_<foreign table>_schema(). See tripal_feature_chado_feature_schema for an example.";
      watchdog('tripal_core', $message);
      drupal_set_message($message,'error');      
   }
   return false;
}

/** 
 * Generates an object containing the full details of a record(s) in chado. 
 *
 * @todo Add expand_tables arrays
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
 *      any table. Tokens available in criteria are <field_value> and <field_name>. For example:
 * @code
          mymodule_exclude_type_by_default() {
            return array('text' => 'length(<field_value>) > 50');
          }
 * @endcode
 *      will exclude all text fields with a length > 50. Thus if $feature.residues is longer than 50 *      it will be excluded, otherwise it will be added.
 *
 * @ingroup tripal_api
 */
function tripal_core_generate_chado_var($table, $values) {
  
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
    //replace <field_name> with the current field name & 
    $criteria = preg_replace('/<field_name>/', $field_name, $criteria);

    // if field_value needed we can't deal with this field yet
    if (preg_match('/<field_value>/', $criteria)) { break; }

    //if criteria then remove from query
    $success = drupal_eval('<?php return '.$criteria.'; ?>');
    watchdog('tripal_core', 
      'Evaluating criteria (%criteria) for field %field in tripal_core_generate_chado_var for %table evaluated to %success',
      array('%table' => $table, '%criteria'=>$criteria, '%field' => $field_name, '%success'=>$success),
      WATCHDOG_NOTICE
    );
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
      //replace <field_name> with the current field name & 
      $criteria = preg_replace('/<field_name>/', $field_name, $criteria);
      
      foreach ($field_types[$field_type] as $field_name) {
        // if field_value needed we can't deal with this field yet
        if (preg_match('/<field_value>/', $criteria)) { 
          $fields_to_remove[$field_name] = $criteria;
          continue; 
        }

        // if field_value needed we can't deal with this field yet
        if (preg_match('/<field_value>/', $criteria)) { break; }

        //if criteria then remove from query
        $success = drupal_eval('<?php return '.$criteria.'; ?>');
        watchdog('tripal_core', 
          'Evaluating criteria (%criteria) for field %field of $type in tripal_core_generate_chado_var for %table evaluated to %success',
          array('%table'=>$table, '%criteria'=>$criteria, '%field'=>$field_name, '%type'=>$field_type, '%success'=>$success),
          WATCHDOG_NOTICE
        );
        if ($success) {
          unset($table_columns[array_search($field_name, $table_columns)]);
          $all->expandable_fields[] = $table . '.' . $field_name;
        }
      } //end of foreach field of that type
    }
  } //end of foreach type to be removed
  
  // get the values for the record in the current table---------------------------------------------
  $results = tripal_core_chado_select($table, $table_columns, $values);   
  
  foreach ($results as $key => $object) {
    // Add empty expandable_x arrays
    $object->expandable_fields = $all->expandable_fields;
    $object->expandable_tables = $all->expandable_tables;
    $object->expandable_nodes = $all->expandable_nodes;
    
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
      $criteria = preg_replace('/<field_value>/', $object->{$field_name}, $criteria);
      //if criteria then remove from query
      $success = drupal_eval('<?php return '.$criteria.'; ?>');
      watchdog('tripal_core', 
        'Evaluating criteria (%criteria) for field %field in tripal_core_generate_chado_var for   %table evaluated to %success',
        array('%table' => $table, '%criteria'=>$criteria, '%field' => $field_name, '%success'=>$success),
        WATCHDOG_NOTICE
      );
      if ($success) {
        unset($object->{$field_name});
        $object->expandable_fields[] = $table . '.' . $field_name;
      }      
    }
    
    // recursively follow foreign key relationships nesting objects as we go------------------------
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
        $object->{$foreign_key}->tablename = $foreign_table;
        
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
  
    // check only one result returned
  if (sizeof($results) == 1) {
    // add results to object
    return $results[0];
  } elseif (!empty($results)) {
    return $results;
  } else {
    // no results returned
    watchdog('tripal_core',
      'tripal_core_generate_chado_var for %table: No record matches criteria values:%values',
      array('%table'=>$table, '%values'=>print_r($values,TRUE)),
      WATCHDOG_ERROR
    );  
  } 
  
}

/**
 * Retrieves fields/tables/nodes that were excluded by default from a variable and adds them
 *
 * This function exists to allow tripal_core_generate_chado_var() to excldue some 
 * fields/tables/nodes from the default form of a variable without making it extremely difficult for 
 * the tripal admin to get at these variables if he/she wants them.
 *
 * @ingroup tripal_api
 */
function tripal_core_expand_chado_vars () {

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
 *   - <field_name>
 *       Replaced by the name of the field to be excluded
 *   - <field_value>
 *       Replaced by the value of the field in the current record
 * Also keep in mind that if your criteria doesn't contain the <field_value> token then it will be 
 * evaluated before the query is executed and if the field is excluded it won't be included in the 
 * query.
 *
 * @return
 *   An array of type => criteria where the type is excluded if the criteria evaluates to TRUE
 *
 * @ingroupt tripal_api
 */
function tripal_core_exclude_type_by_default() {
  return array('text' => "strlen('<field_value>') > 100");
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
 *   - <field_name>
 *       Replaced by the name of the field to be excluded
 *   - <field_value>
 *       Replaced by the value of the field in the current record
 * Also keep in mind that if your criteria doesn't contain the <field_value> token then it will be 
 * evaluated before the query is executed and if the field is excluded it won't be included in the 
 * query.
 *
 * @return
 *   An array of type => criteria where the type is excluded if the criteria evaluates to TRUE
 *
 * @ingroupt tripal_api
 */
function tripal_core_exclude_field_from_feature_by_default() {
  return array();
}