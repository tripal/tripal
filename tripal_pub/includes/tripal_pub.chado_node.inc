<?php

/**
 * Implementation of hook_tripal_pub_node_info().
 *
 * This node_info, is a simple node that describes the functionallity of the module.
 *
 */
function tripal_pub_node_info() {

  return array(
    'chado_pub' => array(
      'name' => t('Publication'),
      'base' => 'chado_pub',
      'description' => t('A publication from the Chado database'),
      'title_label' => t('Article Title'),
      'body_label' => t('Abstract'),
      'has_title' => TRUE,
      'has_body' => FALSE,
      'chado_node_api' => array(
        'base_table' => 'pub',
        'hook_prefix' => 'chado_pub',
        'record_type_title' => array(
          'singular' => t('Publication'),
          'plural' => t('Publications')
        ),
        'sync_filters' => array(
          'type_id' => FALSE,
          'organism_id' => FALSE
        ),
      ),
    ),
  );
}

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
  // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
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

  // Properties Form
  // ----------------------------------
  // Need to pass in our own select_options since we use cvtermpath to filter ours
  $select_options = array();
  $select_options[] = 'Select a Property';
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
      $select_options[$prop->cvterm_id] = $prop->name;
    }
  }

  $details = array(
    'property_table' => 'pubprop',
    'base_foreign_key' => 'pub_id',
    'base_key_value' => $pub_id,
    'cv_name' => 'tripal_pub',
    'select_options' => $select_options,
  );
  chado_node_properties_form($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  // We want to use the contact_relationship_types cv if there are any terms available
  // and if not, to default to the relationship ontology
  $cv_result = tripal_core_chado_select('cv',array('cv_id'),array('name' => 'pub_relationship_types'));
  $cv_id = $cv_result[0]->cv_id;
  $select_options = tripal_cv_get_cvterm_options($cv_id);
  if (empty($select_options)) {
    $cv_result = tripal_core_chado_select('cv',array('cv_id'),array('name' => 'relationship'));
    $cv_id = $cv_result[0]->cv_id;
    $select_options = tripal_cv_get_cvterm_options($cv_id);
  }
  // D7 @TODO: tell tripal admin's about this

  $details = array(
    'relationship_table' => 'pub_relationship', // the name of the _relationship table
    'base_table' => 'pub',                      // the name of your chado base table
    'base_foreign_key' => 'pub_id',             // the name of the key in your base chado table
    'base_key_value' => $pub_id,                // the value of example_id for this record
    'nodetype' => 'pub',                        // the human-readable name of your node type
    'cv_name' => 'pub_relationship_types',      // the cv.name of the cv governing example_relationship.type_id
    'base_name_field' => 'uniquename',          // the base table field you want to be used as the name
    'select_options' => $select_options
  );
  // Adds the form elements to your current form
  chado_node_relationships_form($form, $form_state, $details);
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


/**
 * Implement hook_access().
 *
 * This hook allows node modules to limit access to the node types they define.
 *
 *  @param $node
 *  The node on which the operation is to be performed, or, if it does not yet exist, the
 *  type of node to be created
 *
 *  @param $op
 *  The operation to be performed
 *
 *  @param $account
 *  A user object representing the user for whom the operation is to be performed
 *
 *  @return
 *  If the permission for the specified operation is not set then return FALSE. If the
 *  permission is set then return NULL as this allows other modules to disable
 *  access.  The only exception is when the $op == 'create'.  We will always
 *  return TRUE if the permission is set.
 *
 */
function chado_pub_node_access($node, $op, $account ) {
  if ($op == 'create') {
    if (!user_access('create chado_pub content', $account)) {
      return FALSE;
    }
    return TRUE;
  }

  if ($op == 'update') {
    if (!user_access('edit chado_pub content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'delete') {
    if (!user_access('delete chado_pub content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'view') {
    if (!user_access('access chado_pub content', $account)) {
      return FALSE;
    }
  }
  return NULL;
}





/**
 * Implementation of tripal_pub_insert().
 *
 * This function inserts user entered information pertaining to the Publication instance into the
 * 'pubauthor', 'pubprop', 'chado_pub', 'pub' talble of the database.
 *
 *  @parm $node
 *    Then node which contains the information stored within the node-ID
 *
 *
 */
function chado_pub_insert($node) {

  $title        = trim($node->pubtitle);
  $pyear        = trim($node->pyear);
  $uniquename   = trim($node->uniquename);
  $is_obsolete  = $node->is_obsolete;
  $type_id      = $node->type_id;

  // we need an array suitable for the tripal_pub_create_citation() function
  // to automatically generate a citation if a uniquename doesn't already exist
  $pub_arr = array();

  // if there is an pub_id in the $node object then this must be a sync so
  // we can skip adding the pub as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'pub_id')) {

    $properties = array(); // stores all of the properties we need to add
    $cross_refs = array(); // stores any cross references for this publication

    // get the properties from the form
    $properties = chado_node_properties_form_retreive($node);

    // get the list of properties for easy lookup (without doing lots of database queries
    $properties_list = array();
    $sql = "
      SELECT CVTS.cvterm_id, CVTS.name, CVTS.definition
      FROM {cvtermpath} CVTP
        INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
        INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
        INNER JOIN {cv} ON CVTO.cv_id = CV.cv_id
      WHERE CV.name = 'tripal_pub' and CVTO.name = 'Publication Details' and
        NOT CVTS.is_obsolete = 1
      ORDER BY CVTS.name ASC
    ";
    $prop_types = chado_query($sql);
    while ($prop = $prop_types->fetchObject()) {
      $properties_list[$prop->cvterm_id] = $prop->name;
      // The 'Citation' term is special because it serves
      // both as a property and as the uniquename for the
      // pub and we want it stored in both the pub table and the pubprop table
      if ($prop->name == 'Citation') {
        $citation_id = $prop->cvterm_id;
        if (!empty($node->uniquename)) {
          $properties[$citation_id] = $node->uniquename;
        }
      }
    }

    // iterate through all of the properties and remove those that really are
    // part of the pub table fields
    $volume = '';
    $volumetitle = '';
    $issue = '';
    $pages = '';
    $publisher = '';
    $series_name = '';
    $pubplace = '';
    $miniref = '';
    $cross_refs = array();
    foreach ($properties as $type_id => $element) {
      $value = trim($element[0]);
      $name = $properties_list[$type_id];

      // populate our $pub_array for building a citation
      $pub_arr[$name] = $value;

      // remove properties that are stored in the pub table
      if ($name == "Volume") {
        $volume = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Volume Title") {
        $volumetitle = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Issue") {
        $issue = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Pages") {
        $pages = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Publisher") {
        $publisher = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Series Name" or $name == "Journal Name" or $name == "Conference Name") {
        $series_name = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Journal Country" or $name == "Published Location") {
        $pubplace = $value;
        // allow this property to go into the pubprop table so we don't loose info
        // so don't unset it. But it will also go into the pub.pubplace field
      }
      elseif ($name == "Publication Dbxref") {
        // we will add the cross-references to the pub_dbxref table
        // but we also want to keep the property in the pubprop table so don't unset it
        $cross_refs[] = $value;
      }
    }
    // generate a citation for this pub if one doesn't already exist
    if (!$node->uniquename and !array_key_exists($citation_id, $properties)) {
      $pub_type = tripal_cv_get_cvterm_by_id($node->type_id);
      $pub_arr['Title'] = $node->pubtitle;
      $pub_arr['Publication Type'][0] = $pub_type->name;
      $pub_arr['Year'] = $node->pyear;
      $node->uniquename = tripal_pub_create_citation($pub_arr);
      $properties[$citation_id][0] = $node->uniquename;
    }

    // insert the pub record
    $values = array(
      'title'       => $node->pubtitle,
      'series_name' => substr($series_name, 0, 255),
      'type_id'     => $node->type_id,
      'pyear'       => $node->pyear,
      'is_obsolete' => $node->is_obsolete ? 'true' : 'false',
      'uniquename'  => $node->uniquename,
      'volumetitle' => $volumetitle,
      'volume'      => $volume,
      'issue'       => $issue,
      'pages'       => $pages,
      'miniref'     => substr($miniref, 0, 255),
      'publisher'   => substr($publisher, 0, 255),
      'pubplace'    => substr($pubplace, 0, 255),
    );
    $pub = tripal_core_chado_insert('pub', $values);
    if (!$pub) {
      drupal_set_message("Error inserting publication", "error");
      watchdog('tripal_pub', "Error inserting publication", array(), WATCHDOG_ERROR);
      return;
    }
    $pub_id = $pub['pub_id'];

    // now add in the properties
    // Only adds in those not used in the pub record
    $details = array(
      'property_table' => 'pubprop',
      'base_table' => 'pub',
      'foreignkey_name' => 'pub_id',
      'foreignkey_value' => $pub_id
    );
    chado_node_properties_form_update_properties($node, $details, $properties);

    // * Relationships Form *
    $details = array(
      'relationship_table' => 'pub_relationship',  // name of the _relationship table
      'foreignkey_value' => $pub_id                // value of the pub_id key
    );
    chado_node_relationships_form_update_relationships($node, $details);

    // add in any database cross-references
    foreach ($cross_refs as $index => $ref) {
      $pub_dbxref = tripal_pub_add_pub_dbxref($pub['pub_id'], trim($ref));
      if (!$pub_dbxref) {
        drupal_set_message("Error cannot add publication cross reference: $ref", "error");
        watchdog('tripal_pub', "Error cannot add publication cross reference: %ref",
        array('%ref' => $ref), WATCHDOG_ERROR);
      }
    }
  }
  else {
    $pub_id = $node->pub_id;
  }

  // Make sure the entry for this pub doesn't already exist in the
  // chado_pub table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_for_node('pub', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->pub_id = $pub_id;
    drupal_write_record('chado_pub', $record);
  }

}

/*
 *
* Implements hook_update
*
* The purpose of the function is to allow the module to take action when an edited node is being
* updated. It updates any name changes to the database tables that werec reated upon registering a Publication.
* As well, the database will be changed, so the user changed information will be saved to the database.
*
* @param $node
*   The node being updated
*
* @ingroup tripal_pub
*/
function chado_pub_update($node) {
  $title        = trim($node->pubtitle);
  $pyear        = trim($node->pyear);
  $uniquename   = trim($node->uniquename);
  $is_obsolete  = $node->is_obsolete;
  $type_id      = $node->type_id;

  // we need an array suitable for the tripal_pub_create_citation() function
  // to automatically generate a citation if a uniquename doesn't already exist
  $pub_arr = array();

  // get the publication ID for this publication
  $pub_id = chado_get_id_for_node('pub', $node->nid) ;

  $properties = array(); // stores all of the properties we need to add
  $cross_refs = array(); // stores any cross references for this publication

  // get the properties from the form
  $properties = chado_node_properties_form_retreive($node);

  // get the list of properties for easy lookup (without doing lots of database queries
  $properties_list = array();
  $sql = "
    SELECT DISTINCT CVTS.cvterm_id, CVTS.name, CVTS.definition
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
      INNER JOIN {cv} ON CVTO.cv_id = CV.cv_id
    WHERE CV.name = 'tripal_pub' and
      (CVTO.name = 'Publication Details' or CVTS.name = 'Publication Type') and
      NOT CVTS.is_obsolete = 1
    ORDER BY CVTS.name ASC
  ";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    $properties_list[$prop->cvterm_id] = $prop->name;
    // The 'Citation' term is special because it serves
    // both as a property and as the uniquename for the
    // pub and we want it stored in both the pub table and the pubprop table
    if ($prop->name == 'Citation') {
      $citation_id = $prop->cvterm_id;
      if (!empty($node->uniquename)) {
        $properties[$citation_id][0] = $node->uniquename;
      }
    }
  }

  // iterate through all of the properties and remove those that really are
  // part of the pub table fields
  $volume = '';
  $volumetitle = '';
  $issue = '';
  $pages = '';
  $publisher = '';
  $series_name = '';
  $pubplace = '';
  $miniref = '';
  $cross_refs = array();
  foreach ($properties as $type_id => $element) {
    foreach ($element as $index => $value) {
      $name = $properties_list[$type_id];
      // populate our $pub_array for building a citation
      $pub_arr[$name] = $value;

      // remove properties that are stored in the pub table
      if ($name == "Volume") {
        $volume = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Volume Title") {
        $volumetitle = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Issue") {
        $issue = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Pages") {
        $pages = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Publisher") {
        $publisher = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Journal Name" or $name == "Conference Name") {
        $series_name = $value;
        unset($properties[$type_id]);
      }
      elseif ($name == "Journal Country" or $name == "Published Location") {
        $pubplace = $value;
        // allow this property to go into the pubprop table so we don't loose info
        // so don't unset it. But it will also go into the pub.pubplace field
      }
      elseif ($name == "Publication Dbxref") {
        // we will add the cross-references to the pub_dbxref table
        // but we also want to keep the property in the pubprop table so don't unset it
        $cross_refs[] = $value;
      }
    }
  }
  // generate a citation for this pub if one doesn't already exist
  if (!$node->uniquename) {
    $pub_type = tripal_cv_get_cvterm_by_id($node->type_id);
    $pub_arr['Title'] = $node->pubtitle;
    $pub_arr['Publication Type'][0] = $pub_type->name;
    $pub_arr['Year'] = $node->pyear;
    $node->uniquename = tripal_pub_create_citation($pub_arr);
    $properties[$citation_id][0] = $node->uniquename;
  }

  // update the pub record
  $match = array(
    'pub_id' => $pub_id,
  );
  $values = array(
    'title'       => $node->pubtitle,
    'type_id'     => $node->type_id,
    'pyear'       => $node->pyear,
    'is_obsolete' => $node->is_obsolete ? 'true' : 'false',
    'uniquename'  => $node->uniquename,
    'series_name' => substr($series_name, 0, 255),
    'volumetitle' => $volumetitle,
    'volume'      => $volume,
    'issue'       => $issue,
    'pages'       => $pages,
    'miniref'     => substr($miniref, 0, 255),
    'publisher'   => substr($publisher, 0, 255),
    'pubplace'    => substr($pubplace, 0, 255),
  );
  $status = tripal_core_chado_update('pub', $match, $values);
  if (!$status) {
    drupal_set_message("Error updating publication", "error");
    watchdog('tripal_pub', "Error updating publication", array(), WATCHDOG_ERROR);
    return;
  }

  // now add in the properties by first removing any the publication
  // already has and adding the ones we have
  $details = array(
    'property_table' => 'pubprop',
    'base_table' => 'pub',
    'foreignkey_name' => 'pub_id',
    'foreignkey_value' => $pub_id
  );
  chado_node_properties_form_update_properties($node, $details, $properties);

  // * Relationships Form *
  $details = array(
    'relationship_table' => 'pub_relationship',  // name of the _relationship table
    'foreignkey_value' => $pub_id                // value of the pub_id key
  );
  chado_node_relationships_form_update_relationships($node, $details);

  // add in any database cross-references after first removing
  tripal_core_chado_delete('pub_dbxref', array('pub_id' => $pub_id));
  foreach ($cross_refs as $index => $ref) {
    $pub_dbxref = tripal_pub_add_pub_dbxref($pub_id, trim($ref));
    if (!$pub_dbxref) {
      drupal_set_message("Error cannot add publication cross reference: $ref", "error");
      watchdog('tripal_pub', "Error cannot add publication cross reference: %ref",
      array('%ref' => $ref), WATCHDOG_ERROR);
    }
  }
}


/**
 * Implementation of tripal_pub_load().
 *
 *
 * @param $node
 *   The node that is to be accessed from the database
 *
 * @return $node
 *   The node with the information to be loaded into the database
 *
 */
function chado_pub_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // find the pub and add in the details
    $pub_id = chado_get_id_for_node('pub', $nid);

    // get the pub
    $values = array('pub_id' => $pub_id);
    $pub = tripal_core_generate_chado_var('pub', $values);

    // expand the 'text' fields as those aren't included by default
    // and they really shouldn't be so large to cause problems
    $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.title');
    $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.volumetitle');
    $pub = tripal_core_expand_chado_vars($pub, 'field', 'pub.uniquename');

    // set the URL path
    $nodes[$nid]->path = "pub/$pub_id";
    $nodes[$nid]->pub = $pub;
  }
}

/**
 * Implementation of tripal_pub_delete().
 *
 * This function takes a node and if the delete button has been chosen by the user, the publication
 * and it's details will be removed.Following,given the node-ID, the instance will be deleted from
 * the 'chado_pub' table.
 *
 *  @parm $node
 *    Then node which contains the information stored within the node-ID
 *
 */
function chado_pub_delete(&$node) {

  $pub_id = chado_get_id_for_node('pub', $node->nid);

  // if we don't have a pub id for this node then this isn't a node of
  // type chado_pub or the entry in the chado_pub table was lost.
  if (!$pub_id) {
    return;
  }

  // Remove data from {chado_pub}, {node} and {node_revision} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_pub} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from pub and pubprop tables of chado database as well
  chado_query("DELETE FROM {pubprop} WHERE pub_id = :pub_id", array(':pub_id' => $pub_id));
  chado_query("DELETE FROM {pub} WHERE pub_id = :pub_id", array(':pub_id' => $pub_id));
}



/**
 *
 * @ingroup tripal_feature
 */
function tripal_pub_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
  	case 'chado_pub':
  	  // Show feature browser and counts
  	  if ($view_mode == 'full') {
  	    $node->content['tripal_pub_authors'] = array(
  	      '#value' => theme('tripal_pub_authors', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_base'] = array(
  	      '#value' => theme('tripal_pub_base', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_featuremaps'] = array(
  	      '#value' => theme('tripal_pub_featuremaps', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_features'] = array(
  	      '#value' => theme('tripal_pub_features', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_libraries'] = array(
  	      '#value' => theme('tripal_pub_libraries', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_projects'] = array(
  	      '#value' => theme('tripal_pub_projects', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_properties'] = array(
  	      '#value' => theme('tripal_pub_properties', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_references'] = array(
  	      '#value' => theme('tripal_pub_references', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_relationships'] = array(
  	      '#value' => theme('tripal_pub_relationships', array('node' => $node)),
  	    );
  	    $node->content['tripal_pub_stocks'] = array(
  	      '#value' => theme('tripal_pub_stocks', array('node' => $node)),
  	    );
  	  }
  	  if ($view_mode == 'teaser') {
  	    $node->content['tripal_pub_teaser'] = array(
  	      '#value' => theme('tripal_pub_teaser', array('node' => $node)),
  	    );
  	  }
  	  break;
  }
}
/**
 *
 * @param $node
 */
function tripal_pub_node_insert($node) {
  // we want the publications to always have a URL of http://[base url]/pub/[pub id]
  // where [pub id] is the Chado publication ID.  This will allow for easy linking
  // into the publication without needing to know the node.  Of course if you know the
  // node that will still work too (e.g. http://[base url]/node/[node id]
  // so the nodeapi function ensures that the URL path is set after insert or update
  // of the node and when the node is loaded if it hasn't yet been set.
  if ($node->type == 'chado_pub') {
    $pub_id = chado_get_id_for_node('pub', $node->nid);
    tripal_pub_set_pub_url($node, $pub_id);
  }
}
/**
 *
 * @param $node
 * @param $types
 */
function tripal_pub_node_load($nodes, $types) {

  // we want the publications to always have a URL of http://[base url]/pub/[pub id]
  // where [pub id] is the Chado publication ID.  This will allow for easy linking
  // into the publication without needing to know the node.  Of course if you know the
  // node that will still work too (e.g. http://[base url]/node/[node id]
  // so the nodeapi function ensures that the URL path is set after insert or update
  // of the node and when the node is loaded if it hasn't yet been set.
  if (count(array_intersect(array('chado_pub'), $types))) {
    foreach ($nodes as $nid => $node) {
      if ($node->type == 'chado_pub' and !property_exists($node, 'path')) {
        $pub_id = chado_get_id_for_node('pub', $node->nid);
        $path = tripal_pub_set_pub_url($node, $pub_id);
      }
    }
  }
}
/**
 *
 * @param $node
 */
function tripal_pub_node_update($node) {
  // we want the publications to always have a URL of http://[base url]/pub/[pub id]
  // where [pub id] is the Chado publication ID.  This will allow for easy linking
  // into the publication without needing to know the node.  Of course if you know the
  // node that will still work too (e.g. http://[base url]/node/[node id]
  // so the nodeapi function ensures that the URL path is set after insert or update
  // of the node and when the node is loaded if it hasn't yet been set.
  if ($node->type == 'chado_pub') {
    $pub_id = chado_get_id_for_node('pub', $node->nid);
    tripal_pub_set_pub_url($node, $pub_id);
  }
}