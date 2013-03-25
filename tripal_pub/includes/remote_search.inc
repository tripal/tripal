<?php
/*
 * 
 */
function theme_tripal_pub_remote_search_form($form) {
  $rows = array(
    array(            
      drupal_render($form['scope']),
      drupal_render($form['operation']),
      drupal_render($form['search_terms']),
    ),
  );
  $headers = array('Search Terms', 'Scope', 'Operation');
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
  $remote_db = $_SESSION['tripal_pub_search_criteria']['remote_db'];
  $search_terms = $_SESSION['tripal_pub_search_criteria']['search_terms'];
  $days = $_SESSION['tripal_pub_search_criteria']['days'];
  $scope = $_SESSION['tripal_pub_search_criteria']['scope'];
  $operation = $_SESSION['tripal_pub_search_criteria']['operation'];
  
  $remote_dbs = array('Pubmed' => 'Pubmed');
  $form['remote_db'] = array(
    '#title' => t('Remote Publication Database'),
    '#type' => 'select',
    '#options' => $remote_dbs,
    '#default_value' => $remote_db,
  );

  $form['search_terms']= array(
    '#type'          => 'textfield',
    '#description'   => t('Please provide a list of words, separated by spaces for searching.'),
    '#default_value' => $search_terms,
  );
  $form['scope']= array(
    '#type'          => 'select',
    '#description'   => t('Please select the fields to search for this term.'),
    '#options'       => array('title'    => 'Title',
                              'abstract' => 'Title/Abstract',
                              'author'   => 'Author'),
    '#default_value' => $scope,
  );
  $form['operation']= array(
    '#type'          => 'select',
    '#options'       => array('AND' => 'AND',
                              'OR'  => 'OR',
                              'NOT' => 'NOT'),
    '#default_value' => $operation,
  );

  $form['days']= array(
    '#type'          => 'textfield',
    '#title'         => t('Number of Days'),
    '#description'   => t('Please provide the number of days to limit your search.  For example, to search only the last 60 days, enter the number 60 in the field.'),
    '#default_value' => $days,
  );
  
  $form['submit'] = array(
    '#type'         => 'submit',
    '#value'        => t('Submit'),
    '#executes_submit_callback' => TRUE,
  );

  return $form;
}

/**
 *
 */
function tripal_pub_remote_search_form_validate($form, &$form_state) {
  $remote_db =  $form_state['values']['remote_db'];
  $search_terms =  $form_state['values']['search_terms'];
  $scope =  $form_state['values']['scope'];
  $operation =  $form_state['values']['operation'];
  $days =  $form_state['values']['days'];
  
  if ($days and !is_numeric($days) or preg_match('/\./', $days)) {
    form_set_error('days', "Please enter a numeric, non decimal value, for the number of days.");
  }
}

/**
 *
 */
function tripal_pub_remote_search_form_submit($form, &$form_state) {
  
  $remote_db =  $form_state['values']['remote_db'];
  $search_terms =  $form_state['values']['search_terms'];
  $scope =  $form_state['values']['scope'];
  $operation =  $form_state['values']['operation'];
  $days =  $form_state['values']['days'];
  
  // store the search settings in a session variable. Then when the page
  // is refreshed these values will be available for the page to 
  // generate results. 
  if ($op == 'Reset') {   
    unset($_SESSION['tripal_pub_search_criteria']); 
    unset($form_state['values']);
  }
  else {
    $_SESSION['tripal_pub_search_criteria'] = array(
      'remote_db' => $remote_db,
      'search_terms' => $search_terms,
      'scope' => $scope,
      'operation' => $operation,
      'days' => $days
    );
  }  
}
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
  $remote_db = $_SESSION['tripal_pub_search_criteria']['remote_db'];
  
  $search_array['search_terms'] = $_SESSION['tripal_pub_search_criteria']['search_terms'];
  $search_array['days'] = $_SESSION['tripal_pub_search_criteria']['days'];  
  $search_array['scope'] = $_SESSION['tripal_pub_search_criteria']['scope'];  
  $search_array['operation'] = $_SESSION['tripal_pub_search_criteria']['operation'];
    
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
  $output = $form . "<p><b>Found " . number_format($total_items) .  
    ". Page " . ($page + 1) . " of $total_pages. " .
    " Results</b></br>" . $table . '</p>' . $pager;    
  
  return $output;
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

