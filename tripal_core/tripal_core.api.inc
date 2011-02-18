<?php

require_once "chado_tables.schema.inc";

function tripal_core_chado_insert_test(){
   $values =  array(
     'organism_id' => array(
         'genus' => 'Citrus',
         'species' => 'sinensis',
      ),
     'name' => 'orange1.1g000034m.g',
     'uniquename' => 'orange1.1g000034m.g',
     'type_id' => array (
         'cv_id' => array (
            'name' => 'sequence',
         ),
         'name' => 'gene',
         'is_obsolete' => 0
      ),
   );
   $result = tripal_core_chado_insert('feature',$values);
   return "<pre>$result</pre>";
}
/************************************************************************
*
*/
function tripal_core_chado_insert($table,$values){
   $chado = tripal_core_get_chado_schema();
   $insert_values = array();
   
   // get the table description
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

   return print_r($insert_values,1) . " " .print_r($values,1) . " " . print_r($table_desc,1) ;
}
/************************************************************************
*
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
               $result = tripal_core_chado_select($table,$select_cols,$values);
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
}
/************************************************************************
*
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
   return db_fetch_object(db_query($sql,$args));
}
