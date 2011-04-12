<?php

/**
 * Implements hook_chado_library_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the library table
 *
 * @ingroup tripal_library
 */
function tripal_library_chado_library_schema() {
  $description = array();
  
  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_feature_schema()

  $description['foreign keys']['organism'] = array(
        'table' => 'organism',
        'columns' => array(
          'organism_id' => 'organism_id',
        ),
  );  
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $referring_tables = array(
      'library_cvterm',
      'library_feature',
      'library_pub',
      'library_synonym',
      'libraryprop' 
  );
  $description['referring_tables'] = $referring_tables;
  
  return $description;
}

/**
 * Implements hook_chado_library_feature_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the library_feature table
 *
 * @ingroup tripal_library
 */
function tripal_library_chado_library_feature_schema() {
  $description = array();
  
  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_library_feature_schema()

  $description['foreign keys']['library'] = array(
        'table' => 'library',
        'columns' => array(
          'library_id' => 'library_id',
        ),
  );  
  
  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
  );
  
  return $description;
}