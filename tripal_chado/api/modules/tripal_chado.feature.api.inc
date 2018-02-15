<?php
/**
 * @file
 * Provides an application programming interface (API) for working with features
 */

/**
 * @defgroup tripal_feature_api Chado Feature
 * @ingroup tripal_chado_api
 * @{
 * @}
 */
/**
 * Used for autocomplete in forms for identifying for publications.
 *
 * @param $field
 *   The field in the publication to search on.
 * @param $string
 *   The string to search for
 *
 * @return
 *   A json array of terms that begin with the provided string
 *
 * @ingroup tripal_feature_api
 */
function tripal_autocomplete_feature($string = '') {
  $items = array();
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
  $features = chado_query($sql, array(':str' => $string . '%'));
  while ($feature = $features->fetchObject()) {
    $key = "$feature->uniquename [id: $feature->feature_id]";
    $items[$key] = "$feature->uniquename ($feature->name, $feature->type, $feature->genus $feature->species)";
  }

  drupal_json_output($items);
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
function tripal_reverse_compliment_sequence($sequence) {

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
 *      be retrieved
 *    - name: The feature name. This will appear on the FASTA definition line
 *    - parent_id:  (optional) only retrieve a sequence if 'derive_from_parent'
 *      is true and the parent matches this ID.
 *    - featureloc_id: (optional) only retrieve a sequence if 'derive_from_parent' is
 *      true and the alignment is defined with this featureloc_id
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - width: Indicate the number of bases to use per line.  A new line will
 *      be added after the specified number of bases on each line.
 *    - is_html: Set to '1' if  the sequence is meant to be displayed on a web
 *      page. This will cause a <br> tag to separate lines of the FASTA sequence.
 *    - derive_from_parent: Set to '1' if the sequence should be obtained from
 *      the parent to which this feature is aligned.
 *    - aggregate: Set to '1' if the sequence should only contain sub features,
 *      excluding intro sub feature sequence.  For example, set this option to
 *      obtain just the coding sequence of an mRNA.
 *    - upstream: An integer specifing the number of upstream bases to include
 *      in the output
 *    - downstream: An integer specifying the number of downstream bases to
 *      include in the output.
 *    - sub_feature_types: Only include sub features (or child features) of
 *      the types provided in the array
 *    - relationship_type: If a relationship name is provided (e.g. sequence_of)
 *      then any sequences that are in relationships of this type with matched
 *      sequences are also included
 *    - relationship_part: If a relationship is provided in the preceeding
 *      argument then the rel_part must be either 'object' or 'subject' to
 *      indicate which side of the relationship the matched features belong
 *
 * @return
 *   an array of matching sequence in the following keys for each sequence:
 *      'types'         => an array of feature types that were used to derive
 *         the sequence (e.g. from an aggregated sequence)
 *      'upstream'      => the number of upstream bases included in the sequence
 *      'downstream'    => the number of downstream bases included in the
 *        sequence
 *      'defline'       => the definintion line used to create a FASTA sequence
 *      'residues'      => the residues
 *      'featureloc_id' => the featureloc_id if the sequences is from an
 *        alignment
 *
 * @ingroup tripal_feature_api
 */
function tripal_get_feature_sequences($feature, $options) {

  // Default values for finding the feature.
  $feature_id         = array_key_exists('feature_id', $feature)     ? $feature['feature_id']     : 0;
  $parent_id          = array_key_exists('parent_id', $feature)      ? $feature['parent_id']      : 0;
  $featureloc_id      = array_key_exists('featureloc_id', $feature)  ? $feature['featureloc_id']  : 0;
  $feature_name       = array_key_exists('name', $feature)           ? $feature['name']           : '';

  // Default values for building the sequence.
  $num_bases_per_line = array_key_exists('width', $options)              ? $options['width']              : 50;
  $derive_from_parent = array_key_exists('derive_from_parent', $options) ? $options['derive_from_parent'] : 0;
  $aggregate          = array_key_exists('aggregate', $options)          ? $options['aggregate']          : 0;
  $upstream           = array_key_exists('upstream', $options)           ? $options['upstream']           : 0;
  $downstream         = array_key_exists('downstream', $options)         ? $options['downstream']         : 0;
  $sub_features       = array_key_exists('sub_feature_types', $options)  ? $options['sub_feature_types']  : array();
  $relationship       = array_key_exists('relationship_type', $options)  ? $options['relationship_type']  : '';
  $rel_part           = array_key_exists('relationship_part', $options)  ? $options['relationship_part']  : '';
  $is_html            = array_key_exists('is_html', $options)            ? $options['is_html']            : 0;
  $is_txt             = array_key_exists('is_txt', $options)             ? $options['is_txt']             : 0;
  $is_raw             = array_key_exists('is_raw', $options)             ? $options['is_raw']             : 1;

  if (!$upstream) {
    $upstream = 0;
  }
  if (!$downstream) {
    $downstream = 0;
  }

  // Make sure the sub_features variable is an array.
  if (!is_array($sub_features)) {
    tripal_report_error('tripal_feature', TRIPAL_ERROR,
      "'sub_features' option must be an array for function tripal_get_feature_sequences().",
      array()
    );
    return array();
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
      $features = chado_query($sql, array(':feature_id' => $feature_id, ':relationship' => $relationship));
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
      $features = chado_query($sql, array(':feature_id' => $feature_id, ':relationship' => $relationship));
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
      return tripal_get_feature_sequences(
        array(
          'feature_id' => $feature->feature_id,
          'name' => $defline,
          'parent_id' => $parent_id,
        ),
        array(
          'width' => $num_bases_per_line,
          'derive_from_parent' => $derive_from_parent,
          'aggregate' => $aggregate,
          'upstream' => $upstream,
          'downstream' => $downstream,
          'sub_features' => $sub_features,
        )
      );
    }
  }

  // Prepare the queries we're going to use later during the render phase
  // This SQL statement uses conditionals in the select clause to handle
  // cases cases where the alignment is in the reverse direction and when
  // the upstream and downstream extensions go beyond the lenght of the
  // parent sequence.
  $parent_sql ='
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
      WHERE SF.feature_id = :feature_id and NOT (OF.residues = \'\' or OF.residues IS NULL)) as tbl1
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
  $fsql ='
    SELECT count(*) as num_children
    FROM {feature_relationship} FR
      INNER JOIN {feature} SF    ON SF.feature_id = FR.subject_id
      INNER JOIN {cvterm} CVT    ON CVT.cvterm_id = SF.type_id
      INNER JOIN {featureloc} FL ON FL.feature_id = FR.subject_id
      INNER JOIN {feature} PF    ON PF.feature_id = FL.srcfeature_id
    WHERE FR.object_id = :feature_id and PF.feature_id = :srcfeature_id
  ';

  // The array to be returned.
  $sequences = array();

  // If we need to get the sequence from the parent then do so now.
  if ($derive_from_parent) {

    // Execute the query to get the sequence from the parent.
    $parents = chado_query($parent_sql, array(':upstream' => $upstream, ':downstream' => $downstream, ':feature_id' => $feature_id));
    while ($parent = $parents->fetchObject()) {

      // If the user specified a particular parent and this one doesn't
      // match then skip it.
      if ($parent_id and $parent_id != $parent->srcfeature_id) {
        continue;
      }
      // if the user specified a particular featureloc_id and this one
      // doesn't match then skip it.
      if ($featureloc_id and $featureloc_id != $parent->featureloc_id) {
        continue;
      }
      // Initialize the sequence for each parent.
      $seq = '';
      $notes = '';
      $types = array();

      // if we are to aggregate then we will ignore the feature returned
      // by the query above and rebuild it using the sub features
      if ($aggregate) {

        // now get the sub features that are located on the parent.
        $children = chado_query($sfsql, array(':feature_id' => $feature_id, ':srcfeature_id' => $parent->srcfeature_id));
        $num_children = chado_query($fsql, array(':feature_id' => $feature_id, ':srcfeature_id' => $parent->srcfeature_id))->fetchField();

        // Iterate through the sub features and concat their sequences. They
        // should already be in order.
        $i = 0;
        $already_processed_children = array();
        while ($child = $children->fetchObject()) {
          // In some cases, a feature may be discontinuous (i.e. one feature
          // spread over several positions). In this case, the feature will
          // appear multiple times and we want to prevent addition of the
          // sequence multiple times.  A simple check to make sure we haven't
          // seen the feature already should suffice.
          if (count($already_processed_children) > 0 and in_array($child->feature_id, $already_processed_children)){
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

          // keep up with the types
          if (!in_array($child->type_name, $types)) {
            $types[] = $child->type_name;
          }

          // if the first sub feature we need to include the upstream bases. first check if
          // the feature is in the foward direction or the reverse.
          if ($i == 0 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //    ....---->  ---->
            //     up    1       2
            $q = chado_query($parent_sql, array(':upstream' => $upstream, ':downstream' => 0, ':feature_id' => $child->feature_id));
          }
          elseif ($i == 0 and $parent->strand < 0) { // reverse direction
            // -------------------------- ref
            //    ....<----  <----
            //    down  1       2
            $q = chado_query($parent_sql, array(':upstream' => 0, ':downstream' => $downstream, ':feature_id' => $child->feature_id));
          }

          // Next, if the last sub feature we need to include the downstream bases. first check if
          // the feature is in teh forward direction or the reverse
          elseif ($i == $num_children - 1 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //        ---->  ---->....
            //          1       2 down
            $q = chado_query($parent_sql, array(':upstream' => 0, ':downstream' => $downstream, ':feature_id' => $child->feature_id));
          }
          elseif ($i == $num_children - 1 and $parent->strand < 0) { // reverse direction
            // -------------------------- ref
            //        <----  <----....
            //          1       2  up
            $q = chado_query($parent_sql, array(':upstream' => $upstream, ':downstream' => 0, ':feature_id' => $child->feature_id));
          }
          // for internal sub features we don't want upstream or downstream bases
          else {
            $q = chado_query($parent_sql, array(':upstream' => 0, ':downstream' => 0, ':feature_id' => $child->feature_id));
          }
          while ($subseq = $q->fetchObject()) {
            // concatenate the sequences of all the sub features
            if ($subseq->srcfeature_id == $parent->srcfeature_id) {
              $seq .= $subseq->residues;
            }
            if ($subseq->upstream > 0 ) {
              $notes .= "Includes " . $subseq->upstream . " bases upstream.  ";
            }
            if ($subseq->downstream > 0) {
              $notes .= "Includes " . $subseq->downstream . " bases downstream.  ";
            }
          }
          $i++;
        }
      }
      // if this isn't an aggregate then use the parent residues
      else {
         $seq = $parent->residues;
         if ($parent->upstream > 0) {
           $notes .= "Includes " . $parent->upstream . " bases upstream.  ";
         }
         if ($parent->downstream > 0) {
           $notes .= "Includes " . $parent->downstream . " bases downstream.  ";
         }
      }

      // get the reverse compliment if feature is on the reverse strand
      $dir = 'forward';
      $length = strlen($seq);
      if ($parent->strand < 0) {
        $seq = tripal_reverse_compliment_sequence($seq);
        $dir = 'reverse';
      }
      // now format for display
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
        $notes .= "Excludes all bases but those of type(s): " . implode(', ', $types) . ". " ;
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
      $f = chado_generate_var('feature', array('feature_id' => $feature_id));
      $defline = tripal_get_fasta_defline($f, $notes, $featureloc, '', $length);

      $sequences[] = array(
        'types'         => $types,
        'upstream'      => $parent->upstream,
        'downstream'    => $parent->downstream,
        'defline'       => $defline,
        'residues'      => $seq,
        'featureloc_id' => $parent->featureloc_id,
        'length'        => $length,
      );
    }
  }
  // If we are not getting the sequence from the parent sequence then
  // use what comes through from the feature record.
  else {

    $f = chado_generate_var('feature', array('feature_id' => $feature_id));
    $f = chado_expand_var($f, 'field', 'feature.residues');

    $residues = $f->residues;
    $length = strlen($residues);
    if ($is_html) {
      $residues = wordwrap($residues, $num_bases_per_line, "<br>", TRUE);
    }
    else {
      $residues = wordwrap($residues, $num_bases_per_line, "\r\n", TRUE);
    }

    // get the definintion line for this feature
    $defline = tripal_get_fasta_defline($f, '', NULL, '', $length);

    // add to the sequence array
    $sequences[] = array(
      'types'      => $f->type_id->name,
      'upstream'   => 0,
      'downstream' => 0,
      'defline'    => $defline,
      'residues'   => $residues,
      'length'     => $length,
    );
  }

  return $sequences;
}

/**
 *
 * @param $options
 *   An associative array of options for selecting a feature. Valid keys include:
 *    - org_commonname: The common name of the organism for which sequences
 *      should be retrieved
 *    - genus: The genus of the organism for which sequences should be retrieved
 *    - species: The species of the organism for which sequences should be
 *      retrieved
 *    - analysis_name: The name of an analysis to which sequences belong. Only
 *      those that are associated with the analysis will be retrieved.
 *    - type: The type of feature (a sequence ontology term).
 *    - feature_name: the name of the feature. Can be an array of feature names.
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
 *    - relationship_type: If a relationship name is provided (e.g. sequence_of)
 *      then any sequences that are in relationships of this type with matched
 *      sequences are also included
 *    - relationship_part: If a relationship is provided in the preceeding
 *      argument then the rel_part must be either 'object' or 'subject' to
 *      indicate which side of the relationship the matched features belong
 *    - width: Indicate the number of bases to use per line.  A new line will
 *      be added after the specified number of bases on each line.
 *    - is_html: Set to '1' if  the sequence is meant to be displayed on a
 *      web page. This will cause a <br> tag to separate lines of the FASTA
 *      sequence.
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
function tripal_get_bulk_feature_sequences($options) {

  // default values for building the sequence
  $org_commonname     = array_key_exists('org_commonname', $options)     ? $options['org_commonname']    : '';
  $genus              = array_key_exists('genus', $options)              ? $options['genus']             : '';
  $species            = array_key_exists('species', $options)            ? $options['species']           : '';
  $analysis_name      = array_key_exists('analysis_name', $options)      ? $options['analysis_name']     : '';
  $type               = array_key_exists('type', $options)               ? $options['type']              : '';
  $feature_name       = array_key_exists('feature_name', $options)       ? $options['feature_name']      : '';
  $feature_uname      = array_key_exists('feature_uname', $options)      ? $options['feature_uname']     : '';
  $derive_from_parent = array_key_exists('derive_from_parent', $options) ? $options['derive_from_parent'] : 0;
  $aggregate          = array_key_exists('aggregate', $options)          ? $options['aggregate']          : 0;
  $sub_features       = array_key_exists('sub_feature_types', $options)  ? $options['sub_feature_types']  : array();
  $relationship       = array_key_exists('relationship_type', $options)  ? $options['relationship_type']  : '';
  $rel_part           = array_key_exists('relationship_part', $options)  ? $options['relationship_part']  : '';
  $num_bases_per_line = array_key_exists('width', $options)              ? $options['width']              : 50;
  $upstream           = array_key_exists('upstream', $options)           ? $options['upstream']       : 0;
  $downstream         = array_key_exists('downstream', $options)         ? $options['downstream']     : 0;

  if (!$type and !$feature_name and !$genus) {
    print "Please provide a type, feature name or genus\n";
    return;
  }

  // get the list of features
  $vars = array();
  $sql  = "
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
      // remove the trailing comma and close the paren
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
      // remove the trailing comma and close the paren
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

  $sequences = array();
  while ($feature = $q->fetchObject()) {
    // get the sequences
    $seqs = tripal_get_feature_sequences(array('feature_id' => $feature->feature_id), $options);
    $sequences = array_merge($sequences, $seqs);
    $num_seqs++;
  }

  return $sequences;
}

/**
 * Returns a definition line that can be used in a FASTA file
 *
 * @param $feature
 *   A single feature object containing all the fields from the chado.feature table.
 *   Best case is to provide an object generated by the chado_generate_var() function.
 * @param $notes
 *   Optional: additional notes to be added to the definition line
 * @param $featureloc
 *   Optional: a single featureloc object generated using chado_generate_var that
 *   contains a record from the chado.featureloc table. Provide this if the
 *   sequence was obtained by using the alignment rather than from the feature.residues
 *   column
 * @param $type
 *   Optional: the type of sequence. By default the feature type is used.
 * @param $length
 *   Optional: the length of the sequence
 *
 * @return
 *   A string of the format: uniquename|name|type|feature_id
 *   or if an alignment:  srcfeature_name:fmin..fmax[+-]; alignment of uniquename|name|type|feature_id
 */
function tripal_get_fasta_defline($feature, $notes = '', $featureloc = NULL, $type = '', $length = 0) {

  // make sure the featureloc object has the srcfeature if not, then add it
  if ($featureloc) {
    if (!is_object($featureloc->srcfeature_id)) {
      $featureloc->srcfeature_id = chado_generate_var('feature', array('feature_id' => $featureloc->srcfeature_id));
    }
    if (!is_object($featureloc->srcfeature_id->organism_id)) {
      $featureloc->srcfeature_id->organism_id = chado_generate_var('organism', array('organism_id' => $featureloc->srcfeature_id->organism_id));
    }
  }
  // make sure the feature object has the organism if not, then add it
  if (!is_object($feature->organism_id)) {
    $feature->organism_id = chado_generate_var('organism', array('organism_id' => $feature->organism_id));
  }

  // if a type is not provided then use the default type
  if (!$type) {
    $type = $feature->type_id->name;
  }

  // construct the definition line
  $defline = $feature->uniquename . " " .
             'ID=' . $feature->uniquename . "|" .
             'Name=' . $feature->name . "|" .
             'organism=' . $feature->organism_id->genus . " " . $feature->organism_id->species .  "|" .
             'type=' . $type . '|';
  if ($length > 0) {
    $defline .= "length=" . $length . "bp|";
  }
  if ($featureloc) {
    $defline .= "location=Sequence derived from alignment at " . tripal_get_location_string($featureloc);
    $defline .= " (" . $featureloc->srcfeature_id->organism_id->genus . " " . $featureloc->srcfeature_id->organism_id->species . ")|";
  }
  if ($notes) {
    $defline .= "Notes=$notes|";
  }
  $defline = substr($defline, 0, -1); // remove the trailing |
  return $defline;
}

/**
 * Returns a string representing a feature location in an alignment
 *
 * @param unknown $featureloc
 *   A single featureloc object generated using chado_generate_var that
 *   contains a record from the chado.featureloc table.
 */
function tripal_get_location_string($featureloc) {
  $feature = $featureloc->feature_id;

  $strand = '';
  if ($featureloc->strand == 1) {
    $strand = '+';
  }
  elseif ($featureloc->strand == -1) {
    $strand = '-';
  }

  return $featureloc->srcfeature_id->name . ":" . ($featureloc->fmin + 1) . ".." . $featureloc->fmax .  $strand;
}


/**
 * Quickly retrieves relationships for a feature.
 *
 * Using the chado_expand_var function to retrieve a set
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
function tripal_get_feature_relationships($feature) {
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
    'return_array' => 1,
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
//       $sql = "SELECT nid FROM {chado_feature} WHERE feature_id = :feature_id";
//       $n = db_query($sql, array(':feature_id' => $relationship->subject_id->feature_id))->fetchObject();
//       if ($n) {
//         $rel->record->nid = $n->nid;
//       }

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

//       // get the node id of the subject
//       $sql = "SELECT nid FROM {chado_feature} WHERE feature_id = :feature_id";
//       $n = db_query($sql, array(':feature_id' => $relationship->object_id->feature_id))->fetchObject();
//       if ($n) {
//         $rel->record->nid = $n->nid;
//       }

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

/**
 *
 * @param unknown $feature_id
 * @param unknown $featurelocs
 * @return multitype:|Ambigous <multitype:, an>
 */
function chado_get_featureloc_sequences($feature_id, $featurelocs) {

  // if we don't have any featurelocs then no point in continuing
  if (!$featurelocs) {
    return array();
  }

  // Get the list of relationships (including any aggregators) and iterate
  // through each one to find information needed to color-code the reference sequence
  $relationships = _tripal_feature_get_aggregate_relationships($feature_id);
  if (!$relationships) {
    return array();
  }


  // iterate through each of the realtionships features and get their
  // locations
  foreach ($relationships as $rindex => $rel) {
    // get the featurelocs for each of the relationship features
    $rel_featurelocs = chado_get_featurelocs($rel->subject_id, 'as_child', 0);
    foreach ($rel_featurelocs as $rfindex => $rel_featureloc) {
      // keep track of this unique source feature
      $src = $rel_featureloc->src_feature_id . "-" . $rel_featureloc->src_cvterm_id;

      // copy over the results to the relationship object.  Since there can
      // be more than one feature location for each relationship feature we
      // use the '$src' variable to keep track of these.
      $rel->featurelocs = new stdClass();
      $rel->featurelocs->$src = new stdClass();
      $rel->featurelocs->$src->src_uniquename = $rel_featureloc->src_uniquename;
      $rel->featurelocs->$src->src_cvterm_id  = $rel_featureloc->src_cvterm_id;
      $rel->featurelocs->$src->src_cvname     = $rel_featureloc->src_cvname;
      $rel->featurelocs->$src->fmin           = $rel_featureloc->fmin;
      $rel->featurelocs->$src->fmax           = $rel_featureloc->fmax;
      $rel->featurelocs->$src->src_name       = $rel_featureloc->src_name;

      // keep track of the individual parts for each relationship
      $start = $rel->featurelocs->$src->fmin;
      $end   = $rel->featurelocs->$src->fmax;
      $type  = $rel->subject_type;
      $rel_locs[$src]['parts'][$start][$type]['start'] = $start;
      $rel_locs[$src]['parts'][$start][$type]['end']   = $end;
      $rel_locs[$src]['parts'][$start][$type]['type']  = $type;
    }
  }

  // the featurelocs array provided to the function contains the locations
  // where this feature is found.   We want to get the sequence for each
  // location and then annotate it with the parts found from the relationships
  // locations determiend above.
  $floc_sequences = array();
  foreach ($featurelocs as $featureloc) {

    // build the src name so we can keep track of the different parts for each feature
    $src = $featureloc->srcfeature_id->feature_id . "-" . $featureloc->srcfeature_id->type_id->cvterm_id;

    // orient the parts to the beginning of the feature sequence
    if (!empty($rel_locs[$src]['parts'])) {
      $parts = $rel_locs[$src]['parts'];
      $rparts = array();  // we will fill this up if we're on the reverse strand

      foreach ($parts as $start => $types) {
        foreach ($types as $type_name => $type) {
          if ($featureloc->strand >= 0) {
            // this is on the forward strand.  We need to convert the start on the src feature to the
            // start on this feature's sequence
            $parts[$start][$type_name]['start'] = $parts[$start][$type_name]['start'] - $featureloc->fmin;
            $parts[$start][$type_name]['end']   = $parts[$start][$type_name]['end'] - $featureloc->fmin;
            $parts[$start][$type_name]['type']  = $type_name;
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
            $rparts[$new_start][$type_name]['end']   = $new_end;
            $rparts[$new_start][$type_name]['type']  = $type_name;
          }
        }
      }

      // now sort the parts
      // if we're on the reverse strand we need to resort
      if ($featureloc->strand >= 0) {
        usort($parts, 'chado_feature__residues_sort_rel_parts_by_start');
      }
      else {
        usort($rparts, 'chado_feature__residues_sort_rel_parts_by_start');
        $parts = $rparts;
      }

      $floc_sequences[$src]['id'] = $src;
      $floc_sequences[$src]['type'] = $featureloc->feature_id->type_id->name;
      $args = array(':feature_id' => $featureloc->srcfeature_id->feature_id);
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
      $floc_sequences[$src]['residues'] = $residues;
      //$floc_sequences[$src]['formatted_seq'] =  tripal_feature_color_sequence($residues, $parts, $floc_sequences[$src]['defline']);
    }
  }
  return $floc_sequences;
}

/**
 * Get features related to the current feature to a given depth. Recursive function.
 *
 * @param $feature_id
 * @param $substitute
 * @param $levels
 * @param $base_type_id
 * @param $depth
 *
 */
function chado_get_aggregate_feature_relationships($feature_id, $substitute=1,
  $levels=0, $base_type_id=NULL, $depth=0) {

  // we only want to recurse to as many levels deep as indicated by the
  // $levels variable, but only if this variable is > 0. If 0 then we
  // recurse until we reach the end of the relationships tree.
  if ($levels > 0 and $levels == $depth) {
    return NULL;
  }

  // first get the relationships for this feature
  return chado_get_feature_relationships($feature_id, 'as_object');

}

/**
 * Get the relationships for a feature.
 *
 * @param $feature_id
 *   The feature to get relationships for
 * @param $side
 *   The side of the relationship this feature is (ie: 'as_subject' or 'as_object')
 *
 */
function chado_get_feature_relationships($feature_id, $side = 'as_subject') {

  $feature = chado_generate_var('feature', array('feature_id' => $feature_id));

  // get the relationships for this feature.  The query below is used for both
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
  if (strcmp($side, 'as_object')==0) {
    $sql .= " WHERE FR.object_id = :feature_id";
  }
  if (strcmp($side, 'as_subject')==0) {
    $sql .= " WHERE FR.subject_id = :feature_id";
  }
  $sql .= " ORDER BY FR.rank";

  // get the relationships
  $results = chado_query($sql, array(':feature_id' => $feature_id));

  // Get the bundle for this feature type, if one exists.
  $term = tripal_load_term_entity(array(
    'vocabulary' => $feature->type_id->dbxref_id->db_id->name,
    'accession' => $feature->type_id->dbxref_id->accession,
  ));
  $bundle = tripal_load_bundle_entity(array('term_id' => $term->id));

  // iterate through the relationships, put these in an array and add
  // in the Drupal node id if one exists
  $i=0;
  $relationships = array();
  while ($rel = $results->fetchObject()) {

    $entity = chado_get_record_entity_by_bundle($bundle, $rel->subject_id);
    if ($entity) {
      $rel->subject_entity_id = $entity->entity_id;
    }
    $entity = chado_get_record_entity_by_bundle($bundle, $rel->object_id);
    if ($entity) {
      $rel->object_entity_id = $entity->entity_id;
    }
    $relationships[$i++] = $rel;
  }
  return $relationships;
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
 * @ingroup tripal_feature
 */
function chado_get_featurelocs($feature_id, $side = 'as_parent', $aggregate = 1) {

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
  if (strcmp($side, 'as_parent')==0) {
    $sql .= "WHERE FL.srcfeature_id = :feature_id ";
  }
  if (strcmp($side, 'as_child')==0) {
    $sql .= "WHERE FL.feature_id = :feature_id ";
  }

  $flresults = chado_query($sql, array(':feature_id' => $feature_id));

  // copy the results into an array
  $i=0;
  $featurelocs = array();
  while ($loc = $flresults->fetchObject()) {
    // if a drupal entity exists for this feature then add the nid to the
    // results object


    // Get the bundle for this feature type, if one exists.
    $ffeature = chado_generate_var('feature', array('feature_id' => $loc->feature_id));
    $sfeature = chado_generate_var('feature', array('feature_id' => $loc->src_feature_id));
    $fterm = tripal_load_term_entity(array(
      'vocabulary' => $ffeature->type_id->dbxref_id->db_id->name,
      'accession' => $ffeature->type_id->dbxref_id->accession,
    ));
    $sterm = tripal_load_term_entity(array(
      'vocabulary' => $sfeature->type_id->dbxref_id->db_id->name,
      'accession' => $sfeature->type_id->dbxref_id->accession,
    ));

    if($fterm) {
      $fbundle = tripal_load_bundle_entity(array('term_id' => $fterm->id));
      $loc->feid = chado_get_record_entity_by_bundle($fbundle, $loc->feature_id);
    }
    if ($sterm) {
      $sbundle = tripal_load_bundle_entity(array('term_id' => $sterm->id));
      $loc->seid = chado_get_record_entity_by_bundle($sbundle, $loc->src_feature_id);
    }
    // add the result to the array
    $featurelocs[$i++] = $loc;
  }

  // Add the relationship feature locs if aggregate is turned on
  if ($aggregate and strcmp($side, 'as_parent')==0) {
  // get the relationships for this feature without substituting any children
  // for the parent. We want all relationships
  $relationships = _tripal_feature_get_aggregate_relationships($feature_id, 0);
  foreach ($relationships as $rindex => $rel) {
    // get the featurelocs for each of the relationship features
    $rel_featurelocs = tripal_feature_load_featurelocs($rel->subject_id, 'as_child', 0);
      foreach ($rel_featurelocs as $findex => $rfloc) {
        $featurelocs[$i++] = $rfloc;
      }
    }
  }

  usort($featurelocs, 'chado_feature__residues_sort_locations');
  return $featurelocs;
}

/**
 * Get features related to the current feature to a given depth. Recursive function.
 *
 * @param $feature_id
 * @param $substitute
 * @param $levels
 * @param $base_type_id
 * @param $depth
 *
 */
function _tripal_feature_get_aggregate_relationships($feature_id, $substitute=1,
    $levels=0, $base_type_id=NULL, $depth=0) {

  // we only want to recurse to as many levels deep as indicated by the
  // $levels variable, but only if this variable is > 0. If 0 then we
  // recurse until we reach the end of the relationships tree.
  if ($levels > 0 and $levels == $depth) {
    return NULL;
  }

  // first get the relationships for this feature
  return _tripal_feature_load_relationships($feature_id, 'as_object');

}

/**
 * Get the relationships for a feature.
 *
 * @param $feature_id
 *   The feature to get relationships for
 * @param $side
 *   The side of the relationship this feature is (ie: 'as_subject' or 'as_object')
 *
 */
function _tripal_feature_load_relationships($feature_id, $side = 'as_subject') {
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
  if (strcmp($side, 'as_object')==0) {
    $sql .= " WHERE FR.object_id = :feature_id";
  }
  if (strcmp($side, 'as_subject')==0) {
    $sql .= " WHERE FR.subject_id = :feature_id";
  }
  $sql .= " ORDER BY FR.rank";

  // Get the relationships.
  $results = chado_query($sql, array(':feature_id' => $feature_id));

  // Iterate through the relationships, put these in an array and add
  // in the Drupal node id if one exists.
  $i=0;
  $nodesql = "SELECT nid FROM {chado_feature} WHERE feature_id = :feature_id";
  $relationships = array();
  while ($rel = $results->fetchObject()) {
    $node = db_query($nodesql, array(':feature_id' => $rel->subject_id))->fetchObject();
    if ($node) {
      $rel->subject_nid = $node->nid;
    }
    $node = db_query($nodesql, array(':feature_id' => $rel->object_id))->fetchObject();
    if ($node) {
      $rel->object_nid = $node->nid;
    }
    $relationships[$i++] = $rel;
  }
  return $relationships;
}
/**
  * Used to sort the list of relationship parts by start position
  *
  * @ingroup tripal_feature
  */
function chado_feature__residues_sort_rel_parts_by_start($a, $b) {
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
 * @ingroup tripal_feature
 */
function chado_feature__residues_sort_locations($a, $b) {
  return strnatcmp($a->fmin, $b->fmin);
}

