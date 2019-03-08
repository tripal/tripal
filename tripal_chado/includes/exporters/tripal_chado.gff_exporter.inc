<?php

function tripal_chado_gff3_exporter($source, $filters) {

  // Make sure the filters array is setup properly.
  if (!array_key_exists('types', $filters) and
    !array_key_exists('feature_id', $filters)) {
    tripal_report_error("tripal_chado", TRIPAL_ERROR,
      "Please provide one or more types to include in the exported GFF file.",
      []);
    exit;
  }

  $select = "
    SELECT SF.uniquename as landmark_uname, SF.name as landmark_name,
      F.feature_id, F.dbxref_id, F.uniquename, F.name, CVT.name as type,
      FL.fmin, FL.fmax, FL.strand, FL.phase
  ";
  $from = "
    FROM {featureloc} FL
      INNER JOIN {feature} F on FL.feature_id = F.feature_id
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = F.type_id
      INNER JOIN {feature} SF on FL.srcfeature_id = SF.feature_id
  ";
  $where = "
    WHERE 1=1
  ";
  $order = "
    ORDER BY SF.uniquename, FL.fmin
  ";

  $args = [];

  // Filter by organism
  if (array_key_exists('genus', $filters) or array_key_exists('species', $filters)) {
    $from .= "INNER JOIN {organism} O on F.organism_id = O.organism_id ";
    if (array_key_exists('genus', $filters)) {
      $where .= "AND O.genus = :genus ";
      $args[':genus'] = $filters['genus'];
    }
    if (array_key_exists('species', $filters)) {
      $where .= "AND O.species = :species ";
      $args[':species'] = $filters['species'];
    }
  }

  if (array_key_exists('types', $filters)) {
    if (is_array($filters['types'])) {
      $where .= "AND CVT.name IN (:types) ";
      $args[':types'] = $filters['types'];
    }
    else {
      tripal_report_error("tripal_chado", TRIPAL_ERROR,
        "The 'types' element of the filters array must be an array.",
        []);
      exit;
    }
  }

  // Filter by exact feature_id
  if (array_key_exists('feature_id', $filters)) {
    $where .= "AND F.feature_id = :feature_id ";
    $args[':feature_id'] = $filters['feature_id'];
  }
  // Filter by analysis
  if (array_key_exists('analysis_id', $filters) or
    array_key_exists('analysis_name', $filters)) {
    $from .= "INNER JOIN {analysisfeature} AF on AF.feature_id = F.feature_id ";
    if (array_key_exists('analysis_id', $filters)) {
      $where .= "AND AF.analysis_id = :analysis_id ";
      $args[':analysis_id'] = $filters['analysis_id'];
    }
    if (array_key_exists('analysis_name', $filters)) {
      $from .= "INNER JOIN {analysis} A on AF.analysis_id = A.analysis_id ";
      $where .= "AND A.name = :analysis_name ";
      $args[':analysis_name'] = $filters['analysis_name'];
    }
  }

  $sql = "$select $from $where $order";

  // The SQL statement for feature properties.
  $props_sql = "
    SELECT CVT.name, FP.value
    FROM {featureprop} FP
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = FP.type_id
    WHERE FP.feature_id = :feature_id
    ORDER BY CVT.name
  ";

  // The SQL statement for Dbxrefs associated with this feature
  $dbxref_sql = "
    SELECT DB.name as dbname, DBX.accession
    FROM {dbxref} DBX
      INNER JOIN {db} DB on DB.db_id = DBX.db_id
    WHERE DBX.dbxref_id = :dbxref_id
    UNION
    SELECT DB.name, DBX.accession
    FROM {feature_dbxref} FDBX
      INNER JOIN {dbxref} DBX on DBX.dbxref_id = FDBX.dbxref_id
      INNER JOIN {db} DB on DB.db_id = DBX.db_id
    WHERE FDBX.feature_id = :feature_id
  ";

  // The SQL statement for CVTerms assigned to this feature.
  $cvterm_sql = "
    SELECT CV.name as db_name, DBX.accession
    FROM {feature_cvterm} FCVT
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = FCVT.cvterm_id
      INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
      INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
    WHERE FCVT.feature_id = :feature_id
  ";

  // The SQL for finding the parents of this feature.
  $parent_sql = "
    SELECT F.name, F.uniquename, F.feature_id
    FROM {feature_relationship} FR
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = FR.type_id
      INNER JOIN {feature} F on FR.object_id = F.feature_id
    WHERE CVT.name = 'part_of' AND FR.subject_id = :feature_id
  ";

  // The SQL for aliases of this feature.
  $alias_sql = "
    SELECT S.name
    FROM {feature_synonym} FS
      INNER JOIN {synonym} S on FS.synonym_id = S.synonym_id
    WHERE FS.feature_id = :feature_id
  ";

  // Get the score
  $score_sql = "
    SELECT rawscore as score
      FROM {analysisfeature} AF
    WHERE feature_id = :feature_id
    ORDER BY AF.analysis_id
    OFFSET 0 LIMIT 1
  ";

  $results = chado_query($sql, $args);
  while ($line = $results->fetchObject()) {

    // Get the score for this feature
    $score = chado_query($score_sql, [':feature_id' => $line->feature_id])->fetchField();
    print $line->landmark_uname . "\t";
    print $source . "\t";
    print $line->type . "\t";
    print ($line->fmin + 1) . "\t";
    print $line->fmax . "\t";

    if ($score) {
      print $score . "\t";
    }
    else {
      print "." . "\t";
    }
    if ($line->strand) {
      print $line->strand . "\t";
    }
    else {
      print '.' . "\t";
    }
    if ($line->phase) {
      print $line->phase . "\t";
    }
    else {
      print '.' . "\t";
    }
    print "ID=" . $line->uniquename . ";Name=" . $line->name . ";";

    // Look for a parent of this feature
    $args = [':feature_id' => $line->feature_id];
    $parents = chado_query($parent_sql, $args);
    $attr = '';
    while ($parent = $parents->fetchObject()) {
      $attr .= $parent->uniquename . ",";
    }
    if ($attr) {
      print "Parent=" . substr($attr, 0, -1) . ";";
    }

    // Add in any aliases
    $args = [':feature_id' => $line->feature_id];
    $aliases = chado_query($alias_sql, $args);
    $attr = '';
    while ($alias = $aliases->fetchObject()) {
      $attr .= $alias->name . ",";
    }
    if ($attr) {
      print "Alias=" . substr($attr, 0, -1) . ";";
    }

    $props = chado_query($props_sql, [':feature_id' => $line->feature_id]);
    $prop_name = '';
    while ($prop = $props->fetchobject()) {
      // If this is the first time we've encountered this property then
      // add the name=value key pair.
      if ($prop_name != $prop->name) {
        if ($prop_name) {
          // End the previous property
          print ";";
        }
        // TODO: urlencode the properties
        print $prop->name . "=" . $prop->value;
        $prop_name = $prop->name;
      }
      // If we've seen this property before then just add the value.
      else {
        print "," . $prop->value;
      }
    }
    // End the last property
    if ($prop_name) {
      print ";";
    }

    // Add in any DBXref records
    $args = [
      ':feature_id' => $line->feature_id,
      ':dbxref_id' => $line->dbxref_id,
    ];
    $dbxrefs = chado_query($dbxref_sql, $args);
    $xrefs = '';
    while ($dbxref = $dbxrefs->fetchObject()) {
      if ($dbxref->dbname = 'GFF_source') {
        continue;
      }
      $xrefs .= $dbxref->dbname . ":" . $dbxref->accession . ",";
    }
    if ($xrefs) {
      print "Dbxref=" . substr($xrefs, 0, -1) . ";";
    }

    // Add in any CVTerm records
    $args = [':feature_id' => $line->feature_id];
    $cvterms = chado_query($cvterm_sql, $args);
    $xrefs = '';
    while ($cvterm = $cvterms->fetchObject()) {
      $xrefs .= $cvterm->db_name . ":" . $cvterm->accession . ",";
    }
    if ($xrefs) {
      print "Ontology_term=" . substr($xrefs, 0, -1) . ";";
    }

    print "\n";

    // Look for children of this feature and recursively add them.
    $children_sql = "
      SELECT FR.subject_id
      FROM {feature_relationship} FR
        INNER JOIN {cvterm} CVT on CVT.cvterm_id = FR.type_id
      WHERE CVT.name = 'part_of' AND FR.object_id = :feature_id
    ";
    $children = chado_query($children_sql, [':feature_id' => $line->feature_id]);
    while ($child = $children->fetchObject()) {
      $child_filters = [
        'feature_id' => $child->subject_id,
      ];
      tripal_chado_gff3_exporter($source, $child_filters);
    }
  }
}