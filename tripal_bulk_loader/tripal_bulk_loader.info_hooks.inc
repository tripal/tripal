<?php

/**
 * Implements hook_tripal_bulk_loader_supported_modules
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
  );
}

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