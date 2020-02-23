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
  try {

    // Iterate through the referring tables to see what records are there.
    foreach ($base_tables as $tablename) {
      $ref_schema = chado_get_schema($tablename);
      $fkeys = $ref_schema['foreign keys'];
      if (!isset($fkeys['cvterm']['columns'])) {
        continue;
      }
      foreach ($fkeys['cvterm']['columns'] as $local_id => $remote_id) {

        // Get the list of cvterm_ids from existing records in the table.
        $sql = "
          SELECT $local_id
          FROM {" . $tablename . "}
          GROUP BY $local_id
        ";
        $results = chado_query($sql);
        while ($cvterm_id = $results->fetchField()) {
          tripal_chado_add_cvterm_mapping($cvterm_id, $tablename, $local_id);
        }
      }
    }

    // Now we also want to map tripal terms for existing bundles
    $sql =
      "SELECT
         (SELECT vocabulary FROM tripal_vocab TV WHERE id = TM.vocab_id),
          accession,
          name
       FROM tripal_term TM";
    $results = db_query($sql);
    while ($tripal_term = $results->fetchObject()) {
      $voc = $tripal_term->vocabulary;
      $accession = $tripal_term->accession;
      $name = $tripal_term->name;
      $dbxref_sql =
        "SELECT dbxref_id
          FROM {dbxref}
          WHERE
            accession = :accession
          AND
            db_id = (SELECT db_id FROM {db} WHERE name = :voc)";
      $dbxref_id = chado_query($dbxref_sql, [
        ':accession' => $accession,
        ':voc' => $voc,
      ])->fetchField();
      if ($dbxref_id) {
        $cvterm_sql =
          "SELECT cvterm_id
            FROM {cvterm}
            WHERE
              dbxref_id = :dbxref_id
            AND name = :name";
        $cvterm_id = chado_query($cvterm_sql, [
          ':dbxref_id' => $dbxref_id,
          ':name' => $name,
        ])->fetchField();
        if ($cvterm_id) {
          // Check if this term is already mapped in the chado_cvterm_mapping table
          $check_sql =
            "SELECT mapping_id
             FROM chado_cvterm_mapping
             WHERE cvterm_id = :cvterm_id";
          $mapped = db_query($check_sql, [':cvterm_id' => $cvterm_id])->fetchField();
          // If mapping does not exist and a table name matches the term name, add it
          if (!$mapped && chado_table_exists($name)) {
            print "Adding mapped tripal term: $name\n";
            tripal_chado_add_cvterm_mapping($cvterm_id, $name, NULL);
          }
        }
      }
    }
  } catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    $transaction->rollback();
    watchdog_exception('tripal_chado', $e);
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
}

/**
 * Add a cvterm mapping record
 *
 * Check if the cvterm mapping record exists. If not, add it to the
 * chado_cvterm_mapping table
 *
 * @param $cvterm_id
 *   The cvterm_id to map.
 * @param $tablename
 *   The name of the table that the term maps to.
 * @param $chado_field
 *   The field that the term maps to.
 */
function tripal_chado_add_cvterm_mapping($cvterm_id, $tablename, $chado_field) {
  // check if the record exists
  $record = db_select('chado_cvterm_mapping', 'tcm')
    ->fields('tcm', ['mapping_id'])
    ->condition('cvterm_id', $cvterm_id)
    ->execute()
    ->fetchField();
  // insert records into the chado_cvterm_mapping table.
  if (!$record) {
    db_insert('chado_cvterm_mapping')
      ->fields(
        [
          'cvterm_id' => $cvterm_id,
          'chado_table' => $tablename,
          'chado_field' => $chado_field,
        ]
      )
      ->execute();
  }
  // if the record exists, update the term mapping
  else {
    db_update('chado_cvterm_mapping')
      ->fields(
        [
          'chado_table' => $tablename,
          'chado_field' => $chado_field,
        ]
      )
      ->condition('cvterm_id', $cvterm_id)
      ->execute();
  }
}
