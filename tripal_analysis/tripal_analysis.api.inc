<?php

/****************************************************************************
 * @section Chado Table Descriptions
 ****************************************************************************/
  
 /****************************************************************************
 * Implements hook_chado_analysisfeature_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the analysisfeature table
 */
function tripal_analysis_chado_analysisfeature_schema() {
  $description = array();

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
  );

  $description['foreign keys']['analysis'] = array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
  );
  
  return $description;
}

 /****************************************************************************
 * Implements hook_chado_analysisprop_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the analysisprop table
 */
function tripal_analysis_chado_analysisprop_schema() {
  $description = array();

  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );

  $description['foreign keys']['analysis'] = array(
        'table' => 'analysis',
        'columns' => array(
          'analysis_id' => 'analysis_id',
        ),
  );

  return $description;
}
/**
* Adds a single property to an existing analysis record.
*
* @ingroup tripal_api
*/
function tripal_analysis_get_property($analysis_id,$property){
   // construct the array of values to be inserted  
   $values = array (
      'analysis_id' => $analysis_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => 'tripal',
         ),
         'name' => $property,
         'is_obsolete' => 0
      ),
   );
   $columns = array('type_id','value');
   $results = tripal_core_chado_select('analysisprop',$columns,$values);
   // this next bit is a hack until we get the recursive add vars routine written
   foreach($results as $prop){
      $prop->type_id = new stdClass();
      $prop->type_id->table_name = 'cvterm';
      $prop->type_id->name = $property;
   }
   return $results;
}
/**
* Adds a single property to an existing analysis record.
*
* @ingroup tripal_api
*/
function tripal_analysis_insert_property($analysis_id,$property,$value){
   // construct the array of values to be inserted  
   $values = array (
      'analysis_id' => $analysis_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => 'tripal',
         ),
         'name' => $property,
         'is_obsolete' => 0
      ),
      'value' => $value, 
      'rank' => 0,
   );
   return tripal_core_chado_insert('analysisprop',$values);
}
/**
* Adds a single property to an existing analysis record.
*
* @ingroup tripal_api
*/
function tripal_analysis_update_property($analysis_id,$property,$value){
   // construct the array that will match the exact record to update
   $match = array (
      'analysis_id' => $analysis_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => 'tripal',
         ),
         'name' => $property,
      ),
   );
   // construct the array of values to be updated
   $values = array (      
      'value' => $value, 
   );
   return tripal_core_chado_update('analysisprop',$match,$values);
}
