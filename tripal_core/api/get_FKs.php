<?php
// This script will generate an updated schema API array for each 
// Chado table.  It requires Chado is installed in a 'chado' schema of 
// the drupal database.  It also requires existing schema hooks for 
// version of Chado.  The goal is to use the output of this script to 
// update the existing schema hooks.  Redirect the output of this script to 
// a file and then replace the existing schema API include file (e.g. 
// tripal_core.schema_v1.2.api.inc).  Be sure to check it before replacing

// thie script requires a single argument (-v) which is the Chado version
//
// example usage in drupal directory root:
//
// php ./sites/all/modules/tripal/tripal_core/api/get_FKs.php -v 1.11 > \
//   ./sites/all/modules/tripal/tripal_core/apitripal_core.schema_v1.11.api.inc.new
//
// php ./sites/all/modules/tripal/tripal_core/api/get_FKs.php -v 1.2 > \
//   ./sites/all/modules/tripal/tripal_core/api/tripal_core.schema_v1.2.api.inc.new


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
 *
 */
function get_chado_fk_relationships($version){

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
    WHERE constraint_type = 'FOREIGN KEY' AND tc.table_name='%s'
  ";
    
  // iterate through the tables and get the foreign keys
  print "<?php\n";
  foreach ($tables as $table){

     // get the existing table array
     $table_arr = tripal_core_get_chado_table_schema($table);
     
     if(empty($table_arr)){
        print "ERROR: emptye table definition $table\n";
     }

     // get the foreign keys and add them to the array
     $fks = db_query($sql,$table);
     while($fk = db_fetch_object($fks)){
        $table_arr['foreign keys'][$fk->foreign_table_name]['table'] = $fk->foreign_table_name;
        $table_arr['foreign keys'][$fk->foreign_table_name]['columns'][$fk->column_name] = $fk->foreign_column_name;
      }
      
      // reformat the array to be more legible
      $arr = var_export($table_arr,1);
      $arr = preg_replace("/\n\s+array/","array",$arr); // move array( to previous line
      $arr = preg_replace("/\n/","\n  ",$arr); // add indentation
      
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
