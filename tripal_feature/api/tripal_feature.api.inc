<?php
/**
 * @file
 * @todo Add file header description
 */

/**
 * Implements hook_chado_feature_schema()
 * Purpose: To add descriptions to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the feature table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_schema_v1_11_feature() {
  $description = array();

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
 * Purpose: To add descriptions to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the featureprop table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_schema_v1_11_featureprop() {
  $description = array();

  $referring_tables = array('analysisfeature',
    'featureprop_pub',
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}

/**
 * Implements hook_chado_schema_v1_11_table()
 * Purpose: To add descriptions to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the featureloc table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_schema_v1_11_featureloc() {
  $description = array();

  $referring_tables = array('analysisfeature',
    'featureloc_pub',
  );
  $description['referring_tables'] = $referring_tables;

  return $description;
}
/**
 * Implements hook_chado_schema_v1_2_table()
 * Purpose: To add descriptions and to default table description
 * Note: This array will be merged with the array from all other implementations
 *
 * @return
 *    Array describing the featureloc table
 *
 * @ingroup tripal_schema_api
 */
function tripal_feature_chado_schema_v1_2_featureloc() {
  $description = array();

  $referring_tables = array('analysisfeature',
    'featureloc_pub',
  );
  $description['referring_tables'] = $referring_tables;

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

/**
 * Performs a reverse compliment of a nucleotide sequence
 *
 * @param $sequence
 *   The nucelotide sequence
 *
 * @return
 *   an upper-case reverse complemented sequence
 *
 * @ingroup tripal_feature_api
 */
function trpial_feature_reverse_complement($sequence) {

  $seq = strtoupper($sequence);
  $seq = strrev($seq);
  $seq = str_replace("A", "t", $seq);
  $seq = str_replace("T", "a", $seq);
  $seq = str_replace("G", "c", $seq);
  $seq = str_replace("C", "g", $seq);
  $seq = str_replace("Y", "r", $seq);
  $seq = str_replace("R", "y", $seq);
  $seq = str_replace("W", "w", $seq);
  $seq = str_replace("S", "s", $seq);
  $seq = str_replace("K", "m", $seq);
  $seq = str_replace("M", "k", $seq);
  $seq = str_replace("D", "h", $seq);
  $seq = str_replace("V", "b", $seq);
  $seq = str_replace("H", "d", $seq);
  $seq = str_replace("B", "v", $seq);
  return strtoupper($seq);
}
/**
 * Retrieves the sequence for a feature.  
 *
 * @param $feature_id
 *   The feature_id of the feature for which the sequence will be retrieved
 * @param $feature_name
 *   The feature name. This will appear on the FASTA definition line
 * @param $num_bases_per_line
 *   Indicate the number of bases to use per line.  A new line will be added
 *   after the specified number of bases on each line.
 * @param $derive_from_parent
 *   Set to '1' if the sequence should be obtained from the parent to which
 *   this feature is aligned.
 * @param $aggregate
 *   Set to '1' if the sequence should only contain sub features, excluding 
 *   intra sub feature sequence.  For example, set this option to obtain just
 *   the coding sequence of an mRNA.
 * @param $output_format
 *   The type of format.  Valid formats include 'fasta_html', 'fasta_txt' and 
 *   'raw'.  The format 'fasta_txt' outputs line
 *   breaks as <br> tags and the entire return value is in a <span> tag
 *   with a fixed-width font definition.  'fasta_txt' outputs line breaks with
 *   windows format carriage returns (e.g. \r\n) with no other formatting. The
 *   raw format is simply the sequence with now FASTA formatting and no
 *   line breaks.
 * @param $upstream
 *   An integer specifing the number of upstream bases to include in the output
 * @param $downstream
 *   An integer specifying the number of downstream bases to include in the 
 *   output.
 *
 * @return
 *   The DNA/protein sequence formated as requested.
 *
 * @ingroup tripal_feature_api
 */
function trpial_feature_get_formatted_sequence($feature_id, $feature_name, 
  $num_bases_per_line, $derive_from_parent, $aggregate, $output_format,
  $upstream, $downstream) {
  
  // to speed things up we need to make sure we have a persistent connection
  tripal_db_persistent_chado(); 
  
  if (!$upstream) {
     $upstream = 0;
  }
  if (!$downstream) {
     $downstream = 0;
  }
  
  // if we need to get the sequence from the parent but there is no aggregation
  // then do so now.
  if ($derive_from_parent) {             
    
    // execute our prepared statement
    if (!tripal_core_is_sql_prepared('sequence_by_parent')) {
      // prepare the queries we're going to use later during the render phase
      // This SQL statement uses conditionals in the select clause to handle
      // cases cases where the alignment is in the reverse direction and when
      // the upstream and downstream extensions go beyond the lenght of the 
      // parent sequence.
      $psql ='PREPARE sequence_by_parent (int, int, int) AS 
              SELECT
                OF.name srcname, FL.srcfeature_id, FL.strand, OCVT.name as srctypename, SCVT.name as typename,
                FL.fmin, FL.fmax,
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmin - $1 <= 0 THEN 0
                       ELSE FL.fmin - $1
                    END
                  WHEN FL.strand < 0 THEN
                    CASE 
                       WHEN FL.fmin - $2 <= 0 THEN 0
                       ELSE FL.fmin - $2
                    END                   
                END as adjfmin,                                
                                
                CASE 
                  WHEN FL.strand >= 0 THEN
                    CASE 
                      WHEN FL.fmax + $2 > OF.seqlen THEN OF.seqlen 
                      ELSE FL.fmax + $2
                    END
                  WHEN FL.strand < 0 THEN
                    CASE
                      WHEN FL.fmax + $1 > OF.seqlen THEN OF.seqlen
                      ELSE FL.fmax + $1   
                    END               
                END as adjfmax,     
                
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmin - $1 <= 0 THEN FL.fmin
                       ELSE $1
                    END
                  ELSE
                    CASE 
                       WHEN FL.fmax + $1 > OF.seqlen THEN OF.seqlen - FL.fmax
                       ELSE $1
                    END                   
                END as upstream,
                
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmax + $2 > OF.seqlen THEN OF.seqlen - FL.fmax
                       ELSE $2
                    END
                  ELSE
                    CASE 
                       WHEN FL.fmin - $2 <= 0 THEN FL.fmin
                       ELSE $2
                    END                   
                END as downstream,            
                              
                CASE 
                  WHEN FL.strand >= 0 THEN 
                    CASE 
                       WHEN FL.fmin - $1 <= 0 THEN substring(OF.residues from 1 for ((FL.fmax - FL.fmin) + $1 + $2)) 
                       ELSE substring(OF.residues from (FL.fmin + 1 - $1) for ((FL.fmax - FL.fmin) + $1 + $2))
                    END
                  WHEN FL.strand < 0 THEN
                    CASE 
                      WHEN FL.fmin - $2 <= 0 THEN substring(OF.residues from 1 for ((FL.fmax - FL.fmin) + $1 + $2)) 
                      ELSE substring(OF.residues from (FL.fmin + 1 - $2) for ((FL.fmax - FL.fmin) + $1 + $2))      
                    END
                END as residues
              FROM featureloc FL 
                INNER JOIN feature SF on FL.feature_id = SF.feature_id
                INNER JOIN cvterm SCVT on SF.type_id = SCVT.cvterm_id
                INNER JOIN feature OF on FL.srcfeature_id = OF.feature_id                
                INNER JOIN cvterm OCVT on OF.type_id = OCVT.cvterm_id
              WHERE SF.feature_id = $3';
              
      $status = chado_query($psql);
      if (!$status) {
        watchdog('tripal_views_handler_field_sequence', 
          "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'sequence_by_parent'), 'WATCHDOG ERROR');
      }
      // this query is meant to get all of the sub features of any given
      // feature (arg #1) and order them as they appear on the reference
      // feature (arg #2).
      $psql ='PREPARE sub_features (int, int) AS 
              SELECT SF.feature_id, CVT.name as type_name, SF.type_id
              FROM feature_relationship FR
                INNER JOIN feature SF on SF.feature_id = FR.subject_id
                INNER JOIN cvterm CVT on CVT.cvterm_id = SF.type_id
                INNER JOIN featureloc FL on FL.feature_id = FR.subject_id
                INNER JOIN feature PF on PF.feature_id = FL.srcfeature_id
              WHERE FR.object_id = $1 and PF.feature_id = $2
              ORDER BY FL.fmin ASC';
              
      $status = chado_query($psql);
      if (!$status) {
        watchdog('tripal_views_handler_field_sequence', 
          "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'ssub_features'), 'WATCHDOG ERROR');
      }
      $psql ='PREPARE count_sub_features (int, int) AS 
              SELECT count(*) as num_children
              FROM feature_relationship FR
                INNER JOIN feature SF on SF.feature_id = FR.subject_id
                INNER JOIN cvterm CVT on CVT.cvterm_id = SF.type_id
                INNER JOIN featureloc FL on FL.feature_id = FR.subject_id
                INNER JOIN feature PF on PF.feature_id = FL.srcfeature_id
              WHERE FR.object_id = $1 and PF.feature_id = $2';
              
      $status = chado_query($psql);
      if (!$status) {
        watchdog('tripal_views_handler_field_sequence', 
          "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'count_sub_features'), 'WATCHDOG ERROR');
      }
    }
    
    // execute the query
    $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";
    $parents = chado_query($sql, $upstream, $downstream, $feature_id);
    
    while ($parent = db_fetch_object($parents)) {  
      $seq = '';  // initialize the sequence for each parent

      // if we are to aggregate then we will ignore the feature returned
      // by the query above and rebuild it using the sub features
      if ($aggregate) {
        
        // now get the sub features that are located on the parent.
        $sql = "EXECUTE sub_features (%d, %d)";
        $children = chado_query($sql, $feature_id, $parent->srcfeature_id);
        $sql = "EXECUTE count_sub_features (%d, %d)";
        $num_children = db_fetch_object(chado_query($sql, $feature_id, $parent->srcfeature_id));
               
        // iterate through the sub features and concat their sequences. They
        // should already be in order.
        $types = array();
        $i = 0;
        while ($child = db_fetch_object($children)) {
          // keep up with the types
          if (!in_array($child->type_name, $types)) {
            $types[] = $child->type_name;
          }
          
          $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";

          // if the first sub feature we need to include the upstream bases
          if ($i == 0 and $parent->strand >= 0) {
            // -------------------------- ref
            //    ....---->  ---->        
            //     up    1       2         
            $q = chado_query($sql, $upstream, 0, $child->feature_id);
          }
          elseif ($i == 0 and $parent->strand < 0) {
            // -------------------------- ref
            //    ....<----  <----
            //    down  1       2
            $q = chado_query($sql, 0, $downstream, $child->feature_id);
          }          
          // if the last sub feature we need to include the downstream bases
          elseif ($i == $num_children->num_children - 1 and $parent->strand >= 0) {
            // -------------------------- ref
            //        ---->  ---->....
            //          1       2 down
            $q = chado_query($sql, 0, $downstream, $child->feature_id);
          }
          elseif ($i == $num_children->num_children - 1 and $parent->strand < 0) {
            // -------------------------- ref
            //        <----  <----....
            //          1       2  up
            $q = chado_query($sql, $upstream, 0, $child->feature_id);
          }
          
          // for internal sub features we don't want upstream or downstream bases
          else {         
            $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";
            $q = chado_query($sql, 0, 0, $child->feature_id);
          }
          
          while ($subseq = db_fetch_object($q)) {
            // concatenate the sequences of all the sub features            
            if ($subseq->srcfeature_id == $parent->srcfeature_id) {
              $seq .= $subseq->residues;   
            }
          }                 
          $i++;
        } 
      } 
      // if this isn't an aggregate then use the parent residues
      else {
         $seq = $parent->residues;
      }
                            
      // get the reverse compliment if feature is on the reverse strand
      $dir = 'forward';
      if ($parent->strand < 0) {
        $seq = trpial_feature_reverse_complement($seq);
        $dir = 'reverse';
      }
      
      // now format for display
      if ($output_format == 'fasta_html') {
         $seq = wordwrap($seq, $num_bases_per_line, "<br>", TRUE);
      } 
      elseif ($output_format == 'fasta_txt') {
         $seq = wordwrap($seq, $num_bases_per_line, "\n", TRUE);
      }
      $residues .= ">$feature_name ($parent->typename) $parent->srcname:" . ($parent->adjfmin + 1) . ".." . $parent->adjfmax ." ($dir). ";
      if (count($types) > 0) {
        $residues .= "Excludes all bases but those of type(s): " . implode(', ', $types) . ". " ;
      }
      if ($parent->upstream > 0) {
         $residues .= "Includes " . $parent->upstream . " bases upstream.  ";
      }
      if ($parent->downstream > 0) {
         $residues .= "Includes " . $parent->downstream . " bases downstream.  ";
      }
      if (!$seq) {
        
        if ($output_format == 'fasta_html') {
          $residues .= "No sequence available.</br>"; 
        } 
        else {
          $residues .= "No sequence available.\n"; 
        }         
      }
      else {
        if ($output_format == 'fasta_html') {
          $residues .= "<br>";
        }
        $residues .= "\n" . $seq . "\n";          
        if ($output_format == 'fasta_html') {
          $residues .= "<br>";
        }
      }
    }
  }
  // if we are not getting the sequence from the parent sequence then
  // use what comes through from the feature record
  else {
    $residues = $values->$field; 
    if ($output_format == 'fasta_html') {
       $residues = wordwrap($residues, $num_bases_per_line, "<br>", TRUE);  
    } 
    elseif ($output_format == 'fasta_txt') {
       $residues = wordwrap($residues, $num_bases_per_line, "\n", TRUE);  
    }
  }
  
  // format the residues for display
  if ($residues and $num_bases_per_line) {
    if ($output_format == 'fasta_html') {
       $residues = '<span style="font-family: monospace;">' . $residues . '</span>';
    }
  }
  return $residues;
}
