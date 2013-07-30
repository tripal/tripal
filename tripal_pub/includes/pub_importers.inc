<?php
/**
 * A function to render a listing of all publication importers
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importers_list() {
  // clear out the session variable when we view the list.
  unset($_SESSION['tripal_pub_import']);

  $header = array('', 'Importer Name', 'Database', 'Search String', 'Disabled', 'Create Contact', '');
  $rows = array();
  $importers = db_query("SELECT * FROM {tripal_pub_import} ORDER BY name");

  while ($importer = db_fetch_object($importers)) {
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
      l(t('Edit/Test'), "admin/tripal/tripal_pub/import/edit/$importer->pub_import_id"),
      $importer->name,
      $criteria['remote_db'],
      $criteria_str,
      $importer->disabled ? 'Yes' : 'No',
      $importer->do_contact ? 'Yes' : 'No',
      l(t('Delete'), "admin/tripal/tripal_pub/import/delete/$importer->pub_import_id"),
    );
  }

  $rows[] = array(
    'data' => array(
     array('data' => l(t('Create a new publication importer.'), "admin/tripal/tripal_pub/import/new"),
        'colspan' => 7),
     )
  );

  $page = theme('table', $header, $rows);
  return $page;
}

/*
 *
 */
function tripal_pub_importer_setup($action = 'new', $pub_import_id = NULL) {
  global $pager_total, $pager_total_items;

  $pager_id = 0;
  $limit = 20;
  
  // make sure the tripal_pub and tripal_contact ontologies are loaded
  $values = array('name' => 'tripal_pub');
  $tpub_cv = tripal_core_chado_select('cv', array('cv_id'), $values);
  if (count($tpub_cv) == 0) {
  	drupal_set_message(t('Before importing publications you must first ') . l(t('load the Tripal Pub Ontology'), 'admin/tripal/tripal_cv/obo_loader'), 'error');
  }
  $values = array('name' => 'tripal_contact');
  $tpub_cv = tripal_core_chado_select('cv', array('cv_id'), $values);
  if (count($tpub_cv) == 0) {
    drupal_set_message(t('If you want to create contact pages for authors, you must first ') . l(t('load the Tripal Contact Ontology'), 'admin/tripal/tripal_cv/obo_loader'), 'error');
  }

  // generate the search form
  $form = drupal_get_form('tripal_pub_importer_setup_form',  $pub_import_id, $action);

  $output = l("Return to publication importers list", "admin/tripal/tripal_pub/import_list");
  $output .= $form;

  // retrieve any results
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


  if ($_SESSION['tripal_pub_import']['perform_search']) {
    // get the list of publications from the remote database using the search criteria.
    $pubs = tripal_pub_get_remote_search_results($remote_db, $search_array, $limit, $pager_id);
    //dpm($pubs);

    // generate the pager
    $total_pages = $pager_total[$pager_id];
    $total_items = $pager_total_items[$pager_id];
    $page = isset($_GET['page']) ? $_GET['page'] : '0';
    $pager = theme('pager');

    // iterate through the results and construct the table displaying the publications
    $rows = array();
    $i = $page * $limit + 1;
    if (count($pubs) > 0) {
      foreach ($pubs as $pub) {
        $citation = htmlspecialchars($pub['Citation']);
        $raw_link = '';
        if($pub['Publication Dbxref']) {
          $raw_link = l('raw', 'admin/tripal/tripal_pub/import/raw/' . $pub['Publication Dbxref'], array('attributes' => array('target' => '_blank')));
        }
        $rows[] = array(
        number_format($i),
        $citation,
        $raw_link,
        );
        $i++;
      }
    }
    $headers = array('', 'Publication', '');
    $table = theme('table', $headers, $rows);

    // join all to form the results
    $output .= "<br><p><b>Found " . number_format($total_items) .
      ". Page " . ($page + 1) . " of $total_pages. " .
      " Results</b></br>" . $table . '</p>' . $pager;    
  }
  return $output;
}
/*
 *
 */
function theme_tripal_pub_importer_setup_form($form) {
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

  $markup  = '';
  $markup .= '<div>' . drupal_render($form['remote_db']) . '</div>';
  $markup .= '<div id="pub-search-form-row1">';
  $markup .= '  <div id="pub-search-form-col1">' . drupal_render($form['loader_name']) . '</div>';
  $markup .= '  <div id="pub-search-form-col3">' . drupal_render($form['days']) . '</div>';
  $markup .= '</div>';
  $markup .= '<div id="pub-search-form-row2">' . drupal_render($form['disabled']) . '</div>';
  $markup .= '<div id="pub-search-form-row3">' . drupal_render($form['do_contact']) . '</div>';
  $markup .= theme('table', $headers, $rows, array('id' => 'tripal-pub-importer-table'));

  $form['criteria'] = array(
    '#type' => 'markup',
    '#value' =>  $markup,
    '#weight' => -10,
  );
  return drupal_render($form);
}
/**
 * Purpose: Provides the form to search pubmed
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importer_setup_form(&$form_state = NULL, $pub_import_id = NULL, $action = 'new') {
  tripal_core_ahah_init_form();
   
  // Set the default values. If the pub_import_id isn't already defined by the form values
  // and one is provided then look it up in the database
  $criteria = NULL;
  if ($action == "edit" and !$form_state['values']) {
    $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = %d";
    $importer = db_fetch_object(db_query($sql, $pub_import_id));

    $criteria       = unserialize($importer->criteria);
    $remote_db      = $criteria['remote_db'];
    $days           = $criteria['days'];
    $disabled       = $criteria['disabled'];
    $do_contact     = $criteria['do_contact'];
    $num_criteria   = $criteria['num_criteria'];
    $loader_name    = $criteria['loader_name'];
  }

  // if we're here because the form was posted then load from the session variable (we lost the form state)
  $num_criteria = isset($_SESSION['tripal_pub_import']['num_criteria']) ? $_SESSION['tripal_pub_import']['num_criteria'] : $num_criteria;
  $loader_name  = isset($_SESSION['tripal_pub_import']['loader_name'])  ? $_SESSION['tripal_pub_import']['loader_name']  : $loader_name;
  $remote_db    = isset($_SESSION['tripal_pub_import']['remote_db'])    ? $_SESSION['tripal_pub_import']['remote_db']    : $remote_db;
  $disabled     = isset($_SESSION['tripal_pub_import']['disabled'])     ? $_SESSION['tripal_pub_import']['disabled']     : $disabled;
  $do_contact   = isset($_SESSION['tripal_pub_import']['do_contact'])   ? $_SESSION['tripal_pub_import']['do_contact']   : $do_contact;
  $days         = isset($_SESSION['tripal_pub_import']['days'])         ? $_SESSION['tripal_pub_import']['days']         : $days;


  // If the form_state has variables then use those.  This happens when an error occurs on the form or the
  // form is resbumitted using AJAX
  if ($form_state['values']) {
    $num_criteria = $form_state['values']['num_criteria'] ? $form_state['values']['num_criteria'] : $num_criteria;
    $loader_name  = $form_state['values']['loader_name']  ? $form_state['values']['loader_name']  : $loader_name;
    $remote_db    = $form_state['values']['remote_db']    ? $form_state['values']['remote_db']    : $remote_db;
    $disabled     = $form_state['values']['disabled']     ? $form_state['values']['disabled']     : $disabled;
    $do_contact   = $form_state['values']['do_contact']   ? $form_state['values']['do_contact']   : $do_contact;
    $days         = $form_state['values']['days']         ? $form_state['values']['days']         : $days;
  }

  // check if the pub_import_id in the session variable is not the same as the one we've been provided
  // if so, then clear the session variable
  if ($pub_import_id and $pub_import_id != $_SESSION['tripal_pub_import']['pub_import_id']) {
    unset($_SESSION['tripal_pub_import']);
  }

   
  // change the number of criteria based on form_state post data.
  if (!$num_criteria) {
    $num_criteria = 1;
  }
  if($form_state['post']["add-$num_criteria"]) {
    $num_criteria++;
  }
  if($form_state['post']["remove-$num_criteria"]) {
    $num_criteria--;
  }

  $form['pub_import_id'] = array(
    '#type'          => 'hidden',
    '#value'         => $pub_import_id,
    '#required'      => TRUE,
  );
  $form['action'] = array(
    '#type'          => 'hidden',
    '#value'         => $action,
    '#required'      => TRUE,
  );

  $form['loader_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Loader Name'),
    '#description'   => t('Please provide a name for this loader setup..'),
    '#default_value' => $loader_name,
    '#required'      => TRUE,
  );

  $supported_dbs = variable_get('tripal_pub_supported_dbs', array());
  $remote_dbs = array();
  $values = array(
    'name' => $supported_dbs,
  );
  $dbs = tripal_core_chado_select('db', array('*'), $values);
  foreach ($dbs as $index => $db) {
    $remote_dbs[$db->name] = $db->description;
  };
  // use PubMed as the default
  if (!$remote_db) {
    $remote_db = 'PMID';
  }
  $form['remote_db'] = array(
    '#title' => t('Remote Database'),
    '#type' => 'select',
    '#options' => $remote_dbs,
    '#default_value' => $remote_db,
    '#ahah' => array(
      'path'    => "admin/tripal/tripal_pub/import/changedb",
      'wrapper' => 'tripal-pub-importer-setup-form',
      'event'   => 'click',
      'method'  => 'replace',
  ),
  );

  $form['num_criteria']= array(
    '#type'          => 'hidden',
    '#default_value' => $num_criteria,
  );
  $form['pub_import_id']= array(
    '#type'          => 'hidden',
    '#default_value' => $pub_import_id,
  );

  $form['days'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Days since record modified'),
    '#description'   => t('Limit the search to include pubs that have been added no more than this many days before today.'),
    '#default_value' => $days,
    '#size'          => 5,
  );
  $form['disabled'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Disabled'),
    '#description'   => t('Check to disable this importer.'),
    '#default_value' => $disabled,
  );
  $form['do_contact'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Create Contact'),
    '#description'   => t('Check to create an entry in the contact table for each author of a matching publication during import. This allows storage of 
       additional information such as affilation, etc. Otherwise, only authors names are retrieved.'),
    '#default_value' => $do_contact,
  );

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

      // if we have criteria supplied from the database then use that as the initial defaults
      if ($criteria) {
        $search_terms = $criteria['criteria'][$i]['search_terms'];
        $scope = $criteria['criteria'][$i]['scope'];
        $is_phrase = $criteria['criteria'][$i]['is_phrase'];
        $operation = $criteria['criteria'][$i]['operation'];
      }
      // if we're here because the form was posted then load from the session variable (we lost the form state)
      $search_terms = isset($_SESSION['tripal_pub_import']['criteria'][$i]['search_terms']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['search_terms'] : $search_terms;
      $scope        = isset($_SESSION['tripal_pub_import']['criteria'][$i]['scope'])        ? $_SESSION['tripal_pub_import']['criteria'][$i]['scope']        : $scope;
      $is_phrase    = isset($_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase'])    ? $_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase']    : $is_phrase;
      $operation    = isset($_SESSION['tripal_pub_import']['criteria'][$i]['operation'])    ? $_SESSION['tripal_pub_import']['criteria'][$i]['operation']    : $operation;
      
      // If the form_state has variables then use those.  This happens when an error occurs on the form or the
      // form is resbumitted using AJAX
      if ($form_state['values']) {
        $search_terms = $form_state['values']["search_terms-$i"];
        $scope        = $form_state['values']["scope-$i"];
        $is_phrase    = $form_state['values']["is_phrase-$i"];
        $operation    = $form_state['values']["operation-$i"];
      }

      $form['criteria'][$i]["search_terms-$i"] = array(
        '#type'          => 'textfield',
        '#description'   => t('Please provide a list of words for searching. You may use 
          conjunctions such as "AND" or "OR" to separate words if they are expected in 
          the same scope, but do not mix ANDs and ORs.  Uncheck the "Is Phrase" checkbox to use conjunctions'),
        '#default_value' => $search_terms,
        '#required'      => TRUE,
        '#maxlength' => 2048,
      );
      $form['criteria'][$i]["scope-$i"] = array(
        '#type'          => 'select',
        '#description'   => t('Please select the fields to search for this term.'),
        '#options'       => $scope_choices,
        '#default_value' => $scope,
      );
      $form['criteria'][$i]["is_phrase-$i"] = array(
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
        $form['criteria'][$i]["operation-$i"] = array(
        '#type'          => 'select',
        '#options'       => $op_choices,
        '#default_value' => $operation,
        );
      }
      if ($i == $num_criteria) {
        if($i > 1) {
          $form['criteria'][$i]["remove-$i"] = array(
          '#type'         => 'image_button',
          '#value'        => t('Remove'),
          '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
          '#ahah' => array(
            'path'    => "admin/tripal/tripal_pub/import/criteria/minus/$i",
            'wrapper' => 'tripal-pub-importer-setup-form',
            'event'   => 'click',
            'method'  => 'replace',
          ),
          '#attributes' => array('onClick' => 'return false;'),
          );
        }
        $form['criteria'][$i]["add-$i"] = array(
        '#type'         => 'image_button',      
        '#value'        => t('Add'),
        '#src'          => drupal_get_path('theme', 'tripal') . '/images/add.png',
        '#ahah' => array(
          'path'    => "admin/tripal/tripal_pub/import/criteria/add/$i",
          'wrapper' => 'tripal-pub-importer-setup-form',
          'event'   => 'click',
          'method'  => 'replace',          
        ),
        '#attributes' => array('onClick' => 'return false;'),
        );
      }
    }

    $form['test'] = array(
    '#type'         => 'submit',
    '#value'        => t('Test Importer'),
    );
    $form['save'] = array(
    '#type'         => 'submit',
    '#value'        => t('Save Importer'),
    );
    /*
     $form['import'] = array(
     '#type'         => 'submit',
     '#value'        => t('Save & Import Now'),
     );*/
    $form['delete'] = array(
    '#type'         => 'submit',
    '#value'        => t('Delete Importer'),
    );

    // allow the selected remote database to make changes to the form if needed
    $callback = "tripal_pub_remote_alter_form_$remote_db";
    $form = call_user_func($callback, $form, $form_state);

    return $form;
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
    $operation =  $form_state['values']["operation-$i"];
    
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
    $operation =  $form_state['values']["operation-$i"];

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
    $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = %d";
    $importer = db_fetch_object(db_query($sql, $pub_import_id));
    if($importer) {
      // do the update
      $record['pub_import_id'] = $pub_import_id;
      if(drupal_write_record('tripal_pub_import', $record, 'pub_import_id')){
        unset($_SESSION['tripal_pub_import']);
        drupal_set_message('Publication import settings updated.');
        drupal_goto('admin/tripal/tripal_pub/import_list');
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
        drupal_goto('admin/tripal/tripal_pub/import_list');
      }
      else {
        drupal_set_message('Could not save publication import settings.', 'error');
      }
    }
  }
  if ($form_state['values']['op'] == 'Delete Importer') {
    $sql = "DELETE FROM {tripal_pub_import} WHERE pub_import_id = %d";
    $success = db_query($sql, $pub_import_id);
    if ($success) {
      drupal_set_message('Publication importer deleted.');
      drupal_goto('admin/tripal/tripal_pub/import_list');
    }
    else {
      drupal_set_message('Could not delete publication importer.', 'error');
    }
  }
}

/*
 *
 */
function tripal_pub_importer_delete($pub_import_id) {
  $sql = "DELETE FROM {tripal_pub_import} WHERE pub_import_id = %d";
  $success = db_query($sql, $pub_import_id);
  if ($success) {
    drupal_set_message('Publication importer deleted.');
    drupal_goto('admin/tripal/tripal_pub/import_list');
  }
  else {
    drupal_set_message('Could not delete publication importer.', 'error');
  }
}
/*
 * AHAH callback
 */
function tripal_pub_importer_setup_page_update_remotedb() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();
  $data = theme('tripal_pub_importer_setup_form', $form);

  // bind javascript events to the new objects that will be returned
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
  array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
  )
  );
}
/*
 * AHAH callback
 */
function tripal_pub_importer_setup_page_update_criteria($action, $i) {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();
  $data = theme('tripal_pub_importer_setup_form', $form);

  // bind javascript events to the new objects that will be returned
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
  array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
  )
  );
}
