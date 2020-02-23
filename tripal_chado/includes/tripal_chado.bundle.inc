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
    $term = tripal_load_term_entity(['term_id' => $bundle->term_id]);
    $vocab = tripal_load_vocab_entity(['vocab_id' => $term->vocab_id]);
    $cvterm = chado_generate_var('cvterm', [
      'dbxref_id' => [
        'db_id' => [
          'name' => $vocab->vocabulary,
        ],
        'accession' => $term->accession,
      ],
    ]);
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
    $record = [
      'bundle_id' => $bundle->id,
      'data_table' => $storage_args['data_table'],
      'type_id' => $type_id,
    ];
    if (array_key_exists('type_linker_table', $storage_args)) {
      $record['type_linker_table'] = $storage_args['type_linker_table'];
    }
    if (array_key_exists('type_column', $storage_args)) {
      $record['type_column'] = $storage_args['type_column'];
    }
    if (array_key_exists('type_value', $storage_args)) {
      $record['type_value'] = $storage_args['type_value'];
    }
    if (array_key_exists('base_type_id', $storage_args)) {
      $record['base_type_id'] = $storage_args['base_type_id'];
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
  $schema = [
    'description' => 'The linker table that associates TripalEntities with Chado records for entities of type ' . $bundle->name . '.',
    'fields' => [
      'mapping_id' => [
        'type' => 'serial',
        'not null' => TRUE,
      ],
      'entity_id' => [
        'description' => 'The unique entity id.',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'record_id' => [
        'description' => 'The unique numerical identifier for the record that this entity is associated with (e.g. feature_id, stock_id, library_id, etc.).',
        'type' => 'int',
        'not null' => TRUE,
      ],
      'nid' => [
        'description' => 'Optional. For linking nid to the entity when migrating Tripal v2 content',
        'type' => 'int',
      ],
    ],
    'primary key' => [
      'mapping_id',
    ],
    'indexes' => [
      'record_id' => ['record_id'],
      'entity_id' => ['entity_id'],
      'nid' => ['nid'],
    ],
    'unique keys' => [
      'table_record' => ['record_id'],
      'entity_id' => ['entity_id'],
    ],
  ];
  $chado_entity_table = chado_get_bundle_entity_table($bundle);
  if (!$chado_entity_table) {
    throw new Exception('Cannot create entity linker table for chado.');
  }
  db_create_table($chado_entity_table, $schema);

  // Ensure we clear the drupal schema cache after creating this table.
  // This is needed so we can publish records directly after creating a new
  // content type.
  drupal_get_complete_schema(TRUE);
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

/**
 * Implements hook_bundle_find_orphans().
 *
 */
function tripal_chado_bundle_find_orphans($bundle, $count = FALSE, $offset = 0, $limit = 10) {
  $chado_bundle_table = chado_get_bundle_entity_table($bundle);
  $schema = chado_get_schema($bundle->data_table);
  $primary_key = $schema['primary key'][0];

  $select = "CT.entity_id";
  if ($count) {
    $select = "count(*) as count";
  }
  $qlimit = '';
  if (!$count and $limit) {
    $qlimit = "LIMIT $limit OFFSET $offset ";
  }
  // Get the count
  $query = "
    SELECT $select
    FROM [$chado_bundle_table] CT
      LEFT JOIN {" . $bundle->data_table . "} BT ON CT.record_id = BT.$primary_key
    WHERE BT.$primary_key IS NULL
    $qlimit
  ";

  $results = chado_query($query);
  if ($count) {
    $num_orphans = (int) $results->fetchField();
    return $num_orphans;
  }
  else {
    $ids = [];
    while ($entity_id = $results->fetchField()) {
      $ids[] = $entity_id;
    }
    return $ids;
  }
}

/**
 * Implements hook_bundle_delete_orphans().
 */
function tripal_chado_bundle_delete_orphans(TripalBundle $bundle, array $ids, TripalJob $job = NULL) {

  $chado_bundle_table = chado_get_bundle_entity_table($bundle);
  $schema = chado_get_schema($bundle->data_table);
  $primary_key = $schema['primary key'][0];

  $num_deleted = db_delete($chado_bundle_table)
    ->condition('entity_id', $ids, 'IN')
    ->execute();

  if ($job) {
    $job->addItemsHandled($num_deleted);
    $job->logMessage("Removed !num orphaned entities", ['!num' => $num_deleted]);
  }

  return $num_deleted;
}
