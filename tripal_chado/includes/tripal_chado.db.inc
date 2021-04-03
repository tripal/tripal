<?php

/**
 * Launchpad for database reference administration
 *
 * @ingroup tripal_db
 */
function tripal_chado_admin_db_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Storage', 'admin/tripal/storage');
  $breadcrumb[] = l('Chado', 'admin/tripal/storage/chado');
  $breadcrumb[] = l('Databases', 'admin/tripal/loaders/chado_db');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $dbs_view = views_embed_view('tripal_db_admin_dbs', 'default');
  $dbxrefs_view = views_embed_view('tripal_db_admin_dbxrefs', 'default');
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
      $output .= '<li>' . l('DB Admin', 'admin/tripal/loaders/chado_db/views/dbs/enable') . '</li>';
    }
    if (!isset($dbxrefs_view)) {
      $output .= '<li>' . l('DB Reference Admin', 'admin/tripal/loaders/chado_db/views/dbxrefs/enable') . '</li>';
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
function tripal_chado_db_edit_form($form, &$form_state) {

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

  $dbs = [];
  $dbs[] = 'Select a database';
  foreach ($results as $db) {
    $dbs[$db->db_id] = $db->name;
  }

  $form['dbid'] = [
    '#title' => t('External Database Name'),
    '#type' => 'select',
    '#options' => $dbs,
    '#ajax' => [
      'callback' => 'tripal_chado_edit_form_ajax',
      'wrapper' => 'db-edit-div',
      'effect' => 'fade',
      'event' => 'change',
      'method' => 'replace',
    ],
    '#default_value' => $dbid,
  ];


  // If we don't have a db_id then we can  return the form, otherwise
  // add in the other fields.
  if ($dbid) {
    tripal_chado_add_db_form_fields($form, $form_state, $dbid);

    $form['update'] = [
      '#type' => 'submit',
      '#value' => t('Update'),
    ];

    $dbxref_count = chado_query('select count(dbxref_id) from {dbxref} WHERE db_id = :db_id', [':db_id' => $dbid])->fetchField();

    if ($dbxref_count == 0) {
      $form['delete'] = [
        '#type' => 'submit',
        '#value' => t('Delete'),
        '#attributes' => ['onclick' => 'if(!confirm("Really Delete?")){return false;}'],
      ];
    }
    else {
      tripal_set_message("You cannot delete this db without first deleting the ${dbxref_count} dbxrefs associated with it.", TRIPAL_NOTICE);
    }
  }
  else {
    // if we don't have a dbid then this is the first time the form has
    // benn loaded and we need to create the div where ajax replacement elements get stored
    $form['div_replace'] = [
      '#type' => 'item',
      '#prefix' => '<div id="db-edit-div">',
      '#suffix' => '</div>',
    ];
  }
  return $form;
}

/**
 * Form to add new databases.
 * Implements hook_form().
 *
 * @ingroup tripal_db
 */
function tripal_chado_db_add_form($form, &$form_state) {

  // add in the form fields to this form
  tripal_chado_add_db_form_fields($form, $form_state);

  $form['add'] = [
    '#type' => 'submit',
    '#value' => t('Add'),
    '#weight' => 5,
  ];
  return $form;
}

/**
 * Fields commmon between the add/edit forms
 *
 * @ingroup tripal_db
 */
function tripal_chado_add_db_form_fields(&$form, $form_state, $dbid = NULL) {

  $default_db = '';
  $default_desc = '';
  $default_url = '';
  $default_urlprefix = '';

  // get the default values from the database first
  if ($dbid) {
    $values = ['db_id' => $dbid];
    $result = chado_select_record('db', ['*'], $values);
    $db = $result[0];
    $default_db = $db->name;
    $default_desc = $db->description;
    $default_url = $db->url;
    $default_urlprefix = $db->urlprefix;

    $form['dbid'] = [
      '#type' => 'value',
      '#value' => $dbid,
    ];
  }

  // add a fieldset for the Drupal Schema API
  $form['fields'] = [
    '#type' => 'fieldset',
    '#title' => 'Database Details',
    '#collapsible' => 0,
  ];

  $form['fields']['name'] = [
    '#type' => 'textfield',
    '#title' => t("Database Name"),
    '#description' => t('Please enter the name for this external database.'),
    '#required' => TRUE,
    '#default_value' => $default_db,
    '#maxlength' => 255,
  ];

  $form['fields']['description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Please enter a description for this database'),
    '#default_value' => $default_desc,
    '#maxlength' => 255,
  ];
  $form['fields']['url'] = [
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => t('Please enter the web address for this database.'),
    '#default_value' => $default_url,
    '#maxlength' => 255,
  ];
  $form['fields']['urlprefix'] = [
    '#type' => 'textfield',
    '#title' => t('URL prefix'),
    '#description' => t('Tripal can provide links to external databases
        when accession numbers or unique identifiers are known.  Typically,
        a database will provide a unique web address for each accession and
        the accession usually is the last component of the page address.
        Please enter the web address, minus the accession number for this
        database.  By default, Tripal will
        combine this "URL prefix" with the database short name and
        accession to create a link to the external site. But, you can
        also use the tokens {db} and {accession} within the URL prefix
        to specify exactly where the database short name and the accession
        should be added.  Tripal will substitute the actual values in
        place of these tokens to create the link. (e.g. URL prefix: https://phytozome.jgi.doe.gov/phytomine/portal.do?externalid=PAC:{accession}).'),
    '#default_value' => $default_urlprefix,
    '#maxlength' => 255,
  ];
  return $form;
}

/**
 * Validation function for tripal_db_db_add_form
 *
 * @ingroup tripal_db
 */
function tripal_chado_db_add_form_validate($form, &$form_state) {
  tripal_chado_db_form_fields_validate($form, $form_state);
}

/**
 * Validation function for tripal_db_db_edit_form
 *
 * @ingroup tripal_db
 */
function tripal_chado_db_edit_form_validate($form, &$form_state) {
  tripal_chado_db_form_fields_validate($form, $form_state);
}

/**
 * Generic form validation for shared fields of both the edit and add forms
 *
 * @ingroup tripal_db
 */
function tripal_chado_db_form_fields_validate($form, &$form_state) {
  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $desc = array_key_exists('description', $form_state['values']) ? trim($form_state['values']['description']) : '';
  $url = array_key_exists('url', $form_state['values']) ? trim($form_state['values']['url']) : '';
  $urlp = array_key_exists('urlprefix', $form_state['values']) ? trim($form_state['values']['urlprefix']) : '';
  $dbid = array_key_exists('dbid', $form_state['values']) ? trim($form_state['values']['dbid']) : '';

  // make sure the database name is unique
  $values = ['name' => $name];
  $results = chado_select_record('db', ['db_id'], $values);
  if (count($results) > 0 and $results[0]->db_id != $dbid) {
    form_set_error('name', 'The database name must be unique');
  }
}

/**
 * Submit for add db form
 *
 */
function tripal_chado_db_add_form_submit($form, &$form_state) {

  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $desc = array_key_exists('description', $form_state['values']) ? trim($form_state['values']['description']) : '';
  $url = array_key_exists('url', $form_state['values']) ? trim($form_state['values']['url']) : '';
  $urlp = array_key_exists('urlprefix', $form_state['values']) ? trim($form_state['values']['urlprefix']) : '';

  $values = [
    'name' => $name,
    'description' => $desc,
    'url' => $url,
    'urlprefix' => $urlp,
  ];
  $success = chado_insert_record('db', $values);
  if ($success) {
    drupal_set_message(t("External database added"));
    drupal_goto('admin/tripal/loaders/chado_db');
  }
  else {
    drupal_set_message(t("Failed to add external database."));
  }
}

/**
 * Submit for edit db form
 *
 */
function tripal_chado_db_edit_form_submit($form, &$form_state) {

  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $desc = array_key_exists('description', $form_state['values']) ? trim($form_state['values']['description']) : '';
  $url = array_key_exists('url', $form_state['values']) ? trim($form_state['values']['url']) : '';
  $urlp = array_key_exists('urlprefix', $form_state['values']) ? trim($form_state['values']['urlprefix']) : '';
  $dbid = array_key_exists('dbid', $form_state['values']) ? trim($form_state['values']['dbid']) : '';
  $op = trim($form_state['values']['op']);

  $values = [
    'name' => $name,
    'description' => $desc,
    'url' => $url,
    'urlprefix' => $urlp,
  ];

  if (strcmp($op, 'Update') == 0) {
    $match = ['db_id' => $dbid];
    $success = chado_update_record('db', $match, $values);
    if ($success) {
      drupal_set_message(t("External database updated"));
      drupal_goto('admin/tripal/loaders/chado_db');
    }
    else {
      drupal_set_message(t("Failed to update external database."));
    }
  }
  if (strcmp($op, 'Delete') == 0) {
    $match = ['db_id' => $dbid];
    $success = chado_delete_record('db', $match);
    if ($success) {
      drupal_set_message(t("External database deleted"));
      drupal_goto('admin/tripal/loaders/chado_db');
    }
    else {
      drupal_set_message(t("Failed to delete external database."));
    }
  }
}

/**
 * Ajax callback for the tripal_chado_db_form
 *
 */
function tripal_chado_db_edit_form_ajax($form, $form_state) {

  $elements = [];

  // add in the form fields and the buttons
  if (array_key_exists('dbid', $form_state['values'])) {
    $elements['fields'] = $form['fields'];
    $elements['update'] = $form['update'];
    $elements['delete'] = $form['delete'];
  }

  // add back in the db-edit-div that is used for the next round of AJAX
  $elements['fields']['#prefix'] = '<div id="db-edit-div">';
  $elements['fields']['#suffix'] = '</div">';

  // reset the values for the fields to the defaults
  $elements['fields']['name']['#value'] = $elements['fields']['name']['#default_value'];
  $elements['fields']['description']['#value'] = $elements['fields']['description']['#default_value'];
  $elements['fields']['url']['#value'] = $elements['fields']['url']['#default_value'];
  $elements['fields']['urlprefix']['#value'] = $elements['fields']['urlprefix']['#default_value'];

  //drupal_set_message('<pre>' . print_r($elements, TRUE) . '</pre>', "status");

  return $elements;
}
