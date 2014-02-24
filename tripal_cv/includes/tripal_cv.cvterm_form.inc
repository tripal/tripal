<?php
/**
 * @file
 * Provides a form for creating & editing chado controlled vocabularies
 */

/**
 * Form for editing cvterms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_edit_form($form, &$form_state) {

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
      $result = db_select('chado.cvterm','c')
        ->fields('c', array('name'))
        ->condition('c.cvterm_id',$cvterm_id)
        ->execute();
      $cvterm_name = $result->fetchObject()->name;
      $step = 1;
    }
  }

  // get a list of CVs
  $cvs = array();
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);
  $cvs[] = 'Select a vocabulary';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }
  $form['cv_id'] = array(
    '#title' => t('Controlled Vocabulary (Ontology) Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#required' => TRUE,
    '#default_value' => $cv_id,
    '#ajax' => array(
       'callback'    => 'tripal_cv_cvterm_edit_form_ajax',
       'wrapper' => 'cvterm-edit-div',
       'event'   => 'change',
       'method'  => 'replace',
       'event'    => 'change',
    ),
  );

  if ($cv_id and $step == 0) {

    $form['name']= array(
      '#type'          => 'textfield',
      '#title'         => t("Term Name"),
      '#default_value' => $cvterm,
      '#required'      => TRUE,
      '#autocomplete_path' => "admin/tripal/tripal_cv/cvterm/auto_name/$cv_id",
      '#description'   => t('Enter the term to edit.')
    );
    $form['continue']= array(
      '#type'          => 'submit',
      '#value'         => 'continue',
    );
  }
  elseif ($step == 1) {

    tripal_cv_add_cvterm_form_fields($form, $form_state, $cv_id, $cvterm_name);

    // when editing there are certain fields the user should not change for a term
    // let's mark those as disabled
    $form['cv_id']['#disabled'] = TRUE;
    $form['fields']['db_id']['#disabled'] = TRUE;
    $form['fields']['accession']['#disabled'] = TRUE;

    // add in the div for replacing the fields if needed
    $form['fields']['#prefix'] = '<div id="cvterm-edit-div">';
    $form['fields']['#suffix'] = '</div>';

    // add in the cvterm id
    $form['fields']['cvterm_id'] = array(
      '#type' => 'hidden',
      '#value' =>  $cvterm_id,
    );

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

  if ($step == 0) {
    // if we don't have a cv_id then this is the first time the form has
    // benn loaded and we need to create the div where ajax replacement elements get stored
    $form['div_replace'] = array(
      '#type' => 'item',
      '#prefix' => '<div id="cvterm-edit-div">',
      '#suffix' => '</div>',
    );
  }
  return $form;
}

/**
 * Form for adding cvterms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_add_form($form, &$form_state) {
  $cv_id = 0;
  if (array_key_exists('values', $form_state)) {
    $cv_id = $form_state['values']['cv_id'];
  }
  elseif (isset($form_state['build_info']['args'][0])) {
    $cv_id = $form_state['build_info']['args'][0];
  }

  // get a list of CVs
  $cvs = array();
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);
  $cvs[] = 'Select a vocabulary';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }
  $form['cv_id'] = array(
    '#title' => t('Controlled Vocabulary (Ontology) Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#required' => TRUE,
    '#default_value' => $cv_id,
  );
  tripal_cv_add_cvterm_form_fields($form, $form_state);

  $form['add'] = array(
    '#type'         => 'submit',
    '#value'        => t('Add Term'),
  );

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

  // get default values
  if ($cvterm_name) {
    $values = array('cv_id' => $cv_id, 'name' => $cvterm_name);
    $cvterm = chado_generate_var('cvterm', $values);
    $name = $cvterm->name;
    $definition = $cvterm->definition;
    $is_relationship = $cvterm->is_relationshiptype;
    $is_obsolete = $cvterm->is_obsolete;
    $db_id = $cvterm->dbxref_id->db_id->db_id;
    $accession = $cvterm->dbxref_id->accession;
  }

  // add a fieldset for the Drupal Schema API
  $form['fields'] = array(
    '#type' => 'fieldset',
    '#title' => 'Term Details',
    '#collapsible' => 0,
  );


  $form['fields']['name']= array(
    '#type'          => 'textfield',
    '#title'         => t("Term Name"),
    '#default_value' => $name,
    '#required'      => TRUE,
  );

  $form['fields']['definition']= array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#description'   => t('Please enter a description for this term'),
    '#default_value' => $definition,
  );

  $form['fields']['is_relationship'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('This term describes a relationship?'),
    '#default_value' => $is_relationship,
  );

  $form['fields']['is_obsolete'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('This term is obsolete?'),
    '#default_value' => $is_obsolete,
  );

  $values = array();
  $columns = array('db_id', 'name');
  $options = array('order_by' => array('name' => 'ASC'));
  $results = chado_select_record('db', $columns, $values, $options);
  $dbs = array();
  $dbs[] = '';
  foreach ($results as $db) {
    $dbs[$db->db_id] = $db->name;
  }
  $form['fields']['db_id'] = array(
    '#type'         => 'select',
    '#title'         => t('Database'),
    '#description'   => t('All terms must be assocated with a database. If the term you are adding is custom for this database, consider creating a database specific for your site and use that anytime you would like to add terms.'),
    '#options'      => $dbs,
    '#default_value' => $db_id,
    '#required' => TRUE,
  );

  $form['fields']['accession']= array(
    '#type'          => 'textfield',
    '#title'         => t("Accession"),
    '#description'   => t('If this term has an existing accession (unique identifier) in the database
       please enter that here.  If the accession is numeric with a database prefix (e.g. GO:003023), please
       enter just the numeric value.  The database prefix will be appended whenever the term is displayed.
       If the accession is not numeric then enter it as is.  If no value is provied, the term name
       provided above will be used as the accession.'),
    '#required'      => TRUE,
    '#default_value' => $accession,
  );
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
    $values = array('name' => $name, 'cv_id' => $cv_id);
    $results = chado_select_record('cvterm', array('cvterm_id'), $values);
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

  $values = array('cv_id' => $cv_id);
  $results = chado_select_record('cv', array('name'), $values);
  if (!$results or count($results) == 0) {
    form_set_error('cv_id', 'The controlled vocabulary does not exist');
  }

  // make sure the DB exists
  $values = array('db_id' => $db_id);
  $results = chado_select_record('db', array('name'), $values);
  if (!$results or count($results) == 0) {
    form_set_error('db_id', 'The database name does not exist');
  }

  // make sure the cv term name is unique for this vocabulary
  $values = array('name' => $name, 'cv_id' => $cv_id);
  $results = chado_select_record('cvterm', array('cvterm_id'), $values);
  if (count($results) > 0) {
    form_set_error('name', 'The term name must be unique for this vocabulary. Another term with this name already exists.');
  }

  // make sure this accession is unique for the database
  $values = array('accession' => $accession, 'db_id' => $db_id);
  $results = chado_select_record('dbxref', array('dbxref_id'), $values);
  if (count($results) > 0 ) {
    form_set_error('accession', 'The accession is not uniuqe for this vocabulary\'s database.');
  }

}

/**
 * Edits existing controlled vocabulary terms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_edit_form_submit($form, &$form_state) {

  $cv_id = array_key_exists('cv_id', $form_state['values']) ? $form_state['values']['cv_id'] : '';
  $name = array_key_exists('name', $form_state['values']) ? $form_state['values']['name'] : '';
  $definition = array_key_exists('definition', $form_state['values']) ? $form_state['values']['definition'] : '';
  $is_relationship = array_key_exists('is_relationship', $form_state['values']) ? $form_state['values']['is_relationship'] : '';
  $is_obsolete = array_key_exists('is_obsolete', $form_state['values']) ? $form_state['values']['is_obsolete'] : '';
  $cvterm_id = array_key_exists('cvterm_id', $form_state['values']) ? $form_state['values']['cvterm_id'] : '';

  $db_id = array_key_exists('db_id', $form_state['values']) ? $form_state['values']['db_id'] : '';
  $accession = array_key_exists('accession', $form_state['values']) ? $form_state['values']['accession'] : '';

  $op    = array_key_exists('op', $form_state['values'])         ? trim($form_state['values']['op']) : '';


  $step = $form_state['storage']['step'];

  switch ($step) {
    case 0:  // a cvterm name has been selected
      $cv_id = array_key_exists('cv_id', $form_state['values']) ? trim($form_state['values']['cv_id']) : '';
      $name  = array_key_exists('name', $form_state['values'])  ? trim($form_state['values']['name'])  : '';

      // get the original cvterm_id
      $values = array('name' => $name, 'cv_id' => $cv_id);
      $results = chado_select_record('cvterm', array('cvterm_id'), $values);
      $cvterm = $results[0];

      $form_state['storage']['cv_id'] = $cv_id;
      $form_state['storage']['name'] = $name;
      $form_state['storage']['step'] = 1;
      $form_state['storage']['cvterm_id'] = $cvterm->cvterm_id;
      $form_state['rebuild'] = TRUE;
      break;

    case 1:  // update/delete button has been clicked

      if ($op == 'Update') {
        // get the cv
        $values = array('cv_id' => $cv_id);
        $results = chado_select_record('cv', array('name'), $values);
        $cv = $results[0];

        // get the db
        $values = array('db_id' => $db_id);
        $results = chado_select_record('db', array('name'), $values);
        $db = $results[0];

        // now add the term
        $term = array(
          'name' => $name,
          'namespace' => $cv->name,
          'id' => $accession,
          'def' => $definition,
          'is_obsolete' => $is_obsolete,
        );

        $cvterm = tripal_cv_add_cvterm($term, $cv->name, $is_relationship, TRUE, $db->name);
        if ($cvterm) {
          drupal_set_message('Term updated successfully.');
        }
        else {
          drupal_set_message('Could not add term. Check Drupal recent logs for error messages.', 'error');
        }
      }
      if ($op == 'Delete') {
        $values = array('cvterm_id' => $cvterm_id);
        $success = chado_delete_record('cvterm', $values);
        if ($success) {
          drupal_set_message('Term deleted successfully.');
        }
        else {
          drupal_set_message('Could not delete term term. Check Drupal recent logs for error messages.', 'error');
        }
      }
      break;
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
  $values = array('db_id' => $db_id);
  $results = chado_select_record('db', array('name'), $values);
  $db = $results[0];

  // get the cv
  $values = array('cv_id' => $cv_id);
  $results = chado_select_record('cv', array('name'), $values);
  $cv = $results[0];

  // now add the term
  $term = array(
    'name' => $name,
    'namespace' => $cv->name,
    'id' => $accession,
    'def' => $definition,
    'is_obsolete' => $is_obsolete,
  );

  $cvterm = tripal_cv_add_cvterm($term, $cv->name, $is_relationship, TRUE, $db->name);
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

  $elements = array();

  $elements['name'] = $form['name'];
  $elements['continue'] = $form['continue'];

  // add back in the cv-edit-div that is used for the next round of AJAX
  $elements['name']['#prefix'] =  '<div id="cvterm-edit-div">';
  $elements['name']['#suffix'] =  '</div">';


  return $elements;
}
