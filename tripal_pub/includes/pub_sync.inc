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
    $node = tripal_pub_sync_pub($pub);
  }
}
/**
 * @param $pub
 *   A publication object
 *
 * @return
 *   A new Drupal node object on success. FALSE on failure
 *
 * @ingroup tripal_pub
 */
function tripal_pub_sync_pub($pub) {
  global $user;

  if(!$pub->pyear) {
    watchdog('tpub_sync', "Skipping pub without published year: %title.", 
      array('%title' => $pub->title), WATCHDOG_WARNING);
    return FALSE;
  }

  $new_node = new stdClass();
  $new_node->pub_id      = $pub->pub_id;
  $new_node->type        = 'chado_pub';
  $new_node->uid         = $user->uid;
  $new_node->title       = substr($pub->title, 0 ,255); // node titles can't be longer than 255 characters
  $new_node->pubtitle    = $pub->title;
  $new_node->pyear       = $pub->pyear;
  $new_node->uniquename  = $pub->uniquename;
  $new_node->type_id     = $pub->type_id;
  $new_node->series_name = $pub->series_name;

  node_validate($new_node);
  $errors = form_get_errors();
  if (!$errors) {
    $node = node_submit($new_node);
    node_save($node);
    if ($node->nid) {
      print "Added " . $pub->pub_id . "\n";
    }
    else {
      watchdog('tpub_sync', "Unable to create publication node: %title.", 
        array('%title' => $pub->title), WATCHDOG_ERROR);
      return FALSE;
    }
  }
  // if there are form errors then we need to reset the form errors cache, print a message and return
  else {
    form_set_error(NULL,'',TRUE);
    watchdog('tpub_sync', "Unable to create publication node: %title\n%errs",
      array('%title' => $pub->title, '%errs' => print_r($errors, TRUE)), WATCHDOG_ERROR);
    return FALSE;
  }
  return $node;
}
