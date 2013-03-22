<?php
/*
 * 
 */
function tripal_pub_remote_search_page() {
  global $pager_total;
  
  // generate the search form 
  $form = drupal_get_form('tripal_pub_remote_search_form');

  // retrieve any results
  $remote_db = $_SESSION['tripal_pub_search_criteria']['remote_db'];
  $search_terms = $_SESSION['tripal_pub_search_criteria']['search_terms'];  
  $results = tripal_pub_get_remote_search_results($remote_db, $search_terms, 10, 0);
  
  // iterate through the results and construct the table displaying the publications
  if (count($results) > 0) {
  
  }
  
  // generate the pager
  $total_items = $pager_total[0];
  $pager = theme('pager');
  
  // join all to form the final page
  $output = $form . "<p><b>Found " . number_format($total_items) . " Results</b></br>" . $table . '</p>' . $pager;
    dpm($results);
  
  return $output;
}
/*
 * 
 */
function tripal_pub_get_remote_search_results($remote_db, $search_terms, $num_to_retrieve, $pager_id) {
 
  $results = array();
  $callback = 'tripal_pub_remote_search_' . strtolower($remote_db);
  if (function_exists($callback) and $search_terms) {
    $results = call_user_func($callback, $search_terms, $num_to_retrieve, $pager_id);
  }    
  
  return $results;  
}
/**
 * Purpose: Provides the form to search pubmed
 *
  * @ingroup tripal_pub
 */
function tripal_pub_remote_search_form(&$form_state = NULL) {
  $remote_db = $_SESSION['tripal_pub_search_criteria']['remote_db'];
  $search_terms = $_SESSION['tripal_pub_search_criteria']['search_terms'];
  
  $remote_dbs = array('Pubmed' => 'Pubmed');
  $form['remote_db'] = array(
    '#title' => t('Remote Publication Database'),
    '#type' => 'select',
    '#options' => $remote_dbs,
    '#default_value' => $remote_db,
  );

  $form['search_terms']= array(
    '#type'          => 'textfield',
    '#title'         => t('Search Terms'),
    '#description'   => t('Please provide a list of words, separated by spaces for searching'),
    '#default_value' => $search_terms,
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
}

/**
 *
 */
function tripal_pub_remote_search_form_submit($form, &$form_state) {
  $remote_db =  $form_state['values']['remote_db'];
  $search_terms =  $form_state['values']['search_terms'];
  
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
    );
  }  
}
