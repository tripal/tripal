<?php
/**
 * A helper function to retreive the entity_id of a given record_id.
 *
 * @param $bundle
 *   A bundle object (as retreieved from tripal_load_bundle_entity().
 * @param $record_id
 *   The ID of the record in the Chado table. The record must belong to
 *   the table to which the bundle is associated in chado.
 *
 * @return
 *   A database result object.
 */
function tripal_chado_get_record_entity($bundle, $record_id) {
  if (!$bundle or !$record_id) {
    return NULL;
  }
  $chado_entity_table = tripal_chado_get_bundle_entity_table($bundle);
  return db_select($chado_entity_table, 'CE')
    ->condition('CE.record_id', $record_id)
    ->execute()
    ->fetchObject();
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
 *   A bundle object (as retreieved from tripal_load_bundle_entity().
 *
 * @return
 *   The name of the mapping table that Chado uses to map entities to records.
 */
function tripal_chado_get_bundle_entity_table($bundle) {
  if (!$bundle) {
    return '';
  }
  return 'chado_' . $bundle->name;
}

