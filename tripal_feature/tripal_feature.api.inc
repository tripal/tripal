<?php
/**
 * @file
 * @todo Add file header description
 */

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
          'cvterm_id' => 'cvterm_id',
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

/**
 * Retrieve properties from the analysisfeatureprop table for a feature.
 *
 * @param $analysis_id
 *    The analysis ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with a feature ID.
 * @param $feature_id
 *    The feature ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with an analysis ID.
 * @param $analysisfeature_id
 *    The analysis feature ID for the analysis feature. This argument is
 *    optional and can be used rather than specifying the $analysis_id and
 *    $feature_id arguments.  If all three arguments are specified (e.g.
 *    an $analysis_id, $feature_id and $analysisfeature_id, then the
 *    $analysisfeature_id is used and the other two arguments are ignored.
 * @param $property
 *    The cvterm name of the properties to retrieve
 * @param $cv_name
 *    Optional.  The name of the cv to which the property belongs.  By
 *    default this is the 'tripal' cv.
 *
 * @return
 *    An analysisfeature chado variable with the specified properties expanded
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_analysis_get_property($analysis_id = NULL, $feature_id = NUll,
   $analysisfeature_id = NULL, $property, $cv_name = 'tripal') {

   // check that the incoming arguments are correct
   if (($analysis_id and !$feature_id) or
       (!$analysis_id and $feature_id)) {
       watchdog('tripal_feature',
          'tripal_feature_analysis_get_property: Both an analysis ID and feature ID should be specified',
           array(), WATCHDOG_WARNING);
   }

   // get the analysisfeature_id if one is not provided
   if (!$analysisfeature_id) {
      $columns = array('analysisfeature_id');
      $values = array('analysis_id' => $analysis_id, 'feature_id' => $feature_id);
      $result = tripal_core_chado_select('analysisfeature', $columns, $values);
      $analysisfeature_id = $result[0]->analysisfeature_id;
   }

   // get the property.
   return tripal_core_get_property('analysisfeature', $analysisfeature_id, $property, $cv_name);
}

/**
 * Insert a property for an analysis feature
 *
 * @param $analysis_id
 *    The analysis ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with a feature ID.
 * @param $feature_id
 *    The feature ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with an analysis ID.
 * @param $analysisfeature_id
 *    The analysis feature ID for the analysis feature. This argument is
 *    optional and can be used rather than specifying the $analysis_id and
 *    $feature_id arguments.  If all three arguments are specified (e.g.
 *    an $analysis_id, $feature_id and $analysisfeature_id, then the
 *    $analysisfeature_id is used and the other two arguments are ignored.
 * @param $property
 *   The cvterm name of the property to insert
 * @param $value
 *   The value of the property to insert
 * @param $update_if_present
 *   A boolean indicated whether to update the record if it's already present
 * @param $cv_name
 *    Optional.  The name of the cv to which the property belongs.  By
 *    default this is the 'tripal' cv.
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_analysis_insert_property($analysis_id = NULL, $feature_id = NUll,
   $analysisfeature_id = NULL, $property, $value, $update_if_present = 0,
   $cv_name = 'tripal') {

   // check that the incoming arguments are correct
   if (($analysis_id and !$feature_id) or
       (!$analysis_id and $feature_id)) {
       watchdog('tripal_feature',
          'tripal_feature_analysis_insert_property: Both an analysis ID and feature ID should be specified',
           array(), WATCHDOG_WARNING);
   }

   // get the analysisfeature_id if one is not provided
   if (!$analysisfeature_id) {
      $columns = array('analysisfeature_id');
      $values = array('analysis_id' => $analysis_id, 'feature_id' => $feature_id);
      $result = tripal_core_chado_select('analysisfeature', $columns, $values);
      $analysisfeature_id = $result[0]->analysisfeature_id;
   }

   // insert the property.
   return tripal_core_insert_property('analysisfeature', $analysisfeature_id,
      $property, $cv_name, $value, $update_if_present);
}

/**
 * Update an analysis feature property using the property name. Use this
 * when a property only exists once for a given analysis feature.  When more
 * than one value can exist for the same property use the
 * tripal_feature_analysis_update_property_by_id() function.
 *
 * @param $analysis_id
 *    The analysis ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with a feature ID.
 * @param $feature_id
 *    The feature ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with an analysis ID.
 * @param $analysisfeature_id
 *    The analysis feature ID for the analysis feature. This argument is
 *    optional and can be used rather than specifying the $analysis_id and
 *    $feature_id arguments.  If all three arguments are specified (e.g.
 *    an $analysis_id, $feature_id and $analysisfeature_id, then the
 *    $analysisfeature_id is used and the other two arguments are ignored.
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 * @param $cv_name
 *    Optional.  The name of the cv to which the property belongs.  By
 *    default this is the 'tripal' cv.
 *
 * Note: The property will be identified using the unique combination of the $analysis_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_analysis_update_property($analysis_id = NULL, $feature_id = NUll,
   $analysisfeature_id = NULL, $property, $value, $insert_if_missing = 0,
   $cv_name = 'tripal') {

   // check that the incoming arguments are correct
   if (($analysis_id and !$feature_id) or
       (!$analysis_id and $feature_id)) {
       watchdog('tripal_feature',
          'tripal_feature_analysis_update_property: Both an analysis ID and feature ID should be specified',
           array(), WATCHDOG_WARNING);
   }

   // get the analysisfeature_id if one is not provided
   if (!$analysisfeature_id) {
      $columns = array('analysisfeature_id');
      $values = array('analysis_id' => $analysis_id, 'feature_id' => $feature_id);
      $result = tripal_core_chado_select('analysisfeature', $columns, $values);
      $analysisfeature_id = $result[0]->analysisfeature_id;
   }

   // update the property.
   return tripal_core_update_property('analysisfeature', $analysisfeature_id, $property, $cv_name, $value, $insert_if_missing);
}

/**
 * Update a property for an analysis feature using the analysisfeatureprop_id.
 *
 * @param $analysisfeatureprop_id
 *    The analysis feature property ID for the analysis feature.
 * @param $property
 *   The cvterm name of the property
 * @param $value
 *   The value of the property
 * @param $cv_name
 *    Optional.  The name of the cv to which the property belongs.  By
 *    default this is the 'tripal' cv.
 * *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_analysis_update_property_by_id($analysisfeatureprop_id,
   $property, $value, $cv_name = 'tripal') {

   // update the property.
   return tripal_core_update_property_by_id('analysisfeature',
      $analysisfeatureprop_id, $property, $cv_name, $value);
}
/**
 * Delete an analysis feature property using the property name.  Use this
 * when a property only exists once for a given analysis feature.  When more
 * than one value can exist for the same property use the
 * tripal_feature_analysis_delete_property_by_id() function.
 *
 * @param $analysis_id
 *    The analysis ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with a feature ID.
 * @param $feature_id
 *    The feature ID for the analysis feature. This argument is optional but
 *    if specified it must also be accompanied with an analysis ID.
 * @param $analysisfeature_id
 *    The analysis feature ID for the analysis feature. This argument is
 *    optional and can be used rather than specifying the $analysis_id and
 *    $feature_id arguments.  If all three arguments are specified (e.g.
 *    an $analysis_id, $feature_id and $analysisfeature_id, then the
 *    $analysisfeature_id is used and the other two arguments are ignored.
 * @param $property
 *    The cvterm name of the property to delete
 * @param $cv_name
 *    Optional.  The name of the cv to which the property belongs.  By
 *    default this is the 'tripal' cv.
 *
 * Note: The property will be identified using the unique combination of the $analysis_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_analysis_delete_property($analysis_id = NULL, $feature_id = NUll,
   $analysisfeature_id = NULL, $property, $cv_name = 'tripal') {
   // check that the incoming arguments are correct
   if (($analysis_id and !$feature_id) or
       (!$analysis_id and $feature_id)) {
       watchdog('tripal_feature',
          'tripal_feature_analysis_delete_property: Both an analysis ID and feature ID should be specified',
           array(), WATCHDOG_WARNING);
   }

   // get the analysisfeature_id if one is not provided
   if (!$analysisfeature_id) {
      $columns = array('analysisfeature_id');
      $values = array('analysis_id' => $analysis_id, 'feature_id' => $feature_id);
      $result = tripal_core_chado_select('analysisfeature', $columns, $values);
      $analysisfeature_id = $result[0]->analysisfeature_id;
   }

   // get the property.
   return tripal_core_delete_property('analysisfeature', $analysisfeature_id, $property, $cv_name);
}
/**
 * Delete a property using the analysisfeatureprop_id
 *
 * @param $analysisfeatureprop_id
 *    The analysis feature property ID for the analysis feature.
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_analysis_delete_property_by_id($analysisfeatureprop_id) {
   // get the property.
   return tripal_core_delete_property_by_id('analysisfeature', $analysisfeatureprop_id);
}
/**
 * Retrieve properties of a given type for a given feature
 *
 * @param $feature_id
 *    The feature_id of the properties you would like to retrieve
 * @param $property
 *    The cvterm name of the properties to retrieve
 * @param $cv_name
 *    Optional.  The name of the cv to which the property belongs.  By
 *    default this is the 'tripal' cv.
 *
 * @return
 *    A feature chado variable with the specified properties expanded
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_get_property($feature_id, $property, $cv_name='tripal') {
   return tripal_core_get_property('feature', $feature_id, $property, $cv_name);
}

/**
 * Insert a given property
 *
 * @param $feature_id
 *   The feature_id of the property to insert
 * @param $property
 *   The cvterm name of the property to insert
 * @param $value
 *   The value of the property to insert
 * @param $update_if_present
 *   A boolean indicated whether to update the record if it's already present
 * @param $cv_name
 *    Optional.  The name of the cv to which the property belongs.  By
 *    default this is the 'tripal' cv.
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_insert_property($feature_id, $property, $value,
   $update_if_present = 0, $cv_name = 'tripal') {
    return tripal_core_insert_property('feature', $feature_id, $property,
       $cv_name, $value, $update_if_present);
}
/**
 * Update a feature property using the property name. Only use this
 * if the property is unique and only exist once for the feature.
 *
 * @param $feature_id
 *   The feature_id of the property to update
 * @param $property
 *   The cvterm name of the property to update
 * @param $value
 *   The value of the property to update
 * @param $insert_if_missing
 *   A boolean indicated whether to insert the record if it's absent
 * @param $cv_name
 *   Optional.  The name of the cv to which the property belongs.  By
 *   default this is the 'tripal' cv.
 *
 * Note: The property will be identified using the unique combination of the $feature_id and $property
 * and then it will be updated with the supplied value
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_update_property($feature_id, $property,
  $value, $insert_if_missing = 0, $cv_name = 'tripal') {
   return tripal_core_update_property('feature', $feature_id, $property, $cv_name, $value, $insert_if_missing);
}

/**
 * Update a given feature property using the featureprop_id
 *
 * @param $featureprop_id
 *   The featureprop_id of the property to update
 * @param $property
 *   The cvterm name of the property
 * @param $value
 *   The value of the property
 * @param $cv_name
 *   Optional.  The name of the cv to which the property belongs.  By
 *   default this is the 'tripal' cv.
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_update_property_by_id($featureprop_id, $property,
  $value, $cv_name = 'tripal') {
   return tripal_core_update_property_by_id('feature', $featureprop_id, $property, $cv_name, $value);
}

/**
 * Delete a given feature property using the property name.  Only use this
 * if the property is unique and only exists once for the feature.
 *
 * @param $feature_id
 *   The feature_id of the property to delete
 * @param $property
 *   The cvterm name of the property to delete
 * @param $cv_name
 *   Optional.  The name of the cv to which the property belongs.  By
 *   default this is the 'tripal' cv.
 *
 * Note: The property will be identified using the unique combination of the $feature_id and $property
 * and then it will be deleted
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_delete_property($feature_id, $property, $cv_name='tripal') {
   return tripal_core_delete_property('feature', $feature_id, $property, $cv_name);
}
/**
 * Delete a given feature property using the featureprop_id
 *
 * @param $featureprop_id
 *   The feature_id of the property to delete
 *
 * @return
 *   True of success, False otherwise
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_delete_property_by_id($featureprop_id) {
   return tripal_core_delete_property_by_id('feature', $featureprop_id);
}
