<?php

/**
 * Loads an OBO File using the new TripalImporter. Expected to be run by a
 * Tripal Job.
 */
function tripal_cv_load_obo($obo_id, $job = NULL) {

  module_load_include('inc', 'tripal_chado', 'includes/TripalImporter/OBOImporter');
  $obo_importer = new OBOImporter();
  $obo_importer->create(['obo_id' => $obo_id]);
  if ($job) {
    $obo_importer->setJob($job);
  }
  $obo_importer->run();

}

/**
 * Provide landing page to the new admin pages
 *
 * @ingroup tripal_cv
 */
function tripal_cv_admin_cv_listing() {
  $output = '';

  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Chado Vocabularies', 'admin/tripal/loaders/chado_vocabs');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $cvs_view = views_embed_view('tripal_cv_admin_cvs', 'default');
  $cvterms_view = views_embed_view('tripal_cv_admin_cvterms', 'default');
  if (isset($cvs_view) && isset($cvterms_view)) {
    $output .= $cvs_view;
  }
  else {
    $output .= '<p>The Tripal Controlled Vocabulary module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    if (!isset($cvs_view)) {
      $output .= '<li>' . l('Tripal Vocabularies', 'admin/tripal/vocab/views/cvs/enable') . '</li>';
    }
    if (!isset($cvterm_view)) {
      $output .= '<li>' . l('Tripal Vocabulary Terms', 'admin/tripal/vocab/views/cvterms/enable') . '</li>';
    }
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Provides the actual "Select CV" form on the Update/Delete Controlled
 *   Vocabulary page. This form also triggers the edit javascript
 *
 * @todo Modify this form to use Drupal AJAX
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_edit_form($form, &$form_state, $cv_id = NULL) {

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Chado Vocabularies', 'admin/tripal/loaders/chado_vocabs');
  $breadcrumb[] = l('Manage Chado CVs', 'admin/tripal/loaders/chado_vocabs/chado_cvs');
  drupal_set_breadcrumb($breadcrumb);

  $default_name = '';
  $default_desc = '';

  if ($cv_id) {
    $values = ['cv_id' => $cv_id];
    $result = chado_select_record('cv', ['*'], $values);
    $cv = $result[0];
    $default_name = $cv->name;
    $default_desc = $cv->definition;
  }

  $form['cv_id'] = [
    '#type' => 'value',
    '#value' => $cv_id,
  ];

  $form['name'] = [
    '#type' => 'textfield',
    '#title' => t("Controlled Vocabulary name"),
    '#description' => t('Please enter the name for this vocabulary.'),
    '#required' => TRUE,
    '#default_value' => $default_name,
    '#maxlength' => 255,
  ];

  $form['definition'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Please enter a definition for this vocabulary'),
    '#default_value' => $default_desc,
  ];

  $form['update'] = [
    '#type' => 'submit',
    '#value' => t('Update'),
  ];
  $form['delete'] = [
    '#type' => 'markup',
    '#markup' => l('delete', 'admin/tripal/loaders/chado_vocabs/chado_cv/delete/' . $cv_id),
  ];

  return $form;
}

/**
 * Validation function for tripal_cv_cv_edit_form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_edit_form_validate($form, &$form_state) {
  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $desc = array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';
  $cv_id = array_key_exists('cv_id', $form_state['values']) ? trim($form_state['values']['cv_id']) : '';

  // make sure the cv name is unique
  $values = ['name' => $name];
  $results = chado_select_record('cv', ['cv_id'], $values);
  if (count($results) > 0 and $results[0]->cv_id != $cv_id) {
    form_set_error('name', 'The vocabulary name must be unique');
  }
}

/**
 * Submit cv edit form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_edit_form_submit($form, &$form_state) {
  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $desc = array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';
  $cv_id = array_key_exists('cv_id', $form_state['values']) ? trim($form_state['values']['cv_id']) : '';
  $op = array_key_exists('op', $form_state['values']) ? trim($form_state['values']['op']) : '';

  $values = [
    'name' => $name,
    'definition' => $desc,
  ];
  if (strcmp($op, 'Update') == 0) {
    $match = ['cv_id' => $cv_id];
    $success = chado_update_record('cv', $match, $values);

    // Clear the cached terms
    cache_clear_all('tripal_chado:term:*', 'cache', TRUE);

    if ($success) {
      drupal_set_message(t("Controlled vocabulary updated"));
      drupal_goto('admin/tripal/loaders/chado_vocabs/chado_cvs');
    }
    else {
      drupal_set_message(t("Failed to update controlled vocabulary."));
    }
  }
}

/**
 * Form to add controlled vocabularies
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_add_form($form, &$form_state) {

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Chado Vocabularies', 'admin/tripal/loaders/chado_vocabs');
  $breadcrumb[] = l('Manage Chado CVs', 'admin/tripal/loaders/chado_vocabs/chado_cvs');
  drupal_set_breadcrumb($breadcrumb);

  $default_name = '';
  $default_desc = '';

  // add a fieldset for the Drupal Schema API
  $form = [
    '#type' => 'fieldset',
    '#title' => 'Controlled Vocabulary Details',
    '#collapsible' => 0,
  ];

  $form['name'] = [
    '#type' => 'textfield',
    '#title' => t("Controlled Vocabulary name"),
    '#description' => t('Please enter the name for this vocabulary.'),
    '#required' => TRUE,
    '#default_value' => $default_name,
    '#maxlength' => 255,
  ];

  $form['definition'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Please enter a definition for this vocabulary'),
    '#default_value' => $default_desc,
  ];

  $form['add'] = [
    '#type' => 'submit',
    '#value' => t('Add'),
  ];
  return $form;
}

/**
 * Validation function for tripal_cv_cv_add_form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_add_form_validate($form, &$form_state) {
  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $desc = array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';

  // make sure the cv name is unique
  $values = ['name' => $name];
  $results = chado_select_record('cv', ['cv_id'], $values);
  if (count($results) > 0) {
    form_set_error('name', 'The vocabulary name must be unique');
  }
}

/**
 * Submit cv add form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_add_form_submit($form, &$form_state) {

  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $desc = array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';

  $values = [
    'name' => $name,
    'definition' => $desc,
  ];
  $success = chado_insert_record('cv', $values);
  if ($success) {
    drupal_set_message(t("Controlled vocabulary added"));
    drupal_goto('admin/tripal/loaders/chado_vocabs/chado_cvs');
  }
  else {
    drupal_set_message(t("Failed to add controlled vocabulary."));
  }
}


/**
 * Ajax callback for the tripal_cv_form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_edit_form_ajax($form, $form_state) {

  $elements = [];

  // add in the form fields and the buttons
  if (array_key_exists('cv_id', $form_state['values'])) {
    $elements['fields'] = $form;
    $elements['update'] = $form['update'];
    $elements['delete'] = $form['delete'];
  }

  // add back in the cv-edit-div that is used for the next round of AJAX
  $elements['fields']['#prefix'] = '<div id="cv-edit-div">';
  $elements['fields']['#suffix'] = '</div">';

  // reset the values for the fields to the defaults
  $elements['fields']['name']['#value'] = $elements['fields']['name']['#default_value'];
  $elements['fields']['definition']['#value'] = $elements['fields']['definition']['#default_value'];

  //drupal_set_message('<pre>' . print_r($elements, TRUE) . '</pre>', "status");

  return $elements;
}

/**
 * Form for editing cvterms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_edit_form($form, &$form_state) {
  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Chado Vocabularies', 'admin/tripal/loaders/chado_vocabs');
  $breadcrumb[] = l('Controlled Vocabulary Terms', 'admin/tripal/loaders/chado_vocabs/chado_cvterms');
  drupal_set_breadcrumb($breadcrumb);

  $step = 0;
  if (empty($form_state['storage']['step'])) {
    $form_state['storage']['step'] = 0;
  }
  else {
    $step = $form_state['storage']['step'];
  }

  $cv_id = 0;
  if ($step == 1) {
    $cv_id = $form_state['storage']['cv_id'];
    $cvterm_name = $form_state['storage']['name'];
    $cvterm_id = $form_state['storage']['cvterm_id'];
  }
  // get the cv if form was submitted via AJAX
  $cvterm = '';
  if (array_key_exists('values', $form_state)) {
    $cv_id = $form_state['values']['cv_id'];
    if (array_key_exists('cvterm', $form_state['values'])) {
      $cvterm = $form_state['values']['cvterm'];
    }
  }
  elseif (isset($form_state['build_info']['args'][0])) {
    $cv_id = $form_state['build_info']['args'][0];
    $cvterm_id = $form_state['build_info']['args'][1];
    if ($form_state['build_info']['args'][1]) {
      $cvterm_name = chado_query('SELECT name FROM {cvterm} WHERE cvterm_id = :id',
        [':id' => $cvterm_id])->fetchField();
      $step = 1;
    }
  }

  // get a list of CVs
  $cvs = [];
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);
  $cvs[] = 'Select a vocabulary';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }
  $form['cv_id'] = [
    '#title' => t('Controlled Vocabulary Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#required' => TRUE,
    '#default_value' => $cv_id,
    '#ajax' => [
      'callback' => 'tripal_cv_cvterm_edit_form_ajax',
      'wrapper' => 'cvterm-edit-div',
      'event' => 'change',
      'method' => 'replace',
      'event' => 'change',
    ],
  ];

  if ($cv_id and $step == 0) {

    $form['name'] = [
      '#type' => 'textfield',
      '#title' => t("Term Name"),
      '#default_value' => $cvterm,
      '#required' => TRUE,
      '#autocomplete_path' => "admin/tripal/tripal_cv/cvterm/auto_name/$cv_id",
      '#description' => t('Enter the term to edit.'),
    ];
    $form['continue'] = [
      '#type' => 'submit',
      '#value' => 'continue',
    ];
  }
  elseif ($step == 1) {

    tripal_cv_add_cvterm_form_fields($form, $form_state, $cv_id, $cvterm_name);

    // when editing there are certain fields the user should not change for a term
    // let's mark those as disabled
    $form['cv_id']['#disabled'] = TRUE;
    $form['db_id']['#disabled'] = TRUE;
    $form['accession']['#disabled'] = TRUE;

    // add in the div for replacing the fields if needed
    $form['#prefix'] = '<div id="cvterm-edit-div">';
    $form['#suffix'] = '</div>';

    // add in the cvterm id
    $form['cvterm_id'] = [
      '#type' => 'hidden',
      '#value' => $cvterm_id,
    ];

    $form['update'] = [
      '#type' => 'submit',
      '#value' => t('Update'),
    ];
    $form['delete'] = [
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#attributes' => ['onclick' => 'if(!confirm("Really Delete?")){return false;}'],
    ];
  }

  if ($step == 0) {
    // if we don't have a cv_id then this is the first time the form has
    // benn loaded and we need to create the div where ajax replacement elements get stored
    $form['div_replace'] = [
      '#type' => 'item',
      '#prefix' => '<div id="cvterm-edit-div">',
      '#suffix' => '</div>',
    ];
  }
  return $form;
}

/**
 * Form for adding cvterms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_add_form($form, &$form_state) {

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Chado Vocabularies', 'admin/tripal/loaders/chado_vocabs');
  $breadcrumb[] = l('Controlled Vocabulary Terms', 'admin/tripal/loaders/chado_vocabs/chado_cvterms');
  drupal_set_breadcrumb($breadcrumb);

  $cv_id = 0;
  if (array_key_exists('values', $form_state)) {
    $cv_id = $form_state['values']['cv_id'];
  }
  elseif (isset($form_state['build_info']['args'][0])) {
    $cv_id = $form_state['build_info']['args'][0];
  }

  // get a list of CVs
  $cvs = [];
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);
  $cvs[] = 'Select a vocabulary';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }
  $form['cv_id'] = [
    '#title' => t('Controlled Vocabulary (Ontology) Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#required' => TRUE,
    '#default_value' => $cv_id,
  ];
  tripal_cv_add_cvterm_form_fields($form, $form_state);

  $form['add'] = [
    '#type' => 'submit',
    '#value' => t('Add Term'),
  ];

  return $form;
}

/**
 * Form fields in common between add/edit forms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_add_cvterm_form_fields(&$form, $form_state, $cv_id = 0, $cvterm_name = '') {

  $name = '';
  $definition = '';
  $is_relationship = '';
  $is_obsolete = '';
  $db_id = '';
  $accession = '';
  $cvterm = NULL;

  // get default values
  if ($cvterm_name) {
    $values = ['cv_id' => $cv_id, 'name' => $cvterm_name];
    $cvterm = chado_generate_var('cvterm', $values);
    $name = $cvterm->name;
    $definition = $cvterm->definition;
    $is_relationship = $cvterm->is_relationshiptype;
    $is_obsolete = $cvterm->is_obsolete;
    $db_id = $cvterm->dbxref_id->db_id->db_id;
    $accession = $cvterm->dbxref_id->accession;
  }

  $form['name'] = [
    '#type' => 'textfield',
    '#title' => t("Term Name"),
    '#default_value' => $name,
    '#description' => t('The term must be unique within the database selected below.'),
    '#required' => TRUE,
  ];

  $form['internal_id'] = [
    '#type' => 'item',
    '#title' => t("Internal ID"),
    '#markup' => $cvterm ? $cvterm->cvterm_id : '',
  ];

  $form['definition'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Please enter a description for this term'),
    '#default_value' => $definition,
  ];

  $form['is_relationship'] = [
    '#type' => 'checkbox',
    '#title' => t('This term describes a relationship?'),
    '#default_value' => $is_relationship,
  ];

  $form['is_obsolete'] = [
    '#type' => 'checkbox',
    '#title' => t('This term is obsolete?'),
    '#default_value' => $is_obsolete,
  ];

  $values = [];
  $columns = ['db_id', 'name'];
  $options = ['order_by' => ['name' => 'ASC']];
  $results = chado_select_record('db', $columns, $values, $options);
  $dbs = [];
  $dbs[] = '';
  foreach ($results as $db) {
    $dbs[$db->db_id] = $db->name;
  }
  $form['db_id'] = [
    '#type' => 'select',
    '#title' => t('Database'),
    '#description' => t('All terms must be assocated with a database. If there is no database for this term (e.g. it is a custom term specific to this site) then select the database \'null\' or consider creating a database specific for your site and use that anytime you would like to add terms.'),
    '#options' => $dbs,
    '#default_value' => $db_id,
    '#required' => TRUE,
  ];

  $form['accession'] = [
    '#type' => 'textfield',
    '#title' => t("Accession"),
    '#description' => t('If this term has an existing accession (unique identifier) in the database
       please enter that here.  If the accession is numeric with a database prefix (e.g. GO:003023), please
       enter just the numeric value.  The database prefix will be appended whenever the term is displayed.
       If you do not have a numeric value consider entering the term name as the accession.'),
    '#required' => TRUE,
    '#default_value' => $accession,
  ];
}

/**
 * Validate cvterm edit form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_edit_form_validate($form, &$form_state) {
  $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : '';
  $db_id = array_key_exists('db_id', $form_state['values']) ? $form_state['values']['db_id'] : '';
  $name = array_key_exists('name', $form_state['values']) ? $form_state['values']['name'] : '';
  $cvterm_id = array_key_exists('cvterm_id', $form_state['values']) ? $form_state['values']['cvterm_id'] : '';
  $accession = array_key_exists('accession', $form_state['values']) ? $form_state['values']['accession'] : '';

  $step = $form_state['storage']['step'];

  // make sure the cv term name is unique for this vocabulary
  if ($step == 1) {
    $values = ['name' => $name, 'cv_id' => $cv_id];
    $results = chado_select_record('cvterm', ['cvterm_id'], $values);
    foreach ($results as $r) {
      if ($r->cvterm_id != $cvterm_id) {
        form_set_error('name', 'The term name must be unique for this vocabulary. Another term with this name already exists.');
      }
    }
  }
}

/**
 * Validate cv add form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_add_form_validate($form, &$form_state) {
  $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : '';
  $db_id = array_key_exists('db_id', $form_state['values']) ? $form_state['values']['db_id'] : '';
  $name = array_key_exists('name', $form_state['values']) ? $form_state['values']['name'] : '';
  $accession = array_key_exists('accession', $form_state['values']) ? $form_state['values']['accession'] : '';

  $values = ['cv_id' => $cv_id];
  $results = chado_select_record('cv', ['name'], $values);
  if (!$results or count($results) == 0) {
    form_set_error('cv_id', 'The controlled vocabulary does not exist');
  }

  // make sure the DB exists
  $values = ['db_id' => $db_id];
  $results = chado_select_record('db', ['name'], $values);
  if (!$results or count($results) == 0) {
    form_set_error('db_id', 'The database name does not exist');
  }

  // make sure the cv term name is unique for this vocabulary
  $values = ['name' => $name, 'cv_id' => $cv_id];
  $results = chado_select_record('cvterm', ['cvterm_id'], $values);
  if (count($results) > 0) {
    form_set_error('name', 'The term name must be unique for this vocabulary. Another term with this name already exists.');
  }

  // make sure this accession is unique for the database
  $values = ['accession' => $accession, 'db_id' => $db_id];
  $results = chado_select_record('dbxref', ['dbxref_id'], $values);
  if (count($results) > 0) {
    form_set_error('accession', 'The accession is not uniuqe for this vocabulary\'s database.');
  }

}

/**
 * Edits existing controlled vocabulary terms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_edit_form_submit($form, &$form_state) {

  $cv_id = array_key_exists('cv_id', $form_state['values']) ? trim($form_state['values']['cv_id']) : '';
  $name = array_key_exists('name', $form_state['values']) ? trim($form_state['values']['name']) : '';
  $definition = array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';
  $is_relationship = array_key_exists('is_relationship', $form_state['values']) ? trim($form_state['values']['is_relationship']) : '';
  $is_obsolete = array_key_exists('is_obsolete', $form_state['values']) ? trim($form_state['values']['is_obsolete']) : '';
  $cvterm_id = array_key_exists('cvterm_id', $form_state['values']) ? trim($form_state['values']['cvterm_id']) : '';
  $db_id = array_key_exists('db_id', $form_state['values']) ? trim($form_state['values']['db_id']) : '';
  $accession = array_key_exists('accession', $form_state['values']) ? trim($form_state['values']['accession']) : '';
  $op = array_key_exists('op', $form_state['values']) ? trim($form_state['values']['op']) : '';

  if ($op == 'Update') {
    // get the original cvterm_id
    $values = ['name' => $name, 'cv_id' => $cv_id];
    $results = chado_select_record('cvterm', ['cvterm_id'], $values);
    $cvterm = $results[0];

    // get the cv
    $values = ['cv_id' => $cv_id];
    $results = chado_select_record('cv', ['name'], $values);
    $cv = $results[0];

    // get the db
    $values = ['db_id' => $db_id];
    $results = chado_select_record('db', ['name'], $values);
    $db = $results[0];

    // now add the term
    $term = [
      'name' => $name,
      'namespace' => $cv->name,
      'id' => $accession,
      'definition' => $definition,
      'is_obsolete' => $is_obsolete,
      'cv_name' => $cv->name,
      'is_relationship' => $is_relationship,
      'db_name' => $db->name,
    ];

    $cvterm = chado_insert_cvterm($term, ['update_existing' => TRUE]);
    if ($cvterm) {
      drupal_set_message('Term updated successfully.');
      drupal_goto('admin/tripal/loaders/chado_vocabs/chado_cvterms');
    }
    else {
      drupal_set_message('Could not add term. Check Drupal recent logs for error messages.', 'error');
    }
  }
  if ($op == 'Delete') {
    $values = ['cvterm_id' => $cvterm_id];
    $success = chado_delete_record('cvterm', $values);
    if ($success) {
      drupal_set_message('Term deleted successfully.');
    }
    else {
      drupal_set_message('Could not delete term term. Check Drupal recent logs for error messages.', 'error');
    }
  }
}

/**
 * Adds new terms to an existing cv
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_add_form_submit($form, &$form_state) {
  $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : '';
  $name = array_key_exists('name', $form_state['values']) ? $form_state['values']['name'] : '';
  $definition = array_key_exists('definition', $form_state['values']) ? $form_state['values']['definition'] : '';
  $is_relationship = array_key_exists('is_relationship', $form_state['values']) ? $form_state['values']['is_relationship'] : '';
  $is_obsolete = array_key_exists('is_obsolete', $form_state['values']) ? $form_state['values']['is_obsolete'] : '';

  $db_id = array_key_exists('db_id', $form_state['values']) ? $form_state['values']['db_id'] : '';
  $accession = array_key_exists('accession', $form_state['values']) ? $form_state['values']['accession'] : '';

  // get the database
  $values = ['db_id' => $db_id];
  $results = chado_select_record('db', ['name'], $values);
  $db = $results[0];

  // get the cv
  $values = ['cv_id' => $cv_id];
  $results = chado_select_record('cv', ['name'], $values);
  $cv = $results[0];

  // now add the term
  $term = [
    'name' => $name,
    'namespace' => $cv->name,
    'id' => $accession,
    'definition' => $definition,
    'is_obsolete' => $is_obsolete,
    'cv_name' => $cv->name,
    'is_relationship' => $is_relationship,
    'db_name' => $db->name,
  ];

  $cvterm = chado_insert_cvterm($term, ['update_existing' => TRUE]);
  if ($cvterm) {
    drupal_set_message('Term added successfully.');
  }
  else {
    drupal_set_message('Could not add term. Check Drupal recent logs for error messages.', 'error');
  }
}

/**
 * Ajax callback for the tripal_cv_form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_edit_form_ajax($form, $form_state) {

  $elements = [];

  $elements['name'] = $form['name'];
  $elements['continue'] = $form['continue'];

  // add back in the cv-edit-div that is used for the next round of AJAX
  $elements['name']['#prefix'] = '<div id="cvterm-edit-div">';
  $elements['name']['#suffix'] = '</div">';


  return $elements;
}

/**
 * Form for re-doing the cvterm path
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvtermpath_form() {

  // get a list of db from chado for user to choose
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);

  $cvs = [];
  $cvs[] = '';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }

  $form['cvid'] = [
    '#title' => t('Controlled Vocabulary/Ontology Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#description' => t('The Chado cvtermpath is a database table that provides lineage for ontology terms
      and is useful for quickly finding any ancestor parent of a term.  This table must be populated for each
      ontology.  Select a controlled vocabulary for which you would like to upate the cvtermpath.'),
  ];

  $form['description'] = [
    '#type' => 'item',
    '#value' => t("Submit a job to update chado cvtermpath table."),
    '#weight' => 1,
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Update cvtermpath'),
    '#weight' => 2,
  ];

  return $form;
}

/**
 * Cvterm path form submit
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvtermpath_form_submit($form, &$form_state) {
  global $user;

  $cvid = $form_state['values']['cvid'];

  // first get the controlled vocabulary name:
  $sql = "SELECT * FROM {cv} WHERE cv_id = :cv_id";
  $cv = chado_query($sql, [':cv_id' => $cvid])->fetchObject();

  // Submit a job to update cvtermpath
  $job_args = [$cvid];
  if ($form_state['values']['op'] == t('Update cvtermpath')) {
    tripal_add_job("Update cvtermpath: $cv->name", 'tripal_cv',
      'chado_update_cvtermpath', $job_args, $user->uid, 10);
  }
}

/**
 * A confirmation form for deleting a controlled vocabulary.
 */
function tripal_cv_cv_delete_form($form, &$form_state, $cv_id) {

  $cv = chado_get_cv(['cv_id' => $cv_id]);

  $form['cv_id'] = [
    '#type' => 'value',
    '#value' => $cv_id,
  ];

  return confirm_form($form,
    t('Confirm removal of the vocabulary: "' . $cv->name . '"? '),
    'admin/tripal/loaders/chado_vocabs/chado_cv/edit/' . $cv_id,
    t('WARNING: removal of a vocabulary will result in removal of all terms, and any associations used for other records in the site that use those terms.')
  );
}

/**
 * Implements the submit hook for tripal_cv_cv_delete_form.
 */
function tripal_cv_cv_delete_form_submit($form, &$form_state) {
  $cv_id = $form_state['values']['cv_id'];

  try {
    $match = ['cv_id' => $cv_id];
    $success = chado_delete_record('cv', $match);
    drupal_set_message(t("Controlled vocabulary deleted"));
    drupal_goto('admin/tripal/loaders/chado_vocabs/chado_cv');
  } catch (Exception $e) {
    drupal_set_message(t("Failed to delete controlled vocabulary."), 'error');
  }
}
