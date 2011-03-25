<?php
 
/**
 * Implements hook_chado_feature_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the feature table
 *
 * @ingroup tripal_feature
 */
function tripal_feature_chado_feature_schema() {
  $description = array();

  $description['foreign keys']['organism'] = array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
  );
  
  $description['foreign keys']['dbxref'] = array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
  );
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $referring_tables = array('analysisfeature',
    'element',
    'feature_cvterm',
    'feature_dbxref',
    'feature_expression',
    'feature_genotype',
    'feature_phenotype',
    'feature_pub',
    'feature_relationship',
    'feature_synonym',
    'featureloc',
    'featurepos',
    'featureprop',
    'featurerange',
    'library_feature',
    'phylonode',
    'wwwuser_feature'
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

