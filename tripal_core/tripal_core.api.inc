<?php
/**
* @file
* The Tripal Core API
*
* This file provides the API needed for all other Tripal and Tripal dependent
* modules.
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
   if(db_query($sql,$ivalues)){
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
   if(db_query($sql,$uargs)){
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
* 
* @return
*  A database query result resource, or FALSE if the query was not executed 
*  correctly. 
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
*/
function tripal_core_chado_select($table,$columns,$values){

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
   
   $resource = db_query($sql,$args);
   $results = array();
   while ($r = db_fetch_object($resource)) {
     $results[] = $r;    
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
*  A table description in Drupal Schema API format for the table with the
*  foreign key relationship that needs to be identified. This field is generated
*  by hook_chado_<table name>_schema(). If more than one module implements a hook
*  the two arrays will be merged. (ie: if the same foreign key is defined in two modules
*  then $table_desc['foreign keys'][<table>]['table'] will be an array as will 
*  $table_desc['foreign keys'][<table>]['columns'] -use the one with the highest index)
* @param $field
*  The field in the table that is the foreign key.
* @param $values
*  An associative array containing the values 
* 
* @return
*  A string containg the results of the foreign key lookup, or FALSE if 
*  failed.
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

