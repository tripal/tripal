<?php
/*
 * 
 */
function tripal_contact_sync_form() {

  $form['sync_all'] = array(
    '#type' => 'item',
    '#value' => t('Syncing a contact will create a Drupal page for every contact record in the Chado database. Click the button below to sync all contacts in Chado that currently are not already synced with Drupal.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10,
    '#value' => t('Sync contacts')
  );

  return $form;
}


/*
 * 
 */
function tripal_contact_sync_form_submit($form, $form_state) {
  global $user;    //needed to make the current users details available so access of user id is available
  $job_args = array();
  $job_id = tripal_add_job('Sync contacts', 'tripal_contact', 'tripal_contact_sync_contacts', $job_args, $user->uid);

}
/**
 *
 *
 * @ingroup tripal_contact
 */
function tripal_contact_sync_contacts($job_id = NULL) {
  
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
    $node = tripal_contact_sync_contact($contact);
  }
}
/**
 * @param $contact
 *   A contactlication object
 *   
 * @return
 *   A new Drupal node object on success. FALSE on failure
 *   
 * @ingroup tripal_contact
 */
function tripal_contact_sync_contact($contact) {
  global $user;
  
  $new_node = new stdClass();
  $new_node->contact_id = $contact->contact_id;
  $new_node->type = 'chado_contact';
  $new_node->title = $contact->name;
  $new_node->description = $contact->description;
  $new_node->type_id = $contact->type_id;

  node_validate($new_node);
  $errors = form_get_errors();
  if (!$errors) {
    $node = node_submit($new_node);
    node_save($node);
    if ($node->nid) {
      print "Added " . $contact->contact_id . "\n";      
    }
    else {
      print "ERROR: Unable to create " . $contact->name . "\n";
      return FALSE;
    }
  }
  else {
    print "ERROR: Unable to create " . $contact->name . "\n" . print_r($errors, TRUE) . "\n";
    return FALSE;
  }
  return $node; 
}
