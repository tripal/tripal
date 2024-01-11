<?php

/**
 * @file
 * Provides API functions specifically for managing phylogenetic and taxonomic
 * tree records in Chado.
 */

/**
 * @defgroup tripal_phylotree_api Chado Phylotree
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing phylogenetic and taxonomic
 * tree records in Chado.  The API consists of functions for creation,
 * retrieval, update and deltion (CRUD) for phylogenetic tree records as
 * well as importing of trees in the newick file format.
 * @}
 */


/**
 * Validates an $options array for insert or update of a phylotree record.
 *
 * If validation passes then any values that needed validation lookups
 * (such as the dbxref, analysis, leaf_type, etc) will have their approriate
 * primary_keys added to the $options array, and missing default values
 * will also be added.
 *
 * @param $val_type
 *   The type of validation. Can be either 'insert' or 'update'.
 * @param $options
 *   An array of key/value pairs containing any of the valid keys for
 *   either the chado_insert_phylotree() or chado_update_phylotree()
 *   functions.
 * @param $errors
 *   An empty array where validation error messages will be set. The keys
 *   of the array will be the name of the field from the options array and
 *   the value is the error message.
 * @param $warnings
 *   An empty array where validation warning messagges will be set. The
 *   warnings should not stop an insert or an update but should be provided
 *   to the user as information by a drupal_set_message() if appropriate. The
 *   keys of the array will be the name of the field from the options array
 *   and the value is the error message.
 *
 * @return
 *   If validation fails then FALSE is returned.  Any options that do not pass
 *   validation checks will be added in the $errors array with the key being
 *   the option and the value being the error message.  If validation
 *   is successful then TRUE is returned.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_validate_phylotree($val_type, &$options, &$errors, &$warnings, $schema_name = 'chado') {
  $chado = \Drupal::service('tripal_chado.database');
  $chado->setSchemaName($schema_name);
  if ($val_type != 'insert' and $val_type != 'update') {
    \Drupal::service('tripal.logger')->error("The \$val_type argument to"
        . " chado_validate_phylotree() must be either 'update or 'insert'.");
  }

  // Set Defaults.
  if ($val_type == 'insert') {
    // Match by feature name.
    if (!array_key_exists('match', $options)) {
      $options['match'] = 'name';
    }
    // The default regular expression is to match the entire node name.
    if (!array_key_exists('name_re', $options) or (!$options['name_re'])) {
      $options['name_re'] = '^(.*)$';
    }
    // A dbxref is not required by Tripal, but is required by the database
    // field in the phylotree table. Therefore, if the dbxref is not provided,
    // we can set this to be the null database and null dbxref which
    // is represented as 'null:local:null'
    if (!array_key_exists('dbxref', $options)) {
      $options['dbxref'] = "null:local:null";
    }
  }

  // Make sure required values are set.
  if ($val_type == 'insert') {
    if (!array_key_exists('name', $options)) {
      $errors['name'] = t('Please provide the name of the tree.');
      return FALSE;
    }
    if (!array_key_exists('description', $options)) {
      $errors['description'] = t('Please provide a description for this tree.');
      return FALSE;
    }
    if (!array_key_exists('format', $options) or !$options['format']) {
      $errors['format'] = t('Please provide a file format for the tree file.');
      return FALSE;
    }
    // Make sure the file format is correct.
    if ($options['format'] != 'newick' and $options['format'] != 'taxonomy') {
      $errors['format'] = t('The file format "%format" is not supported. '
        . 'Currently only the "newick" file format is supported.',
        [ '%format' => $options['format']]);
      return FALSE;
    }
  }
  // else $val_type == 'update'
  else {
    // Does the phylotree ID exist and is it valid.
    if (!array_key_exists('phylotree_id', $options)) {
      $errors['phylotree_id'] = t('Please provide the ID for the tree.');
      return FALSE;
    }
    $exists = $chado->select('1:phylotree', 'phylotree')
      ->fields('phylotree')
      ->condition('phylotree_id', $options['phylotree_id'])
      ->execute()
      ->fetchObject();
    if (!$exists) {
      $errors['phylotree_id'] = t('The phylotree_id "%id" does not exist.',
        ['%id' => $options['phylotree_id']]);
      return FALSE;
    }
  }

  // Make sure the file exists if one is specified.
  if (array_key_exists('tree_file', $options) and $options['tree_file']) {
    // If this is a numeric Drupal file then all is good, no need to check.
    if (!is_numeric($options['tree_file'])) {
      if (!file_exists($options['tree_file'])) {
        $errors['tree_file'] = t('The provided file "%file" does not exist.',
          ['%file' => $options['tree_file']]);
        return FALSE;
      }
    }

    // Make sure the file format is correct.
    if (!array_key_exists('format', $options) or
      ($options['format'] != 'newick' and $options['format'] != 'taxonomy')) {
      $errors['format'] = t('Please provide a supported file format. '
        . 'Currently only the "newick" file format is supported.');
      return FALSE;
    }

    // If no leaf type is provided then assume a taxonomic tree.
    if (!array_key_exists('leaf_type', $options) or !$options['leaf_type']) {
      $options['leaf_type'] = 'taxonomy';
    }
  }

  // Make sure the analysis exists.
  $analysis = NULL;
  if (array_key_exists('analysis_id', $options) and $options['analysis_id']) {
    $analysis = $chado->select('1:analysis', 'analysis')
      ->fields('analysis')
      ->condition('analysis_id', $options['analysis_id'])
      ->execute()
      ->fetchObject();
    if (!$analysis) {
      $errors['analysis_id'] = t(
        'The analysis ID provided "%id" does not exist.',
        ['%id' => $options['analysis_id']]);
      return FALSE;
    }
    $options['analysis_id'] = $analysis->analysis_id;
  }
  elseif (array_key_exists('analysis', $options) and $options['analysis']) {
    $analysis = $chado->select('1:analysis', 'analysis')
      ->fields('analysis')
      ->condition('name', $options['analysis'])
      ->execute()
      ->fetchObject();
    if (!$analysis) {
      $errors['analysis'] = t(
        'The analysis name provided "%name" does not exist.',
        ['%name' => $options['analysis']]);
      return FALSE;
    }
    $options['analysis_id'] = $analysis->analysis_id;
  }

  // Make sure the leaf type exists.
  $type = NULL;
  if (array_key_exists('leaf_type', $options) and $options['leaf_type']) {
    if ($options['leaf_type'] == 'taxonomy') {
      $values = [
        'cv_id' => [
          'name' => 'EDAM',
        ],
        'name' => 'Species tree',
      ];

      // Find the cv_id for EDAM
      $cv_id = $chado->select('1:cv', 'cv')
        ->fields('cv')
        ->condition('name', 'EDAM')
        ->execute()
        ->fetchObject()
        ->cv_id;

      $type = $chado->select('1:cvterm', 'cvterm')
        ->fields('cvterm')
        ->condition('name', 'Species tree')
        ->condition('cv_id', $cv_id)
        ->execute()
        ->fetchObject();
    }
    else {
      $values = [
        'cv_id' => [
          'name' => 'sequence',
        ],
        'name' => $options['leaf_type'],
      ];
      // $type = chado_select_record('cvterm', ['cvterm_id'], $values, NULL, $schema_name);

      // Find the cv_id for sequence
      $cv_id = $chado->select('1:cv', 'cv')
        ->fields('cv')
        ->condition('name', 'sequence')
        ->execute()
        ->fetchObject()
        ->cv_id;

      $type = $chado->select('1:cvterm', 'cvterm')
        ->fields('cvterm')
        ->condition('name', $options['leaf_type'])
        ->condition('cv_id', $cv_id)
        ->execute()
        ->fetchObject();

      if (!$type) {
        $errors['leaf_type'] = t('The leaf_type provided "%term" is not a valid Sequence Ontology term.',
          ['%term' => $options['leaf_type']]);
        return FALSE;
      }
    }
    $options['type_id'] = $type->cvterm_id;
  }

  // A Dbxref is required by the phylotree module, but if the
  // tree was generated in-house and the site admin doesn't want to
  // assign a local dbxref then we will set it to the null db
  // and the local:null dbxref.
  if (array_key_exists('dbxref', $options)) {
    if (!$options['dbxref']) {
      $options['dbxref'] = 'null:local:null';
    }
    $matches = [];
    preg_match('/^(.*?):(.*)$/', $options['dbxref'], $matches);
    $db_name = $matches[1];
    $accession = $matches[2];
    $values = [
      'accession' => $accession,
      'db_id' => [
        'name' => $db_name,
      ],
    ];
    $dbxref = chado_generate_var('dbxref', $values, [], $schema_name);

    if (!$dbxref) {

      $db = chado_generate_var('db', ['name' => $db_name], [], $schema_name);
      if (!$db) {
        $errors['dbxref'] = t(
            'dbxref could not be created for %dbname:%dbxref, this DB does not exist.',
            ['%dbname' => $db_name, '%dbxref' => $dbxref]);
        return FALSE;
      }

      // Here we create the new dbxref for the specified new accession.
      $dbxref = $chado->insert('1:dbxref')->fields([
        'accession' => $values['accession'],
        'db_id' => $db->db_id
      ])->execute();
      if (!$dbxref) {
        $errors['dbxref'] = t(
            'dbxref could not be created for %dbname:%dbxref.',
            ['%dbname' => $db_name, '%dbxref' => $dbxref]);
        return FALSE;
      }
    }

    if (is_object($dbxref)) {
      $options['dbxref_id'] = $dbxref->dbxref_id;
    }
    elseif (is_array($dbxref)) {
      $options['dbxref_id'] = $dbxref['dbxref_id'];
    }
    else {
      $options['dbxref_id'] = $dbxref;
    }
  }

  // Make sure the tree name is unique.
  if (array_key_exists('name', $options) and $options['name']) {
    $sql = "
      SELECT *
      FROM {1:phylotree} P
      WHERE
        P.name = :name
    ";
    $args = [':name' => $options['name']];
    if ($val_type == 'update') {
      $sql .= " AND NOT P.phylotree_id = :phylotree_id";
      $args[':phylotree_id'] = $options['phylotree_id'];
    }
    $result = $chado->query($sql, $args)->fetchObject();
    if ($result) {
      $errors['name'] = t('The tree name "%name" is in use by another tree.'
        . ' Please provide a different unique name for this tree.',
        ['%name' => $options['name']]);
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * Inserts a phylotree record into Chado.
 *
 * This function validates the options passed prior to insertion of the record,
 * and if validation passes then any values in the options array that needed
 * validation lookups (such as the dbxref, analysis, leaf_type, etc) will have
 * their approriate primary key values added to the options array.
 *
 * @param $options
 *  An array of key value pairs with the following keys required:
 *     'name':       The name of the tree. This will be displayed to users.
 *     'description: A description about the tree
 *     'analysis_id: The ID of the analysis to which this phylotree should be
 *                   associated.
 *     'analysis':   If the analysis_id key is not used then the analysis name
 *                   may be provided to identify the analysis to which the tree
 *                   should be associated.
 *     'leaf_type':  A sequence ontology term or the word 'taxonomy'. If the
 *                   type is 'taxonomy' then this tree represents a
 *                   taxonomic tree. The default, if not specified, is a
 *                   taxonomic tree.
 *     'tree_file':  The path of the file containing the phylogenetic tree to
 *                   import or a Drupal managed_file numeric ID.
 *     'format':     The file format. Currently only 'newick' is supported.
 *
 *  Optional keys:
 *     'dbxref':     A database cross-reference of the form DB:ACCESSION.
 *                   Where DB is the database name, which is already present
 *                   in Chado, and ACCESSION is the unique identifier for
 *                   this tree in the remote database.
 *     'name_re':    The value of this field can be a regular expression to pull
 *                   out the name of the feature or organism from the node label
 *                   in the input tree. If no value is provided the entire label
 *                   is used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *     'load_later': If set, the tree will be loaded via a separate Tripal
 *                   jobs call. Otherwise, the tree will be loaded immediately.
 *     'no_load':    If set the tree file will not be loaded.
 *     'job':        The Tripal job object, if present.
 * @param $errors
 *   An empty array where validation error messages will be set. The keys
 *   of the array will be name of the field from the options array and the
 *   value is the error message.
 * @param $warnings
 *   An empty array where validation warning messagges will be set. The
 *   warnings should not stop an insert or an update but should be provided
 *   to the user as information by a drupal_set_message() if appropriate. The
 *   keys of the array will be name of the field from the options array and the
 *   value is the error message.
 *
 * @return
 *   TRUE for success and FALSE for failure.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_insert_phylotree(&$options, &$errors, &$warnings, $schema_name = 'chado') {
  $chado = \Drupal::service('tripal_chado.database');
  $chado->setSchemaName($schema_name);

  global $user;

  // If no leaf type is provided then assume a taxonomic tree.
  if (!array_key_exists('leaf_type', $options) or !$options['leaf_type']) {
    $options['leaf_type'] = 'taxonomy';
  }

  $options['name_re'] = isset($options['name_re']) ? trim($options['name_re']) : NULL;
  $options['leaf_type'] = trim($options['leaf_type']);
  $options['name'] = trim($options['name']);
  $options['format'] = trim($options['format']);
  $options['tree_file'] = trim($options['tree_file']);
  $options['analysis_id'] = isset($options['analysis_id']) ? $options['analysis_id'] : NULL;

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog but we do for the job and to
  // the terminal
  $options['message_type'] = 'tripal_phylogeny';
  $options['message_opts'] = [
    'watchdog' => FALSE,
    'job' => isset($options['job']) ? $options['job'] : NULL,
    'print' => TRUE,
  ];

  // Validate the incoming options.
  $success = chado_validate_phylotree('insert', $options, $errors, $warnings, $schema_name);
  if (!$success) {
    foreach ($errors as $field => $message) {
      \Drupal::service('tripal.logger')->error($message);
    }
    return FALSE;
  }

  // If we're here then all is good, so add the phylotree record.
  $values = [
    'analysis_id' => $options['analysis_id'],
    'name' => $options['name'],
    'dbxref_id' => $options['dbxref_id'],
    'comment' => $options['description'],
    'type_id' => $options['type_id'],
  ];

  $phylotree = $chado->insert('1:phylotree')
    ->fields($values)
    ->execute();
  $phylotree = $chado->select('1:phylotree', 'phylotree')
    ->fields('phylotree')
    ->condition('phylotree_id', $phylotree)
    ->execute()
    ->fetchAssoc();
  if (!$phylotree) {
    drupal_set_message(t('Unable to add phylotree.'), 'warning');
    \Drupal::service('tripal.logger')->warning(
      'Insert phylotree: Unable to create phylotree where values: %values',
      ['%values' => print_r($values, TRUE)]);
    return FALSE;
  }
  $phylotree_id = $phylotree['phylotree_id'];
  \Drupal::service('tripal.logger')->info(
    'Insert phylotree: Created phylotree with phylotree_id: %phylotree_id',
    ['%phylotree_id' => $phylotree_id]);
  $options['phylotree_id'] = $phylotree_id;

  // If the tree_file is numeric then it is a Drupal managed file and
  // we want to make the file permanent and associated with the tree.
  if (is_numeric($options['tree_file'])) {
    $file = NULL;
    $file = file_load($options['tree_file']);
    $file->status = FILE_STATUS_PERMANENT;
    $file = file_save($file);
    file_usage_add($file, 'tripal_phylogeny', $options['format'], $phylotree_id);
    $real_file_path = drupal_realpath($file->uri);
  }
  else {
    $real_file_path = $options['tree_file'];
  }

  $args = [
    'phylotree_id' => $phylotree_id,
    'leaf_type' => $options['leaf_type'],
    'match' => $options['match'] ? 'uniquename' : 'name',
    'name_re' => $options['name_re'],
    'message_type' => $options['message_type'],
    'message_opts' => $options['message_opts'],
  ];
  chado_phylogeny_import_tree_file($real_file_path, $options['format'], $args, NULL, $schema_name);

  return TRUE;
}

/**
 * Updates a phylotree record into Chado.
 *
 * This function validates the options passed prior to update of the record
 * and if validation passes then any values in the options array that needed
 * validation lookups (such as the dbxref, analysis, leaf_type, etc) will have
 * their approriate primary key values added to the options array. A Drupal
 * File object will be added to the options array for the tree file if one
 * is provided.
 *
 *
 * @param $phylotree_id
 *   The ID of the phylotree to update.
 * @param $options
 *  An array of key value pairs with the following optional keys:
 *     'name':       The name of the tree. This will be displayed to users.
 *     'description: A description about the tree
 *     'analysis_id: The ID of the analysis to which this phylotree should be
 *                   associated.
 *     'analysis':   If the analysis_id key is not used then the analysis name
 *                   may be provided to identify the analysis to which the tree
 *                   should be associated.
 *     'leaf_type':  A sequence ontology term or the word 'taxonomy'. If the
 *                   type is 'taxonomy' then this tree represents a
 *                   taxonomic tree. The default, if not specified, is a
 *                   taxonomic tree.
 *     'tree_file':  The path of the file containing the phylogenetic tree to
 *                   import or a Drupal managed_file numeric ID.
 *     'format':     The file format. Currently only 'newick' is supported
 *     'dbxref':     A database cross-reference of the form DB:ACCESSION.
 *                   Where DB is the database name, which is already present
 *                   in Chado, and ACCESSION is the unique identifier for
 *                   this tree in the remote database.
 *     'name_re':    The value of this field can be a regular expression to pull
 *                   out the name of the feature or organism from the node label
 *                   in the input tree. If no value is provided the entire label
 *                   is used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *     'load_later': If set, the tree will be loaded via a separate Tripal
 *                   jobs call. Otherwise, the tree will be loaded immediately.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_update_phylotree($phylotree_id, &$options, $schema_name = 'chado') {
  // global $user; // Unused variable detected by RISH VSCODE IDE [8/27/2023]

  $chado = \Drupal::service('tripal_chado.database');
  $chado->setSchemaName($schema_name);

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog but we do for the job and to
  // the terminal
  $options['message_type'] = 'tripal_phylogeny';
  $options['message_opts'] = [
    'watchdog' => FALSE,
    'job' => $options['job'],
    'print' => TRUE,
  ];

  // Validate the incoming options.
  $errors = [];
  $warnings = [];
  $success = chado_validate_phylotree('update', $options, $errors, $warnings, $schema_name);
  if (!$success) {
    foreach ($errors as $field => $message) {
      \Drupal::service('tripal.logger')->error($message);
    }
    return FALSE;
  }

  // If we're here then all is good, so update the phylotree record.
  $match = [
    'phylotree_id' => $phylotree_id,
  ];
  if (array_key_exists('name', $options) and $options['name']) {
    $values['name'] = $options['name'];
  }
  if (array_key_exists('analysis_id', $options) and $options['analysis_id']) {
    $values['analysis_id'] = $options['analysis_id'];
  }
  if (array_key_exists('dbxref_id', $options) and $options['dbxref_id']) {
    $values['dbxref_id'] = $options['dbxref_id'];
  }
  if (array_key_exists('description', $options) and $options['description']) {
    $values['comment'] = $options['description'];
  }
  if (array_key_exists('type_id', $options) and $options['type_id']) {
    $values['type_id'] = $options['type_id'];
  }

  $phylotree = chado_update_record('phylotree', $match, $values, ['return_record' => TRUE], $schema_name);
  if (!$phylotree) {
    drupal_set_message(t('Unable to update phylotree.'), 'warning');
    \Drupal::service('tripal.logger')->warning(
      'Update phylotree: Unable to update phylotree where values: %values',
      ['%values' => print_r($values, TRUE)]);
  }

  // If we have a tree file, then import the tree.
  if (array_key_exists('tree_file', $options) and $options['tree_file']) {

    // Remove any existing nodes
    $chado->delete('1:phylonode')
      ->condition('phylotree_id', $options['phylotree_id'])
      ->execute();

    // Make sure if we already have a file that we remove the old one.
    $sql = "
      SELECT FM.fid
      FROM {file_managed} FM
        INNER JOIN {file_usage} FU on FM.fid = FU.fid
      WHERE FU.id = :id and FU.module = 'tripal_phylogeny'
    ";
    $fid = db_query($sql, [':id' => $options['phylotree_id']])->fetchField();
    if ($fid) {
      $file = file_load($fid);
      file_delete($file, TRUE);
    }

    // If the tree_file is numeric then it is a Drupal managed file and
    // we want to make the file permanent and associated with the tree.
    if (is_numeric($options['tree_file'])) {
      $file = file_load($options['tree_file']);
      $file->status = FILE_STATUS_PERMANENT;
      $file = file_save($file);
      file_usage_add($file, 'tripal_phylogeny', 'newick', $options['phylotree_id']);

      // Add a job to parse the new node tree.
      $real_file_path = drupal_realpath($file->uri);
    }
    else {
      $real_file_path = $options['tree_file'];
    }

    $args = [
      'phylotree_id' => $options['phylotree_id'],
      'leaf_type' => $options['leaf_type'],
      'match' => $options['match'] ? 'uniquename' : 'name',
      'name_re' => $options['name_re'],
      'message_type' => $options['message_type'],
      'message_opts' => $options['message_opts'],
    ];
    chado_phylogeny_import_tree_file($real_file_path, $options['format'], $args, NULL, $schema_name);
  }

  return TRUE;
}

/**
 * Deletes a phylotree record from Chado.
 *
 * @param $phylotree_id
 *
 * @return
 *   TRUE on success, FALSE on failure.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_delete_phylotree($phylotree_id, $schema_name = 'chado') {

  $chado = \Drupal::service('tripal_chado.database');
  $chado->setSchemaName($schema_name);

  // If we don't have a phylotree id for this node then this isn't a node of
  // type chado_phylotree or the entry in the chado_phylotree table was lost.
  if (!$phylotree_id) {
    \Drupal::service('tripal.logger')->error('Please provide a phylotree_id to delete a tree');
    return FALSE;
  }

  // Remove the tree
  $values = ['phylotree_id' => $phylotree_id];
  // return chado_delete_record('phylotree', $values, NULL, $schema_name);
  $status = false;
  // RISH [8/27/2023] The below $num_deleted concept is taken from the Drupal Database API for 9.x
  // https://www.drupal.org/docs/drupal-apis/database-api/delete-queries
  $num_deleted = $chado->delete('1:phylotree')
    ->condition('phylotree_id', $phylotree_id)
    ->execute();
  return TRUE;

}

/**
 * Iterates through the tree and sets the left and right indices.
 *
 * @param $tree
 *   The tree array.
 * @param $index
 *   This parameters is not used when the function is first called. It
 *   is used for recursive calls.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_assign_phylogeny_tree_indices(&$tree, &$index = 1) {
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
      chado_assign_phylogeny_tree_indices($tree['branch_set'][$key], $index);
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
 *   -organism_id:  The organism_id for associating the node with an organism.
 *   -properties:   An array of key/value pairs where the key is the cvterm_id
 *                  and the value is the property value.  These properties
 *                  will be associated with the phylonode.
 *
 * Prior to importing the tree the indices can be set by using the
 * chado_assign_phylogeny_tree_indices() function.
 *
 * @param $tree
 *   The tree array.
 * @param $phylotree .
 *   The phylotree object (from Chado).
 * @param $options
 *   The options provide some direction for how the tree is imported.  The
 *   following keys can be used:
 *   -leaf_type: Set to the leaf type name. If this is a non-taxonomic tree
 *               that is associated with features, then this should be the
 *               Sequence Ontology term for the feature (e.g. polypeptide).
 *               If this is a taxonomic tree then set to 'taxonomy'.
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
 *
 * @ingroup tripal_phylotree_api
 */
function chado_phylogeny_import_tree(&$tree, $phylotree, $options, $vocab = [], $parent = NULL, $schema_name = 'chado') {
  $chado = \Drupal::service('tripal_chado.database');
  $chado->setSchemaName($schema_name);

  // Used for final summary message at end of recursion.
  static $n_associated = 0;
  static $n_not_associated = 0;

  // Get the vocabulary terms used to describe nodes in the tree if one
  // wasn't provided.
  if (count($vocab) == 0) {
    $vocab = chado_phylogeny_get_node_types_vocab($options, $schema_name);
  }

  if (is_array($tree) and array_key_exists('name', $tree)) {
    $values = [
      'phylotree_id' => $phylotree->phylotree_id,
      'left_idx' => $tree['left_index'],
      'right_idx' => $tree['right_index'],
    ];
    // Add in any optional values to the $values array if they are present.
    if (!empty($tree['name']) and $tree['name'] != '') {
      $values['label'] = $tree['name'];
    }
    if (!empty($tree['length']) and $tree['length'] != '') {
      $values['distance'] = $tree['length'];
    }
    // Set the type of node.
    // echo "DEBUG Check is_root\n";
    if (isset($tree['is_root']) && $tree['is_root'] == true) {
      $values['type_id'] = $vocab['root']->cvterm_id;
    }
    else {
      // echo "DEBUG Check is_internal\n";
      if (isset($tree['is_internal']) && $tree['is_internal'] == true) {
        $values['type_id'] = $vocab['internal']->cvterm_id;
        $values['parent_phylonode_id'] = $parent['phylonode_id'];
        // TODO: a feature may be associated here but it is recommended that it
        // be a feature of type SO:match and should represent the alignment of
        // all features beneath it.
      }
      else {
        if (isset($tree['is_leaf']) && $tree['is_leaf']) {
          $values['type_id'] = $vocab['leaf']->cvterm_id;
          $values['parent_phylonode_id'] = $parent['phylonode_id'];

          // Match this leaf node with an organism or feature depending on the
          // type of tree. But we can't do that if we don't have a name.
          if (!empty($tree['name']) and $tree['name'] != '') {
            if (!($options['leaf_type'] == 'taxonomy')) {

              // This is a sequence-based tree. Try to match leaf nodes with
              // features.
              // First, get the name and uniquename for the feature.
              $matches = [];
              $sel_values = [];
              if ($options['match'] == "name") {
                $sel_values['name'] = $tree['name'];
                $re = $options['name_re'];
                if (($re) and (preg_match("/$re/", $tree['name'], $matches))) {
                  $sel_values['name'] = $matches[1];
                }
              }
              else {
                $sel_values['uniquename'] = $tree['name'];
                $re = $options['name_re'];
                if (($re) and (preg_match("/$re/", $tree['name'], $matches))) {
                  $sel_values['uniquename'] = $matches[1];
                }
              }
              $sel_values['type_id'] = [
                'name' => $options['leaf_type'],
                'cv_id' => [
                  'name' => 'sequence',
                ],
              ];
              $sel_columns = ['feature_id'];

              // Find the cv_id for sequence
              $cv_id = $chado->select('1:cv', 'cv')->fields('cv')->condition('name', 'sequence')->execute()->fetchObject()->cv_id;

              // Find the cvterm_id for $options['leaf_type']
              $cvterm_id = $chado->select('1:cvterm', 'cvterm')->fields('cvterm')
                ->condition('name', $options['leaf_type'])
                ->condition('cv_id', $cv_id)
                ->execute()
                ->fetchObject()->cvterm_id;

              $feature = $chado->select('1:feature', 'feature')
                ->fields('feature')
                ->condition('type_id', $cvterm_id);
              if (isset($sel_values['name'])) {
                $feature = $feature->condition('name', $sel_values['name']);
              }
              else if (isset($sel_values['uniquename'])) {
                $feature = $feature->condition('uniquename', $sel_values['uniquename']);
              }

              $feature = $feature->execute()
                ->fetchAll();

              if (count($feature) > 1) {
                // Found multiple features, cannot make an association.
                \Drupal::service('tripal.logger')->warning('Import phylotree: Warning, unable to associate to a feature,'
                    . ' more than one feature matches the %matchtype: %value',
                    ['%matchtype' => $options['match'], '%value' => $sel_values[$options['match']] ]);
              }
              else {
                if (count($feature) == 1) {
                  $values['feature_id'] = $feature[0]->feature_id;
                  $n_associated++;
                  \Drupal::service('tripal.logger')->info('Import phylotree: Associated'
                    . ' %value by %matchtype to feature_id: %fid',
                    ['%matchtype' => $options['match'],
                     '%value' => $sel_values[$options['match']],
                     '%fid' => $values['feature_id'] ]);
                }
                else {
                  // Could not find a feature that matches the name or uniquename
                  $n_not_associated++;
                  \Drupal::service('tripal.logger')->warning('Import phylotree: Warning, unable to associate to a'
                    . ' feature that matches the %matchtype: %value',
                    ['%matchtype' => $options['match'],
                     '%value' => $sel_values[$options['match']] ]);
                }
              }
            }
            else {
              // This is a taxonomy tree. Try to match leaf nodes with organisms.
              $organism_name = $tree['name'];
              $re = isset($options['name_re']) ? $options['name_re'] : NULL;
              if (($re) and (preg_match("/$re/", $organism_name, $matches))) {
                $organism_name = $matches[1];
              }
              $organism_id = chado_phylogeny_lookup_organism_by_name($organism_name, $schema_name);
              if ($organism_id) {
                $tree['organism_id'] = $organism_id;
                $n_associated++;
                \Drupal::service('tripal.logger')->info(
                  'Import phylotree: Associated %name to organism_id: %organism_id',
                  ['%name' => $tree['name'], '%organism_id' => $organism_id]);
              }
              else {
                $n_not_associated++;
                \Drupal::service('tripal.logger')->warning('Import phylotree: Warning, unable to'
                  . ' associate to an organism that matches %name',
                  ['%name' => $tree['name']]);
              }
            }
          }
        }
      }
    }

    // Insert the new node and then add its assigned phylonode_id to the node.
    // $phylonode = chado_insert_record('phylonode', $values, [], $schema_name);
    $phylonode = $chado->insert('1:phylonode')->fields($values)->execute();
    // Get the phylonode record
    $phylonode = $chado->select('1:phylonode', 'p')->fields('p')->condition('phylonode_id', $phylonode)->execute()->fetchAssoc();

    $tree['phylonode_id'] = $phylonode['phylonode_id'];

    // This is a taxonomic tree, so associate this node with an
    // organism if one is provided.
    if (array_key_exists('organism_id', $tree)) {
      $values = [
        'phylonode_id' => $tree['phylonode_id'],
        'organism_id' => $tree['organism_id'],
      ];
      $phylonode_organism = $chado->insert('1:phylonode_organism')->fields($values)->execute();
    }

    // Associate any properties.
    if (array_key_exists('properties', $tree)) {
      foreach ($tree['properties'] as $type_id => $value) {
        $values = [
          'phylonode_id' => $tree['phylonode_id'],
          'type_id' => $type_id,
          'value' => $value,
        ];
        $phylonode_organism = $chado->insert('1:phylonodeprop')->fields($values)->execute();
      }
    }
  }
  if (is_array($tree) and array_key_exists('branch_set', $tree)) {
    foreach ($tree['branch_set'] as $key => $node) {
      chado_phylogeny_import_tree($tree['branch_set'][$key], $phylotree, $options, $vocab, $tree, $schema_name);
    }
  }
  // Report summary status of association of leaf nodes at end of recursion.
  if (!$parent) {
    \Drupal::service('tripal.logger')->info('Import phylotree summary: %n_associated nodes'
      . ' were successfully associated to content, %n_not_associated nodes could not be associated',
      ['%n_associated' => $n_associated, '%n_not_associated' => $n_not_associated]);
  }
}

/**
 * Lookup an organism_id given an organism name (genus species)
 *
 * @param $name
 *   The organism name. Infraspecific type abbreviation is allowed.
 *
 * @return
 *   organism_id from chado.organism table on success, or FALSE on failure.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_phylogeny_lookup_organism_by_name($name, $schema_name = 'chado') {
  // Spaces in names are often replaced with underscores in newick files.
  $name = trim(preg_replace('/_/', ' ', $name ));

  // Call api function to look up organism_id, case insensitive.
  $organism_id = chado_get_organism_id_from_scientific_name($name, [], $schema_name);

  // The unique constraint on the organism table ensures we get
  // either zero or one record returned.
  if (empty($organism_id)) {
    return FALSE;
  }
  else {
    return $organism_id[0];
  }
}

/**
 * Get the vocabulary terms used to describe nodes in the tree.
 *
 * @return
 *  Array of vocab info or FALSE on failure.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_phylogeny_get_node_types_vocab($options, $schema_name = 'chado') {
  // Get the three default vocabulary terms used to describe nodes in the tree.
  $terms = ['leaf' => 'phylo_leaf', 'internal' => 'phylo_interior', 'root' => 'phylo_root'];
  $vocab = [];
  foreach ($terms as $key => $name) {
    $values = [
      'name' => $name,
      'cv_id' => [
        'name' => 'local',
      ],
    ];
    $cvterm = chado_generate_var('cvterm', $values, [], $schema_name);
    if (!$cvterm) {
      \Drupal::service('tripal.logger')->error(
        "Could not find the leaf vocabulary term: '%name'. It should " .
        "already be present as part of the local vocabulary.",
        ['%name' => $name], $options['message_opts']);
      return FALSE;
    }
    $vocab[$key] = $cvterm;
  }
  return $vocab;
}


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
 *     'leaf_type':  A sequence ontology term or the word 'taxonomy'. If the
 *                   type is 'taxonomy' then this tree represents a
 *                   taxonomic tree. The default, if not specified, is a
 *                   taxonomic tree.
 *     'name_re':    The value of this field can be a regular expression to pull
 *                   out the name of the feature or organism from the node label
 *                   in the input tree. If no value is provided the entire label
 *                   is used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *
 * @ingroup tripal_phylotree_api
 */
function chado_phylogeny_import_tree_file($file_name, $format, $options = [], $job_id = NULL, $schema_name = 'chado') {

  // Set some option details.
  if (!array_key_exists('leaf_type', $options)) {
    $options['leaf_type'] = 'taxonomy';
  }
  if (!array_key_exists('match', $options)) {
    $options['match'] = 'name';
  }
  if (!array_key_exists('name_re', $options) or (!$options['name_re'])) {
    $options['name_re'] = '^(.*)$';
  }
  $options['name_re'] = trim($options['name_re']);

  // We want the job object in order to report progress with the job, unless
  // it was passed in through $options
  if (!array_key_exists('message_type', $options)) {
    $options['message_type'] = 'tripal_phylogeny';
  }
  if (!array_key_exists('message_opts', $options)) {
    if (is_numeric($job)) {
      $job_id = $job;
      $job = new TripalJob();
      $job->load($job_id);
    }
    $options['message_opts'] = [
      'watchdog' => FALSE,
      'job' => $job,
      'print' => TRUE,
    ];
  }

  // If a phylotree ID is not passed in then make sure we have the other
  // required fields for creating a tree.
  if (!array_key_exists('phylotree_id', $options)) {
    if (!array_key_exists('name', $options)) {
      \Drupal::service('tripal.logger')->error(
        'The phylotree_id is required for importing the tree.'
      );
      return FALSE;
    }
  }

  // Get the phylotree record.
  $values = ['phylotree_id' => $options['phylotree_id']];
  $phylotree = chado_generate_var('phylotree', $values, [], $schema_name);

  if (!$phylotree) {
    \Drupal::service('tripal.logger')->error(
      'Could not find the phylotree using the ID provided: %phylotree_id.',
      ['%phylotree_id' => $options['phylotree_id']], $options['message_opts']);
    return FALSE;
  }

  // $transaction = db_transaction(); // OLD T3
  $chado = \Drupal::service('tripal_chado.database');
  $chado->setSchemaName($schema_name);
  $transaction_chado = $chado->startTransaction();
  // print "\nNOTE: Loading of this tree file is performed using a database transaction. \n" .
  //   "If the load fails or is terminated prematurely then the entire set of \n" .
  //   "insertions/updates is rolled back and will not be found in the database\n\n";
  try {


    // Parse the file according to the format indicated.
    if ($format == 'newick') {
      // TODO: [RISH] Discussed with Stephen Ficklin 24th May 2023
      // To be upgraded at a later time

      // // Parse the tree into the expected nested node format.
      // echo "Parsing newick file...\n";
      // T3 OLD CODE
      // module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.phylotree_newick');
      // T4 - this file is added as an API file
      $tree = tripal_phylogeny_parse_newick_file($file_name);
      // // Assign the right and left indices to the tree nodes.
      chado_assign_phylogeny_tree_indices($tree);
    }

    // Iterate through the tree nodes and add them to Chado in accordance
    // with the details in the $options array.
    chado_phylogeny_import_tree($tree, $phylotree, $options, [], NULL, $schema_name);
  } catch (Exception $e) {
    // $transaction->rollback(); // OLD T3
    $transaction_chado->rollback();
    watchdog_exception($options['message_type'], $e);
  }
}
