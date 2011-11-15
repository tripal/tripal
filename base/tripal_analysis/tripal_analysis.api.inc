<?php

/**
 * @defgroup tripal_analysis_api Analysis Module API
 * @ingroup tripal_analysis
 * @ingroup tripal_api
 */
/****************************************************************************
 * @section Chado Table Descriptions
 ****************************************************************************/
/**
 * Implements hook_chado_analysis_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the analysis table
 *
 * @ingroup tripal_schema_api
 */
function tripal_analysis_chado_analysis_schema() {
  $description = array();
  
  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_analysis_schema()
  
  $referring_tables = array(
      'analysisfeature',
      'analysisprop',
      'phylotree',
      'quantification'
  );
  $description['referring_tables'] = $referring_tables;
  
  return $description;
}

 /****************************************************************************
 * Implements hook_chado_analysisfeature_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the analysisfeature table
 *
 * @ingroup tripal_schema_api
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
 * Implements hook_chado_analysisfeatureprop_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the analysisfeatureprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_analysis_chado_analysisfeatureprop_schema() {
  $description = array();

  $description['foreign keys']['analysisfeature'] = array(
        'table' => 'analysisfeature',
        'columns' => array(
          'analysisfeature_id' => 'analysisfeature_id',
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


 /****************************************************************************
  * Implements hook_chado_analysisprop_schema()
  * Purpose: To add descriptions and foreign keys to default table description
  * Note: This array will be merged with the array from all other implementations
  *
  * @return
  *    Array describing the analysisprop table
  *
  * @ingroup tripal_schema_api
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
 * Retrieve properties of a given type for a given analysis
 *
 * @param $analysis_id
 *    The analysis_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 *
 * @return
 *    An analysis chado variable with the specified properties expanded
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_get_property($analysis_id,$property){
   return tripal_core_get_property('analysis',$analysis_id,$property,'tripal');
}

/**
 * Insert a given property
 *
 * @param $analysis_id
 *   The analysis_id of the property to insert
 * @param $property
 *   The cvterm name of the property to insert
 * @param $value
 *   The value of the property to insert
 * @param $update_if_present
 *   A boolean indicated whether to update the record if it's already present
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_insert_property($analysis_id,$property,$value,$update_if_present = 0){
    return tripal_core_insert_property('analysis',$analysis_id,$property,'tripal',$value,$update_if_present);
}

/**
 * Update a given property
 *
 * @param $analysis_id
 *   The analysis_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing 
 *   A boolean indicated whether to insert the record if it's absent
 *
 * Note: The property will be identified using the unique combination of the $analysis_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_update_property($analysis_id,$property,$value,$insert_if_missing = 0){
   return tripal_core_update_property('analysis',$analysis_id,$property,'tripal',$value, $insert_if_missing);
}

/**
 * Delete a given property
 *
 * @param $analysis_id
 *   The analysis_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 *
 * Note: The property will be identified using the unique combination of the $analysis_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise 
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_delete_property($analysis_id,$property){
   return tripal_core_delete_property('analysis',$analysis_id,$property,'tripal');
}
/**
 * Retreives the node of a sync'ed analysis
 *
 * @param $analysis_id
 *   The analysis_id of the property to delete
 *
 * @return
 *   node of analysis on success, null otherwise 
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_get_node($analysis_id){
   $sql = "SELECT * 
           FROM chado_analysis CA
              INNER JOIN node N on CA.nid = N.nid
           WHERE analysis_id = %d";
   $node = db_fetch_object(db_query($sql,$analysis_id));
   return $node;
}
