<?php


/**
 * Imports a tree file.
 *
 * This function is used as a wrapper for loading a phylogenetic tree using
 * any number of file loaders.
 *
 * @param $file_name
 *   The name of the file containing the phylogenetic tree to import.
 * @param $format
 *   The format of the file. Currently only the 'newick' file format is
 *   supported.
 * @param $options
 *   Options if the phylotree record already exists:
 *     'phylotree_id': The imported nodes will be associated with this tree.
 *     'leaf_type':  A sequence ontology term or the word 'organism'. If the
 *                   type is 'organism' then this tree represents a
 *                   taxonomic tree.  The default, if not specified, is the
 *                   term 'polypeptide'.
 *     'name_re':    If the leaf type is NOT 'taxonomy', then the value of
 *                   this field can be a regular expression to pull out
 *                   the name of the feature from the node label in the
 *                   intput tree. If no value is provided the entire label is
 *                   used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *
 */
function tripal_phylogeny_import_tree_file($file_name, $format, $options = array(), $job_id = NULL) {

    // Set some option details.
  if (!array_key_exists('leaf_type', $options)) {
    $options['leaf_type'] = 'polypeptide';
  }
  if (!array_key_exists('match', $options)) {
    $options['match'] = 'name';
  }
  if (!array_key_exists('name_re', $options)) {
    $options['name_re'] = '^(.*)$';
  }
  $options['name_re'] = trim($options['name_re']);

  // If a phylotree ID is not passed in then make sure we have the other
  // required fields for creating a tree.
  if (!array_key_exists('phylotree_id', $options)) {
    if (!array_key_exists('name', $options)) {
       tripal_report_error('tripal_phylogeny', TRIPAL_ERROR,
         'The phylotree_id is required for importing the tree.');
       return FALSE;
    }
  }

  // get the phylotree record.
  $values = array('phylotree_id' => $options['phylotree_id']);
  $phylotree = chado_generate_var('phylotree', $values);

  if (!$phylotree) {
    tripal_report_error('tripal_phylogeny', TRIPAL_ERROR,
        'Could not find the phylotree using the ID provided: %phylotree_id.',
        array('%phylotree_id' => $options['phylotree_id']));
    return FALSE;
  }

  $transaction = db_transaction();
  print "\nNOTE: Loading of this tree file is performed using a database transaction. \n" .
      "If the load fails or is terminated prematurely then the entire set of \n" .
      "insertions/updates is rolled back and will not be found in the database\n\n";
  try {

    // Parse the file according to the format indicated.
    if ($format == 'newick') {
      // Parse the tree into the expected nested node format.
      module_load_include('inc', 'tripal_phylogeny', 'includes/parsers/tripal_phylogeny.newick_parser');
      $tree = tripal_phylogeny_parse_newick_file($file_name);

      // Assign the right and left indecies to the tree ndoes
      tripal_phylogeny_assign_tree_indices($tree);
    }
    // Iterate through the tree nodes and add them to Chado in accordance
    // with the details in the $options array.
    tripal_phylogeny_import_tree($tree, $phylotree, $options);
  }
  catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('tripal_phylogeny', $e);
    print "\nFAILED: Rolling back database changes...\n";
  }
  print "\nDone Importing Tree.\n";
}

/**
 *
 * @return boolean|multitype:Either
 */
function tripal_phylogeny_get_node_types_vocab() {
  // Get the vocabulary terms used to describe nodes in the tree
  $values = array(
    'name' => 'phylo_leaf',
    'cv_id' => array(
      'name' => 'tripal_phylogeny',
    ),
  );
  $leaf = chado_generate_var('cvterm', $values);
  if (!$leaf) {
    tripal_report_error('tripal_phylogeny', TRIPAL_ERROR,
      "Could not find the leaf vocabulary term: 'phylo_leaf'. It should " .
      "already be present as part of the tripal_phylogeny vocabulary.");
    return FALSE;
  }
  $values['name'] = 'phylo_interior';
  $internal = chado_generate_var('cvterm', $values);
  if (!$internal) {
    tripal_report_error('tripal_phylogeny', TRIPAL_ERROR,
      "Could not find the leaf vocabulary term: 'phylo_interior'. It should " .
      "already be present as part of the tripal_phylogeny vocabulary.");
    return FALSE;
  }
  $values['name'] = 'phylo_root';
  $root = chado_generate_var('cvterm', $values);
  if (!$root) {
    tripal_report_error('tripal_phylogeny', TRIPAL_ERROR,
      "Could not find the leaf vocabulary term: 'phylo_root'. It should " .
      "already be present as part of the tripal_phylogeny vocabulary.");
    return FALSE;
  }
  $vocab = array(
    'leaf' => $leaf,
    'internal' => $internal,
    'root' => $root,
  );
  return $vocab;
}

/**
 * Iterates through the tree and sets the left and right indicies .
 *
 * @param $tree
 *   The tree array.
 * @param $index
 *   This parameters is not used when the function is first called. It
 *   is used for recursive calls.
 */
function tripal_phylogeny_assign_tree_indices(&$tree, &$index = 1) {
  // Assign a left and right index to each node.  The child node must
  // have a right and left index less than that of it's parents.  We
  // increment the index by 100 to give space for new nodes that might
  // be added later.
  if (array_key_exists('name', $tree)) {
    $tree['left_index'] = $index += 100;
    if (array_key_exists('is_leaf', $tree)) {
      $tree['right_index'] = $index += 100;
    }
  }
  if (array_key_exists('branch_set', $tree)) {
    foreach ($tree['branch_set'] as $key => $node) {
      tripal_phylogeny_assign_tree_indices($tree['branch_set'][$key], $index);
      $tree['right_index'] = $index += 100;
    }
  }
}

/**
 * Iterates through the tree array and creates phylonodes in Chado.
 *
 * The function iterates through the tree in a top-down approach adding
 * parent internal nodes prior to leaf nodes.  Each node of the tree should have
 * the following fields:
 *
 *   -name:         The name (or label) for this node.
 *   -depth:        The depth of the node in the tree.
 *   -is_root:      Set to 1 if this node is a root node.
 *   -is_leaf:      Set to 1 if this node is a leaf node.
 *   -is_internal:  Set to 1 if this node is an internal node.
 *   -left_index:   The index of the node to the left in the tree.
 *   -right_index:  The index of the node to the right in the tree.
 *   -branch_set:   An array containing a list of nodes of that are children
 *                  of the node.
 *   -parent:       The name of the parent node.
 *   -organism_id:  The organism_id for associtating the node with an organism.
 *   -properties:   An array of key/value pairs where the key is the cvterm_id
 *                  and the value is the property value.  These properties
 *                  will be assocaited with the phylonode.
 *
 * Prior to importing the tree the indicies can be set by using the
 * tripal_phylogeny_assign_tree_indices() function.
 *
 * @param $tree
 *   The tree array.
 * @param $options
 *   The options provide some direction for how the tree is imported.  The
 *   following keys can be used:
 *   -taxonomy:  Set to 1 if this tree is a taxonomic tree. Set to 0
 *               otherwise.
 *   -leaf_type: Set to the leaf type name. If this is a non-taxonomic tree
 *               that is associated with features, then this should be the
 *               Sequence Ontology term for the feature (e.g. polypeptide).
 *               If this is a taxonomic tree then this option is not needed.
 *   -match:     Set to either 'name' or 'uniquename'.  This is used for
 *               matching the feature name or uniquename with the node name.
 *               This is not needed for taxonomic trees.
 *   -match_re:  Set to a regular that can be used for matching the node
 *               name with the feature name if the node name is not
 *               identical to the feature name.
 * @param $vocab
 *   Optional. An array containing a set of key/value pairs that maps node
 *   types to CV terms.  The keys must be 'root', 'internal' or 'leaf'.  If
 *   no vocab is provded then the terms provided by the tripal_phylogeny
 *   CV will be used.
 * @param $parent
 *   This argument is not needed when the funtion is first called. This
 *   function is recursive and this argument is used on recursive calls.
 */
function tripal_phylogeny_import_tree(&$tree, $phylotree, $options, $vocab = array(), $parent = NULL) {

  // Get the vocabulary terms used to describe nodes in the tree if one
  // wasn't provided.
  if (count($vocab) == 0) {
    $vocab = tripal_phylogeny_get_node_types_vocab();
  }

  if (is_array($tree) and array_key_exists('name', $tree)) {
    $values = array(
      'phylotree_id' => $phylotree->phylotree_id,
      'left_idx'  => $tree['left_index'],
      'right_idx' => $tree['right_index'],
    );
    // Add in any optional values to the $values array if they are present
    if (!empty($tree['name']) and $tree['name'] != '') {
      $values['label'] = $tree['name'];
    }
    if (!empty($tree['length']) and $tree['length'] != '') {
      $values['distance'] = $tree['length'];
    }
    // Set the type of node
    if ($tree['is_root']) {
      $values['type_id'] = $vocab['root']->cvterm_id;
    }
    else if ($tree['is_internal']) {
      $values['type_id'] = $vocab['internal']->cvterm_id;
      $values['parent_phylonode_id'] = $parent['phylonode_id'];
      // TOOD: a feature may be associated here but it is recommended that it
      // be a feature of type SO:match and should represent the alignment of
      // all features beneath it.
    }
    else if ($tree['is_leaf']) {
      $values['type_id'] = $vocab['leaf']->cvterm_id;
      $values['parent_phylonode_id'] = $parent['phylonode_id'];

      // Match this leaf node with an organism or feature depending on the
      // type of tree. But we can't do that if we don't have a name.
      if (!empty($tree['name']) and $tree['name'] != '') {
        if (!$options['taxonomy']) {

          // This is a sequence-based tree. Try to match leaf nodes with features.
          // First, Get the Name and uniquename for the feature
          $matches = array();
          $sel_values = array();
          if ($options['match'] == "name") {
            $sel_values['name'] = $tree['name'];
            $re = $options['name_re'];
            if (preg_match("/$re/", $tree['name'], $matches)) {
              $sel_values['name'] = $matches[1];
            }
          }
          else {
            $sel_values['uniquename'] = $tree['name'];
            $re = $options['name_re'];
            if (preg_match("/$re/", $tree['name'], $matches)) {
              $sel_values['uniquename'] = $matches[1];
            }
          }
          $sel_values['type_id'] = array(
            'name' => $options['leaf_type'],
            'cv_id' => array(
              'name' => 'sequence'
            ),
          );
          $sel_columns = array('feature_id');
          $feature = chado_select_record('feature', $sel_columns, $sel_values);
          if (count($feature) > 1) {
            // Found multiple features, cannot make an association.
          }
          else if (count($feature) == 1) {
            $values['feature_id'] = $feature[0]->feature_id;
          }
          else {
            // Could not find a feature that matches the name or uniquename
          }
        }
      }
    }

    // Insert the new node and then add it's assigned phylonode_id to the node
    $phylonode = chado_insert_record('phylonode', $values);
    $tree['phylonode_id'] = $phylonode['phylonode_id'];

    // This is a taxonomic tree, so assocaite this node with an
    // organism if one is provided.
    if (array_key_exists('organism_id', $tree)) {
      $values = array(
        'phylonode_id' => $tree['phylonode_id'],
        'organism_id' => $tree['organism_id']
      );
      $pylonode_organism = chado_insert_record('phylonode_organism', $values);
    }

    // Associate any properties
    if (array_key_exists('properties', $tree)) {
      foreach ($tree['properties'] as $type_id => $value) {
        $values = array(
          'phylonode_id' => $tree['phylonode_id'],
          'type_id' => $type_id,
          'value' => $value,
        );
        $pylonode_organism = chado_insert_record('phylonodeprop', $values);
      }
    }
  }
  if (is_array($tree) and array_key_exists('branch_set', $tree)) {
    foreach ($tree['branch_set'] as $key => $node) {
      tripal_phylogeny_import_tree($tree['branch_set'][$key], $phylotree, $options, $vocab, $tree);
    }
  }
}