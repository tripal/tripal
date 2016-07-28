<?php

/**
 * This function populates the Tripal entity tables using existing
 * data in the database.
 */
function tripal_chado_map_cvterms() {
  // Get chado base tables
  $base_tables = chado_get_base_tables();

  // Perform this action in a transaction
  $transaction = db_transaction();
  print "\nNOTE: Populating of tripal_cvterm_mapping table is performed using a database transaction. \n" .
      "If the load fails or is terminated prematurely then the entire set of \n" .
      "insertions/updates is rolled back and will not be found in the database\n\n";
  try {

    // Iterate through the referring tables to see what records are there.
    foreach ($base_tables as $tablename) {
      print "Examining $tablename...\n";
      $ref_schema = chado_get_schema($tablename);
      $fkeys = $ref_schema['foreign keys'];
      if (!isset($fkeys['cvterm']['columns'])) {
        continue;
      }
      foreach ($fkeys['cvterm']['columns'] as $local_id => $remote_id) {

        // Get the list of cvterm_ids from existing records in the table.
        $sql = "
          SELECT $local_id
          FROM { " . $tablename . "}
          GROUP BY $local_id
        ";
        $results = chado_query($sql);
        while ($cvterm_id = $results->fetchField()) {
          tripal_chado_add_cvterm_mapping($cvterm_id, $tablename, $local_id);
        }
      }
    }
  }
  catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    $transaction->rollback();
    watchdog_exception('tripal_chado', $e);
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
}

/*
 * Add a cvterm mapping record
 *
 * Check if the cvterm mapping record exists. If not, add it to the tripal_cvterm_mapping
 * table
 */
function tripal_chado_add_cvterm_mapping($cvterm_id, $tablename, $chado_field) {
  // check if the record exists
  $record = db_select('tripal_cvterm_mapping', 'tcm')
  ->fields('tcm', array('mapping_id'))
  ->condition('cvterm_id', $cvterm_id)
  ->condition('chado_table', $tablename)
  ->condition('chado_field', $chado_field)
  ->execute()
  ->fetchField();

  // insert records into the tripal_cvterm_mapping table.
  if (!$record) {
    db_insert('tripal_cvterm_mapping')
    ->fields(
        array(
          'cvterm_id' => $cvterm_id,
          'chado_table' => $tablename,
          'chado_field' => $chado_field
        )
        )
        ->execute();
  }
}