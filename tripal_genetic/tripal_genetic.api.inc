<?php 

/**
 * Implements hook_chado_genotype_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the genotype table
 *
 * @ingroup tripal_schema_api
 */
function tripal_genetic_chado_genotype_schema () {
  $description = array();
 
  $referring_tables = array('analysisfeature',
    'feature_genotype',
    'phendesc',
    'phenotype_comparison',
    'phenstatement',
    'stock_genotype',
  );
  $description['referring_tables'] = $referring_tables;
  
  return $description;
  
}

/**
 * Implements hook_chado_genotype_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the genotype table
 *
 * @ingroup tripal_schema_api
 */
function tripal_genetic_chado_feature_genotype_schema () {
  $description = array();

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
          'chromosome_id' => 'feature_id',
        ),
  ); 

  $description['foreign keys']['genotype'] = array(
        'table' => 'genotype',
        'columns' => array(
          'genotype_id' => 'genotype_id',
        ),
  );  

  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'cvterm_id' => 'cvterm_id',
        ),
  );
  
  return $description;
  
}