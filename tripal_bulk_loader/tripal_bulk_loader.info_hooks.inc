<?php

/**
 * Implements hook_tripal_bulk_loader_supported_modules()
 * This is used to supply the base tables or modules supported by the bulk loader.
 * If a base table is included in this list there must also be a corresponding 
 * implementation of hook_tripal_bulk_loader_<base table>_related_tables()
 * 
 * @return 
 *   An array of <chado table> => <Human-readable Name> describing the modules supported
 */
function tripal_bulk_loader_tripal_bulk_loader_supported_modules() {
  return array(
    'feature' => 'Feature',
    'organism' => 'Organism',
    'library' => 'Library',
    'analysis' => 'Analysis',
    'stock' => 'Stocks',
    'genotype' => 'Genotypes',
  );
}

/**
 * Implements hook_tripal_bulk_loader_<base table>_related_tables()
 * This is used to supply the bulk loader with a list of tables and their 
 * relationship to the base table.
 *
 * @return
 * An array of relationships (any combination of 'Base Table', 'Foreign Key Relations', 
 * 'Direct Relations', 'Indirect Relations') where each relationship is keyed by the type
 * of relationship and the value is an array of tables with that relationship to the base table.
 * Each table in the array of tables with that relationship should be of the form
 * <chado table> => <Human-readable Name>.
 */
function tripal_bulk_loader_tripal_bulk_loader_feature_related_tables () {

  return array(
    'Base Table' => array(
      'feature' => 'Feature'
    ),
    'Foreign Key Relations' => array(
      'organism' => 'Organism',
      'cvterm' => 'Type: Cv Term',
      'dbxref' => 'Database Reference',
    ),
    'Direct Relations' => array(
      'element' => 'Element',
      'featureloc' => 'Feature Locations',
      'featurepos' => 'Feature Positions on Map',
      'featureprop' => 'Feature Properties',
      'featurerange' => 'Feature Range',
      'feature_relationship' => 'Feature Relationships',
      'polynode' => 'Polynode'
    ),
    'Indirect Relations' => array(
      'analysisfeature' => 'Analysis',
      'feature_cvterm' => 'Additional Cv Terms',
      'feature_dbxref' => 'Additional Database References',
      'feature_expression' => 'Feature Expressions',
      'feature_genotype' => 'Genotypes',
      'library_feature' => 'Libraries',
      'feature_phenotype' => 'Phenotypes',
      'feature_pub' => 'Publications',
      'feature_synonym' => 'Synonyms',
    ),
  );

}

/**
 * Implements hook_tripal_bulk_loader_<base table>_related_tables()
 * This is used to supply the bulk loader with a list of tables and their 
 * relationship to the base table.
 *
 * @return
 * An array of relationships (any combination of 'Base Table', 'Foreign Key Relations', 
 * 'Direct Relations', 'Indirect Relations') where each relationship is keyed by the type
 * of relationship and the value is an array of tables with that relationship to the base table.
 * Each table in the array of tables with that relationship should be of the form
 * <chado table> => <Human-readable Name>.
 */
function tripal_bulk_loader_tripal_bulk_loader_organism_related_tables () {
  return array(
    'Base Table' => array(
      'feature' => 'Organism'
    ),
    'Direct Relations' => array(
      'biomaterial' => 'Biological Material',
      'feature' => 'Features',
      'library' => 'Libraries',
      'organismprop' => 'Organism Properties',
      'stock' => 'Stocks'
    ),
    'Indirect Relations' => array(
      'organism_dbxref' => 'Additional Database References',
      'polynode_organism' => 'Polynodes',
    ),
  );
}

/**
 * Implements hook_tripal_bulk_loader_<base table>_related_tables()
 * This is used to supply the bulk loader with a list of tables and their 
 * relationship to the base table.
 *
 * @return
 * An array of relationships (any combination of 'Base Table', 'Foreign Key Relations', 
 * 'Direct Relations', 'Indirect Relations') where each relationship is keyed by the type
 * of relationship and the value is an array of tables with that relationship to the base table.
 * Each table in the array of tables with that relationship should be of the form
 * <chado table> => <Human-readable Name>.
 */
function tripal_bulk_loader_tripal_bulk_loader_library_related_tables () {
  return array(
    'Base Table' => array(
      'library' => 'Library'
    ),
    'Foreign Key Relations' => array(
      'organism' => 'Organism',
      'cvterm' => 'Type: Cv Term',
    ),
    'Direct Relations' => array(
      'libraryprop' => 'Library Properties',
    ),
    'Indirect Relations' => array(
      'library_cvterm' => 'Additional Cv Terms',
      'library_feature' => 'Features',
      'library_pub' => 'Publications',
      'library_synonym' => 'Synonyms',
    ),
  );
}

/**
 * Implements hook_tripal_bulk_loader_<base table>_related_tables()
 * This is used to supply the bulk loader with a list of tables and their 
 * relationship to the base table.
 *
 * @return
 * An array of relationships (any combination of 'Base Table', 'Foreign Key Relations', 
 * 'Direct Relations', 'Indirect Relations') where each relationship is keyed by the type
 * of relationship and the value is an array of tables with that relationship to the base table.
 * Each table in the array of tables with that relationship should be of the form
 * <chado table> => <Human-readable Name>.
 */
function tripal_bulk_loader_tripal_bulk_loader_analysis_related_tables () {
  return array(
    'Base Table' => array(
      'analysis' => 'Analysis'
    ),
    'Direct Relations' => array(
      'analysisprop' => 'Analysis Properties',
      'phylotree' => 'Phylogeny Tree',
      'quantification' => 'Quantification'
    ),
    'Indirect Relations' => array(
      'analysis_feature' => 'Features',
    ),
  );
}

/**
 * Implements hook_tripal_bulk_loader_<base table>_related_tables()
 * This is used to supply the bulk loader with a list of tables and their 
 * relationship to the base table.
 *
 * @return
 * An array of relationships (any combination of 'Base Table', 'Foreign Key Relations', 
 * 'Direct Relations', 'Indirect Relations') where each relationship is keyed by the type
 * of relationship and the value is an array of tables with that relationship to the base table.
 * Each table in the array of tables with that relationship should be of the form
 * <chado table> => <Human-readable Name>.
 */
function tripal_bulk_loader_tripal_bulk_loader_stock_related_tables () {

  return array(
    'Base Table' => array(
      'stock' => 'Stock'
    ),
    'Foreign Key Relations' => array(
      'organism' => 'Organism',
      'cvterm' => 'Type: Cv Term',
      'dbxref' => 'Database Reference',
    ),
    'Direct Relations' => array(
      'stockprop' => 'Stock Properties',
      'stock_relationship' => 'Stock Relationships',
    ),
    'Indirect Relations' => array(
      'stock_cvterm' => 'Additional Cv Terms',
      'stock_dbxref' => 'Additional Database References',
      'stock_genotype' => 'Genotypes',
      'stock_pub' => 'Publications',
      'stockcollection_stock' => 'Stock Collections'
    ),
  );

}

/**
 * Implements hook_tripal_bulk_loader_<base table>_related_tables()
 * This is used to supply the bulk loader with a list of tables and their 
 * relationship to the base table.
 *
 * @return
 * An array of relationships (any combination of 'Base Table', 'Foreign Key Relations', 
 * 'Direct Relations', 'Indirect Relations') where each relationship is keyed by the type
 * of relationship and the value is an array of tables with that relationship to the base table.
 * Each table in the array of tables with that relationship should be of the form
 * <chado table> => <Human-readable Name>.
 */
function tripal_bulk_loader_tripal_bulk_loader_genotype_related_tables () {

  return array(
    'Base Table' => array(
      'genotype' => 'Genotype'
    ),
    'Indirect Relations' => array(
      'feature_genotype' => 'Features',
      'nd_experiment_genotype' => 'ND Experiments',
      'phendesc' => 'Phenotypic Statements',
      'phenotype_comparison' => 'Phenotype Comparisons',
      'phenstatement' => 'Phenotypic Statements',
      'stock_genotype' => 'Stocks',
    ),
  );

}

