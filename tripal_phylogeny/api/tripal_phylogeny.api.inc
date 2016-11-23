<?php

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
 *   either the tripal_insert_phylotree() or tripal_update_phylotree()
 *   functions.
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
 * @return
 *   If validation failes then FALSE is returned.  Any options that do not pass
 *   validation checks will be added in the $errors array with the key being
 *   the option and the value being the error message.  If validation
 *   is successful then TRUE is returned.
 *
 */
function tripal_validate_phylotree($val_type, &$options, &$errors, &$warnings) {

  if ($val_type != 'insert' and $val_type != 'update') {
    tripal_report_error('tripal_phylogeny', TRIPAL_ERROR, "The $val_type argument must be either 'update or 'insert'.");
  }

  // Set Defaults.
  if ($val_type == 'insert') {
    // Match by feature name.
    if (!array_key_exists('match', $options)) {
      $options['match'] = 'name';
    }
    // The regular expression is to match the entire node name.
    if (!array_key_exists('name_re', $options)) {
      $options['name_re'] = '^(.*)$';
    }
    // A dbxref is not required by Tripal but is required by the database
    // field in the phylotree table.  Therefore, if the dbxref is not provided
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
    if (!array_key_exists('analysis', $options) and !array_key_exists('analysis_id', $options)) {
      $errors['analysis'] = t('Please provide an analysis or analysis_id for this tree.');
      return FALSE;
    }
    if (!array_key_exists('tree_file', $options)) {
      $errors['tree_file'] = t('Please provide either the full path to the tree_file or a Drupal managed file ID number.');
      return FALSE;
    }
    if (!array_key_exists('format', $options) or !$options['format']) {
      $errors['format'] = t('Please provide a file format for the tree file.');
      return FALSE;
    }
    // Make sure the file format is correct
    if ($options['format'] != 'newick' and $options['format'] != 'taxonomy') {
      $errors['format'] = t('The file format is not supported. Currently only the "newick" file format is supported.');
      return FALSE;
    }
  }
  else {
    // Does the phylotree ID exist and is it valid
    if (!array_key_exists('phylotree_id', $options)) {
      $errors['phylotree_id'] = t('Please provide the ID for the tree.');
      return FALSE;
    }
    $exists = chado_select_record('phylotree', array('phylotree_id'),
        array('phylotree_id' => $options['phylotree_id']), array('has_record' => 1));
    if (!$exists) {
      $errors['phylotree_id'] = t('The phylotree_id does not exist.');
      return FALSE;
    }

  }

  // Make sure the file exists if one is specified
  if (array_key_exists('tree_file', $options) and $options['tree_file']) {
    // If this is a numeric Drupal file then all is good, no need to check.
    if (!is_numeric($options['tree_file'])) {
      if (!file_exists($options['tree_file'])) {
        $errors['tree_file'] = t('The file provided does not exists.');
        return FALSE;
      }
    }
    // Make sure the file format is correct
    if (!array_key_exists('format', $options) or
        ($options['format'] != 'newick' and $options['format'] != 'taxonomy')) {
      $errors['format'] = t('Please provide a supported file format. Currently only the "newick" file format is supported.');
      return FALSE;
    }

    // If no leaf type is provided then use the polypeptide term.
    if (!array_key_exists('leaf_type', $options) or !$options['leaf_type']) {
      $options['leaf_type'] = 'polypeptide';
    }
  }

  // Make sure the analysis exists.
  $analysis = NULL;
  if (array_key_exists('analysis_id', $options) and $options['analysis_id']) {
    $analysis = chado_select_record('analysis', array('analysis_id'), array('analysis_id' => $options['analysis_id']));
    if (!$analysis) {
      $errors['analysis_id'] = t('The analysis name provided does not exist.');
      return FALSE;
    }
    $options['analysis_id'] = $analysis[0]->analysis_id;
  }
  if (array_key_exists('analysis', $options) and $options['analysis']) {
    $analysis = chado_select_record('analysis', array('analysis_id'), array('name' => $options['analysis']));
    if (!$analysis) {
      $errors['analysis'] = t('The analysis ID provided does not exist.');
      return FALSE;
    }
    $options['analysis_id'] = $analysis[0]->analysis_id;
  }

  // Make sure the leaf type exists.
  $type = NULL;
  if (array_key_exists('leaf_type', $options) and $options['leaf_type']) {
    if ($options['leaf_type'] == 'taxonomy') {
      $values = array(
        'cv_id' => array(
           'name' => 'tripal_phylogeny'
        ),
        'name' => 'taxonomy'
      );
      $type = chado_select_record('cvterm', array('cvterm_id'), $values);
    }
    else {
      $values = array(
        'cv_id' => array(
           'name' => 'sequence'
        ),
        'name' => $options['leaf_type']
      );
      $type = chado_select_record('cvterm', array('cvterm_id'), $values);
      if (!$type) {
        $errors['leaf_type'] = t('The leaf_type provided is not a valid Sequence Ontology term: %term.');
        return FALSE;
      }
    }
    $options['type_id'] = $type[0]->cvterm_id;
  }

  // A Dbxref is required by the phylotree module, but if the
  // tree was generated in-house and the site admin doens't want to
  // assign a local dbxref then we will set it to the null db
  // and the local:null dbxref.
  if (array_key_exists('dbxref', $options)) {
    if (!$options['dbxref']) {
        $options['dbxref'] = 'null:local:null';
    }
    $matches = array();
    preg_match('/^(.*?):(.*)$/', $options['dbxref'], $matches);
    $db_name = $matches[1];
    $accession = $matches[2];
    $values = array(
      'accession' => $accession,
      'db_id' => array(
        'name' => $db_name
      ),
    );
    $dbxref = chado_generate_var('dbxref', $values);
    if (!$dbxref) {
      $errors['dbxref'] = t('The dbxref provided does not exist in the database: %dbxref.', array('%dbxref' => $dbxref));
      return FALSE;
    }
    $options['dbxref_id'] = $dbxref->dbxref_id;
  }

  // Make sure the tree name is unique
  if (array_key_exists('name', $options) and $options['name']) {
    $sql = "
      SELECT *
      FROM {phylotree} P
      WHERE
        P.name = :name
    ";
    $args = array(':name' => $options['name']);
    if ($val_type == 'update') {
      $sql .= " AND NOT P.phylotree_id = :phylotree_id";
      $args[':phylotree_id'] = $options['phylotree_id'];
    }
    $result = chado_query($sql, $args)->fetchObject();
    if ($result) {
      $errors['name'] = t("The tree name is in use by another tree. Please provide a different unique name for this tree.");
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
 *     'anlaysis_id: The ID of the analysis to which this phylotree should be
 *                   associated.
 *     'analysis':   If the analysis_id key is not used then the analysis name
 *                   may be provided to identify the analysis to which the tree
 *                   should be associated.
 *     'leaf_type':  A sequence ontology term or the word 'organism'. If the
 *                   type is 'organism' then this tree represents a
 *                   taxonomic tree.  The default, if not specified, is the
 *                   term 'polypeptide'.
 *     'tree_file':  The path of the file containing the phylogenetic tree to
 *                   import or a Drupal managed_file numeric ID.
 *     'format':     The file format. Currently only 'newick is supported'
 *  Optional keys:
 *     'dbxref':     A database cross-reference of the form DB:ACCESSION.
 *                   Where DB is the database name, which is already present
 *                   in Chado, and ACCESSION is the unique identifier for
 *                   this tree in the remote database.
 *     'name_re':    If the leaf type is NOT 'taxonomy', then the value of
 *                   this field can be a regular expression to pull out
 *                   the name of the feature from the node label in the
 *                   intput tree. If no value is provided the entire label is
 *                   used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *     'load_now':   If set, the tree will be loaded immediately if a tree_file
 *                   is provided. Otherwise, the tree will be loaded via
 *                   a Tripal jobs call.
 *     'no_load':    If set the tree file will not be loaded.
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
 * @return
 *   TRUE for success and FALSE for failure.
 */
function tripal_insert_phylotree(&$options, &$errors, &$warnings) {
  global $user;

  $options['name_re'] = trim($options['name_re']);
  $options['leaf_type'] = trim($options['leaf_type']);
  $options['name'] = trim($options['name']);
  $options['format'] = trim($options['format']);
  $options['tree_file'] = trim($options['tree_file']);

  // Validate the incoming options.
  $success = tripal_validate_phylotree('insert', $options, $errors, $warnings);
  if (!$success) {
    foreach ($errors as $field => $message) {
      tripal_report_error('tripal_phylogeny', TRIPAL_ERROR, $message);
    }
    return FALSE;
  }

  // If we're here then all is good, so add the phylotree record.
  $values = array(
    'analysis_id' => $options['analysis_id'],
    'name' => $options['name'],
    'dbxref_id' => $options['dbxref_id'],
    'comment' => $options['description'],
    'type_id' => $options['type_id'],
  );
  $phylotree = chado_insert_record('phylotree', $values);
  if (!$phylotree) {
    drupal_set_message(t('Unable to add phylotree.'), 'warning');
    tripal_report_error('tripal_phylogeny', TRIPAL_WARNING, 'Insert phylotree: Unable to create phylotree where values: %values',
        array('%values' => print_r($values, TRUE)));
    return FALSE;
  }
  $phylotree_id = $phylotree['phylotree_id'];
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

  // If caller has requested to load the file now then do so, otherwise
  // submit using a Tripal job.
  if (!array_key_exists('no_load', $options) or !$options['no_load']) {
    if (array_key_exists('load_now', $options) and $options['load_now']) {
      $args = array(
        'phylotree_id' => $phylotree_id,
        'leaf_type' => $options['leaf_type'],
        'match' => $options['match'] ? 'uniquename' : 'name',
        'name_re' => $options['name_re'],
      );
      tripal_phylogeny_import_tree_file($real_file_path, $options['format'], $args);
    }
    else {
      $args = array(
          $real_file_path,
          'newick',
          array(
            'phylotree_id' => $phylotree_id,
            'leaf_type' => $options['leaf_type'],
            'match' => $options['match'] ? 'uniquename' : 'name',
            'name_re' => $options['name_re'],
          ),
      );
      if (tripal_add_job("Import Tree File: " . $file->filename, 'tripal_phylogeny',
          'tripal_phylogeny_import_tree_file', $args, $user->uid)) {
          drupal_set_message(t('The tree visualizations will appear once the tree is fully imported.'));
      }
    }
  }

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
 *     'anlaysis_id: The ID of the analysis to which this phylotree should be
 *                   associated.
 *     'analysis':   If the analysis_id key is not used then the analysis name
 *                   may be provided to identify the analysis to which the tree
 *                   should be associated.
 *     'leaf_type':  A sequence ontology term or the word 'organism'. If the
 *                   type is 'organism' then this tree represents a
 *                   taxonomic tree.  The default, if not specified, is the
 *                   term 'polypeptide'.
 *     'tree_file':  The path of the file containing the phylogenetic tree to
 *                   import or a Drupal managed_file numeric ID.
 *     'format':     The file format. Currently only 'newick is supported'
 *     'dbxref':     A database cross-reference of the form DB:ACCESSION.
 *                   Where DB is the database name, which is already present
 *                   in Chado, and ACCESSION is the unique identifier for
 *                   this tree in the remote database.
 *     'name_re':    If the leaf type is NOT 'taxonomy', then the value of
 *                   this field can be a regular expression to pull out
 *                   the name of the feature from the node label in the
 *                   intput tree. If no value is provided the entire label is
 *                   used.
 *     'match':      Set to 'uniquename' if the leaf nodes should be matched
 *                   with the feature uniquename.
 *     'load_now':   If set, the tree will be loaded immediately if a tree_file
 *                   is provided. Otherwise, the tree will be loaded via
 *                   a Tripal jobs call.
 */
function tripal_update_phylotree($phylotree_id, &$options) {
  global $user;

  // Validate the incoming options.
  $errors = array();
  $warnings = array();
  $success = tripal_validate_phylotree('update', $options, $errors, $warnings);
  if (!$success) {
    foreach ($errors as $field => $message) {
      tripal_report_error('tripal_phylogeny', TRIPAL_ERROR, $message);
    }
    return FALSE;
  }

  // If we're here then all is good, so update the phylotree record.
  $match = array(
      'phylotree_id' => $phylotree_id,
  );
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

  $phylotree = chado_update_record('phylotree', $match, $values, array('return_record' => TRUE));
  if (!$phylotree) {
    drupal_set_message(t('Unable to update phylotree.'), 'warning');
    tripal_report_error('tripal_phylogeny', TRIPAL_WARNING,
        'Update phylotree: Unable to update phylotree where values: %values',
        array('%values' => print_r($values, TRUE))
    );
  }

  // If we have a tree file, then import the tree
  if (array_key_exists('tree_file', $options) and $options['tree_file']) {

    // Remove any existing nodes
    chado_delete_record('phylonode', array('phylotree_id' => $options['phylotree_id']));

    // Make sure if we already have a file that we remove the old one.
    $sql = "
      SELECT FM.fid
      FROM {file_managed} FM
        INNER JOIN {file_usage} FU on FM.fid = FU.fid
      WHERE FU.id = :id and FU.module = 'tripal_phylogeny'
    ";
    $fid = db_query($sql, array(':id' => $options['phylotree_id']))->fetchField();
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

    // If caller has requested to load the file now then do so, otherwise
    // submit using a Tripal job.
    if (array_key_exists('load_now', $options) and $options['load_now']) {
      $args = array(
        'phylotree_id' => $options['phylotree_id'],
        'leaf_type' => $options['leaf_type'],
        'match' => $options['match'] ? 'uniquename' : 'name',
        'name_re' => $options['name_re'],
      );
      tripal_phylogeny_import_tree_file($real_file_path, $options['format'], $args);
    }
    else {
      $args = array(
        $real_file_path,
        'newick',
        array(
          'phylotree_id' => $options['phylotree_id'],
          'leaf_type' => $options['leaf_type'],
          'match' => $options['match'] ? 'uniquename' : 'name',
          'name_re' => $options['name_re'],
        ),
      );
      if (tripal_add_job("Import Tree File: " . $file->filename, 'tripal_phylogeny',
          'tripal_phylogeny_import_tree_file', $args, $user->uid)) {
        drupal_set_message(t('The tree visualizations will appear once the tree is fully imported.'));
      }
    }
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
 */
function tripal_delete_phylotree($phylotree_id) {

  // if we don't have a phylotree id for this node then this isn't a node of
  // type chado_phylotree or the entry in the chado_phylotree table was lost.
  if (!$phylotree_id) {
    tripal_report_error('tripal_phylogeny', TRIPAL_ERROR,
        'Please provide a phylotree_id to delete a tree.');
    return FALSE;
  }

  // Remove the tree
  $values = array('phylotree_id' => $phylotree_id);
  return chado_delete_record('phylotree', $values);
}