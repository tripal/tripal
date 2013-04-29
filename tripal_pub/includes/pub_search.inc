<?php
/*
 * 
 */
function tripal_pub_search_page() {
  global $pager_total, $pager_total_items;
  
  $pager_id = 0;
  $limit = 25;
  
  // generate the search form 
  $form = drupal_get_form('tripal_pub_search_form');  
  $output .= $form;

  // retrieve any results       
  if ($_SESSION['tripal_pub_search_form']['perform_search']) {
    $num_criteria = $_SESSION['tripal_pub_search_form']['num_criteria'];
     
    $search_array = array();
    $search_array['num_criteria'] = $num_criteria;
    for ($i = 0; $i <= $num_criteria; $i++) {
      $search_array['criteria'][$i]['search_terms'] = $_SESSION['tripal_pub_search_form']['criteria'][$i]['search_terms'];
      $search_array['criteria'][$i]['scope']        = $_SESSION['tripal_pub_search_form']['criteria'][$i]['scope'];  
      $search_array['criteria'][$i]['mode']         = $_SESSION['tripal_pub_search_form']['criteria'][$i]['mode'];
      $search_array['criteria'][$i]['operation']    = $_SESSION['tripal_pub_search_form']['criteria'][$i]['operation'];
    }
  
    // get the list of publications from the remote database using the search criteria.  
    $pubs = tripal_pub_get_search_results($search_array, $limit, $pager_id);
  
    // generate the pager
    $total_pages = $pager_total[$pager_id];
    $total_items = $pager_total_items[$pager_id];
    $page = isset($_GET['page']) ? $_GET['page'] : '0';
    $pager = theme('pager');
    
    // iterate through the results and construct the table displaying the publications
    $rows = array();
    $i = $page * $limit + 1;
    while($pub = db_fetch_object($pubs)) {
      $title =  $pub->title;
      if ($pub->nid) {
        $title = l($pub->title,'node/' . $pub->nid, array('attributes' => array('target' => '_blank')));
      }
      $rows[] = array(number_format($i), $pub->pyear, $title);
      $i++;
    }

    $headers = array('', 'Year', 'Title');
    $table = theme('table', $headers, $rows);   
  
    // join all to form the results
    $output .= "<br><p><b>Found " . number_format($total_items) .  
      ". Page " . ($page + 1) . " of $total_pages. " .
      " Results</b></br>" . $table . '</p>' . $pager;    
  }
  return $output;
}
/**
 * Purpose: Provides the form to search pubmed
 *
  * @ingroup tripal_pub
 */
function tripal_pub_search_form(&$form_state = NULL) {
  tripal_core_ahah_init_form();
 
  // Set the default values. If the pub_import_id isn't already defined by the form values 
  // and one is provided then look it up in the database
  $criteria = NULL;

  // if the session has variables then use those.  This should only happen when
  // the 'Test Criteria' button is clicked.
  $num_criteria = $_SESSION['tripal_pub_search_form']['num_criteria'] ? $_SESSION['tripal_pub_search_form']['num_criteria'] : $num_criteria;    
 
  
  // If the form_state has variables then use those.  This happens when an error occurs on the form or the 
  // form is resbumitted using AJAX
  $num_criteria = $form_state['values']['num_criteria'] ? $form_state['values']['num_criteria'] : $num_criteria;    
  
   
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
  
  $form['num_criteria']= array(
    '#type'          => 'hidden',
    '#default_value' => $num_criteria,
  );
  
  // get publication properties list 
  $properties = array();
  $properties[] = 'Any Field';
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
  while ($prop = db_fetch_object($prop_types)) {
    $properties[$prop->cvterm_id] = $prop->name;
  }
  
  for($i = 0; $i <= $num_criteria; $i++) {
    $search_terms = '';
    $scope = '';
    $operation = '';
    $mode = '';
    // if we have criteria supplied from the database then use that, othrewise look from the form_state or the session
    if ($criteria) {
      $search_terms = $criteria['criteria'][$i]['search_terms'];
      $scope = $criteria['criteria'][$i]['scope'];
      $mode = $criteria['criteria'][$i]['mode'];
      $operation = $criteria['criteria'][$i]['operation'];
    }
    // first populate defaults using any values in the SESSION variable
    $search_terms = $_SESSION['tripal_pub_search_form']['criteria'][$i]['search_terms'] ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['search_terms'] : $search_terms;
    $scope        = $_SESSION['tripal_pub_search_form']['criteria'][$i]['scope']        ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['scope']        : $scope;
    $mode         = $_SESSION['tripal_pub_search_form']['criteria'][$i]['mode']         ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['mode']         : $mode;
    $operation    = $_SESSION['tripal_pub_search_form']['criteria'][$i]['operation']    ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['operation']    : $operation;
    
    // next populate defaults using any form values
    $search_terms = $form_state['values']["search_terms-$i"] ? $form_state['values']["search_terms-$i"] : $search_terms;
    $scope        = $form_state['values']["scope-$i"]        ? $form_state['values']["scope-$i"]        : $scope;
    $mode         = $form_state['values']["mode-$i"]         ? $form_state['values']["mode-$i"]         : $mode;
    $operation    = $form_state['values']["operation-$i"]    ? $form_state['values']["operation-$i"]    : $operation;
    
    // default to searching the title and abstract
    if (!$scope) {
      $scope = 'abstract';
    }
  
    $form['criteria'][$i]["search_terms-$i"] = array(
      '#type'          => 'textfield',
      '#description'   => t('Please provide a list of words for searching"'),
      '#default_value' => $search_terms,
      '#required'      => TRUE,
    );
    $form['criteria'][$i]["scope-$i"] = array(
      '#type'          => 'select',
      '#description'   => t('Please select the fields to search for this term.'),
      '#options'       => $properties,
      '#default_value' => $scope,
    );  
    $form['criteria'][$i]["mode-$i"] = array(
      '#type'          => 'select',
      '#options'       => array(
        'Contains'    => 'Contains',
        'Starts With' => 'Starts With',
        'Ends With'   => 'Ends With',
        'Exactly'     => 'Exactly'),
      '#default_value' => $mode,
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
            'path'    => "find/publications/criteria/minus/$i",
            'wrapper' => 'tripal-pub-search-form',
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
          'path'    => "find/publications/criteria/add/$i",
          'wrapper' => 'tripal-pub-search-form',
          'event'   => 'click',
          'method'  => 'replace',          
        ),   
        '#attributes' => array('onClick' => 'return false;'),
      );
    }
  }
  
  $form['search'] = array(
    '#type'         => 'submit',
    '#value'        => t('Search'),
  );

  return $form;
}
/*
 * 
 */
function theme_tripal_pub_search_form($form) {
  $rows = array();
  foreach ($form['criteria'] as $i => $element) {
    if(is_numeric($i)) {
      $rows[] = array(  
        array('data' => drupal_render($element["operation-$i"]), 'width' => '10%'),    
        array('data' => drupal_render($element["scope-$i"]), 'width' => '10%'),
        drupal_render($element["mode-$i"]) . drupal_render($element["search_terms-$i"]),
        array('data' => drupal_render($element["add-$i"]) . drupal_render($element["remove-$i"]), 'width' => '5%'),
      );
    }
  } 
  $headers = array('Operation','Scope', 'Search Terms', '');  
  $markup = theme('table', $headers, $rows, array('id' => 'tripal-pub-search-table'));
  
  $form['criteria'] = array(
    '#type' => 'markup',
    '#value' =>  $markup,
    '#weight' => -10,
  );
  return drupal_render($form);
}
/**
 *
 */
function tripal_pub_search_form_submit($form, &$form_state) {
   
  $num_criteria = $form_state['values']['num_criteria'];

  // set the session variables
  $_SESSION['tripal_pub_search_form']['num_criteria'] = $num_criteria;  
  unset($_SESSION['tripal_pub_search_form']['criteria']);
  for ($i = 0; $i <= $num_criteria; $i++) {
    $search_terms =  trim($form_state['values']["search_terms-$i"]);
    $scope =  $form_state['values']["scope-$i"];
    $mode =  $form_state['values']["mode-$i"];
    $operation =  $form_state['values']["operation-$i"];
    
    $_SESSION['tripal_pub_search_form']['criteria'][$i] = array(
      'search_terms' => $search_terms,
      'scope' => $scope,
      'mode' => $mode,
      'operation' => $operation
    );
  }
  
  $_SESSION['tripal_pub_search_form']['perform_search'] = 1;
}

/*
 * AHAH callback
 */
function tripal_pub_search_page_update_criteria($action, $i) {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();   
  $data = theme('tripal_pub_search_form', $form);  

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
 * 
 */
function tripal_pub_get_search_results($search_array, $limit, $pager_id) {

  // build the SQL based on the criteria provided by the user
  $select = "SELECT DISTINCT P.*, CP.nid ";
  $from   = "FROM {pub} P
               LEFT JOIN public.chado_pub CP on P.pub_id = CP.pub_id 
            ";
  $where  = "WHERE ";
  $order  = "ORDER BY P.pyear DESC, P.title ASC";
  $args = array();
  $join = 0;
  
  $num_criteria = $search_array['num_criteria'];
  for ($i = 0; $i <= $num_criteria; $i++) {
    $value = $search_array['criteria'][$i]['search_terms'];
    $type_id = $search_array['criteria'][$i]['scope'];
    $mode = $search_array['criteria'][$i]['mode'];
    $op = $search_array['criteria'][$i]['operation'];
    
    // to prevent SQL injection make sure our operator is
    // what we expect    
    if ($op and $op != "AND" and $op != "OR" and $op != 'NOT') {
      $op = 'AND';
    }
    
    $action = "= lower('%s')";
    if($mode == 'Contains') {
      $action = "LIKE lower('%%%s%%')";
    }
    if($mode == 'Starts With') {
      $action = "= lower('%%%s')";  
    }
    if($mode == 'Ends With') {
      $action = "= lower('%s%%')";  
    }
   
    // get the scope type
    $values = array('cvterm_id' => $type_id);
    $cvterm = tripal_core_chado_select('cvterm', array('name'), $values);
    $type_name = $cvterm[0]->name;
    if ($type_name == 'Title') {
      $where .= " $op (lower(P.title)  $action) ";
      $args[] = $value;
    }
    elseif ($type_name == 'Year') {
      $where .= " $op (lower(P.pyear)  $action) ";
      $args[] = $value;
    }
    elseif ($type_name == 'Volume') {
      $where .= " $op (lower(P.volume)  $action) ";
      $args[] = $value;
    }
    elseif ($type_name == 'Issue') {
      $where .= " $op (lower(P.issue)  $action)";
      $args[] = $value;
    }
    elseif ($type_id == 0) { //'Any Field'
      $join = 1;
      $where .= " $op ((lower(PP.value)  $action) OR (lower(P.title) $action)) ";
      $args[] = $value;
      $args[] = $value;  
    }
    else {
      $join = 1;
      $where .= " $op (lower(PP.value) $action AND PP.type_id = %d) ";
      $args[] = $value;
      $args[] = $type_id;
    }

  }  
  if ($join) {
    $from .= " INNER JOIN {pubprop} PP ON PP.pub_id = P.pub_id ";    
  }
  $sql = "$select $from $where $order";
  $count = "SELECT count(*) FROM ($select $from $where $order) as t1";
  //dpm(array($mode, $sql, $args));
  return chado_pager_query($sql, $limit, $pager_id, $count, $args);
}