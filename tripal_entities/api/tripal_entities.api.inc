<?php

/**
 * TODO: The code for creating the title needs to be updated to not
 * use nodes but rather entities.
 *
 * @param unknown $node
 * @return mixed
 */
function chado_get_entity_title($entity) {

  // Get the base table for the entity
  $details = db_select('chado_entity', 'ce')
    ->fields('ce')
    ->condition('entity_id', $entity->id)
    ->execute()
    ->fetchObject();

  $tablename = $details->data_table;
  $type_field = $details->field;
  $schema = chado_get_schema($tablename);
  $pkey_field = $schema['primary key'][0];
  $record_id = $details->record_id;

  $record = chado_generate_var($tablename, array($pkey_field => $record_id));

  // TODO: fix this so it's native for entities and doesn't expect nodes.
  // Fake a node
  $node = new stdClass();
  $node->$tablename = $record;

  // Get the tokens and format
  $tokens = array(); // this will be set by chado_node_get_title_format
  $title = chado_node_get_title_format('chado_' . $tablename, $tokens);

  // Determine which tokens were used in the format string
  if (preg_match_all('/\[[^]]+\]/', $title, $used_tokens)) {

    // Get the value for each token used
    foreach ($used_tokens[0] as $token) {
      $token_info = $tokens[$token];
      if (!empty($token_info)) {
        $value = chado_get_token_value($token_info, $node);
        $title = str_replace($token, $value, $title);
      }
    }
  }
  else {
    return $title;
  }

  return $title;
}

/**
 * Creates a new Tripal Entity type (i.e. bundle).
 *
 * An entity is created by first adding a record to the tripal_term and
 * tripal_vocabulary tables.  This helps Tripal keep track of which terms
 * are used in which tables.  Second, records are added to the tripal_bundle
 * table which is where Tripal keeps track of all of the entity_types. Finally,
 * the hook_add_bundle_fields() is called that allows other modules to
 * add fields to the entity.
 *
 * @param $cvterm
 *  A cvterm object created using the chado_generate_var() function.
 * @param $error
 *  A string, passed by reference, that is filled with the error message
 *  if the function fails.
 *
 * @return
 *  TRUE if the entity type (bundle) was succesfully created.  FALSE otherwise.
 */
function tripal_create_entity_type($cvterm, &$error = '') {

  // Before creating the entity we must add records to the tripal_vocabulary
  // tripal_vocabulary_usage, tripal_term, and tripal_term_usage tables.
  $match = array('cv_id' => $cvterm->cv_id->cv_id);
  $vocab = chado_select_record('tripal_vocabulary', array('*'), $match);
  if (count($vocab) == 0) {
    $values = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'db_id' => $cvterm->dbxref_id->db_id->db_id,
      'publish' => 1,
    );
    $values = chado_insert_record('tripal_vocabulary', $values);
    if (!$values) {
      $error = 'Could not add vocabulary to tripal_vocabluary table.';
      return FALSE;
    }
    // Convert the values array into an object.
    $vocab = new stdClass();
    $vocab->vocabulary_id = $values['vocabulary_id'];
    $vocab->cv_id = $values['cv_id'];
  }
  else {
    // Make sure the vocabulary is set to publish
    $values = array('publish' => 1);
    chado_update_record('tripal_vocabulary', $match, $values);
    $vocab = $vocab[0];
  }

  // Look to see if this vocabulary is used as a default for any table. If
  // so then we can use that to populate the tripal_vocabulary_usage table.
  $default = db_select('tripal_cv_defaults', 't')
    ->fields('t')
    ->condition('cv_id', $vocab->cv_id)
    ->execute()
    ->fetchObject();
  if ($default) {
    $values = array(
      'vocabulary_id' => $vocab->vocabulary_id,
      'data_table' => $default->table_name,
      'type_table' => $default->table_name,
      'field' =>  $default->field_name,
    );
    $vocab_usage = chado_select_record('tripal_vocabulary_usage', array('*'), $values);
    if (count($vocab_usage) == 0) {
      $values = chado_insert_record('tripal_vocabulary_usage', $values);
      if (!$values) {
        $error = 'Could not add vocabulary to tripal_vocabulary_usage table.';
        return FALSE;
      }
      $vocab_usage = new stdClass();
      $vocab_usage->vocabulary_id = $values['vocabulary_id'];
      $vocab_usage->data_table = $values['data_table'];
      $vocab_usage->type_table = $values['type_table'];
      $vocab_usage->field = $values['field'];
    }
  }
  // The organism table does not have a type_id so we won't ever find
  // a record for it in the tripal_cv_defaults table.
  else if ($cvterm->name == 'organism') {
    $values = array(
      'vocabulary_id' => $vocab->vocabulary_id,
      'data_table' => 'organism',
      'type_table' => 'organism',
      'field' =>  '',
    );
    $vocab_usage = chado_select_record('tripal_vocabulary_usage', array('*'), $values);
    if (count($vocab_usage) == 0) {
      $values = chado_insert_record('tripal_vocabulary_usage', $values);
      if (!$values) {
        $error = 'Could not add vocabulary to tripal_vocabulary_usage table.';
        return FALSE;
      }
      $vocab_usage = new stdClass();
      $vocab_usage->vocabulary_id = $values['vocabulary_id'];
      $vocab_usage->data_table = $values['data_table'];
      $vocab_usage->type_table = $values['type_table'];
      $vocab_usage->field = $values['field'];
    }
  }
  // The analysis table does not have a type_id so we won't ever find
  // a record for it in the tripalcv_defaults table.
  else if ($cvterm->name == 'analysis') {
    $values = array(
      'vocabulary_id' => $vocab->vocabulary_id,
      'data_table' => 'analysis',
      'type_table' => 'analysis',
      'field' =>  '',
    );
    $vocab_usage = chado_select_record('tripal_vocabulary_usage', array('*'), $values);
    if (count($vocab_usage) == 0) {
      $values = chado_insert_record('tripal_vocabulary_usage', $values);
      if (!$values) {
        $error = 'Could not add vocabulary to tripal_vocabulary_usage table.';
        return FALSE;
      }
      $vocab_usage = new stdClass();
      $vocab_usage->vocabulary_id = $values['vocabulary_id'];
      $vocab_usage->data_table = $values['data_table'];
      $vocab_usage->type_table = $values['type_table'];
      $vocab_usage->field = $values['field'];
    }
  }
  // If there is no default table then we have an error, and we should
  // set a variable so that the form can help the user deal with the problem.
  else {
    $error = t('There is no default mapping of this term\'s ' .
        'vocabulary to a table in Chado.  Therefore, it is not possible to ' .
        'determine how to store data of this type.');
    return FALSE;
  }

  // Now add the tripal_term record if it doesn't already exist.
  $match = array(
    'vocabulary_id' => $vocab->vocabulary_id,
    'cvterm_id' => $cvterm->cvterm_id,
  );
  $term = chado_select_record('tripal_term', array('*'), $match);
  if (count($term) == 0) {
    $values = array(
      'vocabulary_id' => $vocab->vocabulary_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'publish' => 1
    );
    $values = chado_insert_record('tripal_term', $values);
    if (!$values) {
      $error =  'Could not add term to tripal_term table.';
      return FALSE;
    }
    $term = new stdClass();
    $term->term_id = $values['term_id'];
  }
  else {
    $values = array('publish' => 1);
    chado_update_record('tripal_term', $match, $values);
    $term = $term[0];
  }

  // Finally, add the tripal_term_usage record if it doesn't already exist.
  $match = array('term_id' => $term->term_id);
  $options = array('has_record' => TRUE);
  if (!chado_select_record('tripal_term_usage', array('*'), $match, $options)) {
    $values = array(
      'term_id' => $term->term_id,
      'data_table' => $vocab_usage->data_table,
      'type_table' => $vocab_usage->type_table,
      'field' =>  $vocab_usage->field,
    );
    $values = chado_insert_record('tripal_term_usage', $values);
    if (!$values) {
      $error = 'Could not add term to tripal_term table.';
      return FALSE;
    }
  }

  // Clear the entity cache so that Drupal will read our
  // hook_entity_info() implementation which now will have the entities
  // described because we set the publish column to 1 in the tripal_term
  // table.
  global $language;
  $langcode = $language->language;
  cache_clear_all("entity_info:$langcode", 'cache');

  // Create the bundle name and entity type name.  The bundle name is the
  // dbxref ID.  This isn't very human readable, but the alternative is to
  // use the accession which may not always be alpha-numeric.
  $bundle_name = 'dbxref_' . $cvterm->dbxref_id->dbxref_id;

  // Check to see if this bundle exists. If not then create it
  $bundle = db_select('tripal_bundle', 't')
    ->fields('t')
    ->condition('type', 'BioData')
    ->condition('bundle', $bundle_name)
    ->execute()
    ->fetchObject();

  if (!$bundle) {
    // The TripalBundle Entity manages the bundles we have available.
    // Therefore, we need to add a new entity for each bundle "type".
    $vals = array(
      'label' => $cvterm->name,
      'type' => 'BioData',
      'bundle' => $bundle_name,
      'data' => serialize(array()),
      'module' => 'tripal_entities'
    );
    $tripal_bundle = new TripalBundle($vals, 'BioData_bundles');
    $tripal_bundle->save();
  }
  // Allow modules to now add fields to the bundle
  module_invoke_all('add_bundle_fields', 'BioData', $bundle_name, $cvterm);

  return TRUE;
}
