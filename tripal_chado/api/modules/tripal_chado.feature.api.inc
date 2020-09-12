<?php
/**
 * @file
 * Provides API functions specifically for managing feature
 * records in Chado.
 */

/**
 * @defgroup tripal_feature_api Chado Feature
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing feature
 * records in Chado especially retrieving relationships and sequences derived
 * from relationships and feature alignments.
 * @}
 */
/**
 * Used for autocomplete in forms for identifying for publications.
 *
 * @param $field
 *   The field in the publication to search on.
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_feature_api
 */
function chado_autocomplete_feature($string = '') {
  $items = [];
  $sql = "
    SELECT
      F.feature_id, F.uniquename, F.name,
      O.genus, O,species,
      CVT.name as type
    FROM {feature} F
      INNER JOIN {organism} O ON O.organism_id = F.organism_id
      INNER JOIN {cvterm} CVT ON CVT.cvterm_id = F.type_id
    WHERE lower(F.uniquename) like lower(:str)
    ORDER by F.uniquename
    LIMIT 25 OFFSET 0
  ";
  $features = chado_query($sql, [':str' => $string . '%']);
  while ($feature = $features->fetchObject()) {
    $key = "$feature->uniquename [id: $feature->feature_id]";
    $items[$key] = "$feature->uniquename ($feature->name, $feature->type, $feature->genus $feature->species)";
  }

  drupal_json_output($items);
}

/**
 * Performs a reverse compliment of a nucleotide sequence.
 *
 * @param $sequence
 *   The nucelotide sequence.
 *
 * @return
 *   an upper-case reverse complemented sequence.
 *
 * @ingroup tripal_feature_api
 */
function chado_reverse_compliment_sequence($sequence) {

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
 * Retrieves the sequences for a given feature.
 *
 * If a feature has multiple alignments or multiple relationships then
 * multiple sequences will be returned.
 *
 * @param $feature
 *   An associative array describing the feature. Valid keys include:
 *    - feature_id: The feature_id of the feature for which the sequence will
 *      be retrieved.
 *    - name: The feature name. This will appear on the FASTA definition line.
 *    - parent_id:  (optional) only retrieve a sequence if 'derive_from_parent'
 *      is true and the parent matches this ID.
 *    - featureloc_id: (optional) only retrieve a sequence if
 *      'derive_from_parent' is true and the alignment is defined with this
 *      featureloc_id.
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - width: Indicate the number of bases to use per line.  A new line will
 *      be added after the specified number of bases on each line.
 *    - is_html: Set to '1' if  the sequence is meant to be displayed on a web
 *      page. This will cause a <br> tag to separate lines of the FASTA
 *   sequence.
 *    - derive_from_parent: Set to '1' if the sequence should be obtained from
 *      the parent to which this feature is aligned.
 *    - aggregate: Set to '1' if the sequence should only contain sub features,
 *      excluding intro sub feature sequence.  For example, set this option to
 *      obtain just the coding sequence of an mRNA.
 *    - upstream: An integer specifing the number of upstream bases to include
 *      in the output.
 *    - downstream: An integer specifying the number of downstream bases to
 *      include in the output.
 *    - sub_feature_types: Only include sub features (or child features) of
 *      the types provided in the array.
 *    - relationship_type: If a relationship name is provided (e.g.
 *   sequence_of)
 *      then any sequences that are in relationships of this type with matched
 *      sequences are also included.
 *    - relationship_part: If a relationship is provided in the preceding
 *      argument then the rel_part must be either 'object' or 'subject' to
 *      indicate which side of the relationship the matched features belong.
 *
 * @return
 *   an array of matching sequence in the following keys for each sequence:
 *    - types: an array of feature types that were used to derive
 *      the sequence (e.g. from an aggregated sequence)
 *    - upstream: the number of upstream bases included in the sequence
 *    - downstream: the number of downstream bases included in the
 *      sequence
 *    - defline: the definintion line used to create a FASTA sequence
 *    - residues: the residues
 *    - featureloc_id: the featureloc_id if the sequences is from an
 *        alignment
 *
 * @ingroup tripal_feature_api
 */
function chado_get_feature_sequences($feature, $options) {

  // Default values for finding the feature.
  $feature_id = array_key_exists('feature_id', $feature) ? $feature['feature_id'] : 0;
  $parent_id = array_key_exists('parent_id', $feature) ? $feature['parent_id'] : 0;
  $featureloc_id = array_key_exists('featureloc_id', $feature) ? $feature['featureloc_id'] : 0;
  $feature_name = array_key_exists('name', $feature) ? $feature['name'] : '';

  // Default values for building the sequence.
  $num_bases_per_line = array_key_exists('width', $options) ? $options['width'] : 50;
  $derive_from_parent = array_key_exists('derive_from_parent', $options) ? $options['derive_from_parent'] : 0;
  $aggregate = array_key_exists('aggregate', $options) ? $options['aggregate'] : 0;
  $upstream = array_key_exists('upstream', $options) ? $options['upstream'] : 0;
  $downstream = array_key_exists('downstream', $options) ? $options['downstream'] : 0;
  $sub_features = array_key_exists('sub_feature_types', $options) ? $options['sub_feature_types'] : [];
  $relationship = array_key_exists('relationship_type', $options) ? $options['relationship_type'] : '';
  $rel_part = array_key_exists('relationship_part', $options) ? $options['relationship_part'] : '';
  $is_html = array_key_exists('is_html', $options) ? $options['is_html'] : 0;
  $is_txt = array_key_exists('is_txt', $options) ? $options['is_txt'] : 0;
  $is_raw = array_key_exists('is_raw', $options) ? $options['is_raw'] : 1;

  if (!$upstream) {
    $upstream = 0;
  }
  if (!$downstream) {
    $downstream = 0;
  }

  // Make sure the sub_features variable is an array.
  if (!is_array($sub_features)) {
    tripal_report_error('tripal_feature', TRIPAL_ERROR,
      "'sub_features' option must be an array for function chado_get_feature_sequences().",
      []
    );
    return [];
  }

  // If a relationship was specified then retreive and the sequences that
  // have the given relationship and the recurse to extract the appropriate
  // sequence.
  if ($rel_part == "object" or $rel_part == "subject") {
    if ($rel_part == "subject") {
      $sql = '
        SELECT FO.feature_id, FO.name, FO.uniquename, CVTO.name as feature_type, O.genus, O.species
        FROM {feature} FS
          INNER JOIN {feature_relationship} FR ON FR.subject_id   = FS.feature_id
          INNER JOIN {cvterm} CVTFR            ON CVTFR.cvterm_id = FR.type_id
          INNER JOIN {feature} FO              ON FO.feature_id   = FR.object_id
          INNER JOIN {cvterm} CVTO             ON CVTO.cvterm_id  = FO.type_id
          INNER JOIN {organism} O              ON O.organism_id   = FO.organism_id
        WHERE
          FS.feature_id = :feature_id AND
          CVTFR.name    = :relationship
      ';
      $features = chado_query($sql, [
        ':feature_id' => $feature_id,
        ':relationship' => $relationship,
      ]);
    }
    if ($rel_part == "object") {
      $sql = '
        SELECT FS.feature_id, FS.name, FS.uniquename, CVTO.name as feature_type, O.genus, O.species
        FROM {feature} FO
          INNER JOIN {feature_relationship} FR ON FR.object_id    = FO.feature_id
          INNER JOIN {cvterm} CVTFR            ON CVTFR.cvterm_id = FR.type_id
          INNER JOIN {feature} FS              ON FS.feature_id   = FR.subject_id
          INNER JOIN {cvterm} CVTO             ON CVTO.cvterm_id  = FS.type_id
          INNER JOIN {organism} O              ON O.organism_id   = FS.organism_id
        WHERE
          FO.feature_id = :feature_id AND
          CVTFR.name    = :relationship
      ';
      $features = chado_query($sql, [
        ':feature_id' => $feature_id,
        ':relationship' => $relationship,
      ]);
    }
    $sequences = '';
    while ($feature = $features->fetchObject()) {

      // Recurse and get the sequences for these in the relationship.
      if ($rel_part == "subject") {
        $defline = "$feature_name, $relationship, $feature->uniquename $feature->feature_type ($feature->genus $feature->species)";
      }
      if ($rel_part == "object") {
        $defline = "$feature->uniquename $feature->feature_type ($feature->genus $feature->species), $relationship, $feature_name";
      }
      return chado_get_feature_sequences(
        [
          'feature_id' => $feature->feature_id,
          'name' => $defline,
          'parent_id' => $parent_id,
        ],
        [
          'width' => $num_bases_per_line,
          'derive_from_parent' => $derive_from_parent,
          'aggregate' => $aggregate,
          'upstream' => $upstream,
          'downstream' => $downstream,
          'sub_features' => $sub_features,
        ]
      );
    }
  }

  // Prepare the queries we're going to use later during the render phase
  // This SQL statement uses conditionals in the select clause to handle
  // cases cases where the alignment is in the reverse direction and when
  // the upstream and downstream extensions go beyond the lenght of the
  // parent sequence.
  $parent_sql = '
    SELECT featureloc_id, srcname, srcfeature_id, strand, srctypename, typename,
      fmin, fmax, upstream, downstream, adjfmin, adjfmax,
      substring(residues from (cast(adjfmin as int4) + 1) for cast((upstream + (fmax - fmin) + downstream) as int4))  as residues,
      genus, species
    FROM (
      SELECT
        FL.featureloc_id, OF.name srcname, FL.srcfeature_id, FL.strand,
        OCVT.name as srctypename, SCVT.name as typename,
        FL.fmin, FL.fmax, OO.genus, OO.species,
        CASE
          WHEN FL.strand >= 0 THEN
            CASE
               WHEN FL.fmin - :upstream <= 0 THEN 0
               ELSE FL.fmin - :upstream
            END
          WHEN FL.strand < 0 THEN
            CASE
               WHEN FL.fmin - :downstream <= 0 THEN 0
               ELSE FL.fmin - :downstream
            END
        END as adjfmin,
        CASE
          WHEN FL.strand >= 0 THEN
            CASE
              WHEN FL.fmax + :downstream > OF.seqlen THEN OF.seqlen
              ELSE FL.fmax + :downstream
            END
          WHEN FL.strand < 0 THEN
            CASE
              WHEN FL.fmax + :upstream > OF.seqlen THEN OF.seqlen
              ELSE FL.fmax + :upstream
            END
        END as adjfmax,
        CASE
          WHEN FL.strand >= 0 THEN
            CASE
               WHEN FL.fmin - :upstream <= 0 THEN FL.fmin
               ELSE :upstream
            END
          ELSE
            CASE
               WHEN FL.fmax + :upstream > OF.seqlen THEN OF.seqlen - FL.fmax
               ELSE :upstream
            END
        END as upstream,
        CASE
          WHEN FL.strand >= 0 THEN
            CASE
               WHEN FL.fmax + :downstream > OF.seqlen THEN OF.seqlen - FL.fmax
               ELSE :downstream
            END
          ELSE
            CASE
               WHEN FL.fmin - :downstream <= 0 THEN FL.fmin
               ELSE :downstream
            END
        END as downstream,
        OF.residues
      FROM {featureloc} FL
        INNER JOIN {feature} SF   on FL.feature_id    = SF.feature_id
        INNER JOIN {cvterm}  SCVT on SF.type_id       = SCVT.cvterm_id
        INNER JOIN {feature} OF   on FL.srcfeature_id = OF.feature_id
        INNER JOIN {cvterm}  OCVT on OF.type_id       = OCVT.cvterm_id
        INNER JOIN {organism} OO  on OF.organism_id   = OO.organism_id
      WHERE SF.feature_id = :feature_id and NOT (OF.residues = \'\' or OF.residues IS NULL) ORDER BY fmin) as tbl1
  ';
  // This query is meant to get all of the sub features of any given
  // feature (arg #1) and order them as they appear on the reference
  // feature (arg #2).
  $sfsql = '
    SELECT SF.feature_id, CVT.name as type_name, SF.type_id
    FROM {feature_relationship} FR
      INNER JOIN {feature} SF    ON SF.feature_id = FR.subject_id
      INNER JOIN {cvterm} CVT    ON CVT.cvterm_id = SF.type_id
      INNER JOIN {featureloc} FL ON FL.feature_id = FR.subject_id
      INNER JOIN {feature} PF    ON PF.feature_id = FL.srcfeature_id
    WHERE FR.object_id = :feature_id and PF.feature_id = :srcfeature_id
    ORDER BY FL.fmin ASC
  ';
  // For counting the number of children.
  $fsql = '
    SELECT count(*) as num_children
    FROM {feature_relationship} FR
      INNER JOIN {feature} SF    ON SF.feature_id = FR.subject_id
      INNER JOIN {cvterm} CVT    ON CVT.cvterm_id = SF.type_id
      INNER JOIN {featureloc} FL ON FL.feature_id = FR.subject_id
      INNER JOIN {feature} PF    ON PF.feature_id = FL.srcfeature_id
    WHERE FR.object_id = :feature_id and PF.feature_id = :srcfeature_id
  ';

  // The array to be returned.
  $sequences = [];

  // If we need to get the sequence from the parent then do so now.
  if ($derive_from_parent) {

    // Execute the query to get the sequence from the parent.
    $parents = chado_query($parent_sql, [
      ':upstream' => $upstream,
      ':downstream' => $downstream,
      ':feature_id' => $feature_id,
    ]);
    while ($parent = $parents->fetchObject()) {

      // If the user specified a particular parent and this one doesn't
      // match then skip it.
      if ($parent_id and $parent_id != $parent->srcfeature_id) {
        continue;
      }
      // If the user specified a particular featureloc_id and this one
      // doesn't match then skip it.
      if ($featureloc_id and $featureloc_id != $parent->featureloc_id) {
        continue;
      }
      // Initialize the sequence for each parent.
      $seq = '';
      $notes = '';
      $types = [];

      // If we are to aggregate then we will ignore the feature returned
      // by the query above and rebuild it using the sub features.
      if ($aggregate) {

        // now get the sub features that are located on the parent.
        $children = chado_query($sfsql, [
          ':feature_id' => $feature_id,
          ':srcfeature_id' => $parent->srcfeature_id,
        ]);
        $num_children = chado_query($fsql, [
          ':feature_id' => $feature_id,
          ':srcfeature_id' => $parent->srcfeature_id,
        ])->fetchField();

        // Iterate through the sub features and concat their sequences. They
        // should already be in order.
        $i = 0;
        $already_processed_children = [];
        while ($child = $children->fetchObject()) {
          // In some cases, a feature may be discontinuous (i.e. one feature
          // spread over several positions). In this case, the feature will
          // appear multiple times and we want to prevent addition of the
          // sequence multiple times.  A simple check to make sure we haven't
          // seen the feature already should suffice.
          if (count($already_processed_children) > 0 and in_array($child->feature_id, $already_processed_children)) {
            continue;
          }
          $already_processed_children[] = $child->feature_id;

          // If the callee has specified that only certain sub features should be
          // included then continue if this child is not one of those allowed
          // subfeatures.
          if (count($sub_features) > 0 and !in_array($child->type_name, $sub_features)) {
            $i++;
            continue;
          }

          // Keep up with the types.
          if (!in_array($child->type_name, $types)) {
            $types[] = $child->type_name;
          }

          // If the first sub feature we need to include the upstream bases.
          // First check if the feature is in the foward direction or the
          // reverse.
          if ($i == 0 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //    ....---->  ---->
            //     up    1       2
            $q = chado_query($parent_sql, [
              ':upstream' => $upstream,
              ':downstream' => 0,
              ':feature_id' => $child->feature_id,
            ]);
          }
          elseif ($i == 0 and $parent->strand < 0) { // reverse direction
            // -------------------------- ref
            //    ....<----  <----
            //    down  1       2
            $q = chado_query($parent_sql, [
              ':upstream' => 0,
              ':downstream' => $downstream,
              ':feature_id' => $child->feature_id,
            ]);
          }

          // Next, if the last sub feature we need to include the downstream
          // bases. First check if the feature is in the forward direction or
          // the reverse.
          elseif ($i == $num_children - 1 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //        ---->  ---->....
            //          1       2 down
            $q = chado_query($parent_sql, [
              ':upstream' => 0,
              ':downstream' => $downstream,
              ':feature_id' => $child->feature_id,
            ]);
          }
          elseif ($i == $num_children - 1 and $parent->strand < 0) { // reverse direction
            // -------------------------- ref
            //        <----  <----....
            //          1       2  up
            $q = chado_query($parent_sql, [
              ':upstream' => $upstream,
              ':downstream' => 0,
              ':feature_id' => $child->feature_id,
            ]);
          }
          // For internal sub features we don't want upstream or downstream bases.
          else {
            $q = chado_query($parent_sql, [
              ':upstream' => 0,
              ':downstream' => 0,
              ':feature_id' => $child->feature_id,
            ]);
          }
          while ($subseq = $q->fetchObject()) {
            // concatenate the sequences of all the sub features
            if ($subseq->srcfeature_id == $parent->srcfeature_id) {
              $seq .= $subseq->residues;
            }
            if ($subseq->upstream > 0) {
              $notes .= "Includes " . $subseq->upstream . " bases upstream.  ";
            }
            if ($subseq->downstream > 0) {
              $notes .= "Includes " . $subseq->downstream . " bases downstream.  ";
            }
          }
          $i++;
        }
      }
      // If this isn't an aggregate then use the parent residues.
      else {
        $seq = $parent->residues;
        if ($parent->upstream > 0) {
          $notes .= "Includes " . $parent->upstream . " bases upstream.  ";
        }
        if ($parent->downstream > 0) {
          $notes .= "Includes " . $parent->downstream . " bases downstream.  ";
        }
      }

      // Get the reverse compliment if feature is on the reverse strand.
      $dir = 'forward';
      $length = strlen($seq);
      if ($parent->strand < 0) {
        $seq = chado_reverse_compliment_sequence($seq);
        $dir = 'reverse';
      }
      // Now format for display.
      if ($is_html) {
        $seq = wordwrap($seq, $num_bases_per_line, "<br>", TRUE);
      }
      if ($is_txt) {
        $seq = wordwrap($seq, $num_bases_per_line, "\r\n", TRUE);
      }
      if (!$seq) {
        $notes .= "No sequence available.";
      }

      if (count($types) > 0) {
        $notes .= "Excludes all bases but those of type(s): " . implode(', ', $types) . ". ";
      }

      // Construct the definition line for this feature. To construct the
      // defline we need a featureloc record, so we'll create one using
      // the information we have.
      $featureloc = new stdClass;
      $featureloc->feature_id = $feature;
      $featureloc->fmin = $parent->adjfmin;
      $featureloc->fmax = $parent->adjfmax;
      $featureloc->strand = $parent->strand;
      $featureloc->srcfeature_id = new stdClass;
      $featureloc->srcfeature_id->name = $parent->srcname;
      $featureloc->srcfeature_id->type_id = $parent->srctypename;
      $featureloc->srcfeature_id->organism_id = new stdClass;
      $featureloc->srcfeature_id->organism_id->genus = $parent->genus;
      $featureloc->srcfeature_id->organism_id->species = $parent->species;
      // Get a proper feature object.
      $f = chado_generate_var('feature', ['feature_id' => $feature_id]);
      $defline = chado_get_fasta_defline($f, $notes, $featureloc, '', $length);

      $sequences[] = [
        'types' => $types,
        'upstream' => $parent->upstream,
        'downstream' => $parent->downstream,
        'defline' => $defline,
        'residues' => $seq,
        'featureloc_id' => $parent->featureloc_id,
        'length' => $length,
      ];
    }
  }
  // If we are not getting the sequence from the parent sequence then
  // use what comes through from the feature record.
  else {

    $f = chado_generate_var('feature', ['feature_id' => $feature_id]);
    $f = chado_expand_var($f, 'field', 'feature.residues');

    $residues = $f->residues;
    $length = strlen($residues);
    if ($is_html) {
      $residues = wordwrap($residues, $num_bases_per_line, "<br>", TRUE);
    }
    else {
      $residues = wordwrap($residues, $num_bases_per_line, "\r\n", TRUE);
    }

    // Get the definintion line for this feature.
    $defline = chado_get_fasta_defline($f, '', NULL, '', $length);

    // Add to the sequence array.
    $sequences[] = [
      'types' => $f->type_id->name,
      'upstream' => 0,
      'downstream' => 0,
      'defline' => $defline,
      'residues' => $residues,
      'length' => $length,
    ];
  }

  return $sequences;
}

/**
 * Retrieves the bulk sequences for a given feature.
 *
 * @param $options
 *   An associative array of options for selecting a feature. Valid keys
 *   include:
 *    - org_commonname: The common name of the organism for which sequences
 *      should be retrieved
 *    - genus: The genus of the organism for which sequences should be
 *   retrieved
 *    - species: The species of the organism for which sequences should be
 *      retrieved
 *    - analysis_name: The name of an analysis to which sequences belong. Only
 *      those that are associated with the analysis will be retrieved.
 *    - type: The type of feature (a sequence ontology term).
 *    - feature_name: the name of the feature. Can be an array of feature
 *   names.
 *    - feature_uname: the uniquename of the feature.  Can be an array of
 *      feature unique names.
 *    - upstream: An integer specifing the number of upstream bases to include
 *      in the output
 *    - downstream: An integer specifying the number of downstream bases to
 *      include in the output.
 *    - derive_from_parent: Set to '1' if the sequence should be obtained from
 *      the parent to which this feature is aligned.
 *    - aggregate: Set to '1' if the sequence should only contain sub features,
 *      excluding intro sub feature sequence.  For example, set this option to
 *      obtain just the coding sequence of an mRNA.
 *    - sub_feature_types: Only include sub features (or child features) of
 *      the types provided in the array
 *    - relationship_type: If a relationship name is provided (e.g.
 *   sequence_of)
 *      then any sequences that are in relationships of this type with matched
 *      sequences are also included
 *    - relationship_part: If a relationship is provided in the preceding
 *      argument then the rel_part must be either 'object' or 'subject' to
 *      indicate which side of the relationship the matched features belong
 *    - width: Indicate the number of bases to use per line.  A new line will
 *      be added after the specified number of bases on each line.
 *    - is_html: Set to '1' if  the sequence is meant to be displayed on a
 *      web page. This will cause a <br> tag to separate lines of the FASTA
 *      sequence.
 *
 * @return
 *   Returns an array of sequences. The sequences will be in an array with the
 *   following keys for each sequence:
 *      'types'         => an array of feature types that were used to derive
 *         the sequence (e.g. from an aggregated sequence)
 *      'upstream'      => the number of upstream bases in the sequence
 *      'downstream'    => the number of downstream bases in the sequence
 *      'defline'       => the definintion line used to create a FASTA sequence
 *      'residues'      => the residues
 *      'featureloc_id' => the featureloc_id if from an alignment
 *
 * @ingroup tripal_feature_api
 */
function chado_get_bulk_feature_sequences($options) {

  // Default values for building the sequence
  $org_commonname = array_key_exists('org_commonname', $options) ? $options['org_commonname'] : '';
  $genus = array_key_exists('genus', $options) ? $options['genus'] : '';
  $species = array_key_exists('species', $options) ? $options['species'] : '';
  $analysis_name = array_key_exists('analysis_name', $options) ? $options['analysis_name'] : '';
  $type = array_key_exists('type', $options) ? $options['type'] : '';
  $feature_name = array_key_exists('feature_name', $options) ? $options['feature_name'] : '';
  $feature_uname = array_key_exists('feature_uname', $options) ? $options['feature_uname'] : '';
  $derive_from_parent = array_key_exists('derive_from_parent', $options) ? $options['derive_from_parent'] : 0;
  $aggregate = array_key_exists('aggregate', $options) ? $options['aggregate'] : 0;
  $sub_features = array_key_exists('sub_feature_types', $options) ? $options['sub_feature_types'] : [];
  $relationship = array_key_exists('relationship_type', $options) ? $options['relationship_type'] : '';
  $rel_part = array_key_exists('relationship_part', $options) ? $options['relationship_part'] : '';
  $num_bases_per_line = array_key_exists('width', $options) ? $options['width'] : 50;
  $upstream = array_key_exists('upstream', $options) ? $options['upstream'] : 0;
  $downstream = array_key_exists('downstream', $options) ? $options['downstream'] : 0;

  if (!$type and !$feature_name and !$genus) {
    print "Please provide a type, feature name or genus\n";
    return;
  }

  // Get the list of features.
  $vars = [];
  $sql = "
    SELECT DISTINCT F.feature_id, F.name, F.uniquename,
      O.genus, O.species, CVT.name as feature_type
    FROM {feature} F
      INNER JOIN {organism} O on O.organism_id = F.organism_id
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
  ";
  if ($analysis_name) {
    $sql .= "
      INNER JOIN {analysisfeature} AF on AF.feature_id = F.feature_id
      INNER JOIN {analysis} A on AF.analysis_id = A.analysis_id
    ";
  }
  $sql .= "WHERE (1=1) ";
  if ($org_commonname) {
    $sql .= "AND O.common_name = :common_name ";
    $vars[':common_name'] = $org_commonname;
  }
  if ($genus) {
    $sql .= "AND O.genus = :genus ";
    $vars[':genus'] = $genus;
  }
  if ($species) {
    $sql .= "AND O.species = :species ";
    $vars[':species'] = $species;
  }
  if ($type) {
    $sql .= "AND CVT.name = :cvtname ";
    $vars[':cvtname'] = $type;
  }
  if ($feature_name) {
    if (is_array($feature_name)) {
      $sql .= "AND F.name IN (";
      foreach ($feature_name as $i => $fname) {
        $sql .= ":fname$i, ";
        $vars[":fname$i"] = $fname;
      }
      // Remove the trailing comma and close the parenthesis.
      $sql = substr($sql, 0, -2) . ")";
    }
    else {
      $sql .= "AND F.name = :fname";
      $vars[':fname'] = $feature_name;
    }
  }
  if ($feature_uname) {
    if (is_array($feature_uname)) {
      $sql .= "AND F.uniquename IN (";
      foreach ($feature_uname as $i => $funame) {
        $sql .= ":funame$i, ";
        $vars[":funame$i"] = $funame;
      }
      // Remove the trailing comma and close the parenthesis.
      $sql = substr($sql, 0, -2) . ")";
    }
    else {
      $sql .= "AND F.uniquename = :funame";
      $vars[':funame'] = $feature_uname;
    }
  }
  if ($analysis_name) {
    $sql .= "AND A.name = :aname";
    $vars[':aname'] = $analysis_name;
  }
  $num_bases_per_line = 50;
  $num_seqs = 0;
  $q = chado_query($sql, $vars);

  $sequences = [];
  while ($feature = $q->fetchObject()) {
    // get the sequences
    $seqs = chado_get_feature_sequences(['feature_id' => $feature->feature_id], $options);
    $sequences = array_merge($sequences, $seqs);
    $num_seqs++;
  }

  return $sequences;
}

/**
 * Returns a definition line that can be used in a FASTA file.
 *
 * @param $feature
 *   A single feature object containing all the fields from the chado.feature
 *   table. Best case is to provide an object generated by the
 *   chado_generate_var() function.
 * @param $notes
 *   Optional: additional notes to be added to the definition line.
 * @param $featureloc
 *   Optional: a single featureloc object generated using chado_generate_var
 *   that contains a record from the chado.featureloc table. Provide this if the
 *   sequence was obtained by using the alignment rather than from the
 *   feature.residues column.
 * @param $type
 *   Optional: the type of sequence. By default the feature type is used.
 * @param $length
 *   Optional: the length of the sequence.
 *
 * @return
 *   A string of the format: uniquename|name|type|feature_id
 *   or if an alignment:  srcfeature_name:fmin..fmax[+-]; alignment of
 *   uniquename|name|type|feature_id.
 *
 * @ingroup tripal_feature_api
 */
function chado_get_fasta_defline($feature, $notes = '', $featureloc = NULL, $type = '', $length = 0) {

  // Make sure the featureloc object has the srcfeature if not, then add it.
  if ($featureloc) {
    if (!is_object($featureloc->srcfeature_id)) {
      $featureloc->srcfeature_id = chado_generate_var('feature', ['feature_id' => $featureloc->srcfeature_id]);
    }
    if (!is_object($featureloc->srcfeature_id->organism_id)) {
      $featureloc->srcfeature_id->organism_id = chado_generate_var('organism', ['organism_id' => $featureloc->srcfeature_id->organism_id]);
    }
  }
  // Make sure the feature object has the organism if not, then add it.
  if (!is_object($feature->organism_id)) {
    $feature->organism_id = chado_generate_var('organism', ['organism_id' => $feature->organism_id]);
  }

  // If a type is not provided then use the default type.
  if (!$type) {
    $type = $feature->type_id->name;
  }

  // Construct the definition line.
  $defline = $feature->uniquename . " " .
    'ID=' . $feature->uniquename . "|" .
    'Name=' . $feature->name . "|" .
    'organism=' . $feature->organism_id->genus . " " . $feature->organism_id->species . "|" .
    'type=' . $type . '|';
  if ($length > 0) {
    $defline .= "length=" . $length . "bp|";
  }
  if ($featureloc) {
    $defline .= "location=Sequence derived from alignment at " . chado_get_location_string($featureloc);
    $defline .= " (" . $featureloc->srcfeature_id->organism_id->genus . " " . $featureloc->srcfeature_id->organism_id->species . ")|";
  }
  if ($notes) {
    $defline .= "Notes=$notes|";
  }
  $defline = substr($defline, 0, -1); // remove the trailing |
  return $defline;
}

/**
 * Returns a string representing a feature location in an alignment.
 *
 * @param $featureloc
 *   A single featureloc object generated using chado_generate_var that
 *   contains a record from the chado.featureloc table.
 *
 * @return
 *   A string of the format: uniquename:featurelocmin..featurelocmax.strand
 *
 * @ingroup tripal_feature_api
 */
function chado_get_location_string($featureloc) {
  $feature = $featureloc->feature_id;

  $strand = '';
  if ($featureloc->strand == 1) {
    $strand = '+';
  }
  elseif ($featureloc->strand == -1) {
    $strand = '-';
  }

  return $featureloc->srcfeature_id->name . ":" . ($featureloc->fmin + 1) . ".." . $featureloc->fmax . $strand;
}
