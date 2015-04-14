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
    $from_year    = $_SESSION['tripal_pub_search_form']['from_year'];
    $to_year      = $_SESSION['tripal_pub_search_form']['to_year'];
     
    $search_array = array();
    $search_array['num_criteria'] = $num_criteria;
    $search_array['from_year'] = $from_year;
    $search_array['to_year'] = $to_year;
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
      // get the citation for this publication
      $values = array(
        'pub_id' => $pub->pub_id, 
        'type_id' => array(
          'name' => 'Citation',
      ),
      );
      $citation_rec = tripal_core_generate_chado_var('pubprop', $values);
      $citation_rec = tripal_core_expand_chado_vars($citation_rec, 'field', 'pubprop.value');

      // if we have the citation then use it, otherwise, just use the title
      $title = htmlspecialchars($pub->title);
      $result = $title;
      if ($pub->nid) {
        $result = l($title ,'node/' . $pub->nid, array('attributes' => array('target' => '_blank')));
      }
      if ($citation_rec->value) {
        $citation = htmlspecialchars($citation_rec->value);
        $result .= '<br>' . $citation;
      }
      $rows[] = array(
      number_format($i) . ".",
      $pub->pyear,
      $result
      );
      $i++;
    }

    $headers = array('', 'Year', 'Publication');
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
  $from_year = $_SESSION['tripal_pub_search_form']['from_year'] ? $_SESSION['tripal_pub_search_form']['from_year'] : '';
  $to_year = $_SESSION['tripal_pub_search_form']['to_year'] ? $_SESSION['tripal_pub_search_form']['to_year'] : '';


  // If the form_state has variables then use those.  This happens when an error occurs on the form or the
  // form is resbumitted using AJAX
  $num_criteria = $form_state['values']['num_criteria'] ? $form_state['values']['num_criteria'] : $num_criteria;

   
  // change the number of criteria based on form_state post data.
  if (!$num_criteria) {
    $num_criteria = 2;
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
  $form['instructions'] = array(
    '#type'  => 'item',
    '#value' => t('To search for publications enter keywords in the text boxes below.  You can limit your search by selecting the field in the dropdown box. Click the plus and minus symbols to add additional fields for searching. '),
  );

  // get publication properties list
  $properties = array();
  $properties[] = 'Any Field';
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
  $allowed_fields = variable_get('tripal_pub_allowed_search_fields', array());
  $prop_types = chado_query($sql);
  while ($prop = db_fetch_object($prop_types)) {
    if($allowed_fields[$prop->cvterm_id] > 0) {
      $properties[$prop->cvterm_id] = $prop->name;
    }
  }

  for($i = 1; $i <= $num_criteria; $i++) {
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
      '#default_value' => $search_terms,
      '#required'      => FALSE,
    );
    $form['criteria'][$i]["scope-$i"] = array(
      '#type'          => 'select',
      '#options'       => $properties,
      '#default_value' => $scope,
      '#attributes' => array('class' => 'tripal-pub-search-form-scope-select'),
    );
    /*
     $form['criteria'][$i]["mode-$i"] = array(
     '#type'          => 'select',
     '#options'       => array(
     'Contains'    => 'Contains',
     'Starts With' => 'Starts With',
     'Ends With'   => 'Ends With',
     'Exactly'     => 'Exactly'),
     '#default_value' => $mode,
     );*/

    if ($i > 1) {
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
      if($i > 1) {
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
  $form['criteria']["date"] = array(
      '#type'          => 'select',
      '#options'       => array('Years' => 'Years'),
      '#attributes' => array('class' => 'tripal-pub-search-form-scope-select'),
  );
  $form['criteria']["from_year"] = array(
    '#type'          => 'textfield',
    '#default_value' => $from_year,
    '#required'      => FALSE,
    '#title'         => 'from',
    '#size'          => 4,
    '#maxlength'     => 4,
  );
  $form['criteria']["to_year"] = array(
    '#type'          => 'textfield',
    '#default_value' => $to_year,
    '#required'      => FALSE,
    '#title'         => 'to',
    '#size'          => 4,
    '#maxlength'     => 4,
  );

  $form['search'] = array(
    '#type'         => 'submit',
    '#value'        => t('Search'),
  );
  $form['reset'] = array(
    '#type'         => 'submit',
    '#value'        => t('Reset'),
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
      drupal_render($element["operation-$i"]),
      drupal_render($element["scope-$i"]),
      //drupal_render($element["mode-$i"]) .
      drupal_render($element["search_terms-$i"]),
      drupal_render($element["add-$i"]) . drupal_render($element["remove-$i"]),
      );
    }
  }

  $rows[] = array(
    '&nbsp;',
  drupal_render($form['criteria']['date']),
  array(
      'data' =>
        "<div id=\"pub-search-form-dates-row\"> 
           <div id=\"pub-search-form-dates\"> ".
  drupal_render($form['criteria']['from_year']) .
  drupal_render($form['criteria']['to_year']) . "
          </div>
         </div>",
  ),
    ''
    );
    $table = theme('table', $headers, $rows, array('id' => 'tripal-pub-search-form-table', 'border' => '0'));
    $headers = array();
    $markup  = drupal_render($form['instructions']) . "
     <div id=\"pub-search-form-row1\">$table</div>
     <div style=\"clear: both;\"></div>
  "; 

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
function tripal_pub_search_form_validate($form, &$form_state) {
  $num_criteria = $form_state['values']['num_criteria'];
  $from_year = $form_state['values']['from_year'];
  $to_year   = $form_state['values']['to_year'];
  $op        = $form_state['values']['op'];

  // no need to vlaidate on a reset
  if ($op == 'Reset') {
    return;
  }

  if($from_year and !$to_year) {
    form_set_error('to_year', 'Please provide a 4-digit year.');
  }
  if(!$from_year and $to_year) {
    form_set_error('from_year', 'Please provide a 4-digit year.');
  }
  if($from_year and !preg_match('/\d\d\d\d/' , $from_year)) {
    form_set_error('from_year', 'Please provide a 4-digit year.');
  }
  if($to_year and !preg_match('/\d\d\d\d/' , $to_year)) {
    form_set_error('to_year', 'Please provide a 4-digit year.');
  }
}
/**
 *
 */
function tripal_pub_search_form_submit($form, &$form_state) {
  $num_criteria = $form_state['values']['num_criteria'];
  $from_year    = $form_state['values']['from_year'];
  $to_year      = $form_state['values']['to_year'];
  $op           = $form_state['values']['op'];

  // set the session variables
  if($op == 'Search') {
    $_SESSION['tripal_pub_search_form']['num_criteria'] = $num_criteria;
    unset($_SESSION['tripal_pub_search_form']['criteria']);
    for ($i = 0; $i <= $num_criteria; $i++) {
      $search_terms =  trim($form_state['values']["search_terms-$i"]);
      $scope =  $form_state['values']["scope-$i"];
      //$mode =  $form_state['values']["mode-$i"];
      $mode = 'Contains';
      $operation =  $form_state['values']["operation-$i"];

      $_SESSION['tripal_pub_search_form']['criteria'][$i] = array(
        'search_terms' => $search_terms,
        'scope' => $scope,
        'mode' => $mode,
        'operation' => $operation
      );
    }
    $_SESSION['tripal_pub_search_form']['from_year'] = $from_year;
    $_SESSION['tripal_pub_search_form']['to_year'] = $to_year;
    $_SESSION['tripal_pub_search_form']['perform_search'] = 1;
  }
  if($op == 'Reset') {
    unset($_SESSION['tripal_pub_search_form']);
  }
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
               INNER JOIN {cvterm} CVT on CVT.cvterm_id = P.type_id
            ";
  $where  = "WHERE (NOT P.title = 'null') "; // always exclude the dummy pub
  $order  = "ORDER BY P.pyear DESC, P.title ASC";
  $fargs = array(); // arguments for from, inner join clause
  $wargs = array();  // arguments for where clause
  $join = 0;

  $num_criteria = $search_array['num_criteria'];
  $from_year    = $search_array['from_year'];
  $to_year      = $search_array['to_year'];

  for ($i = 1; $i <= $num_criteria; $i++) {
    $value = $search_array['criteria'][$i]['search_terms'];
    $type_id = $search_array['criteria'][$i]['scope'];
    $mode = $search_array['criteria'][$i]['mode'];
    $op = $search_array['criteria'][$i]['operation'];

    // skip criteria with no values
    if(!$value) {
      continue;
    }

    // to prevent SQL injection make sure our operator is
    // what we expect
    if ($op and $op != "AND" and $op != "OR" and $op != 'NOT') {
      $op = 'AND';
    }
    if ($op == 'NOT') {
      $op = 'AND NOT';
    }
    if (!$op) {
      $op = 'AND';
    }

    $action = "= lower('%s')";
    if($mode == 'Contains') {
      $action = 'LIKE lower(\'%%%s%%\')';
    }
    if($mode == 'Starts With') {
      $action = '= lower(\'%%%s\')';
    }
    if($mode == 'Ends With') {
      $action = '= lower(\'%s%%\')';
    }
     
    // get the scope type
    $values = array('cvterm_id' => $type_id);
    $cvterm = tripal_core_chado_select('cvterm', array('name'), $values);
    $type_name = $cvterm[0]->name;
    if ($type_name == 'Title') {
      $where .= " $op (lower(P.title)  $action) ";
      $wargs[] = $value;
    }
    elseif ($type_name == 'Year') {
      $where .= " $op (lower(P.pyear)  $action) ";
      $wargs[] = $value;
    }
    elseif ($type_name == 'Volume') {
      $where .= " $op (lower(P.volume)  $action) ";
      $wargs[] = $value;
    }
    elseif ($type_name == 'Issue') {
      $where .= " $op (lower(P.issue)  $action)";
      $wargs[] = $value;
    }
    elseif ($type_name == 'Journal Name') {
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id AND PP$i.type_id = %d ";
      $where .= " $op ((lower(P.series_name) $action and CVT.name = 'Journal Article') OR
                       (lower(PP$i.value) $action)) ";
      $fargs[] = $type_id;
      $wargs[] = $value;
      $wargs[] = $value;
    }
    elseif ($type_name == 'Conference Name') {
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id AND PP$i.type_id = %d ";
      $where .= " $op ((lower(P.series_name) $action and CVT.name = 'Conference Proceedings') OR
                       (lower(PP$i.value) $action)) ";
      $fargs[] = $type_id;
      $wargs[] = $value;
      $wargs[] = $value;
    }
    elseif ($type_name == 'Publication Type') {
      $where .= " $op (lower(CVT.name) $action)";
      $wargs[] = $value;
    }
    elseif ($type_id == 0) { //'Any Field'
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id ";
      $where .= " $op (lower(PP$i.value)  $action OR
                       lower(P.title) $action OR 
                       lower(P.volumetitle) $action OR
                       lower(P.publisher) $action OR
                       lower(P.uniquename) $action OR
                       lower(P.pubplace) $action OR
                       lower(P.miniref) $action OR
                       lower(P.series_name) $action) ";
      $wargs[] = $value;
      $wargs[] = $value;
      $wargs[] = $value;
      $wargs[] = $value;
      $wargs[] = $value;
      $wargs[] = $value;
      $wargs[] = $value;
      $wargs[] = $value;
    }
    // for all other properties
    else {
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id AND PP$i.type_id = %d ";
      $where .= " $op (lower(PP$i.value) $action) ";
      $fargs[] = $type_id;
      $wargs[] = $value;
    }

  }
  if($from_year and $to_year) {
    $where .= " AND (P.pyear ~ '....' AND to_number(P.pyear,'9999') >= %d AND to_number(P.pyear,'9999') <= %d) ";
    $wargs[] = $from_year;
    $wargs[] = $to_year;
  }
  $sql = "$select $from $where $order";
  $count = "SELECT count(*) FROM ($select $from $where $order) as t1";
  $args = array_merge($fargs, $wargs);
  //dpm(array($mode, $sql, $args));
  return chado_pager_query($sql, $limit, $pager_id, $count, $args);
}
