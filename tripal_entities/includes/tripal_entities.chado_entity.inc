<?php

/**
 *
 * Implements hook_entity_load().
 */
function tripal_entities_entity_presave($entity, $type) {

}
/**
 *
 * @param $entity
 * @param $type
 */
function tripal_entities_entity_postsave($entity, $type) {
  // Set the title for this entity using the chado data.
  $title = chado_get_entity_title($entity);
  $ec = new TripalEntityController($entity->type);
  $ec->setTitle($entity, $title);
}
/**
 *
 * Implements hook_entity_load().
 */
function tripal_entities_entity_load($entities, $type) {

}

/**
 *
 * Implements hook_entity_insert().
 */
function tripal_entities_entity_insert($entity, $type) {

}

/**
 *
 * Implements hook_entity_update().
 */
function tripal_entities_entity_update($entity, $type) {

}

/**
 *
 * Implements hook_entity_delete().
 */
function tripal_entities_entity_delete($entity, $type) {
  $record = db_select('chado_entity', 'ce')
   ->fields('ce', array('chado_entity_id', 'data_table', 'record_id'))
   ->condition('entity_id', $entity->id)
   ->execute()
   ->fetchObject();
  
  // Delete the corresponding record in Chado
  $table = $record->data_table;
  $record_id = $record->record_id;  
  chado_delete_record($table, array($table . '_id' => $record_id));
  
  //Delete the record in the public.chado_entity table
  $sql = "DELETE FROM {chado_entity} WHERE chado_entity_id = :id";
  db_query($sql, array(':id' => $record->chado_entity_id));
  
}