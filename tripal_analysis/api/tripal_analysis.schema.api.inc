<?php

/**
 * Implements hook_chado_schema_v1_11_analysisfeatureprop()
 * Purpose: To describe the structure of 'analysisfeatureprop' to tripal
 * @see chado_insert_record()
 * @see chado_update_record()
 * @see chado_select_record()
 *
 * @return
 *    An array describing the 'analysisfeatureprop' table
 *
 * @ingroup tripal_chado_v1.11_schema_api
 *
 */
function tripal_core_chado_schema_v1_11_analysisfeatureprop() {
  $description =  array(
    'table' => 'analysisfeatureprop',
    'fields' => array(
      'analysisfeatureprop_id' => array(
        'type' => 'serial',
        'not NULL' => '1',
      ),
      'analysisfeature_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'type_id' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
      'value' => array(
        'type' => 'text',
        'not NULL' => '',
      ),
      'rank' => array(
        'type' => 'int',
        'not NULL' => '1',
      ),
    ),
    'primary key' => array(
      0 => 'analysisfeatureprop_id',
    ),
    'unique keys' => array(
      'analysisfeature_id_type_id_rank' => array(
        0 => 'analysisfeature_id',
        1 => 'type_id',
        2 => 'rank',
      ),
    ),
    'indexes' => array(
    ),
    'foreign keys' => array(
      'cvterm' => array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
      ),
      'analysisfeature' => array(
        'table' => 'analysisfeature',
        'columns' => array(
          'analysisfeature_id' => 'analysisfeature_id',
        ),
      ),
    ),
  );
  return $description;
}