<?php
/**
 * @file
 * Add to the tripal schema definitions
 */

/**
 * Implements hook_chado_schema_v1_11_table()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the genotype table
 *
 * @ingroup tripal_schema_api
 */
function tripal_genetic_chado_schema_v1_11_genotype() {
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