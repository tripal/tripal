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
 *
 * @ingroup tripal_drush
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
 *
 * @ingroup tripal_drush
 */
function tripal_feature_drush_command() {
  $items = array();
  $items['tripal-get-sequence'] = array(
    'description' => dt('Prints sequences that match specified categories.'),
    'options'    => array(
      'org'      => dt('The organism\'s common name. If specified, features for this organism will be retrieved.'),
      'genus'    => dt('The organism\'s genus. If specified, features for all organism with this genus will be retrieved.'),
      'species'  => dt('The organism\'s species name. If specified, features for this all organism with this species will be retrieved.'),
      'analysis' => dt('The analysis name. If specified, features for this analysis will be retrieved.'),
      'type'     => dt('The type of feature to retrieve (e.g. mRNA). All features that match this type will be retrieved.'),
      'name'     => dt('The name of the feature to retrieve.'),
      'up'       => dt('An integer value specifying the number of upstream bases to include.'),
      'down'     => dt('An integer value specifying the number of downstream bases to incldue.'),
      'out'      => dt('The output format. Valid options are "fasta_html", "fasta_txt" and raw.'),
      'parent'   => dt('Set this argument to 1 to retrieve the sequence from the parent in an alignment rather than the residues column of the feature itself.'),
      'agg'      => dt('Set this argument to 1 to aggregate sub features into a single sequence.  This is useful, for example, for obtaining CDS sequence from an mRNA'),
      'child'    => dt('Set this argument to the sequence ontology term for the children to aggregate.  This is useful in the case where a gene has exons as well as CDSs and UTRs.  You may sepcify as many feature types as desired by separating each with a single comma (no spaces).'),
      'relationship'  => dt('Retreives the sequence of any feature in the specified relationship with the matched features.'),
      'rel_part' => dt('If a relationship is provided, then this will be "subject" or "object" indicating the side of the relationship for the matched features. If the matched features are the "object" then the "subject" features will have their sequences included in the output (and vice versa).'),
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
 * Retrieves the sequence of the indicated features
 *
 * @ingroup tripal_drush
 */
function drush_tripal_feature_tripal_get_sequence() {

  $org_commonname = drush_get_option('org');
  $genus = drush_get_option('genus');
  $species = drush_get_option('species');
  $analysis_name = drush_get_option('analysis');
  $type = drush_get_option('type');
  $feature_name = drush_get_option('name');
  $upstream = drush_get_option('up');
  $downstream = drush_get_option('down');
  $output_format = drush_get_option('out');
  $derive_from_parent = drush_get_option('parent');
  $aggregate = drush_get_option('agg');
  $child = drush_get_option('child');
  $relationship = drush_get_option('relationship');
  $rel_part = drush_get_option('rel_part');

  if ($relationship and !$rel_part) {
    print "Please specify both 'relationship' and a 'rel_part' arguments. Both must be used together\n";
    return;
  }

  tripal_feature_seq_extract_get_features($org_commonname, $genus, $species, $analysis_name,
    $type, $feature_name, $upstream, $downstream, $output_format, $derive_from_parent,
    $aggregate, $child, $relationship, $rel_part);
}

/**
 * Sync Chado Features with Drupal (ie: create nodes)
 *
 * @ingroup tripal_drush
 */
function drush_tripal_feature_sync() {
  $feature_id = drush_get_option('id');
  tripal_feature_sync_feature($feature_id);
}
