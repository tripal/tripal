<?php
/**
 * @file
 * Functions to manage citations
 */

/**
 * The admin form for submitting job to create citations
 *
 * @param $form_state
 *
 * @ingroup tripal_pub
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

/**
 * Submit form. Create Tripal job for citations
 *
 * @param $form_state
 *
 * @ingroup tripal_pub
 */
function tripal_pub_citation_form_submit(&$form_state) {
  $options [0] = $form_state['options']['#value'];
  tripal_add_job("Create citations ($options[0])", 'tripal_pub', 'tripal_pub_create_citations', $options, $user->uid);
}

/**
 * Launch the Tripal job to generate citations. Called by tripal jobs
 *
 * @param $options
 *  Options pertaining to what publications to generate citations for.
 *  One of the following must be present:
 *   - all: Create and replace citation for all pubs
 *   - new: Create citation for pubs that don't already have one
 *
 * @ingroup tripal_pub
 */
function tripal_pub_create_citations($options) {
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
      $citation = tripal_pub_create_citation($pub_arr);
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


/**
 * This function generates citations for publications.  It requires
 * an array structure with keys being the terms in the Tripal
 * publication ontology.  This function is intended to be used
 * for any function that needs to generate a citation.
 *
 * @param $pub
 *   An array structure containing publication details where the keys
 *   are the publication ontology term names and values are the
 *   corresponding details.  The pub array can contain the following
 *   keys with corresponding values:
 *     - Publication Type:  an array of publication types. a publication can have more than one type
 *     - Authors: a  string containing all of the authors of a publication
 *     - Journal Name:  a string containing the journal name
 *     - Journal Abbreviation: a string containing the journal name abbreviation
 *     - Series Name: a string containing the series (e.g. conference proceedings) name
 *     - Series Abbreviation: a string containing the series name abbreviation
 *     - Volume: the serives volume number
 *     - Issue: the series issue number
 *     - Pages: the page numbers for the publication
 *     - Publication Date:  A date in the format "Year Month Day"
 *
 * @return
 *   A text string containing the citation
 *
 * @ingroup tripal_pub
 */
function tripal_pub_create_citation($pub) {
  $citation = '';
  $pub_type = '';

  // An article may have more than one publication type. For example,
  // a publication type can be 'Journal Article' but also a 'Clinical Trial'.
  // Therefore, we need to select the type that makes most sense for
  // construction of the citation. Here we'll iterate through them all
  // and select the one that matches best.
  if(is_array($pub['Publication Type'])) {
    foreach ($pub['Publication Type'] as $ptype) {
      if ($ptype == 'Journal Article' ) {
        $pub_type = $ptype;
        break;
      }
      else if ($ptype == 'Conference Proceedings'){
        $pub_type = $ptype;
        break;
      }
      else if ($ptype == 'Review') {
        $pub_type = $ptype;
        break;
      }
      else if ($ptype == 'Book') {
        $pub_type = $ptype;
        break;
      }
      else if ($ptype == 'Letter') {
        $pub_type = $ptype;
        break;
      }
      else if ($ptype == 'Book Chapter') {
        $pub_type = $ptype;
        break;
      }
      else if ($ptype == "Research Support, Non-U.S. Gov't") {
        $pub_type = $ptype;
        // we don't break because if the article is also a Journal Article
        // we prefer that type
      }
    }
    if (!$pub_type) {
      tripal_report_error('tripal_pub', TRIPAL_ERROR,
        "Cannot generate citation for publication type: %types.\nTitle: %title.\nDbxref: %dbxref",
        array(
          '%types' => print_r($pub['Publication Type'], TRUE),
          '%title' => $pub['Title'],
          '%dbxref' => $pub['Publication Dbxref'],
        )
      );
      return FALSE;
    }
  }
  else {
    $pub_type = $pub['Publication Type'];
  }
  //----------------------
  // Journal Article
  //----------------------
  if ($pub_type == 'Journal Article') {
    if (array_key_exists('Authors', $pub)) {
      $citation = $pub['Authors'] . '. ';
    }
    
    $citation .= $pub['Title'] .  '. ';

    if (array_key_exists('Journal Name', $pub)) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    elseif (array_key_exists('Journal Abbreviation', $pub)) {
      $citation .= $pub['Journal Abbreviation'] . '. ';
    }
    elseif (array_key_exists('Series Name', $pub)) {
      $citation .= $pub['Series Name'] . '. ';
    }
    elseif (array_key_exists('Series Abbreviation', $pub)) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if (array_key_exists('Publication Date', $pub)) {
      $citation .= $pub['Publication Date'];
    }
    elseif (array_key_exists('Year', $pub)) {
      $citation .= $pub['Year'];
    }
    if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages',$pub)) {
      $citation .= '; ';
    }
    if (array_key_exists('Volume', $pub)) {
      $citation .= $pub['Volume'];
    }
    if (array_key_exists('Issue', $pub)) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if (array_key_exists('Pages', $pub)) {
      if (array_key_exists('Volume', $pub)) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }
  //----------------------
  // Review
  //----------------------
  if ($pub_type == 'Review') {
    if (array_key_exists('Authors', $pub)) {
      $citation = $pub['Authors'] . '. ';
    }
    
    $citation .= $pub['Title'] .  '. ';

    if (array_key_exists('Journal Name', $pub)) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    elseif (array_key_exists('Journal Abbreviation', $pub)) {
      $citation .= $pub['Journal Abbreviation'] . '. ';
    }
    elseif (array_key_exists('Series Name', $pub)) {
      $citation .= $pub['Series Name'] . '. ';
    }
    elseif (array_key_exists('Series Abbreviation', $pub)) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if (array_key_exists('Publication Date', $pub)) {
      $citation .= $pub['Publication Date'];
    }
    elseif (array_key_exists('Year', $pub)) {
      $citation .= $pub['Year'];
    }
    if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages',$pub)) {
      $citation .= '; ';
    }
    if (array_key_exists('Volume', $pub)) {
      $citation .= $pub['Volume'];
    }
    if (array_key_exists('Issue', $pub)) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if (array_key_exists('Pages', $pub)) {
      if (array_key_exists('Volume', $pub)) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }
  //----------------------
  // Research Support, Non-U.S. Gov't
  //----------------------
  elseif ($pub_type == "Research Support, Non-U.S. Gov't") {
    if (array_key_exists('Authors', $pub)) {
      $citation = $pub['Authors'] . '. ';
    }
    
    $citation .= $pub['Title'] .  '. ';

    if (array_key_exists('Journal Name', $pub)) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    if (array_key_exists('Publication Date', $pub)) {
      $citation .= $pub['Publication Date'];
    }
    elseif (array_key_exists('Year', $pub)) {
      $citation .= $pub['Year'];
    }
    $citation .= '.';
  }
  //----------------------
  // Letter
  //----------------------
  elseif ($pub_type == 'Letter') {
    if (array_key_exists('Authors', $pub)) {
      $citation = $pub['Authors'] . '. ';
    }
    
    $citation .= $pub['Title'] .  '. ';
    if (array_key_exists('Journal Name', $pub)) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    elseif (array_key_exists('Journal Abbreviation', $pub)) {
      $citation .= $pub['Journal Abbreviation'] . '. ';
    }
    elseif (array_key_exists('Series Name', $pub)) {
      $citation .= $pub['Series Name'] . '. ';
    }
    elseif (array_key_exists('Series Abbreviation', $pub)) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if (array_key_exists('Publication Date', $pub)) {
      $citation .= $pub['Publication Date'];
    }
    elseif (array_key_exists('Year', $pub)) {
      $citation .= $pub['Year'];
    }
    if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages',$pub)) {
      $citation .= '; ';
    }
    if (array_key_exists('Volume', $pub)) {
      $citation .= $pub['Volume'];
    }
    if (array_key_exists('Issue', $pub)) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if (array_key_exists('Pages', $pub)) {
      if (array_key_exists('Volume', $pub)) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }
  //----------------------
  // Book
  //----------------------
  elseif ($pub_type == 'Book') {

  }
  //----------------------
  // Book Chapter
  //----------------------
  elseif ($pub_type == 'Book Chapter') {

  }
  //----------------------
  // Conference Proceedings
  //----------------------
  elseif ($pub_type == 'Conference Proceedings') {
    if (array_key_exists('Authors', $pub)) {
      $citation = $pub['Authors'] . '. ';
    }
    
    $citation .= $pub['Title'] .  '. ';
    if (array_key_exists('Conference Name', $pub)) {
      $citation .= $pub['Conference Name'] . '. ';
    }
    elseif (array_key_exists('Series Name', $pub)) {
      $citation .= $pub['Series Name'] . '. ';
    }
    elseif (array_key_exists('Series Abbreviation', $pub)) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if (array_key_exists('Publication Date', $pub)) {
      $citation .= $pub['Publication Date'];
    }
    elseif (array_key_exists('Year', $pub)) {
      $citation .= $pub['Year'];
    }
    if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages',$pub)) {
      $citation .= '; ';
    }
    if (array_key_exists('Volume', $pub)) {
      $citation .= $pub['Volume'];
    }
    if (array_key_exists('Issue', $pub)) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if (array_key_exists('Pages', $pub)) {
      if (array_key_exists('Volume', $pub)) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }

  return $citation;
}
