<?php


/**
 * Deletes a tripal_entity.
 */
function tripal_entity_delete(TripalEntity $tripal_entity) {
  $tripal_entity->delete();
}

/**
 * Saves a tripal_entity to the database.
 *
 * @param $tripal_entity
 *   The tripal_entity object.
 */
function tripal_entity_save(TripalEntity $entity) {
  return $entity->save();
}


/**
 * Saves a tripal_entity type to the db.
 */
function tripal_bundle_save(TripalBundle $entity) {
  $entity->save();
}


/**
 * Deletes a tripal_entity type from the db.
 */
function tripal_bundle_delete(TripalBundle $type) {
  $type->delete();
}


/**
 * URI callback for tripal_entitys
 */
function tripal_entity_uri(TripalEntity $entity){
  return array(
    'path' => 'BioData/' . $entity->id,
  );
}

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
