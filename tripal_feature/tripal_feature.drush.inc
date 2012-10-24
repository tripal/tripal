<?php

/**
 * @file
 * Contains function relating to drush-integration of this module.
 */

/**
 * Describes each drush command implemented by the module
 *
 * @return
 *   The first line of description when executing the help for a given command
 */
function tripal_feature_drush_help($command) {
  switch ($command) {
    case 'drush:tripal-get_sequence':
      return dt('Prints sequences that match specified categories.');
  }
}

/**
 * Registers a drush command and constructs the full help for that command
 *
 * @return
 *   And array of command descriptions
 */
function tripal_feature_drush_command() {
  $items = array();
  $items['tripal-get-sequence'] = array(
    'description' => dt('Prints sequences that match specified categories.'),
    'options'   => array(
      'org'    => dt('The organism\'s common name. If specified, features for this organism will be retrieved.'),
      'analysis' => dt('The analysis name. If specified, features for this analysis will be retrieved.'),
      'type'   => dt('The type of feature to retrieve (e.g. mRNA). All features that match this type will be retrieved.'),
      'name'   => dt('The name of the feature to retrieve.'),
      'up'     => dt('An integer value specifying the number of upstream bases to include.'),
      'down'   => dt('An integer value specifying the number of downstream bases to incldue.'),
      'out'    => dt('The output format. Valid options are "fasta_html", "fasta_txt" and raw.'),
      'parent' => dt('Set this argument to 1 to retrieve the sequence from the parent in an alignment rather than the residues column of the feature itself.'),
      'agg'    => dt('Set this argument to 1 to aggregate sub features into a single sequence.  This is useful, for example, for obtaining CDS sequence from an mRNA'),
      'child'  => dt('Set this argument to the sequence ontology term for the children to aggregate.  This is useful in the case where a gene has exons as well as CDSs and UTRs.  You may sepcify as many feature types as desired by separating each with a single comma (no spaces).'),
    ),
    'examples' => array(
      'Standard example' => 'drush tripal-current-job',
    ),
    'aliases' => array('trp-get-seq'),
  );
  $items['tripal-feature-sync'] = array(
    'description' => dt('Syncs an individual feature.'),
    'options'   => array(
      'id'    => dt('The feature ID of the feature to sync'),
    ),
    'examples' => array(
      'Standard example' => 'drush tripal-feature-sync --id=48273',
    ),
    'aliases' => array('trp-fsync'),
  );
  return $items;
}

/**
 * Executes jobs in the Tripal Jobs Queue
 *
 * NOTE: The following code is executed when drush 'trpjob-run' or 'drush tripal-launch-jobs' is called
 */
function drush_tripal_feature_tripal_get_sequence() {

  $org_commonname = drush_get_option('org');  
  $analysis_name = drush_get_option('analysis');  
  $type = drush_get_option('type');
  $feature_name = drush_get_option('name');
  $upstream = drush_get_option('up');
  $downstream = drush_get_option('down');
  $output_format = drush_get_option('out');
  $derive_from_parent = drush_get_option('parent');
  $aggregate = drush_get_option('agg');
  $child = drush_get_option('child');
  
  $sub_features = explode(',', $child);
    
  if (!$output_format) {
     $output_format = 'fasta_txt';
  }
  
  if (!$type and !$feature_name and !$org_commonname) {
     print "Please provide a type, feature name or organism common name\n";
     return;
  }

  // get the list of features
  $vars = array();
  $sql  = "SELECT DISTINCT F.feature_id, F.name, F.uniquename, O.genus, O.species, CVT.name as feature_type ".
          "FROM feature F ".
          "  INNER JOIN organism O on O.organism_id = F.organism_id ".
          "  INNER JOIN cvterm CVT on CVT.cvterm_id = F.type_id ";
  if ($analysis_name) {
     $sql .= "  INNER JOIN analysisfeature AF on AF.feature_id = F.feature_id ".
             "  INNER JOIN analysis A on AF.analysis_id = A.analysis_id ";
  }         
  $sql .= "WHERE (1=1) ";
  if ($org_commonname) {
     $sql .= "AND O.common_name = '%s' ";
     $vars[] = $org_commonname;
  }
  if ($type) {
    $sql .= "AND CVT.name = '%s' ";
    $vars[] = $type; 
  }
  if ($feature_name) {
     $sql .= "AND F.name = '%s'";
     $vars[] = $feature_name;
  }
  if ($analysis_name) {
     $sql .= "AND A.name = '%s'";
     $vars[] = $analysis_name;
  }
  $num_bases_per_line = 50;
  $q = chado_query($sql, $vars);
  while ($feature = db_fetch_object($q)) {
    $feature_id = $feature->feature_id;
    $feature_name = "$feature->uniquename $feature->name $feature->feature_type ($feature->genus $feature->species)";
      
    $sequence = tripal_feature_get_formatted_sequence($feature_id, $feature_name, 
      $num_bases_per_line, $derive_from_parent, $aggregate, $output_format,
      $upstream, $downstream, $sub_features);
    print $sequence;
  }
}
/*
 * 
 */
function drush_tripal_feature_sync() {
  $feature_id = drush_get_option('id');  
  tripal_feature_sync_feature($feature_id);
}