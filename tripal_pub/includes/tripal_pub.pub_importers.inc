<?php
/**
 * @file
 * Management of importers
 */

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
 * Creates the page that contains the publication importer setup form and
 * test results.
 *
 * @param $action
 *   The action to perform
 * @param $pub_import_id
 *   The importer ID
 *
 * @return
 *   The HTML for the importer setup page
 *
 * @ingroup tripal_pub
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
      $results = tripal_get_remote_pubs($remote_db, $search_array, $limit, $page);
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
 * The form used for creating publication importers.
 *
 * @param $form
 *   The Drupal form
 * @param $form_state
 *   The form state
 * @param $pub_import_id
 *   The publication importer ID
 * @param $action
 *   The action to perform
 *
 * @return
 *   A form array
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
 * A helper function for the importer setup form that adds the criteria to
 * the form that belong to the importer.
 *
 * @param $form
 *   The form
 * @param $form_state
 *   The form state
 * @param $num_criteria
 *   The number of criteria that exist for the importer
 * @param $criteria
 *   An array containing the criteria
 *
 *@return
 *  A form array
 *
 * @ingroup tripal_pub
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
 *
 * @ingroup tripal_pub
 */
function  tripal_pub_setup_form_ajax_button_validate($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}

/**
 * This function is just a dummy to override the default form submit on ajax calls for buttons
 *
 * @ingroup tripal_pub
 */
function tripal_pub_setup_form_ajax_button_submit($form, &$form_state){
  // do nothing
}

/**
 * Validate the tripal_pub_importer_setup_form form
 *
 * @ingroup tripal_pub
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
 * Submit the tripal_pub_importer_setup_form form
 *
 * @ingroup tripal_pub
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
          tripal_execute_pub_importer($record['pub_import_id']);
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
 *
 * @ingroup tripal_pub
 */
function tripal_pubs_setup_form_ajax_update($form, $form_state) {
  return $form['themed_element'];
}

/**
 * Theme the tripal_pub_importer_setup_form form.
 *
 * @ingroup tripal_pub
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
 *   The id of the importer to submit a job to update
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importer_submit_job($import_id) {
  global $user;

  // get all of the loaders
  $args = array(':import_id' => $import_id);
  $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :import_id ";
  $import = db_query($sql, $args)->fetchObject();

  $args = array($import_id);
  tripal_add_job("Import publications $import->name", 'tripal_pub',
    'tripal_execute_pub_importer', $args, $user->uid);

  drupal_goto('admin/tripal/chado/tripal_pub/import_list');
}

/**
 * Adds publications that have been retrieved from a remote database and
 * consolidated into an array of details.
 *
 * @param $pubs
 *   An array containing a list of publications to add to Chado.  The
 *   array contains a set of details for the publication.
 * @param $do_contact
 *   Set to TRUE if authors should automatically have a contact record added
 *   to Chado.
 * @param $update
 *   If set to TRUE then publications that already exist in the Chado database
 *   will be updated, whereas if FALSE only new publications will be added
 *
 * @return
 *   Returns an array containing the number of publications that were
 *   inserted, updated, skipped and which had an error during import.
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_publications($pubs, $do_contact, $update = FALSE) {
  $report = array();
  $report['error'] = 0;
  $report['inserted'] = array();
  $report['skipped'] = array();
  $total_pubs = count($pubs);

  // iterate through the publications and add each one
  $i = 1;
  foreach ($pubs as $pub) {
    $memory = number_format(memory_get_usage()) . " bytes";
    print "Processing $i of $total_pubs. Memory usage: $memory.\r";

    // add the publication to Chado
    $action = '';
    $pub_id = tripal_pub_add_publication($pub, $action, $do_contact, $update);
    if ($pub_id){
      // add the publication cross reference (e.g. to PubMed)
      if ($pub_id and $pub['Publication Dbxref']) {
        $pub_dbxref = tripal_pub_add_pub_dbxref($pub_id, $pub['Publication Dbxref']);
      }
      $pub['pub_id'] = $pub_id;
    }

    switch ($action) {
      case 'error':
        $report['error']++;
        break;
      case 'inserted':
        $report['inserted'][] = $pub;
        break;
      case 'updated':
        $report['updated'][] = $pub;
        break;
      case 'skipped':
        $report['skipped'][] = $pub;
        break;
    }
    $i++;
  }
  print "\n";
  return $report;
}

/**
 * Adds a new publication to the Chado, along with all properties and
 * database cross-references. If the publication does not already exist
 * in Chado then it is added.  If it does exist nothing is done.  If
 * the $update parameter is TRUE then the publication is updated if it exists.
 *
 * @param $pub_details
 *   An associative array containing all of the details about the publication.
 * @param $action
 *   This variable will get set to a text value indicating the action that was
 *   performed. The values include 'skipped', 'inserted', 'updated' or 'error'.
 * @param $do_contact
 *   Optional. Set to TRUE if a contact entry should be added to the Chado contact table
 *   for authors of the publication.
 * @param $update_if_exists
 *   Optional.  If the publication already exists then this function will return
 *   without adding a new publication.  However, set this value to TRUE to force
 *   the function to pudate the publication using the $pub_details that are provided.
 *
 * @return
 *   If the publication already exists, is inserted or updated then the publication
 *   ID is returned, otherwise FALSE is returned. If the publication already exists
 *   and $update_if_exists is not TRUE then the $action variable is set to 'skipped'.
 *   If the publication already exists and $update_if_exists is TRUE and if the update
 *   was successful then $action is set to 'updated'.  Otherwise on successful insert
 *   the $action variable is set to 'inserted'.  If the function failes then the
 *   $action variable is set to 'error'
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_publication($pub_details, &$action, $do_contact = FALSE, $update_if_exists = FALSE) {
  $pub_id = 0;

  if (!is_array($pub_details)) {
    return FALSE;
  }

  // before proceeding check to see if the publication already exists. If there is only one match
  // and the $update_if_exists is NOT set then return FALSE
  $pub_ids = chado_does_pub_exist($pub_details);

  if(count($pub_ids) == 1 and !$update_if_exists) {
    tripal_report_error('tripal_pub', TRIPAL_NOTICE,
     "There is a publication that is a duplicate of this publication. Cannot continue. It either ".
     "has a matching Dbxref (e.g. PubMed ID), a non-unique citation or matches on the unique  " .
     "constraint set by the Tripal publication module configuration page. \nCitation: %title %dbxref.\nMatching Pub id: %ids",
       array(
        '%title' => $pub_details['Citation'],
        '%dbxref' => $pub_details['Publication Dbxref'],
        '%ids' => implode(",", $pub_ids),
       )
    );
    $action = 'skipped';
    return FALSE;
  }
  // if we have more than one matching pub then return an error as we don't know which to update even if
  // update_if_exists is set to TRUE
  if(count($pub_ids) > 1) {
    tripal_report_error('tripal_pub', TRIPAL_NOTICE,
      "There are %num publications that are duplicates of this publication. They either " .
      "have a matching Dbxref (e.g. PubMed ID) or match on the unique constraint set by the Tripal publication module ".
      "configuration page.  \nCitation: %title %dbxref.\nMatching Pub ids: %ids",
       array(
         '%num' => count($pub_ids),
         '%title' => $pub_details['Citation'],
         '%dbxref' => $pub_details['Publication Dbxref'],
        '%ids' => implode(",", $pub_ids),
       )
    );
    $action = 'skipped';
    return FALSE;
  }

  // get the publication type (use the first publication type)
  if (array_key_exists('Publication Type', $pub_details)) {
    $pub_type = '';
    if(is_array($pub_details['Publication Type'])) {
      $pub_type = $pub_details['Publication Type'][0];
    }
    else {
      $pub_type = $pub_details['Publication Type'];
    }
    $identifiers = array(
      'name' => $pub_type,
      'cv_id' => array(
        'name' => 'tripal_pub'
      ),
    );
    $pub_type = tripal_get_cvterm($identifiers);
  }
  else {
    tripal_report_error('tripal_pub', TRIPAL_ERROR,
      "The Publication Type is a required property but is missing", array());
    $action = 'error';
    return FALSE;
  }
  if (!$pub_type) {
    tripal_report_error('tripal_pub', TRIPAL_ERROR, "Cannot find publication type: '%type'",
      array('%type' => $pub_details['Publication Type'][0]));
    $action = 'error';
    return FALSE;
  }

  // the series name field in the pub table is only 255 characters, so we should trim just in case
  $series_name = '';
  if (array_key_exists('Series_Name', $pub_details)) {
    $series_name = substr($pub_details['Series Name'], 0, 255);
  }
  if (array_key_exists('Journal Name', $pub_details)) {
    $series_name = substr($pub_details['Journal Name'], 0, 255);
  }

  // build the values array for inserting or updating
  $values = array(
    'title'       => $pub_details['Title'],
    'volume'      => $pub_details['Volume'],
    'series_name' => $series_name,
    'issue'       => $pub_details['Issue'],
    'pyear'       => $pub_details['Year'],
    'pages'       => $pub_details['Pages'],
    'uniquename'  => $pub_details['Citation'],
    'type_id'     => $pub_type->cvterm_id,
  );

  // if there is no pub_id then we need to do an insert.
  if (!$pub_id) {
    $options = array('statement_name' => 'ins_pub_tivoseispypaunty');
    $pub = chado_insert_record('pub', $values, $options);
    if (!$pub) {
      tripal_report_error('tripal_pub', TRIPAL_ERROR, "Cannot insert the publication with title: %title",
      array('%title' => $pub_details['Title']));
      $action = 'error';
      return FALSE;
    }
    $pub_id = $pub['pub_id'];
    $action = 'inserted';
  }

  // if there is a pub_id and we've been told to update, then do the update
  if ($pub_id and $update_if_exists) {
    $match = array('pub_id' => $pub_id);
    $options = array('statement_name' => 'up_pub_tivoseispypaunty');
    $success = chado_update_record('pub', $match, $values, $options);
    if (!$success) {
      tripal_report_error('tripal_pub', TRIPAL_ERROR, "Cannot update the publication with title: %title",
      array('%title' => $pub_details['Title']));
      $action = 'error';
      return FALSE;
    }
    $action = 'updated';
  }

  // before we add any new properties we need to remove those that are there if this
  // is an update.  The only thing we don't want to remove are the 'Publication Dbxref'
  if ($update_if_exists) {
    $sql = "
      DELETE FROM {pubprop}
      WHERE
        pub_id = :pub_id AND
        NOT type_id in (
          SELECT cvterm_id
          FROM {cvterm}
          WHERE name = 'Publication Dbxref'
        )
    ";
    chado_query($sql, array(':pub_id' => $pub_id));
  }

  // iterate through the properties and add them
  foreach ($pub_details as $key => $value) {
    // the pub_details may have the raw search data (e.g. in XML from PubMed.  We'll irgnore this for now
    if($key == 'raw') {
      continue;
    }

    // get the cvterm by name
    $identifiers = array(
      'name' => $key,
      'cv_id' => array(
        'name' => 'tripal_pub'
      ),
    );
    $cvterm = tripal_get_cvterm($identifiers);

    // if we could not find the cvterm by name then try by synonym
    //$cvterm = tripal_cv_get_cvterm_by_name($key, NULL, 'tripal_pub');
    if (!$cvterm) {
      $identifiers = array(
        'synonym' => array(
          'name' => $key,
          'cv_name' => 'tripal_pub'
        )
      );
      $cvterm = tripal_get_cvterm($identifiers);
    }
    if (!$cvterm) {
      tripal_report_error('tripal_pub', TRIPAL_ERROR, "Cannot find term: '%prop'. Skipping.", array('%prop' => $key));
      continue;
    }

    // skip details that won't be stored as properties
    if ($key == 'Author List') {
      tripal_pub_add_authors($pub_id, $value, $do_contact);
      continue;
    }
    if ($key == 'Title' or $key == 'Volume' or $key == 'Journal Name' or $key == 'Issue' or
    $key == 'Year' or $key == 'Pages') {
      continue;
    }

    $success = 0;
    if (is_array($value)) {
      foreach ($value as $subkey => $subvalue) {
        // if the key is an integer then this array is a simple list and
        // we will insert using the primary key. Otheriwse, use the new key
        if(is_int($subkey)) {
          $success = chado_insert_property('pub', $pub_id, $key, 'tripal_pub', $subvalue, FALSE);
        }
        else {
          $success = chado_insert_property('pub', $pub_id, $subkey, 'tripal_pub', $subvalue, FALSE);
        }
      }
    }
    else {
      $success = chado_insert_property('pub', $pub_id, $key, 'tripal_pub', $value, TRUE);
    }
    if (!$success) {
      tripal_report_error('tripal_pub', TRIPAL_ERROR, "Cannot add property '%prop' to publication. Skipping.",
      array('%prop' => $key));
      continue;
    }
  }

  return $pub_id;
}



/**
 * Add one or more authors to a publication
 *
 * @param $pub_id
 *   The publication ID of the pub in Chado.
 * @param $authors
 *   An array of authors.  Each author should have a set of keys/value pairs
 *   describing the author.
 * @param $do_contact
 *   Optional. Set to TRUE if a contact entry should be added to the Chado contact table
 *   for authors of the publication.
 * @ingroup tripal_pub
 */
function tripal_pub_add_authors($pub_id, $authors, $do_contact) {
  $rank = 0;

  // first remove any of the existing pubauthor entires
  $sql = "DELETE FROM {pubauthor} WHERE pub_id = :pub_id";
  chado_query($sql, array(':pub_id' => $pub_id));

  // iterate through the authors and add them to the pubauthors and contact
  // tables of chado, then link them through the custom pubauthors_contact table
  foreach ($authors as $author) {
    // skip invalid author entires
    if ($author['valid'] == 'N') {
      continue;
    }
    // remove the 'valid' property as we don't have a CV term for it
    unset($author['valid']);

    // construct the contact.name field using the author information
    $name = '';
    $type = 'Person';
    if ($author['Given Name']) {
      $name .= $author['Given Name'];
    }
    if ($author['Surname']) {
      $name .= ' ' . $author['Surname'];
    }
    if ($author['Suffix']) {
      $name .= ' ' . $author['Suffix'];
    }
    if ($author['Collective']) {
      $name = $author['Collective'];
      $type = 'Collective';
    }
    $name = trim($name);

    // add an entry to the pubauthors table
    $values = array(
      'pub_id' => $pub_id,
      'rank' => $rank,
      'surname' => $author['Surname'] ? substr($author['Surname'], 0, 100) : substr($author['Collective'], 0, 100),
      'givennames' => $author['Given Name'],
      'suffix' => $author['Suffix'],
    );
    $options = array('statement_name' => 'ins_pubauthor_idrasugisu');
    $pubauthor = chado_insert_record('pubauthor', $values, $options);

    // if the user wants us to create a contact for each author then do it.
    if ($do_contact) {
      // Add the contact
      $contact = tripal_contact_add_contact($name, '', $type, $author);

      // if we have succesfully added the contact and the pubauthor entries then we want to
      // link them together
      if ($contact and $pubauthor) {

        // link the pubauthor entry to the contact
        $values = array(
          'pubauthor_id' => $pubauthor['pubauthor_id'],
          'contact_id' => $contact['contact_id'],
        );
        $options = array('statement_name' => 'ins_pubauthorcontact_puco');
        $pubauthor_contact = chado_insert_record('pubauthor_contact', $values, $options);
        if (!$pubauthor_contact) {
          tripal_report_error('tripal_pub', TRIPAL_ERROR, "Cannot link pub authro and contact.", array());
        }
      }
    }
    $rank++;
  }
}

/**
 * This function generates an array suitable for use with the
 * tripal_pub_create_citation function for any publication
 * already stored in the Chado tables.
 *
 * @param $pub_id
 *   The publication ID
 * @param $skip_existing
 *   Set to TRUE to skip publications that already have a citation
 *   in the pubprop table.  Set to FALSE to generate a citation
 *   regardless if the citation already exists.
 *
 * @return
 *   An array suitable for the trpial_pub_create_citation function. On
 *   failure returns FALSE.
 *
 * @ingroup tripal_pub
 */
function tripal_pub_get_publication_array($pub_id, $skip_existing = TRUE) {

  $options = array('return_array' => 1);

  // ---------------------------------
  // get the publication
  // ---------------------------------
  $values = array('pub_id' => $pub_id);
  $pub = chado_generate_var('pub', $values);

  // expand the title
  $pub = chado_expand_var($pub, 'field', 'pub.title');
  $pub = chado_expand_var($pub, 'field', 'pub.volumetitle');
  $pub = chado_expand_var($pub, 'field', 'pub.uniquename');
  $pub_array = array();
  if (trim($pub->title)) {
    $pub_array['Title'] = $pub->title;
  }
  if (trim($pub->volumetitle)) {
    $pub_array['Volume Title'] = $pub->volumetitle;
  }
  if (trim($pub->volume)) {
    $pub_array['Volume'] = $pub->volume;
  }
  if (trim($pub->series_name)) {
    $pub_array['Series Name'] = $pub->series_name;
  }
  if (trim($pub->issue)) {
    $pub_array['Issue'] = $pub->issue;
  }
  if (trim($pub->pyear)) {
    $pub_array['Year'] = $pub->pyear;
  }
  if (trim($pub->pages)) {
    $pub_array['Pages'] = $pub->pages;
  }
  if (trim($pub->miniref)) {
    $pub_array['Mini Ref'] = $pub->miniref;
  }
  if (trim($pub->uniquename)) {
    $pub_array['Uniquename'] = $pub->uniquename;
  }
  $pub_array['Publication Type'][] = $pub->type_id->name;

  // ---------------------------------
  // get the citation
  // ---------------------------------
  $values = array(
    'pub_id' => $pub->pub_id,
    'type_id' => array(
      'name' => 'Citation',
    ),
  );
  $citation = chado_generate_var('pubprop', $values);
  if ($citation) {
    $citation = chado_expand_var($citation, 'field', 'pubprop.value', $options);
    if (count($citation) > 1) {
      tripal_report_error('tripal_pub', TRIPAL_ERROR, "Publication has multiple citations already: %pub_id",
      array('%pub_id' => $pubid));
      return FALSE;
    }
    elseif (count($citation) == 1 and $skip_existing == TRUE) {
      // skip this publication, it already has a citation
      return FALSE;
    }
  }

  // ---------------------------------
  // get the publication types
  // ---------------------------------
  $values = array(
    'pub_id' => $pub->pub_id,
    'type_id' => array(
      'name' => 'Publication Type',
    ),
  );
  $ptypes = chado_generate_var('pubprop', $values, $options);
  if ($ptypes) {
    $ptypes = chado_expand_var($ptypes, 'field', 'pubprop.value', $options);
    foreach ($ptypes as $ptype) {
      $pub_array['Publication Type'][] = $ptype->value;
    }
  }

  // ---------------------------------
  // get the authors list
  // ---------------------------------
  $values = array(
    'pub_id' => $pub->pub_id,
    'type_id' => array(
      'name' => 'Authors',
    ),
  );
  $authors = chado_generate_var('pubprop', $values);
  $authors = chado_expand_var($authors, 'field', 'pubprop.value', $options);
  if (count($authors) > 1) {
    tripal_report_error('tripal_pub', TRIPAL_ERROR, "Publication has multiple author lists. It should have only one list: %pub_id",
    array('%pub_id' => $pubid));
    return FALSE;
  }
  else if (trim($authors->value)) {
    $pub_array['Authors'] = $authors->value;
  }
  // if there is no 'Author's property then try to retreive authors from the pubauthor table
  else {
    $sql = "
      SELECT string_agg(surname || ' ' || givennames, ', ')
      FROM {pubauthor}
      WHERE pub_id = :pub_id
      GROUP BY pub_id
    ";
    $au = chado_query($sql, array(':pub_id' => $pub_id))->fetchField();
    if ($au) {
      $pub_array['Authors'] = $au;
    }
  }

  //Get other props
  $props = array(
    'Journal Abbreviation',
    'Elocation',
    'Media Code',
    'Conference Name',
    'Keywords',
    'Series Name',
    'pISSN',
    'Publication Date',
    'Journal Code',
    'Journal Alias',
    'Journal Country',
    'Published Location',
    'Publication Model',
    'Language Abbr',
    'Alias',
    'Publication Dbxref',
    'Copyright',
    'Abstract',
    'Notes',
    'Citation',
    'Language',
    'URL',
    'eISSN',
    'DOI',
    'ISSN',
    'Publication Code',
    'Comments',
    'Publisher',
    'Media Alias',
    'Original Title'
  );
  foreach ($props AS $prop) {
    $sql =
      "SELECT value FROM {pubprop}
       WHERE type_id =
         (SELECT cvterm_id
          FROM {cvterm}
          WHERE name = :cvtname AND cv_id =
            (SELECT cv_id
             FROM {cv}
             WHERE name = 'tripal_pub'
            )
         )
       AND pub_id = :pub_id
    ";
    $val = trim(chado_query($sql, array(':cvtname' => $prop, ':pub_id' => $pub->pub_id))->fetchField());
    if ($val) {
      $pub_array[$prop] =$val;
    }
  }
  return $pub_array;
}