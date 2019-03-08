<?php


/**
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_preprocess_tripal_feature_sequence(&$variables) {
  // we want to provide a new variable that contains the matched features.
  $feature = $variables['node']->feature;

  // get the featureloc src features
  $options = [
    'return_array' => 1,
    'include_fk' => [
      'srcfeature_id' => [
        'type_id' => 1,
      ],
    ],
  ];

  $feature = chado_expand_var($feature, 'table', 'featureloc', $options);

  // because there are two foriegn keys in the featureloc table with the feature table
  // we have to access the records for each by specifying the field name after the table name:
  $ffeaturelocs = $feature->featureloc->feature_id;

  // now extract the sequences
  $featureloc_sequences = tripal_feature_load_featureloc_sequences($feature->feature_id, $ffeaturelocs);
  $feature->featureloc_sequences = $featureloc_sequences;

  // if this feature has associated protein sequences (or others via relationships
  // then we want to make sure the relationships are added so that we can
  // show the protein sequences
  if (!property_exists($feature, 'all_relationships')) {
    $feature->all_relationships = tripal_feature_get_feature_relationships($feature);
  }
}

/**
 * Get the sequence this feature is located on
 *
 * @param $feature_id
 * @param $featurelocs
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_load_featureloc_sequences($feature_id, $featurelocs) {

  // if we don't have any featurelocs then no point in continuing
  if (!$featurelocs) {
    return [];
  }

  // get the list of relationships (including any aggregators) and iterate
  // through each one to find information needed to color-code the reference sequence
  $relationships = tripal_feature_get_aggregate_relationships($feature_id);
  if (!$relationships) {
    return [];
  }


  // iterate through each of the realtionships features and get their
  // locations
  foreach ($relationships as $rindex => $rel) {
    // get the featurelocs for each of the relationship features
    $rel_featurelocs = tripal_feature_load_featurelocs($rel->subject_id, 'as_child', 0);
    foreach ($rel_featurelocs as $rfindex => $rel_featureloc) {
      // keep track of this unique source feature
      $src = $rel_featureloc->src_feature_id . "-" . $rel_featureloc->src_cvterm_id;

      // copy over the results to the relationship object.  Since there can
      // be more than one feature location for each relationship feature we
      // use the '$src' variable to keep track of these.
      $rel->featurelocs = new stdClass();
      $rel->featurelocs->$src = new stdClass();
      $rel->featurelocs->$src->src_uniquename = $rel_featureloc->src_uniquename;
      $rel->featurelocs->$src->src_cvterm_id = $rel_featureloc->src_cvterm_id;
      $rel->featurelocs->$src->src_cvname = $rel_featureloc->src_cvname;
      $rel->featurelocs->$src->fmin = $rel_featureloc->fmin;
      $rel->featurelocs->$src->fmax = $rel_featureloc->fmax;
      $rel->featurelocs->$src->src_name = $rel_featureloc->src_name;

      // keep track of the individual parts for each relationship
      $start = $rel->featurelocs->$src->fmin;
      $end = $rel->featurelocs->$src->fmax;
      $type = $rel->subject_type;
      $rel_locs[$src]['parts'][$start][$type]['start'] = $start;
      $rel_locs[$src]['parts'][$start][$type]['end'] = $end;
      $rel_locs[$src]['parts'][$start][$type]['type'] = $type;
    }
  }

  // the featurelocs array provided to the function contains the locations
  // where this feature is found.   We want to get the sequence for each
  // location and then annotate it with the parts found from the relationships
  // locations determiend above.
  $floc_sequences = [];
  foreach ($featurelocs as $featureloc) {

    // build the src name so we can keep track of the different parts for each feature
    $src = $featureloc->srcfeature_id->feature_id . "-" . $featureloc->srcfeature_id->type_id->cvterm_id;

    // orient the parts to the beginning of the feature sequence
    if (!empty($rel_locs[$src]['parts'])) {
      $parts = $rel_locs[$src]['parts'];
      $rparts = [];  // we will fill this up if we're on the reverse strand

      foreach ($parts as $start => $types) {
        foreach ($types as $type_name => $type) {
          if ($featureloc->strand >= 0) {
            // this is on the forward strand.  We need to convert the start on the src feature to the
            // start on this feature's sequence
            $parts[$start][$type_name]['start'] = $parts[$start][$type_name]['start'] - $featureloc->fmin;
            $parts[$start][$type_name]['end'] = $parts[$start][$type_name]['end'] - $featureloc->fmin;
            $parts[$start][$type_name]['type'] = $type_name;
          }
          else {
            // this is on the reverse strand.  We need to swap the start and stop and calculate from the
            // begining of the reverse sequence
            $size = ($featureloc->fmax - $featureloc->fmin);
            $start_orig = $parts[$start][$type_name]['start'];
            $end_orig = $parts[$start][$type_name]['end'];
            $new_start = $size - ($end_orig - $featureloc->fmin);
            $new_end = $size - ($start_orig - $featureloc->fmin);

            $rparts[$new_start][$type_name]['start'] = $new_start;
            $rparts[$new_start][$type_name]['end'] = $new_end;
            $rparts[$new_start][$type_name]['type'] = $type_name;
          }
        }
      }

      // now sort the parts
      // if we're on the reverse strand we need to resort
      if ($featureloc->strand >= 0) {
        usort($parts, 'tripal_feature_sort_rel_parts_by_start');
      }
      else {
        usort($rparts, 'tripal_feature_sort_rel_parts_by_start');
        $parts = $rparts;
      }

      $floc_sequences[$src]['id'] = $src;
      $floc_sequences[$src]['type'] = $featureloc->feature_id->type_id->name;
      $args = [':feature_id' => $featureloc->srcfeature_id->feature_id];
      $start = $featureloc->fmin + 1;
      $size = $featureloc->fmax - $featureloc->fmin;

      // TODO: fix the hard coded $start and $size
      // the $start and $size variables are hard-coded in the SQL statement
      // because the db_query function places quotes around all placeholders
      // (e.g. :start & :size) and screws up the substring function
      $sql = "
        SELECT substring(residues from $start for $size) as residues
        FROM {feature}
        WHERE feature_id = :feature_id
      ";
      $sequence = chado_query($sql, $args)->fetchObject();
      $residues = $sequence->residues;
      if ($featureloc->strand < 0) {
        $residues = tripal_reverse_compliment_sequence($residues);
      }
      $strand = '.';
      if ($featureloc->strand == 1) {
        $strand = '+';
      }
      elseif ($featureloc->strand == -1) {
        $strand = '-';
      }
      $floc_sequences[$src]['location'] = tripal_get_location_string($featureloc);
      $floc_sequences[$src]['defline'] = tripal_get_fasta_defline($featureloc->feature_id, '', $featureloc, '', strlen($residues));
      $floc_sequences[$src]['featureloc'] = $featureloc;
      $floc_sequences[$src]['formatted_seq'] = tripal_feature_color_sequence($residues, $parts, $floc_sequences[$src]['defline']);
    }
  }
  return $floc_sequences;
}

/**
 * Used to sort the list of relationship parts by start position
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_sort_rel_parts_by_start($a, $b) {
  foreach ($a as $type_name => $details) {
    $astart = $a[$type_name]['start'];
    break;
  }
  foreach ($b as $type_name => $details) {
    $bstart = $b[$type_name]['start'];
    break;
  }
  return strnatcmp($astart, $bstart);
}

/**
 * Load the locations for a given feature
 *
 * @param $feature_id
 *   The feature to look up locations for
 * @param $side
 *   Whether the feature is the scrfeature, 'as_parent', or feature, 'as_child'
 * @param $aggregate
 *   Whether or not to get the locations for related features
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_load_featurelocs($feature_id, $side = 'as_parent', $aggregate = 1) {

  $sql = "
    SELECT
       F.name, F.feature_id, F.uniquename,
       FS.name as src_name, FS.feature_id as src_feature_id, FS.uniquename as src_uniquename,
       CVT.name as cvname, CVT.cvterm_id,
       CVTS.name as src_cvname, CVTS.cvterm_id as src_cvterm_id,
       FL.fmin, FL.fmax, FL.is_fmin_partial, FL.is_fmax_partial,FL.strand, FL.phase
     FROM {featureloc} FL
       INNER JOIN {feature} F   ON FL.feature_id = F.feature_id
       INNER JOIN {feature} FS  ON FS.feature_id = FL.srcfeature_id
       INNER JOIN {cvterm} CVT  ON F.type_id     = CVT.cvterm_id
       INNER JOIN {cvterm} CVTS ON FS.type_id    = CVTS.cvterm_id
   ";
  if (strcmp($side, 'as_parent') == 0) {
    $sql .= "WHERE FL.srcfeature_id = :feature_id ";
  }
  if (strcmp($side, 'as_child') == 0) {
    $sql .= "WHERE FL.feature_id = :feature_id ";
  }

  $flresults = chado_query($sql, [':feature_id' => $feature_id]);

  // copy the results into an array
  $i = 0;
  $featurelocs = [];
  while ($loc = $flresults->fetchObject()) {
    // if a drupal node exists for this feature then add the nid to the
    // results object
    $loc->fnid = chado_get_nid_from_id('feature', $loc->feature_id);
    $loc->snid = chado_get_nid_from_id('feature', $loc->src_feature_id);
    // add the result to the array
    $featurelocs[$i++] = $loc;
  }

  // Add the relationship feature locs if aggregate is turned on
  if ($aggregate and strcmp($side, 'as_parent') == 0) {
    // get the relationships for this feature without substituting any children
    // for the parent. We want all relationships
    $relationships = tripal_feature_get_aggregate_relationships($feature_id, 0);
    foreach ($relationships as $rindex => $rel) {
      // get the featurelocs for each of the relationship features
      $rel_featurelocs = tripal_feature_load_featurelocs($rel->subject_id, 'as_child', 0);
      foreach ($rel_featurelocs as $findex => $rfloc) {
        $featurelocs[$i++] = $rfloc;
      }
    }
  }

  usort($featurelocs, 'tripal_feature_sort_locations');
  return $featurelocs;
}

/**
 * Used to sort the feature locs by start position
 *
 * @param $a
 *   One featureloc record (as an object)
 * @param $b
 *   The other featureloc record (as an object)
 *
 * @return
 *   Which feature location comes first
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_sort_locations($a, $b) {
  return strnatcmp($a->fmin, $b->fmin);
}

/**
 * Returns the marked up fasta sequence for the described feature
 *
 * @param $sequence
 * @param $parts
 * @param $defline
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_color_sequence($sequence, $parts, $defline) {


  $types = [];
  // first get the list of types so we can create a color legend
  foreach ($parts as $index => $t) {
    foreach ($t as $type_name => $details) {
      $types[$type_name] = 1;
    }
  }

  $newseq = "<div id=\"tripal_feature-featureloc_sequence-legend\">Legend: ";
  foreach ($types as $type_name => $present) {
    $newseq .= "<span id=\"tripal_feature-legend-$type_name\" class=\"tripal_feature-legend-item tripal_feature-featureloc_sequence-$type_name\" script=\"\">$type_name</span>";
  }
  $newseq .= "</div>Hold the cursor over a type above to highlight its positions in the sequence below.";


  // set the background color of the rows based on the type
  $pos = 0;
  $newseq .= "<pre class=\"tripal_feature-sequence\">";
  $newseq .= ">$defline<br>";

  // iterate through the parts. They should be in order.
  $starts = []; // an array holding all of the children starting locations
  $ends = []; // an array holding all of the children's ending locations
  $seqcount = 0;
  foreach ($parts as $index => $types) {

    // get the start for this part.  All types in this part start at the
    // same position so we only need the first record
    foreach ($types as $type => $child) {
      $start = $child['start'];
      $starts[$start][] = $type;
    }

    // next, sort the parts by their end. We want the span tag to
    // to be added in the order the parts end.
    usort($types, 'tripal_feature_sort_rel_parts_by_end');

    // iterate through the types in order that then end and create a
    // span for it.
    foreach ($types as $type) {
      $end = $type['end'];
      $ends[$end][] = $type;
    }
  }

  // iterate through each nucleotide in the sequence, add a new line very
  // 50 characters and add the spans as we encounter them
  for ($i = 0; $i < strlen($sequence); $i++) {

    // if we are at and end of a span then close it
    if (array_key_exists($i, $ends)) {
      foreach ($ends[$i] as $index => $type) {
        $newseq .= "</span>";
      }
    }

    // if we are at and end of a span then close it
    if (array_key_exists($i, $starts)) {
      foreach ($starts[$i] as $index => $type) {
        $class = "tripal_feature-featureloc_sequence-" . $type;
        $newseq .= "<span class=\"$class\">";
      }
    }

    $newseq .= $sequence{$i};
    $seqcount++;
    if ($seqcount % 50 == 0) {
      $newseq .= "\n";
    }
  }

  $newseq .= "</pre>";
  return $newseq;
}

/**
 * Used to sort the list of relationship parts by start position
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_sort_rel_parts_by_end($a, $b) {
  $val = strnatcmp($b['end'], $a['end']);
  if ($val == 0) {
    return strcmp($a['type'], $b['type']);
  }
  return $val;
}

/**
 *
 *
 * @ingroup tripal_legacy_feature
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
 * @ingroup tripal_legacy_feature
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
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_preprocess_tripal_feature_alignments(&$variables) {

  // we want to provide a new variable that contains the matched features.
  $feature = $variables['node']->feature;
  $options = [
    'return_array' => 1,
    'include_fk' => [
      'srcfeature_id' => [
        'type_id' => 1,
      ],
      'feature_id' => [
        'type_id' => 1,
      ],
    ],
  ];
  $feature = chado_expand_var($feature, 'table', 'featureloc', $options);

  // get alignments as child
  $cfeaturelocs = $feature->featureloc->feature_id;
  if (!$cfeaturelocs) {
    $cfeaturelocs = [];
  }
  // get alignment as parent
  $pfeaturelocs = $feature->featureloc->srcfeature_id;
  if (!$pfeaturelocs) {
    $pfeaturelocs = [];
  }

  // get matched alignments (those with an itermediate 'match' or 'EST_match', etc
  $mfeaturelocs = tripal_feature_get_matched_alignments($feature);
  $feature->matched_featurelocs = $mfeaturelocs;

  // combine all three alignments into a single array for printing together in
  // a single list
  $alignments = [];
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
    $select = ['feature_id' => $featureloc->right_srcfeature_id];
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
 * This function is for features that align through an intermediate such
 * as 'EST_match' or 'match'.  This occurs in the case where two sequences
 * align but where one does not align perfectly.  Some ESTs may be in a contig
 * but not all of the EST.  Portions may overhang and not be included in the
 * consensus if quality is bad.
 * For example:
 *    Feature 1: Contig --------------------
 *    Feature 2: EST_match           -------
 *    Feature 3: EST                 ---------
 *
 * The feature provided to the function will always be the feature 1.  The
 * featureloc columns prefixed with 'right' (e.g. right_fmin) belong to the
 * alignment of feature 3 with feature 2
 *
 * Features may align to more than one feature and are not matches. We do
 * not want to include these, so we have to filter on the SO terms:
 * match, or %_match
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_get_matched_alignments($feature) {

  $sql = "
     SELECT
       FL1.featureloc_id    as left_featureloc_id,
       FL1.srcfeature_id    as left_srcfeature_id,
       FL1.feature_id       as left_feature_id,
       FL1.fmin             as left_fmin,
       FL1.is_fmin_partial  as left_is_fmin_partial,
       FL1.fmax             as left_fmax,
       FL1.is_fmax_partial  as left_is_fmax_partial,
       FL1.strand           as left_strand,
       FL1.phase            as left_phase,
       FL1.locgroup         as left_locgroup,
       FL1.rank             as left_rank,
       FL2.featureloc_id    as right_featureloc_id,
       FL2.srcfeature_id    as right_srcfeature_id,
       FL2.feature_id       as right_feature_id,
       FL2.fmin             as right_fmin,
       FL2.is_fmin_partial  as right_is_fmin_partial,
       FL2.fmax             as right_fmax,
       FL2.is_fmax_partial  as right_is_fmax_partial,
       FL2.strand           as right_strand,
       FL2.phase            as right_phase,
       FL2.locgroup         as right_locgroup,
       FL2.rank             as right_rank
     FROM {feature} F1
       INNER JOIN {featureloc} FL1 on FL1.srcfeature_id = F1.feature_id
       INNER JOIN {feature} F2 on FL1.feature_id = F2.feature_id
       INNER JOIN {featureloc} FL2 on FL2.feature_id = F2.feature_id
       INNER JOIN {cvterm} CVT2 on F2.type_id = CVT2.cvterm_id
     WHERE
       F1.feature_id = :feature_id  AND
       (CVT2.name = 'match' or CVT2.name like '%_match')
     ORDER BY FL1.fmin
   ";

  $results = chado_query($sql, [':feature_id' => $feature->feature_id]);

  // iterate through the results and add them to our featurelocs array
  $featurelocs = [];
  while ($fl = $results->fetchObject()) {
    // ignore featurelocs where the left and right srcfeature is the same
    if (strcmp($fl->left_srcfeature_id, $fl->right_srcfeature_id) == 0) {
      continue;
    }
    $featurelocs[] = $fl;
  }
  return $featurelocs;
}

/**
 *
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_preprocess_tripal_organism_feature_counts(&$variables, $hook) {
  $organism = $variables['node']->organism;
  $organism->feature_counts = tripal_feature_load_organism_feature_counts($organism);
}

/**
 * Load the arguments for the organism feature counts browser
 *
 * @param $organism
 *  The organism of interest
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_load_organism_feature_counts($organism) {

  $args = [];
  $order = [];
  $names = [];

  // We should not assume this table is present since it is a materialized view.
  if (!chado_table_exists('organism_feature_count')) {
    return NULL;
  }

  // build the where clause for the SQL statement if we have a custom term list
  // we'll also keep track of the names the admin provided (if any) and the
  // order that the terms should appear.
  $is_custom = 0;
  $temp = rtrim(variable_get('tripal_feature_summary_report_mapping', ''));
  $where = '';
  if ($temp) {
    $is_custom = 1;
    $temp = explode("\n", $temp);
    $i = 0;
    foreach ($temp as $value) {
      // separate the key value pairs
      $temp2 = explode("=", $value);
      $feature_type = rtrim($temp2[0]);
      $order[] = $feature_type;  // save the order of the these terms
      $where .= " OFC.feature_type = :name$i OR ";
      $args[":name$i"] = rtrim($temp2[0]);

      // if the admin specified a new name then store that otherwise use the
      // the default sequence ontology term name
      if (count($temp2) == 2) {
        $names[] = rtrim($temp2[1]);
      }
      else {
        $names[] = $feature_type;
      }
      $i++;
    }
    if ($where) {
      $where = drupal_substr($where, 0, -4);  # remove OR from the end
      $where = "($where) AND";
    }
  }

  // get the feature counts.  This is dependent on a materialized view
  // installed with the organism module
  $sql = "
    SELECT OFC.num_features,OFC.feature_type,CVT.definition
    FROM {organism_feature_count} OFC
      INNER JOIN {cvterm} CVT on OFC.cvterm_id = CVT.cvterm_id
    WHERE $where organism_id = :organism_id
    ORDER BY num_features desc
  ";
  $args[':organism_id'] = $organism->organism_id;
  $org_features = chado_query($sql, $args);

  // iterate through the types
  $types = [];
  while ($type = $org_features->fetchObject()) {
    $types[$type->feature_type] = $type;
    // if we don't have an order this means we didn't go through the loop
    // above to set the names, so do that now
    if (!$is_custom) {
      $names[] = $type->feature_type;
      $order[] = $type->feature_type;
    }
  }

  // now reorder the types
  $ordered_types = [];
  foreach ($order as $type) {
    if (array_key_exists($type, $types)) {
      $ordered_types[] = $types[$type];
    }
  }
  return [
    'types' => $ordered_types,
    'names' => $names,
  ];
}

/**
 * Using the chado_expand_var function to retrieve a set
 * of relationships can be very slow, especialy if there are many relationships
 * This function is intended to help speed up the retrieval of relationships
 * by only retrieving the base information for the relationship and returning
 * an array with
 *
 * @param $feature
 *   The feature object
 *
 * @return
 *   An array with two objects
 *
 * @ingroup tripal_legacy_feature_api
 */
function tripal_feature_get_feature_relationships($feature) {
  // expand the feature object to include the feature relationships.
  $options = [
    'return_array' => 1,
    'order_by' => ['rank' => 'ASC'],
    // we don't want to fully recurse we only need information about the
    // relationship type and the object and subject features (including feature type
    // and organism)
    'include_fk' => [
      'type_id' => 1,
      'object_id' => [
        'type_id' => 1,
        'organism_id' => 1,
      ],
      'subject_id' => [
        'type_id' => 1,
        'organism_id' => 1,
      ],
    ],
  ];
  $feature = chado_expand_var($feature, 'table', 'feature_relationship', $options);

  // get the subject relationships
  $srelationships = $feature->feature_relationship->subject_id;
  $orelationships = $feature->feature_relationship->object_id;


  // get alignment as child. The $feature->featureloc element
  // is already populated from the alignment preprocess function
  $options = [
    'include_fk' => [
      'srcfeature_id' => 1,
      'feature_id' => 1,
    ],
  ];
  $feature = chado_expand_var($feature, 'table', 'featureloc', $options);
  $cfeaturelocs = $feature->featureloc->feature_id;
  if (!$cfeaturelocs) {
    $cfeaturelocs = [];
  }
  elseif (!is_array($cfeaturelocs)) {
    $cfeaturelocs = [$cfeaturelocs];
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
  $relationships = [];
  $relationships['object'] = [];
  $relationships['subject'] = [];

  // iterate through the object relationships
  if ($orelationships) {
    foreach ($orelationships as $relationship) {
      $rel = new stdClass();
      // get locations where the child feature and this feature overlap with the
      // same landmark feature.
      $rel->child_featurelocs = [];
      foreach ($cfeaturelocs as $featureloc) {
        $res = chado_query($flrels_sql, [
          ':feature_id' => $relationship->subject_id->feature_id,
          ':srcfeature_id' => $featureloc->srcfeature_id->feature_id,
        ]);
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
      $n = db_query($sql, [':feature_id' => $relationship->subject_id->feature_id])->fetchObject();
      if ($n) {
        $rel->record->nid = $n->nid;
      }

      if (!array_key_exists($rel_type, $relationships['object'])) {
        $relationships['object'][$rel_type] = [];
      }
      if (!array_key_exists($child_type, $relationships['object'][$rel_type])) {
        $relationships['object'][$rel_type][$child_type] = [];
      }
      $relationships['object'][$rel_type][$child_type][] = $rel;
    }
  }

  // now add in the subject relationships
  if ($srelationships) {
    foreach ($srelationships as $relationship) {
      $rel = new stdClass();
      // get locations where this feature overlaps with the parent
      $rel->parent_featurelocs = [];
      foreach ($cfeaturelocs as $featureloc) {
        $res = chado_query($flrels_sql, [
          ':feature_id' => $relationship->object_id->feature_id,
          ':srcfeature_id' => $featureloc->srcfeature_id->feature_id,
        ]);
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
      $n = db_query($sql, [':feature_id' => $relationship->object_id->feature_id])->fetchObject();
      if ($n) {
        $rel->record->nid = $n->nid;
      }

      if (!array_key_exists($rel_type, $relationships['subject'])) {
        $relationships['subject'][$rel_type] = [];
      }
      if (!array_key_exists($parent_type, $relationships['subject'][$rel_type])) {
        $relationships['subject'][$rel_type][$parent_type] = [];
      }
      $relationships['subject'][$rel_type][$parent_type][] = $rel;
    }
  }
  return $relationships;
}

/**
 *
 */
function tripal_feature_preprocess_tripal_feature_bar_chart_type_organism_summary(&$vars) {

  // Add in all the javascript/css files.
  tripal_add_d3js();
  drupal_add_css(drupal_get_path('module', 'tripal_feature') . '/theme/css/tripal_feature.css');
  drupal_add_js(drupal_get_path('module', 'tripal_feature') . '/theme/js/tripalFeature.adminChart.js');

  // Retrieve and process all the data and save it as javascript settings.
  //'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

  // We are using the organism_feature_count materialized view as the source for our data.
  // Thus grab all the records from this materialized view.
  $organism_feature_count = chado_select_record(
    'organism_feature_count',
    ['*'],
    [],
    [
      'order_by' => [
        'genus' => 'ASC',
        'species' => 'ASC',
        'feature_type' => 'ASC',
        'num_features' => 'DESC',
      ],
    ]
  );

  // Initialize variables.
  $chart = [];
  $type_names = [];
  $organism_names = [];
  $max_bar_height = 0;

  // Process each row of the materialzied view into the chart array.
  // Note: it's first keyed by type since each type will be a bar. Each type will have
  // a "bars" array with the start (y0) and end (y1) height on the bar for a given
  // organism. Finally we keep a record of the names of the types & organisms
  // for axis' and legend generation respectively.
  foreach ($organism_feature_count as $row) {

    // Build up the easy details for the current row's type. These will be overridden
    // multiple times but that's more efficient than checking each time.
    $chart[$row->cvterm_id]['cvterm_id'] = $row->cvterm_id;
    $chart[$row->cvterm_id]['name'] = str_replace('_', ' ', $row->feature_type);

    // Save the name of the type and organism into their respective arrays
    // for generation of axis' and legends for the chart.
    $type_names[$row->cvterm_id] = $row->feature_type;
    $organism_names[$row->organism_id] = $row->genus . ' ' . $row->species;

    // Save information about the current organism. This isn't actually used by the
    // chart but can be used to debug the bar generation to follow.
    $chart[$row->cvterm_id]['organisms'][] = [
      'name' => $row->genus . ' ' . $row->species,
      'value' => (int) $row->num_features,
    ];

    // Now to build the bar array with the start (y0) and end (y1) height on the
    // bar for a given organism.
    // NOTE: we cannot assume the types are all in order so store y0 & y1 in the
    // $chart[type] array.
    // If y0 has not yet been set for this type then we're starting with the first
    // chunk (organism) on the bar.
    if (!isset($chart[$row->cvterm_id]['y0'])) {
      $chart[$row->cvterm_id]['y0'] = 0;
      $chart[$row->cvterm_id]['y1'] = $row->num_features;
    }
    // Otherwise, add the next chunk (organism) on top of the pre-existing bar.
    else {
      $chart[$row->cvterm_id]['y0'] = $chart[$row->cvterm_id]['y1'];
      $chart[$row->cvterm_id]['y1'] = $chart[$row->cvterm_id]['y0'] + $row->num_features;
    }
    // Now save the bar chunk we just determined.
    $chart[$row->cvterm_id]['bars'][] = [
      'name' => $row->genus . ' ' . $row->species,
      'y0' => $chart[$row->cvterm_id]['y0'],
      'y1' => $chart[$row->cvterm_id]['y1'],
    ];

    // We also need to keep track of the total number of features for a single bar (Type).
    $chart[$row->cvterm_id]['total_features'] = (int) $chart[$row->cvterm_id]['y1'];
    // And the maximum "height" for all bars.
    if ($max_bar_height < $chart[$row->cvterm_id]['total_features']) {
      $max_bar_height = (int) $chart[$row->cvterm_id]['total_features'];
    }
  }

  // Sort based on the total number of features.
  // NOTE: This changes the keys so it's no longer the organism/type_id.
  usort($chart, 'tripal_feature_admin_summary_sort');
  sort($type_names);
  sort($organism_names);

  // We also need to add information about the materialized views
  // so that admin can update it and know how recent the data is.
  $mview = db_query('
    SELECT mview_id, name, last_update
    FROM tripal_mviews
    WHERE mv_table=:mv_table',
    [':mv_table' => 'organism_feature_count']
  )->fetchObject();

  $vars['chart_details'] = [
    'summary' => $chart,
    'types' => $type_names,
    'organisms' => $organism_names,
    'legendPosition' => 'top',
    'maxBarHeight' => $max_bar_height,
    'mviewUrl' => url('admin/tripal/storage/chado/mviews/update/' . $mview->mview_id),
    'mviewTable' => $mview->name,
    'mviewLastUpdate' => $mview->last_update ? format_date($mview->last_update) : '',
  ];

  // Save everything we just determined as a Drupal JS settings so that we have access to
  // it in our js script.
  drupal_add_js(['tripalFeature' => ['admin' => $vars['chart_details']]], 'setting');
}


/**
 * Get features related to the current feature to a given depth. Recursive
 * function.
 *
 * @param $feature_id
 * @param $substitute
 * @param $levels
 * @param $base_type_id
 * @param $depth
 *
 */
function tripal_feature_get_aggregate_relationships($feature_id, $substitute = 1,
                                                    $levels = 0, $base_type_id = NULL, $depth = 0) {

  // we only want to recurse to as many levels deep as indicated by the
  // $levels variable, but only if this variable is > 0. If 0 then we
  // recurse until we reach the end of the relationships tree.
  if ($levels > 0 and $levels == $depth) {
    return NULL;
  }

  // first get the relationships for this feature
  return tripal_feature_load_relationships($feature_id, 'as_object');

}

/**
 * Get the relationships for a feature.
 *
 * @param $feature_id
 *   The feature to get relationships for
 * @param $side
 *   The side of the relationship this feature is (ie: 'as_subject' or
 *   'as_object')
 *
 *
 */
function tripal_feature_load_relationships($feature_id, $side = 'as_subject') {
  // Get the relationships for this feature.  The query below is used for both
  // querying the object and subject relationships
  $sql = "
    SELECT
      FS.name as subject_name, FS.uniquename as subject_uniquename,
      CVTS.name as subject_type, CVTS.cvterm_id as subject_type_id,
      FR.subject_id, FR.type_id as relationship_type_id, FR.object_id, FR.rank,
      CVT.name as rel_type,
      FO.name as object_name, FO.uniquename as object_uniquename,
      CVTO.name as object_type, CVTO.cvterm_id as object_type_id
    FROM {feature_relationship} FR
     INNER JOIN {cvterm} CVT  ON FR.type_id    = CVT.cvterm_id
     INNER JOIN {feature} FS  ON FS.feature_id = FR.subject_id
     INNER JOIN {feature} FO  ON FO.feature_id = FR.object_id
     INNER JOIN {cvterm} CVTO ON FO.type_id    = CVTO.cvterm_id
     INNER JOIN {cvterm} CVTS ON FS.type_id    = CVTS.cvterm_id
  ";
  if (strcmp($side, 'as_object') == 0) {
    $sql .= " WHERE FR.object_id = :feature_id";
  }
  if (strcmp($side, 'as_subject') == 0) {
    $sql .= " WHERE FR.subject_id = :feature_id";
  }
  $sql .= " ORDER BY FR.rank";

  // Get the relationships.
  $results = chado_query($sql, [':feature_id' => $feature_id]);

  // Iterate through the relationships, put these in an array and add
  // in the Drupal node id if one exists.
  $i = 0;
  $nodesql = "SELECT nid FROM {chado_feature} WHERE feature_id = :feature_id";
  $relationships = [];
  while ($rel = $results->fetchObject()) {
    $node = db_query($nodesql, [':feature_id' => $rel->subject_id])->fetchObject();
    if ($node) {
      $rel->subject_nid = $node->nid;
    }
    $node = db_query($nodesql, [':feature_id' => $rel->object_id])->fetchObject();
    if ($node) {
      $rel->object_nid = $node->nid;
    }
    $relationships[$i++] = $rel;
  }
  return $relationships;
}