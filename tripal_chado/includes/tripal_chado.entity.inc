<?php

/**
 *
 * Implements hook_entity_load().
 */
function tripal_chado_entity_presave($entity, $type) {
  // Add the Chado table and column to an entity to which
  // data of this cvterm will map.
  if ($type == 'TripalEntity') {
    $bundle_id = $entity->bundle;
    $bundle = db_select('tripal_bundle', 'tb')
      ->fields('tb')
      ->condition('bundle', $bundle_id)
      ->execute()
      ->fetchObject();

    $data = unserialize($bundle->data);
    $entity->chado_table = $data['data_table'];
    $entity->chado_field = $data['field'];

    // If we have an ID then this entity has been saved and it will
    // also therefore have a chado_entity record.  We want to
    // load this record so it is always part of the entity object.
    if (property_exists($entity, 'id') and $entity->id) {
      $details = db_select('chado_entity', 'ce')
        ->fields('ce')
        ->condition('entity_id', $entity->id)
        ->execute()
        ->fetchObject();

      // Add the chado entity details to the entity in case it's needed
      // downstream (e.g. in field widget construction).
      $entity->chado_entity_id = $details->chado_entity_id;

      // Add in the record.
      $schema = chado_get_schema($entity->chado_table);
      $pkey = $schema['primary key'][0];
      $entity->chado_record_id = $details->record_id;
      $entity->chado_record = chado_generate_var($entity->chado_table, array($pkey =>+ $details->record_id));
    }
  }
}
/**
 *
 * @param $entity
 * @param $type
 */
function tripal_chado_entity_postsave($entity, $type) {
  // Set the title for this entity using the chado data.
  $title = chado_get_entity_title($entity);
  $ec = new TripalEntityController($entity->type);
  $ec->setTitle($entity, $title);
}
/**
 *
 * Implements hook_entity_load().
 */
function tripal_chado_entity_load($entities, $type) {

}

/**
 *
 * Implements hook_entity_update().
 */
function tripal_chado_entity_update($entity, $type) {

}

/**
 *
 * Implements hook_entity_delete().
 */
function tripal_chado_entity_delete($entity, $type) {
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
 * This theme function is meant to override the data_combo theme.
 *
 * @param $variables
 */
function theme_tripal_chado_date_combo($variables) {
  $element = $variables['element'];
  $field = field_info_field($element['#field_name']);
  $instance = field_info_instance($element['#entity_type'], $element['#field_name'], $element['#bundle']);

  // Group start/end items together in fieldset.
  $fieldset = array(
    '#title' => t($element['#title']) . ' ' . ($element['#delta'] > 0 ? intval($element['#delta'] + 1) : ''),
    '#value' => '',
    '#description' => !empty($element['#fieldset_description']) ? $element['#fieldset_description'] : '',
    '#attributes' => array(),
    '#children' => $element['#children'],
    '#attributes' => array('class' => array('collapsible', 'collapsed')),
  );
  return theme('fieldset', array('element' => $fieldset));
}


/**
 * Determines whether the given user has access to a tripal data entity.
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

/**
 * Menu callback to display an entity.
 *
 * As we load the entity for display, we're responsible for invoking a number
 * of hooks in their proper order.
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 * @see hook_entity_view_alter()
 */
function tripal_chado_view_entity($entity, $view_mode = 'full') {
  $content = '';
  $controller = entity_get_controller($entity->type);
  $content = $controller->view(array($entity->id => $entity));
  drupal_set_title($entity->title);
  return $content;
}

/**
 * Menu title callback for showing individual entities
 */
function tripal_chado_entity_title($entity){
  if ($entity) {
    return $entity->title;
  }
}
