<?php
/*
 *
 */
function tripal_pub_search_page() {

  $limit = 25;

  // generate the search form
  $form = drupal_get_form('tripal_pub_search_form');
  $output = drupal_render($form);

  // retrieve any results
  if (array_key_exists('tripal_pub_search_form', $_SESSION) and 
      $_SESSION['tripal_pub_search_form']['perform_search']) {
    $num_criteria = $_SESSION['tripal_pub_search_form']['num_criteria'];
    $from_year    = $_SESSION['tripal_pub_search_form']['from_year'];
    $to_year      = $_SESSION['tripal_pub_search_form']['to_year'];
     
    $search_array = array();
    $search_array['num_criteria'] = $num_criteria;
    $search_array['from_year']    = $from_year;
    $search_array['to_year']      = $to_year;
    for ($i = 0; $i <= $num_criteria; $i++) {
      $search_array['criteria'][$i]['search_terms'] = $_SESSION['tripal_pub_search_form']['criteria'][$i]['search_terms'];
      $search_array['criteria'][$i]['scope']        = $_SESSION['tripal_pub_search_form']['criteria'][$i]['scope'];
      $search_array['criteria'][$i]['mode']         = $_SESSION['tripal_pub_search_form']['criteria'][$i]['mode'];
      $search_array['criteria'][$i]['operation']    = $_SESSION['tripal_pub_search_form']['criteria'][$i]['operation'];
    }

    // get the list of publications from the remote database using the search criteria.
    $pubs = tripal_pub_get_search_results($search_array, $limit);
    $page = isset($_GET['page']) ? $_GET['page'] : '0';
    $total_records = $_SESSION['chado_pager'][0]['total_records'];
    $total_pages = (int) ($total_records / $limit) + 1;

    // iterate through the results and construct the table displaying the publications
    $rows = array();
    $i = $page * $limit + 1;
    foreach ($pubs as $pub) {
      // get the citation for this publication
      $values = array(
        'pub_id' => $pub->pub_id, 
        'type_id' => array(
          'name' => 'Citation',
        ),
      );
      $citation_rec = chado_generate_var('pubprop', $values);
      $citation_rec = chado_expand_var($citation_rec, 'field', 'pubprop.value');

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
    
    if (count($rows) == 0) {
      $rows[] = array(
        array(
          'data' => 'No results found',
          'colspan' => 3
        )
      );
    }

    $headers = array('', 'Year', 'Publication');
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array('id' => 'tripal-pub-search-results-table', 'border' => '0'),
      'sticky' => TRUE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => '',
    );
    $results = theme_table($table);
    
    // generate the pager
    pager_default_initialize($total_records, $limit);
    $pager = array(
      'tags' => array(),
      'element' => 0,
      'parameters' => array(),
      'quantity' => $limit,
    );
    $pager = theme_pager($pager);
    
    // join all to form the results
    $output .= "<br><p><b>Found " . number_format($total_records) .
      ". Page " . ($page + 1) . " of $total_pages. " .
      " Results</b></br>" . $results . $pager;    
  }
  return $output;
}
/**
 * Purpose: Provides the form to search pubmed
 *
 * @ingroup tripal_pub
 */
function tripal_pub_search_form($form, &$form_state) {
  // Default values can come in the following ways:
  //
  // 1) as elements of the $pub_importer object.  This occurs when editing an existing importer
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  
  // Set the default values. If the pub_import_id isn't already defined by the form values
  // and one is provided then look it up in the database
  $criteria = NULL;
  $num_criteria = 2;
  $from_year = '';
  $to_year = '';

  // if the session has variables then use those.  This should only happen when
  // the 'Test Criteria' button is clicked.
  if (array_key_exists('tripal_pub_search_form', $_SESSION)) {
    $num_criteria = $_SESSION['tripal_pub_search_form']['num_criteria'] ? $_SESSION['tripal_pub_search_form']['num_criteria'] : $num_criteria;
    $from_year    = $_SESSION['tripal_pub_search_form']['from_year']    ? $_SESSION['tripal_pub_search_form']['from_year']    : '';
    $to_year      = $_SESSION['tripal_pub_search_form']['to_year']      ? $_SESSION['tripal_pub_search_form']['to_year']      : '';
  }
  if (array_key_exists('values', $form_state)) {
    $num_criteria = $form_state['values']['num_criteria'] ? $form_state['values']['num_criteria'] : $num_criteria;
    $from_year    = $form_state['values']['from_year']    ? $form_state['values']['from_year']    : $from_year;
    $to_year      = $form_state['values']['to_year']      ? $form_state['values']['to_year']      : $to_year;
  }
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $num_criteria = $form_state['input']['num_criteria'] ? $form_state['input']['num_criteria'] : $num_criteria;
    $from_year    = $form_state['input']['from_year']    ? $form_state['input']['from_year']    : $from_year;
    $to_year      = $form_state['input']['to_year']      ? $form_state['input']['to_year']      : $to_year;
    
  }

  if (array_key_exists('triggering_element', $form_state) and
  $form_state['triggering_element']['#name'] == 'add') {
    $num_criteria++;
  }
  if (array_key_exists('triggering_element', $form_state) and
  $form_state['triggering_element']['#name'] == 'remove') {
    $num_criteria--;
  }
  
  $form['num_criteria']= array(
    '#type'          => 'hidden',
    '#default_value' => $num_criteria,
  );
  
  $form['admin-instructions'] = array(
    '#markup'  =>  theme('tripal_admin_message', array('message' => 
      t('Administrators, you can select the fields with which a user can use to search,
         by checking the desired fields on the ' .
         l('Publication Module Settings Page', 'admin/tripal/chado/tripal_pub/configuration'). ' in
        the section titled "Search Options".  The selected fields will appear in the dropdowns below.'))),
  ); 
  $form['instructions'] = array(
    '#markup'  =>  t('To search for publications enter keywords in the text boxes below.  
        You can limit your search by selecting the field in the dropdown box. Click the 
        add and remove buttons to add additional fields for searching. '),
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
  foreach ($prop_types as $prop) {
    if(array_key_exists($prop->cvterm_id, $allowed_fields) and $allowed_fields[$prop->cvterm_id] > 0) {
      $properties[$prop->cvterm_id] = $prop->name;
    }
  }

  for($i = 1; $i <= $num_criteria; $i++) {
    $search_terms = '';
    $scope = '';
    $operation = '';
    $mode = '';

    // first populate defaults using any values in the SESSION variable
    if (array_key_exists('tripal_pub_search_form', $_SESSION)) {
      $search_terms = $_SESSION['tripal_pub_search_form']['criteria'][$i]['search_terms'] ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['search_terms'] : $search_terms;
      $scope        = $_SESSION['tripal_pub_search_form']['criteria'][$i]['scope']        ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['scope']        : $scope;
      $mode         = $_SESSION['tripal_pub_search_form']['criteria'][$i]['mode']         ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['mode']         : $mode;
      $operation    = $_SESSION['tripal_pub_search_form']['criteria'][$i]['operation']    ? $_SESSION['tripal_pub_search_form']['criteria'][$i]['operation']    : $operation;
    }
    if (array_key_exists('values', $form_state)) {      
      $search_terms = array_key_exists("search_terms-$i", $form_state['values']) ? $form_state['values']["search_terms-$i"] : $search_terms;
      $scope        = array_key_exists("scope-$i", $form_state['values'])        ? $form_state['values']["scope-$i"]        : $scope;
      $mode         = array_key_exists("mode-$i", $form_state['values'])         ? $form_state['values']["mode-$i"]         : $mode;
      $operation    = array_key_exists("operation-$i", $form_state['values'])    ? $form_state['values']["operation-$i"]    : $operation;
    }
    if (array_key_exists('input', $form_state)) {
      $search_terms = array_key_exists("search_terms-$i", $form_state['input']) ? $form_state['input']["search_terms-$i"] : $search_terms;
      $scope        = array_key_exists("scope-$i", $form_state['input'])        ? $form_state['input']["scope-$i"]        : $scope;
      $mode         = array_key_exists("mode-$i", $form_state['input'])         ? $form_state['input']["mode-$i"]         : $mode;
      $operation    = array_key_exists("operation-$i", $form_state['input'])    ? $form_state['input']["operation-$i"]    : $operation;
    }

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
      '#attributes' => array('class' => array('tripal-pub-search-form-scope-select')),
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
          '#type'  => 'button',
          '#name'  => 'remove',
          '#value' => t('Remove'),
          '#ajax' => array(
            'callback' => "tripal_pubs_search_form_ajax_update",
            'wrapper'  => 'tripal-pub-search-form-criteria',
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
          '#submit'   => array('tripal_pub_search_form_ajax_button_submit'),
          '#validate' => array('tripal_pub_search_form_ajax_button_validate'),
          '#limit_validation_errors' => array(),
        );
      }
      $form['criteria'][$i]["add-$i"] = array(
        '#type'  => 'button',      
        '#name'  => 'add',
        '#value' => t('Add'),
        '#ajax' => array(
          'callback' => "tripal_pubs_search_form_ajax_update",
          'wrapper'  => 'tripal-pub-search-form-criteria',
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
        '#submit'   => array('tripal_pub_search_form_ajax_button_submit'),
        '#validate' => array('tripal_pub_search_form_ajax_button_validate'),
        '#limit_validation_errors' => array(),
      );
    }
  }
  $form['criteria']["date"] = array(
      '#type'          => 'select',
      '#options'       => array('Years' => 'Years'),
      '#attributes'    => array('class' => array('tripal-pub-search-form-scope-select')),
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
  
  $form['criteria']['#theme'] = 'tripal_pub_search_setup_form_elements';

  return $form;
}
/**
 * This function is used to rebuild the form if an ajax call is made vai a button.
 * The button causes the form to be submitted. We don't want this so we override
 * the validate and submit routines on the form button. Therefore, this function
 * only needs to tell Drupal to rebuild the form
 */
function tripal_pub_search_form_ajax_button_submit() {
  $form_state['rebuild'] = TRUE;
  
}
/**
 * This function is just a dummy to override the default form submit on ajax calls for buttons
 */
function tripal_pub_search_form_ajax_button_validate() {
  // do nothing
}

/**
 *
 */
function tripal_pub_search_form_validate($form, &$form_state) {
  $num_criteria = $form_state['values']['num_criteria'];
  $from_year    = $form_state['values']['from_year'];
  $to_year      = $form_state['values']['to_year'];
  $op           = $form_state['values']['op'];

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
      $search_terms = '';
      $scope = '';
      $mode = 'Contains';
      $operation = '';
      if (array_key_exists("search_terms-$i", $form_state['values'])) {
        $search_terms =  trim($form_state['values']["search_terms-$i"]);
      }
      if (array_key_exists("scope-$i", $form_state['values'])) {
        $scope =  $form_state['values']["scope-$i"];
      }
      if (array_key_exists("operation-$i", $form_state['values'])) {
        $operation =  $form_state['values']["operation-$i"];
      }
      //$mode =  $form_state['values']["mode-$i"];
      
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
/**
 *
 */
function tripal_pub_get_search_results($search_array, $limit) {

  // build the SQL based on the criteria provided by the user
  $select = "SELECT DISTINCT P.*, CP.nid ";
  $from   = "FROM {pub} P
               LEFT JOIN public.chado_pub CP on P.pub_id = CP.pub_id 
               INNER JOIN {cvterm} CVT on CVT.cvterm_id = P.type_id
            ";
  $where  = "WHERE (NOT P.title = 'null') "; // always exclude the dummy pub
  $order  = "ORDER BY P.pyear DESC, P.title ASC";
  $args = array();  // arguments for where clause
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

    // get the scope type
    $values = array('cvterm_id' => $type_id);
    $cvterm = chado_select_record('cvterm', array('name'), $values);
    $type_name = '';
    if (count($cvterm) > 0) {
      $type_name = $cvterm[0]->name;
    }
    if ($type_name == 'Title') {
      $where .= " $op (lower(P.title) LIKE lower(:crit$i)) ";
      $args[":crit$i"] = '%' . $value . '%';
    }
    elseif ($type_name == 'Year') {
      $where .= " $op (lower(P.pyear) = lower(:crit$i)) ";
      $args[":crit$i"] = '%' . $value . '%';
    }
    elseif ($type_name == 'Volume') {
      $where .= " $op (lower(P.volume) = lower(:crit$i)) ";
      $args[":crit$i"] = '%' . $value . '%';
    }
    elseif ($type_name == 'Issue') {
      $where .= " $op (lower(P.issue) = lower(:crit$i)) ";
      $args[":crit$i"] = '%' . $value . '%';
    }
    elseif ($type_name == 'Journal Name') {
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id AND PP$i.type_id = :crit$i ";
      $where .= " $op ((lower(P.series_name) = lower(:crit$i) and CVT.name = 'Journal Article') OR
                       (lower(PP$i.value) = lower(:crit$i))) ";
      $args[":crit$i"] = $type_id;
    }
    elseif ($type_name == 'Conference Name') {
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id AND PP$i.type_id = :crit$i ";
      $where .= " $op ((lower(P.series_name) = lower(:crit$i) and CVT.name = 'Conference Proceedings') OR
                       (lower(PP$i.value) = lower(:crit$i))) ";
      $args[":crit$i"] = $type_id;
    }
    elseif ($type_name == 'Publication Type') {
      $where .= " $op (lower(CVT.name) = lower(:crit$i))";
      $args[":crit$i"] = $value;
    }
    elseif ($type_id == 0) { //'Any Field'
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id ";
      $where .= " $op (lower(PP$i.value)  LIKE lower(:crit$i) OR
                       lower(P.title) LIKE lower(:crit$i) OR 
                       lower(P.volumetitle) LIKE lower(:crit$i) OR
                       lower(P.publisher) LIKE lower(:crit$i) OR
                       lower(P.uniquename) LIKE lower(:crit$i) OR
                       lower(P.pubplace) LIKE lower(:crit$i) OR
                       lower(P.miniref) LIKE lower(:crit$i) OR
                       lower(P.series_name) LIKE lower(:crit$i)) ";
      $args[":crit$i"] = '%' . $value . '%';
    }
    // for all other properties
    else {
      $from .= " LEFT JOIN {pubprop} PP$i ON PP$i.pub_id = P.pub_id AND PP$i.type_id = :type_id$i ";
      $where .= " $op (lower(PP$i.value) LIKE lower(:crit$i)) ";
      $args[":crit$i"] = '%' . $value . '%';
      $args[":type_id$i"] = $type_id;
    }
  }
  if($from_year and $to_year) {
    $where .= " AND (P.pyear ~ '....' AND to_number(P.pyear,'9999') >= :from$i AND to_number(P.pyear,'9999') <= :to$i) ";
    $args[":from$i"] = $from_year;
    $args[":to$i"] = $to_year;
  }
  $sql = "$select $from $where $order";
  $count = "SELECT count(*) FROM ($select $from $where $order) as t1";
  return chado_pager_query($sql, $args, $limit, 0, $count);
}

/**
 * 
 */
function tripal_pubs_search_form_ajax_update($form, $form_state) {
  return $form['criteria'];
}

/**
*
* @param unknown $form
*/
function theme_tripal_pub_search_setup_form_elements($variables) {
  $form = $variables['form'];

  $rows = array();
  // put each criteria element in a single table row
  foreach ($form as $i => $element) {
    if(is_numeric($i)) {
      $rows[] = array(
        drupal_render($element["operation-$i"]),
        drupal_render($element["scope-$i"]),
        //drupal_render($element["mode-$i"]) .
        drupal_render($element["search_terms-$i"]),
        array(
          'data' => drupal_render($element["add-$i"]) . drupal_render($element["remove-$i"]),
          'nowrap' => 'nowrap', 
        ),
      );
    }
  }

  // add in the from_year and to_year elements as the final row of the table
  $rows[] = array(
    '&nbsp;',
    drupal_render($form['date']),
    array(
      'data' =>
        "<div id=\"pub-search-form-dates-row\">
           <div id=\"pub-search-form-dates\"> ".
             drupal_render($form['from_year']) .
             drupal_render($form['to_year']) . "
           </div>
         </div>
        ",
    ),
    ''
  );
  
  $headers = array();  
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array('id' => 'tripal-pub-search-form-table', 'border' => '0'),
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );
  $results  = '<div id="tripal-pub-search-form-criteria">';
  $results .= theme_table($table);
  $results .= '</div>';
  return $results;
}