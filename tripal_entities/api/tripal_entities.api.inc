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
    'path' => 'data/' . $entity->id,
  );
}


