<?php 

/**
 * This function populates the Tripal entity tables using existing
 * data in the database.
 */
function tripal_entities_populate_entity_tables() {
  // Get the cvterm table and look for all of the tables that link to it.
  $schema = chado_get_schema('cvterm');
  $referring = $schema['referring_tables'];

  // Perform this action in a transaction
  $transaction = db_transaction();
  print "\nNOTE: Populating of tripal entity tables is performed using a database transaction. \n" .
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

              // First add a record to the tripal_vocabulary table.
              $values = array(
                'cv_id' => $cvterm->cv_id->cv_id,
                'db_id' => $cvterm->dbxref_id->db_id->db_id,
                'publish' => 0,
              );
              $entity_type_id = 0;
              $entity_type = chado_select_record('tripal_vocabulary', array('vocabulary_id'), $values);
              if (count($entity_type) == 0) {
                $entity_type = chado_insert_record('tripal_vocabulary', $values);
                $entity_type_id = $entity_type['vocabulary_id'];
              }
              else {
                $entity_type_id = $entity_type[0]->vocabulary_id;
              }

              // Next add a record to the tripal_term table.
              $values = array(
                'vocabulary_id' => $entity_type_id,
                'cvterm_id' => $cvterm_id,
                'publish' => 0
              );
              $bundle_id = 0;
              $bundle = chado_select_record('tripal_term', array('term_id'), $values);
              if (count($bundle) == 0) {
                $bundle = chado_insert_record('tripal_term', $values);
                $bundle_id = $bundle['term_id'];
              }
              else {
                $bundle_id = $bundle[0]->term_id;
              }

              // Add the table where the records are found.
              $values = array(
                'term_id' => $bundle_id,
                'data_table' => $tablename,
                'type_table' => $tablename,
                'field' => $local_id
              );
              if (!chado_select_record('tripal_term_usage', array('bundle_source_id'), $values, array('has_record' => TRUE))) {
                chado_insert_record('tripal_term_usage', $values);
              }

              // Add the table where the records are found.
              $values = array(
                'vocabulary_id' => $entity_type_id,
                'data_table' => $tablename,
                'type_table' => $tablename,
                'field' => $local_id
              );
              if (!chado_select_record('tripal_vocabulary_usage', array('vocabulary_id'), $values, array('has_record' => TRUE))) {
                chado_insert_record('tripal_vocabulary_usage', $values);
              }
            }
          }
    }
  }
  catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    $transaction->rollback();
    watchdog_exception('tripal_ws', $e);
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
}