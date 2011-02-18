<?php
/**
* @file
* The Tripal Core API
*
* This file provides the API needed for all other Tripal and Tripal dependent
* modules.
*/

require_once "chado_tables.schema.inc";

// just a temporary function for debugging
function tripal_core_chado_insert_test(){
   $values =  array(
     'organism_id' => array(
         'genus' => 'Citrus',
         'species' => 'sinensis',
      ),
     'name' => 'orange1.1g000034m.g',
     'uniquename' => 'orange1.1g000034m.g7',
     'type_id' => array (
         'cv_id' => array (
            'name' => 'sequence',
         ),
         'name' => 'gene',
         'is_obsolete' => 0
      ),
   );
   $result = tripal_core_chado_insert('feature',$values);
   return "$result->feature_id";
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
*  On success this function returns an object containing the serial, or
*  incremental fields (such as primary keys) for the record that was just
*  inserted. On failure, it returns FALSE.
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
   $chado = tripal_core_get_chado_schema();
   $table_desc = $chado[$table];

   // iterate through the values array and create a new 'insert_values' array
   // that has all the values needed for insert with all foreign relationsihps
   // resolved.
   foreach($values as $field => $value){
      if(is_array($value)){
         // select the value from the foreign key relationship for this value
         $insert_values[$field] = tripal_core_chado_get_foreign_key($table_desc,$field,$value);
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
      if(db_fetch_object(tripal_core_chado_select($table,$ukselect_cols,$ukselect_vals))){
         watchdog('tripal_core',"Cannot insert duplicate record into $table table: " . print_r($values,1),array(),'WATCHDOG_ERROR');
         return false;
      }
   }

   // if trying to insert a field that is the primary key, make sure it also is unique
   $pkey = $table_desc['primary key'][0];
   if($insert_values[$pkey]){
      if(db_fetch_object(tripal_core_chado_select($table,array($pkey),array($pkey => $insert_values[$pkey])))){
         watchdog('tripal_core',"Cannot insert duplicate primary key into $table table: " . print_r($values,1),array(),'WATCHDOG_ERROR');
         return false;
      }
   }

   // make sure required fields have a value and get any fields that are of type serial
   $fields = $table_desc['fields'];
   $serials = array();
   foreach($fields as $field => $def){
      // a field is considered missing if it cannot be null and there is no default
      // value for it or it is of type 'serial'
      if($def['not null'] == 1 and !$insert_values[$field] and !$def['default'] and strcmp($def['type'],serial)!=0){
         watchdog('tripal_core',"Field $field cannot be null: " . print_r($values,1),array(),'WATCHDOG_ERROR');
         return false;
      }
      // get a list of the serial types
      if(strcmp($fields[$field]['type'],'serial')==0){
         array_push($serials,$field);
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
   $sql = "INSERT INTO $table (" . implode(", ",$ifields) . ") VALUES (". implode(", ",$itypes) .")";

   // finally perform the insert.  If successful, return an object with the
   // primary keys (or all serial types) populated with the values from the
   // insert
   $object = array();
   if(db_query($sql,$ivalues)){
      $object = (object) $object; // convert the array to an object
      foreach ($serials as $field) {
         $object->$field = db_last_insert_id($table, $field);
      }
      return $object;
   } 
   else {
      watchdog('tripal_core',"Cannot insert record into $table table: " . print_r($values,1),array(),'WATCHDOG_ERROR');
      return false;
   }
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
   $chado = tripal_core_get_chado_schema();
   $table_desc = $chado[$table];

   $select = '';
   $from = ''; 
   $where = '';
   $args = array();
   foreach($values as $field => $value){
      $select[] = $field;
      if(is_array($value)){
         // select the value from the foreign key relationship for this value
         $where[$field] = tripal_core_chado_get_foreign_key($table_desc,$field,$value);
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
      $sql .= "$field = '%s' AND ";
      $args[] = $value;
   }
   $sql .= " 1=1"; // just add a 1=1 so we don't have trim off the last 'AND'
   return db_query($sql,$args);
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
*  foreign key relationship that needs to be identified.
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
         $table = $def['table'];
         $columns = $def['columns'];
         // iterate through the columns of the foreign key relationship
         foreach($columns as $left => $right){
            // does the left column in the relationship match our field?
            if(strcmp($field,$left)==0){
               // the column name of the foreign key matches the field we want 
               // so this is the right relationship.  Now we want to select
               $select_cols = array($right);
               $result = db_fetch_object(tripal_core_chado_select($table,$select_cols,$values));
               return $result->$right;
            }
         }
      } 
   } 
   else {
      // TODO: what do we do if we get to this point and we have a fk 
      // relationship expected but we don't have any definition for one in the
      // table schema??
   }
   return false;
}
