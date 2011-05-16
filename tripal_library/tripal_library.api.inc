<?php

/**
 * Implements hook_chado_library_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the library table
 *
 * @ingroup tripal_library
 */
function tripal_library_chado_library_schema() {
  $description = array();
  
  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_feature_schema()

  $description['foreign keys']['organism'] = array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
  );  
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $referring_tables = array(
      'library_cvterm',
      'library_feature',
      'library_pub',
      'library_synonym',
      'libraryprop' 
  );
  $description['referring_tables'] = $referring_tables;
  
  return $description;
}

/**
 * Implements hook_chado_library_feature_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the library_feature table
 *
 * @ingroup tripal_library
 */
function tripal_library_chado_library_feature_schema() {
  $description = array();
  
  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_library_feature_schema()

  $description['foreign keys']['library'] = array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
  );  
  
  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
  );
  
  return $description;
}
/**
 * Implements hook_chado_libraryprop_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the libraryprop table
 *
 * @ingroup tripal_library
 */
function tripal_library_chado_libraryprop_schema() {
  $description = array();
  
  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_library_feature_schema()

  $description['foreign keys']['library'] = array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
  );  
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  return $description;
}
/**
* Adds a single property to an existing library record.
*
* @ingroup tripal_api
*/
function tripal_library_get_property($library_id,$property){
   // construct the array of values to be inserted  
   $values = array (
      'library_id' => $library_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => 'tripal',
         ),
         'name' => $property,
         'is_obsolete' => 0
      ),
   );
   $results = tripal_core_generate_chado_var('libraryprop',$values);
   $results = tripal_core_expand_chado_vars($results,'field','libraryprop.value');
   return $results;
}
/**
* Adds a single property to an existing library record.
*
* @ingroup tripal_api
*/
function tripal_library_insert_property($library_id,$property,$value,$update_if_present = 0){
   // first see if the property already exists, if so we can't insert
   $prop = tripal_library_get_property($library_id,$property);
   if(count($prop)>0){
      if($update_if_present){
        return tripal_library_update_property($library_id,$property,$value) ;
      } else {
        return FALSE;
      }
   }

   // construct the array of values to be inserted  
   $values = array (
      'library_id' => $library_id,
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
   return tripal_core_chado_insert('libraryprop',$values);
}
/**
* Adds a single property to an existing library record.
*
* @ingroup tripal_api
*/
function tripal_library_update_property($library_id,$property,$value,$insert_if_missing = 0){

   // first see if the property is missing (we can't update a missing property
   $prop = tripal_library_get_property($library_id,$property);
   if(count($prop)==0){
      if($insert_if_missing){
        return tripal_library_insert_property($library_id,$property,$value);
      } else {
        return FALSE;
      }
   }

   // construct the array that will match the exact record to update
   $match = array (
      'library_id' => $library_id,
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
   return tripal_core_chado_update('libraryprop',$match,$values);
}
/**
* Adds a single property to an existing library record.
*
* @ingroup tripal_api
*/
function tripal_library_delete_property($library_id,$property){


   // construct the array that will match the exact record to update
   $match = array (
      'library_id' => $library_id,
      'type_id' => array ( 
         'cv_id' => array (
            'name' => 'tripal',
         ),
         'name' => $property,
      ),
   );
   return tripal_core_chado_delete('libraryprop',$match);
}
