<?php

/**
 * This is the chado_pub node form callback. The arguments
 * are out of order from a typical form because it's a defined callback
 */

function chado_pub_form($node, $form_state) {
  $form = array();
  
  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing pub
  // 2) in the $form_state['values'] array which occurs on a failed validation or 
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit 
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $pub_id      = null;
  $title       = '';
  $pyear       = '';
  $uniquename  = '';
  $type_id     = '';
  $is_obsolete = '';
  
  // some of the fields in the pub table should show up in the properties 
  // form elements to make the form more seemless.  We will add them
  // to this array.
  $more_props = array();

  // if we are editing an existing node then the pub is already part of the node
  if (property_exists($node, 'pub')) {
    $pub = $node->pub;
    $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.title');
    $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.volumetitle');
    $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.uniquename');
    $pub_id = $pub->pub_id;
    
    $title       = $pub->title;
    $pyear       = $pub->pyear;
    $uniquename  = $pub->uniquename;
    $type_id     = $pub->type_id->cvterm_id;
    $is_obsolete = $pub->is_obsolete;
    
    // if the obsolete value is set by the database then it is in the form of
    // 't' or 'f', we need to convert to 1 or 0
    $is_obsolete = $is_obsolete == 't' ? 1 : $is_obsolete;
    $is_obsolete = $is_obsolete == 'f' ? 0 : $is_obsolete;
    
    // set the organism_id in the form
    $form['pub_id'] = array(
      '#type' => 'value',
      '#value' => $pub->pub_id,
    );
    
    // get fields from the pub table and convert them to properties. We will add these to the $more_props
    // array which gets passed in to the tripal_core_properties_form() API call further down
    if ($pub->volumetitle) {
      $cvterm = tripal_cv_get_cvterm_by_name('Volume Title', NULL, 'tripal_pub');
      $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->volumetitle);
    }
    if ($pub->volume) {
      $cvterm = tripal_cv_get_cvterm_by_name('Volume', NULL, 'tripal_pub');
      $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->volume);
    }
    if ($pub->series_name) {
      switch ($pub->type_id->name) {
        case 'Journal Article':
          $cvterm = tripal_cv_get_cvterm_by_name('Journal Name', NULL, 'tripal_pub');
          $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->series_name);
          break;
        case 'Conference Proceedings':
          $cvterm = tripal_cv_get_cvterm_by_name('Conference Name', NULL, 'tripal_pub');
          $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->series_name);
          break;
        default:
          $cvterm = tripal_cv_get_cvterm_by_name('Series Name', NULL, 'tripal_pub');
          $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->series_name);
      }
    }
    if ($pub->issue) {
      $cvterm = tripal_cv_get_cvterm_by_name('Issue', NULL, 'tripal_pub');
      $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->issue);
    }
    if ($pub->pages) {
      $cvterm = tripal_cv_get_cvterm_by_name('Pages', NULL, 'tripal_pub');
      $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->pages);
    }
    if ($pub->miniref) {
      // not sure what to do with this one
    }
    if ($pub->publisher) {
      $cvterm = tripal_cv_get_cvterm_by_name('Publisher', NULL, 'tripal_pub');
      $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->publisher);
    }
    if ($pub->pubplace) {
      $cvterm = tripal_cv_get_cvterm_by_name('Published Location', NULL, 'tripal_pub');
      $more_props[] = array('cvterm' => $cvterm, 'value' => $pub->pages);
    }
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $title        = $form_state['values']['pubtitle'];
    $pyear        = $form_state['values']['pyear'];
    $uniquename   = $form_state['values']['uniquename'];
    $type_id      = $form_state['values']['type_id'];
    $is_obsolete  = $form_state['values']['is_obsolete'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $title        = $form_state['input']['pubtitle'];
    $uniquename   = $form_state['input']['uniquename'];
    $type_id      = $form_state['input']['type_id'];
    $is_obsolete  = $form_state['input']['is_obsolete'];
  }
 
  // a drupal title can only be 255 characters, but the Chado title can be much longer.
  // we use the publication title as the drupal title, but we'll need to truncate it.
  $form['title'] = array(
    '#type' => 'hidden',
    '#value' => substr($title, 0, 255),
  );
  $form['pubtitle'] = array(
    '#type' => 'textarea',
    '#title' => t('Publication Title'),
    '#default_value' => $title,
    '#required' => TRUE,
  );
  // get the list of publication types.  In the Tripal publication
  // ontologies these are all grouped under the term 'Publication Type'
  // we want the default to be 'Journal Article'
  $sql = "
    SELECT
      CVTS.cvterm_id, CVTS.name
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id  = CVTO.cvterm_id
      INNER JOIN {cv}          ON CVTO.cv_id      = CV.cv_id
    WHERE
      CV.name = 'tripal_pub' AND CVTO.name = 'Publication Type' AND
      NOT CVTS.is_obsolete = 1
    ORDER BY CVTS.name ASC
  ";
  $results = chado_query($sql);
  $pub_types = array();
  while ($pub_type = $results->fetchObject()) {
    $pub_types[$pub_type->cvterm_id] = $pub_type->name;
    // if we don't have a default type then set the default to be 'Journal Article'
    if (strcmp($pub_type->name,"Journal Article") == 0 and !$type_id) {
      $type_id = $pub_type->cvterm_id;
    }
  }
  
  $form['type_id'] = array(
    '#type' => 'select',
    '#title' => t('Publication Type'),
    '#options' => $pub_types,
    '#required' => TRUE,
    '#default_value' => $type_id,
  );
  $form['pyear'] = array(
    '#type' => 'textfield',
    '#title' => t('Publication Year'),
    '#default_value' => $pyear,
    '#required' => TRUE,
    '#size' => 5,
    '#description' => t('Enter the year of publication. Also, if available, please add a <b>Publication Date</b> property to specify the full date of publication.'),
  );
  $form['uniquename'] = array(
    '#type' => 'textarea',
    '#title' => t('Citation'),
    '#default_value' => $uniquename,
    '#description' => t('All publications must have a unique citation. 
      <b>Please enter the full citation for this publication or leave blank and one will be generated 
      automatically if possible</b>.  For PubMed style citations list 
      the last name of the author followed by initials. Each author should be separated by a comma. Next comes 
      the title, followed by the series title (e.g. journal name), publication date (4 digit year, 3 character Month, day), volume, issue and page numbers. You may also use HTML to provide a link in the citation.  
      Below is an example: <pre>Medeiros PM, Ladio AH, Santos AM, Albuquerque UP. <a href="http://www.ncbi.nlm.nih.gov/pubmed/23462414" target="_blank">Does the selection of medicinal plants by Brazilian local populations 
        suffer taxonomic influence?</a> J Ethnopharmacol. 2013 Apr 19; 146(3):842-52.</pre>'),
  );
  $form['is_obsolete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Is Obsolete? (Check for Yes)'),
    '#default_value' => $is_obsolete,
  );
 
  // get publication properties list
  $properties_select = array();
  $properties_select[] = 'Select a Property';
  $sql = "
    SELECT
      DISTINCT CVTS.cvterm_id, CVTS.name, CVTS.definition
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id  = CVTO.cvterm_id
      INNER JOIN {cv}          ON CVTO.cv_id      = CV.cv_id
    WHERE CV.name = 'tripal_pub' and
      (CVTO.name = 'Publication Details' OR CVTS.name = 'Publication Type') AND
      NOT CVTS.is_obsolete = 1
    ORDER BY CVTS.name ASC
  ";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    // add all properties except the Citation. That property is set via the uniquename field
    if ($prop->name != 'Citation') {
      $properties[$prop->cvterm_id] = $prop->name;
    }
  }
  
  // add in the properties fields. The 'Citation' term is special because it serves
  // both as a property and as the uniquename for the publiation table so we exclude it
  // as it shouldn't be selected as a property
  $exclude = array("Citation");
  $instructions = '';
  tripal_core_properties_form($form, $form_state, 'pubprop', 'pub_id', 'tripal_pub',
    $properties, $pub_id, $exclude, $more_props, $instructions);

  return $form;

}
/*
 *
 */
function chado_pub_validate($node, $form, &$form_state) {

  // get the submitted values
  $title        = trim($node->pubtitle);
  $pyear        = trim($node->pyear);
  $uniquename   = trim($node->uniquename);
  $is_obsolete  = $node->is_obsolete;
  $type_id      = $node->type_id;  

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a pub_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'pub_id') and $node->pub_id != 0) {
    return;
  }
  
  $pub = array();
  
  // make sure the year is four digits
  if(!preg_match('/^\d{4}$/', $pyear)){
    form_set_error('pyear', t('The publication year should be a 4 digit year.'));
    return;
  }

  // get the type of publication
  $values = array('cvterm_id' => $type_id);
  $options = array('statement_name' => 'sel_pub_ty');
  $cvterm = tripal_core_chado_select('cvterm', array('name'), $values, $options);
  if (count($cvterm) == 0) {
    $message = t('Invalid publication type.');
    form_set_error('type_id', $message);
    return;
  }  

  // get the media name looking at the properties
  $series_name = '';
  foreach ($node as $element => $value) {
    // if this is an existing property (either previously in the database or
    // added via AHAH/AJAX callback)
    if (preg_match('/^prop_value-(\d+)-(\d+)$/', $element, $matches)) {
      $prop_type_id = $matches[1];
      $prop_type = tripal_cv_get_cvterm_by_id($prop_type_id);
      if($prop_type->name == 'Conference Name' or $prop_type->name == 'Journal Name') {
        $series_name = $value;
      }
      if($prop_type->name == 'Citation') {
        $uniquename = $value;
      }
      $pub[$prop_type->name] = $value;
    }
    // if this is a new property (added by this submit of the form)
    elseif ($element == 'new_id') {       
      $prop_type = tripal_cv_get_cvterm_by_id($value);
      if($prop_type->name == 'Conference Name' or $prop_type->name == 'Journal Name') {
        $series_name = $node->new_value;
      }
      if($prop_type->name == 'Citation') {
        $uniquename = $node->new_value;
      }
      $pub[$prop_type->name] = $node->new_value;
    }
  }
  // if the citation is missing then try to generate one
  if (!$uniquename) {
    $pub['Title'] = $title;
    $pub['Publication Type'][0] = $cvterm[0]->name;
    $pub['Year'] = $pyear;
    $uniquename = tripal_pub_create_citation($pub);
    if (!$uniquename) {
      form_set_error('uniquename', "Cannot automatically generate a citation for this publication type. Please add one manually.");
    }
  }

  $skip_duplicate_check = 0;

  // if this publication is a Patent then skip the validation below.  Patents can have the title
  // name and year but be different
  if (strcmp($cvterm[0]->name,'Patent') == 0) {
    $skip_duplicate_check = 1;
  }

  // Validating for an update
  if (!is_null($node->nid)) { 
    
    $pub_id = $node->pub_id;
    
    // first get the original title, type and year before it was changed
    $values = array('pub_id' => $pub_id);
    $columns = array('title', 'pyear', 'type_id', 'series_name');
    $options = array('statement_name' => 'sel_pub_id');
    $pub = tripal_core_chado_select('pub', $columns, $values, $options);

    // if the title, type,  year or series_name have changed then check the pub
    // to see if it is a duplicate of another
    if((strcmp(strtolower($pub[0]->title), strtolower($title)) == 0) and
       (strcmp(strtolower($pub[0]->series_name), strtolower($series_name)) == 0) and
       ($pub[0]->type_id == $type_id) and
       ($pub[0]->pyear == $pyear)) {
      $skip_duplicate_check = 1;
    }

    // check to see if a duplicate publication already exists
    if (!$skip_duplicate_check) {
      chado_pub_validate_check_duplicate($title, $pyear, $series_name, $cvterm, $pub_id); 
    }
    chado_pub_validate_check_uniquename($uniquename, $pub_id);
  }
  // Validating for an insert
  else {
    chado_pub_validate_check_duplicate($title, $pyear, $series_name, $cvterm);
    chado_pub_validate_check_uniquename($uniquename);
  }
}
/**
 * 
 * @param unknown $uniquename
 */
function chado_pub_validate_check_uniquename($uniquename, $pub_id = NULL) {
  
  $results = tripal_pub_get_pub_by_uniquename($uniquename);
  // make sure we don't capture our pub_id in the list (remove it)
  foreach ($results as $index => $found_pub_id) {
    if($found_pub_id == $pub_id){
      unset($results[$index]);
    }
  }
  if (count($results) > 0) {
    $message = t('A publication with this unique citation already exists.');
    form_set_error('uniquename', $message);
  }
}
/**
 * 
 */
function chado_pub_validate_check_duplicate($title, $pyear, $series_name, $cvterm, $pub_id = NULL) {
  
  // make sure the publication is unique using the prefereed import duplication check
  $import_dups_check = variable_get('tripal_pub_import_duplicate_check', 'title_year_media');
  switch ($import_dups_check) {
    case 'title_year':
      $results = tripal_pub_get_pubs_by_title_type_pyear_series($title, NULL, $pyear, NULL);
      // make sure we don't capture our pub_id in the list (remove it)
      foreach ($results as $index => $found_pub_id) {
        if($found_pub_id == $pub_id){
          unset($results[$index]);
        }
      }
      if (count($results) > 0) {
        $message = t('A publication with this title and publication year, already exists.');
        form_set_error('pyear', $message);
      }
      break;
    case 'title_year_type':
      $results = tripal_pub_get_pubs_by_title_type_pyear_series($title, $cvterm[0]->name, $pyear, NULL);
  
      // make sure we don't capture our pub_id in the list (remove it)
      foreach ($results as $index => $found_pub_id) {
        if($found_pub_id == $pub_id){
          unset($results[$index]);
        }
      }
      if (count($results) > 0) {
        $message = t('A publication with this title, type and publication year, already exists.');
        form_set_error('pyear', $message);
      }
      break;
    case 'title_year_media':
      $results = tripal_pub_get_pubs_by_title_type_pyear_series($title, NULL, $pyear, $series_name);
  
      // make sure we don't capture our pub_id in the list (remove it)
      foreach ($results as $index => $found_pub_id) {
        if($found_pub_id == $pub_id){
          unset($results[$index]);
        }
      }
      if (count($results) > 0) {
        $message = t('A publication with this title, media name (e.g. Journal Name) and publication year, already exists.');
        form_set_error('pyear', $message);
      }
      break;
  }
}

