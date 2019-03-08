<?php

/**
 * @file
 * Administration Interface for deleting multiple analyses
 */

/**
 * A form for indicating the analyses to delete
 *
 * @ingroup tripal_legacy_analysis
 */
function tripal_analysis_delete_form() {
  // get the list of analyses
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $org_rset = chado_query($sql);
  $analyses = [];
  while ($analysis = $org_rset->fetchObject()) {
    $analyses[$analysis->analysis_id] = "$analysis->name";
  }

  $form['desc'] = [
    '#markup' => t("Use the following form to delete analyses which may or may
        not be synced. Please be cautious. Deleting an analysis will cause all
        data associated with the analysis to also be deleted."),
  ];

  $form['analyses'] = [
    '#title' => t('analysis'),
    '#type' => 'checkboxes',
    '#options' => $analyses,
    '#prefix' => '<div style="height: 400px; overflow: scroll">',
    '#suffix' => '</div><br>',
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Delete analyses'),
  ];
  return $form;
}

/**
 * Submit for the delete features form
 *
 * @ingroup tripal_legacy_analysis
 */
function tripal_analysis_delete_form_submit($form, &$form_state) {
  global $user;

  // Convert the analyses input into an array of just ids.
  $analyses = $form_state['values']['analyses'];
  $org_ids = [];
  foreach ($analyses as $id => $val) {
    if ($val != 0) {
      $org_ids[] = $id;
    }
  }

  if (count($org_ids) > 0) {
    $args = [$org_ids];

    tripal_add_job("Delete analysis", 'tripal_analysis',
      'tripal_analysis_delete_analyses', $args, $user->uid);
    drupal_set_message(t("analyses will disappear from the list below once the job completes."));
  }
  else {
    drupal_set_message(t("Please select at least one analysis to delete."), "error");
  }
}

/**
 * Function to actually delete the features indicated
 *
 * @param $analysis_id
 *   The list of analysis_id of the features to delete
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_legacy_analysis
 */
function tripal_analysis_delete_analyses($analyses, $job = NULL) {

  global $user;

  // Deleting of analyses will cause a cascade delete on the
  // fassociated tables which may include the featureloc table. The create_point
  // function which is not prefix with the schema, and an error occurs.
  // Therefore, we set the active database to chado to get around that
  // problem.

  // begin the transaction
  $transaction = db_transaction();
  print "\nNOTE: Deleting analyses is performed using a database transaction. \n" .
    "If the load fails or is terminated prematurely then the entire set of \n" .
    "deletions is rolled back and will not be found in the database\n\n";

  try {
    $values = [
      'analysis_id' => $analyses,
    ];
    $num_deletes = chado_select_record('analysis', ['count(*) as cnt'], $values);
    print "Deleting " . $num_deletes[0]->cnt . " analyses\n";
    chado_delete_record('analysis', $values);

    print "Removing orphaned analysis pages\n";
    chado_cleanup_orphaned_nodes('analysis');
  } catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
    watchdog_exception('tripal_analysis', $e);
    return 0;
  }

  print "\nDone\n";
}
