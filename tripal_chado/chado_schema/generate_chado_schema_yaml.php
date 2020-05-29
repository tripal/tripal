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
 *   drush php-script modules/t4d8/tripal_chado/chado_schema/generate_chado_schema_yaml.php > file
 */


$version = $arguments['v'];
$safe_version = preg_replace('/\./', '_', $version);

// The SQL for retreiving details about a table.
$fksql = "
  SELECT
      tc.constraint_name, tc.table_name, kcu.column_name,
      ccu.table_name AS foreign_table_name,
      ccu.column_name AS foreign_column_name
  FROM
      information_schema.table_constraints AS tc
      JOIN information_schema.key_column_usage AS kcu ON tc.constraint_name = kcu.constraint_name
      JOIN information_schema.constraint_column_usage AS ccu ON ccu.constraint_name = tc.constraint_name
  WHERE constraint_type = 'FOREIGN KEY' AND tc.table_name=:table_name
";

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

  // Start with the name of the table.
  print 'chado.' . $table . ":\n";

  // -- Description:
  // Find the comments to add the description.
  $description = \Drupal::database()->query("SELECT obj_description('chado.".$table."'::regclass, 'pg_class')")->fetchAll();
  $description = (!empty($description)) ? str_replace("\n", " ", $description[0]->obj_description) : '';
  print "  description: ". '"' . addslashes($description). '"' ."\n";

  // -- Columns/fields:
  print "  fields:\n";
  $results = \Drupal::database()->query("SELECT column_name, data_type, is_nullable, character_maximum_length, ordinal_position, column_default
    FROM information_schema.columns
    WHERE table_name = :table", [':table' => $table])->fetchAll();
  $columns = [];
  foreach ($results as $c) {
    $columns [ $c->ordinal_position ] = $c->column_name;

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
      print "      not null: TRUE\n";
    }
    else {
      print "      not null: FALSE\n";
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
  $sql = "SELECT con.conname as name, con.contype as type, con.conkey as key_columns
    FROM pg_catalog.pg_constraint con
    INNER JOIN pg_catalog.pg_class rel ON rel.oid = con.conrelid
    INNER JOIN pg_catalog.pg_namespace nsp ON nsp.oid = connamespace
    WHERE nsp.nspname = 'chado' AND rel.relname = '$table'";
  $results = \Drupal::database()->query($sql)->fetchAll();
  $ukeys = [];
  $fkeys = [];
  $pkey = [];
  foreach ($results as $r) {
    switch ($r->type) {
      case 'u':
        $tmp = trim($r->key_columns, "{} \t\n\r\0\x0B");
        foreach (explode(',', $tmp) as $t) {
          $ukeys[ $r->name ][] = $columns[$t];
        }
        break;
      case 'p':
        $tmp = trim($r->key_columns, "{} \t\n\r\0\x0B");
        foreach (explode(',', $tmp) as $t) {
          $pkey[] = $columns[$t];
        }
        break;
      case 'f':
        $tmp = trim($r->key_columns, "{} \t\n\r\0\x0B");
        foreach (explode(',', $tmp) as $t) {
          $fkeys[ $r->name ][] = $columns[$t];
        }
        break;
    }
  }

  // -- Unique Keys:
  if ($ukeys) {
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
    print "  foreign keys:\n";
    foreach ($fkeys as $fname => $fcolumns) {
      print "    " . $fname . ": " . implode(', ', $fcolumns) . "\n";
    }
  }
}
