<?php 

/**
 *
 */
function tripal_genetic_get_genotypes_by_feature_id ($feature_id) {

  /**
  $sql = 'SELECT nd_experiment_id FROM nd_experiment_genotype WHERE genotype_id IN (SELECT genotype_id FROM feature_genotype WHERE feature_id=%d)';
  $resource = db_query($sql, $feature_id);

  $values['nd_experiment_id'] = array();
  while ($r = db_fetch_object($resource)) {
    $values['nd_experiment_id'][] = $r->nd_experiment_id;
  }
  
  $object = tripal_core_generate_chado_var('nd_experiment',$values);
  $object = tripal_core_expand_chado_vars($object, 'table', 'nd_experiment_genotype');
  $object = tripal_core_expand_chado_vars($object, 'table', 'feature_genotype');
  $object = tripal_core_expand_chado_vars($object, 'table', 'nd_experiment_stock');
  */

  $sql = 'SELECT genotype_id FROM genotype WHERE genotype_id IN (SELECT genotype_id FROM feature_genotype WHERE feature_id=%d)';
  $resource = db_query($sql, $feature_id);

  $values['genotype_id'] = array();
  while ($r = db_fetch_object($resource)) {
    $values['genotype_id'][] = $r->genotype_id;
  }
  
  if (!empty($values['genotype_id'])) {
    $object = tripal_core_generate_chado_var('genotype',$values);  
  } else {
    $object = array();
  }
  return $object;
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