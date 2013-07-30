<?php
/*
 * The admin form for submitting job to create citations
*/
function tripal_pub_citation_form($form_state) {

  $form['create_all'] = array(
    '#type' => 'item',
    '#value' => t('Submit a job to create citation for the publications in chado. '),
  );
  
  $form['options'] = array(
    '#type' => 'radios',
    '#options' => 
      array('all' => 'Create citation for all publications. Replace the existing citation if it exists.', 
                'new' => 'Create citation for publication only if it does not already have one.'),
    '#default_value' => 'all'
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10,
    '#value' => t('Submit')
  );

  return $form;
}

/*
 * Submit form. Create Tripal job for citations
 */

function tripal_pub_citation_form_submit(&$form_state) {
  $options [0] = $form_state['options']['#value'];
  tripal_add_job("Create citations ($options[0])", 'tripal_pub',
       'tripal_pub_create_citations', $options, $user->uid);
}

/*
 * Launch the Tripal job to generate citations
 */
function tripal_pub_create_citations ($options) {
  $skip_existing = TRUE;
  $citation_type_id = db_result(chado_query("SELECT cvterm_id FROM {cvterm} WHERE name = 'Citation' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'tripal_pub')"));
  // Create and replace citation for all pubs
  if ($options == 'all') {
  	$sql = "SELECT pub_id FROM {pub} P WHERE pub_id <> 1";
    $skip_existing = FALSE;
  // Create citation for pubs that don't already have one
  } else if ($options == 'new') {
  	$sql = "SELECT pub_id FROM {pub} P WHERE (SELECT value FROM {pubprop} PB WHERE type_id = $citation_type_id AND P.pub_id = PB.pub_id AND rank = 0) IS NULL AND  pub_id <> 1";
  	$skip_existing = TRUE;
  }

  $result = chado_query($sql);
  $counter_updated = 0;
  $counter_generated = 0;
  while ($pub = db_fetch_object($result)) {
    $pub_arr = tripal_pub_get_publication_array($pub->pub_id, $skip_existing);
    if ($pub_arr) {
      $citation = tripal_pub_create_citation ($pub_arr);
      print $citation . "\n\n";
      // Replace if citation exists. This condition is never TRUE if $skip_existing is TRUE
      if ($pub_arr['Citation']) {
        $sql = 
            "UPDATE {pubprop} 
             SET value = '%s' 
             WHERE pub_id = %d 
             AND type_id = %d
             AND rank = %d";
        chado_query($sql, $citation, $pub->pub_id, $citation_type_id, 0);
        $counter_updated ++;
      // Generate a new citation
      } else {
      	$sql = "INSERT INTO {pubprop} (pub_id, type_id, value, rank) VALUES (%d, %d, '%s', %d)";
      	chado_query($sql, $pub->pub_id, $citation_type_id, $citation, 0);
      	$counter_generated ++;
      }
    }
  }
  print "$counter_generated citations generated. $counter_updated citations updated.\n";
}