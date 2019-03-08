<?php
/**
 * @file
 * Administration Interface for deleting multiple organisms
 */

/**
 * A form for indicating the organisms to delete
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_delete_form() {
  // get the list of organisms
  $sql = "SELECT * FROM {organism} ORDER BY genus, species";
  $org_rset = chado_query($sql);
  $organisms = [];
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] = "$organism->genus $organism->species ($organism->common_name)";
  }

  $form['desc'] = [
    '#markup' => t("Use the following form to delete organisms which may or may
        not be synced. Please be cautious. Deleting an organism will cause all
        data associated with the organism to also be deleted."),
  ];

  $form['organisms'] = [
    '#title' => t('Organism'),
    '#type' => 'checkboxes',
    '#options' => $organisms,
    '#prefix' => '<div style="height: 400px; overflow: scroll">',
    '#suffix' => '</div><br>',
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Delete Organisms'),
  ];
  return $form;
}

/**
 * Submit for the delete features form
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_delete_form_submit($form, &$form_state) {
  global $user;

  // Convert the organisms input into an array of just ids.
  $organisms = $form_state['values']['organisms'];
  $org_ids = [];
  foreach ($organisms as $id => $val) {
    if ($val != 0) {
      $org_ids[] = $id;
    }
  }

  if (count($org_ids) > 0) {
    $args = [$org_ids];

    tripal_add_job("Delete Organism", 'tripal_organism',
      'tripal_organism_delete_organisms', $args, $user->uid);
    drupal_set_message(t("Organisms will disappear from the list below once the job completes."));
  }
  else {
    drupal_set_message(t("Please select at least one organism to delete."), "error");
  }
}

/**
 * Function to actually delete the features indicated
 *
 * @param $organism_id
 *   The list of organism_id of the features to delete
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_delete_organisms($organisms, $job = NULL) {

  global $user;

  // Deleting of organisms will cause a cascade delete on the
  // fassociated tables which may include the featureloc table. The create_point
  // function which is not prefix with the schema, and an error occurs.
  // Therefore, we set the active database to chado to get around that
  // problem.

  // begin the transaction
  $transaction = db_transaction();
  print "\nNOTE: Deleting organisms is performed using a database transaction. \n" .
    "If the load fails or is terminated prematurely then the entire set of \n" .
    "deletions is rolled back and will not be found in the database\n\n";

  try {
    $values = [
      'organism_id' => $organisms,
    ];
    $num_deletes = chado_select_record('organism', ['count(*) as cnt'], $values);
    print "Deleting " . $num_deletes[0]->cnt . " organisms\n";
    chado_delete_record('organism', $values);

    print "Removing orphaned organism pages\n";
    chado_cleanup_orphaned_nodes('organism');
  } catch (Exception $e) {
    print "\n"; // make sure we start errors on new line
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
    watchdog_exception('tripal_organism', $e);
    return 0;
  }
  print "\nDone\n";
}
