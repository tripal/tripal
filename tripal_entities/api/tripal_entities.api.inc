<?php


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


