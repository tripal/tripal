<?php 


/**
 * @ingroup tripal_feature
 */
function tripal_feature_preprocess_tripal_feature_sequence(&$variables) {
  // we want to provide a new variable that contains the matched features.
  $feature = $variables['node']->feature;

  // get the featureloc src features
  $options = array(
    'return_array' => 1,
    'include_fk' => array(
      'srcfeature_id' => array(
        'type_id' => 1
      ),
    ),
  );

  $feature = chado_expand_var($feature, 'table', 'featureloc', $options);

  // because there are two foriegn keys in the featureloc table with the feature table
  // we have to access the records for each by specifying the field name after the table name:
  $ffeaturelocs = $feature->featureloc->feature_id;

  // now extract the sequences
  $featureloc_sequences = tripal_feature_load_featureloc_sequences($feature->feature_id, $ffeaturelocs);
  $feature->featureloc_sequences = $featureloc_sequences;
}
/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_preprocess_tripal_feature_relationships(&$variables) {
  // we want to provide a new variable that contains the matched features.
  $feature = $variables['node']->feature;

  if (!property_exists($feature, 'all_relationships')) {
    $feature->all_relationships = tripal_feature_get_feature_relationships($feature);
  }
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_preprocess_tripal_feature_proteins(&$variables) {
  // we want to provide a new variable that contains the matched features.
  $feature = $variables['node']->feature;

  if (!property_exists($feature, 'all_relationships')) {
    $feature->all_relationships = tripal_feature_get_feature_relationships($feature);
  }
}
/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_preprocess_tripal_feature_alignments(&$variables) {

  // we want to provide a new variable that contains the matched features.
  $feature = $variables['node']->feature;
  $options = array(
    'return_array' => 1,
    'include_fk' => array(
      'srcfeature_id' => array(
        'type_id' => 1,
      ),
      'feature_id' => array(
        'type_id' => 1
      ),
    )
  );
  $feature = chado_expand_var($feature, 'table', 'featureloc', $options);

  // get alignments as child
  $cfeaturelocs = $feature->featureloc->feature_id;
  if (!$cfeaturelocs) {
    $cfeaturelocs = array();
  }
  // get alignment as parent
  $pfeaturelocs = $feature->featureloc->srcfeature_id;
  if (!$pfeaturelocs) {
    $pfeaturelocs = array();
  }

  // get matched alignments (those with an itermediate 'match' or 'EST_match', etc
  $mfeaturelocs = tripal_feature_get_matched_alignments($feature);
  $feature->matched_featurelocs = $mfeaturelocs;

  // combine all three alignments into a single array for printing together in
  // a single list
  $alignments = array();
  foreach ($pfeaturelocs as $featureloc) {
    // if type is a 'match' then ignore it. We will handle those below
    if (preg_match('/(^match$|^.*?_match|match_part)$/', $featureloc->feature_id->type_id->name)) {
      continue;
    }
    $alignment = new stdClass();
    $alignment->record = $featureloc;
    $alignment->name = $featureloc->feature_id->name;
    $alignment->type = $featureloc->feature_id->type_id->name;
    $alignment->fmin = $featureloc->fmin;
    $alignment->fmax = $featureloc->fmax;
    $alignment->phase = $featureloc->phase;
    $alignment->strand = $featureloc->strand;
    $alignments[] = $alignment;
    if (property_exists($featureloc->feature_id, 'nid')) {
      $alignment->nid = $featureloc->feature_id->nid;
    }
  }
  foreach ($cfeaturelocs as $featureloc) {
    // if type is a 'match' then ignore it. We will handle those below
    if (preg_match('/(^match$|^.*?_match|match_part)$/', $featureloc->feature_id->type_id->name)) {
      continue;
    }
    $alignment = new stdClass();
    $alignment->record = $featureloc;
    $alignment->name = $featureloc->srcfeature_id->name;
    $alignment->type = $featureloc->srcfeature_id->type_id->name;
    $alignment->fmin = $featureloc->fmin;
    $alignment->is_fmin_partial = $featureloc->is_fmin_partial;
    $alignment->fmax = $featureloc->fmax;
    $alignment->is_fmax_partial = $featureloc->is_fmax_partial;
    $alignment->phase = $featureloc->phase;
    $alignment->strand = $featureloc->strand;
    $alignments[] = $alignment;
    if (property_exists($featureloc->srcfeature_id, 'nid')) {
      $alignment->nid = $featureloc->srcfeature_id->nid;
    }
  }
  // in matching features, the left feature is always the feature
  // provided to this function.
  foreach ($mfeaturelocs as $featureloc) {
    // get more information about the right feature
    $select = array('feature_id' => $featureloc->right_srcfeature_id);
    $rfeature = chado_generate_var('feature', $select);
    // now add to the list
    $alignment = new stdClass();
    $alignment->record = $featureloc;
    $alignment->right_feature = $rfeature;
    $alignment->name = $rfeature->name;
    $alignment->type = $rfeature->type_id->name;
    $alignment->fmin = $featureloc->left_fmin;
    $alignment->is_fmin_partial = $featureloc->left_is_fmin_partial;
    $alignment->fmax = $featureloc->left_fmax;
    $alignment->is_fmax_partial = $featureloc->left_is_fmax_partial;
    $alignment->phase = $featureloc->left_phase;
    $alignment->strand = $featureloc->left_strand;
    $alignment->right_fmin = $featureloc->right_fmin;
    $alignment->right_is_fmin_partial = $featureloc->right_is_fmin_partial;
    $alignment->right_fmax = $featureloc->right_fmax;
    $alignment->right_is_fmax_partial = $featureloc->right_is_fmax_partial;
    $alignment->right_phase = $featureloc->right_phase;
    $alignment->right_strand = $featureloc->right_strand;
    $alignments[] = $alignment;
    if (property_exists($rfeature, 'nid')) {
      $alignment->nid = $rfeature->nid;
    }
  }
  $feature->all_featurelocs = $alignments;
}
/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_preprocess_tripal_organism_feature_counts(&$variables, $hook) {
  $organism = $variables['node']->organism;
  $organism->feature_counts = tripal_feature_load_organism_feature_counts($organism);
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
    'order_by' => array('rank' => 'ASC'),
    // we don't want to fully recurse we only need information about the
    // relationship type and the object and subject features (including feature type
    // and organism)
    'include_fk' => array(
      'type_id' => 1,
      'object_id' => array(
        'type_id' => 1,
        'organism_id' => 1
      ),
      'subject_id'  => array(
        'type_id' => 1,
        'organism_id' => 1
      ),
    ),
  );
  $feature = chado_expand_var($feature, 'table', 'feature_relationship', $options);

  // get the subject relationships
  $srelationships = $feature->feature_relationship->subject_id;
  $orelationships = $feature->feature_relationship->object_id;


  // get alignment as child. The $feature->featureloc element
  // is already populated from the alignment preprocess function
  $options = array(
    'include_fk' => array(
      'srcfeature_id' => 1,
      'feature_id' => 1,
    ),
  );
  $feature = chado_expand_var($feature, 'table', 'featureloc', $options);
  $cfeaturelocs = $feature->featureloc->feature_id;
  if (!$cfeaturelocs) {
    $cfeaturelocs = array();
  }
  elseif (!is_array($cfeaturelocs)) {
    $cfeaturelocs = array($cfeaturelocs);
  }

  // prepare the SQL statement to get the featureloc for the
  // feature in the relationships.
  $flrels_sql = "
    SELECT
      FL.featureloc_id, F.name as srcfeature_name, FL.srcfeature_id,
      FL.feature_id, FL.fmin, FL.fmax, FL.strand, FL.phase
    FROM {featureloc} FL
      INNER JOIN {feature} F ON F.feature_id = FL.srcfeature_id
    WHERE FL.feature_id = :feature_id and FL.srcfeature_id = :srcfeature_id
  ";

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
        $res = chado_query($flrels_sql, array(':feature_id' => $relationship->subject_id->feature_id, ':srcfeature_id' => $featureloc->srcfeature_id->feature_id));
        while ($loc = $res->fetchObject()) {
          // add in the node id of the src feature if it exists and save this location
          if (property_exists($featureloc->srcfeature_id, 'nid')) {
            $loc->nid = $featureloc->srcfeature_id->nid;
          }
          $rel->child_featurelocs[] = $loc;
        }
      }
      $rel->record = $relationship;
       
      // get the relationship and child types
      $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
      $child_type = $relationship->subject_id->type_id->name;
       
      // get the node id of the subject
      $sql = "SELECT nid FROM {chado_feature} WHERE feature_id = :feature_id";
      $n = db_query($sql, array(':feature_id' => $relationship->subject_id->feature_id))->fetchObject();
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
        $res = chado_query($flrels_sql, array(':feature_id' => $relationship->object_id->feature_id, ':srcfeature_id' => $featureloc->srcfeature_id->feature_id));
        while ($loc = $res->fetchObject()) {
          // add in the node id of the src feature if it exists and save this location
          if (property_exists($featureloc->srcfeature_id, 'nid')) {
            $loc->nid = $featureloc->srcfeature_id->nid;
          }
          $rel->parent_featurelocs[] = $loc;
        }
      }
      $rel->record = $relationship;
      $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
      $parent_type = $relationship->object_id->type_id->name;
       
      // get the node id of the subject
      $sql = "SELECT nid FROM {chado_feature} WHERE feature_id = :feature_id";
      $n = db_query($sql, array(':feature_id' => $relationship->object_id->feature_id))->fetchObject();
      if ($n) {
        $rel->record->nid = $n->nid;
      }
       
      if (!array_key_exists($rel_type, $relationships['subject'])) {
        $relationships['subject'][$rel_type] = array();
      }
      if (!array_key_exists($parent_type, $relationships['subject'][$rel_type])) {
        $relationships['subject'][$rel_type][$parent_type] = array();
      }
      $relationships['subject'][$rel_type][$parent_type][] = $rel;
    }
  }
  return $relationships;
}