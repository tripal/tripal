<?php

/**
 * @file
 * Contains functions for the Custom Tables API

 * @defgroup tripal_custom_tables_api Core Module Custom Tables API
 * @{
 * Provides an application programming interface (API) to manage custom tables in Chado.
 * @}
 * @ingroup tripal_api
 */


/**
 * Edits a custom table in the chado database. It supports 
 * using the Drupal Schema API array.
 *
 * @param $table_id
 *   The table_id of the table to edit
 * @param $table_name
 *   The name of the custom table
 * @param $schema
 *   Use the Schema API array to define the custom table.
 * @param $skip_creation
 *   Set as TRUE to skip dropping and re-creation of the table.  This is
 *   useful if the table was already created through another means and you
 *   simply want to make Tripal aware of the table schema.
 *
 * @ingroup tripal_custom_tables_api
 */
function tripal_core_edit_custom_table($table_id, $table_name, $schema, $skip_creation = 0) {

  // Create a new record
  $record = new stdClass();
  $record->table_id = $table_id;
  $record->table_name = $table_name;
  $record->schema = serialize($schema);

  // get the current custom table record
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = %d";
  $custom_table = db_fetch_object(db_query($sql, $table_id));

  // drop the table from chado if it exists
  if(!$skip_creation){
    if (db_table_exists($custom_table->table_name)) {
      chado_query("DROP TABLE %s", $custom_table->table_name);
      drupal_set_message(t("Custom Table '%name' dropped", array('%name' => $custom_table->table_name)));
    }
  }

  // update the custom table record and re-create the table in Chado
  if (drupal_write_record('tripal_custom_tables', $record, 'table_id')) {

    // drop the table from chado if it exists
    if(!$skip_creation){
      if (db_table_exists($custom_table->table_name)) {
        chado_query("DROP TABLE %s", $custom_table->table_name);
        drupal_set_message(t("Custom Table '%name' dropped", array('%name' => $custom_table->table_name)));
      }

      // re-create the table
      if (!tripal_core_create_custom_table ($ret, $table_name, $schema)) {
        drupal_set_message(t("Could not create the custom table. Check Drupal error report logs."));
      }
      else {
        drupal_set_message(t("Custom table '%name' created", array('%name' => $table_name)));
      }
    }
    // TODO: add FK constraints
  }
}

/**
 * Add a new table to the Chado schema. This function is simply a wrapper for
 * the db_create_table() function of Drupal, but ensures the table is created
 * inside the Chado schema rather than the Drupal schema.  If the table already
 * exists then it will be dropped and recreated using the schema provided.
 * However, it will only drop a table if it exsits in the tripal_custom_tables
 * table. This way the function cannot be used to accidentally alter existing
 * non custom tables.  If $skip_creation is set then the table is simply
 * added to the tripal_custom_tables and no table is created in Chado.
 *
 * @param $ret
 *   Array to which query results will be added.
 * @param $table
 *   The name of the table to create.
 * @param $schema
 *   A Drupal-style Schema API definition of the table
 * @param $skip_creation
 *   Set as TRUE to skip dropping and re-creation of the table.  This is
 *   useful if the table was already created through another means and you
 *   simply want to make Tripal aware of the table schema.
 *
 * @return
 *   A database query result resource for the new table, or FALSE if table was not constructed.
 *
 * @ingroup tripal_custom_tables_api
 */
function tripal_core_create_custom_table(&$ret, $table, $schema, $skip_creation = 0) {
  $ret = array();
    
  // see if the table entry already exists
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_name = '%s'";
  $centry = db_fetch_object(db_query($sql, $table));
  
  // If the table exits in Chado but not in the tripal_custom_tables field
  // then call an error.  if the table exits in the tripal_custom_tables but
  // not in Chado then create the table and replace the entry.  
  $previous_db = tripal_db_set_active('chado');  // use chado database
  $exists = db_table_exists($table);
  tripal_db_set_active($previous_db);  // now use drupal database

  if (!$exists) {
    $previous_db = tripal_db_set_active('chado');  // use chado database
    db_create_table($ret, $table, $schema);
    tripal_db_set_active($previous_db);  // now use drupal database
    if (count($ret)==0) {
      watchdog('tripal_core', "Error adding custom table '!table_name'.",
        array('!table_name' => $table), WATCHDOG_ERROR);
      return FALSE;
    }
  }
  if ($exists and !$centry and !$skip_creation) {
    watchdog('tripal_core', "Could not add custom table '!table_name'. It ".
            "already exists but is not known to Tripal as being a custom table.",
      array('!table_name' => $table), WATCHDOG_WARNING);
    return FALSE;
  }
  if ($exists and $centry and !$skip_creation) {
    // drop the table we'll recreate it with the new schema
    $previous_db = tripal_db_set_active('chado');  // use chado database
    db_drop_table($ret, $table);
    db_create_table($ret, $table, $schema);
    tripal_db_set_active($previous_db);  // now use drupal database
  }

  // add an entry in the tripal_custom_table
  $record = new stdClass();
  $record->table_name = $table;
  $record->schema = serialize($schema);

  // if an entry already exists then remove it
  if ($centry) {
    $sql = "DELETE FROM {tripal_custom_tables} WHERE table_name = '%s'";
    db_query($sql, $table);
  }
  $success = drupal_write_record('tripal_custom_tables', $record);
  if (!$success) {
    watchdog('tripal_core', "Error adding custom table %table_name.",
      array('%table_name' => $table), WATCHDOG_ERROR);
    drupal_set_message(t("Could not add custom table %table_name. 
      Please check the schema array.", array('%table_name' => $table)), 'error');      
    return FALSE;
  }
  
  // now add any foreign key constraints
  if(!$skip_creation and array_key_exists('foreign keys', $schema)){
  	$fkeys = $schema['foreign keys'];
  	foreach ($fkeys as $fktable => $fkdetails) {
  		$relations = $fkdetails['columns'];
  		foreach ($relations as $left => $right) {
  			$sql = "ALTER TABLE $table ADD CONSTRAINT " . 
  			  $table . "_" . $left . "_fkey FOREIGN KEY ($left) REFERENCES  $fktable ($right) " .
  			  "ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED";
  			if(!chado_query($sql)){
  			  watchdog('tripal_core', "Error, could not add foreign key contraint to custom table.",
            array('!table_name' => $table), WATCHDOG_ERROR);
			    drupal_set_message(t("Could not add foreign key contraint to table %table_name. 
			      Please check the schema array and the report log for errors.", 
			      array('%table_name' => $table)), 'error');      
          return FALSE;
  			}
  		}
  	}
  }

  return TRUE;
}

/**
 * Retrieve the custom table id given the name
 *
 * @param $table_name
 *   The name of the custom table
 *
 * @return
 *   The unique identifier for the given table
 *
 * @ingroup tripal_custom_tables_api
 */
function tripal_custom_tables_get_table_id($table_name) {
  $sql = "SELECT * FROM {tripal_custom_tables} ".
         "WHERE table_name = '%s'";
  if (db_table_exists('tripal_custom_tables')) {
    $custom_table = db_fetch_object(db_query($sql, $table_name));
    if ($custom_table) {
      return $custom_table->table_id;
    }
  }

  return FALSE;
}



/**
 * A template function which returns markup to display details for the custom table
 *
 * @param $table_id
 *  The unique ID of the custom table
 *
 * @ingroup tripal_custom_tables_api
 */
function tripal_custom_table_view($table_id) {

  // get this custom_table details
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = %d";
  $custom_table = db_fetch_object(db_query($sql, $table_id));

  // create a table with each row containig stats for
  // an individual job in the results set.
  $return_url = url("admin/tripal/custom_tables/");
  $output .= "<p><a href=\"$return_url\">" . t("Return to list of custom tables") . "</a></p>";
  $output .= "<br />";
  $output .= "<p>Details for <b>$custom_table->table_name</b>:</p>";
  $output .= "<br />";
  $output .= "<table class=\"border-table\">";
  if ($custom_table->table_name) {
    $output .= "  <tr>".
    "    <th>Table Name</th>".
    "    <td>$custom_table->table_name</td>".
    "  </tr>";
  }
  if ($custom_table->schema) {
    $output .= "  <tr>".
    "    <th>Table Field Definitions</th>".
    "    <td><pre>" . var_export(unserialize($custom_table->schema),1) . "</pre></td>".
    "  </tr>";
  }

  // build the URLs using the url function so we can handle installations where
  // clean URLs are or are not used
  $delete_url = url("admin/tripal/custom_tables/action/delete/$custom_table->table_id");
  $edit_url = url("admin/tripal/custom_tables/edit/$custom_table->table_id");
  $output .= "<tr><th>Actions</th>".
            "<td>".
            "     <a href='$edit_url'>Edit</a>, ".
            "     <a href='$delete_url'>Delete</a></td></tr>";
  $output .= "</table>";

  return $output;
}

/**
 * A template function to render a listing of all Custom tables
 *
 * @ingroup tripal_custom_tables_api
 */
function tripal_custom_tables_list() {
  $header = array('', 'Table Name', 'Description');
  $rows = array();
  $custom_tables = db_query("SELECT * FROM {tripal_custom_tables} ORDER BY table_name");

  while ($custom_table = db_fetch_object($custom_tables)) {
 
    $rows[] = array(
      l(t('View'), "admin/tripal/custom_tables/view/$custom_table->table_id") ." | ".
      l(t('Edit'), "admin/tripal/custom_tables/edit/$custom_table->table_id") ." | ".
      $custom_table->table_name,
      $custom_table->comment,
      l(t('Delete'), "admin/tripal/custom_tables/action/delete/$custom_table->table_id"),
    );
  }

  $rows[] = array(
    'data' => array(
      array('data' => l(t('Create a new custom table.'), "admin/tripal/custom_tables/new"),
        'colspan' => 6),
    )
  );

  $page = theme('table', $header, $rows);
  return $page;
}

/**
 * A Form to Create/Edit a Custom table
 *
 * @param $form_state
 *   The current state of the form (Form API)
 * @param $table_id
 *   The unique ID of the Custom table to Edit or NULL if creating a new table
 *
 * @return
 *   A form array (Form API)
 *
 * @ingroup tripal_core
 */
function tripal_custom_tables_form(&$form_state = NULL, $table_id = NULL) {

  if (!$table_id) {
    $action = 'Add';
  }
  else {
    $action = 'Edit';
  }

  // get this requested table
  if (strcmp($action, 'Edit')==0) {
    $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = %d ";
    $custom_table = db_fetch_object(db_query($sql, $table_id));

    // set the default values.  If there is a value set in the
    // form_state then let's use that, otherwise, we'll pull
    // the values from the database
    $default_schema = $form_state['values']['schema'];
    $default_skip = $form_state['values']['skip_creation'];

    if (!$default_table_name) {
      $default_table = $custom_table->table_name;
    }
    if (!$default_schema) {
      $default_schema = var_export(unserialize($custom_table->schema),1);
    }
  }

  // Build the form
  $form['action'] = array(
    '#type' => 'value',
    '#value' => $action
  );

  $form['table_id'] = array(
    '#type' => 'value',
    '#value' => $table_id
  );
  
  $form['instructions']= array(
    '#type'          => 'markup',
    '#value'         => t('At times it is necessary to add a custom table to the Chado schema.  
       These are not offically sanctioned tables but may be necessary for local data requirements.  
       Avoid creating custom tables when possible as other GMOD tools may not recognize these tables
       nor the data in them.  Linker tables are often a good candidate for
       a custom table. For example a table to link stocks and libraries (e.g. library_stock).  If the
       table already exists it will be dropped and re-added using the definition supplied below. All 
       data in the table will be lost.  However, If you
       are certain the schema definition you provide is correct for an existing table, select the checkbox
       below to skip creation of the table.
    '),
  );

  $form['skip_creation']= array(
    '#type'          => 'checkbox',
    '#title'         => t('Skip Table Creation'),
    '#description'   => t('If your table already exists, check this box to prevent it from being dropped and re-created.'),
    '#default_value' => $default_skip,
  );
  $form['schema']= array(
    '#type'          => 'textarea',
    '#title'         => t('Schema Array'),
    '#description'   => t('Please enter the Drupal Schema API compatible array that defines the table.'),
    '#required'      => FALSE,
    '#default_value' => $default_schema,
    '#rows'          => 25,
  );

  if ($action == 'Edit') {
    $value = 'Save';
  }
  if ($action == 'Add') {
    $value = 'Add';
  }
  $form['submit'] = array(
    '#type'         => 'submit',
    '#value'        => t($value),
    '#executes_submit_callback' => TRUE,
  );
  $form['#redirect'] = 'admin/tripal/custom_tables';
  
  $form['example']= array(
    '#type'          => 'markup',
    '#value'         => "<br>Example library_stock table: <pre>
array (
  'table' => 'library_stock',
  'fields' => array (
    'library_stock_id' => array(
      'type' => serial,
      'not null' => TRUE,
    ),
    'library_id' => array(
      'type' => 'int',
      'not null' => TRUE,
    ),      
    'stock_id' => array(
      'type' => 'int',
      'not null' => TRUE,
    ),
  ),
  'primary key' => array(
    'library_stock_id'
  ),
  'unique keys' => array(
    'library_stock_c1' => array(
      'library_id',
      'stock_id'
    ),
  ),
  'foreign keys' => array(
    'library' => array(
      'table' => 'library',
      'columns' => array(
        'library_id' => 'library_id',
      ),
    ),
    'stock' => array(
      'table' => 'stock',
      'columns' => array(
        'stock_id' => 'stock_id',
      ),
    ),
  ),
)
    </pre>",
  );
  

  return $form;
}

/**
 * Validate the Create/Edit custom table form
 * Implements hook_form_validate().
 *
 * @ingroup tripal_core
 */
function tripal_custom_tables_form_validate($form, &$form_state) {
  $action = $form_state['values']['action'];
  $table_id = $form_state['values']['table_id'];
  $schema = $form_state['values']['schema'];

  if (!$schema) {
    form_set_error($form_state['values']['schema'],
      t('Schema array field is required.'));
  }

  // make sure the array is valid
  $schema_array = array();
  if ($schema) {
    $success = eval("\$schema_array = $schema;");
    if ($success === FALSE) {
      $error = error_get_last();
      form_set_error($form_state['values']['schema'],
        t("The schema array is improperly formatted. Parse Error : " . $error["message"]));
    }
    if (is_array($schema_array) and !array_key_exists('table', $schema_array)) {
      form_set_error($form_state['values']['schema'],
        t("The schema array must have key named 'table'"));
    }

    // TODO: add in more validation checks of the array to help the user
  }
}

/**
 * Submit the Create/Edit Custom table form
 * Implements hook_form_submit().
 *
 * @ingroup tripal_core
 */
function tripal_custom_tables_form_submit($form, &$form_state) {

  $ret = array();
  $action = $form_state['values']['action'];
  $table_id = $form_state['values']['table_id'];
  $schema = $form_state['values']['schema'];
  $skip_creation = $form_state['values']['skip_creation'];

  // conver the schema into a PHP array
  $schema_arr = array();
  eval("\$schema_arr = $schema;");

  if (strcmp($action, 'Edit') == 0) {
    tripal_core_edit_custom_table($table_id, $schema_arr['table'], $schema_arr, $skip_creation);
  }
  elseif (strcmp($action, 'Add') == 0) {
    tripal_core_create_custom_table($ret, $schema_arr['table'], $schema_arr, $skip_creation);
  }
  else {
    drupal_set_message(t("No action performed."));
  }

  return '';
}
/**
 * Does the specified action for the specified custom table
 *
 * @param $op
 *   The action to be taken. Currenly only delete is available
 * @param $table_id
 *   The unique ID of the custom table for the action to be performed on
 * @param $redirect
 *   TRUE/FALSE depending on whether you want to redirect the user to admin/tripal/custom_tables
 *
 * @ingroup tripal_core
 */
function tripal_custom_tables_action($op, $table_id, $redirect = FALSE) {
  global $user;

  $args = array("$table_id");
  if (!$table_id) {
    return '';
  }

  // get this table details
  $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = %d";
  $custom_table = db_fetch_object(db_query($sql, $table_id));

  if ($op == 'delete') {
  
    // remove the entry from the tripal_custom tables table
    $sql = "DELETE FROM {tripal_custom_tables} ".
           "WHERE table_id = $table_id";
    db_query($sql);
    
    // drop the table from chado if it exists
    if (db_table_exists($custom_table->table_name)) {
      $success = chado_query("DROP TABLE %s", $custom_table->table_name);
      if($success){
        drupal_set_message(t("Custom Table '%name' dropped", array('%name' => $custom_table->table_name)));
      }
    }
  }

  // Redirect the user
  if ($redirect) {
    drupal_goto("admin/tripal/custom_tables");
  }
}
