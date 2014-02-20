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
 * @param $options
 *   An associative array of options. Valid keys include:
 *    - width: Indicate the number of bases to use per line.  A new line will be added
 *      after the specified number of bases on each line.
 *    - derive_from_parent: Set to '1' if the sequence should be obtained from the parent
 *      to which this feature is aligned.
 *    - aggregate: Set to '1' if the sequence should only contain sub features, excluding
 *      intro sub feature sequence.  For example, set this option to obtain just
 *      the coding sequence of an mRNA.
 *    - output_format: The type of format.  Valid formats include 'fasta_html', 'fasta_txt' and
 *      'raw'.  The format 'fasta_txt' outputs line breaks as <br> tags and the entire
 *      return value is in a <span> tag with a fixed-width font definition.  'fasta_txt'
 *      outputs line breaks with windows format carriage returns (e.g. \r\n) with no other
 *      formatting. The raw format is simply the sequence with now FASTA formatting and no
 *      line breaks.
 *    - num_upstream: An integer specifing the number of upstream bases to include in the output
 *    - num_downstream: An integer specifying the number of downstream bases to include in the
 *      output.
 *    - sub_feature_types: Only include sub features (or child features) of the types
 *      provided in the array
 *    - relationship_type: If a relationship name is provided (e.g. sequence_of) then any
 *      sequences that are in relationships of this type with matched sequences are also included
 *    - relationship_part: If a relationship is provided in the preceeding argument then
 *      the rel_part must be either 'object' or 'subject' to indicate which side of the
 *      relationship the matched features belong
 *
 * @return
 *   The DNA/protein sequence formated as requested.
 *
 * @ingroup tripal_feature_api
 */
function tripal_format_sequence($feature, $options) {

  // Default Values
  $feature_id = $feature['feature_id'];
  $feature_name = $feature['name'];

  $num_bases_per_line = $options['width'];
  $derive_from_parent = $options['derive_from_parent'];
  $aggregate = $options['aggregate'];
  $output_format = $options['output_format'];
  $upstream = $options['num_upstream'];
  $downstream = $options['num_downstream'];
  $sub_features = $options['sub_feature_types'];
  $relationship = $options['relationship_type'];
  $rel_part = $options['relationship_part'];

  // to speed things up we need to make sure we have a persistent connection
  $connection = tripal_db_persistent_chado();

  if (!$upstream) {
     $upstream = 0;
  }
  if (!$downstream) {
     $downstream = 0;
  }

  if ($rel_part == "object" or $rel_part == "subject") {
    if ($rel_part == "subject") {
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
        tripal_report_error('tripal_feature', TRIPAL_ERROR, "init: not able to prepare SQL statement '%name'",
          array('%name' => 'feature_by_subject'));
      }
      $sql = "EXECUTE feature_rel_get_object(:feature_id, :relationship)";
      $features = chado_query($sql, array(':feature_id' => $feature_id, ':relationship' => $relationship));
    }
    if ($rel_part == "object") {
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
        tripal_report_error('tripal_feature', TRIPAL_ERROR, "init: not able to prepare SQL statement '%name'",
          array('%name' => 'feature_by_object'));
      }
      $sql = "EXECUTE feature_rel_get_subject(:feature_id, :relationship)";
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
      $sequences .= tripal_feature_get_formatted_sequence($feature->feature_id, $defline,
        $num_bases_per_line, $derive_from_parent, $aggregate, $output_format,
        $upstream, $downstream, $sub_features, '', '');
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
          INNER JOIN {feature} SF   on FL.feature_id    = SF.feature_id
          INNER JOIN {cvterm}  SCVT on SF.type_id       = SCVT.cvterm_id
          INNER JOIN {feature} OF   on FL.srcfeature_id = OF.feature_id
          INNER JOIN {cvterm}  OCVT on OF.type_id       = OCVT.cvterm_id
          INNER JOIN {organism} OO  on OF.organism_id   = OO.organism_id
        WHERE SF.feature_id = $3 and NOT (OF.residues = \'\' or OF.residues IS NULL)) as tbl1
    ';
    $status = tripal_core_chado_prepare('sequence_by_parent', $psql, array('int', 'int', 'int'));
    if (!$status) {
      tripal_report_error('tripal_feature', TRIPAL_ERROR,
        "init: not able to prepare SQL statement '%name'",
        array('%name' => 'sequence_by_parent'));
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
      tripal_report_error('tripal_feature', TRIPAL_ERROR,
        "init: not able to prepare SQL statement '%name'",
        array('%name' => 'ssub_features'));
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
      tripal_report_error('tripal_feature', TRIPAL_ERROR,
        "init: not able to prepare SQL statement '%name'",
        array('%name' => 'count_sub_features'));
    }
  }

  // if we need to get the sequence from the parent then do so now.
  if ($derive_from_parent) {

    // execute the query to get the sequence from the parent
    $sql = "EXECUTE sequence_by_parent (:upstream, :downstream, :feature_id)";
    $parents = chado_query($sql, array(':uptream' => $upstream, ':downstream' => $downstream, ':feature_id' => $feature_id));

    while ($parent = $parents->fetchObject()) {
      $seq = '';  // initialize the sequence for each parent

      // if we are to aggregate then we will ignore the feature returned
      // by the query above and rebuild it using the sub features
      if ($aggregate) {

        // now get the sub features that are located on the parent.
        $sql = "EXECUTE sub_features (:feature_id, :srcfeature_id)";
        $children = chado_query($sql, array(':feature_id' => $feature_id, ':srcfeature_id' => $parent->srcfeature_id));
        $sql = "EXECUTE count_sub_features (:feature_id, :srcfeature_id)";
        $sub_features = chado_query($sql, array(':feature_id' => $feature_id, ':srcfeature_id' => $parent->srcfeature_id));
        $num_children = $sub_features->fetchObject();

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

          $sql = "EXECUTE sequence_by_parent (:upstream, %d, :feature_id)";

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
            $sql = "EXECUTE sequence_by_parent (%d, %d, %d)";
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
      $residues .= ">$feature_name. Sequence derived from feature of type, '$parent->srctypename', of $parent->genus $parent->species: $parent->srcname:" . ($parent->adjfmin + 1) . ".." . $parent->adjfmax . " ($dir). ";
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
    $sql = "SELECT * FROM {feature} F WHERE feature_id = :feature_id";
    $values = chado_query($sql, array(':feature_id' => $feature_id))->fetchObject();
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
function tripal_get_fasta_sequence($feature, $desc) {

  $fasta  = ">" . variable_get('chado_feature_accession_prefix', 'FID') . "$feature->feature_id|$feature->name";
  $fasta .= " $desc\n";
  $fasta .= wordwrap($feature->residues, 50, "\n", TRUE);
  $fasta .= "\n\n";

  return $fasta;

}