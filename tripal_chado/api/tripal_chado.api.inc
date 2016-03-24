<?php

/**
 * Publishes content in Chado as a new TripalEntity entity.
 *
 * @param $values
 *   A key/value associative array that supports the following keys:
 *   - bundle_name:  The name of the the TripalBundle (e.g. bio_data-12345).
 * @param $job_id
 *   (Optional) The numeric job ID as provided by the Tripal jobs system. There
 *   is no need to specify this argument if this function is being called
 *   outside of the jobs systems.
 *
 * @return boolean
 *   TRUE if all of the records of the given bundle type were published, and
 *   FALSE if a failure occured.
 */
function tripal_chado_publish_records($values, $job_id = NULL) {
  $bundle_name = $values['bundle_name'];
  $sync_node = array_key_exists('sync_node', $values) ? $values['sync_node'] : '';

  if (!array_key_exists('bundle_name', $values) or !$values['bundle_name']) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Could not publish record: @error", array('@error' => 'The bundle name was not provided'));
    return FALSE;
  }

  $bundle = tripal_load_bundle_entity(array('name' => $bundle_name));
  $bundle_id = $bundle->id;
  $table = tripal_get_bundle_variable('chado_table', $bundle_id);
  $column = tripal_get_bundle_variable('chado_column', $bundle_id);
  $cvterm_id = tripal_get_bundle_variable('chado_cvterm_id', $bundle_id);

  // Get the table information
  $table_schema = chado_get_schema($table);
  $pkey_field = $table_schema['primary key'][0];

  $select = "SELECT $pkey_field as record_id ";
  $from = "FROM {" . $table . "} T
      LEFT JOIN public.chado_entity CE on CE.record_id = T.$pkey_field
    AND CE.data_table = '$table'
  ";

  if ($sync_node && db_table_exists('chado_' . $table)) {
    $select = "SELECT T.$pkey_field as record_id, CT.nid ";
    $from .= "INNER JOIN public.chado_$table CT ON CT.$pkey_field = T.$pkey_field";
  }
  $where = " WHERE CE.record_id IS NULL ";
  if ($table != 'analysis' and $table != 'organism') {
    $where .= "AND $column = $cvterm_id";
  }

  $sql = $select . $from . $where;
  $records = chado_query($sql);
  $num_published = 0;
  try {
    while($record = $records->fetchObject()) {

      // First save the tripal_entity record.
      $record_id = $record->record_id;
      $ec = entity_get_controller('TripalEntity');
      $entity = $ec->create(array(
        'bundle' => $bundle_name,
        'term_id' => $bundle->term_id,
      ));
      $entity->save();

      // Next save the chado_entity record.
      $entity_record = array(
        'entity_id' => $entity->id,
        'record_id' => $record_id,
        'data_table' => $table,
        'type_table' => $table,
        'field' => $column,
      );

      // For the Tv2 to Tv3 migration we want to add the nid to the
      // entity so we can associate the node with the entity.
      if (property_exists($record, 'nid')) {
        $entity_record['nid'] = $record->nid;
      }
      $success = drupal_write_record('chado_entity', $entity_record);

      $entity = entity_load('TripalEntity', array($entity->id));
      $entity = reset($entity);
      $title_format = tripal_get_title_format($bundle);
      $title = tripal_replace_tokens($title_format, $entity, $bundle);
      $ec->setTitle($entity, $title);
      $num_published++;
    }
  }
  catch (Exception $e) {
    $error = $e->getMessage();
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Could not publish record: @error", array('@error' => $error));
    drupal_set_message('Failed publishing record. See recent logs for more details.', 'error');
    return FALSE;
  }
  drupal_set_message("Succesfully published $num_published " . $bundle->label . " record(s).");
  return TRUE;
}