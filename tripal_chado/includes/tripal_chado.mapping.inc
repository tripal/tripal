<?php

/**
 * This function populates the Tripal entity tables using existing
 * data in the database.
 */
function tripal_chado_map_cvterms() {
  // Get the cvterm table and look for all of the tables that link to it.
  $schema = chado_get_schema('cvterm');
  $referring = $schema['referring_tables'];

  // Perform this action in a transaction
  $transaction = db_transaction();
  print "\nNOTE: Populating of tripal_cvterm_mapping table is performed using a database transaction. \n" .
      "If the load fails or is terminated prematurely then the entire set of \n" .
      "insertions/updates is rolled back and will not be found in the database\n\n";
  try {

    // Iterate through the referring tables to see what records are there.
    foreach ($referring as $tablename) {

      // We only want to look at base tables.
      if ($tablename == 'cvterm_dbxref' || $tablename == 'cvterm_relationship' ||
          $tablename == 'cvtermpath' || $tablename == 'cvtermprop' || $tablename == 'chadoprop' ||
          $tablename == 'cvtermsynonym' || preg_match('/_relationship$/', $tablename) ||
          preg_match('/_cvterm$/', $tablename)) {
            continue;
      }

      print "Examining $tablename...\n";
      $ref_schema = chado_get_schema($tablename);
      $fkeys = $ref_schema['foreign keys'];
      foreach ($fkeys['cvterm']['columns'] as $local_id => $remote_id) {

        // Get the list of cvterm_ids from existing records in the table.
        $sql = "
          SELECT $local_id
          FROM { " . $tablename . "}
          GROUP BY $local_id
        ";
        $results = chado_query($sql);
        while ($cvterm_id = $results->fetchField()) {

          // Get the CV term details and add it to the tripal_vocabulary table if
          // it doesn't already exist.
          $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $cvterm_id));

          // TODO insert records into the tripal_cvterm_mapping table.
          db_insert('tripal_cvterm_mapping')
            ->fields(
              array(
                'cvterm_id' => $cvterm->cvterm_id,
                'chado_table' => $tablename,
                'chado_field' => $remote_id
              )
            )
            ->execute();
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