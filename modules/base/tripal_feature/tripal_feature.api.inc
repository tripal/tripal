<?php
 
/**
 * Implements hook_chado_feature_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the feature table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_feature_schema() {
  $description = array();

  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_feature_schema()

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

/**
 * Implements hook_chado_featureprop_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the featureprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_featureprop_schema() {
  $description = array();

  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_featureprop_schema()

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
  );
  
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  
  $referring_tables = array('analysisfeature',
    'featureprop_pub',
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

/**
 * Implements hook_chado_featureloc_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the featureloc table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_featureloc_schema() {
  $description = array();

  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_featureloc_schema()

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
          'srcfeature_id' => 'feature_id'
        ),
  );
  
  $referring_tables = array('analysisfeature',
    'featureloc_pub',
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

/**
 * Implements hook_chado_feature_dbxref_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the feature_dbxref table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_feature_dbxref_schema() {
  $description = array();

  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_feature_dbxref_schema()

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
  );

  $description['foreign keys']['dbxref'] = array(
        'table' => 'dbxref',
        'columns' => array(
          'dbxref_id' => 'dbxref_id',
        ),
  );

  return $description;
}
/**
 * Implements hook_chado_feature_relationship_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the feature_dbxref table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_feature_relationship_schema() {
  $description = array();

  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_feature_dbxref_schema()

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'object_id' => 'feature_id',
          'subject_id' => 'feature_id',
        ),
  );
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  return $description;
}
/**
 * Implements hook_chado_feature_relationship_schema()
 * Purpose: To add descriptions and foreign keys to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the feature_dbxref table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_feature_cvterm_schema() {
  $description = array();

  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_feature_dbxref_schema()

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
  );
  $description['foreign keys']['cvterm'] = array(
        'table' => 'cvterm',
        'columns' => array(
          'type_id' => 'cvterm_id',
        ),
  );
  $description['foreign keys']['pub'] = array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
  );
  return $description;
}
/**
*
* @ingroup tripal_schema_api
*/
function tripal_feature_chado_feature_synonym_schema() {
  $description = array();

  // Default table description in tripal_core.schema.api.inc: tripal_core_chado_feature_dbxref_schema()

  $description['foreign keys']['feature'] = array(
        'table' => 'feature',
        'columns' => array(
          'feature_id' => 'feature_id',
        ),
  );
  $description['foreign keys']['synonym'] = array(
        'table' => 'synonym',
        'columns' => array(
          'synonym_id' => 'synonym_id',
        ),
  );
  $description['foreign keys']['pub'] = array(
        'table' => 'pub',
        'columns' => array(
          'pub_id' => 'pub_id',
        ),
  );

  return $description;
}
