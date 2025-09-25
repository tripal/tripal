<?php

/**
 * Finds and saves all published entities from a Tripal 3 site.
 *
 * @param string $output_file_name
 *   The name of a tab-delimited file to be created.
 *   The columns are:
 *     bundle_label,
 *     chado table name,
 *     chado table pkey value,
 *     entity id value
 */
function find_all_published(string $output_file_name) {
  $grand_total = 0;

  // Get a list of defined bio_data bundles
  $sql = "SELECT name, label FROM [tripal_bundle] ORDER BY term_id";
  $results = chado_query($sql, []);
  $bundle_names = [];
  while ($obj = $results->fetchObject()) {
    $bundle_names[$obj->name] = $obj->label;
  }

  // Open output file for writing
  $outf = fopen($output_file_name, 'w');

  foreach($bundle_names as $bundle_name => $bundle_label) {
    print "Processing \"$bundle_name\" = \"$bundle_label\"\n";
    $total = 0;

    list($table, $pkey_field, $results) = query_one_bundle($bundle_name);

    if ($results) {
      while ($record = $results->fetchAssoc()) {
        fwrite($outf, implode("\t",
          [
            $bundle_label,
            $table,
            $record[$pkey_field],
            $record['entity_id']
          ]) . "\n");
        $total++;
        $grand_total++;
      }
    }
    print '  ' . number_format($total) . " $bundle_label records saved\n";
  }

  print number_format($grand_total) . " total records saved to \"$output_file_name\"\n";
}

/**
 * Finds and returns all published entities from one bundle
 *
 * @param string $bundle_name
 *   The bundle name e.g. "bio_data_8"
 */
function query_one_bundle(string $bundle_name) {

  // The code in this function is based on
  // tripal/tripal_chado/api/tripal_chado.api.inc function chado_publish_records()
  // with some modifications

  // Load the bundle entity so we can get information about which Chado
  // table/field this entity belongs to.
  $bundle = tripal_load_bundle_entity(['name' => $bundle_name]);

  if (!$bundle) {
    throw new Exception("Unknown bundle $bundle_name");
  }
  $chado_entity_table = chado_get_bundle_entity_table($bundle);

  // Get the mapping of the bio data type to the Chado table.
  $chado_bundle = db_select('chado_bundle', 'cb')
    ->fields('cb')
    ->condition('bundle_id', $bundle->id)
    ->execute()
    ->fetchObject();
  if (!$chado_bundle) {
    throw new Exception("Cannot find mapping of bundle $bundle_name to Chado tables");
  }

  $table = $chado_bundle->data_table;
  $type_column = $chado_bundle->type_column;
  $type_linker_table = $chado_bundle->type_linker_table;
  $cvterm_id = $chado_bundle->type_id;
  $type_value = $chado_bundle->type_value;

  // Get the table information for the Chado table.
  $table_schema = chado_get_schema($table);
  $pkey_field = $table_schema['primary key'][0];

  // Construct the SQL for identifying which records should be used.
  $args = [];
  $from = "
    FROM {" . $table . "} T
      LEFT JOIN [" . $chado_entity_table . "] CE on CE.record_id = T.$pkey_field
  ";

  $where = " WHERE CE.record_id IS NOT NULL";

  // Handle records that are mapped to property tables.
  if ($type_linker_table and $type_column and $type_value) {
    $propschema = chado_get_schema($type_linker_table);
    $fkeys = $propschema['foreign keys'][$table]['columns'];
    foreach ($fkeys as $leftkey => $rightkey) {
      if ($rightkey == $pkey_field) {
        $from .= " INNER JOIN {" . $type_linker_table . "} LT ON T.$pkey_field = LT.$leftkey ";
      }
    }
    $where .= " AND LT.$type_column = :cvterm_id and LT.value = :prop_value";
    $args[':cvterm_id'] = $cvterm_id;
    $args[':prop_value'] = $type_value;
  }

  // Handle records that are mapped to cvterm linking tables.
  if ($type_linker_table and $type_column and !$type_value) {
    $cvtschema = chado_get_schema($type_linker_table);
    $fkeys = $cvtschema['foreign keys'][$table]['columns'];
    foreach ($fkeys as $leftkey => $rightkey) {
      if ($rightkey == $pkey_field) {
        $from .= " INNER JOIN {" . $type_linker_table . "} LT ON T.$pkey_field = LT.$leftkey ";
      }
    }
    $where .= " AND LT.$type_column = :cvterm_id";
    $args[':cvterm_id'] = $cvterm_id;
  }

  // Handle records that are mapped via a type_id column in the base table.
  if (!$type_linker_table and $type_column) {
    $where .= " AND T.$type_column = :cvterm_id";
    $args[':cvterm_id'] = $cvterm_id;
  }

  // Handle the case where records are in the cvterm table and mapped via a single
  // vocab.  Here we use the type_value for the cv_id.
  if ($table == 'cvterm' and $type_value) {
    $where .= " AND T.cv_id = :cv_id";
    $args[':cv_id'] = $type_value;
  }

  // Handle the case where records are in the cvterm table but we want to
  // use all of the child terms.
  if ($table == 'cvterm' and !$type_value) {
    $where .= " AND T.cvterm_id IN (
        SELECT CVTP.subject_id
        FROM {cvtermpath} CVTP
        WHERE CVTP.object_id = :cvterm_id)
      ";
    $args[':cvterm_id'] = $cvterm_id;
  }

  $sql = "SELECT * " . $from . $where;
  $results = chado_query($sql, $args);
  return [$table, $pkey_field, $results];
}



$output_file_name = drush_shift();
if ($output_file_name) {
  find_all_published($output_file_name);
}
else {
  print "An output file name must be supplied, for example: drush php:script export_tripal3_entity_mapping.php tripal3.tsv\n";
}
