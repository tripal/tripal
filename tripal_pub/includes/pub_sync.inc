<?php
/*
 * 
 */
function tripal_pub_sync_form() {

  $form['sync_all'] = array(
    '#type' => 'item',
    '#value' => t('Syncing a publication will create a Drupal page for every publicatoin record in the Chado database. Click the button below to sync all publications in Chado that currently are not already synced with Drupal.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10,
    '#value' => t('Sync Publications')
  );

  return $form;
}


/*
 * 
 */
function tripal_pub_sync_form_submit($form, $form_state) {
  global $user;    //needed to make the current users details available so access of user id is available
  $job_args = array();
  $job_id = tripal_add_job('Sync Publications', 'tripal_pub', 'tripal_pub_sync_pubs', $job_args, $user->uid);

}
/**
 *
 *
 * @ingroup tripal_pub
 */
function tripal_pub_sync_pubs($job_id = NULL) {

  global $user;
  $page_content = '';

  // get the list of pubs that have not yet been synced
  // and ignore the default 'NULL' pub. we don't want
  // to sync that one.
  $sql = "
    SELECT P.*
    FROM chado.pub P
      LEFT JOIN {chado_pub} CP ON CP.pub_id = P.pub_id
    WHERE CP.pub_id IS NULL and NOT P.title = 'NULL'
  ";
  $results = db_query($sql);


  while ($pub = db_fetch_object($results)) {

    $new_node = new stdClass();
    $new_node->pub_id = $pub->pub_id;
    $new_node->type = 'chado_pub';
    $new_node->uid = $user->uid;
    $new_node->title = $pub->title;
    $new_node->pyear = $pub->pyear;
    $new_node->uniquename = $pub->uniquename;
    $new_node->type_id = $pub->type_id;

    node_validate($new_node);
    $errors = form_get_errors();
    if (!$errors) {
      $node = node_submit($new_node);
      node_save($node);
      if ($node->nid) {
        print "Added " . $pub->pub_id . "\n";
      }
      else {
        print "ERROR: Unable to create " . $pub->name . "\n";
      }
    }
    else {
      print "ERROR: Unable to create " . $pub->name . "\n" . print_r($errors, TRUE) . "\n";
    }   
  }
}