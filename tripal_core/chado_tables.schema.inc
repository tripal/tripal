<?php

/**
 * Retrieves the schema for all tables
 * This function is deprecated
 *
 * @ingroup tripal_schema_api
 */
function tripal_core_get_chado_schema (){

   $schema['organism_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'organism_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('organism_dbxref_id'),
     'unique keys' => array(
       'organism_dbxref_c1' => array('organism_id', 'dbxref_id')
     ),
     'indexes' => array(
       'organism_dbxref_idx1' => array('organism_id'),
       'organism_dbxref_idx2' => array('dbxref_id'),
     ),
   );
   $schema['organismprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'organismprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('organismprop_id'),
     'unique keys' => array(
       'organismprop_c1' => array('organism_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'organismprop_idx1' => array('organism_id'),
       'organismprop_idx2' => array('type_id'),
     ),
   );
   $schema['phendesc'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phendesc_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'genotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'environment_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phendesc_id'),
     'unique keys' => array(
       'phendesc_c1' => array('genotype_id', 'environment_id', 'type_id', 'pub_id')
     ),
     'indexes' => array(
       'phendesc_idx1' => array('genotype_id'),
       'phendesc_idx2' => array('environment_id'),
       'phendesc_idx3' => array('pub_id'),
     ),
   );
   $schema['phenotype'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phenotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'observable_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'attr_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'cvalue_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('phenotype_id'),
     'unique keys' => array(
       'phenotype_c1' => array('uniquename')
     ),
     'indexes' => array(
       'phenotype_idx1' => array('cvalue_id'),
       'phenotype_idx2' => array('observable_id'),
       'phenotype_idx3' => array('attr_id'),
     ),
   );
   $schema['organism'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'abbreviation' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'genus' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'species' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'common_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'comment' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('organism_id'),
     'unique keys' => array(
       'organism_c1' => array('genus', 'species')
     ),
   );
   $schema['materialized_view'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'materialized_view_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'last_update' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
       ),
       'refresh_time' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '64',
         'not null' => FALSE,
       ),
       'mv_schema' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '64',
         'not null' => FALSE,
       ),
       'mv_table' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '128',
         'not null' => FALSE,
       ),
       'mv_specs' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'indexed' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'query' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'special_index' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'unique keys' => array(
       'name' => array('name')
     ),
   );
   $schema['libraryprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'libraryprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('libraryprop_id'),
     'unique keys' => array(
       'libraryprop_c1' => array('library_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'libraryprop_idx1' => array('library_id'),
       'libraryprop_idx2' => array('type_id'),
     ),
   );

   $schema['libraryprop_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'libraryprop_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'libraryprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('libraryprop_pub_id'),
     'unique keys' => array(
       'libraryprop_pub_c1' => array('libraryprop_id', 'pub_id')
     ),
     'indexes' => array(
       'libraryprop_pub_idx1' => array('libraryprop_id'),
       'libraryprop_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['magedocumentation'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'magedocumentation_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'mageml_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'tableinfo_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'row_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'mageidentifier' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('magedocumentation_id'),
     'indexes' => array(
       'magedocumentation_idx1' => array('mageml_id'),
       'magedocumentation_idx2' => array('tableinfo_id'),
       'magedocumentation_idx3' => array('row_id'),
     ),
   );
   $schema['mageml'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'mageml_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'mage_package' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'mage_ml' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('mageml_id'),
   );
   $schema['phenotype_comparison'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phenotype_comparison_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'genotype1_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'environment1_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'genotype2_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'environment2_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'phenotype1_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'phenotype2_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phenotype_comparison_id'),
     'unique keys' => array(
       'phenotype_comparison_c1' => array('genotype1_id', 'environment1_id', 'genotype2_id', 'environment2_id', 'phenotype1_id', 'pub_id')
     ),
     'indexes' => array(
       'phenotype_comparison_idx1' => array('genotype1_id'),
       'phenotype_comparison_idx2' => array('genotype2_id'),
       'phenotype_comparison_idx4' => array('pub_id'),
     ),
   );
   $schema['phenotype_comparison_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phenotype_comparison_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phenotype_comparison_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('phenotype_comparison_cvterm_id'),
     'unique keys' => array(
       'phenotype_comparison_cvterm_c1' => array('phenotype_comparison_id', 'cvterm_id')
     ),
     'indexes' => array(
       'phenotype_comparison_cvterm_idx1' => array('phenotype_comparison_id'),
       'phenotype_comparison_cvterm_idx2' => array('cvterm_id'),
     ),
   );
   $schema['phylonode_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylonode_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'phylotree_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phylonode_relationship_id'),
     'unique keys' => array(
       'subject_id' => array('subject_id', 'object_id', 'type_id')
     ),
     'indexes' => array(
       'phylonode_relationship_idx1' => array('subject_id'),
       'phylonode_relationship_idx2' => array('object_id'),
       'phylonode_relationship_idx3' => array('type_id'),
     ),
   );
   $schema['phylonodeprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylonodeprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phylonode_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
         'default' => '',
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('phylonodeprop_id'),
     'unique keys' => array(
       'phylonode_id' => array('phylonode_id', 'type_id', 'value', 'rank')
     ),
     'indexes' => array(
       'phylonodeprop_idx1' => array('phylonode_id'),
       'phylonodeprop_idx2' => array('type_id'),
     ),
   );
   $schema['phylotree'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylotree_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'analysis_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'comment' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('phylotree_id'),
     'indexes' => array(
       'phylotree_idx1' => array('phylotree_id'),
     ),
   );
   $schema['phylotree_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylotree_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phylotree_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phylotree_pub_id'),
     'unique keys' => array(
       'phylotree_id' => array('phylotree_id', 'pub_id')
     ),
     'indexes' => array(
       'phylotree_pub_idx1' => array('phylotree_id'),
       'phylotree_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['phylonode_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylonode_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phylonode_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phylonode_pub_id'),
     'unique keys' => array(
       'phylonode_id' => array('phylonode_id', 'pub_id')
     ),
     'indexes' => array(
       'phylonode_pub_idx1' => array('phylonode_id'),
       'phylonode_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['phylonode_organism'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylonode_organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phylonode_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phylonode_organism_id'),
     'unique keys' => array(
       'phylonode_id' => array('phylonode_id')
     ),
     'indexes' => array(
       'phylonode_organism_idx1' => array('phylonode_id'),
       'phylonode_organism_idx2' => array('organism_id'),
     ),
   );
   $schema['phenotype_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phenotype_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phenotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('phenotype_cvterm_id'),
     'unique keys' => array(
       'phenotype_cvterm_c1' => array('phenotype_id', 'cvterm_id', 'rank')
     ),
     'indexes' => array(
       'phenotype_cvterm_idx1' => array('phenotype_id'),
       'phenotype_cvterm_idx2' => array('cvterm_id'),
     ),
   );
   $schema['phenstatement'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phenstatement_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'genotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'environment_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'phenotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phenstatement_id'),
     'unique keys' => array(
       'phenstatement_c1' => array('genotype_id', 'phenotype_id', 'environment_id', 'type_id', 'pub_id')
     ),
     'indexes' => array(
       'phenstatement_idx1' => array('genotype_id'),
       'phenstatement_idx2' => array('phenotype_id'),
     ),
   );
   $schema['phylonode'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylonode_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phylotree_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'parent_phylonode_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'left_idx' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'right_idx' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'label' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'distance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('phylonode_id'),
     'unique keys' => array(
       'phylotree_id' => array('phylotree_id', 'left_idx'),
       'phylonode_phylotree_id_key1' => array('phylotree_id', 'right_idx')
     ),
   );
   $schema['phylonode_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'phylonode_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'phylonode_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('phylonode_dbxref_id'),
     'unique keys' => array(
       'phylonode_id' => array('phylonode_id', 'dbxref_id')
     ),
     'indexes' => array(
       'phylonode_dbxref_idx1' => array('phylonode_id'),
       'phylonode_dbxref_idx2' => array('dbxref_id'),
     ),
   );
   $schema['library_synonym'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'library_synonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'synonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'ru',
       ),
       'is_internal' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
     ),
     'primary key' => array('library_synonym_id'),
     'unique keys' => array(
       'library_synonym_c1' => array('synonym_id', 'library_id', 'pub_id')
     ),
     'indexes' => array(
       'library_synonym_idx1' => array('synonym_id'),
       'library_synonym_idx2' => array('library_id'),
       'library_synonym_idx3' => array('pub_id'),
     ),
   );
   $schema['library_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'library_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('library_pub_id'),
     'unique keys' => array(
       'library_pub_c1' => array('library_id', 'pub_id')
     ),
     'indexes' => array(
       'library_pub_idx1' => array('library_id'),
       'library_pub_idx2' => array('pub_id'),
     ),
   );

   $schema['featureprop_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featureprop_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'featureprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('featureprop_pub_id'),
     'unique keys' => array(
       'featureprop_pub_c1' => array('featureprop_id', 'pub_id')
     ),
     'indexes' => array(
       'featureprop_pub_idx1' => array('featureprop_id'),
       'featureprop_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['featureprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featureprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('featureprop_id'),
     'unique keys' => array(
       'featureprop_c1' => array('feature_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'featureprop_idx1' => array('feature_id'),
       'featureprop_idx2' => array('type_id'),
     ),
   );
   $schema['featurerange'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featurerange_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'featuremap_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'leftstartf_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'leftendf_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'rightstartf_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'rightendf_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rangestr' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('featurerange_id'),
     'indexes' => array(
       'featurerange_idx1' => array('featuremap_id'),
       'featurerange_idx2' => array('feature_id'),
       'featurerange_idx3' => array('leftstartf_id'),
       'featurerange_idx4' => array('leftendf_id'),
       'featurerange_idx5' => array('rightstartf_id'),
       'featurerange_idx6' => array('rightendf_id'),
     ),
   );
   $schema['featureset_meets'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['fnr_type'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'residues' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'seqlen' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'md5checksum' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'char',
         'length' => '32',
         'not null' => FALSE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'timeaccessioned' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
       ),
       'timelastmodified' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['featurepos'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featurepos_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'featuremap_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'map_feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'mappos' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('featurepos_id'),
     'indexes' => array(
       'featurepos_idx1' => array('featuremap_id'),
       'featurepos_idx2' => array('feature_id'),
       'featurepos_idx3' => array('map_feature_id'),
     ),
   );
   $schema['featuremap_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featuremap_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'featuremap_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('featuremap_pub_id'),
     'indexes' => array(
       'featuremap_pub_idx1' => array('featuremap_id'),
       'featuremap_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['feature_union'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'subject_strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'object_strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'fmin' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmax' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['featureloc'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featureloc_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmin' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'is_fmin_partial' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
       'fmax' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'is_fmax_partial' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
       'strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'phase' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'residue_info' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'locgroup' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('featureloc_id'),
     'unique keys' => array(
       'featureloc_c1' => array('feature_id', 'locgroup', 'rank')
     ),
     'indexes' => array(
       'binloc_boxrange' => array('fmin'),
       'binloc_boxrange_src' => array('srcfeature_id', 'fmin'),
       'featureloc_idx1' => array('feature_id'),
       'featureloc_idx2' => array('srcfeature_id'),
       'featureloc_idx3' => array('srcfeature_id', 'fmin', 'fmax'),
     ),
   );
   $schema['featureloc_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featureloc_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'featureloc_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('featureloc_pub_id'),
     'unique keys' => array(
       'featureloc_pub_c1' => array('featureloc_id', 'pub_id')
     ),
     'indexes' => array(
       'featureloc_pub_idx1' => array('featureloc_id'),
       'featureloc_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['featuremap'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featuremap_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'unittype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('featuremap_id'),
     'unique keys' => array(
       'featuremap_c1' => array('name')
     ),
   );
   $schema['fp_key'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'pkey' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['genotype'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'genotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('genotype_id'),
     'unique keys' => array(
       'genotype_c1' => array('uniquename')
     ),
     'indexes' => array(
       'genotype_idx1' => array('uniquename'),
       'genotype_idx2' => array('name'),
     ),
   );
   $schema['library_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'library_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('library_cvterm_id'),
     'unique keys' => array(
       'library_cvterm_c1' => array('library_id', 'cvterm_id', 'pub_id')
     ),
     'indexes' => array(
       'library_cvterm_idx1' => array('library_id'),
       'library_cvterm_idx2' => array('cvterm_id'),
       'library_cvterm_idx3' => array('pub_id'),
     ),
   );
   $schema['library_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'library_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'ru',
       ),
     ),
     'primary key' => array('library_dbxref_id'),
     'unique keys' => array(
       'library_dbxref_c1' => array('library_id', 'dbxref_id')
     ),
     'indexes' => array(
       'library_dbxref_idx1' => array('library_id'),
       'library_dbxref_idx2' => array('dbxref_id'),
     ),
   );
   $schema['library_feature'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'library_feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('library_feature_id'),
     'unique keys' => array(
       'library_feature_c1' => array('library_id', 'feature_id')
     ),
     'indexes' => array(
       'library_feature_idx1' => array('library_id'),
       'library_feature_idx2' => array('feature_id'),
     ),
   );
   $schema['library_feature_count'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'num_features' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'feature_type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
     ),
     'unique keys' => array(
       'library_feature_count_index' => array('library_id')
     ),
   );

   $schema['library'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_obsolete' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
       'timeaccessioned' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
       'timelastmodified' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
     ),
     'primary key' => array('library_id'),
     'unique keys' => array(
       'library_c1' => array('organism_id', 'uniquename', 'type_id')
     ),
     'indexes' => array(
       'library_idx1' => array('organism_id'),
       'library_idx2' => array('type_id'),
       'library_idx3' => array('uniquename'),
       'library_name_ind1' => array('name'),
     ),
   );
   $schema['intronloc_view'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'exon1_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'exon2_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmin' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmax' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['gff3atts'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'attribute' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['gff3view'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'ref' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'source' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'fstart' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fend' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'score' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => FALSE,
       ),
       'strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'phase' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'seqlen' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['gffatts'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'attribute' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['intron_combined_view'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'exon1_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'exon2_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmin' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmax' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'intron_rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'transcript_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['project'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'project_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('project_id'),
     'unique keys' => array(
       'project_c1' => array('name')
     ),
   );
   $schema['protein_coding_gene'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'residues' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'seqlen' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'md5checksum' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'char',
         'length' => '32',
         'not null' => FALSE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'is_analysis' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => FALSE,
       ),
       'is_obsolete' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => FALSE,
       ),
       'timeaccessioned' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
       ),
       'timelastmodified' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['studyfactorvalue'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'studyfactorvalue_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'studyfactor_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'factorvalue' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('studyfactorvalue_id'),
     'indexes' => array(
       'studyfactorvalue_idx1' => array('studyfactor_id'),
       'studyfactorvalue_idx2' => array('assay_id'),
     ),
   );
   $schema['studyfactor'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'studyfactor_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'studydesign_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('studyfactor_id'),
     'indexes' => array(
       'studyfactor_idx1' => array('studydesign_id'),
       'studyfactor_idx2' => array('type_id'),
     ),
   );
   $schema['studyprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'studyprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'study_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('studyprop_id'),
     'unique keys' => array(
       'study_id' => array('study_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'studyprop_idx1' => array('study_id'),
       'studyprop_idx2' => array('type_id'),
     ),
   );
   $schema['studyprop_feature'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'studyprop_feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'studyprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('studyprop_feature_id'),
     'unique keys' => array(
       'studyprop_id' => array('studyprop_id', 'feature_id')
     ),
     'indexes' => array(
       'studyprop_feature_idx1' => array('studyprop_id'),
       'studyprop_feature_idx2' => array('feature_id'),
     ),
   );
   $schema['synonym'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'synonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'synonym_sgml' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('synonym_id'),
     'unique keys' => array(
       'synonym_c1' => array('name', 'type_id')
     ),
     'indexes' => array(
       'synonym_idx1' => array('type_id'),
     ),
   );
   $schema['studydesignprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'studydesignprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'studydesign_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('studydesignprop_id'),
     'unique keys' => array(
       'studydesignprop_c1' => array('studydesign_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'studydesignprop_idx1' => array('studydesign_id'),
       'studydesignprop_idx2' => array('type_id'),
     ),
   );
   $schema['studydesign'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'studydesign_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'study_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('studydesign_id'),
     'indexes' => array(
       'studydesign_idx1' => array('study_id'),
     ),
   );
   $schema['stockprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stockprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('stockprop_id'),
     'unique keys' => array(
       'stockprop_c1' => array('stock_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'stockprop_idx1' => array('stock_id'),
       'stockprop_idx2' => array('type_id'),
     ),
     'foreign keys' => array(
       'stock' => array(
         'table' => 'stock',
         'columns' => array('stock_id' => 'stock_id'),
       ),
       'cvterm' => array(
         'table' => 'cvterm',
         'columns' => array('type_id' => 'cvterm_id'),
       ),
     ),
   );
   $schema['stockprop_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stockprop_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stockprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('stockprop_pub_id'),
     'unique keys' => array(
       'stockprop_pub_c1' => array('stockprop_id', 'pub_id')
     ),
     'indexes' => array(
       'stockprop_pub_idx1' => array('stockprop_id'),
       'stockprop_pub_idx2' => array('pub_id'),
     ),
   );

   $schema['study'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'study_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'contact_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('study_id'),
     'unique keys' => array(
       'study_c1' => array('name')
     ),
     'indexes' => array(
       'study_idx1' => array('contact_id'),
       'study_idx2' => array('pub_id'),
       'study_idx3' => array('dbxref_id'),
     ),
   );
   $schema['study_assay'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'study_assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'study_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('study_assay_id'),
     'unique keys' => array(
       'study_assay_c1' => array('study_id', 'assay_id')
     ),
     'indexes' => array(
       'study_assay_idx1' => array('study_id'),
       'study_assay_idx2' => array('assay_id'),
     ),
   );
   $schema['tableinfo'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'tableinfo_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '30',
         'not null' => TRUE,
       ),
       'primary_key_column' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '30',
         'not null' => FALSE,
       ),
       'is_view' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
       'view_on_table_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'superclass_table_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'is_updateable' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 1,
       ),
       'modification_date' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'date',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
     ),
     'primary key' => array('tableinfo_id'),
     'unique keys' => array(
       'tableinfo_c1' => array('name')
     ),
   );
   $schema['treatment'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'treatment_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
       'biomaterial_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'protocol_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('treatment_id'),
     'indexes' => array(
       'treatment_idx1' => array('biomaterial_id'),
       'treatment_idx2' => array('type_id'),
       'treatment_idx3' => array('protocol_id'),
     ),
   );
   $schema['tripal_obo'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cv_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'file' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'url' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('cv_id'),
     'indexes' => array(
       'cv_id' => array('cv_id'),
     ),
   );
   $schema['tripal_organism_views_common_name'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'nid' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'common_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
         'default' => 'NA',
       ),
     ),
   );
   $schema['type_feature_count'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'num_features' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['stockcollectionprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stockcollectionprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stockcollection_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('stockcollectionprop_id'),
     'unique keys' => array(
       'stockcollectionprop_c1' => array('stockcollection_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'stockcollectionprop_idx1' => array('stockcollection_id'),
       'stockcollectionprop_idx2' => array('type_id'),
     ),
   );
   $schema['stockcollection_stock'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stockcollection_stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stockcollection_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('stockcollection_stock_id'),
     'unique keys' => array(
       'stockcollection_stock_c1' => array('stockcollection_id', 'stock_id')
     ),
     'indexes' => array(
       'stockcollection_stock_idx1' => array('stockcollection_id'),
       'stockcollection_stock_idx2' => array('stock_id'),
     ),
   );
   $schema['public_feature'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'vid' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'nid' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
       'sync_date' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('nid'),
     'unique keys' => array(
       'nid_vid' => array('nid', 'vid'),
       'vid' => array('vid')
     ),
     'indexes' => array(
       'feature_id' => array('feature_id'),
     ),
   );
   $schema['public_library'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'vid' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'nid' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('nid'),
     'unique keys' => array(
       'nid_vid' => array('nid', 'vid'),
       'vid' => array('vid')
     ),
     'indexes' => array(
       'library_id' => array('library_id'),
     ),
   );
   $schema['public_organism'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'vid' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'nid' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('nid'),
     'unique keys' => array(
       'nid_vid' => array('nid', 'vid'),
       'vid' => array('vid')
     ),
     'indexes' => array(
       'organism_id' => array('organism_id'),
     ),
   );
   $schema['pubprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'pubprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('pubprop_id'),
     'unique keys' => array(
       'pubprop_c1' => array('pub_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'pubprop_idx1' => array('pub_id'),
       'pubprop_idx2' => array('type_id'),
     ),
   );
   $schema['pubauthor'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'pubauthor_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'editor' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => FALSE,
         'default' => 'als',
       ),
       'surname' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '100',
         'not null' => TRUE,
       ),
       'givennames' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '100',
         'not null' => FALSE,
       ),
       'suffix' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '100',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('pubauthor_id'),
     'unique keys' => array(
       'pubauthor_c1' => array('pub_id', 'rank')
     ),
     'indexes' => array(
       'pubauthor_idx2' => array('pub_id'),
     ),
   );
   $schema['pub_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'pub_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('pub_relationship_id'),
     'unique keys' => array(
       'pub_relationship_c1' => array('subject_id', 'object_id', 'type_id')
     ),
     'indexes' => array(
       'pub_relationship_idx1' => array('subject_id'),
       'pub_relationship_idx2' => array('object_id'),
       'pub_relationship_idx3' => array('type_id'),
     ),
   );
   $schema['protocol'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'protocol_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'uri' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'protocoldescription' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'hardwaredescription' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'softwaredescription' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('protocol_id'),
     'unique keys' => array(
       'protocol_c1' => array('name')
     ),
     'indexes' => array(
       'protocol_idx1' => array('type_id'),
       'protocol_idx2' => array('pub_id'),
       'protocol_idx3' => array('dbxref_id'),
     ),
   );

   $schema['protocolparam'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'protocolparam_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'protocol_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'datatype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'unittype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('protocolparam_id'),
     'indexes' => array(
       'protocolparam_idx1' => array('protocol_id'),
       'protocolparam_idx2' => array('datatype_id'),
       'protocolparam_idx3' => array('unittype_id'),
     ),
   );
   $schema['pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'title' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'volumetitle' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'volume' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'series_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'issue' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'pyear' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'pages' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'miniref' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_obsolete' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => FALSE,
         'default' => 'als',
       ),
       'publisher' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'pubplace' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('pub_id'),
     'unique keys' => array(
       'pub_c1' => array('uniquename')
     ),
     'indexes' => array(
       'pub_idx1' => array('type_id'),
     ),
   );
   $schema['pub_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'pub_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'ru',
       ),
     ),
     'primary key' => array('pub_dbxref_id'),
     'unique keys' => array(
       'pub_dbxref_c1' => array('pub_id', 'dbxref_id')
     ),
     'indexes' => array(
       'pub_dbxref_idx1' => array('pub_id'),
       'pub_dbxref_idx2' => array('dbxref_id'),
     ),
   );
   $schema['quantification'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'quantification_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'acquisition_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'operator_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'protocol_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'analysis_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'quantificationdate' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
         'default' => 'ow(',
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'uri' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('quantification_id'),
     'unique keys' => array(
       'quantification_c1' => array('name', 'analysis_id')
     ),
     'indexes' => array(
       'quantification_idx1' => array('acquisition_id'),
       'quantification_idx2' => array('operator_id'),
       'quantification_idx3' => array('protocol_id'),
       'quantification_idx4' => array('analysis_id'),
     ),
   );
   $schema['quantification_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'quantification_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('quantification_relationship_id'),
     'unique keys' => array(
       'quantification_relationship_c1' => array('subject_id', 'object_id', 'type_id')
     ),
     'indexes' => array(
       'quantification_relationship_idx1' => array('subject_id'),
       'quantification_relationship_idx2' => array('type_id'),
       'quantification_relationship_idx3' => array('object_id'),
     ),
   );
   $schema['stock_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stock_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('stock_pub_id'),
     'unique keys' => array(
       'stock_pub_c1' => array('stock_id', 'pub_id')
     ),
     'indexes' => array(
       'stock_pub_idx1' => array('stock_id'),
       'stock_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['stock_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stock_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('stock_relationship_id'),
     'unique keys' => array(
       'stock_relationship_c1' => array('subject_id', 'object_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'stock_relationship_idx1' => array('subject_id'),
       'stock_relationship_idx2' => array('object_id'),
       'stock_relationship_idx3' => array('type_id'),
     ),
     'foreign keys' => array(
       'stock' => array(
         'table' => 'stock',
         'columns' => array('subject_id' => 'stock_id'),
       ),
       'cvterm' => array(
         'table' => 'cvterm',
         'columns' => array('type_id' => 'cvterm_id'),
       ),
     ),
   );
   $schema['stock_relationship_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stock_relationship_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stock_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('stock_relationship_pub_id'),
     'unique keys' => array(
       'stock_relationship_pub_c1' => array('stock_relationship_id', 'pub_id')
     ),
     'indexes' => array(
       'stock_relationship_pub_idx1' => array('stock_relationship_id'),
       'stock_relationship_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['stockcollection'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stockcollection_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'contact_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('stockcollection_id'),
     'unique keys' => array(
       'stockcollection_c1' => array('uniquename', 'type_id')
     ),
     'indexes' => array(
       'stockcollection_idx1' => array('contact_id'),
       'stockcollection_idx2' => array('type_id'),
       'stockcollection_idx3' => array('uniquename'),
       'stockcollection_name_ind1' => array('name'),
     ),
   );
   $schema['stock_genotype'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stock_genotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'genotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('stock_genotype_id'),
     'unique keys' => array(
       'stock_genotype_c1' => array('stock_id', 'genotype_id')
     ),
     'indexes' => array(
       'stock_genotype_idx1' => array('stock_id'),
       'stock_genotype_idx2' => array('genotype_id'),
     ),
   );
   $schema['stock_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stock_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'ru',
       ),
     ),
     'primary key' => array('stock_dbxref_id'),
     'unique keys' => array(
       'stock_dbxref_c1' => array('stock_id', 'dbxref_id')
     ),
     'indexes' => array(
       'stock_dbxref_idx1' => array('stock_id'),
       'stock_dbxref_idx2' => array('dbxref_id'),
     ),
     'foreign keys' => array(
       'dbxref' => array(
         'table' => 'dbxref',
         'columns' => array('dbxref_id' => 'dbxref_id'),
       ),
       'stock' => array(
         'table' => 'stock',
         'columns' => array('stock_id' => 'stock_id'),
       ),
     ),
   );
   $schema['quantificationprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'quantificationprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'quantification_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('quantificationprop_id'),
     'unique keys' => array(
       'quantificationprop_c1' => array('quantification_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'quantificationprop_idx1' => array('quantification_id'),
       'quantificationprop_idx2' => array('type_id'),
     ),
   );
   $schema['stats_paths_to_root'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'total_paths' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'big',
         'not null' => FALSE,
       ),
       'avg_distance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'numeric',
         'not null' => FALSE,
         'precision' => 0,
         'scale' => 0,
       ),
       'min_distance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'max_distance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['stock'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_obsolete' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
     ),
     'primary key' => array('stock_id'),
     'unique keys' => array(
       'stock_c1' => array('organism_id', 'uniquename', 'type_id')
     ),
     'indexes' => array(
       'stock_idx1' => array('dbxref_id'),
       'stock_idx2' => array('organism_id'),
       'stock_idx3' => array('type_id'),
       'stock_idx4' => array('uniquename'),
       'stock_name_ind1' => array('name'),
     ),
    'foreign keys' => array(
       'organism' => array(
         'table' => 'organism',
         'columns' => array('organism_id' => 'organism_id'),
       ),
       'dbxref' => array(
         'table' => 'dbxref',
         'columns' => array('dbxref_id' => 'dbxref_id'),
       ),
       'cvterm' => array(
         'table' => 'cvterm',
         'columns' => array('type_id' => 'cvterm_id'),
       ),
     ),
   );
   $schema['stock_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'stock_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'stock_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('stock_cvterm_id'),
     'unique keys' => array(
       'stock_cvterm_c1' => array('stock_id', 'cvterm_id', 'pub_id')
     ),
     'indexes' => array(
       'stock_cvterm_idx1' => array('stock_id'),
       'stock_cvterm_idx2' => array('cvterm_id'),
       'stock_cvterm_idx3' => array('pub_id'),
     ),
   );
   $schema['feature_synonym'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_synonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'synonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
       'is_internal' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
     ),
     'primary key' => array('feature_synonym_id'),
     'unique keys' => array(
       'feature_synonym_c1' => array('synonym_id', 'feature_id', 'pub_id')
     ),
     'indexes' => array(
       'feature_synonym_idx1' => array('synonym_id'),
       'feature_synonym_idx2' => array('feature_id'),
       'feature_synonym_idx3' => array('pub_id'),
     ),
   );
   $schema['feature_relationshipprop_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_relationshipprop_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_relationshipprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_relationshipprop_pub_id'),
     'unique keys' => array(
       'feature_relationshipprop_pub_c1' => array('feature_relationshipprop_id', 'pub_id')
     ),
     'indexes' => array(
       'feature_relationshipprop_pub_idx1' => array('feature_relationshipprop_id'),
       'feature_relationshipprop_pub_idx2' => array('pub_id'),
     ),
   );

   $schema['cell_line_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('cell_line_relationship_id'),
     'unique keys' => array(
       'cell_line_relationship_c1' => array('subject_id', 'object_id', 'type_id')
     ),
   );
   $schema['cell_line_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('cell_line_pub_id'),
     'unique keys' => array(
       'cell_line_pub_c1' => array('cell_line_id', 'pub_id')
     ),
   );
   $schema['cell_line_synonym'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_synonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'synonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
       'is_internal' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
     ),
     'primary key' => array('cell_line_synonym_id'),
     'unique keys' => array(
       'cell_line_synonym_c1' => array('synonym_id', 'cell_line_id', 'pub_id')
     ),
   );
   $schema['cell_lineprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_lineprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('cell_lineprop_id'),
     'unique keys' => array(
       'cell_lineprop_c1' => array('cell_line_id', 'type_id', 'rank')
     ),
   );
   $schema['cell_lineprop_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_lineprop_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_lineprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('cell_lineprop_pub_id'),
     'unique keys' => array(
       'cell_lineprop_pub_c1' => array('cell_lineprop_id', 'pub_id')
     ),
   );
   $schema['cell_line_library'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'library_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('cell_line_library_id'),
     'unique keys' => array(
       'cell_line_library_c1' => array('cell_line_id', 'library_id', 'pub_id')
     ),
   );
   $schema['cell_line_feature'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('cell_line_feature_id'),
     'unique keys' => array(
       'cell_line_feature_c1' => array('cell_line_id', 'feature_id', 'pub_id')
     ),
   );
   $schema['cell_line'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'timeaccessioned' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
       'timelastmodified' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
     ),
     'primary key' => array('cell_line_id'),
     'unique keys' => array(
       'cell_line_c1' => array('uniquename', 'organism_id')
     ),
   );
   $schema['cell_line_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('cell_line_cvterm_id'),
     'unique keys' => array(
       'cell_line_cvterm_c1' => array('cell_line_id', 'cvterm_id', 'pub_id', 'rank')
     ),
   );
   $schema['cell_line_cvtermprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_cvtermprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('cell_line_cvtermprop_id'),
     'unique keys' => array(
       'cell_line_cvtermprop_c1' => array('cell_line_cvterm_id', 'type_id', 'rank')
     ),
   );
   $schema['cell_line_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cell_line_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cell_line_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'ru',
       ),
     ),
     'primary key' => array('cell_line_dbxref_id'),
     'unique keys' => array(
       'cell_line_dbxref_c1' => array('cell_line_id', 'dbxref_id')
     ),
   );
   $schema['channel'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'channel_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'definition' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('channel_id'),
     'unique keys' => array(
       'channel_c1' => array('name')
     ),
   );
   $schema['common_ancestor_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvterm1_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'cvterm2_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'ancestor_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'pathdistance1' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'pathdistance2' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'total_pathdistance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['cv_cvterm_count_with_obs'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'num_terms_incl_obs' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['cv_leaf'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cv_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['cv_link_count'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cv_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'relation_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'relation_cv_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'num_links' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['cv_path_count'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cv_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'relation_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'relation_cv_name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'num_paths' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
   );

   $schema['cv_cvterm_count'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'num_terms_excl_obs' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['cv'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cv_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'definition' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('cv_id'),
     'unique keys' => array(
       'cv_c1' => array('name')
     ),
   );
   $schema['common_descendant_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvterm1_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'cvterm2_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'ancestor_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'pathdistance1' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'pathdistance2' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'total_pathdistance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['contact'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'contact_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('contact_id'),
     'unique keys' => array(
       'contact_c1' => array('name')
     ),
   );
   $schema['contact_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'contact_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('contact_relationship_id'),
     'unique keys' => array(
       'contact_relationship_c1' => array('subject_id', 'object_id', 'type_id')
     ),
     'indexes' => array(
       'contact_relationship_idx1' => array('type_id'),
       'contact_relationship_idx2' => array('subject_id'),
       'contact_relationship_idx3' => array('object_id'),
     ),
   );
   $schema['control'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'control_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'tableinfo_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'row_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('control_id'),
     'indexes' => array(
       'control_idx1' => array('type_id'),
       'control_idx2' => array('assay_id'),
       'control_idx3' => array('tableinfo_id'),
       'control_idx4' => array('row_id'),
     ),
   );
   $schema['biomaterialprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'biomaterialprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'biomaterial_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('biomaterialprop_id'),
     'unique keys' => array(
       'biomaterialprop_c1' => array('biomaterial_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'biomaterialprop_idx1' => array('biomaterial_id'),
       'biomaterialprop_idx2' => array('type_id'),
     ),
   );
   $schema['biomaterial_treatment'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'biomaterial_treatment_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'biomaterial_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'treatment_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'unittype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('biomaterial_treatment_id'),
     'unique keys' => array(
       'biomaterial_treatment_c1' => array('biomaterial_id', 'treatment_id')
     ),
     'indexes' => array(
       'biomaterial_treatment_idx1' => array('biomaterial_id'),
       'biomaterial_treatment_idx2' => array('treatment_id'),
       'biomaterial_treatment_idx3' => array('unittype_id'),
     ),
   );
   $schema['acquisition_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'acquisition_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('acquisition_relationship_id'),
     'unique keys' => array(
       'acquisition_relationship_c1' => array('subject_id', 'object_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'acquisition_relationship_idx1' => array('subject_id'),
       'acquisition_relationship_idx2' => array('type_id'),
       'acquisition_relationship_idx3' => array('object_id'),
     ),
   );
   $schema['acquisition'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'acquisition_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'protocol_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'channel_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'acquisitiondate' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
         'default' => 'ow(',
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'uri' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('acquisition_id'),
     'unique keys' => array(
       'acquisition_c1' => array('name')
     ),
     'indexes' => array(
       'acquisition_idx1' => array('assay_id'),
       'acquisition_idx2' => array('protocol_id'),
       'acquisition_idx3' => array('channel_id'),
     ),
   );
   $schema['acquisitionprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'acquisitionprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'acquisition_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('acquisitionprop_id'),
     'unique keys' => array(
       'acquisitionprop_c1' => array('acquisition_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'acquisitionprop_idx1' => array('acquisition_id'),
       'acquisitionprop_idx2' => array('type_id'),
     ),
   );
   $schema['all_feature_names'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['analysis'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'analysis_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'program' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'programversion' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'algorithm' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'sourcename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'sourceversion' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'sourceuri' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'timeexecuted' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
     ),
     'primary key' => array('analysis_id'),
     'unique keys' => array(
       'analysis_c1' => array('program', 'programversion', 'sourcename')
     ),
   );
   $schema['analysisfeature'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'analysisfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'analysis_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rawscore' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => FALSE,
       ),
       'normscore' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => FALSE,
       ),
       'significance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => FALSE,
       ),
       'identity' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('analysisfeature_id'),
     'unique keys' => array(
       'analysisfeature_c1' => array('feature_id', 'analysis_id')
     ),
     'indexes' => array(
       'analysisfeature_idx1' => array('feature_id'),
       'analysisfeature_idx2' => array('analysis_id'),
     ),
   );
   $schema['analysisfeatureprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'analysisfeatureprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'analysisfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('analysisfeatureprop_id'),
     'unique keys' => array(
       'analysisfeature_id_type_id_rank' => array('analysisfeature_id', 'type_id', 'rank')
     ),
   );
   $schema['assayprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'assayprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('assayprop_id'),
     'unique keys' => array(
       'assayprop_c1' => array('assay_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'assayprop_idx1' => array('assay_id'),
       'assayprop_idx2' => array('type_id'),
     ),
   );
   $schema['biomaterial'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'biomaterial_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'taxon_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'biosourceprovider_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('biomaterial_id'),
     'unique keys' => array(
       'biomaterial_c1' => array('name')
     ),
     'indexes' => array(
       'biomaterial_idx1' => array('taxon_id'),
       'biomaterial_idx2' => array('biosourceprovider_id'),
       'biomaterial_idx3' => array('dbxref_id'),
     ),
   );

   $schema['biomaterial_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'biomaterial_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'biomaterial_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('biomaterial_dbxref_id'),
     'unique keys' => array(
       'biomaterial_dbxref_c1' => array('biomaterial_id', 'dbxref_id')
     ),
     'indexes' => array(
       'biomaterial_dbxref_idx1' => array('biomaterial_id'),
       'biomaterial_dbxref_idx2' => array('dbxref_id'),
     ),
   );
   $schema['biomaterial_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'biomaterial_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('biomaterial_relationship_id'),
     'unique keys' => array(
       'biomaterial_relationship_c1' => array('subject_id', 'object_id', 'type_id')
     ),
     'indexes' => array(
       'biomaterial_relationship_idx1' => array('subject_id'),
       'biomaterial_relationship_idx2' => array('object_id'),
       'biomaterial_relationship_idx3' => array('type_id'),
     ),
   );
   $schema['assay_project'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'assay_project_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'project_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('assay_project_id'),
     'unique keys' => array(
       'assay_project_c1' => array('assay_id', 'project_id')
     ),
     'indexes' => array(
       'assay_project_idx1' => array('assay_id'),
       'assay_project_idx2' => array('project_id'),
     ),
   );
   $schema['assay_biomaterial'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'assay_biomaterial_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'biomaterial_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'channel_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('assay_biomaterial_id'),
     'unique keys' => array(
       'assay_biomaterial_c1' => array('assay_id', 'biomaterial_id', 'channel_id', 'rank')
     ),
     'indexes' => array(
       'assay_biomaterial_idx1' => array('assay_id'),
       'assay_biomaterial_idx2' => array('biomaterial_id'),
       'assay_biomaterial_idx3' => array('channel_id'),
     ),
   );
   $schema['analysisprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'analysisprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'analysis_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('analysisprop_id'),
     'unique keys' => array(
       'analysisprop_c1' => array('analysis_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'analysisprop_idx1' => array('analysis_id'),
       'analysisprop_idx2' => array('type_id'),
     ),
   );
   $schema['arraydesign'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'arraydesign_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'manufacturer_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'platformtype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'substratetype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'protocol_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'version' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'array_dimensions' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'element_dimensions' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'num_of_elements' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'num_array_columns' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'num_array_rows' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'num_grid_columns' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'num_grid_rows' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'num_sub_columns' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'num_sub_rows' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('arraydesign_id'),
     'unique keys' => array(
       'arraydesign_c1' => array('name')
     ),
     'indexes' => array(
       'arraydesign_idx1' => array('manufacturer_id'),
       'arraydesign_idx2' => array('platformtype_id'),
       'arraydesign_idx3' => array('substratetype_id'),
       'arraydesign_idx4' => array('protocol_id'),
       'arraydesign_idx5' => array('dbxref_id'),
     ),
   );
   $schema['arraydesignprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'arraydesignprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'arraydesign_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('arraydesignprop_id'),
     'unique keys' => array(
       'arraydesignprop_c1' => array('arraydesign_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'arraydesignprop_idx1' => array('arraydesign_id'),
       'arraydesignprop_idx2' => array('type_id'),
     ),
   );
   $schema['assay'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'assay_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'arraydesign_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'protocol_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'assaydate' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
         'default' => 'ow(',
       ),
       'arrayidentifier' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'arraybatchidentifier' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'operator_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('assay_id'),
     'unique keys' => array(
       'assay_c1' => array('name')
     ),
     'indexes' => array(
       'assay_idx1' => array('arraydesign_id'),
       'assay_idx2' => array('protocol_id'),
       'assay_idx3' => array('operator_id'),
       'assay_idx4' => array('dbxref_id'),
     ),
   );
   $schema['cv_root'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cv_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'root_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cv_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => TRUE,
       ),
       'definition' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_obsolete' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
       'is_relationshiptype' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('cvterm_id'),
     'unique keys' => array(
       'cvterm_c1' => array('name', 'cv_id', 'is_obsolete'),
       'cvterm_c2' => array('dbxref_id')
     ),
     'indexes' => array(
       'cvterm_idx1' => array('cv_id'),
       'cvterm_idx2' => array('name'),
       'cvterm_idx3' => array('dbxref_id'),
     ),
     'foreign keys' => array(
       'cv' => array(
         'table' => 'cv',
         'columns' => array('cv_id' => 'cv_id'),
       ),
       'dbxref' => array(
         'table' => 'dbxref',
         'columns' => array('dbxref_id' => 'dbxref_id'),
       ),
     ),
   );
   $schema['feature_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_current' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'ru',
       ),
     ),
     'primary key' => array('feature_dbxref_id'),
     'unique keys' => array(
       'feature_dbxref_c1' => array('feature_id', 'dbxref_id')
     ),
     'indexes' => array(
       'feature_dbxref_idx1' => array('feature_id'),
       'feature_dbxref_idx2' => array('dbxref_id'),
     ),
   );
   $schema['feature_cvtermprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_cvtermprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('feature_cvtermprop_id'),
     'unique keys' => array(
       'feature_cvtermprop_c1' => array('feature_cvterm_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'feature_cvtermprop_idx1' => array('feature_cvterm_id'),
       'feature_cvtermprop_idx2' => array('type_id'),
     ),
   );
   $schema['feature_difference'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'fmin' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmax' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['feature_disjoint'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['feature_distance'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'subject_strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'object_strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'distance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['feature_cvterm_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_cvterm_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_cvterm_pub_id'),
     'unique keys' => array(
       'feature_cvterm_pub_c1' => array('feature_cvterm_id', 'pub_id')
     ),
     'indexes' => array(
       'feature_cvterm_pub_idx1' => array('feature_cvterm_id'),
       'feature_cvterm_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['feature_cvterm_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_cvterm_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_cvterm_dbxref_id'),
     'unique keys' => array(
       'feature_cvterm_dbxref_c1' => array('feature_cvterm_id', 'dbxref_id')
     ),
     'indexes' => array(
       'feature_cvterm_dbxref_idx1' => array('feature_cvterm_id'),
       'feature_cvterm_dbxref_idx2' => array('dbxref_id'),
     ),
   );

   $schema['f_type'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => FALSE,
       ),
       'residues' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'seqlen' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'md5checksum' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'char',
         'length' => '32',
         'not null' => FALSE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'timeaccessioned' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
       ),
       'timelastmodified' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['feature'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'organism_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'residues' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'seqlen' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'md5checksum' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'char',
         'length' => '32',
         'not null' => FALSE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_analysis' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
       'is_obsolete' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
       'timeaccessioned' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
       'timelastmodified' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'datetime',
         'not null' => TRUE,
         'default' => 'ow(',
       ),
     ),
     'primary key' => array('feature_id'),
     'unique keys' => array(
       'feature_c1' => array('organism_id', 'uniquename', 'type_id')
     ),
     'indexes' => array(
       'feature_idx1' => array('dbxref_id'),
       'feature_idx2' => array('organism_id'),
       'feature_idx3' => array('type_id'),
       'feature_idx4' => array('uniquename'),
       'feature_name_ind1' => array('name'),
     ),
     'foreign keys' => array(
       'organism' => array(
         'table' => 'organism',
         'columns' => array('organism_id' => 'organism_id'),
       ),
       'dbxref' => array(
         'table' => 'dbxref',
         'columns' => array('dbxref_id' => 'dbxref_id'),
       ),
       'cvterm' => array(
         'table' => 'cvterm',
         'columns' => array('type_id' => 'cvterm_id'),
       ),
     ),
   );
   $schema['feature_contains'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['feature_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_not' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => TRUE,
         'default' => 'als',
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('feature_cvterm_id'),
     'unique keys' => array(
       'feature_cvterm_c1' => array('feature_id', 'cvterm_id', 'pub_id', 'rank')
     ),
     'indexes' => array(
       'feature_cvterm_idx1' => array('feature_id'),
       'feature_cvterm_idx2' => array('cvterm_id'),
       'feature_cvterm_idx3' => array('pub_id'),
     ),
   );
   $schema['feature_expression'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_expression_id'),
     'unique keys' => array(
       'feature_expression_c1' => array('expression_id', 'feature_id', 'pub_id')
     ),
     'indexes' => array(
       'feature_expression_idx1' => array('expression_id'),
       'feature_expression_idx2' => array('feature_id'),
       'feature_expression_idx3' => array('pub_id'),
     ),
   );
   $schema['feature_expressionprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_expressionprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('feature_expressionprop_id'),
     'unique keys' => array(
       'feature_expressionprop_c1' => array('feature_expression_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'feature_expressionprop_idx1' => array('feature_expression_id'),
       'feature_expressionprop_idx2' => array('type_id'),
     ),
   );
   $schema['feature_pubprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_pubprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('feature_pubprop_id'),
     'unique keys' => array(
       'feature_pubprop_c1' => array('feature_pub_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'feature_pubprop_idx1' => array('feature_pub_id'),
     ),
   );
   $schema['feature_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('feature_relationship_id'),
     'unique keys' => array(
       'feature_relationship_c1' => array('subject_id', 'object_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'feature_relationship_idx1' => array('subject_id'),
       'feature_relationship_idx2' => array('object_id'),
       'feature_relationship_idx3' => array('type_id'),
     ),
   );
   $schema['feature_relationship_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_relationship_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_relationship_pub_id'),
     'unique keys' => array(
       'feature_relationship_pub_c1' => array('feature_relationship_id', 'pub_id')
     ),
     'indexes' => array(
       'feature_relationship_pub_idx1' => array('feature_relationship_id'),
       'feature_relationship_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['feature_relationshipprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_relationshipprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('feature_relationshipprop_id'),
     'unique keys' => array(
       'feature_relationshipprop_c1' => array('feature_relationship_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'feature_relationshipprop_idx1' => array('feature_relationship_id'),
       'feature_relationshipprop_idx2' => array('type_id'),
     ),
   );
   $schema['feature_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_pub_id'),
     'unique keys' => array(
       'feature_pub_c1' => array('feature_id', 'pub_id')
     ),
     'indexes' => array(
       'feature_pub_idx1' => array('feature_id'),
       'feature_pub_idx2' => array('pub_id'),
     ),
   );
   $schema['feature_phenotype'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_phenotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'phenotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_phenotype_id'),
     'unique keys' => array(
       'feature_phenotype_c1' => array('feature_id', 'phenotype_id')
     ),
     'indexes' => array(
       'feature_phenotype_idx1' => array('feature_id'),
       'feature_phenotype_idx2' => array('phenotype_id'),
     ),
   );
   $schema['feature_genotype'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_genotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'genotype_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'chromosome_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cgroup' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('feature_genotype_id'),
     'unique keys' => array(
       'feature_genotype_c1' => array('feature_id', 'genotype_id', 'cvterm_id', 'chromosome_id', 'rank', 'cgroup')
     ),
     'indexes' => array(
       'feature_genotype_idx1' => array('feature_id'),
       'feature_genotype_idx2' => array('genotype_id'),
     ),
   );
   $schema['feature_intersection'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'subject_strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'object_strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'fmin' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'fmax' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['feature_meets'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['feature_meets_on_same_strand'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['f_loc'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'nbeg' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'nend' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
     ),
   );

   $schema['expressionprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'expressionprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('expressionprop_id'),
     'unique keys' => array(
       'expressionprop_c1' => array('expression_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'expressionprop_idx1' => array('expression_id'),
       'expressionprop_idx2' => array('type_id'),
     ),
   );
   $schema['db_dbxref_count'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'num_dbxrefs' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'big',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'db_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'accession' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'version' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
         'default' => '',
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('dbxref_id'),
     'unique keys' => array(
       'dbxref_c1' => array('db_id', 'accession', 'version')
     ),
     'indexes' => array(
       'dbxref_idx1' => array('db_id'),
       'dbxref_idx2' => array('accession'),
       'dbxref_idx3' => array('version'),
     ),
   );
   $schema['dbxrefprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'dbxrefprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
         'default' => '',
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('dbxrefprop_id'),
     'unique keys' => array(
       'dbxrefprop_c1' => array('dbxref_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'dbxrefprop_idx1' => array('dbxref_id'),
       'dbxrefprop_idx2' => array('type_id'),
     ),
   );
   $schema['dfeatureloc'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'featureloc_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'srcfeature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'nbeg' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'is_nbeg_partial' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => FALSE,
       ),
       'nend' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'is_nend_partial' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'boolean',
         'not null' => FALSE,
       ),
       'strand' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'size' => 'small',
         'not null' => FALSE,
       ),
       'phase' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'residue_info' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'locgroup' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
   );
   $schema['db'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'db_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'name' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'urlprefix' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
       'url' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('db_id'),
     'unique keys' => array(
       'db_c1' => array('name')
     ),
   );
   $schema['cvtermsynonym'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvtermsynonym_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'synonym' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '1024',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('cvtermsynonym_id'),
     'unique keys' => array(
       'cvtermsynonym_c1' => array('cvterm_id', 'synonym')
     ),
     'indexes' => array(
       'cvtermsynonym_idx1' => array('cvterm_id'),
     ),
   );
   $schema['cvterm_dbxref'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvterm_dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'is_for_definition' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('cvterm_dbxref_id'),
     'unique keys' => array(
       'cvterm_dbxref_c1' => array('cvterm_id', 'dbxref_id')
     ),
     'indexes' => array(
       'cvterm_dbxref_idx1' => array('cvterm_id'),
       'cvterm_dbxref_idx2' => array('dbxref_id'),
     ),
   );
   $schema['cvterm_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvterm_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('cvterm_relationship_id'),
     'unique keys' => array(
       'cvterm_relationship_c1' => array('subject_id', 'object_id', 'type_id')
     ),
     'indexes' => array(
       'cvterm_relationship_idx1' => array('type_id'),
       'cvterm_relationship_idx2' => array('subject_id'),
       'cvterm_relationship_idx3' => array('object_id'),
     ),
   );
   $schema['cvtermpath'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvtermpath_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cv_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pathdistance' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('cvtermpath_id'),
     'unique keys' => array(
       'cvtermpath_c1' => array('subject_id', 'object_id', 'type_id', 'pathdistance')
     ),
     'indexes' => array(
       'cvtermpath_idx1' => array('type_id'),
       'cvtermpath_idx2' => array('subject_id'),
       'cvtermpath_idx3' => array('object_id'),
       'cvtermpath_idx4' => array('cv_id'),
     ),
   );
   $schema['cvtermprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'cvtermprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
         'default' => '',
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('cvtermprop_id'),
     'unique keys' => array(
       'cvterm_id' => array('cvterm_id', 'type_id', 'value', 'rank')
     ),
     'indexes' => array(
       'cvtermprop_idx1' => array('cvterm_id'),
       'cvtermprop_idx2' => array('type_id'),
     ),
   );
   $schema['eimage'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'eimage_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'eimage_data' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'eimage_type' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => TRUE,
       ),
       'image_uri' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'varchar',
         'length' => '255',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('eimage_id'),
   );
   $schema['element'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'element_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'feature_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'arraydesign_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
       'dbxref_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('element_id'),
     'unique keys' => array(
       'element_c1' => array('feature_id', 'arraydesign_id')
     ),
     'indexes' => array(
       'element_idx1' => array('feature_id'),
       'element_idx2' => array('arraydesign_id'),
       'element_idx3' => array('type_id'),
       'element_idx4' => array('dbxref_id'),
     ),
   );
   $schema['expression_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'expression_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
       'cvterm_type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('expression_cvterm_id'),
     'unique keys' => array(
       'expression_cvterm_c1' => array('expression_id', 'cvterm_id', 'cvterm_type_id')
     ),
     'indexes' => array(
       'expression_cvterm_idx1' => array('expression_id'),
       'expression_cvterm_idx2' => array('cvterm_id'),
       'expression_cvterm_idx3' => array('cvterm_type_id'),
     ),
   );
   $schema['expression_cvtermprop'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'expression_cvtermprop_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'expression_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('expression_cvtermprop_id'),
     'unique keys' => array(
       'expression_cvtermprop_c1' => array('expression_cvterm_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'expression_cvtermprop_idx1' => array('expression_cvterm_id'),
       'expression_cvtermprop_idx2' => array('type_id'),
     ),
   );
   $schema['expression_image'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'expression_image_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'eimage_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('expression_image_id'),
     'unique keys' => array(
       'expression_image_c1' => array('expression_id', 'eimage_id')
     ),
     'indexes' => array(
       'expression_image_idx1' => array('expression_id'),
       'expression_image_idx2' => array('eimage_id'),
     ),
   );
   $schema['expression_pub'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'expression_pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'pub_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('expression_pub_id'),
     'unique keys' => array(
       'expression_pub_c1' => array('expression_id', 'pub_id')
     ),
     'indexes' => array(
       'expression_pub_idx1' => array('expression_id'),
       'expression_pub_idx2' => array('pub_id'),
     ),
   );

   $schema['expression'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'expression_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'md5checksum' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'char',
         'length' => '32',
         'not null' => FALSE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('expression_id'),
     'unique keys' => array(
       'expression_c1' => array('uniquename')
     ),
   );
   $schema['environment_cvterm'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'environment_cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'environment_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'cvterm_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('environment_cvterm_id'),
     'unique keys' => array(
       'environment_cvterm_c1' => array('environment_id', 'cvterm_id')
     ),
     'indexes' => array(
       'environment_cvterm_idx1' => array('environment_id'),
       'environment_cvterm_idx2' => array('cvterm_id'),
     ),
   );
   $schema['element_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'element_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('element_relationship_id'),
     'unique keys' => array(
       'element_relationship_c1' => array('subject_id', 'object_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'element_relationship_idx1' => array('subject_id'),
       'element_relationship_idx2' => array('type_id'),
       'element_relationship_idx3' => array('object_id'),
       'element_relationship_idx4' => array('value'),
     ),
   );
   $schema['elementresult'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'elementresult_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'element_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'quantification_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'signal' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'float',
         'size' => 'big',
         'not null' => TRUE,
       ),
     ),
     'primary key' => array('elementresult_id'),
     'unique keys' => array(
       'elementresult_c1' => array('element_id', 'quantification_id')
     ),
     'indexes' => array(
       'elementresult_idx1' => array('element_id'),
       'elementresult_idx2' => array('quantification_id'),
       'elementresult_idx3' => array('signal'),
     ),
   );
   $schema['elementresult_relationship'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'elementresult_relationship_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'subject_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'type_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'object_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
       ),
       'value' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
       'rank' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'int',
         'not null' => TRUE,
         'default' => 0,
       ),
     ),
     'primary key' => array('elementresult_relationship_id'),
     'unique keys' => array(
       'elementresult_relationship_c1' => array('subject_id', 'object_id', 'type_id', 'rank')
     ),
     'indexes' => array(
       'elementresult_relationship_idx1' => array('subject_id'),
       'elementresult_relationship_idx2' => array('type_id'),
       'elementresult_relationship_idx3' => array('object_id'),
       'elementresult_relationship_idx4' => array('value'),
     ),
   );
   $schema['environment'] = array(
     'description' => t('TODO: please describe this table!'),
     'fields' => array(
       'environment_id' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'serial',
         'not null' => TRUE,
       ),
       'uniquename' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => TRUE,
       ),
       'description' => array(
         'description' => t('TODO: please describe this field!'),
         'type' => 'text',
         'not null' => FALSE,
       ),
     ),
     'primary key' => array('environment_id'),
     'unique keys' => array(
       'environment_c1' => array('uniquename')
     ),
     'indexes' => array(
       'environment_idx1' => array('uniquename'),
     ),
   );
   return $schema;
}
