<?php
/**
 * @file
 * Provides a form for updating controlled vocabularies path
 */

/**
 * Form for re-doing the cvterm path
 *
 * @ingroup tripal_cv
 */
function tripal_cv_cvtermpath_form() {

  // get a list of db from chado for user to choose
  $sql = "SELECT * FROM {cv} WHERE NOT name = 'tripal' ORDER BY name ";
  $results = chado_query($sql);

  $cvs = array();
  $cvs[] = '';
  foreach ($results as $cv) {
    $cvs[$cv->cv_id] = $cv->name;
  }

  $form['cvid'] = array(
    '#title' => t('Controlled Vocabulary/Ontology Name'),
    '#type' => 'select',
    '#options' => $cvs,
    '#description' => t('The Chado cvtermpath is a database table that provides lineage for ontology terms
      and is useful for quickly finding any ancestor parent of a term.  This table must be populated for each
      ontology.  Select a controlled vocabulary for which you would like to upate the cvtermpath.'),
  );

  $form['description'] = array(
    '#type' => 'item',
    '#value' => t("Submit a job to update chado cvtermpath table."),
    '#weight' => 1,
  );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Update cvtermpath'),
    '#weight' => 2,
  );

  return $form;
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
  $sql = "SELECT * FROM {cv} WHERE cv_id = :cv_id";
  $cv = chado_query($sql, array(':cv_id' => $cvid))->fetchObject();

  // Submit a job to update cvtermpath
  $job_args = array($cvid);
  if ($form_state['values']['op'] == t('Update cvtermpath')) {
    tripal_add_job("Update cvtermpath: $cv->name", 'tripal_cv',
       'tripal_cv_update_cvtermpath', $job_args, $user->uid);
  }
}
