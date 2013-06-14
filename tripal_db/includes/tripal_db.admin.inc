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
 */
function tripal_db_select_form() {
  return $form;
}

/**
 * 
 * @param $form
 * @param $form_state
 * 
 * @ingroup tripal_db
 */
function tripal_db_db_edit_form($form, $form_state) {
  
	// get the dbid if form was submitted via AJAX
	$dbid = 0;
	if (array_key_exists('values', $form_state)) {
    $dbid = $form_state['values']['dbid'];
	}  
  
  // get a list of db from chado for user to choose
  $sql = "SELECT * FROM {db} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);
  
  $dbs = array();
  $dbs[] = 'Select a database';
  foreach ($results as $db) {
    $dbs[$db->db_id] = $db->name;
  }
  
  $form['dbid'] = array(
    '#title' => t('External Database Name'),
    '#type' => 'select',
    '#options' => $dbs,
    '#ajax' => array(
      'callback' => 'tripal_db_edit_form_ajax',
      'wrapper'  => 'db-edit-div',
      'effect'   => 'fade',
      'event'    => 'change',
      'method'   => 'replace',
    ),
    '#suffix' => '<div id="db-edit-div"></div>',
    '#default_value' => $dbid,
  );
      
  // if we don't have a db_id then we can safetly return the form
  if ($dbid) {
  	  	
    // if we are here because of an AJAX calback, that means the database select
    // box was changed and the form is being rebuilt on an AJAX call not on a
    // form validation error.  Therefore, we want to provide the dbid  
    if (array_key_exists('triggering_element', $form_state) and 
        $form_statte['triggerming_element']['#ajax']['callback'] = tripal_db_edit_form_ajax) {
	    tripal_db_add_db_form_fields($form, $form_state, $dbid);
    }
    else {
    	tripal_db_add_db_form_fields($form, $form_state);
    }
	  
	  $form['update'] = array(
	    '#type'         => 'submit',
	    '#value'        => t('Update'),
	    '#weight'       => 5,
	  );
	  $form['delete'] = array(
	    '#type'         => 'submit',
	    '#value'        => t('Delete'),
      '#weight'       => 6,
    );
  }
  return $form;
}
/**
 * 
 * @param $form
 * @param $form_state
 * 
 * @ingroup tripal_db
 */
function tripal_db_db_add_form($form, $form_state) {
	
	// add in the form fields to this form
  tripal_db_add_db_form_fields($form, $form_state);
  
  $form['add'] = array(
    '#type'         => 'submit',
    '#value'        => t('Add'),
    '#weight'       => 5,
  );
  return $form;
}
/**
 * 
 * @param $form
 * @param $form_state
 * @param $dbid
 * 
 * @ingroup tripal_db
 */
function tripal_db_add_db_form_fields(&$form, $form_state, $dbid = NULL) {
	
	$default_db        = '';
  $default_desc      = '';
  $default_url       = '';
  $default_urlprefix = '';
      
  // get the default values from the databae first  
  if ($dbid) {
    $values = array('db_id' => $dbid);
    $db = tripal_core_chado_select('db', array('*'), $values);    
    $default_db        = $db[0]->name;
    $default_desc      = $db[0]->description;
    $default_url       = $db[0]->url;
    $default_urlprefix = $db[0]->urlprefix;  
  }    
  // set the default value to the form_state if those exist
  elseif (array_key_exists('values', $form_state)) {
    $default_db        = array_key_exists('name', $form_state['values'])        ? $form_state['values']['name']        : $default_db;
    $default_desc      = array_key_exists('description', $form_state['values']) ? $form_state['values']['description'] : $default_desc;
    $default_url       = array_key_exists('url', $form_state['values'])         ? $form_state['values']['url']         : $default_url;
    $default_urlprefix = array_key_exists('urlprefix', $form_state['values'])   ? $form_state['values']['urlprefix']   : $default_urlprefix;      
  }     
    
  // add a fieldset for the Drupal Schema API
  $form['fields'] = array(
    '#type' => 'fieldset',
    '#title' => 'Database Details',
    '#collapsible' => 0,
  );    
  
  $form['fields']['name']= array(
    '#type'          => 'textfield',
    '#title'         => t("Database Name"),
    '#description'   => t('Please enter the name for this external database.'),
    '#required'      => TRUE,
    '#default_value' => $default_db,
    '#weight'        => 1,
    '#maxlength'     => 255,
  );

  $form['fields']['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#description'   => t('Please enter a description for this database'),
    '#default_value' => $default_desc,
    '#weight'        => 2,
    '#maxlength'     => 255,
  );
  $form['fields']['url']= array(
    '#type'          => 'textfield',
    '#title'         => t('URL'),
    '#description'   => t('Please enter the web address for this database.'),
    '#default_value' => $default_url,
    '#weight'        => 3,
    '#maxlength'     => 255,
  );
  $form['fields']['urlprefix']= array(
    '#type'          => 'textfield',
    '#title'         => t('URL prefix'),
    '#description'   => t('Tripal can provide links to external databases when accession numbers or unique identifiers are known.  Typically, a database will provide a unique web address for each accession and the accession usually is the last component of the page address.  Please enter the web address, minus the accession number for this database.  When an accession number is present, Tripal will combine this web address with the accession and provide a link to the external site.'),
    '#default_value' => $default_urlprefix,
    '#weight'        => 4,
    '#maxlength'     => 255,
  );

  return $form;
}

/**
 * Validation fucntion for tripal_db_db_add_form
 * @param $form
 * @param $form_state
 * 
 * @ingroup tripal_db
 */
function tripal_db_db_add_form_validate($form, &$form_state) {
  tripal_db_form_fields_validate($form, $form_state); 
}
/**
 * Validation fucntion for tripal_db_db_edit_form
 * @param unknown_type $form
 * @param unknown_type $form_state
 * 
 * @ingroup tripal_db
 */
function tripal_db_db_edit_form_validate($form, &$form_state) {
  tripal_db_form_fields_validate($form, $form_state); 
}
/**
 * Genetic validation form for shared fields of both the edit and add forms
 * @param $form
 * @param $form_state
 * 
 * @ingroup tripal_db
 */
function tripal_db_form_fields_validate($form, &$form_state) {
  $name =  trim($form_state['values']['name']);
  $desc =  trim($form_state['values']['description']);
  $url  =  trim($form_state['values']['url']);
  $urlp =  trim($form_state['values']['urlprefix']);
 
  // make sure the database name is unique
  $values = array('name' => $name);
  $results = tripal_core_chado_select('db', array('db_id'), $values);   
  if (count($results) > 0 and $results[0]->db_id != $dbid) {
    form_set_error('name', 'The database name must be unique');
  }
}
/**
 * 
 * @param $form
 * @param $form_state
 * 
 * @ingroup tripal_db
 */
function tripal_db_db_add_form_submit($form, &$form_state) {

  $name =  trim($form_state['values']['name']);
  $desc =  trim($form_state['values']['description']);
  $url  =  trim($form_state['values']['url']);
  $urlp =  trim($form_state['values']['urlprefix']);

  $values = array(
    'name' => $name,
    'description' => $desc,
    'url' => $url,
    'urlprefix' => $urlp,
  );
  $success = tripal_core_chado_insert('db', $values);
  if ($success) {
    drupal_set_message(t("External database added"));
  }
  else {
    drupal_set_message(t("Failed to add external database."));
  }
}

/**
 * 
 * @param $form
 * @param $form_state
 * 
 * @ingroup tripal_db
 */
function tripal_db_db_edit_form_submit($form, &$form_state) {

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
/**
 * Ajax callback for the tripal_db_form
 * @ingroup tripal_db
 */
function tripal_db_edit_form_ajax($form, $form_state) {
	unset($form['dbid']);	
  return $form;
}