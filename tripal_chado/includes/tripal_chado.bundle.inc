<?php
/**
 * Implements hook_chado_bundle_create().
 *
 * This is a Tripal hook. It allows any module to perform tasks after
 * a bundle has been created.
 *
 * @param $bundle
 *  The TripalBundle object.
 */

function tripal_chado_bundle_create($bundle, $storage_args) {
  $entity_type = $bundle->type;

  if (!array_key_exists('data_table', $storage_args)) {
    throw new Exception('Cannot create content type. Missing the "data_table" for this bundle.');
  }

  $type_id = '';
  if (array_key_exists('type_id', $storage_args)) {
    $type_id = $storage_args['type_id'];
  }
  else {
    $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
    $vocab = tripal_load_vocab_entity(array('vocab_id' => $term->vocab_id));
    $cvterm = chado_generate_var('cvterm', array(
      'dbxref_id' => array(
        'db_id' => array(
          'name' => $vocab->vocabulary,
        ),
        'accession' => $term->accession
      ),
    ));
    $type_id = $cvterm->cvterm_id;
  }

  // Before adding fields to this bundle, let's make sure we associate
  // what table in Chado this bundle is mapped to
  $chado_bundle = db_select('chado_bundle', 'cb')
    ->fields('cb')
    ->condition('bundle_id', $bundle->id)
    ->execute()
    ->fetchObject();
  if (!$chado_bundle) {
    $record = array(
      'bundle_id' => $bundle->id,
      'data_table' => $storage_args['data_table'],
      'type_id' => $type_id,
    );
    if (array_key_exists('type_linker_table', $storage_args)) {
      $record['type_linker_table'] = $storage_args['type_linker_table'];
    }
    if (array_key_exists('type_column', $storage_args)) {
      $record['type_column'] = $storage_args['type_column'];
    }
    if (array_key_exists('type_value', $storage_args)) {
      $record['type_value'] = $storage_args['type_value'];
    }
    $success = drupal_write_record('chado_bundle', $record);
    if (!$success) {
      throw new Exception('Cannot create content type. Problem associating type with Chado.');
    }
  }

  tripal_chado_create_bundle_table($bundle);
}


/**
 * Creates the table that tripal_chado uses to link Chado records with entities.
 *
 * @param $bundle
 *   A bundle object (as retrieved from tripal_load_bundle_entity().
 */
function tripal_chado_create_bundle_table($bundle) {

  // Now create the table where the bundle's records will go
  $schema = array(
    'description' => 'The linker table that associates TripalEntities with Chado records for entities of type ' . $bundle->name . '.',
    'fields' => array(
      'mapping_id' => array(
        'type' => 'serial',
        'not null' => TRUE
      ),
      'entity_id' => array(
        'description' => 'The unique entity id.',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'record_id' => array(
        'description' => 'The unique numerical identifier for the record that this entity is associated with (e.g. feature_id, stock_id, library_id, etc.).',
        'type' => 'int',
        'not null' => TRUE,
      ),
      'nid' => array(
        'description' => 'Optional. For linking nid to the entity when migrating Tripal v2 content',
        'type' => 'int',
      )
    ),
    'primary key' => array(
      'mapping_id',
    ),
    'indexes' => array(
      'record_id' => array('record_id'),
      'entity_id' => array('entity_id'),
      'nid' => array('nid'),
    ),
    'unique keys' => array(
      'table_record' => array('record_id'),
      'entity_id' => array('entity_id'),
    ),
  );
  $chado_entity_table = chado_get_bundle_entity_table($bundle);
  db_create_table($chado_entity_table, $schema);
}

/**
 * Implements hook_bundle_delete().
 *
 */
function tripal_chado_bundle_delete($bundle) {

  // Remove the entries in the appropriate chado entity table
  // and tripal_entity
  $chado_entity_table = chado_get_bundle_entity_table($bundle);
  $sql = "DROP TABLE {$chado_entity_table}";
  db_query($sql);

  // Remove the entry from the chado_bundle table.
  db_delete('chado_bundle')
    ->condition('bundle_id', $bundle->id)
    ->execute();
}

