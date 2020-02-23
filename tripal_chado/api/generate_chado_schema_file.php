<?php
/**
 * @file
 *
 * This script will generate the schema file for the Tripal API for an
 * installation of Chado. To use the script you must install the version of
 * Chado desired using Tripal. Next install and enable the 'schema' module
 * from the Drupal module respository.  Finally, add a new 'chado'
 * entry in the $databases variable of the settings.php file. For
 * example:
 *
 * @code
 *'chado' => array(
 *   'default' => array(
 *     'database' => 'd7x_t2x_c13',
 *     'username' => 'chado',
 *     'password' => 'testing123',
 *     'host' => 'localhost',
 *     'port' => '',
 *     'driver' => 'pgsql',
 *     'prefix' => '',
 *   ),
 * ),
 * @endcode
 *
 * This script requires a single argument (-v) which is the Chado version.
 * Redirect output into a new file as desired.
 *
 * Example usage in drupal directory root:
 *
 * php
 *   ./sites/all/modules/tripal/tripal_core/api/generate_chado_schema_file.php
 *   -v 1.11 > \
 *   ./sites/all/modules/tripal/tripal_core/api/tripal_core.schema_v1.11.api.inc.new
 *
 * php
 *   ./sites/all/modules/tripal/tripal_core/api/generate_chado_schema_file.php
 *   -v 1.2 > \
 *   ./sites/all/modules/tripal/tripal_core/api/tripal_core.schema_v1.2.api.inc.new
 *
 * php
 *   ./sites/all/modules/tripal/tripal_core/api/generate_chado_schema_file.php
 *   -v 1.3 > \
 *   ./sites/all/modules/tripal/tripal_core/api/tripal_core.schema_v1.3.api.inc.new
 */

$arguments = getopt("v:");

if (isset($arguments['v'])) {
  $drupal_base_url = parse_url('http://www.example.com');
  $_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
  $_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
  $_SERVER['REMOTE_ADDR'] = NULL;
  $_SERVER['REQUEST_METHOD'] = NULL;

  define('DRUPAL_ROOT', getcwd());

  require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
  drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

  $version = $arguments['v'];
  $safe_version = preg_replace('/\./', '_', $version);

  print("<?php \n" .
    "/**\n" .
    " * @file\n" .
    " * Describes the chado tables in version $version\n" .
    " */\n" .
    "\n" .
    "/**\n" .
    " * @defgroup tripal_schema_v" . $safe_version . "_api Chado v" . $version . " Schema API\n" .
    " * @ingroup tripal_chado_schema_api\n" .
    " * @{\n" .
    " * Provides an application programming interface (API) for describing Chado\n" .
    " * tables. This API consists of a set of functions, one for each table in Chado.\n" .
    " * Each function simply returns a Drupal style array that defines the table.\n" .
    " *\n" .
    " * Because Drupal does not handle foreign key (FK) relationships, which are\n" .
    " * needed to for Tripal Views, they have been added to the schema defintitions\n" .
    " * below.\n" .
    " *\n" .
    " * The functions provided in this documentation should not be called as is,\n" .
    " * but if you need the Drupal-style array definition for any table, use the\n" .
    " * following function call:\n" .
    " *\n" .
    " *   \$table_desc = chado_get_schema(\$table)\n" .
    " *\n" .
    " * where the variable \$table contains the name of the table you want to\n" .
    " * retireve.  The chado_get_schema function determines the appropriate version\n" .
    " * of Chado and uses the Drupal hook infrastructure to call the appropriate\n" .
    " * hook function to retrieve the table schema.\n" .
    " *\n" .
    " * If you need to augment these schema definitions within your own module,\n" .
    " * you need to implement the hook_chado_schema_v" . $safe_version . "_[table name]() hook where\n" .
    " * [table name] is the name of the chado table whose schema definition you\n" .
    " * want to augment.\n" .
    " * @}\n" .
    " */\n"
  );

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
  $result = db_query($sql);
  $table_schemas = [];
  $referring = [];
  while ($table = $result->fetchField()) {

    // Get the schema for each table.
    $schema = schema_dbobject('chado')->inspect(NULL, $table);
    $schema = $schema[$table];

    // Get the foreign keys and add them to the array.
    $fks = db_query($fksql, [':table_name' => $table]);
    $schema['foreign keys'] = [];
    foreach ($fks as $fk) {
      $schema['foreign keys'][$fk->foreign_table_name]['table'] = $fk->foreign_table_name;
      $schema['foreign keys'][$fk->foreign_table_name]['columns'][$fk->column_name] = $fk->foreign_column_name;
      $reffering[$fk->foreign_table_name][] = $table;
    }

    // Add a table and description key to the top.
    $schema = ['table' => $table] + $schema;
    $schema = ['description' => ''] + $schema;

    // Fix the datetime fields and add a description field.
    foreach ($schema['fields'] as $fname => $details) {
      if ($schema['fields'][$fname]['type'] == "timestamp without time zone") {
        $schema['fields'][$fname]['type'] = 'datetime';
      }
      $schema['fields'][$fname]['description'] = '';
    }

    // Remove the 'name' key.
    unset($schema['name']);

    $table_schemas[$table] = $schema;
  }

  // Now iterate through the tables now that we have all the referring info
  // and generate the function strings.
  foreach ($table_schemas as $table => $schema) {

    $schema['referring_tables'] = [];
    if (count($reffering[$table]) > 0) {
      $schema['referring_tables'] = array_unique($reffering[$table]);
    }

    // Reformat the array to be more legible.
    $arr = var_export($schema, 1);
    // Move array( to previous line.
    $arr = preg_replace("/\n\s+array/", "array", $arr);
    // Add indentation.
    $arr = preg_replace("/\n/", "\n  ", $arr);
    $arr = preg_replace("/true/", "TRUE", $arr);
    $arr = preg_replace("/false/", "FALSE", $arr);
    $arr = preg_replace("/array \(/", "array(", $arr);

    print (
      "/**\n" .
      " * Implements hook_chado_schema_v" . $safe_version . "_" . $table . "()\n" .
      " * \n" .
      " * Purpose: To describe the structure of '$table' to tripal\n" .
      " * @see chado_insert_record()\n" .
      " * @see chado_update_record()\n" .
      " * @see chado_select_record()\n" .
      " * @see chado_generate_var()\n" .
      " * @see chado_expan_var()\n" .
      " *\n" .
      " * @return\n" .
      " *    An array describing the '$table' table\n" .
      " *\n" .
      " * @ingroup tripal_chado_v" . $version . "_schema_api\n" .
      " *\n" .
      " */\n" .
      "function tripal_core_chado_schema_v" . $safe_version . "_" . $table . "() {\n" .
      "  \$description = $arr; \n " .
      "  return \$description;\n" .
      "}\n"
    );
  }
  // Finally add the tables function for this version.
  $table_list = '';
  foreach ($table_schemas as $table => $schema) {
    $table_list .= "    '$table',\n";
  }
  print (
    "/**\n" .
    " * Lists the table names in the v" . $version . " chado schema\n" .
    " *\n" .
    " * @return\n" .
    " *    An array containing all of the table names\n" .
    " *\n" .
    " * @ingroup tripal_chado_v" . $version . "_schema_api\n" .
    " *\n" .
    " */\n" .
    "function tripal_core_chado_get_v" . $safe_version . "_tables() {\n" .
    "  \$tables = array(\n" .
    "$table_list" .
    "  );\n" .
    "  return \$tables;\n" .
    "}\n"
  );
}