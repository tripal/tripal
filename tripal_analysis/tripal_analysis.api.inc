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
function tripal_stock_chado_analysisfeature_schema() {
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
function tripal_stock_chado_analysisprop_schema() {
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