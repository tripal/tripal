<?php


/**
 * Implements hook_entity_create().
 *
 * This hook is called when brand new entities are created, but
 * they are not loaded so the hook_entity_load() is not yet called.
 */
function tripal_chado_entity_create(&$entity, $type) {
  if ($type == 'TripalEntity') {

    // Set some defaults on vars needed by this module.
    $entity->chado_table = NULL;
    $entity->chado_column = NULL;
    $entity->chado_record = NULL;
    $entity->chado_record_id = NULL;

    // Add in the Chado table information for this entity type.
    $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
    $chado_table = tripal_get_bundle_variable('chado_table', $bundle->id);
    $chado_column = tripal_get_bundle_variable('chado_column', $bundle->id);
    if ($chado_table) {
      $entity->chado_table = $chado_table;
      $entity->chado_column = $chado_column;
    }
  }
}
/**
 * Implements hook_entity_presave().
 */
function tripal_chado_entity_presave($entity, $type) { }

/**
 * Implements hook_entity_postsave().
 */
function tripal_chado_entity_postsave($entity, $type) { }

/**
 * Implements hook_entity_load().
 */
function tripal_chado_entity_load($entities, $type) {
  if ($type == 'TripalEntity') {
    foreach ($entities as $entity) {

      // We want to add in the record ID to the entity.
      if (property_exists($entity, 'id')) {

        // Set some defaults on vars needed by this module.
        $entity->chado_table = NULL;
        $entity->chado_column = NULL;
        $entity->chado_record = NULL;
        $entity->chado_record_id = NULL;

        // Add in the Chado table information for this entity type.
        $bundle = tripal_load_bundle_entity(array('name' => $entity->bundle));
        $chado_table = tripal_get_bundle_variable('chado_table', $bundle->id);
        $chado_column = tripal_get_bundle_variable('chado_column', $bundle->id);
        if ($chado_table) {
          $entity->chado_table = $chado_table;
          $entity->chado_column = $chado_column;
        }

        $chado_entity = db_select('chado_entity' ,'ce')
          ->fields('ce')
          ->condition('ce.entity_id', $entity->id)
          ->execute()
          ->fetchObject();
        $schema = chado_get_schema($chado_table);
        $record = chado_generate_var($chado_table, array($schema['primary key'][0] => $chado_entity->record_id));
        $entity->chado_record = $record;
        $entity->chado_record_id = $chado_entity->record_id;
      }
    }
  }
}

/**
 * Implements hook_field_attach_form().
 */
function tripal_chado_field_attach_form($entity_type, $entity, &$form, &$form_state, $langcode) { }

/**
 *
 * Implements hook_entity_insert().
 */
function tripal_chado_entity_insert($entity, $type) { }

/**
 *
 * Implements hook_entity_update().
 */
function tripal_chado_entity_update($entity, $type) { }

/**
 *
 * Implements hook_entity_delete().
 */
function tripal_chaddo_entity_delete($entity, $type) {
  $record = db_select('chado_entity', 'ce')
    ->fields('ce', array('chado_entity_id', 'data_table', 'record_id'))
    ->condition('entity_id', $entity->id)
    ->execute()
    ->fetchObject();

  if ($record && property_exists($record, 'chado_entity_id')) {
    // Delete the corresponding record in Chado
    $table = $record->data_table;
    $record_id = $record->record_id;
    chado_delete_record($table, array($table . '_id' => $record_id));

    //Delete the record in the public.chado_entity table
    $sql = "DELETE FROM {chado_entity} WHERE chado_entity_id = :id";
    db_query($sql, array(':id' => $record->chado_entity_id));
  }
}

/**
 * Determines whether the given user has access to a tripal data entity.
 *
 * TODO: I'm not sure this function should be at this level. I think all
 * access controls should be handled by the tripal_entity module and that
 * storage backends should just attach data as requested.
 *
 * @param $op
 *   The operation being performed. One of 'view', 'update', 'create', 'delete'
 *   or just 'edit' (being the same as 'create' or 'update').
 * @param $entity
 *   Optionally a tripal data entity or a tripal data type to check access for.
 *   If nothing is given, access for all types is determined.
 * @param $account
 *   The user to check for. Leave it to NULL to check for the global user.
 * @return boolean
 *   Whether access is allowed or not.
 */
function tripal_chado_entity_access($op, $entity = NULL, $account = NULL) {
  if (user_access('administer tripal data', $account)) {
    return TRUE;
  }
  if (isset($entity) && $type_name = $entity->type) {
    $op = ($op == 'view') ? 'view' : 'edit';
    if (user_access("$op any $type_name data", $account)) {
      return TRUE;
    }
  }
  return FALSE;
}
