<?php
/**
 * @file
 * Provides an application programming interface (API) for working with features
 *
 * @defgroup tripal_feature_api Feature Module API
 * @ingroup tripal_api
 */

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
  $analysisfeature_id = NULL, $property, $value, $update_if_present = 0, $cv_name = 'tripal') {

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
  $success = tripal_core_insert_property('analysisfeature', $analysisfeature_id,
    $property, $cv_name, $value, $update_if_present);
  if (!$success) {
     watchdog('tripal_feature',
      'tripal_feature_analysis_insert_property: Failed to insert analysis feature property',
       array(), WATCHDOG_WARNING);  
     return FALSE;
  }
  return $success;
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
function tripal_feature_reverse_complement($sequence) {

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
 * @param $sub_features
 *   Only include sub features (or child features) of the types provided in the array 
 * @param $relationship
 *   If a relationship name is provided (e.g. sequence_of) then any sequences that
 *   are in relationships of this type with matched sequences are also included
 * @param $rel_part
 *   If a relationship is provided in the preceeding argument then the rel_part
 *   must be either 'object' or 'subject' to indicate which side of the 
 *   relationship the matched features belong
 *      
 * @return
 *   The DNA/protein sequence formated as requested.
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_get_formatted_sequence($feature_id, $feature_name, 
  $num_bases_per_line, $derive_from_parent, $aggregate, $output_format,
  $upstream, $downstream, $sub_features = array(), $relationship = '', $rel_part = '') {
  
  // to speed things up we need to make sure we have a persistent connection
  $connection = tripal_db_persistent_chado(); 
  
  if (!$upstream) {
     $upstream = 0;
  }
  if (!$downstream) {
     $downstream = 0;
  }
  
  if ($rel_part == "object" or $rel_part == "subject") { 
    if($rel_part == "subject") {
      $psql = '
        PREPARE feature_rel_get_object (int, text) AS
        SELECT FO.feature_id, FO.name, FO.uniquename, CVTO.name as feature_type, O.genus, O.species
        FROM feature FS
          INNER JOIN feature_relationship FR ON FR.subject_id   = FS.feature_id
          INNER JOIN cvterm CVTFR            ON CVTFR.cvterm_id = FR.type_id
          INNER JOIN feature FO              ON FO.feature_id   = FR.object_id
          INNER JOIN cvterm CVTO             ON CVTO.cvterm_id  = FO.type_id
          INNER JOIN organism O              ON O.organism_id   = FO.organism_id
        WHERE 
          FS.feature_id = $1 AND
          CVTFR.name    = $2
      ';  
      $status = tripal_core_chado_prepare('feature_rel_get_object', $psql, array('int', 'text'));
      if (!$status) {
        watchdog('tripal_feature', "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'feature_by_subject'), 'WATCHDOG ERROR');
      }    
      $sql = "EXECUTE feature_rel_get_object(%d,'%s')";
      $features = chado_query($sql, $feature_id, $relationship); 
    }
    if($rel_part == "object") {
      $psql = '
        PREPARE feature_rel_get_subject (int, text) AS
        SELECT FS.feature_id, FS.name, FS.uniquename, CVTO.name as feature_type, O.genus, O.species
        FROM feature FO
          INNER JOIN feature_relationship FR ON FR.object_id    = FO.feature_id
          INNER JOIN cvterm CVTFR            ON CVTFR.cvterm_id = FR.type_id
          INNER JOIN feature FS              ON FS.feature_id   = FR.subject_id
          INNER JOIN cvterm CVTO             ON CVTO.cvterm_id  = FS.type_id
          INNER JOIN organism O              ON O.organism_id   = FS.organism_id
        WHERE 
          FO.feature_id = $1 AND
          CVTFR.name    = $2
      ';
      $status = tripal_core_chado_prepare('feature_rel_get_subject', $psql, array('int', 'text'));
      if (!$status) {
        watchdog('tripal_feature', "init: not able to prepare SQL statement '%name'", 
          array('%name' => 'feature_by_object'), 'WATCHDOG ERROR');
      }
      $sql = "EXECUTE feature_rel_get_subject(%d,'%s')";
      $features = chado_query($sql, $feature_id, $relationship);     
    }
    $sequences = '';
    while ($feature = db_fetch_object($features)) {  

      // recurse and get the sequences for these in the relationship
      if ($rel_part == "subject") {
        $defline = "$feature_name, $relationship, $feature->uniquename $feature->feature_type ($feature->genus $feature->species)";
      }
      if ($rel_part == "object") {
        $defline = "$feature->uniquename $feature->feature_type ($feature->genus $feature->species), $relationship, $feature_name";
      }
      $sequences .= tripal_feature_get_formatted_sequence($feature->feature_id, $defline, 
        $num_bases_per_line, $derive_from_parent, $aggregate, $output_format,
        $upstream, $downstream, $sub_features,'','');  
    }  
    return $sequences;  
  }
  
  // prepare statements we'll need to use later
  if (!tripal_core_is_sql_prepared('sequence_by_parent')) {
    // prepare the queries we're going to use later during the render phase
    // This SQL statement uses conditionals in the select clause to handle
    // cases cases where the alignment is in the reverse direction and when
    // the upstream and downstream extensions go beyond the lenght of the 
    // parent sequence.
    $psql ='
      PREPARE sequence_by_parent (int, int, int) AS 
      SELECT srcname, srcfeature_id, strand, srctypename, typename,
        fmin, fmax, upstream, downstream, adjfmin, adjfmax, 
        substring(residues from (adjfmin + 1) for (upstream + (fmax - fmin) + downstream))  as residues,
        genus, species
      FROM (
        SELECT
          OF.name srcname, FL.srcfeature_id, FL.strand, 
          OCVT.name as srctypename, SCVT.name as typename,
          FL.fmin, FL.fmax, OO.genus, OO.species,
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
          OF.residues                                                     
        FROM {featureloc} FL 
          INNER JOIN {feature} SF on FL.feature_id = SF.feature_id
          INNER JOIN {cvterm} SCVT on SF.type_id = SCVT.cvterm_id
          INNER JOIN {feature} OF on FL.srcfeature_id = OF.feature_id                
          INNER JOIN {cvterm} OCVT on OF.type_id = OCVT.cvterm_id
          INNER JOIN {organism} OO on OF.organism_id = OO.organism_id
        WHERE SF.feature_id = $3 and NOT (OF.residues = \'\' or OF.residues IS NULL)) as tbl1
    ';              
    $status = tripal_core_chado_prepare('sequence_by_parent', $psql, array('int', 'int', 'int'));
    if (!$status) {
      watchdog('tripal_feature', 
        "init: not able to prepare SQL statement '%name'", 
        array('%name' => 'sequence_by_parent'), 'WATCHDOG ERROR');
    }
    
    // this query is meant to get all of the sub features of any given
    // feature (arg #1) and order them as they appear on the reference
    // feature (arg #2).
    $psql ='PREPARE sub_features (int, int) AS 
            SELECT SF.feature_id, CVT.name as type_name, SF.type_id
            FROM {feature_relationship} FR
              INNER JOIN {feature} SF on SF.feature_id = FR.subject_id
              INNER JOIN {cvterm} CVT on CVT.cvterm_id = SF.type_id
              INNER JOIN {featureloc} FL on FL.feature_id = FR.subject_id
              INNER JOIN {feature} PF on PF.feature_id = FL.srcfeature_id
            WHERE FR.object_id = $1 and PF.feature_id = $2
            ORDER BY FL.fmin ASC';            
    $status = tripal_core_chado_prepare('sub_features', $psql, array('int', 'int'));
    if (!$status) {
      watchdog('tripal_views_handler_field_sequence', 
        "init: not able to prepare SQL statement '%name'", 
        array('%name' => 'ssub_features'), 'WATCHDOG ERROR');
    }
    $psql ='PREPARE count_sub_features (int, int) AS 
            SELECT count(*) as num_children
            FROM {feature_relationship} FR
              INNER JOIN {feature} SF on SF.feature_id = FR.subject_id
              INNER JOIN {cvterm} CVT on CVT.cvterm_id = SF.type_id
              INNER JOIN {featureloc} FL on FL.feature_id = FR.subject_id
              INNER JOIN {feature} PF on PF.feature_id = FL.srcfeature_id
            WHERE FR.object_id = $1 and PF.feature_id = $2';            
    $status = tripal_core_chado_prepare('count_sub_features', $psql, array('int', 'int'));
    if (!$status) {
      watchdog('tripal_views_handler_field_sequence', 
        "init: not able to prepare SQL statement '%name'", 
        array('%name' => 'count_sub_features'), 'WATCHDOG ERROR');
    }
  }
    
  // if we need to get the sequence from the parent then do so now.
  if ($derive_from_parent) {                  
    
    // execute the query to get the sequence from the parent
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
          // if the callee has specified that only certain sub features should be
          // included then continue if this child is not one of those allowed
          // subfeatures
          if (count($sub_features) > 0 and !in_array($child->type_name, $sub_features)) {
             continue;
          }
        
          // keep up with the types
          if (!in_array($child->type_name, $types)) {
            $types[] = $child->type_name;
          }
          
          $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";

          // if the first sub feature we need to include the upstream bases. first check if 
          // the feature is in the foward direction or the reverse.
          if ($i == 0 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //    ....---->  ---->        
            //     up    1       2         
            $q = chado_query($sql, $upstream, 0, $child->feature_id);
          }
          elseif ($i == 0 and $parent->strand < 0) { // reverse direction
            // -------------------------- ref
            //    ....<----  <----
            //    down  1       2
            $q = chado_query($sql, 0, $downstream, $child->feature_id);
          }
                    
          // Next, if the last sub feature we need to include the downstream bases. first check if
          // the feature is in teh forward direction or the reverse
          if ($i == $num_children->num_children - 1 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //        ---->  ---->....
            //          1       2 down
            $q = chado_query($sql, 0, $downstream, $child->feature_id);
          }
          elseif ($i == $num_children->num_children - 1 and $parent->strand < 0) { // reverse direction
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
        $seq = tripal_feature_reverse_complement($seq);
        $dir = 'reverse';
      }
      
      // now format for display
      if ($output_format == 'fasta_html') {
         $seq = wordwrap($seq, $num_bases_per_line, "<br>", TRUE);
      } 
      elseif ($output_format == 'fasta_txt') {
         $seq = wordwrap($seq, $num_bases_per_line, "\r\n", TRUE);
      }
      $residues .= ">$feature_name. Sequence derived from feature of type, '$parent->srctypename', of $parent->genus $parent->species: $parent->srcname:" . ($parent->adjfmin + 1) . ".." . $parent->adjfmax ." ($dir). ";
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
          $residues .= "No sequence available.\r\n"; 
        }         
      }
      else {
        if ($output_format == 'fasta_html') {
          $residues .= "<br>";
        }
        $residues .= "\r\n" . $seq . "\r\n";          
        if ($output_format == 'fasta_html') {
          $residues .= "<br>";
        }
      }
    }
  }
  // if we are not getting the sequence from the parent sequence then
  // use what comes through from the feature record
  else {
    $sql = "SELECT * FROM {feature} F WHERE feature_id = %d";
    $values = db_fetch_object(chado_query($sql, $feature_id));
    $residues = $values->residues;
    if ($output_format == 'fasta_html') {
       $residues = wordwrap($residues, $num_bases_per_line, "<br>", TRUE);  
    } 
    elseif ($output_format == 'fasta_txt') {
       $residues = wordwrap($residues, $num_bases_per_line, "\r\n", TRUE);  
    }
    $residues = ">$feature_name\r\n$residues\r\n";
  }
  
  // format the residues for display
  if ($residues and $num_bases_per_line) {
    if ($output_format == 'fasta_html') {
       $residues = '<span style="font-family: monospace;">' . $residues . '</span>';
    }
  }
  return $residues;
}


/**
 * This function defines the custom tables that will be created 
 * in the chado schema.
 *
 * @ingroup tripal_feature
 */
function tripal_feature_get_custom_tables($table = NULL) {

 if (!$table or strcmp($table, 'tripal_gff_temp')==0) {
    $schema['tripal_gff_temp'] = array(
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
  }
  return $schema;
}

/**
 * Using the tripal_core_expand_chado_vars function to retrieve a set
 * of relationships can be very slow, especialy if there are many relationships
 * This function is intended to help speed up the retrieval of relationships
 * by only retrieving the base information for the relationship and returning
 * an array with  
 *
 * @param $feature
 *   The feature object 
 
 * @return
 *   An array with two objects
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_get_feature_relationships($feature) {
  // expand the feature object to include the feature relationships.
  $options = array(
    'return_array' => 1,
    'order_by'     => array('rank' => 'ASC'),
  );
  $feature = tripal_core_expand_chado_vars($feature, 'table',
    'feature_relationship', $options);

  // get the subject relationships
  $srelationships = $feature->feature_relationship->subject_id;
  $orelationships = $feature->feature_relationship->object_id;
  
  // get alignment as child. The $feature->featureloc element
  // is already populated from the alignment preprocess function
  $feature = tripal_core_expand_chado_vars($feature, 'table', 'featureloc');
  $cfeaturelocs = $feature->featureloc->feature_id;
  if (!$cfeaturelocs) {
     $cfeaturelocs = array();
  } 
  elseif (!is_array($cfeaturelocs)) { 
     $cfeaturelocs = array($cfeaturelocs); 
  }
  
  // prepare the SQL statement to get the featureloc for the 
  // feature in the relationships. 
  $connection = tripal_db_persistent_chado();
  $psql = " 
    PREPARE sel_featureloc_preprocess_relationships (int, int) AS 
    SELECT 
      FL.featureloc_id, F.name as srcfeature_name, FL.srcfeature_id,
      FL.feature_id, FL.fmin, FL.fmax, FL.strand, FL.phase
    FROM featureloc FL 
      INNER JOIN feature F ON F.feature_id = FL.srcfeature_id
    WHERE FL.feature_id = $1 and FL.srcfeature_id = $2
  ";
  tripal_core_chado_prepare('sel_featureloc_preprocess_relationships', $psql, array('int', 'int'));

  
  // combine both object and subject relationshisp into a single array
  $relationships = array();
  $relationships['object'] = array();
  $relationships['subject'] = array();
  
  // iterate through the object relationships
  if ($orelationships) {
    foreach ($orelationships as $relationship) {
       $rel = new stdClass(); 
       // get locations where the child feature and this feature overlap with the
       // same landmark feature.
       $rel->child_featurelocs = array();     
       foreach ($cfeaturelocs as $featureloc) {
          $res = chado_query("EXECUTE sel_featureloc_preprocess_relationships (%d, %d)", 
            $relationship->subject_id->feature_id, 
            $featureloc->srcfeature_id->feature_id);        
          while ($loc = db_fetch_object($res)) {
             // add in the node id of the src feature if it exists and save this location
             $loc->nid = $featureloc->srcfeature_id->nid;
             $rel->child_featurelocs[] = $loc;
          }
       }
       $rel->record = $relationship;    
       
       // get the relationship and child types
       $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
       $child_type = $relationship->subject_id->type_id->name;
       
       // get the node id of the subject
       $sql = "SELECT nid FROM {chado_feature} WHERE feature_id = %d";
       $n = db_fetch_object(db_query($sql, $relationship->subject_id->feature_id));
       if ($n) {
          $rel->record->nid = $n->nid;
       }
  
       if (!array_key_exists($rel_type, $relationships['object'])) {
         $relationships['object'][$rel_type] = array();   
       }
       if (!array_key_exists($child_type, $relationships['object'][$rel_type])) {
         $relationships['object'][$rel_type][$child_type] = array();   
       }
       $relationships['object'][$rel_type][$child_type][] = $rel;     
    }
  }
  
  // now add in the subject relationships
  if ($srelationships) {
    foreach ($srelationships as $relationship) {
       $rel = new stdClass(); 
       // get locations where this feature overlaps with the parent
       $rel->parent_featurelocs = array();     
       foreach ($cfeaturelocs as $featureloc) {
          $res = chado_query("EXECUTE sel_featureloc_preprocess_relationships (%d, %d)", 
            $relationship->object_id->feature_id, 
            $featureloc->srcfeature_id->feature_id);
          while ($loc = db_fetch_object($res)) {
             // add in the node id of the src feature if it exists and save this location
             $loc->nid = $featureloc->srcfeature_id->nid;
             $rel->parent_featurelocs[] = $loc;
          }
       }
       $rel->record = $relationship;
       $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
       $parent_type = $relationship->object_id->type_id->name;
       
       // get the node id of the subject
       $sql = "SELECT nid FROM {chado_feature} WHERE feature_id = %d";
       $n = db_fetch_object(db_query($sql, $relationship->object_id->feature_id));
       if ($n) {
          $rel->record->nid = $n->nid;
       }
       
       if (!array_key_exists($rel_type, $relationships['subject'])) {
         $relationships['subject'][$rel_type] = array();   
       }
       if (!array_key_exists($child_type, $relationships['subject'][$rel_type])) {
         $relationships['subject'][$rel_type][$parent_type] = array();   
       }
       $relationships['subject'][$rel_type][$parent_type][] = $rel;
    }
  }
  return $relationships;
} 

/*
 * This function adds an entry to the feature_dbxref table.
  *
 * @param $feature_id
 *   The numeric feature_if of the feature 
 * @param $dbname
 *   The name of the database to which the term belongs
 * @param accession
 *   The accession of the term   
 *    
 * @return
 *   TRUE on success. FALSE on failure.
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_add_dbxref($feature_id, $dbname, $accession) {
   
  // make sure the db exists. If it doesn't, then add it
  $values = array('name' => $dbname);
  $options = array('statement_name' => 'sel_db_na');
  $db = tripal_core_chado_select('db', array('db_id'), $values, $options);
  if (!$db or count($db) == 0) {    
    $options = array('statement_name' => 'ins_db_na');
    $success = tripal_core_chado_insert('db', $values, $options);
      if (!$success) {
      watchdog('tripal_feature', 'tripal_feature_add_dbxref: The feature dbxref entry for feature, %feature_id, ". 
        "could not be added because the database, %dbname, does not exist and cannot be added.', 
        array('%feature_id' => $feature_id, '%dbname' => $dbname), WATCHDOG_WARNING);
      return FALSE;
    }
  }
  
  // first make sure that the record doesn't already exist
  $values = array(
    'dbxref_id' => array(
      'accession' => $accession,
      'db_id' => array(
        'name' => $dbname
      ),
    ),
    'feature_id' => $feature_id,
  );
  $options = array('statement_name' => 'sel_featuredbxref_dbfe');
  $xref = tripal_core_chado_select('feature_dbxref', array('feature_dbxref_id'), $values, $options);
  if (count($xref) == 0) {    
    // if the record doesn't exist then add it.
    $options = array('statement_name' => 'ins_featuredbxref_dbfe');
    $success = tripal_core_chado_insert('feature_dbxref', $values, $options);
    if (!$success) {
      watchdog('tripal_feature', 'tripal_feature_add_dbxref: The feature dbxref entry for feature, %feature_id, '. 
        'could not be added: %db:%accession.', array('%feature_id' => $feature_id, '%db' => $dbname, 
        '%accession' => $accession), WATCHDOG_WARNING);
      return FALSE;
    }
  }
  return TRUE;
}

/*
 * This function adds an entry to the feature_cvterm table.
  *
 * @param $feature_id
 *   The numeric feature_if of the feature 
 * @param $cvname
 *   The name of the controlled vocabulary to which the term belongs
 * @param cvterm
 *   The name of the cvterm  
 *    
 * @return
 *   TRUE on success. FALSE on failure.
 *
 * @ingroup tripal_feature_api
 */
function tripal_feature_add_cvterm($feature_id, $cvname, $cvterm) {
   
  // make sure the cv exists. If it doesn't, then add it
  $values = array('name' => $cvname);
  $options = array('statement_name' => 'sel_cv_na');
  $cv = tripal_core_chado_select('cv', array('cv_id'), $values, $options);
  if (!$cv or count($cv) == 0) {    
    $options = array('statement_name' => 'ins_cv_na');
    $success = tripal_core_chado_insert('cv', $values, $options);
      if (!$success) {
      watchdog('tripal_feature', 'tripal_feature_add_cvterm: The feature cvterm entry for feature, %feature_id, ". 
        "could not be added because the CV, %cvname, does not exist and cannot be added.', 
        array('%feature_id' => $feature_id, '%cvname' => $cvname), WATCHDOG_WARNING);
      return FALSE;
    }
  }
  
  // first make sure that the record doesn't already exist
  $values = array(
    'cvterm_id' => array(
      'name' => $cvterm,
      'cv_id' => array(
        'name' => $cvname
      ),
    ),
    'feature_id' => $feature_id,
    'pub_id' => 1,
  );
  $options = array('statement_name' => 'sel_featuredcvterm_cvfepu');
  $xref = tripal_core_chado_select('feature_cvterm', array('feature_cvterm_id'), $values, $options);
  if (count($xref) == 0) {    
    // if the record doesn't exist then add it.
    $options = array('statement_name' => 'ins_featurecvterm_cvfepu');
    $success = tripal_core_chado_insert('feature_cvterm', $values, $options);
    if (!$success) {
      watchdog('tripal_feature', 'tripal_feature_add_cvterm: The feature cvterm entry for feature, %feature_id, '. 
        'could not be added: %cvterm.', array('%feature_id' => $feature_id, '%cvterm' => $cvterm), WATCHDOG_WARNING);
      return FALSE;
    }
  }
  return TRUE;
}
