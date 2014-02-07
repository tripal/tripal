<?php
/**
 * @file
 * Provides a form for creating & editing chado controlled vocabularies
 */

/**
 * Provides the actual "Select CV" form on the Update/Delete Controlled
 *   Vocabulary page. This form also triggers the edit javascript
 * @todo Modify this form to use Drupal AJAX
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_edit_form($form, &$form_state) {

  // get the cv_d if form was submitted via AJAX
  $cv_id = 0;
  if (array_key_exists('values', $form_state)) {
    $cv_id = $form_state['values']['cv_id'];
  }
  elseif (isset($form_state['build_info']['args'][0])) {
    $cv_id = $form_state['build_info']['args'][0];
  }

  // get a list of db from chado for user to choose
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);

  $cvs = array();
  $cvs[] = 'Select a vocabulary';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }

  $form['cv_id'] = array(
    '#title' => t('Controlled Vocabulary Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#ajax' => array(
      'callback' => 'tripal_cv_edit_form_ajax',
      'wrapper'  => 'cv-edit-div',
      'effect'   => 'fade',
      'event'    => 'change',
      'method'   => 'replace',
    ),
    '#default_value' => $cv_id,
  );


  // if we don't have a db_id then we can  return the form, otherwise
  // add in the other fields
  if ($cv_id) {
    tripal_cv_add_cv_form_fields($form, $form_state, $cv_id);

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
      '#prefix' => '<div id="cv-edit-div">',
      '#suffix' => '</div>',
    );
  }
  return $form;
}

/**
 * Form to add contolled vocabularies
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_add_form($form, $form_state) {

  // add in the form fields to this form
  tripal_cv_add_cv_form_fields($form, $form_state);

  $form['add'] = array(
    '#type'         => 'submit',
    '#value'        => t('Add'),
    '#weight'       => 5,
  );
  return $form;
}

/**
 * Form fields in common between the cv add & edit form.
 *
 * @ingroup tripal_cv
 */
function tripal_cv_add_cv_form_fields(&$form, $form_state, $cv_id = NULL) {

  $default_name = '';
  $default_desc = '';

  if ($cv_id) {
    $values = array('cv_id' => $cv_id);
    $result = chado_select_record('cv', array('*'), $values);
    $cv = $result[0];
    $default_name = $cv->name;
    $default_desc = $cv->definition;
  }

  // add a fieldset for the Drupal Schema API
  $form['fields'] = array(
    '#type' => 'fieldset',
    '#title' => 'Controlled Vocabulary Details',
    '#collapsible' => 0,
  );

  $form['fields']['name']= array(
    '#type'          => 'textfield',
    '#title'         => t("Controlled Vocabulary name"),
    '#description'   => t('Please enter the name for this vocabulary.'),
    '#required'      => TRUE,
    '#default_value' => $default_name,
    '#maxlength'     => 255,
  );

  $form['fields']['definition']= array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#description'   => t('Please enter a definition for this vocabulary'),
    '#default_value' => $default_desc,
  );

  return $form;
}

/**
 * Validation fucntion for tripal_cv_cv_add_form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_add_form_validate($form, &$form_state) {
  tripal_cv_form_fields_validate($form, $form_state);
}

/**
 * Validation fucntion for tripal_cv_cv_edit_form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_edit_form_validate($form, &$form_state) {
  tripal_cv_form_fields_validate($form, $form_state);
}

/**
 * Generic validation form for shared fields of both the edit and add forms
 *
 * @ingroup tripal_cv
 */
function tripal_cv_form_fields_validate($form, &$form_state) {
  $name =  array_key_exists('name', $form_state['values'])       ? trim($form_state['values']['name']) : '';
  $desc =  array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';
  $cv_id = array_key_exists('cv_id', $form_state['values'])      ? trim($form_state['values']['cv_id']) : '';


  // make sure the cv name is unique
  $values = array('name' => $name);
  $results = chado_select_record('cv', array('cv_id'), $values);
  if (count($results) > 0 and $results[0]->cv_id != $cv_id) {
    form_set_error('name', 'The vocabulary name must be unique');
  }
}

/**
 * Submit cv add form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_add_form_submit($form, &$form_state) {

  $name =  array_key_exists('name', $form_state['values'])       ? trim($form_state['values']['name']) : '';
  $desc =  array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';

  $values = array(
    'name' => $name,
    'definition' => $desc,
  );
  $success = chado_insert_record('cv', $values);
  if ($success) {
    drupal_set_message(t("Controlled vocabulary added"));
  }
  else {
    drupal_set_message(t("Failed to add controlled vocabulary."));
  }
}

/**
 * Submit cv edit form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cv_edit_form_submit($form, &$form_state) {
  $name  = array_key_exists('name', $form_state['values'])       ? trim($form_state['values']['name']) : '';
  $desc  = array_key_exists('definition', $form_state['values']) ? trim($form_state['values']['definition']) : '';
  $cv_id = array_key_exists('cv_id', $form_state['values'])      ? trim($form_state['values']['cv_id']) : '';
  $op    = array_key_exists('op', $form_state['values'])         ? trim($form_state['values']['op']) : '';

  $values = array(
    'name' => $name,
    'definition' => $desc,
  );
  if (strcmp($op, 'Update')==0) {
    $match = array('cv_id' => $cv_id);
    $success = chado_update_record('cv', $match, $values);
    if ($success) {
      drupal_set_message(t("Controlled vocabulary updated"));
    }
    else {
      drupal_set_message(t("Failed to update controlled vocabulary."));
    }
  }
  if (strcmp($op, 'Delete')==0) {
    $match = array('cv_id' => $cv_id);
    $success = chado_delete_record('cv', $match);
    if ($success) {
      drupal_set_message(t("Controlled vocabulary deleted"));
    }
    else {
      drupal_set_message(t("Failed to delete controlled vocabulary."));
    }
  }
}

/**
 * Ajax callback for the tripal_cv_form
 *
 * @ingroup tripal_cv
 */
function tripal_cv_edit_form_ajax($form, $form_state) {

  $elements = array();

  // add in the form fields and the buttons
  if (array_key_exists('cv_id', $form_state['values'])) {
    $elements['fields'] = $form['fields'];
    $elements['update'] = $form['update'];
    $elements['delete'] = $form['delete'];
  }

   // add back in the cv-edit-div that is used for the next round of AJAX
  $elements['fields']['#prefix'] =  '<div id="cv-edit-div">';
  $elements['fields']['#suffix'] =  '</div">';

  // reset the values for the fields to the defaults
  $elements['fields']['name']['#value']        = $elements['fields']['name']['#default_value'];
  $elements['fields']['definition']['#value']  = $elements['fields']['definition']['#default_value'];

  //drupal_set_message('<pre>' . print_r($elements, TRUE) . '</pre>', "status");

  return $elements;
}
