<?php
/**
 * @file
 *
 * This script will generate the schema file for the Tripal API for an
 * installation of Chado. To use the script you must install the version of
 * Chado desired using Tripal.
 *
 ***** RUN USING DRUSH ****
 * Example Usage:
 *   cd modules/contrib/tripal/tripal_chado/chado_schema
 *   drush php-script generate_chado_schema_yaml.php > chado_schema-1.3.yml
 */

// Iterate through the tables of Chado and use the Schema module to
// generate a schema array for each table.
$sql = "
  SELECT table_name
  FROM information_schema.tables
  WHERE
    table_schema = 'chado' AND
    table_type = 'BASE TABLE' AND
    table_name NOT like 'tripal%'
  ORDER BY table_name
";
$result = \Drupal::database()->query($sql);
$table_schemas = [];
$referring = [];
while ($table = $result->fetchField()) {

  if ($table == 'materialized_view') {
    continue;
  }

  // Start with the name of the table.
  print '' . $table . ":\n";

  // -- Description:
  // Find the comments to add the description.
  $description = \Drupal::database()->query("SELECT obj_description('chado.".$table."'::regclass, 'pg_class')")->fetchAll();
  $description = (!empty($description)) ? str_replace("\n", " ", $description[0]->obj_description) : '';
  print "  description: ". '"' . addslashes($description). '"' ."\n";

  // -- Columns/fields:
  print "  fields:\n";
  $results = \Drupal::database()->query("SELECT column_name, data_type, is_nullable, character_maximum_length, ordinal_position, column_default
    FROM information_schema.columns
    WHERE table_name = :table
      AND table_schema = 'chado'", [':table' => $table])->fetchAll();
  foreach ($results as $c) {
    print "    " . $c->column_name . ":\n";

    // Data type:
    if (strpos($c->column_default, 'nextval') !== FALSE) {
      print "      type: serial\n";
    }
    else {
      print "      type: " . $c->data_type . "\n";
    }

    // Not Null:
    if ($c->is_nullable == 'YES') {
      print "      not null: FALSE\n";
    }
    else {
      print "      not null: TRUE\n";
    }

    // Size:
    if ($c->character_maximum_length) {
      print "      size: " . $c->character_maximum_length . "\n";
    }

    // Default Value:
    if ((strpos($c->column_default, '::') === FALSE) AND !empty($c->column_default)) {
      print "      default: " . $c->column_default . "\n";
    }
  }

  // -- Retrieve information for unique, primary and foreign keys.
  $sql = "
    SELECT
        tc.constraint_name, tc.constraint_type, tc.table_name, kcu.column_name,
        ccu.table_name AS foreign_table_name,
        ccu.column_name AS foreign_column_name
    FROM
        information_schema.table_constraints AS tc
        JOIN information_schema.key_column_usage AS kcu ON tc.constraint_name = kcu.constraint_name
        JOIN information_schema.constraint_column_usage AS ccu ON ccu.constraint_name = tc.constraint_name
    WHERE
        tc.table_name = :name
        AND tc.table_schema = 'chado'
        ";
  $results = \Drupal::database()->query($sql, [':name' => $table])->fetchAll();
  $ukeys = [];
  $fkeys = [];
  $pkey = [];
  foreach ($results as $r) {
    switch ($r->constraint_type) {
      case 'UNIQUE':
        $ukeys[ $r->constraint_name ][$r->column_name] = $r->column_name;
        break;
      case 'PRIMARY KEY':
        $pkey[$r->column_name] = $r->column_name;
        break;
      case 'FOREIGN KEY':
        $fkeys[$r->foreign_table_name]['table'] = $r->foreign_table_name;
        $fkeys[$r->foreign_table_name]['columns'][$r->column_name] = $r->foreign_column_name;
        break;
    }
  }

  // -- Unique Keys:
  if ($ukeys) {
    ksort($ukeys);
    print "  unique keys:\n";
    foreach ($ukeys as $uname => $ucolumns) {
      print "    " . $uname . ": " . implode(', ', $ucolumns) . "\n";
    }
  }

  // -- Indicies:

  // -- Primary Key:
  print "  primary key: " . implode(', ', $pkey) . "\n";

  // -- Foreign Keys:
  if ($fkeys) {
    ksort($fkeys);
    print "  foreign keys:\n";
    foreach ($fkeys as $fk) {
      print "    " . $fk['table'] . ":\n";
      print "      table: " . $fk['table'] . "\n";
      print "      columns:\n";
      foreach ($fk['columns'] as $r => $l) {
        print "        " . $r . ": " . $l . "\n";
      }
    }
  }

  // -- Referring Tables:
  $sql = "
    SELECT
        tc.constraint_name, tc.constraint_type, tc.table_name, kcu.column_name,
        ccu.table_name AS foreign_table_name,
        ccu.column_name AS foreign_column_name
    FROM
        information_schema.table_constraints AS tc
        JOIN information_schema.key_column_usage AS kcu ON tc.constraint_name = kcu.constraint_name
        JOIN information_schema.constraint_column_usage AS ccu ON ccu.constraint_name = tc.constraint_name
    WHERE
        ccu.table_name = :name
        AND tc.table_schema = 'chado'";
  $results = \Drupal::database()->query($sql, [':name' => $table])->fetchAll();
  if ($results) {
    $reftables = [];
    foreach ($results as $r) {
      if ($r->table_name != $table) {
        $reftables[$r->table_name] = $r->table_name;
      }
    }
    sort($reftables);
    print "  referring_tables: " . implode(", ", $reftables) . "\n";
  }
}
