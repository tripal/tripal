<?php 

function tripal_feature_gff3_exporter($source, $filters) {
  
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

  $args = array();
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
    $where .= "AND CVT.name IN (:types) ";
    $args[':types'] = $filters['types'];
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
  
  // The SQL statement for Dbxrefs
  $dbxref_sql = "
    SELECT DB.name, DBX.accession
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
  
  // The SQL statement for CVTerms
  $cvterm_sql = "
    SELECT CV.name as db_name, DBX.accession
    FROM {feature_cvterm} FCVT
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = FCVT.cvterm_id
      INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
      INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
    WHERE FCVT.feature_id = :feature_id
  ";
  $results = chado_query($sql, $args);
  while ($line = $results->fetchObject()) {
    print $line->landmark_uname . "\t";
    print $source . "\t";
    print $line->type . "\t";
    print $line->fmin . "\t";
    print $line->fmax . "\t";
    print "." . "\t";
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
    
    $props = chado_query($props_sql, array(':feature_id' => $line->feature_id));
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
    $args = array(
      ':feature_id' => $line->feature_id,
      ':dbxref_id' => $line->dbxref_id,
    );
    $dbxrefs = chado_query($dbxref_sql, $args);
    $xrefs = '';
    while ($dbxref = $dbxrefs->fetchObject()) {
      $xrefs .= $dbxref->name . ":" . $dbxref->accession . ",";
    }
    if ($xrefs) {
      print "Dbxref=" . substr($xrefs, 0, -1) . ";";
    }
    
    // Add in any CVTerm records
    $args = array(
      ':feature_id' => $line->feature_id,
    );
    $cvterms = chado_query($cvterm_sql, $args);
    $xrefs = '';
    while ($cvterm = $cvterms->fetchObject()) {
      $xrefs .= $cvterm->db_name . ":" . $cvterm->accession . ",";
    }
    if ($xrefs) {
      print "Ontology_term=" . substr($xrefs, 0, -1) . ";";
    }
    
    print "\n";
  }
}