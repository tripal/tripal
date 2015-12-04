<?php

/**
 * Because the nd_experiment_id can be associated with projects, phenotypes,
 * stocks, genotypes, etc. it becomes slow to allow the template to use the
 * chado_expand_var function to traverse the FK relationships.  Ideally
 * we would use the chado_expand_var function to only return a subset of 
 * paged results.  This function queries for all of the nd_experiment_genotypes
 * that share an nd_experiment_id with a stock
 * 
 * @param $variables
 *   The list of variables that will be passed into the template
 */
function tripal_natural_diversity_preprocess_tripal_stock_nd_genotypes(&$variables) {
  $stock = $variables['node']->stock;

  // because the nd_experiment_id value can be used to associate projects, genotypes,
  // phenotypes, etc, we must do an inner join to only pull out those that
  // associate a stock to a genotype.
  
  $sql = "
    SELECT NDEG.nd_experiment_genotype_id
    FROM {nd_experiment_stock} NDES
      INNER JOIN {nd_experiment_genotype} NDEG ON NDEG.nd_experiment_id = NDES.nd_experiment_id
    WHERE NDES.stock_id = :stock_id
  ";
  $results = chado_query($sql, array(':stock_id' => $stock->stock_id));
  $nd_exp_ids = array();
  foreach ($results as $result) {
    $nd_exp_ids[] = $result->nd_experiment_genotype_id;
  }
  $stock->nd_experiment_genotype_ids = $nd_exp_ids;
}

/**
 * Because the nd_experiment_id can be associated with projects, phenotypes,
 * stocks, genotypes, etc. it becomes slow to allow the template to use the
 * chado_expand_var function to traverse the FK relationships.  Ideally
 * we would use the chado_expand_var function to only return a subset of
 * paged results.  This function queries for all of the nd_experiment_phenotypes
 * that share an nd_experiment_id with a stock
 *
 * @param $variables
 *   The list of variables that will be passed into the template
 */
function tripal_natural_diversity_preprocess_tripal_stock_nd_phenotypes(&$variables) {
  $stock = $variables['node']->stock;

  // because the nd_experiment_id value can be used to associate projects, phenotypes,
  // phenotypes, etc, we must do an inner join to only pull out those that
  // associate a stock to a phenotype.

  $sql = "
    SELECT NDEP.nd_experiment_phenotype_id
    FROM {nd_experiment_stock} NDES
      INNER JOIN {nd_experiment_phenotype} NDEP ON NDEP.nd_experiment_id = NDES.nd_experiment_id
    WHERE NDES.stock_id = :stock_id
  ";
  $results = chado_query($sql, array(':stock_id' => $stock->stock_id));
  $nd_exp_ids = array();
  foreach ($results as $result) {
    $nd_exp_ids[] = $result->nd_experiment_phenotype_id;
  }
  $stock->nd_experiment_phenotype_ids = $nd_exp_ids;
}