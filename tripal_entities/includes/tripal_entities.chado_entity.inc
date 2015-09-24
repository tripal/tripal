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

}