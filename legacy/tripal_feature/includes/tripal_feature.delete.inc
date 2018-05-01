<?php
/**
 * @file
 * Administration Interface for deleting multiple features
 */

/**
 * A form for indicating the features to delete
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_delete_form() {
  $form['desc'] = [
    '#markup' => t("Use one or more of the following fields to identify sets of features to be deleted."),
  ];

  $form['feature_names'] = [
    '#type' => 'textarea',
    '#title' => t('Feature Names'),
    '#description' => t('Please provide a list of feature names or unique names,
       separated by spaces or by new lines to be delete. If you specify feature names then
       all other options below will be ignored (except the unique checkbox).'),
  ];
  $form['is_unique'] = [
    '#title' => t('Names are Unique Names'),
    '#type' => 'checkbox',
    '#description' => t('Select this checbox if the names listed in the feature
      names box above are the unique name of the feature rather than the human readable names.'),
  ];
  $cv = tripal_get_cv(['name' => 'sequence']);
  $form['seq_type'] = [
    '#title' => t('Feature Type'),
    '#type' => 'textfield',
    '#description' => t("Choose the feature type."),
    '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/$cv->cv_id",
  ];

  $organisms = tripal_get_organism_select_options(FALSE);
  $form['organism_id'] = [
    '#title' => t('Organism'),
    '#type' => 'select',
    '#description' => t("Choose the organism for which features will be deleted."),
    '#options' => $organisms,
  ];

  $analyses = tripal_get_analysis_select_options(FALSE);
  $form['analysis_id'] = [
    '#title' => t('Analysis'),
    '#type' => t('select'),
    '#description' => t("Choose the analysis for which associated features will be deleted."),
    '#options' => $analyses,
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Delete Features'),
  ];
  return $form;
}

/**
 * Validation for the delete features form
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_delete_form_validate($form, &$form_state) {
  $organism_id = $form_state['values']['organism_id'];
  $seq_type = trim($form_state['values']['seq_type']);
  $analysis_id = $form_state['values']['analysis_id'];
  $is_unique = $form_state['values']['is_unique'];
  $feature_names = $form_state['values']['feature_names'];

  if (!$analysis_id and !$organism_id and !$seq_type and !$feature_names) {
    form_set_error('feature_names', t("Please select at least one option"));
  }
}

/**
 * Submit for the delete features form
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_delete_form_submit($form, &$form_state) {
  global $user;

  $organism_id = $form_state['values']['organism_id'];
  $seq_type = trim($form_state['values']['seq_type']);
  $analysis_id = $form_state['values']['analysis_id'];
  $is_unique = $form_state['values']['is_unique'];
  $feature_names = $form_state['values']['feature_names'];

  $args = [$organism_id, $analysis_id, $seq_type, $is_unique, $feature_names];
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
 *   (Optional) A Boolean stating whether the names are unique (ie:
 *   feature.uniquename) or not (ie: feature.name)
 * @param $feature_names
 *   (Optional) A space separated list of the names of features to delete
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_delete_features($organism_id, $analysis_id, $seq_type,
                                        $is_unique, $feature_names, $job = NULL) {

  global $user;
  $match = [];

  // Begin the transaction.
  $transaction = db_transaction();
  print "\nNOTE: Deleting features is performed using a database transaction. \n" .
    "If the load fails or is terminated prematurely then the entire set of \n" .
    "deletions is rolled back and will not be found in the database\n\n";
  try {

    // If feature names have been provided then handle those
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
      $num_deletes = chado_select_record('feature', ['count(*) as cnt'], $match);
      print "Deleting " . $num_deletes[0]->cnt . " features\n";

      chado_delete_record('feature', $match);
      return;
    }

    // Now handle the combintation of all other inputs.
    $args = [];
    $sql = "";
    $join = '';
    $where = '';
    if ($analysis_id) {
      $join .= 'INNER JOIN {analysisfeature} AF on F.feature_id = AF.feature_id ';
      $join .= 'INNER JOIN {analysis} A on A.analysis_id = AF.analysis_id ';
      $where .= 'AND A.analysis_id = :analysis_id ';
      $args[':analysis_id'] = $analysis_id;
    }
    if ($organism_id) {
      $where .= 'AND F.organism_id = :organism_id ';
      $args[':organism_id'] = $organism_id;
    }
    if ($seq_type) {
      $join .= 'INNER JOIN {cvterm} CVT ON CVT.cvterm_id = F.type_id';
      $where .= 'AND CVT.name = :type_name';
      $args[':type_name'] = $seq_type;
    }

    // Do not perform a delete if we have no additions to the where clause
    // otherwise all features will be deleted and this is probably not what
    // is wanted.
    if (!$where) {
      throw new Exception('Cannot delete features as no filters are available');
    }
    // First, count the number of records to be deleted
    $sql = "
      SELECT count(F.feature_id)
      FROM {feature} F
        $join
      WHERE 1=1 $where
    ";
    $num_deleted = chado_query($sql, $args)->fetchField();
    // Second, delete the records.
    $sql = "
      DELETE FROM {feature} WHERE feature_id IN (
        SELECT F.feature_id
        FROM {feature} F
          $join
        WHERE 1=1 $where
      )
    ";
    chado_query($sql, $args);
    print "Deletiong completed successfully. Deleted $num_deleted feature(s).\n";

    print "Now removing orphaned feature pages\n";
    chado_cleanup_orphaned_nodes('feature');
  } catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
    watchdog_exception('tripal_feature', $e);
    return 0;
  }
  print "\nDone\n";
}

