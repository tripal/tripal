<?php
/**
 * @file
 * Provides an application programming interface (API) to manage entities
 * that use Chado as their base data.
 */

/**
 * @defgroup tripal_chado_entity_api Chado Entity
 * @ingroup tripal_chado_api
 * @{
 * Provides an application programming interface (API) to manage entities
 * that use Chado as their base data.
 * @}
 */

/**
 * Retreive the entity_id assigned to a given record_id and bundle.
 *
 * @param $bundle
 *   A bundle object (as retrieved from tripal_load_bundle_entity().
 * @param $record_id
 *   The ID of the record in the Chado table. The record must belong to
 *   the table to which the bundle is associated in chado.
 *
 * @return
 *   The ID of the entity that belongs to the given record_id.
 *
 * @ingroup tripal_chado_entity_api
 */
function chado_get_record_entity_by_bundle(TripalBundle $bundle, $record_id) {
  if (!$bundle) {
    throw new Exception('Please provide a TripalBundle object.');
  };
  if (!$record_id) {
    throw new Exception('Please provide an integer record ID.');
  };
  if (!is_numeric($record_id)) {
    throw new Exception('Please provide an integer record ID. The value provided was "' . $record_id . '"');
  }

  $chado_entity_table = chado_get_bundle_entity_table($bundle);
  return db_select($chado_entity_table, 'CE')
    ->fields('CE', ['entity_id'])
    ->condition('CE.record_id', $record_id)
    ->execute()
    ->fetchField();
}


/**
 * Retreive the entity_id assigned to a given record_id and base table.
 *
 * @param $data_table
 *   The name of the Chado base table.
 * @param $record_id
 *   The ID of the record in the Chado table. The record must belong to
 *   the table to which the bundle is associated in chado.
 *
 * @return
 *   The ID of the entity that belongs to the given record_id, or NULL
 *   otherwise.
 *
 * @ingroup tripal_chado_entity_api
 */

function chado_get_record_entity_by_table($data_table, $record_id) {

  // The data table and type_id are required.
  if (!$data_table) {
    throw new Exception('Please provide the $data_table argument.');
  };
  if (!$record_id) {
    throw new Exception('Please provide an integer record ID.');
  };
  if (!is_numeric($record_id)) {
    throw new Exception('Please provide an integer record ID. The value provided was "' . $record_id . '"');
  }

  // Get the list of bundles for this table.
  $bundles = db_select('chado_bundle', 'CB')
    ->fields('CB', ['bundle_id'])
    ->condition('CB.data_table', $data_table)
    ->execute();

  // Look for the record ID in the appropriate chado table.
  while ($bundle_id = $bundles->fetchField()) {
    $entity_id = db_select('chado_bio_data_' . $bundle_id, 'CBD')
      ->fields('CBD', ['entity_id'])
      ->condition('record_id', $record_id)
      ->execute()
      ->fetchField();
    if ($entity_id) {
      return $entity_id;
    }
  }
  return NULL;
}


/**
 * A helper function that provides the Chado mapping table for the bundle.
 *
 * The tripal_chado module must map entities to their corresponding record
 * in Chado.  Each bundl type has their own table for this mapping.  This
 * function provides the name of the table given the bundle name.  A mapping
 * table will only map to one table in Chado so the record_id's of the mapping
 * table should always be unique.
 *
 * @param $bundle
 *   A bundle object (as retrieved from tripal_load_bundle_entity().
 *
 * @return
 *   The name of the mapping table that Chado uses to map entities to records.
 *
 * @ingroup tripal_chado_entity_api
 */
function chado_get_bundle_entity_table($bundle) {
  if (!$bundle) {
    return '';
  }
  return 'chado_' . $bundle->name;
}