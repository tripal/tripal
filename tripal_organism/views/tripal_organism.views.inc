<?php
/*******************************************************************************
 *  This function provides data for the VIEWS module. The function generates
 *  a new field 'Tripal organism: Common name' when adding a 'Node' view type. 
 */

function tripal_organism_views_data() {
   //Delete the custom table that provides data for the cutom view if exists
   if (db_table_exists('tripal_organism_views_common_name')) {
      $sql = "DROP TABLE {tripal_organism_views_common_name}";
      db_query ($sql);
   }
   
   // Create the custom table. After created, it will be deleted only when the 
   // tripal_organism module is uninstalled.
   drupal_install_schema('tripal_organism_views_common_name');
   
   // Populate the custom table
   // Get organism_id from chado_organism table
   $sql = 'SELECT nid, organism_id FROM {chado_organism}';
   $result = db_query($sql);
   
   //Retrieve common_names from chado database using the organism_id
   $sql = 'SELECT common_name FROM {organism} WHERE organism_id=%d';
   $previous_db = tripal_db_set_active ('chado');
   $common_names = array ();
   while ($org = db_fetch_object($result)) {     
      $common_name = db_result(db_query ($sql, $org->organism_id));
      $common_names [$org->nid] = $common_name;
   }
   tripal_db_set_active ($previous_db);
   
   // Insert common_name and nid to the custom table
   $sql = "INSERT INTO {tripal_organism_views_common_name} (nid, common_name) VALUES (%d, '%s')";
   foreach ($common_names as $nid => $common_name) {
      db_query($sql, $nid, $common_name);
   }
   
   // The following describes our custom table to the Views module
   $data['tripal_organism_views_common_name'] = array(
      // This table section is needed for each custom view created
      'table' => array(
         'group' => 'Tripal organism',
         'join' => array(
            'node' => array(
               'left_field' => 'nid',
               'field' => 'nid',        
            ),
         ),
      ),
      // Describe the column 'common_name' to show as a field in the Views
      'common_name' => array(
         'title' => t('Common name'),
         // The help that appears on the UI,
         'help' => t('The common name of an organism.'),
         // Information for displaying the nid
         'field' => array(
            'handler' => 'views_handler_field_node',
            'click sortable' => TRUE,
         ),
         // Information for accepting a common_name as a filter
         'filter' => array(
            'handler' => 'views_handler_filter_numeric',
         ),
         // Information for sorting on a common_name.
         'sort' => array(
            'handler' => 'views_handler_sort',
         ),        
      ),
   );
   return $data;  
}

/*******************************************************************************
 * The table schema generated to provide data for the Views module. The table
 * will be created when the Views module calls to hook_view_data(). It will
 * be deleted only when the tripal_organism module is uninstalled.
 */
function tripal_organism_views_common_name_schema() {

   $schema['tripal_organism_views_common_name'] = array(
      'fields' => array(
         'nid' => array(
            'type' => 'int',
            'unsigned' => TRUE,
            'not null' => TRUE,
            'default' => 0
         ),
         'common_name' => array(
            'type' => 'varchar',
            'length' => 255,
            'not null' => TRUE,
            'default' => 'NA',
            'description' => t('To provide common name for the Views module')
         ),
      ),
  );
  return $schema;
}

//  Experimental codes that don't create extra tables in Drupal database
/*
function tripal_organism_views_data() {

   // Make Chado 'Organism' as a base table, so it can show as a Views type
   $data['organism'] = array(
      // This table section is needed for each custom view created
      'table' => array(
         'group' => 'Tripal',
         'base' => array(
            'field' => 'organism_id',
            'title' => t('Tripal Organism'),
            'help' => t('Tripal Organism'),
            'database' => 'chado',
         ),
      ),
      // Describe the columns of organism table to the Views so it knows how to make the query
      'common_name' => array(
         'title' => t('Common Name'),
         'help' => t('Showing the common name of an organism.'), // The help that appears on the UI,
         // Information for displaying the nid
         'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
         ),
         // Information for accepting a common_name as a filter
         'filter' => array(
            'handler' => 'views_handler_filter_string',
         ),
         // Information for sorting on a common_name.
         'sort' => array(
            'handler' => 'views_handler_sort',
         ),        
      ),
   );

   return $data;  
}
*/
