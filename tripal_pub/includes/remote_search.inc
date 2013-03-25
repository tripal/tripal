<?php

/*
 * 
 */
function tripal_pub_remote_search_page() {
  global $pager_total, $pager_total_items;
  
  $pager_id = 0;
  $limit = 10;
  
  // generate the search form 
  $form = drupal_get_form('tripal_pub_remote_search_form');

  // retrieve any results
  $remote_db = $_SESSION['tripal_pub_search']['remote_db'];
  $num_criteria = $_SESSION['tripal_pub_search']['num_criteria'];
  $days = $_SESSION['tripal_pub_search']['days'];
 
  $search_array = array();
  $search_array['remote_db'] = $remote_db;
  $search_array['num_criteria'] = $num_criteria;
  $search_array['days'] = $days; 
  for ($i = 0; $i <= $num_criteria; $i++) {
    $search_array['criteria'][$i]['search_terms'] = $_SESSION['tripal_pub_search']['criteria'][$i]['search_terms'];
    $search_array['criteria'][$i]['scope'] = $_SESSION['tripal_pub_search']['criteria'][$i]['scope'];  
    $search_array['criteria'][$i]['operation'] = $_SESSION['tripal_pub_search']['criteria'][$i]['operation'];
  }
    
  // get the list of publications from the remote database using the search criteria.
  $pubs = tripal_pub_get_remote_search_results($remote_db, $search_array, $limit, $pager_id);

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
      $rows[] = array(number_format($i), $pub['citation']);
      $i++;
    }
  }
  $headers = array('', 'Citation');
  $table = theme('table', $headers, $rows);
  
  
  // join all to form the final page
  $output =  $form . "<p><b>Found " . number_format($total_items) .  
    ". Page " . ($page + 1) . " of $total_pages. " .
    " Results</b></br>" . $table . '</p>' . $pager;    
  
  return $output;
}
/*
 * 
 */
function theme_tripal_pub_remote_search_form($form) {
  $rows = array();
  foreach ($form['criteria'] as $i => $element) {
    if(is_numeric($i)) {
      $rows[] = array(  
        array('data' => drupal_render($element["operation-$i"]), 'width' => '10%'),    
        array('data' => drupal_render($element["scope-$i"]), 'width' => '10%'),
        drupal_render($element["search_terms-$i"]),
        array('data' => drupal_render($element["add-$i"]) . drupal_render($element["remove-$i"]), 'width' => '5%'),
      );
    }
  }
  $headers = array('Operation','Scope', 'Search Terms', '');
  $form['criteria'] = array(
    '#type' => 'markup',
    '#value' =>  theme('table', $headers, $rows),
    '#weight' => -10,
  );
  return drupal_render($form);
}
/**
 * Purpose: Provides the form to search pubmed
 *
  * @ingroup tripal_pub
 */
function tripal_pub_remote_search_form(&$form_state = NULL) {
  tripal_core_ahah_init_form();

  // set default values.  First use what is in the form_state['values'] array. If nothing there then use the session variable.
  $remote_db = $form_state['values']['remote_db'] ? $form_state['values']['remote_db'] : $_SESSION['tripal_pub_search']['remote_db'];
  $days = $form_state['values']['days'] ? $form_state['values']['days'] : $_SESSION['tripal_pub_search']['days']; 
  $num_criteria = $form_state['values']['num_criteria'] ? $form_state['values']['num_criteria'] : $_SESSION['tripal_pub_search']['num_criteria'];
  $loader_name = $form_state['values']['loader_name'];
  
  // change the number of criteria based on form_state post data.
  if (!$num_criteria) {
    $num_criteria = 0;
  }
  if($form_state['post']["add-$num_criteria"]) {    
    $num_criteria++;
  }
  if($form_state['post']["remove-$num_criteria"]) {    
    $num_criteria--;
  }
  
  $form['loader_name'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Loader Name'),
    '#description'   => t('Please provide a name for this loader setup..'),
    '#default_value' => $loader_name,
  );
   
  $remote_dbs = array('Pubmed' => 'Pubmed');
  $form['remote_db'] = array(
    '#title' => t('Remote Publication Database'),
    '#type' => 'select',
    '#options' => $remote_dbs,
    '#default_value' => $remote_db,
  );

  $form['num_criteria']= array(
    '#type'          => 'hidden',
    '#default_value' => $num_criteria,
  );
  
  for($i = 0; $i <= $num_criteria; $i++) {
    $search_terms = $form_state['values']["search_terms-$i"] ? $form_state['values']["search_terms-$i"] : $_SESSION['tripal_pub_search']['criteria'][$i]['search_terms'];
    $scope = $form_state['values']["scope-$i"] ? $form_state['values']["scope-$i"] : $_SESSION['tripal_pub_search']['criteria'][$i]['scope'];
    $operation = $form_state['values']["operation-$i"] ? $form_state['values']["operation-$i"] : $_SESSION['tripal_pub_search']['criteria'][$i]['operation'];
    
    // default to searching the title and abstract
    if (!$scope) {
      $scope = 'abstract';
    }
  
    $form['criteria'][$i]["search_terms-$i"] = array(
      '#type'          => 'textfield',
      '#description'   => t('Please provide a list of words, separated by spaces for searching.'),
      '#default_value' => $search_terms,
    );
    $form['criteria'][$i]["scope-$i"] = array(
      '#type'          => 'select',
      '#description'   => t('Please select the fields to search for this term.'),
      '#options'       => array(
        'any'      => 'Any Field',
        'title'    => 'Title',
        'abstract' => 'Title/Abstract',
        'author'   => 'Author'),
      '#default_value' => $scope,
    );    
    
    if ($i > 0) {
      $form['criteria'][$i]["operation-$i"] = array(
        '#type'          => 'select',
        '#options'       => array(
          'AND' => 'AND',
          'OR'  => 'OR',
          'NOT' => 'NOT'),
        '#default_value' => $operation,
      );
    }
    if ($i == $num_criteria) {    
      if($i > 0) {
        $form['criteria'][$i]["remove-$i"] = array(
          '#type'         => 'image_button',
          '#value'        => t('Remove'),
          '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
          '#ahah' => array(
            'path'    => "admin/tripal/tripal_pub/import_setup/criteria/minus/$i",
            'wrapper' => 'tripal-pub-remote-search-form',
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
          'path'    => "admin/tripal/tripal_pub/import_setup/criteria/add/$i",
          'wrapper' => 'tripal-pub-remote-search-form',
          'event'   => 'click',
          'method'  => 'replace',          
        ),   
        '#attributes' => array('onClick' => 'return false;'),
      );
    }
  }

  
  $form['days'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Number of Days'),
    '#description'   => t('Please provide the number of days to limit your search.  For example, to search only the last 60 days, enter the number 60 in the field.'),
    '#default_value' => $days,
  );
  
  $form['submit'] = array(
    '#type'         => 'submit',
    '#value'        => t('Test Criteria'),
  );
  $form['submit'] = array(
    '#type'         => 'submit',
    '#value'        => t('Save Criteria'),
  );

  return $form;
}

/**
 *
 */
function tripal_pub_remote_search_form_validate($form, &$form_state) {
  $num_criteria = $form_state['values']['num_criteria'];
  $remote_db =  $form_state['values']["remote_db"];
  $days =  trim($form_state['values']["days"]);

  for ($i = 0; $i <= $num_criteria; $i++) {            
    $search_terms =  trim($form_state['values']["search_terms-$i"]);
    $scope =  $form_state['values']["scope-$i"];
    $operation =  $form_state['values']["operation-$i"];
    
    if ($days and !is_numeric($days) or preg_match('/\./', $days)) {
      form_set_error("days-$i", "Please enter a numeric, non decimal value, for the number of days.");
    }
    if (!$search_terms) {
      form_set_error("search_terms-$i", "Please enter a value for searching.");
    }
  }
}

/**
 *
 */
function tripal_pub_remote_search_form_submit($form, &$form_state) {
 
  $num_criteria = $form_state['values']['num_criteria'];
  $remote_db =  $form_state['values']["remote_db"];
  $days =  trim($form_state['values']["days"]);
    
  $_SESSION['tripal_pub_search']['remote_db'] = $remote_db;
  $_SESSION['tripal_pub_search']['days'] = $days;
  $_SESSION['tripal_pub_search']['num_criteria'] = $num_criteria;
  for ($i = 0; $i <= $num_criteria; $i++) {
    $search_terms =  trim($form_state['values']["search_terms-$i"]);
    $scope =  $form_state['values']["scope-$i"];
    $operation =  $form_state['values']["operation-$i"];
    
    $_SESSION['tripal_pub_search']['criteria'][$i] = array(
      'search_terms' => $search_terms,
      'scope' => $scope,
      'operation' => $operation
    );
  } 
}
/*
 * 
 */
function tripal_pub_get_remote_search_results($remote_db, $search_array, $num_to_retrieve, $pager_id) {
   
  // construct the callback function using the remote database name
  $callback = 'tripal_pub_remote_search_' . strtolower($remote_db);
   
  // now call the callback function to get the rsults
  $pubs = array();
  if (function_exists($callback)) {
    $pubs = call_user_func($callback, $search_array, $num_to_retrieve, $pager_id);
  }    
  
  return $pubs;  
}

/*
 * AHAH callback
 */
function tripal_pub_remote_search_page_update_criteria($action, $i) {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = theme('tripal_pub_remote_search_form', $form);  

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