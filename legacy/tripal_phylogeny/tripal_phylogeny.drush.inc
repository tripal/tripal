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
function tripal_phylogeny_drush_help($command) {
  switch ($command) {
    case 'trp-insert-phylotree':
      return dt('Adds a new phylotree record.');
      break;
    case 'trp-update-phylotree':
      return dt('Updates an existing phylotree record.');
      break;
    case 'trp-delete-phylotree':
      return dt('Deletes an existing phylotree record.');
      break;
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
function tripal_phylogeny_drush_command() {
  $items = [];
  $items['trp-insert-phylotree'] = [
    'description' => dt('Adds a new phylotree record.'),
    'arguments' => [],
    'examples' => [],
    'options' => [
      'username' => [
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
        'required' => TRUE,
      ],
      'name' => [
        'description' => dt('The name of the tree. The name of the tree must be unique.'),
        'required' => TRUE,
      ],
      'description' => [
        'description' => dt('A description for the tree. Use quotes.'),
        'required' => TRUE,
      ],
      'analysis' => [
        'description' => dt('The name of the analysis used to generate this tree. This should be the name of an analysis record already present in Chado. Use quotes.'),
        'required' => TRUE,
      ],
      'leaf_type' => [
        'description' => dt('The Sequence Ontology term for the leaf node type of the tree (e.g. polypeptide). If this is a taxonomic the use the word "taxonomy".'),
        'required' => TRUE,
      ],
      'tree_file' => [
        'description' => dt('The full path to the file containing the tree.'),
        'required' => TRUE,
      ],
      'format' => [
        'description' => dt('The format of the input file. Currently, only the "newick" file format is supported.'),
        'required' => TRUE,
      ],
      'dbxref' => dt('A database cross-reference of the form DB:ACCESSION. Where DB is the database name, which is already present in Chado, and ACCESSION is the unique identifier for this tree in the remote database.'),
      'sync' => dt('Set to 1 if this tree should be synced with Drupal.'),
      'match' => dt('Set to "uniquename" if the leaf nodes should be matched with the feature uniquename'),
      'name_re' => dt('If the leaf type is NOT "taxonomy", then this option can be a regular expression used pull out the name of the feature from the node label in theintput tree.'),
    ],
  ];

  $items['trp-update-phylotree'] = [
    'description' => dt('Adds a new phylotree record. If a new file is provided then the entire existing tree will be rebuilt using the file provided.'),
    'arguments' => [],
    'examples' => [],
    'options' => [
      'username' => [
        'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
        'required' => TRUE,
      ],
      'phylotree_id' => dt('The unique phylotree ID assigned within Chado for the tree that should be updated.'),
      'name' => dt('A unique name for the tree. If the phylotree_id is not provided then the phylotree matching this name will be updated.'),
      'description' => dt('A description for the tree. Use quotes.'),
      'analysis' => dt('The name of the analysis used to generate this tree. This should be the name of an analysis record already present in Chado. Use quotes.'),
      'leaf_type' => dt('The Sequence Ontology term for the leaf node type of the tree (e.g. polypeptide). If this is a taxonomic the use the word "taxonomy".'),
      'tree_file' => dt('The full path to the file containing the tree.'),
      'format' => dt('The format of the input file. Currently, only the "newick" file format is supported.'),
      'dbxref' => dt('A database cross-reference of the form DB:ACCESSION. Where DB is the database name, which is already present in Chado, and ACCESSION is the unique identifier for this tree in the remote database.'),
      'sync' => dt('Set to 1 if this tree should be synced with Drupal.'),
      'match' => dt('Set to "uniquename" if the leaf nodes should be matched with the feature uniquename'),
      'name_re' => dt('If the leaf type is NOT "taxonomy", then this option can be a regular expression used pull out the name of the feature from the node label in theintput tree.'),
    ],
  ];

  $items['trp-delete-phylotree'] = [
    'username' => [
      'description' => dt('The Drupal user name for which the job should be run.  The permissions for this user will be used.'),
      'required' => TRUE,
    ],
    'description' => dt('Deletes a phylotree record and it\'s corresponding tree nodes.'),
    'arguments' => [],
    'examples' => [],
    'options' => [
      'phylotree_id' => dt('The unique phylotree ID assigned within Chado for the tree that should be deleted.'),
      'name' => dt('If the phylotree_id is not provided then the phylotree matching this name will be deleted.'),
    ],
  ];
  return $items;
}

/**
 * Deletes a phylotree record.
 *
 * Executed when 'drush trp-delete-phylotree' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_phylogeny_trp_insert_phylotree() {
  $username = drush_get_option('username');
  drush_tripal_core_set_user($username);

  $options = [
    'name' => drush_get_option('name'),
    'description' => drush_get_option('description'),
    'analysis' => drush_get_option('analysis'),
    'leaf_type' => drush_get_option('leaf_type'),
    'tree_file' => drush_get_option('tree_file'),
    'format' => drush_get_option('format'),
    'dbxref' => drush_get_option('dbxref'),
    'sync' => drush_get_option('sync'),
    'match' => drush_get_option('match'),
    'name_re' => drush_get_option('name_re'),
    'load_now' => TRUE,
  ];
  $errors = [];
  $warnings = [];
  if (tripal_insert_phylotree($options, $errors, $warnings)) {

    if ($options['sync']) {
      chado_node_sync_records('phylotree', FALSE, FALSE,
        [], $ids = [$options['phylotree_id']]);

      $nid = chado_get_nid_from_id('phylotree', $options['phylotree_id']);
      tripal_add_node_variable($nid, 'phylotree_name_re', $options['name_re']);
      tripal_add_node_variable($nid, 'phylotree_use_uniquename', $options['match']);
      tripal_add_node_variable($nid, 'phylotree_tree_file', basename($options['tree_file']));
    }
  }
  drush_print("Done.");
}

/**
 *
 */
function drush_tripal_phylogeny_trp_update_phylotree() {
  $username = drush_get_option('username');
  drush_tripal_core_set_user($username);

  $options = [
    'phylotree_id' => drush_get_option('phylotree_id'),
    'name' => drush_get_option('name'),
    'description' => drush_get_option('description'),
    'analysis' => drush_get_option('analysis'),
    'leaf_type' => drush_get_option('leaf_type'),
    'tree_file' => drush_get_option('tree_file'),
    'format' => drush_get_option('format'),
    'dbxref' => drush_get_option('dbxref'),
    'sync' => drush_get_option('sync'),
    'match' => drush_get_option('match'),
    'name_re' => drush_get_option('name_re'),
    'load_now' => TRUE,
  ];

  if (!$options['phylotree_id'] and $options['name']) {
    $phylotree = chado_select_record('phylotree', ['phylotree_id'], ['name' => $options['name']]);
    if (count($phylotree) > 0) {
      $options['phylotree_id'] = $phylotree[0]->phylotree_id;
    }
    else {
      drush_print('A phylotree record with this name does not exists.');
    }
  }

  if ($options['phylotree_id'] and tripal_update_phylotree($options['phylotree_id'], $options)) {

    if ($options['sync']) {
      chado_node_sync_records('phylotree', FALSE, FALSE,
        [], $ids = [$options['phylotree_id']]);
    }
    $nid = chado_get_nid_from_id('phylotree', $options['phylotree_id']);
    if ($nid) {
      if ($options['name_re']) {
        tripal_delete_node_variables($nid, 'phylotree_name_re');
        tripal_add_node_variable($nid, 'phylotree_name_re', $options['name_re']);
      }
      if ($options['match']) {
        tripal_delete_node_variables($nid, 'phylotree_use_uniquename');
        tripal_add_node_variable($nid, 'phylotree_use_uniquename', $options['match']);
      }
      if ($options['tree_file']) {
        tripal_delete_node_variables($nid, 'phylotree_tree_file');
        tripal_add_node_variable($nid, 'phylotree_tree_file', basename($options['tree_file']));
      }
    }
  }
  drush_print("Done.");
}

/**
 * Deletes a phylotree record.
 *
 * Executed when 'drush trp-delete-phylotree' is called.
 *
 * @ingroup tripal_drush
 */
function drush_tripal_phylogeny_trp_delete_phylotree() {
  $username = drush_get_option('username');
  drush_tripal_core_set_user($username);

  $options = [
    'phylotree_id' => drush_get_option('phylotree_id'),
    'name' => trim(drush_get_option('name')),
  ];
  if (!$options['phylotree_id'] and $options['name']) {
    $phylotree = chado_select_record('phylotree', ['phylotree_id'], ['name' => $options['name']]);
    if (count($phylotree) > 0) {
      $options['phylotree_id'] = $phylotree[0]->phylotree_id;
    }
    else {
      drush_print('A phylotree record with this name does not exists.');
    }
  }

  if ($options['phylotree_id'] and tripal_delete_phylotree($options['phylotree_id'])) {
    chado_cleanup_orphaned_nodes('phylotree');
  }
  drush_print("Done.");
}