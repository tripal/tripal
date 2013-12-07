<?php
/**
 * Purpose: Provides the form for Updating and Deleteing existing
 *   chado controlled vocabularies (See chado cv table)
 *
 * @ingroup tripal_cv
 */
function tripal_cv_edit_page() {
  $output .= drupal_get_form('tripal_cv_select_form');
  $output .= '<div id="cv-edit-div">Please select a vocabulary above to view or edit</div>';

  return $output;
}

/**
 * Purpose: Provides the actual "Select CV" form on the Update/Delete Controlled
 *   Vocabulary page. This form also triggers the edit javascript
 * @todo Modify this form to use Drupal AJAX
 *
 * @ingroup tripal_cv
 */
function tripal_cv_select_form() {

  // get a list of db from chado for user to choose
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);

  $cvs = array();
  $cvs[] = '';
  while ($cv = db_fetch_object($results)) {
    $cvs[$cv->cv_id] = $cv->name;
  }

  $form['cvid'] = array(
    '#title' => t('Controlled Vocabulary/Ontology Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#ahah' => array(
      'path' => 'admin/tripal/tripal_cv/cv/edit/js',
      'wrapper' => 'cv-edit-div',
      'effect' => 'fade',
      'event' => 'change',
      'method' => 'replace',
    ),
  );

  return $form;
}

/**
 * Purpose: The edit controlled vocabulary javascript
 *
 * @ingroup tripal_cv
 */
function tripal_ajax_cv_edit() {

  // get the database id, build the form and then return the JSON object
  $cvid = filter_xss($_POST['cvid']);
  $form = drupal_get_form('tripal_cv_edit_form', $cvid);
  drupal_json(array('status' => TRUE, 'data' => $form));

}

/**
 * Purpose: Provides a form to allow updating/deleteing of controlled vocabularies
 *
 * @ingroup tripal_cv
 */
function tripal_cv_edit_form(&$form_state = NULL, $cvid = NULL) {

  $sql = "SELECT * FROM {cv} WHERE cv_id = :cv_id ";
  $cv = chado_query($sql, array(':cv_id' => $cvid));
  $cv = $cv->fetch_object();

  // set the default values.  If there is a value set in the
  // form_state then let's use that, otherwise, we'll pull
  // the values from the database
  $default_db = $form_state['values']['name'];
  $default_desc = $form_state['values']['description'];
  $default_url = $form_state['values']['url'];
  $default_urlprefix = $form_state['values']['urlprefix'];
  if (!$default_db) {
    $default_cv = $cv->name;
  }
  if (!$default_desc) {
    $default_desc = $cv->definition;
  }

  $form['cvid'] = array(
    '#type' => 'hidden',
    '#value' => $cvid
  );

  $form['name']= array(
    '#type'          => 'textfield',
    '#title'         => t("Controlled Vocabulary name"),
    '#description'   => t('Please enter the name for this vocabulary.'),
    '#required'      => FALSE,
    '#default_value' => $default_cv,
    '#weight'        => 1
  );

  $form['definition']= array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#description'   => t('Please enter a description for this vocabulary'),
    '#default_value' => $default_desc,
    '#weight'        => 2
  );

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

  $form['#redirect'] = 'admin/tripal/tripal_cv';


  return $form;
}

/**
 * Purpose: The submit function of the update/delete controlled vocabulary form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_edit_form_submit($form, &$form_state) {

  $name =  $form_state['values']['name'];
  $desc =  $form_state['values']['definition'];
  $cvid =  $form_state['values']['cvid'];
  $op   =  $form_state['values']['op'];

  if (strcmp($op, 'Update') == 0) {
    $sql = "
       UPDATE {cv} SET
         name = :name,
         definition = :description
       WHERE cv_id = :cv_id
    ";
    $db = chado_query($sql, array(':name' => $name, ':description' => $desc, ':cv_id' => $cvid));
    if ($db) {
      drupal_set_message(t("Controlled vocabulary updated"));
    }
    else {
      drupal_set_message(t("Failed to update controlled vocabulary."), 'error');
    }
  }
  if (strcmp($op, 'Delete')==0) {
    $sql = "
       DELETE FROM {cv}
       WHERE cv_id = :cv_id
    ";
    $db = chado_query($sql, array(':cv_id' => $cvid));
    if ($db) {
      drupal_set_message(t("Controlled vocabulary deleted"));
    }
    else {
      drupal_set_message(t("Failed to delete controlled vocabulary."), 'error');
    }
  }

}


/**
 * Purpose: Provides the Add controlled vocabulary form
 *
  * @ingroup tripal_cv
 */
function tripal_cv_add_form(&$form_state = NULL) {

  $form['cvid'] = array(
    '#type' => 'hidden',
    '#value' => $cvid
  );

  $form['name']= array(
    '#type'          => 'textfield',
    '#title'         => t("Controlled Vocabulary name"),
    '#description'   => t('Please enter the name for this vocabulary.  This field will be ignored if an OBO file or URL is provided above'),
    '#required'      => FALSE,
    '#default_value' => $default_cv,
    '#weight'        => 1
  );

  $form['definition']= array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#description'   => t('Please enter a description for this vocabulary'),
    '#default_value' => $default_desc,
    '#weight'        => 2
  );

  $form['add'] = array(
    '#type'         => 'submit',
    '#value'        => t('Add'),
    '#weight'       => 5,
    '#executes_submit_callback' => TRUE,
  );

  $form['#redirect'] = 'admin/tripal/tripal_cv';

  return $form;
}

/**
 * Purpose: The submit function for the add controlled vocabulary form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_add_form_submit($form, &$form_state) {

  $name =  $form_state['values']['name'];
  $desc =  $form_state['values']['definition'];

  $sql = "
    INSERT INTO {cv}
     (name,definition)
    VALUES
     (:name, :description)
  ";
  $db = chado_query($sql, array(':name' => $name, ':description' => $desc));
  if ($db) {
    drupal_set_message(t("Controlled vocabulary added"));
  }
  else {
    drupal_set_message(t("Failed to add controlled vocabulary."), 'error');
  }

}

/**
 * Purpose: Provides the form that allows adding of terms to an existing
 *   controlled vocabulary
 *
  * @ingroup tripal_cv
 */
function tripal_cv_cvterm_form(&$form_state, $action = 'add') {
  tripal_core_ahah_init_form();

  $form = array();

  // get defaults
  $cv_id = $form_state['values']['cv_id'] ? $form_state['values']['cv_id'] : FALSE;
  $name  = $form_state['values']['name'] ? $form_state['values']['name'] : '';

  // if we have a cv_id and a term name then get the rest of the term details
  if ($cv_id and $name) {
     $values = array(
       'cv_id' => $cv_id,
       'name' => $name,
     );
     $results = tripal_core_chado_select('cvterm', array('*'), $values);
     if (!$results or count($results) == 0) {
       // we can't find the cvterm so reset the name to blank
       $name = '';
     }
     else {
       $cvterm = $results[0];
       $definition = $cvterm->definition;
       $is_relationshiptype = $cvterm->is_relationshiptype;
       $is_obsolete = $cvterm->is_obsolete;

       // now get the database
       $values = array('dbxref_id' => $cvterm->dbxref_id);
       $results = tripal_core_chado_select('dbxref', array('*'), $values);
       $dbxref = $results[0];
       $accession = $dbxref->accession;
       $db_id = $dbxref->db_id;
     }
  }

  $values = array();
  $columns = array('cv_id', 'name');
  $options = array('order_by' => array('name' => 'ASC'));
  $results = tripal_core_chado_select('cv', $columns, $values, $options);
  $cvs = array();
  $cvs[] = '';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }

  $form['wrapper-top'] = array(
    '#type' => 'markup',
    '#value' => '<div id="cvterm-form">',
  );

  $form['form_action'] = array(
    '#type' => 'hidden',
    '#value' => $action,
  );

  $form['cv_id'] = array(
    '#title' => t('Controlled Vocabulary (Ontology) Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#required' => TRUE,
    '#default_value' => $cv_id,
    '#ahah' => array(
       'path'    => 'admin/tripal/tripal_cv/cvterm/ahah',
       'wrapper' => 'cvterm-form',
       'event'   => 'change',
       'method'  => 'replace',
    ),
  );

  if ($cv_id) {
    $form['add_cvterm'] = array(
      '#type'           => 'fieldset',
      '#title'          => t('Term Details'),
      '#prefix'         => '<div id="cvterm-add-div">',
      '#suffix'         => '</div>'
    );
    $description = t('Please enter the name for this vocabulary term.');
    if ($action == 'edit') {
      $description = t('Enter the name of the term to edit.  This field will update automatically as you type. Click outside of the box after entering the term.');
    }
    $form['add_cvterm']['name']= array(
      '#type'          => 'textfield',
      '#title'         => t("Term Name"),
      '#description'   => $description,
      '#default_value' => $name,
      '#required'      => TRUE,
    );
    if ($action == 'edit') {
      if ($name) {
        $form['add_cvterm']['name']['#attributes'] = array('readonly' => 'readonly');
        $form['add_cvterm']['name']['#description'] = 'The term name cannot be changed. If the name is incorrect, please create a new term and make this one as obsolete.';
      }
      else {
        $form['add_cvterm']['name']['#autocomplete_path'] = "admin/tripal/tripal_cv/cvterm/auto_name/$cv_id";
        $form['add_cvterm']['name']['#ahah'] = array(
           'path'    => 'admin/tripal/tripal_cv/cvterm/ahah',
           'wrapper' => 'cvterm-form',
           'method'  => 'replace',
        );
      }
    }

    if ($action == 'add' or $name) {


      $form['add_cvterm']['definition']= array(
        '#type'          => 'textarea',
        '#title'         => t('Description'),
        '#description'   => t('Please enter a description for this term'),
        '#default_value' => $definition,
      );

      $form['add_cvterm']['is_relationshiptype'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('This term describes a relationship?'),
        '#default_value' => $is_relationshiptype,
      );

      $form['add_cvterm']['is_obsolete'] = array(
        '#type'          => 'checkbox',
        '#title'         => t('This term is obsolete?'),
        '#default_value' => $is_obsolete,
      );

      $values = array();
      $columns = array('db_id', 'name');
      $options = array('order_by' => array('name' => 'ASC'));
      $results = tripal_core_chado_select('db', $columns, $values, $options);
      $dbs = array();
      $dbs[] = '';
      foreach ($results as $db) {
        $dbs[$db->db_id] = $db->name;
      }
      $form['add_cvterm']['db_id'] = array(
        '#type'         => 'select',
        '#title'         => t('Database'),
        '#description'   => t('All terms must be assocated with an external database.
                            Please select the external database to associate with
                            this term'),
        '#options'      => $dbs,
        '#default_value' => $db_id,
        '#required' => TRUE,
      );
      if ($action == 'edit') {
        // we don't want to allow the user to change the database on an edit.
        $form['add_cvterm']['db_id']['#disabled'] = TRUE;
        $form['add_cvterm']['db_id']['#description'] = 'The database to which this term belongs cannot be changed.';
      }

      $form['add_cvterm']['accession']= array(
        '#type'          => 'textfield',
        '#title'         => t("Accession"),
        '#description'   => t('If this term has an existing accession (unique identifier) in the database
           please enter that here.  If the accession is numeric with a database prefix (e.g. GO:003023), please
           enter just the numeric value.  The database prefix will be appended whenever the term is displayed.
           If the accession is not numeric then enter it as is.  If no value is provied, the term name
           provided above will be used as the accession.'),
        '#required'      => FALSE,
        '#default_value' => $accession,
      );
      if ($action == 'edit') {
        $form['add_cvterm']['accession']['#attributes'] = array('readonly' => 'readonly');
        $form['add_cvterm']['accession']['#description'] = 'Cannot change the term accession.';
      }
      $button_text = 'Add Term';
      if ($action == 'edit') {
        $button_text = 'Update Term';
      }
      $form['add_cvterm']['submit'] = array(
        '#type'  => 'submit',
        '#value' => $button_text,
      );
    } // end if name selected (or action == 'add')
  } //end of if cv selected


  $form['wrapper-bottom'] = array(
    '#type' => 'markup',
    '#value' => '</div>',
  );

  return $form;
}


/**
 * Purpose: Validates the input for adding a cvterm
 *
  * @ingroup tripal_cv
 */
function tripal_cv_cvterm_form_validate($form, &$form_state) {

  // Ensure that submit does not get called unless the AHAH in the form was called
  if (!empty($form_state['ahah_submission'])) {
    return;
  }

}

/**
 * Purpose: Adds terms to an existing controlled vocabulary
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_form_submit($form, &$form_state) {

  // Ensure the AHAH in the form was called
  if (!empty($form_state['ahah_submission'])) {
    return;
  }

  // get the database
  $values = array('db_id' => $form_state['values']['db_id']);
  $results = tripal_core_chado_select('db', array('name'), $values);
  if (!$results or count($results) == 0) {
    drupal_set_message(t('Unable to add term.  Cannot find the database.'), 'error');
    return;
  }
  $db = $results[0];

  // get the cv
  $values = array('cv_id' => $form_state['values']['cv_id']);
  $results = tripal_core_chado_select('cv', array('name'), $values);
  if (!$results or count($results) == 0) {
    drupal_set_message(t('Unable to add term.  Cannot find the vocabulary.'), 'error');
    return;
  }
  $cv = $results[0];

  // get the accession for this term
  $accession = $form_state['values']['accession'];
  if (!$accession) {
    $accession = $form_state['values']['name'];
  }
  if (is_numeric($accession)) {
    $accession = $db->name . ":" . $accession;
  }


  $update = 0;
  if ($form_state['values']['form_action'] == 'edit') {
    $update = 1;
  }

  // now add the term
  $term = array(
    'name' => $form_state['values']['name'],
    'namespace' => $cv->name,
    'id' => $accession,
    'def' => $form_state['values']['definition'],
    'is_obsolete' => $form_state['values']['is_obsolete'],
  );

  $is_relationship = $form_state['values']['is_relationshiptype'];
  $cvterm = tripal_cv_add_cvterm($term, $cv->name, $is_relationship, $update, $db->name);
  if ($cvterm) {
    if (!$update) {
      drupal_set_message('Term added successfully.');
    }
    else {
      drupal_set_message('Term updated successfully.');
    }
  }
  else {
    drupal_set_message('Could not add term. Check Drupal recent logs for error messages.', 'error');
  }

}

/**
 * Purpose: This function gets called when the selecting of a cv from
 *   the select list triggers it. This function simply rebuilds the form
 *   with new information. No elements are created here
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvterm_callback() {
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
 * Cvterm path form submit
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvtermpath_form_submit($form, &$form_state) {
  global $user;

  $cvid =  $form_state['values']['cvid'];

  // first get the controlled vocabulary name:
  $cv = chado_query("SELECT * FROM {cv} WHERE cv_id = :cv_id", array(':cv_id' => $cvid));
  $cv = $cv->fetch_object();

  // Submit a job to update cvtermpath
  $job_args = array($cvid);
  if ($form_state['values']['op'] == t('Update cvtermpath')) {
    tripal_add_job("Update cvtermpath: $cv->name", 'tripal_cv',
       'tripal_cv_update_cvtermpath', $job_args, $user->uid);
  }
}