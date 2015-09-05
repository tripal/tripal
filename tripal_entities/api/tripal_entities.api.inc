<?php

/**
 * Menu argument loader; Load a tripal data type by string.
 *
 * @param $type
 *   The machine-readable name of a tripal data type to load.
 * @return
 *   A tripal data type array or FALSE if $type does not exist.
 */

function tripal_data_type_load($bundle_type, $reset = FALSE) {

  // Get the type of entity by the ID.
  $bundle_types = db_select('tripal_data_type', 'tdt')
    ->fields('tdt', array('id', 'type'))
    ->condition('bundle', $bundle_type)
    ->execute()
    ->fetchObject();

  // Load the entity.
  $entity = entity_load($bundle_types->type, array($bundle_types->id), array(), $reset);
  return reset($entity);
}

/**
 * Fetch a tripal_data object. Make sure that the wildcard you choose
 * in the tripal_data entity definition fits the function name here.
 *
 * @param $id
 *   Integer specifying the tripal_data id.
 * @param $reset
 *   A boolean indicating that the internal cache should be reset.
 * @return
 *   A fully-loaded $tripal_data object or FALSE if it cannot be loaded.
 *
 * @see tripal_data_load_multiple()
 */
function tripal_data_load($id, $reset = FALSE) {

  // Get the type of entity by the ID.
  $entity_type = db_select('tripal_data', 'td')
    ->fields('td', array('type'))
    ->condition('id', $id)
    ->execute()
    ->fetchField();

  // Load the entity.
  $entity =  entity_load($entity_type, array($id), array(), $reset);
  return reset($entity);
}


/**
 * Deletes a tripal_data.
 */
function tripal_data_delete(TripalData $tripal_data) {
  $tripal_data->delete();
}

/**
 * Saves a tripal_data to the database.
 *
 * @param $tripal_data
 *   The tripal_data object.
 */
function tripal_data_save(TripalData $entity) {
  return $entity->save();
}


/**
 * Saves a tripal_data type to the db.
 */
function tripal_data_type_save(TripalDataType $entity) {
  $entity->save();
}


/**
 * Deletes a tripal_data type from the db.
 */
function tripal_data_type_delete(TripalDataType $type) {
  $type->delete();
}


/**
 * URI callback for tripal_datas
 */
function tripal_data_uri(TripalData $entity){
  return array(
    'path' => 'data/' . $entity->id,
  );
}


