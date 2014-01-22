<?php
/*
 * The admin form for submitting job to create citations
*/
function tripal_pub_citation_form($form_state) {

  $form['instructions'] = array(
    '#markup' => '<p>Use this form to unify publication citations. Citations are created automtically when
      importing publications but citations are set by the user when publications are added manually.  
      Or publications added to the Chado database by tools other than the Tripal Publication Importer may 
      not have citations set. If you are certain that all necessary information for all publications is present (e.g. 
      authors, volume, issue, page numbers, etc.) but citations are not consistent, then you can
      choose to update all citations for all publications using the form below. Alternatively, you
      can update citations only for publication that do not already have one.</p>'
  );
  
  $form['options'] = array(
    '#type' => 'radios',
    '#options' => array(
      'all' => 'Create citation for all publications. Replace the existing citation if it exists.', 
      'new' => 'Create citation for publication only if it does not already have one.'),
    '#default_value' => 'all'
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  return $form;
}

/*
 * Submit form. Create Tripal job for citations
 */

function tripal_pub_citation_form_submit(&$form_state) {
  $options [0] = $form_state['options']['#value'];
  tripal_add_job("Create citations ($options[0])", 'tripal_pub', 'tripal_pub_create_citations', $options, $user->uid);
}

/*
 * Launch the Tripal job to generate citations
 */
function tripal_pub_create_citations ($options) {
  $skip_existing = TRUE;
  $sql = "
    SELECT cvterm_id 
    FROM {cvterm} 
    WHERE 
      name = 'Citation' AND 
      cv_id = (SELECT cv_id FROM {cv} WHERE name = 'tripal_pub')
  ";
  $citation_type_id = chado_query($sql)->fetchField();
  // Create and replace citation for all pubs
  if ($options == 'all') {
    $sql = "SELECT pub_id FROM {pub} P WHERE pub_id <> 1";
    $skip_existing = FALSE;  
  } 
  // Create citation for pubs that don't already have one
  else if ($options == 'new') {
    $sql = "
      SELECT pub_id 
      FROM {pub} P 
      WHERE 
        (SELECT value 
         FROM {pubprop} PB 
         WHERE type_id = :type_id AND P.pub_id = PB.pub_id AND rank = 0) IS NULL 
        AND  pub_id <> 1
    ";
    $skip_existing = TRUE;
  }

  $result = chado_query($sql, array(':type_id' => $citation_type_id));
  $counter_updated = 0;
  $counter_generated = 0;
  while ($pub = $result->fetchObject()) {
    $pub_arr = tripal_pub_get_publication_array($pub->pub_id, $skip_existing);
    if ($pub_arr) {
      $citation = tripal_pub_create_citation ($pub_arr);
      print $citation . "\n\n";
      // Replace if citation exists. This condition is never TRUE if $skip_existing is TRUE
      if ($pub_arr['Citation']) {
        $sql = "
          UPDATE {pubprop} SET value = :value 
          WHERE pub_id = :pub_id  AND type_id = :type_id AND rank = :rank
        ";
        chado_query($sql, array(':value' => $citation, ':pub_id' => $pub->pub_id, 
          ':type_id' => $citation_type_id, ':rank' => 0));
        $counter_updated ++;
      // Generate a new citation
      } else {
        $sql = "
          INSERT INTO {pubprop} (pub_id, type_id, value, rank) 
          VALUES (:pub_id, :type_id, :value, :rank)
        ";
        chado_query($sql, array(':pub_id' => $pub->pub_id, ':type_id' => $citation_type_id, 
          ':value' => $citation, ':rank' => 0));
        $counter_generated ++;
      }
    }
  }
  print "$counter_generated citations generated. $counter_updated citations updated.\n";
}