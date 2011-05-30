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
 * @ingroup tripal_analysis_api
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
 * @ingroup tripal_analysis_api
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
 * @ingroup tripal_analysis_api
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
 * @ingroup tripal_analysis_api
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
*
 *
 * @ingroup tripal_analysis_api
*/
function tripal_analysis_get_property($analysis_id,$property){
   return tripal_core_get_property('analysis',$analysis_id,$property,'tripal');
}
/**
*
 *
 * @ingroup tripal_analysis_api
*/
function tripal_analysis_insert_property($analysis_id,$property,$value,$update_if_present = 0){
    return tripal_core_insert_property('analysis',$analysis_id,$property,'tripal',$value,$update_if_present);
}
/**
*
 *
 * @ingroup tripal_analysis_api
*/
function tripal_analysis_update_property($analysis_id,$property,$value,$insert_if_missing = 0){
   return tripal_core_update_property('analysis',$analysis_id,$property,'tripal',$value, $insert_if_missing);
}
/**
*
 *
 * @ingroup tripal_analysis_api
*/
function tripal_analysis_delete_property($analysis_id,$property){
   return tripal_core_delete_property('analysis',$analysis_id,$property,'tripal');
}
