<?php
/**
 * Gets an array of all chado_data types, keyed by the type name.
 *
 * @param $type_name
 *   If set, the type with the given name is returned.
 * @return ChadoDataType[]
 *   Depending whether $type isset, an array of chado_data types or a single one.
 */
function chado_data_get_types($type_name = NULL) {
  // entity_load will get the Entity controller for our chado_data entity and call the load
  // function of that object - we are loading entities by name here.
  $types = entity_load_multiple_by_name('chado_data_type', isset($type_name) ? array($type_name) : FALSE);
  return isset($type_name) ? reset($types) : $types;
}


/**
 * Menu argument loader; Load a chado_data type by string.
 *
 * @param $type
 *   The machine-readable name of a chado_data type to load.
 * @return
 *   A chado_data type array or FALSE if $type does not exist.
 */
function chado_data_type_load($type) {
  return chado_data_get_types($type);
}


/**
 * Fetch a chado_data object. Make sure that the wildcard you choose
 * in the chado_data entity definition fits the function name here.
 *
 * @param $entity_id
 *   Integer specifying the chado_data id.
 * @param $reset
 *   A boolean indicating that the internal cache should be reset.
 * @return
 *   A fully-loaded $chado_data object or FALSE if it cannot be loaded.
 *
 * @see chado_data_load_multiple()
 */
function chado_data_load($entity_id, $reset = FALSE) {
  $chado_datas = chado_data_load_multiple(array($entity_id), array(), $reset);
  return reset($chado_datas);
}


/**
 * Load multiple chado_datas based on certain conditions.
 *
 * @param $entity_ids
 *   An array of chado_data IDs.
 * @param $conditions
 *   An array of conditions to match against the {chado_data} table.
 * @param $reset
 *   A boolean indicating that the internal cache should be reset.
 * @return
 *   An array of chado_data objects, indexed by entity_id.
 *
 * @see entity_load()
 * @see chado_data_load()
 */
function chado_data_load_multiple($entity_ids = array(), $conditions = array(), $reset = FALSE) {
  return entity_load('chado_data', $entity_ids, $conditions, $reset);
}


/**
 * Deletes a chado_data.
 */
function chado_data_delete(ChadoData $chado_data) {
  $chado_data->delete();
}


/**
 * Delete multiple chado_datas.
 *
 * @param $entity_ids
 *   An array of chado_data IDs.
 */
function chado_data_delete_multiple(array $entity_ids) {
  entity_get_controller('chado_data')->delete($entity_ids);
}


/**
 * Create a chado_data object.
 */
function chado_data_create($values = array()) {
  return entity_get_controller('chado_data')->create($values);
}


/**
 * Saves a chado_data to the database.
 *
 * @param $chado_data
 *   The chado_data object.
 */
function chado_data_save(ChadoData $chado_data) {
  return $chado_data->save();
}


/**
 * Saves a chado_data type to the db.
 */
function chado_data_type_save(ChadoDataType $type) {
  $type->save();
}


/**
 * Deletes a chado_data type from the db.
 */
function chado_data_type_delete(ChadoDataType $type) {
  $type->delete();
}


/**
 * URI callback for chado_datas
 */
function chado_data_uri(ChadoData $entity){
  return array(
    'path' => 'data/' . $entity->entity_id,
  );
}


