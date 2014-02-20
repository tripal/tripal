<?php
/**
 * @file
 * Administration Interface for deleting multiple features
 */

/**
 * A form for indicating the features to delete
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_form() {
  // get the list of organisms
  $sql = "SELECT * FROM {organism} ORDER BY genus, species";
  $org_rset = chado_query($sql);
  $organisms = array();
  $organisms[''] = '';
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }
  $form['desc'] = array(
    '#markup' => t("Use one or more of the following fields to identify sets of features to be deleted."),
  );

  $form['feature_names']= array(
    '#type' => 'textarea',
    '#title' => t('Feature Names'),
    '#description' => t('Please provide a list of feature names or unique names,
       separated by spaces or by new lines to be delete. If you specify feature names then
       all other options below will be ignored (except the unique checkbox).'),
  );
  $form['is_unique'] = array(
    '#title' => t('Names are Unique Names'),
    '#type' => 'checkbox',
    '#description' => t('Select this checbox if the names listed in the feature
      names box above are the unique name of the feature rather than the human readable names.'),
  );
  $form['seq_type']= array(
    '#type' => 'textfield',
    '#title' => t('Sequence Type'),
    '#description' => t('Please enter the Sequence Ontology term that describes the features to be deleted. Use in conjunction with an organism or anaylysis.'),
  );

  $form['organism_id'] = array(
   '#title'       => t('Organism'),
   '#type'        => t('select'),
   '#description' => t("Choose the organism for which features will be deleted."),
   '#options'     => $organisms,
  );


  // get the list of analyses
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $org_rset = chado_query($sql);
  $analyses = array();
  $analyses[''] = '';
  while ($analysis = $org_rset->fetchObject()) {
    $analyses[$analysis->analysis_id] = "$analysis->name ($analysis->program $analysis->programversion, $analysis->sourcename)";
  }
  //  TODO: ADD THIS BACK IN LATER
  //
  //   $form['analysis']['analysis_id'] = array (
  //     '#title'       => t('Analysis'),
  //     '#type'        => t('select'),
  //     '#description' => t("Choose the analysis for which associated features will be deleted."),
  //     '#options'     => $analyses,
  //   );

  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Delete Features'),
  );
  return $form;
}

/**
 * Validation for the delete features form
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_form_validate($form, &$form_state) {
  $organism_id   = $form_state['values']['organism_id'];
  $seq_type      = trim($form_state['values']['seq_type']);
  $analysis_id   = $form_state['values']['analysis_id'];
  $is_unique     = $form_state['values']['is_unique'];
  $feature_names = $form_state['values']['feature_names'];

  if (!$organism_id and !$anaysis_id and !$seq_type and !$feature_names) {
    form_set_error('feature_names', t("Please select at least one option"));
  }

  // check to make sure the types exists
  if ($seq_type) {
    $cvtermsql = "
      SELECT CVT.cvterm_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
        LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
      WHERE cv.name = :cvname and (CVT.name = :name or CVTS.synonym = :synonym)
    ";
    $cvterm = chado_query($cvtermsql,
      array(':cvname' => 'sequence', ':name' => $seq_type, ':synonym' => $seq_type))->fetchObject();
    if (!$cvterm) {
      form_set_error('seq_type', t("The Sequence Ontology (SO) term selected for the " .
      "sequence type is not available in the database. Please check spelling or select another."));
    }
  }
}

/**
 * Submit for the delete features form
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_form_submit($form, &$form_state) {
  global $user;

  $organism_id   = $form_state['values']['organism_id'];
  $seq_type      = trim($form_state['values']['seq_type']);
  $analysis_id   = $form_state['values']['analysis_id'];
  $is_unique     = $form_state['values']['is_unique'];
  $feature_names = $form_state['values']['feature_names'];

  $args = array($organism_id, $analysis_id, $seq_type, $is_unique, $feature_names);

  tripal_add_job("Delete features", 'tripal_feature',
    'tripal_feature_delete_features', $args, $user->uid);
}

/**
 * Function to actually delete the features indicated
 *
 * @param $organism_id
 *   (Optional) The organism_id of the features to delete
 * @param $analysis_id
 *   (Optional) The analysis_id of the features to delete
 * @param $seq_type
 *   (Optional) The cvterm.name of the feature types to delete
 * @param $is_unique
 *   (Optional) A Boolean stating whether the names are unique (ie: feature.uniquename)
 *   or not (ie: feature.name)
 * @param $feature_names
 *   (Optional) A space separated list of the names of features to delete
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_features($organism_id, $analysis_id, $seq_type,
  $is_unique, $feature_names, $job = NULL) {

  global $user;
  $match = array();

  // if feature names have been provided then handle that separately
  if ($feature_names) {
    $names = preg_split('/\s+/', $feature_names);
    if (sizeof($names) == 1) {
      $names = $names[0];
    }
    if ($is_unique) {
      $match['uniquename'] = $names;
    }
    else {
      $match['name'] = $names;
    }
    $num_deletes = chado_select_record('feature', array('count(*) as cnt'), $match);
    print "Deleting " . $num_deletes[0]->cnt . " features\n";
    chado_delete_record('feature', $match);
  }

  // if the user has provided an analysis_id then handle that separately
  elseif ($analysis_id) {
    tripal_feature_delete_by_analysis();
  }
  else {

    if ($organism_id) {
      $match['organism_id'] = $organism_id;
    }
    if ($seq_type) {
      $match['type_id'] = array(
        'name' => $seq_type,
        'cv_id' => array(
          'name' => 'sequence'
        ),
      );
    }
    $num_deletes = chado_select_record('feature', array('count(*) as cnt'), $match);
    print "Deleting " . $num_deletes[0]->cnt . " features\n";
    chado_delete_record('feature', $match);
  }

  print "Removing orphaned feature pages\n";
  tripal_features_cleanup(array(), $user->uid);
}

/**
 * Function to delete features based on an analysis passed in. This has not yet been
 * implemented in the form
 *
 * @todo: Implement this functionality and then add back in the form field
 *
 * @param $organism_id
 *   (Optional) The organism_id of the features to delete
 * @param $analysis_id
 *   (Optional) The analysis_id of the features to delete
 * @param $seq_type
 *   (Optional) The cvterm.name of the feature types to delete
 * @param $is_unique
 *   (Optional) A Boolean stating whether the names are unique (ie: feature.uniquename)
 *   or not (ie: feature.name)
 * @param $feature_names
 *   (Optional) A space separated list of the names of features to delete
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_by_analysis($organism_id, $analysis_id, $seq_type,
  $is_unique, $feature_names, $job = NULL) {

}
