<?php
/**
 * @file
 * This script will add FK relatinsions to an existing schema API array for each
 * Chado table.  It requires Chado is installed in a 'chado' schema of
 * the drupal database.  It also requires existing schema hooks for
 * version of Chado.  The goal is to use the output of this script to
 * update the existing schema hooks.  Redirect the output of this script to
 * a file and then replace the existing schema API include file (e.g.
 * tripal_core.schema_v1.2.api.inc).  Be sure to check it before replacing
 *
 * This script requires a single argument (-v) which is the Chado version.
 *
 * Example usage in drupal directory root:
 *
 * php ./sites/all/modules/tripal/tripal_core/api/get_FKs.php -v 1.11 > \
 *   ./sites/all/modules/tripal/tripal_core/apitripal_core.schema_v1.11.api.inc.new
 *
 * php ./sites/all/modules/tripal/tripal_core/api/get_FKs.php -v 1.2 > \
 *   ./sites/all/modules/tripal/tripal_core/api/tripal_core.schema_v1.2.api.inc.new
 */

$arguments = getopt("v:");

if (isset($arguments['v'])) {
  $drupal_base_url = parse_url('http://www.example.com');
  $_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
  $_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
  $_SERVER['REMOTE_ADDR'] = NULL;
  $_SERVER['REQUEST_METHOD'] = NULL;

  require_once 'includes/bootstrap.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

  $version = $arguments['v'];
  get_chado_fk_relationships($version);
}

/**
 * This function does the actual work of determining the foreign key relationships from
 * the database and creating the schema file.
 */
function get_chado_fk_relationships($version) {

  // convert the version to a form suitable for function names
  $v = $version;
  $v = preg_replace("/\./","_",$v);

  $tables = tripal_core_get_chado_tables();
  $sql ="
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

  // iterate through the tables and get the foreign keys
  print "<?php
/* @file: This file contains default schema definitions for all chado v$version tables
 *        to be used by other function. Specifically these functions are used
 *        by the tripal_core select/insert/update API functions and by
 *        the Tripal Views module.
 *
 *        These schema definitions can be augmented by another modules
 *        (specifically to add missing definitions) by implementing
 *        hook_chado_schema_v" . $v . "_<table name>().
 *
 * @defgroup tripal_schema_api Core Module Schema API
 * @{
 * Provides an application programming interface (API) for describing Chado tables.
 * This API consists of a set of functions, one for each table in Chado.  Each
 * function simply returns a Drupal style array that defines the table.
 *
 * Because Drupal 6 does not handle foreign key (FK) relationships, however FK
 * relationships are needed to for Tripal Views.  Therefore, FK relationships
 * have been added to the schema defintitions below.
 *
 * The functions provided in this documentation should not be called as is, but if you need
 * the Drupal-style array definition for any table, use the following function
 * call:
 *
 *   \$table_desc = tripal_core_get_chado_table_schema(\$table)
 *
 * where the variable \$table contains the name of the table you want to
 * retireve.  The tripal_core_get_chado_table_schema function determines the appropriate version of
 * Chado and uses the Drupal hook infrastructure to call the appropriate
 * hook function to retrieve the table schema.
 *
 * @}
 * @ingroup tripal_api
 */
";
  $referring = array();
  $tables_def = array();
  foreach ($tables as $table) {

    // get the existing table array
    $table_arr = tripal_core_get_chado_table_schema($table);

    if (empty($table_arr)) {
       print "ERROR: empty table definition $table\n";
       continue;
    }

    // add the table name to the array
    $table_arr['table'] = $table;

    // get the foreign keys and add them to the array
    $fks = db_query($sql, array(':table_name' => $table));
    foreach ($fks as $fk) {
      $table_arr['foreign keys'][$fk->foreign_table_name]['table'] = $fk->foreign_table_name;
      $table_arr['foreign keys'][$fk->foreign_table_name]['columns'][$fk->column_name] = $fk->foreign_column_name;
      $reffering[$fk->foreign_table_name][] = $table;
    }
    $tables_def[] = $table_arr;
  }

  // now add in the referring tables and print
  foreach ($tables_def as $table_arr) {
    $table = $table_arr['table'];

    // add in the referring tables
    $table_referring = array_unique($reffering[$table]);
    $table_arr['referring_tables'] = $table_referring;

    // reformat the array to be more legible
    $arr = var_export($table_arr, 1);
    $arr = preg_replace("/\n\s+array/","array", $arr); // move array( to previous line
    $arr = preg_replace("/\n/","\n  ", $arr); // add indentation
    $arr = preg_replace("/true/","TRUE", $arr); // add indentation
    $arr = preg_replace("/false/","FALSE", $arr); // add indentation
    $arr = preg_replace("/array \(/","array(", $arr); // add indentation

      // print out the new Schema API function for this table
print "/**
 * Implements hook_chado_schema_v".$v."_".$table."()
 * Purpose: To describe the structure of '$table' to tripal
 * @see tripal_core_chado_insert()
 * @see tripal_core_chado_update()
 * @see tripal_core_chado_select()
 *
 * @return
 *    An array describing the '$table' table
 *
 * @ingroup tripal_chado_v".$version."_schema_api
 *
 */
function tripal_core_chado_schema_v".$v."_".$table."() {
  \$description =  $arr;
  return \$description;
}
";
  }
}
