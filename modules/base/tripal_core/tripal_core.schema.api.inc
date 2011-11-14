<?php

/* @file: This file contains default schema definitions for all chado tables
 *        to be used with tripal_core chado insert/update/select functions.
 *        These schema definitions can be augmented by the various modules
 *        (specifically to add missing foreign key definitions) by implementing
 *        hook_chado_<table name>_schema().
 */

/** 
 * @defgroup tripal_schema_api Core Module Schema API
 * @{
 * Provides an application programming interface (API) for describing Chado tables.
 * This API consists of a set of functions, one for each table in Chado.  Each 
 * function simply returns a Drupal style array that defines the table. 
 * 
 * Because Drupal 6 does not handle foreign key relationships, a separate 
 * hook is used to define the foreign key relationships for each table.  As
 * of version 0.3b of Tripal, not all foreign key relationships have been 
 * defined.  Therefore, if you need a foreign key you must create the appropriate
 * hook.  Hooks are named hook_chado_table_schema() where 'hook' is replaced
 * with the module name, and 'table' is replaced by the Chado table name.
 *
 * The functions provided in this documentation should not be called as is, but if you need
 * the Drupal-style array definition for any table, use the following function
 * call:
 *
 *   $table_desc = module_invoke_all('chado_'.$table.'_schema');
 * 
 * where the variable $table contains the name of the table you want to 
 * retireve.  The function call above uses the Drupal hook infrastructure to
 * call the appropriate function in the Schema API as well as the appropriate
 * hook to add in any foreign key relationships. 
 * 
 * @}
 * @ingroup tripal_api
 */

/**
 *
 * @ingroup tripal_schema_api
 */
function tripal_core_get_chado_tables() {
  if(is_array($db_url) and array_key_exists('chado',$db_url)){
    $previous_db = tripal_db_set_active('chado');
    $sql = 'SELECT tablename FROM pg_tables';
    $resource = db_query($sql);
    tripal_db_set_active($previous_db);
  } else {
    $sql = "SELECT tablename FROM pg_tables WHERE schemaname='chado'";
    $resource = db_query($sql);
  }
  
  $tables = array();
  while ($r = db_fetch_object($resource)) {
    $tables[$r->tablename] = $r->tablename;
  }
  asort($tables);
  return $tables;
}
 
/**
 * Implements hook_organism_dbxref_schema()
 * Purpose: To describe the structure of organism_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
 */
function tripal_core_chado_organism_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'organism_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'organism_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'organism_dbxref_id',
    ),
    'unique keys' => array(
      'organism_dbxref_c1' => array(
        '0' => 'organism_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'organism_dbxref_idx1' => array(
        '0' => 'organism_id',
      ),
      'organism_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_organismprop_schema()
 * Purpose: To describe the structure of organismprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_organismprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'organismprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'organismprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'organismprop_id',
    ),
    'unique keys' => array(
      'organismprop_c1' => array(
        '0' => 'organism_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'organismprop_idx1' => array(
        '0' => 'organism_id',
      ),
      'organismprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phendesc_schema()
 * Purpose: To describe the structure of phendesc to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phendesc_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phendesc',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phendesc_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'genotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'environment_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phendesc_id',
    ),
    'unique keys' => array(
      'phendesc_c1' => array(
        '0' => 'genotype_id',
        '1' => 'environment_id',
        '2' => 'type_id',
        '3' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phendesc_idx1' => array(
        '0' => 'genotype_id',
      ),
      'phendesc_idx2' => array(
        '0' => 'environment_id',
      ),
      'phendesc_idx3' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phenotype_schema()
 * Purpose: To describe the structure of phenotype to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phenotype_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phenotype',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'observable_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'attr_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'cvalue_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'phenotype_id',
    ),
    'unique keys' => array(
      'phenotype_c1' => array(
        '0' => 'uniquename',
      ),
    ),
    'indexes' => array(
      'phenotype_idx1' => array(
        '0' => 'cvalue_id',
      ),
      'phenotype_idx2' => array(
        '0' => 'observable_id',
      ),
      'phenotype_idx3' => array(
        '0' => 'attr_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_organism_schema()
 * Purpose: To describe the structure of organism to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_organism_schema () {
  $description = array();
  
  $description = array(
    'table' => 'organism',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'abbreviation' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'genus' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'species' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'common_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'comment' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'organism_id',
    ),
    'unique keys' => array(
      'organism_c1' => array(
        '0' => 'genus',
        '1' => 'species',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_materialized_view_schema()
 * Purpose: To describe the structure of materialized_view to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_materialized_view_schema () {
  $description = array();
  
  $description = array(
    'table' => 'materialized_view',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'materialized_view_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'last_update' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
      ),
      'refresh_time' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '64',
        'not null' => '',
      ),
      'mv_schema' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '64',
        'not null' => '',
      ),
      'mv_table' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '128',
        'not null' => '',
      ),
      'mv_specs' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'indexed' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'query' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'special_index' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
      'name' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_libraryprop_schema()
 * Purpose: To describe the structure of libraryprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_libraryprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'libraryprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'libraryprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'libraryprop_id',
    ),
    'unique keys' => array(
      'libraryprop_c1' => array(
        '0' => 'library_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'libraryprop_idx1' => array(
        '0' => 'library_id',
      ),
      'libraryprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_libraryprop_pub_schema()
 * Purpose: To describe the structure of libraryprop_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_libraryprop_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'libraryprop_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'libraryprop_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'libraryprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'libraryprop_pub_id',
    ),
    'unique keys' => array(
      'libraryprop_pub_c1' => array(
        '0' => 'libraryprop_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'libraryprop_pub_idx1' => array(
        '0' => 'libraryprop_id',
      ),
      'libraryprop_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_magedocumentation_schema()
 * Purpose: To describe the structure of magedocumentation to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_magedocumentation_schema () {
  $description = array();
  
  $description = array(
    'table' => 'magedocumentation',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'magedocumentation_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'mageml_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'tableinfo_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'row_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'mageidentifier' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'magedocumentation_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'magedocumentation_idx1' => array(
        '0' => 'mageml_id',
      ),
      'magedocumentation_idx2' => array(
        '0' => 'tableinfo_id',
      ),
      'magedocumentation_idx3' => array(
        '0' => 'row_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_mageml_schema()
 * Purpose: To describe the structure of mageml to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_mageml_schema () {
  $description = array();
  
  $description = array(
    'table' => 'mageml',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'mageml_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'mage_package' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'mage_ml' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'mageml_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phenotype_comparison_schema()
 * Purpose: To describe the structure of phenotype_comparison to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phenotype_comparison_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phenotype_comparison',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_comparison_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'genotype1_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'environment1_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'genotype2_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'environment2_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'phenotype1_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'phenotype2_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phenotype_comparison_id',
    ),
    'unique keys' => array(
      'phenotype_comparison_c1' => array(
        '0' => 'genotype1_id',
        '1' => 'environment1_id',
        '2' => 'genotype2_id',
        '3' => 'environment2_id',
        '4' => 'phenotype1_id',
        '5' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phenotype_comparison_idx1' => array(
        '0' => 'genotype1_id',
      ),
      'phenotype_comparison_idx2' => array(
        '0' => 'genotype2_id',
      ),
      'phenotype_comparison_idx4' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phenotype_comparison_cvterm_schema()
 * Purpose: To describe the structure of phenotype_comparison_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phenotype_comparison_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phenotype_comparison_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_comparison_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phenotype_comparison_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'phenotype_comparison_cvterm_id',
    ),
    'unique keys' => array(
      'phenotype_comparison_cvterm_c1' => array(
        '0' => 'phenotype_comparison_id',
        '1' => 'cvterm_id',
      ),
    ),
    'indexes' => array(
      'phenotype_comparison_cvterm_idx1' => array(
        '0' => 'phenotype_comparison_id',
      ),
      'phenotype_comparison_cvterm_idx2' => array(
        '0' => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylonode_relationship_schema()
 * Purpose: To describe the structure of phylonode_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylonode_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylonode_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'phylotree_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phylonode_relationship_id',
    ),
    'unique keys' => array(
      'subject_id' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'phylonode_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'phylonode_relationship_idx2' => array(
        '0' => 'object_id',
      ),
      'phylonode_relationship_idx3' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylonodeprop_schema()
 * Purpose: To describe the structure of phylonodeprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylonodeprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylonodeprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonodeprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phylonode_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
        'default' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'phylonodeprop_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        '0' => 'phylonode_id',
        '1' => 'type_id',
        '2' => 'value',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'phylonodeprop_idx1' => array(
        '0' => 'phylonode_id',
      ),
      'phylonodeprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylotree_schema()
 * Purpose: To describe the structure of phylotree to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylotree_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylotree',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylotree_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'analysis_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'comment' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'phylotree_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'phylotree_idx1' => array(
        '0' => 'phylotree_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylotree_pub_schema()
 * Purpose: To describe the structure of phylotree_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylotree_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylotree_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylotree_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phylotree_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phylotree_pub_id',
    ),
    'unique keys' => array(
      'phylotree_id' => array(
        '0' => 'phylotree_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phylotree_pub_idx1' => array(
        '0' => 'phylotree_id',
      ),
      'phylotree_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylonode_pub_schema()
 * Purpose: To describe the structure of phylonode_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylonode_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylonode_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phylonode_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phylonode_pub_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        '0' => 'phylonode_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phylonode_pub_idx1' => array(
        '0' => 'phylonode_id',
      ),
      'phylonode_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylonode_organism_schema()
 * Purpose: To describe the structure of phylonode_organism to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylonode_organism_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylonode_organism',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phylonode_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phylonode_organism_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        '0' => 'phylonode_id',
      ),
    ),
    'indexes' => array(
      'phylonode_organism_idx1' => array(
        '0' => 'phylonode_id',
      ),
      'phylonode_organism_idx2' => array(
        '0' => 'organism_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phenotype_cvterm_schema()
 * Purpose: To describe the structure of phenotype_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phenotype_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phenotype_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenotype_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phenotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'phenotype_cvterm_id',
    ),
    'unique keys' => array(
      'phenotype_cvterm_c1' => array(
        '0' => 'phenotype_id',
        '1' => 'cvterm_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'phenotype_cvterm_idx1' => array(
        '0' => 'phenotype_id',
      ),
      'phenotype_cvterm_idx2' => array(
        '0' => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phenstatement_schema()
 * Purpose: To describe the structure of phenstatement to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phenstatement_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phenstatement',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phenstatement_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'genotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'environment_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'phenotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phenstatement_id',
    ),
    'unique keys' => array(
      'phenstatement_c1' => array(
        '0' => 'genotype_id',
        '1' => 'phenotype_id',
        '2' => 'environment_id',
        '3' => 'type_id',
        '4' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'phenstatement_idx1' => array(
        '0' => 'genotype_id',
      ),
      'phenstatement_idx2' => array(
        '0' => 'phenotype_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylonode_schema()
 * Purpose: To describe the structure of phylonode to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylonode_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylonode',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phylotree_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'parent_phylonode_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'left_idx' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'right_idx' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'label' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'distance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'phylonode_id',
    ),
    'unique keys' => array(
      'phylotree_id' => array(
        '0' => 'phylotree_id',
        '1' => 'left_idx',
      ),
      'phylonode_phylotree_id_key1' => array(
        '0' => 'phylotree_id',
        '1' => 'right_idx',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_phylonode_dbxref_schema()
 * Purpose: To describe the structure of phylonode_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_phylonode_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'phylonode_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'phylonode_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'phylonode_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'phylonode_dbxref_id',
    ),
    'unique keys' => array(
      'phylonode_id' => array(
        '0' => 'phylonode_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'phylonode_dbxref_idx1' => array(
        '0' => 'phylonode_id',
      ),
      'phylonode_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_library_synonym_schema()
 * Purpose: To describe the structure of library_synonym to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_library_synonym_schema () {
  $description = array();
  
  $description = array(
    'table' => 'library_synonym',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_synonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'synonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'ru',
      ),
      'is_internal' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      '0' => 'library_synonym_id',
    ),
    'unique keys' => array(
      'library_synonym_c1' => array(
        '0' => 'synonym_id',
        '1' => 'library_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'library_synonym_idx1' => array(
        '0' => 'synonym_id',
      ),
      'library_synonym_idx2' => array(
        '0' => 'library_id',
      ),
      'library_synonym_idx3' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_library_pub_schema()
 * Purpose: To describe the structure of library_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_library_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'library_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'library_pub_id',
    ),
    'unique keys' => array(
      'library_pub_c1' => array(
        '0' => 'library_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'library_pub_idx1' => array(
        '0' => 'library_id',
      ),
      'library_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featureprop_pub_schema()
 * Purpose: To describe the structure of featureprop_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featureprop_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featureprop_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureprop_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'featureprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'featureprop_pub_id',
    ),
    'unique keys' => array(
      'featureprop_pub_c1' => array(
        '0' => 'featureprop_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'featureprop_pub_idx1' => array(
        '0' => 'featureprop_id',
      ),
      'featureprop_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featureprop_schema()
 * Purpose: To describe the structure of featureprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featureprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featureprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'featureprop_id',
    ),
    'unique keys' => array(
      'featureprop_c1' => array(
        '0' => 'feature_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'featureprop_idx1' => array(
        '0' => 'feature_id',
      ),
      'featureprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featurerange_schema()
 * Purpose: To describe the structure of featurerange to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featurerange_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featurerange',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featurerange_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'featuremap_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'leftstartf_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'leftendf_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'rightstartf_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'rightendf_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rangestr' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'featurerange_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'featurerange_idx1' => array(
        '0' => 'featuremap_id',
      ),
      'featurerange_idx2' => array(
        '0' => 'feature_id',
      ),
      'featurerange_idx3' => array(
        '0' => 'leftstartf_id',
      ),
      'featurerange_idx4' => array(
        '0' => 'leftendf_id',
      ),
      'featurerange_idx5' => array(
        '0' => 'rightstartf_id',
      ),
      'featurerange_idx6' => array(
        '0' => 'rightendf_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featureset_meets_schema()
 * Purpose: To describe the structure of featureset_meets to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featureset_meets_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featureset_meets',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_fnr_type_schema()
 * Purpose: To describe the structure of fnr_type to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_fnr_type_schema () {
  $description = array();
  
  $description = array(
    'table' => 'fnr_type',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'residues' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'seqlen' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'md5checksum' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => '',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'timeaccessioned' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
      ),
      'timelastmodified' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featurepos_schema()
 * Purpose: To describe the structure of featurepos to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featurepos_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featurepos',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featurepos_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'featuremap_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'map_feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'mappos' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'featurepos_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'featurepos_idx1' => array(
        '0' => 'featuremap_id',
      ),
      'featurepos_idx2' => array(
        '0' => 'feature_id',
      ),
      'featurepos_idx3' => array(
        '0' => 'map_feature_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featuremap_pub_schema()
 * Purpose: To describe the structure of featuremap_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featuremap_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featuremap_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featuremap_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'featuremap_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'featuremap_pub_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'featuremap_pub_idx1' => array(
        '0' => 'featuremap_id',
      ),
      'featuremap_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_union_schema()
 * Purpose: To describe the structure of feature_union to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_union_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_union',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'subject_strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'object_strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'fmin' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmax' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featureloc_schema()
 * Purpose: To describe the structure of featureloc to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featureloc_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featureloc',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureloc_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmin' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'is_fmin_partial' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
      'fmax' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'is_fmax_partial' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
      'strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'phase' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'residue_info' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'locgroup' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'featureloc_id',
    ),
    'unique keys' => array(
      'featureloc_c1' => array(
        '0' => 'feature_id',
        '1' => 'locgroup',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'binloc_boxrange' => array(
        '0' => 'fmin',
      ),
      'binloc_boxrange_src' => array(
        '0' => 'srcfeature_id',
        '1' => 'fmin',
      ),
      'featureloc_idx1' => array(
        '0' => 'feature_id',
      ),
      'featureloc_idx2' => array(
        '0' => 'srcfeature_id',
      ),
      'featureloc_idx3' => array(
        '0' => 'srcfeature_id',
        '1' => 'fmin',
        '2' => 'fmax',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featureloc_pub_schema()
 * Purpose: To describe the structure of featureloc_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featureloc_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featureloc_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureloc_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'featureloc_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'featureloc_pub_id',
    ),
    'unique keys' => array(
      'featureloc_pub_c1' => array(
        '0' => 'featureloc_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'featureloc_pub_idx1' => array(
        '0' => 'featureloc_id',
      ),
      'featureloc_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_featuremap_schema()
 * Purpose: To describe the structure of featuremap to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_featuremap_schema () {
  $description = array();
  
  $description = array(
    'table' => 'featuremap',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featuremap_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'unittype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'featuremap_id',
    ),
    'unique keys' => array(
      'featuremap_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_fp_key_schema()
 * Purpose: To describe the structure of fp_key to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_fp_key_schema () {
  $description = array();
  
  $description = array(
    'table' => 'fp_key',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'pkey' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_genotype_schema()
 * Purpose: To describe the structure of genotype to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_genotype_schema () {
  $description = array();
  
  $description = array(
    'table' => 'genotype',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'genotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'genotype_id',
    ),
    'unique keys' => array(
      'genotype_c1' => array(
        '0' => 'uniquename',
      ),
    ),
    'indexes' => array(
      'genotype_idx1' => array(
        '0' => 'uniquename',
      ),
      'genotype_idx2' => array(
        '0' => 'name',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_library_cvterm_schema()
 * Purpose: To describe the structure of library_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_library_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'library_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'library_cvterm_id',
    ),
    'unique keys' => array(
      'library_cvterm_c1' => array(
        '0' => 'library_id',
        '1' => 'cvterm_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'library_cvterm_idx1' => array(
        '0' => 'library_id',
      ),
      'library_cvterm_idx2' => array(
        '0' => 'cvterm_id',
      ),
      'library_cvterm_idx3' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_library_dbxref_schema()
 * Purpose: To describe the structure of library_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_library_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'library_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      '0' => 'library_dbxref_id',
    ),
    'unique keys' => array(
      'library_dbxref_c1' => array(
        '0' => 'library_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'library_dbxref_idx1' => array(
        '0' => 'library_id',
      ),
      'library_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_library_feature_schema()
 * Purpose: To describe the structure of library_feature to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_library_feature_schema () {
  $description = array();
  
  $description = array(
    'table' => 'library_feature',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'library_feature_id',
    ),
    'unique keys' => array(
      'library_feature_c1' => array(
        '0' => 'library_id',
        '1' => 'feature_id',
      ),
    ),
    'indexes' => array(
      'library_feature_idx1' => array(
        '0' => 'library_id',
      ),
      'library_feature_idx2' => array(
        '0' => 'feature_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_library_feature_count_schema()
 * Purpose: To describe the structure of library_feature_count to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_library_feature_count_schema () {
  $description = array();
  
  $description = array(
    'table' => 'library_feature_count',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'num_features' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'feature_type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
      'library_feature_count_index' => array(
        '0' => 'library_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_library_schema()
 * Purpose: To describe the structure of library to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_library_schema () {
  $description = array();
  
  $description = array(
    'table' => 'library',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_obsolete' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
      'timeaccessioned' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '1',
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      '0' => 'library_id',
    ),
    'unique keys' => array(
      'library_c1' => array(
        '0' => 'organism_id',
        '1' => 'uniquename',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'library_idx1' => array(
        '0' => 'organism_id',
      ),
      'library_idx2' => array(
        '0' => 'type_id',
      ),
      'library_idx3' => array(
        '0' => 'uniquename',
      ),
      'library_name_ind1' => array(
        '0' => 'name',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_intronloc_view_schema()
 * Purpose: To describe the structure of intronloc_view to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_intronloc_view_schema () {
  $description = array();
  
  $description = array(
    'table' => 'intronloc_view',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'exon1_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'exon2_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmin' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmax' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_gff3atts_schema()
 * Purpose: To describe the structure of gff3atts to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_gff3atts_schema () {
  $description = array();
  
  $description = array(
    'table' => 'gff3atts',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'attribute' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_gff3view_schema()
 * Purpose: To describe the structure of gff3view to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_gff3view_schema () {
  $description = array();
  
  $description = array(
    'table' => 'gff3view',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'ref' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'source' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'fstart' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fend' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'score' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '',
      ),
      'strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'phase' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'seqlen' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_gffatts_schema()
 * Purpose: To describe the structure of gffatts to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_gffatts_schema () {
  $description = array();
  
  $description = array(
    'table' => 'gffatts',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'attribute' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_intron_combined_view_schema()
 * Purpose: To describe the structure of intron_combined_view to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_intron_combined_view_schema () {
  $description = array();
  
  $description = array(
    'table' => 'intron_combined_view',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'exon1_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'exon2_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmin' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmax' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'intron_rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'transcript_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_project_schema()
 * Purpose: To describe the structure of project to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_project_schema () {
  $description = array();
  
  $description = array(
    'table' => 'project',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'project_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'project_id',
    ),
    'unique keys' => array(
      'project_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_protein_coding_gene_schema()
 * Purpose: To describe the structure of protein_coding_gene to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_protein_coding_gene_schema () {
  $description = array();
  
  $description = array(
    'table' => 'protein_coding_gene',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'residues' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'seqlen' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'md5checksum' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => '',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'is_analysis' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '',
      ),
      'is_obsolete' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '',
      ),
      'timeaccessioned' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
      ),
      'timelastmodified' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_studyfactorvalue_schema()
 * Purpose: To describe the structure of studyfactorvalue to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_studyfactorvalue_schema () {
  $description = array();
  
  $description = array(
    'table' => 'studyfactorvalue',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyfactorvalue_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'studyfactor_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'factorvalue' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'studyfactorvalue_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'studyfactorvalue_idx1' => array(
        '0' => 'studyfactor_id',
      ),
      'studyfactorvalue_idx2' => array(
        '0' => 'assay_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_studyfactor_schema()
 * Purpose: To describe the structure of studyfactor to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_studyfactor_schema () {
  $description = array();
  
  $description = array(
    'table' => 'studyfactor',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyfactor_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'studydesign_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'studyfactor_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'studyfactor_idx1' => array(
        '0' => 'studydesign_id',
      ),
      'studyfactor_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_studyprop_schema()
 * Purpose: To describe the structure of studyprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_studyprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'studyprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'study_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'studyprop_id',
    ),
    'unique keys' => array(
      'study_id' => array(
        '0' => 'study_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'studyprop_idx1' => array(
        '0' => 'study_id',
      ),
      'studyprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_studyprop_feature_schema()
 * Purpose: To describe the structure of studyprop_feature to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_studyprop_feature_schema () {
  $description = array();
  
  $description = array(
    'table' => 'studyprop_feature',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studyprop_feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'studyprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'studyprop_feature_id',
    ),
    'unique keys' => array(
      'studyprop_id' => array(
        '0' => 'studyprop_id',
        '1' => 'feature_id',
      ),
    ),
    'indexes' => array(
      'studyprop_feature_idx1' => array(
        '0' => 'studyprop_id',
      ),
      'studyprop_feature_idx2' => array(
        '0' => 'feature_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_synonym_schema()
 * Purpose: To describe the structure of synonym to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_synonym_schema () {
  $description = array();
  
  $description = array(
    'table' => 'synonym',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'synonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'synonym_sgml' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'synonym_id',
    ),
    'unique keys' => array(
      'synonym_c1' => array(
        '0' => 'name',
        '1' => 'type_id',
      ),
    ),
    'indexes' => array(
      'synonym_idx1' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_studydesignprop_schema()
 * Purpose: To describe the structure of studydesignprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_studydesignprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'studydesignprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studydesignprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'studydesign_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'studydesignprop_id',
    ),
    'unique keys' => array(
      'studydesignprop_c1' => array(
        '0' => 'studydesign_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'studydesignprop_idx1' => array(
        '0' => 'studydesign_id',
      ),
      'studydesignprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_studydesign_schema()
 * Purpose: To describe the structure of studydesign to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_studydesign_schema () {
  $description = array();
  
  $description = array(
    'table' => 'studydesign',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'studydesign_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'study_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'studydesign_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'studydesign_idx1' => array(
        '0' => 'study_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stockprop_schema()
 * Purpose: To describe the structure of stockprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stockprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stockprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'stockprop_id',
    ),
    'unique keys' => array(
      'stockprop_c1' => array(
        '0' => 'stock_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'stockprop_idx1' => array(
        '0' => 'stock_id',
      ),
      'stockprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stockprop_pub_schema()
 * Purpose: To describe the structure of stockprop_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stockprop_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stockprop_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockprop_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stockprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'stockprop_pub_id',
    ),
    'unique keys' => array(
      'stockprop_pub_c1' => array(
        '0' => 'stockprop_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stockprop_pub_idx1' => array(
        '0' => 'stockprop_id',
      ),
      'stockprop_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_study_schema()
 * Purpose: To describe the structure of study to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_study_schema () {
  $description = array();
  
  $description = array(
    'table' => 'study',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'study_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'contact_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'study_id',
    ),
    'unique keys' => array(
      'study_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
      'study_idx1' => array(
        '0' => 'contact_id',
      ),
      'study_idx2' => array(
        '0' => 'pub_id',
      ),
      'study_idx3' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_study_assay_schema()
 * Purpose: To describe the structure of study_assay to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_study_assay_schema () {
  $description = array();
  
  $description = array(
    'table' => 'study_assay',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'study_assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'study_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'study_assay_id',
    ),
    'unique keys' => array(
      'study_assay_c1' => array(
        '0' => 'study_id',
        '1' => 'assay_id',
      ),
    ),
    'indexes' => array(
      'study_assay_idx1' => array(
        '0' => 'study_id',
      ),
      'study_assay_idx2' => array(
        '0' => 'assay_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_tableinfo_schema()
 * Purpose: To describe the structure of tableinfo to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_tableinfo_schema () {
  $description = array();
  
  $description = array(
    'table' => 'tableinfo',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'tableinfo_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '30',
        'not null' => '1',
      ),
      'primary_key_column' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '30',
        'not null' => '',
      ),
      'is_view' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
      'view_on_table_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'superclass_table_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'is_updateable' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '1',
      ),
      'modification_date' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'date',
        'not null' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      '0' => 'tableinfo_id',
    ),
    'unique keys' => array(
      'tableinfo_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_treatment_schema()
 * Purpose: To describe the structure of treatment to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_treatment_schema () {
  $description = array();
  
  $description = array(
    'table' => 'treatment',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'treatment_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
      'biomaterial_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'protocol_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'treatment_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'treatment_idx1' => array(
        '0' => 'biomaterial_id',
      ),
      'treatment_idx2' => array(
        '0' => 'type_id',
      ),
      'treatment_idx3' => array(
        '0' => 'protocol_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_tripal_obo_schema()
 * Purpose: To describe the structure of tripal_obo to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_tripal_obo_schema () {
  $description = array();
  
  $description = array(
    'table' => 'tripal_obo',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cv_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'file' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'url' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'cv_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'cv_id' => array(
        '0' => 'cv_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_tripal_organism_views_common_name_schema()
 * Purpose: To describe the structure of tripal_organism_views_common_name to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_tripal_organism_views_common_name_schema () {
  $description = array();
  
  $description = array(
    'table' => 'tripal_organism_views_common_name',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'nid' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'common_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
        'default' => 'NA',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_type_feature_count_schema()
 * Purpose: To describe the structure of type_feature_count to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_type_feature_count_schema () {
  $description = array();
  
  $description = array(
    'table' => 'type_feature_count',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'num_features' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stockcollectionprop_schema()
 * Purpose: To describe the structure of stockcollectionprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stockcollectionprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stockcollectionprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockcollectionprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stockcollection_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'stockcollectionprop_id',
    ),
    'unique keys' => array(
      'stockcollectionprop_c1' => array(
        '0' => 'stockcollection_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'stockcollectionprop_idx1' => array(
        '0' => 'stockcollection_id',
      ),
      'stockcollectionprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stockcollection_stock_schema()
 * Purpose: To describe the structure of stockcollection_stock to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stockcollection_stock_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stockcollection_stock',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockcollection_stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stockcollection_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'stockcollection_stock_id',
    ),
    'unique keys' => array(
      'stockcollection_stock_c1' => array(
        '0' => 'stockcollection_id',
        '1' => 'stock_id',
      ),
    ),
    'indexes' => array(
      'stockcollection_stock_idx1' => array(
        '0' => 'stockcollection_id',
      ),
      'stockcollection_stock_idx2' => array(
        '0' => 'stock_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_public_feature_schema()
 * Purpose: To describe the structure of public_feature to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_public_feature_schema () {
  $description = array();
  
  $description = array(
    'table' => 'public_feature',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'vid' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'nid' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
      'sync_date' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'nid',
    ),
    'unique keys' => array(
      'nid_vid' => array(
        '0' => 'nid',
        '1' => 'vid',
      ),
      'vid' => array(
        '0' => 'vid',
      ),
    ),
    'indexes' => array(
      'feature_id' => array(
        '0' => 'feature_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_public_library_schema()
 * Purpose: To describe the structure of public_library to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_public_library_schema () {
  $description = array();
  
  $description = array(
    'table' => 'public_library',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'vid' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'nid' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'nid',
    ),
    'unique keys' => array(
      'nid_vid' => array(
        '0' => 'nid',
        '1' => 'vid',
      ),
      'vid' => array(
        '0' => 'vid',
      ),
    ),
    'indexes' => array(
      'library_id' => array(
        '0' => 'library_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_public_organism_schema()
 * Purpose: To describe the structure of public_organism to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_public_organism_schema () {
  $description = array();
  
  $description = array(
    'table' => 'public_organism',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'vid' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'nid' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'unsigned' => '1',
        'not null' => '1',
        'default' => '0',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'nid',
    ),
    'unique keys' => array(
      'nid_vid' => array(
        '0' => 'nid',
        '1' => 'vid',
      ),
      'vid' => array(
        '0' => 'vid',
      ),
    ),
    'indexes' => array(
      'organism_id' => array(
        '0' => 'organism_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_pubprop_schema()
 * Purpose: To describe the structure of pubprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_pubprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'pubprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pubprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'pubprop_id',
    ),
    'unique keys' => array(
      'pubprop_c1' => array(
        '0' => 'pub_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'pubprop_idx1' => array(
        '0' => 'pub_id',
      ),
      'pubprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_pubauthor_schema()
 * Purpose: To describe the structure of pubauthor to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_pubauthor_schema () {
  $description = array();
  
  $description = array(
    'table' => 'pubauthor',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pubauthor_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'editor' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '',
        'default' => 'als',
      ),
      'surname' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => '1',
      ),
      'givennames' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => '',
      ),
      'suffix' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '100',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'pubauthor_id',
    ),
    'unique keys' => array(
      'pubauthor_c1' => array(
        '0' => 'pub_id',
        '1' => 'rank',
      ),
    ),
    'indexes' => array(
      'pubauthor_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_pub_relationship_schema()
 * Purpose: To describe the structure of pub_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_pub_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'pub_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pub_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'pub_relationship_id',
    ),
    'unique keys' => array(
      'pub_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'pub_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'pub_relationship_idx2' => array(
        '0' => 'object_id',
      ),
      'pub_relationship_idx3' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_protocol_schema()
 * Purpose: To describe the structure of protocol to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_protocol_schema () {
  $description = array();
  
  $description = array(
    'table' => 'protocol',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'protocol_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'uri' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'protocoldescription' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'hardwaredescription' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'softwaredescription' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'protocol_id',
    ),
    'unique keys' => array(
      'protocol_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
      'protocol_idx1' => array(
        '0' => 'type_id',
      ),
      'protocol_idx2' => array(
        '0' => 'pub_id',
      ),
      'protocol_idx3' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_protocolparam_schema()
 * Purpose: To describe the structure of protocolparam to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_protocolparam_schema () {
  $description = array();
  
  $description = array(
    'table' => 'protocolparam',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'protocolparam_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'protocol_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'datatype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'unittype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'protocolparam_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'protocolparam_idx1' => array(
        '0' => 'protocol_id',
      ),
      'protocolparam_idx2' => array(
        '0' => 'datatype_id',
      ),
      'protocolparam_idx3' => array(
        '0' => 'unittype_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_pub_schema()
 * Purpose: To describe the structure of pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'title' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'volumetitle' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'volume' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'series_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'issue' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'pyear' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'pages' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'miniref' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_obsolete' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '',
        'default' => 'als',
      ),
      'publisher' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'pubplace' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'pub_id',
    ),
    'unique keys' => array(
      'pub_c1' => array(
        '0' => 'uniquename',
      ),
    ),
    'indexes' => array(
      'pub_idx1' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_pub_dbxref_schema()
 * Purpose: To describe the structure of pub_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_pub_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'pub_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'pub_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      '0' => 'pub_dbxref_id',
    ),
    'unique keys' => array(
      'pub_dbxref_c1' => array(
        '0' => 'pub_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'pub_dbxref_idx1' => array(
        '0' => 'pub_id',
      ),
      'pub_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_quantification_schema()
 * Purpose: To describe the structure of quantification to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_quantification_schema () {
  $description = array();
  
  $description = array(
    'table' => 'quantification',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'quantification_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'acquisition_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'operator_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'protocol_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'analysis_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'quantificationdate' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
        'default' => 'ow(',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'uri' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'quantification_id',
    ),
    'unique keys' => array(
      'quantification_c1' => array(
        '0' => 'name',
        '1' => 'analysis_id',
      ),
    ),
    'indexes' => array(
      'quantification_idx1' => array(
        '0' => 'acquisition_id',
      ),
      'quantification_idx2' => array(
        '0' => 'operator_id',
      ),
      'quantification_idx3' => array(
        '0' => 'protocol_id',
      ),
      'quantification_idx4' => array(
        '0' => 'analysis_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_quantification_relationship_schema()
 * Purpose: To describe the structure of quantification_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_quantification_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'quantification_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'quantification_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'quantification_relationship_id',
    ),
    'unique keys' => array(
      'quantification_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'quantification_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'quantification_relationship_idx2' => array(
        '0' => 'type_id',
      ),
      'quantification_relationship_idx3' => array(
        '0' => 'object_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stock_pub_schema()
 * Purpose: To describe the structure of stock_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stock_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stock_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'stock_pub_id',
    ),
    'unique keys' => array(
      'stock_pub_c1' => array(
        '0' => 'stock_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stock_pub_idx1' => array(
        '0' => 'stock_id',
      ),
      'stock_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stock_relationship_schema()
 * Purpose: To describe the structure of stock_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stock_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stock_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'stock_relationship_id',
    ),
    'unique keys' => array(
      'stock_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'stock_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'stock_relationship_idx2' => array(
        '0' => 'object_id',
      ),
      'stock_relationship_idx3' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stock_relationship_pub_schema()
 * Purpose: To describe the structure of stock_relationship_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stock_relationship_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stock_relationship_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_relationship_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stock_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'stock_relationship_pub_id',
    ),
    'unique keys' => array(
      'stock_relationship_pub_c1' => array(
        '0' => 'stock_relationship_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stock_relationship_pub_idx1' => array(
        '0' => 'stock_relationship_id',
      ),
      'stock_relationship_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stockcollection_schema()
 * Purpose: To describe the structure of stockcollection to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stockcollection_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stockcollection',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stockcollection_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'contact_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'stockcollection_id',
    ),
    'unique keys' => array(
      'stockcollection_c1' => array(
        '0' => 'uniquename',
        '1' => 'type_id',
      ),
    ),
    'indexes' => array(
      'stockcollection_idx1' => array(
        '0' => 'contact_id',
      ),
      'stockcollection_idx2' => array(
        '0' => 'type_id',
      ),
      'stockcollection_idx3' => array(
        '0' => 'uniquename',
      ),
      'stockcollection_name_ind1' => array(
        '0' => 'name',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stock_genotype_schema()
 * Purpose: To describe the structure of stock_genotype to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stock_genotype_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stock_genotype',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_genotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'genotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'stock_genotype_id',
    ),
    'unique keys' => array(
      'stock_genotype_c1' => array(
        '0' => 'stock_id',
        '1' => 'genotype_id',
      ),
    ),
    'indexes' => array(
      'stock_genotype_idx1' => array(
        '0' => 'stock_id',
      ),
      'stock_genotype_idx2' => array(
        '0' => 'genotype_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stock_dbxref_schema()
 * Purpose: To describe the structure of stock_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stock_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stock_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      '0' => 'stock_dbxref_id',
    ),
    'unique keys' => array(
      'stock_dbxref_c1' => array(
        '0' => 'stock_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'stock_dbxref_idx1' => array(
        '0' => 'stock_id',
      ),
      'stock_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_quantificationprop_schema()
 * Purpose: To describe the structure of quantificationprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_quantificationprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'quantificationprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'quantificationprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'quantification_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'quantificationprop_id',
    ),
    'unique keys' => array(
      'quantificationprop_c1' => array(
        '0' => 'quantification_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'quantificationprop_idx1' => array(
        '0' => 'quantification_id',
      ),
      'quantificationprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stats_paths_to_root_schema()
 * Purpose: To describe the structure of stats_paths_to_root to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stats_paths_to_root_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stats_paths_to_root',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'total_paths' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'big',
        'not null' => '',
      ),
      'avg_distance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'numeric',
        'not null' => '',
        'precision' => '0',
        'scale' => '0',
      ),
      'min_distance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'max_distance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stock_schema()
 * Purpose: To describe the structure of stock to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stock_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stock',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_obsolete' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      '0' => 'stock_id',
    ),
    'unique keys' => array(
      'stock_c1' => array(
        '0' => 'organism_id',
        '1' => 'uniquename',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'stock_idx1' => array(
        '0' => 'dbxref_id',
      ),
      'stock_idx2' => array(
        '0' => 'organism_id',
      ),
      'stock_idx3' => array(
        '0' => 'type_id',
      ),
      'stock_idx4' => array(
        '0' => 'uniquename',
      ),
      'stock_name_ind1' => array(
        '0' => 'name',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_stock_cvterm_schema()
 * Purpose: To describe the structure of stock_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_stock_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'stock_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'stock_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'stock_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'stock_cvterm_id',
    ),
    'unique keys' => array(
      'stock_cvterm_c1' => array(
        '0' => 'stock_id',
        '1' => 'cvterm_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'stock_cvterm_idx1' => array(
        '0' => 'stock_id',
      ),
      'stock_cvterm_idx2' => array(
        '0' => 'cvterm_id',
      ),
      'stock_cvterm_idx3' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_synonym_schema()
 * Purpose: To describe the structure of feature_synonym to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_synonym_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_synonym',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_synonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'synonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
      'is_internal' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_synonym_id',
    ),
    'unique keys' => array(
      'feature_synonym_c1' => array(
        '0' => 'synonym_id',
        '1' => 'feature_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_synonym_idx1' => array(
        '0' => 'synonym_id',
      ),
      'feature_synonym_idx2' => array(
        '0' => 'feature_id',
      ),
      'feature_synonym_idx3' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_relationshipprop_pub_schema()
 * Purpose: To describe the structure of feature_relationshipprop_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_relationshipprop_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_relationshipprop_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationshipprop_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_relationshipprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_relationshipprop_pub_id',
    ),
    'unique keys' => array(
      'feature_relationshipprop_pub_c1' => array(
        '0' => 'feature_relationshipprop_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_relationshipprop_pub_idx1' => array(
        '0' => 'feature_relationshipprop_id',
      ),
      'feature_relationshipprop_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_relationship_schema()
 * Purpose: To describe the structure of cell_line_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_relationship_id',
    ),
    'unique keys' => array(
      'cell_line_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_pub_schema()
 * Purpose: To describe the structure of cell_line_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_pub_id',
    ),
    'unique keys' => array(
      'cell_line_pub_c1' => array(
        '0' => 'cell_line_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_synonym_schema()
 * Purpose: To describe the structure of cell_line_synonym to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_synonym_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_synonym',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_synonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'synonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
      'is_internal' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_synonym_id',
    ),
    'unique keys' => array(
      'cell_line_synonym_c1' => array(
        '0' => 'synonym_id',
        '1' => 'cell_line_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_lineprop_schema()
 * Purpose: To describe the structure of cell_lineprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_lineprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_lineprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_lineprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_lineprop_id',
    ),
    'unique keys' => array(
      'cell_lineprop_c1' => array(
        '0' => 'cell_line_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_lineprop_pub_schema()
 * Purpose: To describe the structure of cell_lineprop_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_lineprop_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_lineprop_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_lineprop_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_lineprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_lineprop_pub_id',
    ),
    'unique keys' => array(
      'cell_lineprop_pub_c1' => array(
        '0' => 'cell_lineprop_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_library_schema()
 * Purpose: To describe the structure of cell_line_library to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_library_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_library',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'library_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_library_id',
    ),
    'unique keys' => array(
      'cell_line_library_c1' => array(
        '0' => 'cell_line_id',
        '1' => 'library_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_feature_schema()
 * Purpose: To describe the structure of cell_line_feature to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_feature_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_feature',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_feature_id',
    ),
    'unique keys' => array(
      'cell_line_feature_c1' => array(
        '0' => 'cell_line_id',
        '1' => 'feature_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_schema()
 * Purpose: To describe the structure of cell_line to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'timeaccessioned' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '1',
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_id',
    ),
    'unique keys' => array(
      'cell_line_c1' => array(
        '0' => 'uniquename',
        '1' => 'organism_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_cvterm_schema()
 * Purpose: To describe the structure of cell_line_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_cvterm_id',
    ),
    'unique keys' => array(
      'cell_line_cvterm_c1' => array(
        '0' => 'cell_line_id',
        '1' => 'cvterm_id',
        '2' => 'pub_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_cvtermprop_schema()
 * Purpose: To describe the structure of cell_line_cvtermprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_cvtermprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_cvtermprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_cvtermprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_cvtermprop_id',
    ),
    'unique keys' => array(
      'cell_line_cvtermprop_c1' => array(
        '0' => 'cell_line_cvterm_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cell_line_dbxref_schema()
 * Purpose: To describe the structure of cell_line_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cell_line_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cell_line_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cell_line_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cell_line_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      '0' => 'cell_line_dbxref_id',
    ),
    'unique keys' => array(
      'cell_line_dbxref_c1' => array(
        '0' => 'cell_line_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_channel_schema()
 * Purpose: To describe the structure of channel to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_channel_schema () {
  $description = array();
  
  $description = array(
    'table' => 'channel',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'channel_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'definition' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'channel_id',
    ),
    'unique keys' => array(
      'channel_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_common_ancestor_cvterm_schema()
 * Purpose: To describe the structure of common_ancestor_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_common_ancestor_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'common_ancestor_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm1_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'cvterm2_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'ancestor_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'pathdistance1' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'pathdistance2' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'total_pathdistance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cv_cvterm_count_with_obs_schema()
 * Purpose: To describe the structure of cv_cvterm_count_with_obs to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cv_cvterm_count_with_obs_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cv_cvterm_count_with_obs',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'num_terms_incl_obs' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cv_leaf_schema()
 * Purpose: To describe the structure of cv_leaf to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cv_leaf_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cv_leaf',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cv_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cv_link_count_schema()
 * Purpose: To describe the structure of cv_link_count to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cv_link_count_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cv_link_count',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cv_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'relation_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'relation_cv_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'num_links' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cv_path_count_schema()
 * Purpose: To describe the structure of cv_path_count to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cv_path_count_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cv_path_count',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cv_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'relation_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'relation_cv_name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'num_paths' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cv_cvterm_count_schema()
 * Purpose: To describe the structure of cv_cvterm_count to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cv_cvterm_count_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cv_cvterm_count',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'num_terms_excl_obs' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cv_schema()
 * Purpose: To describe the structure of cv to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cv_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cv',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cv_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'definition' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'cv_id',
    ),
    'unique keys' => array(
      'cv_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_common_descendant_cvterm_schema()
 * Purpose: To describe the structure of common_descendant_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_common_descendant_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'common_descendant_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm1_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'cvterm2_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'ancestor_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'pathdistance1' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'pathdistance2' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'total_pathdistance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_contact_schema()
 * Purpose: To describe the structure of contact to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_contact_schema () {
  $description = array();
  
  $description = array(
    'table' => 'contact',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'contact_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'contact_id',
    ),
    'unique keys' => array(
      'contact_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_contact_relationship_schema()
 * Purpose: To describe the structure of contact_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_contact_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'contact_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'contact_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'contact_relationship_id',
    ),
    'unique keys' => array(
      'contact_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'contact_relationship_idx1' => array(
        '0' => 'type_id',
      ),
      'contact_relationship_idx2' => array(
        '0' => 'subject_id',
      ),
      'contact_relationship_idx3' => array(
        '0' => 'object_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_control_schema()
 * Purpose: To describe the structure of control to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_control_schema () {
  $description = array();
  
  $description = array(
    'table' => 'control',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'control_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'tableinfo_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'row_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'control_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
      'control_idx1' => array(
        '0' => 'type_id',
      ),
      'control_idx2' => array(
        '0' => 'assay_id',
      ),
      'control_idx3' => array(
        '0' => 'tableinfo_id',
      ),
      'control_idx4' => array(
        '0' => 'row_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_biomaterialprop_schema()
 * Purpose: To describe the structure of biomaterialprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_biomaterialprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'biomaterialprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterialprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'biomaterial_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'biomaterialprop_id',
    ),
    'unique keys' => array(
      'biomaterialprop_c1' => array(
        '0' => 'biomaterial_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'biomaterialprop_idx1' => array(
        '0' => 'biomaterial_id',
      ),
      'biomaterialprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_biomaterial_treatment_schema()
 * Purpose: To describe the structure of biomaterial_treatment to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_biomaterial_treatment_schema () {
  $description = array();
  
  $description = array(
    'table' => 'biomaterial_treatment',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_treatment_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'biomaterial_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'treatment_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'unittype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'biomaterial_treatment_id',
    ),
    'unique keys' => array(
      'biomaterial_treatment_c1' => array(
        '0' => 'biomaterial_id',
        '1' => 'treatment_id',
      ),
    ),
    'indexes' => array(
      'biomaterial_treatment_idx1' => array(
        '0' => 'biomaterial_id',
      ),
      'biomaterial_treatment_idx2' => array(
        '0' => 'treatment_id',
      ),
      'biomaterial_treatment_idx3' => array(
        '0' => 'unittype_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_acquisition_relationship_schema()
 * Purpose: To describe the structure of acquisition_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_acquisition_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'acquisition_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'acquisition_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'acquisition_relationship_id',
    ),
    'unique keys' => array(
      'acquisition_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'acquisition_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'acquisition_relationship_idx2' => array(
        '0' => 'type_id',
      ),
      'acquisition_relationship_idx3' => array(
        '0' => 'object_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_acquisition_schema()
 * Purpose: To describe the structure of acquisition to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_acquisition_schema () {
  $description = array();
  
  $description = array(
    'table' => 'acquisition',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'acquisition_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'protocol_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'channel_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'acquisitiondate' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
        'default' => 'ow(',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'uri' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'acquisition_id',
    ),
    'unique keys' => array(
      'acquisition_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
      'acquisition_idx1' => array(
        '0' => 'assay_id',
      ),
      'acquisition_idx2' => array(
        '0' => 'protocol_id',
      ),
      'acquisition_idx3' => array(
        '0' => 'channel_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_acquisitionprop_schema()
 * Purpose: To describe the structure of acquisitionprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_acquisitionprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'acquisitionprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'acquisitionprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'acquisition_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'acquisitionprop_id',
    ),
    'unique keys' => array(
      'acquisitionprop_c1' => array(
        '0' => 'acquisition_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'acquisitionprop_idx1' => array(
        '0' => 'acquisition_id',
      ),
      'acquisitionprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_all_feature_names_schema()
 * Purpose: To describe the structure of all_feature_names to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_all_feature_names_schema () {
  $description = array();
  
  $description = array(
    'table' => 'all_feature_names',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_analysis_schema()
 * Purpose: To describe the structure of analysis to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_analysis_schema () {
  $description = array();
  
  $description = array(
    'table' => 'analysis',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysis_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'program' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'programversion' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'algorithm' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'sourcename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'sourceversion' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'sourceuri' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'timeexecuted' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      '0' => 'analysis_id',
    ),
    'unique keys' => array(
      'analysis_c1' => array(
        '0' => 'program',
        '1' => 'programversion',
        '2' => 'sourcename',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_analysisfeature_schema()
 * Purpose: To describe the structure of analysisfeature to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_analysisfeature_schema () {
  $description = array();
  
  $description = array(
    'table' => 'analysisfeature',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysisfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'analysis_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rawscore' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '',
      ),
      'normscore' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '',
      ),
      'significance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '',
      ),
      'identity' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'analysisfeature_id',
    ),
    'unique keys' => array(
      'analysisfeature_c1' => array(
        '0' => 'feature_id',
        '1' => 'analysis_id',
      ),
    ),
    'indexes' => array(
      'analysisfeature_idx1' => array(
        '0' => 'feature_id',
      ),
      'analysisfeature_idx2' => array(
        '0' => 'analysis_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_analysisfeatureprop_schema()
 * Purpose: To describe the structure of analysisfeatureprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_analysisfeatureprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'analysisfeatureprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysisfeatureprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'analysisfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'analysisfeatureprop_id',
    ),
    'unique keys' => array(
      'analysisfeature_id_type_id_rank' => array(
        '0' => 'analysisfeature_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_assayprop_schema()
 * Purpose: To describe the structure of assayprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_assayprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'assayprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assayprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'assayprop_id',
    ),
    'unique keys' => array(
      'assayprop_c1' => array(
        '0' => 'assay_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'assayprop_idx1' => array(
        '0' => 'assay_id',
      ),
      'assayprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_biomaterial_schema()
 * Purpose: To describe the structure of biomaterial to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_biomaterial_schema () {
  $description = array();
  
  $description = array(
    'table' => 'biomaterial',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'taxon_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'biosourceprovider_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'biomaterial_id',
    ),
    'unique keys' => array(
      'biomaterial_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
      'biomaterial_idx1' => array(
        '0' => 'taxon_id',
      ),
      'biomaterial_idx2' => array(
        '0' => 'biosourceprovider_id',
      ),
      'biomaterial_idx3' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_biomaterial_dbxref_schema()
 * Purpose: To describe the structure of biomaterial_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_biomaterial_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'biomaterial_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'biomaterial_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'biomaterial_dbxref_id',
    ),
    'unique keys' => array(
      'biomaterial_dbxref_c1' => array(
        '0' => 'biomaterial_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'biomaterial_dbxref_idx1' => array(
        '0' => 'biomaterial_id',
      ),
      'biomaterial_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_biomaterial_relationship_schema()
 * Purpose: To describe the structure of biomaterial_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_biomaterial_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'biomaterial_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'biomaterial_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'biomaterial_relationship_id',
    ),
    'unique keys' => array(
      'biomaterial_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'biomaterial_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'biomaterial_relationship_idx2' => array(
        '0' => 'object_id',
      ),
      'biomaterial_relationship_idx3' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_assay_project_schema()
 * Purpose: To describe the structure of assay_project to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_assay_project_schema () {
  $description = array();
  
  $description = array(
    'table' => 'assay_project',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assay_project_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'project_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'assay_project_id',
    ),
    'unique keys' => array(
      'assay_project_c1' => array(
        '0' => 'assay_id',
        '1' => 'project_id',
      ),
    ),
    'indexes' => array(
      'assay_project_idx1' => array(
        '0' => 'assay_id',
      ),
      'assay_project_idx2' => array(
        '0' => 'project_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_assay_biomaterial_schema()
 * Purpose: To describe the structure of assay_biomaterial to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_assay_biomaterial_schema () {
  $description = array();
  
  $description = array(
    'table' => 'assay_biomaterial',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assay_biomaterial_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'biomaterial_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'channel_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'assay_biomaterial_id',
    ),
    'unique keys' => array(
      'assay_biomaterial_c1' => array(
        '0' => 'assay_id',
        '1' => 'biomaterial_id',
        '2' => 'channel_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'assay_biomaterial_idx1' => array(
        '0' => 'assay_id',
      ),
      'assay_biomaterial_idx2' => array(
        '0' => 'biomaterial_id',
      ),
      'assay_biomaterial_idx3' => array(
        '0' => 'channel_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_analysisprop_schema()
 * Purpose: To describe the structure of analysisprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_analysisprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'analysisprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'analysisprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'analysis_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'analysisprop_id',
    ),
    'unique keys' => array(
      'analysisprop_c1' => array(
        '0' => 'analysis_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'analysisprop_idx1' => array(
        '0' => 'analysis_id',
      ),
      'analysisprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_arraydesign_schema()
 * Purpose: To describe the structure of arraydesign to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_arraydesign_schema () {
  $description = array();
  
  $description = array(
    'table' => 'arraydesign',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'arraydesign_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'manufacturer_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'platformtype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'substratetype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'protocol_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'version' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'array_dimensions' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'element_dimensions' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'num_of_elements' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'num_array_columns' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'num_array_rows' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'num_grid_columns' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'num_grid_rows' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'num_sub_columns' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'num_sub_rows' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'arraydesign_id',
    ),
    'unique keys' => array(
      'arraydesign_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
      'arraydesign_idx1' => array(
        '0' => 'manufacturer_id',
      ),
      'arraydesign_idx2' => array(
        '0' => 'platformtype_id',
      ),
      'arraydesign_idx3' => array(
        '0' => 'substratetype_id',
      ),
      'arraydesign_idx4' => array(
        '0' => 'protocol_id',
      ),
      'arraydesign_idx5' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_arraydesignprop_schema()
 * Purpose: To describe the structure of arraydesignprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_arraydesignprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'arraydesignprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'arraydesignprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'arraydesign_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'arraydesignprop_id',
    ),
    'unique keys' => array(
      'arraydesignprop_c1' => array(
        '0' => 'arraydesign_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'arraydesignprop_idx1' => array(
        '0' => 'arraydesign_id',
      ),
      'arraydesignprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_assay_schema()
 * Purpose: To describe the structure of assay to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_assay_schema () {
  $description = array();
  
  $description = array(
    'table' => 'assay',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'assay_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'arraydesign_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'protocol_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'assaydate' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
        'default' => 'ow(',
      ),
      'arrayidentifier' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'arraybatchidentifier' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'operator_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'assay_id',
    ),
    'unique keys' => array(
      'assay_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
      'assay_idx1' => array(
        '0' => 'arraydesign_id',
      ),
      'assay_idx2' => array(
        '0' => 'protocol_id',
      ),
      'assay_idx3' => array(
        '0' => 'operator_id',
      ),
      'assay_idx4' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cv_root_schema()
 * Purpose: To describe the structure of cv_root to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cv_root_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cv_root',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cv_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'root_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cvterm_schema()
 * Purpose: To describe the structure of cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cv_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '1',
      ),
      'definition' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_obsolete' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
      'is_relationshiptype' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'cvterm_id',
    ),
    'unique keys' => array(
      'cvterm_c1' => array(
        '0' => 'name',
        '1' => 'cv_id',
        '2' => 'is_obsolete',
      ),
      'cvterm_c2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'cvterm_idx1' => array(
        '0' => 'cv_id',
      ),
      'cvterm_idx2' => array(
        '0' => 'name',
      ),
      'cvterm_idx3' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_dbxref_schema()
 * Purpose: To describe the structure of feature_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_current' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'ru',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_dbxref_id',
    ),
    'unique keys' => array(
      'feature_dbxref_c1' => array(
        '0' => 'feature_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'feature_dbxref_idx1' => array(
        '0' => 'feature_id',
      ),
      'feature_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_cvtermprop_schema()
 * Purpose: To describe the structure of feature_cvtermprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_cvtermprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_cvtermprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvtermprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_cvtermprop_id',
    ),
    'unique keys' => array(
      'feature_cvtermprop_c1' => array(
        '0' => 'feature_cvterm_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_cvtermprop_idx1' => array(
        '0' => 'feature_cvterm_id',
      ),
      'feature_cvtermprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_difference_schema()
 * Purpose: To describe the structure of feature_difference to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_difference_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_difference',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'fmin' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmax' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_disjoint_schema()
 * Purpose: To describe the structure of feature_disjoint to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_disjoint_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_disjoint',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_distance_schema()
 * Purpose: To describe the structure of feature_distance to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_distance_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_distance',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'subject_strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'object_strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'distance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_cvterm_pub_schema()
 * Purpose: To describe the structure of feature_cvterm_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_cvterm_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_cvterm_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvterm_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_cvterm_pub_id',
    ),
    'unique keys' => array(
      'feature_cvterm_pub_c1' => array(
        '0' => 'feature_cvterm_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_cvterm_pub_idx1' => array(
        '0' => 'feature_cvterm_id',
      ),
      'feature_cvterm_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_cvterm_dbxref_schema()
 * Purpose: To describe the structure of feature_cvterm_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_cvterm_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_cvterm_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvterm_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_cvterm_dbxref_id',
    ),
    'unique keys' => array(
      'feature_cvterm_dbxref_c1' => array(
        '0' => 'feature_cvterm_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'feature_cvterm_dbxref_idx1' => array(
        '0' => 'feature_cvterm_id',
      ),
      'feature_cvterm_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_f_type_schema()
 * Purpose: To describe the structure of f_type to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_f_type_schema () {
  $description = array();
  
  $description = array(
    'table' => 'f_type',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '',
      ),
      'residues' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'seqlen' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'md5checksum' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => '',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'timeaccessioned' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
      ),
      'timelastmodified' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_schema()
 * Purpose: To describe the structure of feature to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'organism_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'residues' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'seqlen' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'md5checksum' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => '',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_analysis' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
      'is_obsolete' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
      'timeaccessioned' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '1',
        'default' => 'ow(',
      ),
      'timelastmodified' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'datetime',
        'not null' => '1',
        'default' => 'ow(',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_id',
    ),
    'unique keys' => array(
      'feature_c1' => array(
        '0' => 'organism_id',
        '1' => 'uniquename',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'feature_idx1' => array(
        '0' => 'dbxref_id',
      ),
      'feature_idx2' => array(
        '0' => 'organism_id',
      ),
      'feature_idx3' => array(
        '0' => 'type_id',
      ),
      'feature_idx4' => array(
        '0' => 'uniquename',
      ),
      'feature_name_ind1' => array(
        '0' => 'name',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_contains_schema()
 * Purpose: To describe the structure of feature_contains to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_contains_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_contains',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_cvterm_schema()
 * Purpose: To describe the structure of feature_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_not' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '1',
        'default' => 'als',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_cvterm_id',
    ),
    'unique keys' => array(
      'feature_cvterm_c1' => array(
        '0' => 'feature_id',
        '1' => 'cvterm_id',
        '2' => 'pub_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_cvterm_idx1' => array(
        '0' => 'feature_id',
      ),
      'feature_cvterm_idx2' => array(
        '0' => 'cvterm_id',
      ),
      'feature_cvterm_idx3' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_expression_schema()
 * Purpose: To describe the structure of feature_expression to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_expression_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_expression',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_expression_id',
    ),
    'unique keys' => array(
      'feature_expression_c1' => array(
        '0' => 'expression_id',
        '1' => 'feature_id',
        '2' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_expression_idx1' => array(
        '0' => 'expression_id',
      ),
      'feature_expression_idx2' => array(
        '0' => 'feature_id',
      ),
      'feature_expression_idx3' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_expressionprop_schema()
 * Purpose: To describe the structure of feature_expressionprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_expressionprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_expressionprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_expressionprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_expressionprop_id',
    ),
    'unique keys' => array(
      'feature_expressionprop_c1' => array(
        '0' => 'feature_expression_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_expressionprop_idx1' => array(
        '0' => 'feature_expression_id',
      ),
      'feature_expressionprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_pubprop_schema()
 * Purpose: To describe the structure of feature_pubprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_pubprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_pubprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_pubprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_pubprop_id',
    ),
    'unique keys' => array(
      'feature_pubprop_c1' => array(
        '0' => 'feature_pub_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_pubprop_idx1' => array(
        '0' => 'feature_pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_relationship_schema()
 * Purpose: To describe the structure of feature_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_relationship_id',
    ),
    'unique keys' => array(
      'feature_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'feature_relationship_idx2' => array(
        '0' => 'object_id',
      ),
      'feature_relationship_idx3' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_relationship_pub_schema()
 * Purpose: To describe the structure of feature_relationship_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_relationship_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_relationship_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationship_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_relationship_pub_id',
    ),
    'unique keys' => array(
      'feature_relationship_pub_c1' => array(
        '0' => 'feature_relationship_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_relationship_pub_idx1' => array(
        '0' => 'feature_relationship_id',
      ),
      'feature_relationship_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_relationshipprop_schema()
 * Purpose: To describe the structure of feature_relationshipprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_relationshipprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_relationshipprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_relationshipprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_relationshipprop_id',
    ),
    'unique keys' => array(
      'feature_relationshipprop_c1' => array(
        '0' => 'feature_relationship_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'feature_relationshipprop_idx1' => array(
        '0' => 'feature_relationship_id',
      ),
      'feature_relationshipprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_pub_schema()
 * Purpose: To describe the structure of feature_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_pub_id',
    ),
    'unique keys' => array(
      'feature_pub_c1' => array(
        '0' => 'feature_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'feature_pub_idx1' => array(
        '0' => 'feature_id',
      ),
      'feature_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_phenotype_schema()
 * Purpose: To describe the structure of feature_phenotype to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_phenotype_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_phenotype',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_phenotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'phenotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_phenotype_id',
    ),
    'unique keys' => array(
      'feature_phenotype_c1' => array(
        '0' => 'feature_id',
        '1' => 'phenotype_id',
      ),
    ),
    'indexes' => array(
      'feature_phenotype_idx1' => array(
        '0' => 'feature_id',
      ),
      'feature_phenotype_idx2' => array(
        '0' => 'phenotype_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_genotype_schema()
 * Purpose: To describe the structure of feature_genotype to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_genotype_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_genotype',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_genotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'genotype_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'chromosome_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cgroup' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'feature_genotype_id',
    ),
    'unique keys' => array(
      'feature_genotype_c1' => array(
        '0' => 'feature_id',
        '1' => 'genotype_id',
        '2' => 'cvterm_id',
        '3' => 'chromosome_id',
        '4' => 'rank',
        '5' => 'cgroup',
      ),
    ),
    'indexes' => array(
      'feature_genotype_idx1' => array(
        '0' => 'feature_id',
      ),
      'feature_genotype_idx2' => array(
        '0' => 'genotype_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_intersection_schema()
 * Purpose: To describe the structure of feature_intersection to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_intersection_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_intersection',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'subject_strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'object_strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'fmin' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'fmax' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_meets_schema()
 * Purpose: To describe the structure of feature_meets to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_meets_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_meets',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_feature_meets_on_same_strand_schema()
 * Purpose: To describe the structure of feature_meets_on_same_strand to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_feature_meets_on_same_strand_schema () {
  $description = array();
  
  $description = array(
    'table' => 'feature_meets_on_same_strand',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_f_loc_schema()
 * Purpose: To describe the structure of f_loc to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_f_loc_schema () {
  $description = array();
  
  $description = array(
    'table' => 'f_loc',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'nbeg' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'nend' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_expressionprop_schema()
 * Purpose: To describe the structure of expressionprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_expressionprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'expressionprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expressionprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'expressionprop_id',
    ),
    'unique keys' => array(
      'expressionprop_c1' => array(
        '0' => 'expression_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'expressionprop_idx1' => array(
        '0' => 'expression_id',
      ),
      'expressionprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_db_dbxref_count_schema()
 * Purpose: To describe the structure of db_dbxref_count to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_db_dbxref_count_schema () {
  $description = array();
  
  $description = array(
    'table' => 'db_dbxref_count',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'num_dbxrefs' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'big',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_dbxref_schema()
 * Purpose: To describe the structure of dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'db_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'accession' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'version' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
        'default' => '',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'dbxref_id',
    ),
    'unique keys' => array(
      'dbxref_c1' => array(
        '0' => 'db_id',
        '1' => 'accession',
        '2' => 'version',
      ),
    ),
    'indexes' => array(
      'dbxref_idx1' => array(
        '0' => 'db_id',
      ),
      'dbxref_idx2' => array(
        '0' => 'accession',
      ),
      'dbxref_idx3' => array(
        '0' => 'version',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_dbxrefprop_schema()
 * Purpose: To describe the structure of dbxrefprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_dbxrefprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'dbxrefprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'dbxrefprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
        'default' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'dbxrefprop_id',
    ),
    'unique keys' => array(
      'dbxrefprop_c1' => array(
        '0' => 'dbxref_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'dbxrefprop_idx1' => array(
        '0' => 'dbxref_id',
      ),
      'dbxrefprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_dfeatureloc_schema()
 * Purpose: To describe the structure of dfeatureloc to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_dfeatureloc_schema () {
  $description = array();
  
  $description = array(
    'table' => 'dfeatureloc',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'featureloc_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'srcfeature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'nbeg' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'is_nbeg_partial' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '',
      ),
      'nend' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'is_nend_partial' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'boolean',
        'not null' => '',
      ),
      'strand' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'size' => 'small',
        'not null' => '',
      ),
      'phase' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'residue_info' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'locgroup' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_db_schema()
 * Purpose: To describe the structure of db to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_db_schema () {
  $description = array();
  
  $description = array(
    'table' => 'db',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'db_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'name' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'urlprefix' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
      'url' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'db_id',
    ),
    'unique keys' => array(
      'db_c1' => array(
        '0' => 'name',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cvtermsynonym_schema()
 * Purpose: To describe the structure of cvtermsynonym to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cvtermsynonym_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cvtermsynonym',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvtermsynonym_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'synonym' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '1024',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'cvtermsynonym_id',
    ),
    'unique keys' => array(
      'cvtermsynonym_c1' => array(
        '0' => 'cvterm_id',
        '1' => 'synonym',
      ),
    ),
    'indexes' => array(
      'cvtermsynonym_idx1' => array(
        '0' => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cvterm_dbxref_schema()
 * Purpose: To describe the structure of cvterm_dbxref to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cvterm_dbxref_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cvterm_dbxref',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm_dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'is_for_definition' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'cvterm_dbxref_id',
    ),
    'unique keys' => array(
      'cvterm_dbxref_c1' => array(
        '0' => 'cvterm_id',
        '1' => 'dbxref_id',
      ),
    ),
    'indexes' => array(
      'cvterm_dbxref_idx1' => array(
        '0' => 'cvterm_id',
      ),
      'cvterm_dbxref_idx2' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cvterm_relationship_schema()
 * Purpose: To describe the structure of cvterm_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cvterm_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cvterm_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvterm_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'cvterm_relationship_id',
    ),
    'unique keys' => array(
      'cvterm_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
      ),
    ),
    'indexes' => array(
      'cvterm_relationship_idx1' => array(
        '0' => 'type_id',
      ),
      'cvterm_relationship_idx2' => array(
        '0' => 'subject_id',
      ),
      'cvterm_relationship_idx3' => array(
        '0' => 'object_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cvtermpath_schema()
 * Purpose: To describe the structure of cvtermpath to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cvtermpath_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cvtermpath',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvtermpath_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cv_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pathdistance' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'cvtermpath_id',
    ),
    'unique keys' => array(
      'cvtermpath_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
        '3' => 'pathdistance',
      ),
    ),
    'indexes' => array(
      'cvtermpath_idx1' => array(
        '0' => 'type_id',
      ),
      'cvtermpath_idx2' => array(
        '0' => 'subject_id',
      ),
      'cvtermpath_idx3' => array(
        '0' => 'object_id',
      ),
      'cvtermpath_idx4' => array(
        '0' => 'cv_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_cvtermprop_schema()
 * Purpose: To describe the structure of cvtermprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_cvtermprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'cvtermprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'cvtermprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
        'default' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'cvtermprop_id',
    ),
    'unique keys' => array(
      'cvterm_id' => array(
        '0' => 'cvterm_id',
        '1' => 'type_id',
        '2' => 'value',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'cvtermprop_idx1' => array(
        '0' => 'cvterm_id',
      ),
      'cvtermprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_eimage_schema()
 * Purpose: To describe the structure of eimage to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_eimage_schema () {
  $description = array();
  
  $description = array(
    'table' => 'eimage',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'eimage_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'eimage_data' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'eimage_type' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '1',
      ),
      'image_uri' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'varchar',
        'length' => '255',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'eimage_id',
    ),
    'unique keys' => array(
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_element_schema()
 * Purpose: To describe the structure of element to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_element_schema () {
  $description = array();
  
  $description = array(
    'table' => 'element',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'element_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'feature_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'arraydesign_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
      'dbxref_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'element_id',
    ),
    'unique keys' => array(
      'element_c1' => array(
        '0' => 'feature_id',
        '1' => 'arraydesign_id',
      ),
    ),
    'indexes' => array(
      'element_idx1' => array(
        '0' => 'feature_id',
      ),
      'element_idx2' => array(
        '0' => 'arraydesign_id',
      ),
      'element_idx3' => array(
        '0' => 'type_id',
      ),
      'element_idx4' => array(
        '0' => 'dbxref_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_expression_cvterm_schema()
 * Purpose: To describe the structure of expression_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_expression_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'expression_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
      'cvterm_type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'expression_cvterm_id',
    ),
    'unique keys' => array(
      'expression_cvterm_c1' => array(
        '0' => 'expression_id',
        '1' => 'cvterm_id',
        '2' => 'cvterm_type_id',
      ),
    ),
    'indexes' => array(
      'expression_cvterm_idx1' => array(
        '0' => 'expression_id',
      ),
      'expression_cvterm_idx2' => array(
        '0' => 'cvterm_id',
      ),
      'expression_cvterm_idx3' => array(
        '0' => 'cvterm_type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_expression_cvtermprop_schema()
 * Purpose: To describe the structure of expression_cvtermprop to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_expression_cvtermprop_schema () {
  $description = array();
  
  $description = array(
    'table' => 'expression_cvtermprop',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_cvtermprop_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'expression_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'expression_cvtermprop_id',
    ),
    'unique keys' => array(
      'expression_cvtermprop_c1' => array(
        '0' => 'expression_cvterm_id',
        '1' => 'type_id',
        '2' => 'rank',
      ),
    ),
    'indexes' => array(
      'expression_cvtermprop_idx1' => array(
        '0' => 'expression_cvterm_id',
      ),
      'expression_cvtermprop_idx2' => array(
        '0' => 'type_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_expression_image_schema()
 * Purpose: To describe the structure of expression_image to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_expression_image_schema () {
  $description = array();
  
  $description = array(
    'table' => 'expression_image',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_image_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'eimage_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'expression_image_id',
    ),
    'unique keys' => array(
      'expression_image_c1' => array(
        '0' => 'expression_id',
        '1' => 'eimage_id',
      ),
    ),
    'indexes' => array(
      'expression_image_idx1' => array(
        '0' => 'expression_id',
      ),
      'expression_image_idx2' => array(
        '0' => 'eimage_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_expression_pub_schema()
 * Purpose: To describe the structure of expression_pub to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_expression_pub_schema () {
  $description = array();
  
  $description = array(
    'table' => 'expression_pub',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'pub_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'expression_pub_id',
    ),
    'unique keys' => array(
      'expression_pub_c1' => array(
        '0' => 'expression_id',
        '1' => 'pub_id',
      ),
    ),
    'indexes' => array(
      'expression_pub_idx1' => array(
        '0' => 'expression_id',
      ),
      'expression_pub_idx2' => array(
        '0' => 'pub_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_expression_schema()
 * Purpose: To describe the structure of expression to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_expression_schema () {
  $description = array();
  
  $description = array(
    'table' => 'expression',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'expression_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'md5checksum' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'char',
        'length' => '32',
        'not null' => '',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'expression_id',
    ),
    'unique keys' => array(
      'expression_c1' => array(
        '0' => 'uniquename',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_environment_cvterm_schema()
 * Purpose: To describe the structure of environment_cvterm to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_environment_cvterm_schema () {
  $description = array();
  
  $description = array(
    'table' => 'environment_cvterm',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'environment_cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'environment_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'cvterm_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'environment_cvterm_id',
    ),
    'unique keys' => array(
      'environment_cvterm_c1' => array(
        '0' => 'environment_id',
        '1' => 'cvterm_id',
      ),
    ),
    'indexes' => array(
      'environment_cvterm_idx1' => array(
        '0' => 'environment_id',
      ),
      'environment_cvterm_idx2' => array(
        '0' => 'cvterm_id',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_element_relationship_schema()
 * Purpose: To describe the structure of element_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_element_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'element_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'element_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'element_relationship_id',
    ),
    'unique keys' => array(
      'element_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'element_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'element_relationship_idx2' => array(
        '0' => 'type_id',
      ),
      'element_relationship_idx3' => array(
        '0' => 'object_id',
      ),
      'element_relationship_idx4' => array(
        '0' => 'value',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_elementresult_schema()
 * Purpose: To describe the structure of elementresult to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_elementresult_schema () {
  $description = array();
  
  $description = array(
    'table' => 'elementresult',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'elementresult_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'element_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'quantification_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'signal' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'float',
        'size' => 'big',
        'not null' => '1',
      ),
    ),
    'primary key' => array(
      '0' => 'elementresult_id',
    ),
    'unique keys' => array(
      'elementresult_c1' => array(
        '0' => 'element_id',
        '1' => 'quantification_id',
      ),
    ),
    'indexes' => array(
      'elementresult_idx1' => array(
        '0' => 'element_id',
      ),
      'elementresult_idx2' => array(
        '0' => 'quantification_id',
      ),
      'elementresult_idx3' => array(
        '0' => 'signal',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_elementresult_relationship_schema()
 * Purpose: To describe the structure of elementresult_relationship to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_elementresult_relationship_schema () {
  $description = array();
  
  $description = array(
    'table' => 'elementresult_relationship',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'elementresult_relationship_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'subject_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'type_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'object_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
      ),
      'value' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
      'rank' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'int',
        'not null' => '1',
        'default' => '0',
      ),
    ),
    'primary key' => array(
      '0' => 'elementresult_relationship_id',
    ),
    'unique keys' => array(
      'elementresult_relationship_c1' => array(
        '0' => 'subject_id',
        '1' => 'object_id',
        '2' => 'type_id',
        '3' => 'rank',
      ),
    ),
    'indexes' => array(
      'elementresult_relationship_idx1' => array(
        '0' => 'subject_id',
      ),
      'elementresult_relationship_idx2' => array(
        '0' => 'type_id',
      ),
      'elementresult_relationship_idx3' => array(
        '0' => 'object_id',
      ),
      'elementresult_relationship_idx4' => array(
        '0' => 'value',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}

/**
 * Implements hook_environment_schema()
 * Purpose: To describe the structure of environment to tripal
 * @see tripal_core.api.inc: tripal_core_chado_insert()
 * @see tripal_core.api.inc: tripal_core_chado_update()
 * @see tripal_core.api.inc: tripal_core_chado_select()
 *
 * @return
 *    An array describing the current table
 *
 * @ingroup tripal_schema_api
 *
*/
function tripal_core_chado_environment_schema () {
  $description = array();
  
  $description = array(
    'table' => 'environment',
//    'description' => 'TODO: please describe this table!',
    'fields' => array(
      'environment_id' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'serial',
        'not null' => '1',
      ),
      'uniquename' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '1',
      ),
      'description' => array(
//        'description' => 'TODO: please describe this field!',
        'type' => 'text',
        'not null' => '',
      ),
    ),
    'primary key' => array(
      '0' => 'environment_id',
    ),
    'unique keys' => array(
      'environment_c1' => array(
        '0' => 'uniquename',
      ),
    ),
    'indexes' => array(
      'environment_idx1' => array(
        '0' => 'uniquename',
      ),
    ),
    'foreign keys' => array(
    ),
  );
  
  return $description;
}
