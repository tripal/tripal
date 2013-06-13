<?php

/**
 * @file
 * Contains functions for creating, editing and deleting custom tables
 * on the Tripal website.
 *
 * @ingroup tripal_core
 */

/**
 * A template function which returns markup to display details for the custom table
 *
 * @param $table_id
 *  The unique ID of the custom table
 *
 * @ingroup tripal_core
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
    $output .= "  <tr>" .
    "    <th>Table Name</th>" .
    "    <td>$custom_table->table_name</td>" .
    "  </tr>";
  }
  if ($custom_table->schema) {
    $output .= "  <tr>" .
    "    <th>Table Field Definitions</th>" .
    "    <td><pre>" . var_export(unserialize($custom_table->schema), 1) . "</pre></td>" .
    "  </tr>";
  }

  // build the URLs using the url function so we can handle installations where
  // clean URLs are or are not used
  $delete_url = url("admin/tripal/custom_tables/action/delete/$custom_table->table_id");
  $edit_url = url("admin/tripal/custom_tables/edit/$custom_table->table_id");
  $output .= "<tr><th>Actions</th>" .
            "<td>" .
            "     <a href='$edit_url'>Edit</a>, " .
            "     <a href='$delete_url'>Delete</a></td></tr>";
  $output .= "</table>";

  return $output;
}

/**
 * A template function to render a listing of all Custom tables
 *
 * @ingroup tripal_core
 */
function tripal_custom_tables_list() {
  $header = array('', 'Table Name', 'Description');
  $rows = array();
  $custom_tables = db_query("SELECT * FROM {tripal_custom_tables} ORDER BY table_name");

  while ($custom_table = db_fetch_object($custom_tables)) {
 
    $rows[] = array(
      l(t('View'), "admin/tripal/custom_tables/view/$custom_table->table_id") . " | " .
      l(t('Edit'), "admin/tripal/custom_tables/edit/$custom_table->table_id") . " | " .
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
    $default_force_drop = $form_state['values']['force_drop'];

    if (!$default_table_name) {
      $default_table = $custom_table->table_name;
    }
    if (!$default_schema) {
      $default_schema = var_export(unserialize($custom_table->schema), 1);
      $default_schema = preg_replace('/=>\s+\n\s+array/', '=> array', $default_schema);
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
       nor the data in them.  Linker tables or property tables are often a good candidate for
       a custom table. For example a table to link stocks and libraries (e.g. library_stock) would be
       a good custom table. Try to model linker or propery tables after existing tables.  If the
       table already exists it will not be modified.  To force dropping and recreation of the table
       click the checkbox below.
    '),
  );

  $form['force_drop']= array(
    '#type'          => 'checkbox',
    '#title'         => t('Re-create table'),
    '#description'   => t('Check this box if your table already exists and you would like to drop it and recreate it.'),
    '#default_value' => $default_force_drop,
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
  $force_drop = $form_state['values']['force_drop'];

  if (!$schema) {
    form_set_error($form_state['values']['schema'],
      t('Schema array field is required.'));
  }

  // make sure the array is valid
  $schema_array = array();
  if ($schema) {
    $success = preg_match('/^\s*array/', $schema);
    if (!$success) {
      form_set_error($form_state['values']['schema'],
        t("The schema array should begin with the word 'array'."));
    }
    else {
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
      if ($action == 'Edit') {
        // see if the table name has changed. If so, then check to make sure
        // it doesn't already exists. We don't want to drop a table we didn't mean to
        $sql = "SELECT * FROM {tripal_custom_tables} WHERE table_id = %d";
        $ct = db_fetch_object(db_query($sql, $table_id));
        if ($ct->table_name != $schema_array['table']) {
          $previous_db = tripal_db_set_active('chado');
          $exists = db_table_exists($schema_array['table']);
          tripal_db_set_active($previous_db);        
          if ($exists) {
            form_set_error($form_state['values']['schema'],
              t("The table name already exists, please choose a different name."));  
          }
        }
      }     
    }
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
  $force_drop = $form_state['values']['force_drop'];
  
  $skip_creation = 1;
  if ($force_drop) {
     $skip_creation = 0;
  }

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
    $sql = "DELETE FROM {tripal_custom_tables} " .
           "WHERE table_id = $table_id";
    db_query($sql);
    
    // drop the table from chado if it exists
    if (db_table_exists($custom_table->table_name)) {
      $success = chado_query("DROP TABLE %s", $custom_table->table_name);
      if ($success) {
        drupal_set_message(t("Custom Table '%name' dropped", array('%name' => $custom_table->table_name)));
      }
    }
  }

  // Redirect the user
  if ($redirect) {
    drupal_goto("admin/tripal/custom_tables");
  }
}
