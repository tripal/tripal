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

  // Create the bundle name and entity type name.  The bundle name is the
  // cvterm ID.  This isn't very human readable, but the alternative is to
  // use the accession which may not always be alpha-numeric.
  $bundle_id = 'bio-data_' . $cvterm->cvterm_id;

  // The organism table does not have a type_id so we won't ever find
  // a record for it in the tripal_cv_defaults table.
  $bundle_data = array();
  if ($cvterm->name == 'organism') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'organism',
      'type_table' => 'organism',
      'field' =>  '',
    );
  }
  // The analysis table does not have a type_id so we won't ever find
  // a record for it in the tripalcv_defaults table.
  else if ($cvterm->name == 'analysis') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'analysis',
      'type_table' => 'analysis',
      'field' =>  '',
    );
  }
  else if ($cvterm->name == 'project') {
    $bundle_data = array(
      'cv_id' => $cvterm->cv_id->cv_id,
      'cvterm_id' => $cvterm->cvterm_id,
      'data_table' => 'project',
      'type_table' => 'project',
      'field' =>  '',
    );
  }
  else {
    // TODO: WHAT TO DO IF A VOCABULARY IS USED AS A DEFAULT FOR MULTIPLE
    // TABLES.
    // Look to see if this vocabulary is used as a default for any table.
    $default = db_select('tripal_cv_defaults', 't')
      ->fields('t')
      ->condition('cv_id', $cvterm->cv_id->cv_id)
      ->execute()
      ->fetchObject();
    if ($default) {
      $bundle_data = array(
        'cv_id' => $cvterm->cv_id->cv_id,
        'cvterm_id' => $cvterm->cvterm_id,
        'data_table' => $default->table_name,
        'type_table' => $default->table_name,
        'field' =>  $default->field_name,
      );
    }
    // If there is no default table then we have an error, and we should
    // set a variable so that the form can help the user deal with the problem.
    else {
      $error = t('There is no default mapping of this term\'s ' .
          'vocabulary to a table in Chado.  Therefore, it is not possible to ' .
          'determine how to store data of this type.');
      return FALSE;
    }
  }

  // Check to see if this bundle exists. If not then create it
  $bundle = db_select('tripal_bundle', 't')
    ->fields('t')
    ->condition('type', 'TripalEntity')
    ->condition('bundle', $bundle_id)
    ->execute()
    ->fetchObject();

  if (!$bundle) {
    db_insert('tripal_bundle')
      ->fields(array(
        'label' => $cvterm->name,
        'type' => 'TripalEntity',
        'bundle' => $bundle_id,
        'data' => serialize($bundle_data),
        'module' => 'tripal_entities'
      ))
      ->execute();
  }

  // Clear the entity cache so that Drupal will read our
  // hook_entity_info() implementation.
  global $language;
  $langcode = $language->language;
  cache_clear_all("entity_info:$langcode", 'cache');
  variable_set('menu_rebuild_needed', TRUE);

  // Allow modules to now add fields to the bundle
  module_invoke_all('add_bundle_fields', 'TripalEntity', $bundle_id, $cvterm);

  return TRUE;
}

/**
 * Get Page Title Format for a given Tripal Entity Type.
 *
 * @param TripalBundle $entity
 *   The Entity object for the Tripal Bundle the title format is for.
 */
function tripal_get_title_format($entity) {

  // Title formats are saved as Tripal Bundle Variables.
  // Therefore, first we need the variable_id for title_formats.
  $variable_id = db_select('tripal_variables', 'v')
    ->fields('v', array('variable_id'))
    ->condition('name', 'title_format')
    ->execute()
    ->fetchField();
  
  // Then we can check if there is already a title format for this bundle/type.
  $title_format = db_select('tripal_bundle_variables', 'var')
    ->fields('var', array('value'))
    ->condition('var.bundle_id', $entity->id)
    ->condition('var.variable_id', $variable_id)
    ->execute()
    ->fetchField();
    
  // If there isn't yet a title format for this bundle/type then we should
  // determine the default based on the table unique constraint and save it.
  // @TODO: Replace the label with the base table name.
  // @TODO: make this chado independant.
  if (!$title_format) {
    $title_format = chado_node_get_unique_constraint_format($entity->label);
    tripal_save_title_format($entity, $title_format);
  }
  
  return $title_format;
}

/**
 * Save Page Title Format for a given Tripal Entity Type.
 *
 * @param TripalBundle $entity
 *   The Entity object for the Tripal Bundle the title format is for.
 * @param string $format
 *   The pattern to be used when generating entity titles for the above type.
 */
function tripal_save_title_format($entity, $format) {

  // Title formats are saved as Tripal Bundle Variables.
  // Thus first we need to grab the variable_id for title_format.
  $variable_id = db_select('tripal_variables', 'v')->fields('v', array('variable_id'))->condition('name', 'title_format')->execute()->fetchField();

  // And then we need to write the new format to the tripal_bundle_variables table.
  $record = array(
    'bundle_id' => $entity->id,
    'variable_id' => $variable_id,
    'value' => $format,
  );
  
  // Check whether there is already a format saved.
  $bundle_variable_id = db_select('tripal_bundle_variables', 'var')
    ->fields('var', array('bundle_variable_id'))
    ->condition('var.bundle_id', $record['bundle_id'])
    ->condition('var.variable_id', $record['variable_id'])
    ->execute()
    ->fetchField();
  if ($bundle_variable_id) {
    $record['bundle_variable_id'] = $bundle_variable_id;
    drupal_write_record('tripal_bundle_variables', $record, 'bundle_variable_id');
  }
  else {
    drupal_write_record('tripal_bundle_variables', $record);
  }

}