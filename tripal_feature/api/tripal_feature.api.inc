<?php
/**
 * @file
 * Provides an application programming interface (API) for working with features
 */

/**
 * @defgroup tripal_feature_api Feature API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with features
 * @}
 */

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
 * Retrieves the sequence for a feature.
 *
 * @param $feature
 *   An associative array describing the feature. Valid keys include:
 *    - feature_id: The feature_id of the feature for which the sequence will be retrieved
 *    - name: The feature name. This will appear on the FASTA definition line
 *    - parent_id:  (optional) only retrieve a sequence if 'derive_from_parent' is true 
 *        and the parent matches this ID.
 *    - featureloc_id: (optional) only retrieve a sequence if 'derive_from_parent' is 
 *        true and the alignment is defined with this featureloc_id
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - width: Indicate the number of bases to use per line.  A new line will be added
 *        after the specified number of bases on each line.
 *    - derive_from_parent: Set to '1' if the sequence should be obtained from the parent
 *        to which this feature is aligned.
 *    - aggregate: Set to '1' if the sequence should only contain sub features, excluding
 *        intro sub feature sequence.  For example, set this option to obtain just
 *        the coding sequence of an mRNA.
 *    - output_format: The type of format.  Valid formats include 'fasta_html', 'fasta_txt' and
 *        'raw'.  The format 'fasta_txt' outputs line breaks as <br> tags and the entire
 *        return value is in a <span> tag with a fixed-width font definition.  'fasta_txt'
 *        outputs line breaks with windows format carriage returns (e.g. \r\n) with no other
 *        formatting. The raw format is simply the sequence with no FASTA formatting and no
 *        line breaks.
 *    - num_upstream: An integer specifing the number of upstream bases to include in the output
 *    - num_downstream: An integer specifying the number of downstream bases to include in the
 *        output.
 *    - sub_feature_types: Only include sub features (or child features) of the types
 *        provided in the array
 *    - relationship_type: If a relationship name is provided (e.g. sequence_of) then any
 *        sequences that are in relationships of this type with matched sequences are also included
 *    - relationship_part: If a relationship is provided in the preceeding argument then
 *        the rel_part must be either 'object' or 'subject' to indicate which side of the
 *        relationship the matched features belong
 *
 * @return
 *   an array of matching sequence formated as requested.
 *
 * @ingroup tripal_feature_api
 */
function tripal_get_sequence($feature, $options) {

  // default values for finding the feature
  $feature_id    = array_key_exists('feature_id', $feature)     ? $feature['feature_id']     : 0;
  $parent_id     = array_key_exists('parent_id', $feature)      ? $feature['parent_id']      : 0;
  $featureloc_id = array_key_exists('featureloc_id', $feature)  ? $feature['featureloc_id']  : 0;
  $feature_name  = array_key_exists('name', $feature)           ? $feature['name']           : '';

  // default values for building the sequence
  $num_bases_per_line = array_key_exists('width', $options)              ? $options['width']              : 50;
  $derive_from_parent = array_key_exists('derive_from_parent', $options) ? $options['derive_from_parent'] : 0;
  $aggregate          = array_key_exists('aggregate', $options)          ? $options['aggregate']          : 0;
  $output_format      = array_key_exists('output_format', $options)      ? $options['output_format']      : 'fasta_txt';
  $upstream           = array_key_exists('num_upstream', $options)       ? $options['num_upstream']       : 0;
  $downstream         = array_key_exists('num_downstream', $options)     ? $options['num_downstream']     : 0;
  $sub_features       = array_key_exists('sub_feature_types', $options)  ? $options['sub_feature_types']  : array();
  $relationship       = array_key_exists('relationship_type', $options)  ? $options['relationship_type']  : '';
  $rel_part           = array_key_exists('relationship_part', $options)  ? $options['relationship_part']  : '';
  
  // make sure the sub_features variable is an array
  if (!is_array($sub_features)) {
    tripal_report_error('tripal_deprecated', TRIPAL_ERROR,
      "'sub_features' option must be an array for function tripal_get_sequence().",
      array()
    );
    return array();
  }

  // if a relationship was specified then retreive and the sequences that
  // have the given relationship and the recurse to extract the appropriate 
  // sequence
  if ($rel_part == "object" or $rel_part == "subject") {
    if ($rel_part == "subject") {
      $sql = '
        SELECT FO.feature_id, FO.name, FO.uniquename, CVTO.name as feature_type, O.genus, O.species
        FROM feature FS
          INNER JOIN feature_relationship FR ON FR.subject_id   = FS.feature_id
          INNER JOIN cvterm CVTFR            ON CVTFR.cvterm_id = FR.type_id
          INNER JOIN feature FO              ON FO.feature_id   = FR.object_id
          INNER JOIN cvterm CVTO             ON CVTO.cvterm_id  = FO.type_id
          INNER JOIN organism O              ON O.organism_id   = FO.organism_id
        WHERE
          FS.feature_id = :feature_id AND
          CVTFR.name    = :relationship
      ';
      $features = chado_query($sql, array(':feature_id' => $feature_id, ':relationship' => $relationship));
    }
    if ($rel_part == "object") {
      $sql = '
        SELECT FS.feature_id, FS.name, FS.uniquename, CVTO.name as feature_type, O.genus, O.species
        FROM feature FO
          INNER JOIN feature_relationship FR ON FR.object_id    = FO.feature_id
          INNER JOIN cvterm CVTFR            ON CVTFR.cvterm_id = FR.type_id
          INNER JOIN feature FS              ON FS.feature_id   = FR.subject_id
          INNER JOIN cvterm CVTO             ON CVTO.cvterm_id  = FS.type_id
          INNER JOIN organism O              ON O.organism_id   = FS.organism_id
        WHERE
          FO.feature_id = :feature_id AND
          CVTFR.name    = :relationship
      ';
      $features = chado_query($sql, array(':feature_id' => $feature_id, ':relationship' => $relationship));
    }
    $sequences = '';
    while ($feature = $features->fetchObject()) {

      // recurse and get the sequences for these in the relationship
      if ($rel_part == "subject") {
        $defline = "$feature_name, $relationship, $feature->uniquename $feature->feature_type ($feature->genus $feature->species)";
      }
      if ($rel_part == "object") {
        $defline = "$feature->uniquename $feature->feature_type ($feature->genus $feature->species), $relationship, $feature_name";
      }
      return tripal_get_sequence(
        array(
          'feature_id' => $feature->feature_id, 
          'name' => $defline,
          'parent_id' => $parent_id,
        ),
        array(
          'width' => $num_bases_per_line, 
          'derive_from_pareht' => $derive_from_parent, 
          'aggregate' => $aggregate, 
          'output_format' => $output_format,
          'upstream' => $upstream, 
          'downstream' => $downstream, 
          'sub_features' => $sub_features, 
        )
      );
    }
  }

  // prepare the queries we're going to use later during the render phase
  // This SQL statement uses conditionals in the select clause to handle
  // cases cases where the alignment is in the reverse direction and when
  // the upstream and downstream extensions go beyond the lenght of the
  // parent sequence.
  $sql ='
    SELECT featureloc_id, srcname, srcfeature_id, strand, srctypename, typename,
      fmin, fmax, upstream, downstream, adjfmin, adjfmax,
      substring(residues from (adjfmin + 1) for (upstream + (fmax - fmin) + downstream))  as residues,
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
  // this query is meant to get all of the sub features of any given
  // feature (arg #1) and order them as they appear on the reference
  // feature (arg #2).
  $sfsql = '
    SELECT SF.feature_id, CVT.name as type_name, SF.type_id
    FROM {feature_relationship} FR
      INNER JOIN {feature} SF on SF.feature_id = FR.subject_id
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = SF.type_id
      INNER JOIN {featureloc} FL on FL.feature_id = FR.subject_id
      INNER JOIN {feature} PF on PF.feature_id = FL.srcfeature_id
    WHERE FR.object_id = :feature_id and PF.feature_id = :srcfeature_id
    ORDER BY FL.fmin ASC
  ';
  // for counting the number of children
  $fsql ='
    SELECT count(*) as num_children
    FROM {feature_relationship} FR
      INNER JOIN {feature} SF on SF.feature_id = FR.subject_id
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = SF.type_id
      INNER JOIN {featureloc} FL on FL.feature_id = FR.subject_id
      INNER JOIN {feature} PF on PF.feature_id = FL.srcfeature_id
    WHERE FR.object_id = :feature_id and PF.feature_id = :srcfeature_id
  ';

  // the array to be returned
  $sequences = array();
  
  // if we need to get the sequence from the parent then do so now.
  if ($derive_from_parent) {

    // execute the query to get the sequence from the parent
    $parents = chado_query($sql, array(':upstream' => $upstream, ':downstream' => $downstream, ':feature_id' => $feature_id));

    while ($parent = $parents->fetchObject()) {
      
      // if the user specified a particular parent and this one doesn't match then skip it
      if ($parent_id and $parent_id != $parent->srcfeature_id) {
        continue;
      }
      // if the user specified a particular featureloc_id and this one doesn't match then skip it
      if ($featureloc_id and $featureloc_id != $parent->featureloc_id) {
        continue;
      }
      $seq = '';  // initialize the sequence for each parent

      // if we are to aggregate then we will ignore the feature returned
      // by the query above and rebuild it using the sub features
      if ($aggregate) {

        // now get the sub features that are located on the parent.
        $children = chado_query($sfsql, array(':feature_id' => $feature_id, ':srcfeature_id' => $parent->srcfeature_id));
        $num_children = chado_query($fsql, array(':feature_id' => $feature_id, ':srcfeature_id' => $parent->srcfeature_id))->fetchObject();

        // iterate through the sub features and concat their sequences. They
        // should already be in order.
        $types = array();
        $i = 0;
        while ($child = $children->fetchObject()) {
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

          // if the first sub feature we need to include the upstream bases. first check if
          // the feature is in the foward direction or the reverse.
          if ($i == 0 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //    ....---->  ---->
            //     up    1       2
            $q = chado_query($sql, array(':upstream' => $upstream, ':downstream' => 0, ':feature_id' => $child->feature_id));
          }
          elseif ($i == 0 and $parent->strand < 0) { // reverse direction
            // -------------------------- ref
            //    ....<----  <----
            //    down  1       2
            $q = chado_query($sql, array(':upstream' => 0, ':downstream' => $downstream, ':feature_id' => $child->feature_id));
          }

          // Next, if the last sub feature we need to include the downstream bases. first check if
          // the feature is in teh forward direction or the reverse
          if ($i == $num_children->num_children - 1 and $parent->strand >= 0) {  // forward direction
            // -------------------------- ref
            //        ---->  ---->....
            //          1       2 down
            $q = chado_query($sql, array(':upstream' => 0, ':downstream' => $downstream, ':feature_id' => $child->feature_id));
          }
          elseif ($i == $num_children->num_children - 1 and $parent->strand < 0) { // reverse direction
            // -------------------------- ref
            //        <----  <----....
            //          1       2  up
            $q = chado_query($sql, array(':upstream' => $upstream, ':downstream' => 0, ':feature_id' => $child->feature_id));
          }

          // for internal sub features we don't want upstream or downstream bases
          else {
            $q = chado_query($sql, array(':upstream' => 0, ':downstream' => 0, ':feature_id' => $child->feature_id));
          }

          while ($subseq = $q->fetchObject()) {
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
      if (!$seq) {
        if ($output_format == 'fasta_html') {
          $notes .= "No sequence available.</br>";
        }
        else {
          $notes .= "No sequence available.\r\n";
        }
      }
      
      $notes = "Sequence derived from feature of type, '$parent->srctypename', of $parent->genus $parent->species: $parent->srcname:" . ($parent->adjfmin + 1) . ".." . $parent->adjfmax . " ($dir). ";
      /*
      if (count($types) > 0) {
        $notes .= "Excludes all bases but those of type(s): " . implode(', ', $types) . ". " ;
      }
      if ($parent->upstream > 0) {
         $notes .= "Includes " . $parent->upstream . " bases upstream.  ";
      }
      if ($parent->downstream > 0) {
         $notes .= "Includes " . $parent->downstream . " bases downstream.  ";
      }      
      */
      $sequences[] = array(
        'types'         => $types,
        'upstream'      => $parent->upstream,
        'downstream'    => $parent->downstream,
        'notes'         => $notes,
        'residues'      => $seq,
        'featureloc_id' => $parent->featureloc_id,
      );
    }
  }
  // if we are not getting the sequence from the parent sequence then
  // use what comes through from the feature record
  else {
    $sql = "SELECT * FROM {feature} F WHERE feature_id = :feature_id";
    $values = chado_query($sql, array(':feature_id' => $feature_id))->fetchObject();
    $residues = $values->residues;
    if ($output_format == 'fasta_html') {
       $residues = wordwrap($residues, $num_bases_per_line, "<br>", TRUE);
    }
    elseif ($output_format == 'fasta_txt') {
       $residues = wordwrap($residues, $num_bases_per_line, "\r\n", TRUE);
    }

    $sequences[] = array(
      'types'      => $values->type,
      'upstream'   => 0,
      'downstream' => 0,
      'notes'      => '',
      'residues'   => $residues,
    );
  }

  return $sequences;
}

/**
 * Returns a fasta record for the passed in feature
 *
 * @param $feature
 *   A single feature object containing all the fields from the chado.feature table
 * @param $desc
 *   A string containing any additional details you want added to the definition line of
 *   the fasta record.
 *
 * Note: the feature accession and name separated by a | will be added
 *   before the description but on the same line
 *
 * @ingroup tripal_feature_api
 */
function tripal_format_fasta_sequence($feature, $desc) {

  $fasta  = ">" . variable_get('chado_feature_accession_prefix', 'FID') . "$feature->feature_id|$feature->name";
  $fasta .= " $desc\n";
  $fasta .= wordwrap($feature->residues, 50, "\n", TRUE);
  $fasta .= "\n\n";

  return $fasta;

}

/**
 * Returns a definition line that can be used in a FASTA file
 * 
 * @param $feature
 *   A single feature object containing all the fields from the chado.feature table
 * @param $featureloc
 *   Optional: A single featureloc object generated using chado_generate_var that
 *   contains a record from the chado.featureloc table.
 * @param $type
 *   Optional: the type of sequence. By default the feature type is used.
 *   
 * @return
 *   A string of the format: uniquename|name|type|feature_id
 *   or if an alignment:  srcfeature_name:fmin..fmax[+-]; alignment of uniquename|name|type|feature_id 
 */
function tripal_get_fasta_defline($feature, $featureloc = NULL, $type = '') {
  
  if (!$type) {
    $type = $feature->type_id->name;
  }
  $defline = $feature->uniquename . "|" . $feature->name . "|" . $type . "|" . $feature->feature_id; 
  if ($featureloc) {
    $defline = $defline . "; derived from alignment at " .tripal_get_location_string($featureloc);
  }
  
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
  
  if ($featureloc->strand < 0) {
    $residues = tripal_feature_reverse_complement($residues);
  }
  $strand = '.';
  if ($featureloc->strand == 1) {
    $strand = '+';
  }
  elseif ($featureloc->strand == -1) {
    $strand = '-';
  }
  
  return $featureloc->srcfeature_id->name . ":" . ($featureloc->fmin + 1) . ".." . $featureloc->fmax .  $strand;  
}