<?php
/**
 *
 *
 * @ingroup tripal_db
 */
function tripal_db_admin_page() {
  $add_url = url("admin/tripal/tripal_db/add_db");
  $output = "<a href=\"$add_url\">Add a new external database</a>";
  $output .= drupal_get_form('tripal_db_select_form');
  $output .= '<div id="db-edit-div">Please select a database above to view or edit</div>';
  return $output;
}
/**
 *
 *
 * @ingroup tripal_db
 */
function tripal_db_select_form() {
  return $form;
}
/**
 *
 * @ingroup tripal_db
 */
function tripal_ajax_db_edit() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = drupal_render($form);  

  // bind javascript events to the new objects that will be returned 
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )  
  );
}

/**
 *
 * @ingroup tripal_db
 */
function tripal_db_form(&$form_state = NULL, $action = 'Update') {
  
  $dbid = $form_state['values']['dbid'];
  
  
  if (strcmp($action,'Update')==0) {
    // get a list of db from chado for user to choose
    $sql = "SELECT * FROM {db} WHERE NOT name = 'tripal' ORDER BY name ";
    $results = chado_query($sql);
  
    $dbs = array();
    $dbs[] = '';
    while ($db = db_fetch_object($results)) {
      $dbs[$db->db_id] = $db->name;
    }
  
    $form['dbid'] = array(
      '#title' => t('External Database Name'),
      '#type' => 'select',
      '#options' => $dbs,
      '#ahah' => array(
        'path' => 'admin/tripal/tripal_db/edit/js',
        'wrapper' => 'db-edit-div',
        'effect' => 'fade',
        'event' => 'change',
        'method' => 'replace',
      ),
      '#prefix' => '<div id="db-edit-div">',
      '#suffix' => '</div>',
      '#default_value' => $dbid,
      '#description' => t('Please select a database to edit'),
    );
  }   
  else {
    $default_db = $form_state['values']['name'];
    $default_desc = $form_state['values']['description'];
    $default_url = $form_state['values']['url'];
    $default_urlprefix = $form_state['values']['urlprefix']; 
  }
  
  // get this requested database
  if ($dbid) {
    $values = array('db_id' => $dbid);
    $result = tripal_core_chado_select('db', array('*'), $values);
    $db = $result[0];
    $prev_dbid = $form_state['values']['prev_dbid'];
    // if the database has changed then repopulate the fields with the databaes values
    if ($prev_dbid != $dbid) {
      $default_db        = $db->name;
      $default_desc      = $db->description;
      $default_url       = $db->url;
      $default_urlprefix = $db->urlprefix;
    }
    // if the database did not change then keep the values in the form values
    else {
      $default_db = $form_state['values']['name'];
      $default_desc = $form_state['values']['description'];
      $default_url = $form_state['values']['url'];
      $default_urlprefix = $form_state['values']['urlprefix'];      
    }
  }
  
  $form['form_action'] = array(
    '#type' => 'hidden',
    '#value' => $action, 
  );  

  // we need to distinguish between edits in a field that may have failed
  // and when the user selects a different database from the list.  
  $form['prev_dbid'] = array(
    '#type' => 'hidden',
    '#value' => $dbid, 
  );  
  
  // if we want to update a database but the user has not
  // yet selected a database then return so we don't show the other fields
  // the rest of the fields will be added with the AHAH callback. 
  if (strcmp($action,'Update')==0 and !$dbid) {
    return $form;
  }

  $form['name']= array(
    '#type'          => 'textfield',
    '#title'         => t("Database Name"),
    '#description'   => t('Please enter the name for this external database.'),
    '#required'      => TRUE,
    '#default_value' => $default_db,
    '#weight'        => 1,
    '#maxlength'     => 255,
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#description'   => t('Please enter a description for this database'),
    '#default_value' => $default_desc,
    '#weight'        => 2,
    '#maxlength'     => 255,
  );
  $form['url']= array(
    '#type'          => 'textfield',
    '#title'         => t('URL'),
    '#description'   => t('Please enter the web address for this database.'),
    '#default_value' => $default_url,
    '#weight'        => 3,
    '#maxlength'     => 255,
  );
  $form['urlprefix']= array(
    '#type'          => 'textfield',
    '#title'         => t('URL prefix'),
    '#description'   => t('Tripal can provide links to external databases when accession numbers or unique identifiers are known.  Typically, a database will provide a unique web address for each accession and the accession usually is the last component of the page address.  Please enter the web address, minus the accession number for this database.  When an accession number is present, Tripal will combine this web address with the accession and provide a link to the external site.'),
    '#default_value' => $default_urlprefix,
    '#weight'        => 4,
    '#maxlength'     => 255,
  );


  if (strcmp($action, 'Update')==0) {
    $form['update'] = array(
      '#type'         => 'submit',
      '#value'        => t('Update'),
      '#weight'       => 5,
      '#executes_submit_callback' => TRUE,
    );
    $form['delete'] = array(
      '#type'         => 'submit',
      '#value'        => t('Delete'),
      '#weight'       => 6,
      '#executes_submit_callback' => TRUE,
    );
  }
  else {
    $form['add'] = array(
      '#type'         => 'submit',
      '#value'        => t('Add'),
      '#weight'       => 5,
      '#executes_submit_callback' => TRUE,
    );
  }

  return $form;
}
/**
 *
 * @ingroup tripal_db
 */
function tripal_db_form_validate($form, &$form_state) {
  $name =  trim($form_state['values']['name']);
  $desc =  trim($form_state['values']['description']);
  $url  =  trim($form_state['values']['url']);
  $urlp =  trim($form_state['values']['urlprefix']);
  $dbid =  trim($form_state['values']['dbid']);
  $op   =  trim($form_state['values']['op']);
  $action =  $form_state['values']['form_action'];
 
  // make sure the database name is unique
  $values = array('name' => $name);
  $results = tripal_core_chado_select('db', array('db_id'), $values);   
  if (count($results) > 0 and $results[0]->db_id != $dbid) {
    form_set_error('name', 'The database name must be unique');
  }
}
/**
 *
 * @ingroup tripal_db
 */
function tripal_db_form_submit($form, &$form_state) {

  $name =  trim($form_state['values']['name']);
  $desc =  trim($form_state['values']['description']);
  $url  =  trim($form_state['values']['url']);
  $urlp =  trim($form_state['values']['urlprefix']);
  $dbid =  trim($form_state['values']['dbid']);
  $op   =  trim($form_state['values']['op']);

  $values = array(
    'name' => $name,
    'description' => $desc,
    'url' => $url,
    'urlprefix' => $urlp,
  );
  if ($dbid) {
    if (strcmp($op, 'Update')==0) {      
      $match = array('db_id' => $dbid);
      $success = tripal_core_chado_update('db', $match, $values);
      if ($success) {
        drupal_set_message(t("External database updated"));
      }
      else {
        drupal_set_message(t("Failed to update external database."));
      }
    }
    if (strcmp($op, 'Delete')==0) {
      $match = array('db_id' => $dbid);
      $success = tripal_core_chado_delete('db', $match);
      if ($success) {
        drupal_set_message(t("External database deleted"));
      }
      else {
        drupal_set_message(t("Failed to delete external database."));
      }
    }
  }
  else {
    $success = tripal_core_chado_insert('db', $values);
    if ($success) {
      drupal_set_message(t("External database added"));
    }
    else {
      drupal_set_message(t("Failed to add external database."));
    }
  }
}