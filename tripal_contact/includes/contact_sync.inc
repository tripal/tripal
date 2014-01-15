<?php
/*
 * 
 */
function tripal_contact_sync_form() {

  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync Contacts')
  );
  $form['sync']['sync_all'] = array(
    '#markup' => t('<p>Syncing a contact will create a Drupal page for every contact 
      record in the Chado database. Click the button below to sync all contacts in 
      Chado that currently are not already synced with Drupal.</p>'),
  );

  $form['sync']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Sync contacts')
  );
  
  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#markup' => t("<p>With Drupal and chado residing in different databases ".
      "it is possible that nodes in Drupal and contacts in Chado become ".
      "\"orphaned\".  This can occur if an contact node in Drupal is ".
      "deleted but the corresponding chado contact is not and/or vice ".
      "versa. Click the button below to resolve these discrepancies.</p>"),
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned contacts'),
  );

  return $form;
}


/*
 * 
 */
function tripal_contact_sync_form_submit($form, $form_state) {
  global $user;    //needed to make the current users details available so access of user id is available
  
  if ($form_state['values']['op'] == t('Sync contacts')) {
    $job_args = array();
    $job_id = tripal_add_job('Sync contacts', 'tripal_contact', 'tripal_contact_sync_contacts', 
      $job_args, $user->uid);
  }

  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned contacts')) {
    $job_args = array();
    tripal_add_job('Cleanup orphaned contacts', 'tripal_contact',
     'tripal_contact_cleanup', $job_args, $user->uid);
  }
}
/**
 *
 *
 * @ingroup tripal_contact
 */
function tripal_contact_sync_contacts($job_id = NULL) {
  global $user;
  
  // get the list of contacts that have not yet been synced
  // and ignore the default 'NULL' contact. we don't want
  // to sync that one.
  $sql = "
    SELECT C.*
    FROM chado.contact C
      LEFT JOIN {chado_contact} CP ON CP.contact_id = C.contact_id
    WHERE CP.contact_id IS NULL
  ";
  $results = db_query($sql);

  while ($contact = $results->fetchObject()) {
    $new_node = new stdClass();
    $new_node->uid = $user->uid;
    $new_node->contact_id = $contact->contact_id;
    $new_node->type = 'chado_contact';
    $new_node->title = $contact->name;
    $new_node->description = $contact->description;
    $new_node->type_id = $contact->type_id;
  
    $form = array(); // dummy variable
    $form_state = array(); // dummy variable
    node_validate($new_node, $form, $form_state);
    if (!form_get_errors()) {
      $node = node_submit($new_node);
      node_save($node);
      if ($node->nid) {
        print "Added contact: $new_node->title\n";
      }
    }
    else {
      print "ERROR: Unable to create " . $contact->name . "\n" . print_r($errors, TRUE) . "\n";
      return FALSE;
    }
  } 
}

/**
 * Remove orphaned drupal nodes
 *
 * @param $dummy
 *   Not Used -kept for backwards compatibility
 * @param $job_id
 *   The id of the tripal job executing this function
 *
 * @ingroup tripal_contact
 */
function tripal_contact_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('contact', $job_id);

}