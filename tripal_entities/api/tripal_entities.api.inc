<?php

/**
 * Menu argument loader; Load a tripal data type by string.
 *
 * @param $type
 *   The machine-readable name of a tripal data type to load.
 * @return
 *   A tripal data type array or FALSE if $type does not exist.
 */

function tripal_data_type_load($type) {
  $entity = tripal_data_get_types($type);
  return $entity;
}

/**
 * Gets an array of all tripal_data types, keyed by the type name.
 *
 * @param $type_name
 *   If set, the type with the given name is returned.
 * @return TripalDataType[]
 *   Depending whether $type isset, an array of tripal_data types or a single one.
 */
function tripal_data_get_types($type_name = NULL) {
  // entity_load will get the Entity controller for our tripal_data entity and call the load
  // function of that object - we are loading entities by name here.
  $types = entity_load_multiple_by_name('tripal_data_type', isset($type_name) ? array($type_name) : FALSE);
  return isset($type_name) ? reset($types) : $types;
}

/**
 * Fetch a tripal_data object. Make sure that the wildcard you choose
 * in the tripal_data entity definition fits the function name here.
 *
 * @param $entity_id
 *   Integer specifying the tripal_data id.
 * @param $reset
 *   A boolean indicating that the internal cache should be reset.
 * @return
 *   A fully-loaded $tripal_data object or FALSE if it cannot be loaded.
 *
 * @see tripal_data_load_multiple()
 */
function tripal_data_load($entity_id, $reset = FALSE) {
  $tripal_datas = tripal_data_load_multiple(array($entity_id), array(), $reset);
  return reset($tripal_datas);
}


/**
 * Load multiple tripal_datas based on certain conditions.
 *
 * @param $entity_ids
 *   An array of tripal_data IDs.
 * @param $conditions
 *   An array of conditions to match against the entity table.
 * @param $reset
 *   A boolean indicating that the internal cache should be reset.
 * @return
 *   An array of tripal_data objects, indexed by entity_id.
 *
 * @see entity_load()
 * @see tripal_data_load()
 */
function tripal_data_load_multiple($entity_ids = array(), $conditions = array(), $reset = FALSE) {
  return entity_load('tripal_data', $entity_ids, $conditions, $reset);
}


/**
 * Deletes a tripal_data.
 */
function tripal_data_delete(TripalData $tripal_data) {
  $tripal_data->delete();
}


/**
 * Delete multiple tripal_datas.
 *
 * @param $entity_ids
 *   An array of tripal_data IDs.
 */
function tripal_data_delete_multiple(array $entity_ids) {
  entity_get_controller('tripal_data')->delete($entity_ids);
}


/**
 * Create a tripal_data object.
 */
function tripal_data_create($values = array()) {
  return entity_get_controller('tripal_data')->create($values);
}
function tripal_data_type_create($values = array()) {
  return entity_get_controller('tripal_data_type')->create($values);
}

/**
 * Saves a tripal_data to the database.
 *
 * @param $tripal_data
 *   The tripal_data object.
 */
function tripal_data_save(TripalData $tripal_data) {
  return $tripal_data->save();
}


/**
 * Saves a tripal_data type to the db.
 */
function tripal_data_type_save(TripalDataType $type) {
  $type->save();
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
    'path' => 'data/' . $entity->entity_id,
  );
}


