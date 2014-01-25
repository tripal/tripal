<?php
/*
 *
 */
function tripal_pub_sync_form() {
  $form['sync'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sync')
  );
  $form['sync']['sync_all'] = array(
    '#markup' => t('<p>Syncing a publication will create a Drupal page for every publicatoin record in the Chado database. Click the button below to sync all publications in Chado that currently are not already synced with Drupal.</p>'),
  );

  $form['sync']['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10,
    '#value' => t('Sync Publications')
  );

  $form['cleanup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean Up')
  );
  $form['cleanup']['description'] = array(
    '#markup' => t("<p>With Drupal and chado residing in different databases ".
        "it is possible that nodes in Drupal and publications in Chado become ".
        "\"orphaned\".  This can occur if an pub node in Drupal is ".
        "deleted but the corresponding chado pub is not and/or vice ".
        "versa. Click the button below to resolve these discrepancies.</p>"),
    '#weight' => 1,
  );
  $form['cleanup']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Clean up orphaned publications'),
    '#weight' => 2,
  );
  
  return $form;
}

/*
 *
 */
function tripal_pub_sync_form_submit($form, $form_state) {
  global $user;    //needed to make the current users details available so access of user id is available
  if ($form_state['values']['op'] == t('Sync Publications')) {
    $job_args = array();
    $job_id = tripal_add_job('Sync Publications', 'tripal_pub', 'tripal_pub_sync_pubs', $job_args, $user->uid);
    
  }
  // -------------------------------------
  // Submit the Cleanup Job if selected
  if ($form_state['values']['op'] == t('Clean up orphaned publications')) {
    $job_args = array();
    tripal_add_job('Cleanup orphaned publications', 'tripal_pub',
    'tripal_pub_cleanup', $job_args, $user->uid);
  }
}
/**
 *
 *
 * @ingroup tripal_pub
 */
function tripal_pub_sync_pubs($job_id = NULL) {
  global $user;
  
  // get the list of pubs that have not yet been synced
  // and ignore the default 'NULL' pub. we don't want
  // to sync that one.
  $sql = "
    SELECT P.*
    FROM {pub} P
      LEFT JOIN public.chado_pub CP ON CP.pub_id = P.pub_id
    WHERE CP.pub_id IS NULL and NOT P.title = 'NULL'
  ";
  $results = chado_query($sql);

  // We'll use the following SQL statement for checking if the pub
  // already exists as a drupal node.
  $sql = "SELECT * FROM {chado_pub} WHERE pub_id = :pub_id";
  
  while ($pub = $results->fetchObject()) {

    // check if this pub already exists in the drupal database. if it
    // does then skip this pub and go to the next one.
    if (!db_query($sql, array(':pub_id' => $pub->pub_id))->fetchObject()) {
      
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
    
      $form = array(); // dummy variable
      $form_state = array(); // dummy variable
      node_validate($new_node, $form, $form_state);
      if (!form_get_errors()) {
        $node = node_submit($new_node);
        node_save($node);
        if ($node->nid) {
          print "Added $new_node->title\n";
        }
      }
      else {
        watchdog('tpub_sync', "Unable to create publication node. ID: %pub_id, Title: %title",
          array('%pub_id' => $pub->pub_id, '%title' => $pub->title), WATCHDOG_WARNING);
      }
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
 * @ingroup tripal_pub
 */
function tripal_pub_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_chado_node_cleanup_orphaned('pub', $job_id);

}