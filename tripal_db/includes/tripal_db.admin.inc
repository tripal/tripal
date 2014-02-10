<?php
/**
 * @file
 * Provide administration of dbs & dbxrefs
 */

/**
 * Launchpad for database reference administration
 *
 * @ingroup tripal_db
 */
function tripal_db_admin_db_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Modules', 'admin/tripal/chado');
  $breadcrumb[] = l('Databases', 'admin/tripal/chado/tripal_db');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $dbs_view = views_embed_view('tripal_db_admin_dbs','default');
  $dbxrefs_view = views_embed_view('tripal_db_admin_dbxrefs','default');
  if (isset($dbs_view) && isset($dbxrefs_view)) {
    $output .= $dbs_view;
  }
  else {
    $output .= '<p>The Tripal DB Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    if (!isset($dbs_view)) {
      $output .= '<li>'.l('DB Admin', 'admin/tripal/chado/tripal_db/views/dbs/enable').'</li>';
    }
    if (!isset($dbxrefs_view)) {
      $output .= '<li>'.l('DB Reference Admin', 'admin/tripal/chado/tripal_db/views/dbxrefs/enable').'</li>';
    }
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Form to edit existing databases.
 * Implements hook_form().
 *
 * @ingroup tripal_db
 */
function tripal_db_db_edit_form($form, &$form_state) {

  // get the dbid if form was submitted via AJAX
  $dbid = 0;
  if (array_key_exists('values', $form_state)) {
    $dbid = $form_state['values']['dbid'];
  }
  elseif (isset($form_state['build_info']['args'][0])) {
    $dbid = $form_state['build_info']['args'][0];
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
    '#default_value' => $dbid,
  );


  // if we don't have a db_id then we can  return the form, otherwise
  // add in the other fields
  if ($dbid) {
    tripal_db_add_db_form_fields($form, $form_state, $dbid);

    $form['update'] = array(
      '#type'         => 'submit',
      '#value'        => t('Update'),
    );
    $form['delete'] = array(
      '#type'         => 'submit',
      '#value'        => t('Delete'),
      '#attributes'   => array('onclick' => 'if(!confirm("Really Delete?")){return false;}'),
    );
  }
  else {
    // if we don't have a dbid then this is the first time the form has
    // benn loaded and we need to create the div where ajax replacement elements get stored
    $form['div_replace'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="db-edit-div">',
      '#suffix' => '</div>',
    );
  }
  return $form;
}

/**
 * Form to add new databases.
 * Implements hook_form().
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
 * Fields commmon between the add/edit forms
 *
 * @ingroup tripal_db
 */
function tripal_db_add_db_form_fields(&$form, $form_state, $dbid = NULL) {

  $default_db        = '';
  $default_desc      = '';
  $default_url       = '';
  $default_urlprefix = '';

  // get the default values from the database first
  if ($dbid) {
    $values = array('db_id' => $dbid);
    $result = chado_select_record('db', array('*'), $values);
    $db = $result[0];
    $default_db = $db->name;
    $default_desc = $db->description;
    $default_url = $db->url;
    $default_urlprefix = $db->urlprefix;
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
    '#maxlength'     => 255,
  );

  $form['fields']['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#description'   => t('Please enter a description for this database'),
    '#default_value' => $default_desc,
    '#maxlength'     => 255,
  );
  $form['fields']['url']= array(
    '#type'          => 'textfield',
    '#title'         => t('URL'),
    '#description'   => t('Please enter the web address for this database.'),
    '#default_value' => $default_url,
    '#maxlength'     => 255,
  );
  $form['fields']['urlprefix']= array(
    '#type'          => 'textfield',
    '#title'         => t('URL prefix'),
    '#description'   => t('Tripal can provide links to external databases when accession numbers or unique identifiers are known.  Typically, a database will provide a unique web address for each accession and the accession usually is the last component of the page address.  Please enter the web address, minus the accession number for this database.  When an accession number is present, Tripal will combine this web address with the accession and provide a link to the external site.'),
    '#default_value' => $default_urlprefix,
    '#maxlength'     => 255,
  );
  return $form;
}

/**
 * Validation fucntion for tripal_db_db_add_form
 *
 * @ingroup tripal_db
 */
function tripal_db_db_add_form_validate($form, &$form_state) {
  tripal_db_form_fields_validate($form, $form_state);
}

/**
 * Validation fucntion for tripal_db_db_edit_form
 *
 * @ingroup tripal_db
 */
function tripal_db_db_edit_form_validate($form, &$form_state) {
  tripal_db_form_fields_validate($form, $form_state);
}

/**
 * Generic form validation for shared fields of both the edit and add forms
 *
 * @ingroup tripal_db
 */
function tripal_db_form_fields_validate($form, &$form_state) {
  $name =  array_key_exists('name', $form_state['values'])        ? trim($form_state['values']['name']) : '';
  $desc =  array_key_exists('description', $form_state['values']) ? trim($form_state['values']['description']) : '';
  $url  =  array_key_exists('url', $form_state['values'])         ? trim($form_state['values']['url']) : '';
  $urlp =  array_key_exists('urlprefix', $form_state['values'])   ? trim($form_state['values']['urlprefix']) : '';
  $dbid =  array_key_exists('dbid', $form_state['values'])        ? trim($form_state['values']['dbid']) : '';

  // make sure the database name is unique
  $values = array('name' => $name);
  $results = chado_select_record('db', array('db_id'), $values);
  if (count($results) > 0 and $results[0]->db_id != $dbid) {
    form_set_error('name', 'The database name must be unique');
  }
}

/**
 * Submit for add db form
 *
 * @ingroup tripal_db
 */
function tripal_db_db_add_form_submit($form, &$form_state) {

  $name =  array_key_exists('name', $form_state['values'])        ? trim($form_state['values']['name']) : '';
  $desc =  array_key_exists('description', $form_state['values']) ? trim($form_state['values']['description']) : '';
  $url  =  array_key_exists('url', $form_state['values'])         ? trim($form_state['values']['url']) : '';
  $urlp =  array_key_exists('urlprefix', $form_state['values'])   ? trim($form_state['values']['urlprefix']) : '';

  $values = array(
    'name' => $name,
    'description' => $desc,
    'url' => $url,
    'urlprefix' => $urlp,
  );
  $success = chado_insert_record('db', $values);
  if ($success) {
    drupal_set_message(t("External database added"));
  }
  else {
    drupal_set_message(t("Failed to add external database."));
  }
}

/**
 * Submit for edit db form
 *
 * @ingroup tripal_db
 */
function tripal_db_db_edit_form_submit($form, &$form_state) {

  $name =  array_key_exists('name', $form_state['values'])        ? trim($form_state['values']['name']) : '';
  $desc =  array_key_exists('description', $form_state['values']) ? trim($form_state['values']['description']) : '';
  $url  =  array_key_exists('url', $form_state['values'])         ? trim($form_state['values']['url']) : '';
  $urlp =  array_key_exists('urlprefix', $form_state['values'])   ? trim($form_state['values']['urlprefix']) : '';
  $dbid =  array_key_exists('dbid', $form_state['values'])        ? trim($form_state['values']['dbid']) : '';
  $op   =  trim($form_state['values']['op']);

  $values = array(
    'name' => $name,
    'description' => $desc,
    'url' => $url,
    'urlprefix' => $urlp,
  );

  if (strcmp($op, 'Update')==0) {
    $match = array('db_id' => $dbid);
    $success = chado_update_record('db', $match, $values);
    if ($success) {
      drupal_set_message(t("External database updated"));
    }
    else {
      drupal_set_message(t("Failed to update external database."));
    }
  }
  if (strcmp($op, 'Delete')==0) {
    $match = array('db_id' => $dbid);
    $success = chado_delete_record('db', $match);
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
 *
 * @ingroup tripal_db
 */
function tripal_db_edit_form_ajax($form, $form_state) {

  $elements = array();

  // add in the form fields and the buttons
  if (array_key_exists('dbid', $form_state['values'])) {
    $elements['fields'] = $form['fields'];
    $elements['update'] = $form['update'];
    $elements['delete'] = $form['delete'];
  }

   // add back in the db-edit-div that is used for the next round of AJAX
  $elements['fields']['#prefix'] =  '<div id="db-edit-div">';
  $elements['fields']['#suffix'] =  '</div">';

  // reset the values for the fields to the defaults
  $elements['fields']['name']['#value']        = $elements['fields']['name']['#default_value'];
  $elements['fields']['description']['#value'] = $elements['fields']['description']['#default_value'];
  $elements['fields']['url']['#value']         = $elements['fields']['url']['#default_value'];
  $elements['fields']['urlprefix']['#value']   = $elements['fields']['urlprefix']['#default_value'];

  //drupal_set_message('<pre>' . print_r($elements, TRUE) . '</pre>', "status");

  return $elements;
}
