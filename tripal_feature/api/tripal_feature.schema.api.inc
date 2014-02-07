<?php
/**
 * @file
 * Further describe some of the feature tables to the tripal schema
 */

/**
 * Implements hook_chado_schema_v1_2_tripal_gff_temp()
 * Purpose: To describe the structure of 'tripal_gff_temp' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'tripal_gff_temp' table
 *
 * @ingroup tripal_chado_v1.2_schema_api
 *
 */
function tripal_feature_chado_schema_v1_2_tripal_gff_temp() {
  $schema = array(
    'table' => 'tripal_gff_temp',
    'fields' => array(
      'feature_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'organism_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      ),
      'uniquename' => array(
        'type' => 'text',
        'not null' => TRUE,
      ),
      'type_name' => array(
        'type' => 'varchar',
        'length' => '1024',
        'not null' => TRUE,
      ),
    ),
    'indexes' => array(
      'tripal_gff_temp_idx0' => array('feature_id'),
      'tripal_gff_temp_idx0' => array('organism_id'),
      'tripal_gff_temp_idx1' => array('uniquename'),
    ),
    'unique keys' => array(
      'tripal_gff_temp_uq0' => array('feature_id'),
      'tripal_gff_temp_uq1' => array('uniquename', 'organism_id', 'type_name'),
    ),
  );
  return $schema;
}