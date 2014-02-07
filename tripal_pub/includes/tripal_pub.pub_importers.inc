<?php
/**
 * A function to generate a table containing the list of publication importers
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importers_list() {
  // clear out the session variable when we view the list.
  unset($_SESSION['tripal_pub_import']);
  
  $header = array('', 'Importer Name', 'Database', 'Search String', 'Disabled', 'Create Contact', '');
  $rows = array();
  $importers = db_query("SELECT * FROM {tripal_pub_import} ORDER BY name");

  while ($importer = $importers->fetchObject()) {
    $criteria = unserialize($importer->criteria);
    $num_criteria = $criteria['num_criteria'];
    $criteria_str = '';
    for ($i = 1; $i <= $num_criteria; $i++) {
      $search_terms = $criteria['criteria'][$i]['search_terms'];
      $scope = $criteria['criteria'][$i]['scope'];
      $is_phrase = $criteria['criteria'][$i]['is_phrase'];
      $operation = $criteria['criteria'][$i]['operation'];
      $criteria_str .= "$operation ($scope: $search_terms) ";
    }

    $rows[] = array(
      array(
        'data' => l(t('Edit/Test'), "admin/tripal/chado/tripal_pub/import/edit/$importer->pub_import_id") . '<br>' .
                  l(t('Import Pubs'), "admin/tripal/chado/tripal_pub/import/submit/$importer->pub_import_id"),
        'nowrap' => 'nowrap'
      ),
      $importer->name,
      $criteria['remote_db'],
      $criteria_str,
      $importer->disabled ? 'Yes' : 'No',
      $importer->do_contact ? 'Yes' : 'No',
      l(t('Delete'), "admin/tripal/chado/tripal_pub/import/delete/$importer->pub_import_id"),
    );
  }


  $page  = "<ul class='action-links'>";
  $page .= '  <li>' . l('New Importer', 'admin/tripal/chado/tripal_pub/import/new') . '</li>';
  $page .= '</ul>';
  
  $page .= '<p>' . t(
    "A publication importer is used to create a set of search criteria that can be used
     to query a remote database, find publications that match the specified criteria 
     and then import those publications into the Chado database. An example use case would
     be to peridocially add new publications to this Tripal site that have appeared in PubMed
     in the last 30 days.  You can import publications in one of two ways:
     <ol>
      <li>Create a new importer by clicking the 'New Importer' link above, and after saving it should appear in the list below.  Click the
          link labeled 'Import Pubs' to schedule a job to import the publications</li>
      <li>The first method only performs the import once.  However, you can schedule the 
          importer to run peridically by adding a cron job. See the " . 
     l("Pub Module help instructions", "admin/tripal/chado/tripal_pub/help") . " to learn how to 
     set the importers to run automatically.") . '</li>
     </ol><br>';

  $page .= theme('table', array('header' => $header, 'rows' => $rows));
  
  return $page;
}

/**
 * 
 * @param  action
 * @param $pub_import_id
 * 
 * 
 */
function tripal_pub_importer_setup_page($action = 'new', $pub_import_id = NULL) {
  global $base_path;
  
  // make sure the tripal_pub and tripal_contact ontologies are loaded
  $values = array('name' => 'tripal_pub');
  $tpub_cv = chado_select_record('cv', array('cv_id'), $values);
  if (count($tpub_cv) == 0) {
    drupal_set_message(t('Before importing publications you must first ') . l(t('load the Tripal Pub Ontology'), 'admin/tripal/tripal_cv/obo_loader'), 'error');
  }
  $values = array('name' => 'tripal_contact');
  $tpub_cv = chado_select_record('cv', array('cv_id'), $values);
  if (count($tpub_cv) == 0) {
    drupal_set_message(t('If you want to create contact pages for authors, you must first ') . l(t('load the Tripal Contact Ontology'), 'admin/tripal/tripal_cv/obo_loader'), 'error');
  }
  
  if(!extension_loaded ('yaz')){
    drupal_set_message(t('<b>Note:</b> In order to create an importer using the USDA National Agricultural Library (AGL) you must install the yaz libraries. See the ') . l(t('Pub Module help page'), 'admin/tripal/chado/tripal_pub/help') . ' for assistance.  If you do not want to use AGL you can ignore this warning.', 'warning');
  }

  // generate the search form
  $form = drupal_get_form('tripal_pub_importer_setup_form',  $pub_import_id, $action);

  $output = l("Return to publication importers list", "admin/tripal/chado/tripal_pub/import_list");
  $output .= drupal_render($form);

  // retrieve any results
  if (array_key_exists('tripal_pub_import', $_SESSION)) {
    $remote_db = $_SESSION['tripal_pub_import']['remote_db'];
    $num_criteria = $_SESSION['tripal_pub_import']['num_criteria'];
    $days = $_SESSION['tripal_pub_import']['days'];
  
    $search_array = array();
    $search_array['remote_db'] = $remote_db;
    $search_array['num_criteria'] = $num_criteria;
    $search_array['days'] = $days;
    for ($i = 1; $i <= $num_criteria; $i++) {
      $search_array['criteria'][$i]['search_terms'] = $_SESSION['tripal_pub_import']['criteria'][$i]['search_terms'];
      $search_array['criteria'][$i]['scope']        = $_SESSION['tripal_pub_import']['criteria'][$i]['scope'];
      $search_array['criteria'][$i]['is_phrase']    = $_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase'];
      $search_array['criteria'][$i]['operation']    = $_SESSION['tripal_pub_import']['criteria'][$i]['operation'];
    }
  
    // if the form has been submitted with the 'test' button then get the results
    if ($_SESSION['tripal_pub_import']['perform_search']) {
      
      $limit = 25;
      
      // get the list of publications from the remote database using the search criteria.
      $page = isset($_GET['page']) ? $_GET['page'] : '0';
      $results = tripal_pub_get_remote_search_results($remote_db, $search_array, $limit, $page);
      $total_records = $results['total_records'];
      $search_str    = $results['search_str'];
      $pubs          = $results['pubs'];
      
      // iterate through the results and construct the table displaying the publications
      $rows = array();
      $i = $page * $limit + 1;
      if (count($pubs) > 0) {
        foreach ($pubs as $pub) {
          $citation = htmlspecialchars($pub['Citation']);
          $raw_link = '';
          if($pub['Publication Dbxref']) {
            $raw_link = l('raw', 'admin/tripal/chado/tripal_pub/import/raw/' . $pub['Publication Dbxref'], array('attributes' => array('target' => '_blank')));
          }
          $rows[] = array(
            number_format($i),
            $citation,
            $raw_link,
          );
          $i++;
        }
      }
      
      if (count($rows) == 0) {
        $rows[] = array(
          array(
            'data' => 'No results found',
            'colspan' => 3,
          ),
        );
      }
      
      $headers = array('', 'Publication', 'Raw Results');
      $table = array(
        'header' => $headers,
        'rows' => $rows,
        'attributes' => array(
          'id' => 'tripal_pub-importer-test',
        ),
        'sticky' => FALSE,
        'caption' => '',
        'colgroups' => array(),
        'empty' => '',
      );
      // once we have our table array structure defined, we call Drupal's theme_table()
      // function to generate the table.
      $table = theme_table($table);
      
      // generate the pager
      pager_default_initialize($total_records, $limit);
      $pager = array(
        'tags' => array(),
        'element' => 0,
        'parameters' => array(),
        'quantity' => $limit,
      );
      $pager = theme_pager($pager);
      
      // because this is an ajax callback, the theme_pager will set the URL to be
      // "system/ajax", so we need to reset that
      $pager = str_replace($base_path . "system/ajax", "", $pager) ;
      
      // join all to form the results
      $total_pages = (int) ($total_records / $limit) + 1;
      $page = isset($_GET['page']) ? $_GET['page'] : '0';
      $output .= "$pager<br><b>Found " . number_format($total_records) . " publications. Page " . ($page + 1) . " of $total_pages.</b> " .
      "<br>$remote_db Search String: $search_str $table<br>$pager";
    }
  }
  return $output;
}

/**
 * Purpose: Provides the form to search pubmed
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importer_setup_form($form, &$form_state = NULL, $pub_import_id = NULL, $action = 'new') {

  // Default values can come in the following ways:
  //
  // 1) as elements of the $pub_importer object.  This occurs when editing an existing importer 
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
    
  // Set the default values. If the pub_import_id isn't already defined by the form values
  // and one is provided then look it up in the database
  $criteria = NULL;
  $remote_db = '';
  $days = '';
  $disabled = '';
  $do_contact = '';
  $num_criteria = 1;
  $loader_name = '';
  
  // if this is an edit the we are pulling an import object from the database
  if ($action == "edit") {
    $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :pub_import_id";
    $importer = db_query($sql, array(':pub_import_id' => $pub_import_id))->fetchObject();

    $criteria       = unserialize($importer->criteria);
    $remote_db      = $criteria['remote_db'];
    $days           = $criteria['days'];
    $disabled       = $criteria['disabled'];
    $do_contact     = $criteria['do_contact'];
    $num_criteria   = $criteria['num_criteria'];
    $loader_name    = $criteria['loader_name'];
  }
  
  // if there are any session variables then use those
  if (array_key_exists('tripal_pub_import', $_SESSION)) {
    $remote_db    = $_SESSION['tripal_pub_import']['remote_db'];
    $days         = $_SESSION['tripal_pub_import']['days'];
    $disabled     = $_SESSION['tripal_pub_import']['disabled'];
    $do_contact   = $_SESSION['tripal_pub_import']['do_contact'];
    $num_criteria = $_SESSION['tripal_pub_import']['num_criteria'];
    $loader_name  = $_SESSION['tripal_pub_import']['loader_name'];
    
    // check if the pub_import_id in the session variable is not the same as the one we've been provided
    // if so, then clear the session variable
    if ($pub_import_id and $pub_import_id != $_SESSION['tripal_pub_import']['pub_import_id']) {
      unset($_SESSION['tripal_pub_import']);
    }
  }
  
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $remote_db    = $form_state['values']['remote_db'];
    $days         = $form_state['values']['days'];
    $disabled     = $form_state['values']['disabled'];
    $do_contact   = $form_state['values']['do_contact'];
    $num_criteria = $form_state['values']['num_criteria'];
    $loader_name  = $form_state['values']['loader_name'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $remote_db    = $form_state['input']['remote_db'];
    $days         = $form_state['input']['days'];
    $disabled     = $form_state['input']['disabled'];
    $do_contact   = $form_state['input']['do_contact'];
    $loader_name  = $form_state['input']['loader_name'];
    
    // because the num_criteria is a value and not a visible or hidden form
    // element it is not part of the ['input'] array, so we need to get it from the form
    $num_criteria = $form_state['complete form']['num_criteria']['#value'];
  }

  if (array_key_exists('triggering_element', $form_state) and
  $form_state['triggering_element']['#name'] == 'add') {
    $num_criteria++;
  }
  if (array_key_exists('triggering_element', $form_state) and
  $form_state['triggering_element']['#name'] == 'remove') {
    $num_criteria--;
  }
  

  
  // set the values we need for later but that should not be shown on the form
  $form['num_criteria']= array(
    '#type'  => 'value',
    '#value' => $num_criteria,
  );
  $form['pub_import_id'] = array(
    '#type'  => 'value',
    '#value' => $pub_import_id,
  );
  $form['action'] = array(
    '#type'  => 'value',
    '#value' => $action,
  );

  // add in the elements that will be organized via a theme function
  $form['themed_element']['loader_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Loader Name'),
    '#description'   => t('Please provide a name for this loader setup.'),
    '#default_value' => $loader_name,
    '#required'      => TRUE,
  );

  $supported_dbs = variable_get('tripal_pub_supported_dbs', array());
  $remote_dbs = array();
  $values = array(
    'name' => $supported_dbs,
  );
  $dbs = chado_select_record('db', array('*'), $values);
  foreach ($dbs as $index => $db) {
    $remote_dbs[$db->name] = $db->description;
  };
  // use PubMed as the default
  if (!$remote_db) {
    $remote_db = 'PMID';
  }
  $form['themed_element']['remote_db'] = array(
    '#title' => t('Remote Database'),
    '#type' => 'select',
    '#options' => $remote_dbs,
    '#default_value' => $remote_db,
    '#ajax' => array(
      'callback' => "tripal_pubs_setup_form_ajax_update",
      'wrapper'  => 'tripal-pubs-importer-setup',
      'effect'   => 'fade',
      'method'   => 'replace',
    ),
  );
  $form['themed_element']['days'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Days since record modified'),
    '#description'   => t('Limit the search to include pubs that have been added no more than this many days before today.'),
    '#default_value' => $days,
    '#size'          => 5,
  );
  $form['themed_element']['disabled'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Disabled'),
    '#description'   => t('Check to disable this importer.'),
    '#default_value' => $disabled,
  );
  $form['themed_element']['do_contact'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Create Contact'),
    '#description'   => t('Check to create an entry in the contact table for each author of a matching publication during import. This allows storage of
       additional information such as affilation, etc. Otherwise, only authors names are retrieved.'),
    '#default_value' => $do_contact,
  );
  
  // add in the form for the criteria
  tripal_pub_importer_setup_add_criteria_fields($form, $form_state, $num_criteria, $criteria);

  // add in the buttons
  $form['save'] = array(
    '#type'         => 'submit',
    '#value'        => t('Save Importer'),
  );
  $form['test'] = array(
    '#type'         => 'submit',
    '#value'        => t('Test Importer'),
  );
  $form['delete'] = array(
    '#type'         => 'submit',
    '#value'        => t('Delete Importer'),
    '#attributes'  => array('style' => 'float: right;')
  );
  
  // add in the section where the test results will appear
  $form['results'] = array(
    '#markup' => '<div id="tripal-pub-importer-test-section"></div>',
  );
  
  // allow the selected remote database to make changes to the form if needed
  $callback = "tripal_pub_remote_alter_form_$remote_db";
  $form = call_user_func($callback, $form, $form_state, $num_criteria);
 
  $form['themed_element']['#theme'] = 'tripal_pub_importer_setup_form_elements';
  
  return $form;
}

/**
 * 
 * @param $form
 * @param $form_state
 * @param $num_criteria
 * @param $criteria
 */
function tripal_pub_importer_setup_add_criteria_fields(&$form, &$form_state, $num_criteria, $criteria){
  
  // choices array
  $scope_choices = array(
    'any'      => 'Any Field',
    'abstract' => 'Abstract',
    'author'   => 'Author',
    'id'       => 'Accession',
    'title'    => 'Title',
    'journal'   => 'Journal Name'
  );
  
  $first_op_choices = array(
    ''    => '',
    'NOT' => 'NOT'
  );
  $op_choices = array(
    'AND' => 'AND',
    'OR'  => 'OR',
    'NOT' => 'NOT'
  );
  
  for($i = 1; $i <= $num_criteria; $i++) {
    $is_phrase = 1;
    
    $search_terms = '';
    $scope = '';
    $is_phrase = '';
    $operation = '';
  
    // if we have criteria supplied from the database then use that as the initial defaults
    if ($criteria) {
      $search_terms = $criteria['criteria'][$i]['search_terms'];
      $scope        = $criteria['criteria'][$i]['scope'];
      $is_phrase    = $criteria['criteria'][$i]['is_phrase'];
      $operation    = $criteria['criteria'][$i]['operation'];
    }

    // if the criteria comes the session
    if (array_key_exists('tripal_pub_import', $_SESSION)) {
      $search_terms = isset($_SESSION['tripal_pub_import']['criteria'][$i]['search_terms']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['search_terms'] : $search_terms;
      $scope        = isset($_SESSION['tripal_pub_import']['criteria'][$i]['scope'])        ? $_SESSION['tripal_pub_import']['criteria'][$i]['scope']        : $scope;
      $is_phrase    = isset($_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase'])    ? $_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase']    : $is_phrase;
      $operation    = isset($_SESSION['tripal_pub_import']['criteria'][$i]['operation'])    ? $_SESSION['tripal_pub_import']['criteria'][$i]['operation']    : $operation;
    }
    
    // If the form_state has variables then use those.  This happens when an error occurs on the form or the
    // form is resbumitted using AJAX
    if (array_key_exists('values', $form_state)) {
      $search_terms = $form_state['values']["search_terms-$i"];
      $scope        = $form_state['values']["scope-$i"];
      $is_phrase    = $form_state['values']["is_phrase-$i"];
      $operation    = $form_state['values']["operation-$i"];
    }
    $form['themed_element']['criteria'][$i]["scope-$i"] = array(
      '#type'          => 'select',
      '#description'   => t('Please select the fields to search for this term.'),
      '#options'       => $scope_choices,
      '#default_value' => $scope,
    );
    $form['themed_element']['criteria'][$i]["search_terms-$i"] = array(
      '#type'          => 'textfield',
      '#description'   => t('<span style="white-space: normal">Please provide a list of words for searching. You may use
        conjunctions such as "AND" or "OR" to separate words if they are expected in
        the same scope, but do not mix ANDs and ORs.  Check the "Is Phrase" checkbox to use conjunctions as part of the text to search</span>'),
      '#default_value' => $search_terms,
      '#required'      => TRUE,
      '#maxlength' => 2048,
    );
    $form['themed_element']['criteria'][$i]["is_phrase-$i"] = array(
      '#type'    => 'checkbox',
      '#title'   => t('Is Phrase?'),
      '#default_value' => $is_phrase,
    );
  
    if ($i == 1) {
      /*
       $form['criteria'][$i]["operation-$i"] = array(
         '#type'          => 'select',
         '#options'       => $first_op_choices,
         '#default_value' => $operation,
       );*/
    }
    if ($i > 1) {
      $form['themed_element']['criteria'][$i]["operation-$i"] = array(
        '#type'          => 'select',
        '#options'       => $op_choices,
        '#default_value' => $operation,
      );
    }
    if ($i == $num_criteria) {
      if($i > 1) {
        $form['themed_element']['criteria'][$i]["remove-$i"] = array(
          '#type'         => 'button',
          '#name'         => 'remove',
          '#value'        => t('Remove'),
          '#ajax' => array(
            'callback' => "tripal_pubs_setup_form_ajax_update",
            'wrapper'  => 'tripal-pubs-importer-setup',
            'effect'   => 'fade',
            'method'   => 'replace',
            'prevent'  => 'click'
          ),
          // When this button is clicked, the form will be validated and submitted.
          // Therefore, we set custom submit and validate functions to override the
          // default form submit.  In the validate function we set the form_state
          // to rebuild the form so the submit function never actually gets called,
          // but we need it or Drupal will run the default validate anyway.
          // we also set #limit_validation_errors to empty so fields that
          // are required that don't have values won't generate warnings.
          '#submit'   => array('tripal_pub_setup_form_ajax_button_submit'),
          '#validate' => array('tripal_pub_setup_form_ajax_button_validate'),
          '#limit_validation_errors' => array(),
        );
      }
      $form['themed_element']['criteria'][$i]["add-$i"] = array(
        '#type'  => 'button',
        '#name'  => 'add',
        '#value' => t('Add'),
        '#ajax'  => array(
          'callback' => "tripal_pubs_setup_form_ajax_update",
          'wrapper'  => 'tripal-pubs-importer-setup',
          'effect'   => 'fade',
          'method'   => 'replace',
          'prevent'  => 'click'
        ),
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default form submit.  In the validate function we set the form_state
        // to rebuild the form so the submit function never actually gets called,
        // but we need it or Drupal will run the default validate anyway.
        // we also set #limit_validation_errors to empty so fields that
        // are required that don't have values won't generate warnings.
        '#submit'   => array('tripal_pub_setup_form_ajax_button_submit'),
        '#validate' => array('tripal_pub_setup_form_ajax_button_validate'),
        '#limit_validation_errors' => array(),
      );
    }
  }
}
/**
 * This function is used to rebuild the form if an ajax call is made vai a button.
 * The button causes the form to be submitted. We don't want this so we override
 * the validate and submit routines on the form button. Therefore, this function
 * only needs to tell Drupal to rebuild the form
 */
function  tripal_pub_setup_form_ajax_button_validate($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
/**
 * This function is just a dummy to override the default form submit on ajax calls for buttons
 */
function tripal_pub_setup_form_ajax_button_submit($form, &$form_state){
  // do nothing
}
/**
 *
 */
function tripal_pub_importer_setup_form_validate($form, &$form_state) {
  $num_criteria = $form_state['values']['num_criteria'];
  $remote_db =  $form_state['values']["remote_db"];
  $days =  trim($form_state['values']["days"]);
  $disabled =  $form_state['values']["disabled"];
  $do_contact =  $form_state['values']["do_contact"];
  $loader_name =  trim($form_state['values']["loader_name"]);

  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms =  trim($form_state['values']["search_terms-$i"]);
    $scope =  $form_state['values']["scope-$i"];
    $is_phrase =  $form_state['values']["is_phrase-$i"];
    $operation = '';
    if($i > 1) {      
      $operation =  $form_state['values']["operation-$i"];
    }

    if (!$is_phrase) {
      if (preg_match('/and/i', $search_terms) and preg_match('/or/i', $search_terms)) {
        form_set_error("search_terms-$i", "You may use 'AND' or 'OR' but cannot use both. Add a new entry below with the same scope for the other conunction.");
        $_SESSION['tripal_pub_import']['perform_search'] = 0;
      }
    }
  }

  if ($days and !is_numeric($days) or preg_match('/\./', $days)) {
    form_set_error("days", "Please enter a numeric, non decimal value, for the number of days.");
    $_SESSION['tripal_pub_import']['perform_search'] = 0;
  }
  // allow the selected remote database to validate any changes to the form if needed
  $callback = "tripal_pub_remote_validate_form_$remote_db";
  $form = call_user_func($callback, $form, $form_state);
}

/**
 *
 */
function tripal_pub_importer_setup_form_submit($form, &$form_state) {

  $pub_import_id = $form_state['values']['pub_import_id'];
  $num_criteria = $form_state['values']['num_criteria'];
  $remote_db =  $form_state['values']["remote_db"];
  $days =  trim($form_state['values']["days"]);
  $loader_name =  trim($form_state['values']["loader_name"]);
  $disabled =  $form_state['values']["disabled"];
  $do_contact =  $form_state['values']["do_contact"];

  // set the session variables
  $_SESSION['tripal_pub_import']['remote_db'] = $remote_db;
  $_SESSION['tripal_pub_import']['days'] = $days;
  $_SESSION['tripal_pub_import']['num_criteria'] = $num_criteria;
  $_SESSION['tripal_pub_import']['loader_name'] = $loader_name;
  $_SESSION['tripal_pub_import']['disabled'] = $disabled;
  $_SESSION['tripal_pub_import']['do_contact'] = $do_contact;
  $_SESSION['tripal_pub_import']['pub_import_id'] = $pub_import_id;
  unset($_SESSION['tripal_pub_import']['criteria']);
  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms =  trim($form_state['values']["search_terms-$i"]);
    $scope =  $form_state['values']["scope-$i"];
    $is_phrase =  $form_state['values']["is_phrase-$i"];
    $operation = '';
    if ($i > 1) {
      $operation =  $form_state['values']["operation-$i"];
    }

    $_SESSION['tripal_pub_import']['criteria'][$i] = array(
      'search_terms' => $search_terms,
      'scope' => $scope,
      'is_phrase' => $is_phrase,
      'operation' => $operation
    );
  }

  // now perform the appropriate action for the button clicked
  if ($form_state['values']['op'] == 'Test Importer') {
    $_SESSION['tripal_pub_import']['perform_search'] = 1;
  }
  if ($form_state['values']['op'] == 'Save Importer' or
  $form_state['values']['op'] == 'Save & Import Now') {
    $record = array(
      'name' => $loader_name,
      'criteria' => serialize($_SESSION['tripal_pub_import']),
      'disabled' => $disabled,
      'do_contact' => $do_contact
    );
    // first check to see if this pub_import_id is already present. If so,
    // do an update rather than an insert
    $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :pub_import_id";
    $importer = db_query($sql, array(':pub_import_id' => $pub_import_id))->fetchObject();
    if($importer) {
      // do the update
      $record['pub_import_id'] = $pub_import_id;
      if(drupal_write_record('tripal_pub_import', $record, 'pub_import_id')){
        unset($_SESSION['tripal_pub_import']);
        drupal_set_message('Publication import settings updated.');
        drupal_goto('admin/tripal/chado/tripal_pub/import_list');
      }
      else {
        drupal_set_message('Could not update publication import settings.', 'error');
      }
    }
    else {
      // do the insert
      if(drupal_write_record('tripal_pub_import', $record)){
        unset($_SESSION['tripal_pub_import']);
        drupal_set_message('Publication import settings saved.');
        // if the user wants to do the import now then do it (may time out
        // for long jobs)
        if ($form_state['values']['op'] == 'Save & Import Now') {
          tripal_pub_import_publications($record['pub_import_id']);
        }
        drupal_goto('admin/tripal/chado/tripal_pub/import_list');
      }
      else {
        drupal_set_message('Could not save publication import settings.', 'error');
      }
    }
  }
  if ($form_state['values']['op'] == 'Delete Importer') {
    $sql = "DELETE FROM {tripal_pub_import} WHERE pub_import_id = :pub_import_id";
    $success = db_query($sql, array(':pub_import_id' => $pub_import_id));
    if ($success) {
      drupal_set_message('Publication importer deleted.');
      drupal_goto('admin/tripal/chado/tripal_pub/import_list');
    }
    else {
      drupal_set_message('Could not delete publication importer.', 'error');
    }
  }
}
/**
 * AJAX callback for updating the form.  
 */
function tripal_pubs_setup_form_ajax_update($form, $form_state) {
  return $form['themed_element'];
}

/**
 * 
 * @param $form
 */

function theme_tripal_pub_importer_setup_form_elements($variables) {
  $form = $variables['form'];

  // first render the fields at the top of the form
  $markup  = '';
  $markup .= '<div id="pub-search-form-row0">';
  $markup .= '  <div id="pub-search-form-row0-col1" style="float: left">' . drupal_render($form['remote_db']) . '</div>';
  $markup .= '  <div id="pub-search-form-row0-col2" style="float: left; margin-left: 10px">' . drupal_render($form['loader_name']) . '</div>';
  $markup .= '</div>';
  $markup .= '<div id="pub-search-form-row1" style="clear:both">';
  $markup .= '  <div id="pub-search-form-row1-col1">' . drupal_render($form['days']) . '</div>';
  $markup .= '</div>';
  $markup .= '<div id="pub-search-form-row2">' . drupal_render($form['disabled']) . '</div>';
  $markup .= '<div id="pub-search-form-row3">' . drupal_render($form['do_contact']) . '</div>';

  // next render the criteria fields into a table format
  $rows = array();
  foreach ($form['criteria'] as $i => $element) {
    if(is_numeric($i)) {
      $rows[] = array(
        drupal_render($element["operation-$i"]),
        drupal_render($element["scope-$i"]),
        drupal_render($element["search_terms-$i"]),
        drupal_render($element["is_phrase-$i"]),
        drupal_render($element["add-$i"]) . drupal_render($element["remove-$i"]),
      );
    }
  }
  
  $headers = array('Operation','Scope', 'Search Terms', '','');
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(),
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  $criteria_table = theme_table($table);
  $markup .= $criteria_table;
  
  // add the rendered form
  $form = array(
    '#markup' => $markup,
    '#prefix' => '<div id="tripal-pubs-importer-setup">',
    '#suffix' => '</div>',
  );
  
  return drupal_render($form);
}

/**
 * Add a job to import publications
 * 
 * @param $pub_importer_id
 */
function tripal_pub_importer_submit_job($import_id) {
  global $user;
  
  // get all of the loaders
  $args = array(':import_id' => $import_id);
  $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :import_id ";
  $import = db_query($sql, $args)->fetchObject();
  
  $args = array($import_id);
  tripal_add_job("Import publications $import->name", 'tripal_pub',
    'tripal_pub_import_publications_by_import_id', $args, $user->uid);
  
  drupal_goto('admin/tripal/chado/tripal_pub/import_list');
}

