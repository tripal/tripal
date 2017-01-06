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
    $success = drupal_write_record('chado_bundle', $record);
    if (!$success) {
      throw new Exception('Cannot create content type. Problem associating type with Chado.');
    }
    $chado_bundle = db_select('chado_bundle', 'cb')
      ->fields('cb')
      ->condition('bundle_id', $bundle->id)
      ->execute()
      ->fetchObject();
  }

  // Create/Add the new fields for this bundle.
  tripal_chado_bundle_create_fields($entity_type, $bundle, $chado_bundle);

  // Create/Add the new field instances for this bundle.
  tripal_chado_bundle_create_instances($entity_type, $bundle, $chado_bundle);
}