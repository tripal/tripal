<?php 

/**
 * Migrate Tripal content types
 * 
 * Migrate specified Tripal content type and publish all its content. The content type 
 * will be created if it does not already exist.
 * 
 * @param $type
 *   A type array specifying the vocabular, accession, term_name, and chado data_table
 *   e.g.
 *     $type = array(
 *       'vocabulary' => 'OBI',
 *       'accession' => '0100026',
 *       'term_name' => 'organism',
 *       'storage_args' => array (
 *         'data_table' => $table
 *       )
 *     )
 * 
 */
function tripal_chado_migrate_tripal_content_type($type = array()) {
    
  // Check if the term already exists
  $term = tripal_load_term_entity($type);
  // If term doesn't exist, create a new bundle for this term
  if (!$term) {
    print("Creating bundle for term '" . $type['term_name'] . "'...\n");
    $success = tripal_create_bundle($type);
    $term = tripal_load_term_entity($type);
  }
  // Create bundle name
  $bundle_name = 'bio_data_' . $term->id;
    
  // Publish records for the bundle
  $value = array(
    'sync_node' => 1,
    'bundle_name' => $bundle_name
  );
  tripal_chado_publish_records($value);
}