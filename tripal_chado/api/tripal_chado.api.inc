<?php

/**
 * Retrieves an entity that matches the given table and record id.
 *
 * @param $table
 *   The name of the Chado table.
 * @param $record_id
 *   The record's primary key in the table specified by $table.
 */
function tripal_load_chado_entity($table, $record_id) {
   $entity_id = db_select('chado_entity', 'ce')
     ->fields('ce', array('entity_id'))
     ->condition('ce.record_id', $record_id)
     ->condition('ce.data_table', $table)
     ->execute()
     ->fetchField();
   if ($entity_id) {
     $entity = entity_load('TripalEntity', array($entity_id));
     return reset($entity);
   }
   return NULL;
}