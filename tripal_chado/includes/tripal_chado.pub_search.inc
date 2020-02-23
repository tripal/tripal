<?php
/**
 * @file
 *
 * Functions responsible for creating the publication search form that
 * allows a user of the site to search for publications that are currently
 * in Chado.
 */

function tripal_chado_pub_search_admin_form($form, &$form_state) {
  // -----------------------------------------
  // add in the fields for selecting which fields are used when search for pubs
  $form['searching'] = [
    '#type' => 'fieldset',
    '#title' => t('Searching Options'),
    '#description' => t("The list of checkboxes below indicate which fields a user
      can search with when using the publication search tool.  Check the fields that you want
      to allow users to search with.  Click the 'Save configuration' button below to save changes."),
  ];

  // get publication properties list
  $properties = [];
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
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    $properties[$prop->cvterm_id] = $prop->name;
  }
  $form['allowed_search_fields'] = [
    '#type' => 'checkboxes',
    '#options' => $properties,
    '#default_value' => variable_get('tripal_pub_allowed_search_fields', []),
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => 'Save configuration',
    '#name' => 'sumbit',
  ];
  return $form;
}

/**
 *
 */
function tripal_chado_pub_search_admin_form_submit($form, &$form_state) {
  // set the allowed search fields
  $allowed_fields = $form_state['values']['allowed_search_fields'];
  foreach ($allowed_fields as $cvterm_id => $selected) {
    if (!$selected) {
      unset($allowed_fields[$cvterm_id]);
    }
  }
  variable_set('tripal_pub_allowed_search_fields', $allowed_fields);
  drupal_set_message('Changes saved.');
}

/**
 * The page that contains the publication search form and the results for the
 * search
 *
 * @ingroup tripal_pub
 */
function tripal_chado_pub_search_page() {

  // This line may not be required, but on some sites the $_SESSION
  // variable wasn't being set for anonymous users. This line solves that
  // problem
  drupal_session_start();

  $limit = 25;

  // generate the search form
  $form = drupal_get_form('tripal_chado_pub_search_form');
  $output = drupal_render($form);

  // retrieve any results
  if (array_key_exists('tripal_chado_pub_search_form', $_SESSION) and
    $_SESSION['tripal_chado_pub_search_form']['perform_search']) {
    $num_criteria = $_SESSION['tripal_chado_pub_search_form']['num_criteria'];
    $from_year = $_SESSION['tripal_chado_pub_search_form']['from_year'];
    $to_year = $_SESSION['tripal_chado_pub_search_form']['to_year'];

    $search_array = [];
    $search_array['num_criteria'] = $num_criteria;
    $search_array['from_year'] = $from_year;
    $search_array['to_year'] = $to_year;
    for ($i = 0; $i <= $num_criteria; $i++) {
      $search_array['criteria'][$i]['search_terms'] = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['search_terms'];
      $search_array['criteria'][$i]['scope'] = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['scope'];
      $search_array['criteria'][$i]['mode'] = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['mode'];
      $search_array['criteria'][$i]['operation'] = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['operation'];
    }

    // get the list of publications from the remote database using the search criteria.
    $page = isset($_GET['page']) ? $_GET['page'] : '0';
    $offset = $page * $limit;
    $total_records = 0;
    $pubs = tripal_search_publications($search_array, $offset, $limit, $total_records);
    pager_default_initialize($total_records, $limit, 0);

    // iterate through the results and construct the table displaying the publications
    $rows = [];
    $i = $page * $limit + 1;
    foreach ($pubs as $pub) {
      // get the citation for this publication
      $values = [
        'pub_id' => $pub->pub_id,
        'type_id' => [
          'name' => 'Citation',
        ],
      ];
      $citation_rec = chado_generate_var('pubprop', $values);
      $citation_rec = chado_expand_var($citation_rec, 'field', 'pubprop.value');

      // if we have the citation then use it, otherwise, just use the title
      $title = htmlspecialchars($pub->title);
      $result = $title;
      $link = NULL;

      // tripal v2 link (node)
      if (module_exists('tripal_pub')) {
        $nid = chado_get_nid_from_id('pub', $pub->pub_id);
        if ($nid) {
          $link = "/node/$nid";
        }
      }
      // try tripal v3 link (entity), if it exists, update the link to entity
      $entity_id = chado_get_record_entity_by_table('pub', $pub->pub_id);
      if ($entity_id) {
        $link = "/bio_data/$entity_id";
      }

      if ($link) {
        $result = l($title, $link, ['attributes' => ['target' => '_blank']]);
      }
      if ($citation_rec->value) {
        $citation = htmlspecialchars($citation_rec->value);
        $result .= '<br>' . $citation;
      }
      $rows[] = [
        number_format($i) . ".",
        $pub->pyear,
        $result,
      ];
      $i++;
    }

    $headers = ['', 'Year', 'Reference'];
    $table = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [
        'id' => 'tripal-pub-search-results-table',
        'border' => '0',
        'class' => ['tripal-data-table'],
      ],
      'sticky' => TRUE,
      'caption' => '',
      'colgroups' => [],
      'empty' => 'No publications found',
    ];
    $results = theme_table($table);

    // generate the pager
    $pager = [
      'tags' => [],
      'element' => 0,
      'parameters' => [],
      'quantity' => $limit,
    ];
    $pager = theme_pager($pager);

    // join all to form the results
    $output .= "<p><b>Found " . number_format($total_records) .
      " Results</b></br>" . $results . $pager;
  }
  return $output;
}

/**
 * Provides the form to search pubmed
 *
 * @ingroup tripal_pub
 */
function tripal_chado_pub_search_form($form, &$form_state) {
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
  if (array_key_exists('storage', $form_state)) {
    $num_criteria = $form_state['storage']['num_criteria'];
  }
  if (array_key_exists('tripal_chado_pub_search_form', $_SESSION)) {
    $num_criteria = $_SESSION['tripal_chado_pub_search_form']['num_criteria'] ? $_SESSION['tripal_chado_pub_search_form']['num_criteria'] : $num_criteria;
    $from_year = $_SESSION['tripal_chado_pub_search_form']['from_year'] ? $_SESSION['tripal_chado_pub_search_form']['from_year'] : '';
    $to_year = $_SESSION['tripal_chado_pub_search_form']['to_year'] ? $_SESSION['tripal_chado_pub_search_form']['to_year'] : '';
  }
  if (array_key_exists('values', $form_state)) {
    $from_year = $form_state['values']['from_year'] ? $form_state['values']['from_year'] : $from_year;
    $to_year = $form_state['values']['to_year'] ? $form_state['values']['to_year'] : $to_year;
  }
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $from_year = $form_state['input']['from_year'] ? $form_state['input']['from_year'] : $from_year;
    $to_year = $form_state['input']['to_year'] ? $form_state['input']['to_year'] : $to_year;
  }

  if (array_key_exists('triggering_element', $form_state) and
    $form_state['triggering_element']['#name'] == 'add') {
    $num_criteria++;
  }
  if (array_key_exists('triggering_element', $form_state) and
    $form_state['triggering_element']['#name'] == 'remove') {
    $num_criteria--;
  }

  $form_state['storage']['num_criteria'] = $num_criteria;

  $form['admin-instructions'] = [
    '#markup' => tripal_set_message(
      t('Administrators, you can select the fields with which a user can use to search, by checking the desired fields on the ' .
        l('Publication Search Settings Page', 'admin/tripal/storage/chado/pub-search-config', ['attributes' => ['target' => '_blank']]) . '
      in the section titled "Search Options".  The selected fields will appear in the dropdowns below.'),
      TRIPAL_INFO,
      ['return_html' => 1]),
  ];
  $form['instructions'] = [
    '#markup' => t('To search for publications enter keywords in the text boxes below.
    You can limit your search by selecting the field in the dropdown box. Click the
    add and remove buttons to add additional fields for searching. '),
  ];

  // get publication properties list
  $properties = [];
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
  $allowed_fields = variable_get('tripal_pub_allowed_search_fields', []);
  $prop_types = chado_query($sql);
  foreach ($prop_types as $prop) {
    if (array_key_exists($prop->cvterm_id, $allowed_fields) and $allowed_fields[$prop->cvterm_id] > 0) {
      $properties[$prop->cvterm_id] = $prop->name;
    }
  }

  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms = '';
    $scope = '';
    $operation = '';
    $mode = '';

    // first populate defaults using any values in the SESSION variable
    if (array_key_exists('tripal_chado_pub_search_form', $_SESSION)) {
      $search_terms = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['search_terms'] ? $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['search_terms'] : $search_terms;
      $scope = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['scope'] ? $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['scope'] : $scope;
      $mode = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['mode'] ? $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['mode'] : $mode;
      $operation = $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['operation'] ? $_SESSION['tripal_chado_pub_search_form']['criteria'][$i]['operation'] : $operation;
    }
    if (array_key_exists('values', $form_state)) {
      $search_terms = array_key_exists("search_terms-$i", $form_state['values']) ? $form_state['values']["search_terms-$i"] : $search_terms;
      $scope = array_key_exists("scope-$i", $form_state['values']) ? $form_state['values']["scope-$i"] : $scope;
      $mode = array_key_exists("mode-$i", $form_state['values']) ? $form_state['values']["mode-$i"] : $mode;
      $operation = array_key_exists("operation-$i", $form_state['values']) ? $form_state['values']["operation-$i"] : $operation;
    }
    if (array_key_exists('input', $form_state)) {
      $search_terms = array_key_exists("search_terms-$i", $form_state['input']) ? $form_state['input']["search_terms-$i"] : $search_terms;
      $scope = array_key_exists("scope-$i", $form_state['input']) ? $form_state['input']["scope-$i"] : $scope;
      $mode = array_key_exists("mode-$i", $form_state['input']) ? $form_state['input']["mode-$i"] : $mode;
      $operation = array_key_exists("operation-$i", $form_state['input']) ? $form_state['input']["operation-$i"] : $operation;
    }

    // default to searching the title and abstract
    if (!$scope) {
      $scope = 'abstract';
    }

    $form['criteria'][$i]["search_terms-$i"] = [
      '#type' => 'textfield',
      '#default_value' => $search_terms,
      '#required' => FALSE,
      '#size' => 35,
    ];
    $form['criteria'][$i]["scope-$i"] = [
      '#type' => 'select',
      '#options' => $properties,
      '#default_value' => $scope,
      '#attributes' => ['class' => ['tripal-pub-search-form-scope-select']],
    ];
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
      $form['criteria'][$i]["operation-$i"] = [
        '#type' => 'select',
        '#options' => [
          'AND' => 'AND',
          'OR' => 'OR',
          'NOT' => 'NOT',
        ],
        '#default_value' => $operation,
      ];
    }
    if ($i == $num_criteria) {
      if ($i > 1) {
        $form['criteria'][$i]["remove-$i"] = [
          '#type' => 'button',
          '#name' => 'remove',
          '#value' => t('Remove'),
          '#ajax' => [
            'callback' => "tripal_pubs_search_form_ajax_update",
            'wrapper' => 'tripal-pub-search-form-criteria',
            'effect' => 'fade',
            'method' => 'replace',
            'prevent' => 'click',
          ],
          // When this button is clicked, the form will be validated and submitted.
          // Therefore, we set custom submit and validate functions to override the
          // default form submit.  In the validate function we set the form_state
          // to rebuild the form so the submit function never actually gets called,
          // but we need it or Drupal will run the default validate anyway.
          // we also set #limit_validation_errors to empty so fields that
          // are required that don't have values won't generate warnings.
          '#submit' => ['tripal_chado_pub_search_form_ajax_button_submit'],
          '#validate' => ['tripal_chado_pub_search_form_ajax_button_validate'],
          '#limit_validation_errors' => [],
        ];
      }
      $form['criteria'][$i]["add-$i"] = [
        '#type' => 'button',
        '#name' => 'add',
        '#value' => t('Add'),
        '#ajax' => [
          'callback' => "tripal_pubs_search_form_ajax_update",
          'wrapper' => 'tripal-pub-search-form-criteria',
          'effect' => 'fade',
          'method' => 'replace',
          'prevent' => 'click',
        ],
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default form submit.  In the validate function we set the form_state
        // to rebuild the form so the submit function never actually gets called,
        // but we need it or Drupal will run the default validate anyway.
        // we also set #limit_validation_errors to empty so fields that
        // are required that don't have values won't generate warnings.
        '#submit' => ['tripal_chado_pub_search_form_ajax_button_submit'],
        '#validate' => ['tripal_chado_pub_search_form_ajax_button_validate'],
        '#limit_validation_errors' => [],
      ];
    }
  }
  $form['criteria']["date"] = [
    '#type' => 'select',
    '#options' => ['Years' => 'Years'],
    '#attributes' => ['class' => ['tripal-pub-search-form-scope-select']],
  ];
  $form['criteria']["from_year"] = [
    '#type' => 'textfield',
    '#default_value' => $from_year,
    '#required' => FALSE,
    '#title' => 'from',
    '#size' => 4,
    '#maxlength' => 4,
  ];
  $form['criteria']["to_year"] = [
    '#type' => 'textfield',
    '#default_value' => $to_year,
    '#required' => FALSE,
    '#title' => 'to',
    '#size' => 4,
    '#maxlength' => 4,
  ];

  $form['search'] = [
    '#type' => 'submit',
    '#value' => t('Search'),
  ];
  $form['reset'] = [
    '#type' => 'submit',
    '#value' => t('Reset'),
  ];

  $form['criteria']['#theme'] = 'tripal_chado_pub_search_setup_form_elements';

  return $form;
}

/**
 * This function is used to rebuild the form if an ajax call is made vai a
 * button. The button causes the form to be submitted. We don't want this so we
 * override the validate and submit routines on the form button. Therefore,
 * this function only needs to tell Drupal to rebuild the form
 *
 * @ingroup tripal_pub
 */
function tripal_chado_pub_search_form_ajax_button_submit() {
  $form_state['rebuild'] = TRUE;
}

/**
 * This function is just a dummy to override the default form submit on ajax
 * calls for buttons
 *
 * @ingroup tripal_pub
 */
function tripal_chado_pub_search_form_ajax_button_validate() {
  // do nothing
}

/**
 * Validate the tripal_chado_pub_search_form form
 *
 * @ingroup tripal_pub
 */
function tripal_chado_pub_search_form_validate($form, &$form_state) {
  $num_criteria = $form_state['storage']['num_criteria'];
  $from_year = $form_state['values']['from_year'];
  $to_year = $form_state['values']['to_year'];
  $op = $form_state['values']['op'];

  // no need to vlaidate on a reset
  if ($op == 'Reset') {
    return;
  }

  if ($from_year and !$to_year) {
    form_set_error('to_year', 'Please provide a 4-digit year.');
  }
  if (!$from_year and $to_year) {
    form_set_error('from_year', 'Please provide a 4-digit year.');
  }
  if ($from_year and !preg_match('/\d\d\d\d/', $from_year)) {
    form_set_error('from_year', 'Please provide a 4-digit year.');
  }
  if ($to_year and !preg_match('/\d\d\d\d/', $to_year)) {
    form_set_error('to_year', 'Please provide a 4-digit year.');
  }
}

/**
 * Submit the tripal_chado_pub_search_form form
 *
 * @ingroup tripal_pub
 */
function tripal_chado_pub_search_form_submit($form, &$form_state) {
  $num_criteria = $form_state['storage']['num_criteria'];
  $from_year = $form_state['values']['from_year'];
  $to_year = $form_state['values']['to_year'];
  $op = $form_state['values']['op'];

  // set the session variables
  if ($op == 'Search') {
    $_SESSION['tripal_chado_pub_search_form']['num_criteria'] = $num_criteria;
    unset($_SESSION['tripal_chado_pub_search_form']['criteria']);
    for ($i = 0; $i <= $num_criteria; $i++) {
      $search_terms = '';
      $scope = '';
      $mode = 'Contains';
      $operation = '';
      if (array_key_exists("search_terms-$i", $form_state['values'])) {
        $search_terms = trim($form_state['values']["search_terms-$i"]);
      }
      if (array_key_exists("scope-$i", $form_state['values'])) {
        $scope = $form_state['values']["scope-$i"];
      }
      if (array_key_exists("operation-$i", $form_state['values'])) {
        $operation = $form_state['values']["operation-$i"];
      }
      //$mode =  $form_state['values']["mode-$i"];

      $_SESSION['tripal_chado_pub_search_form']['criteria'][$i] = [
        'search_terms' => $search_terms,
        'scope' => $scope,
        'mode' => $mode,
        'operation' => $operation,
      ];

    }
    $_SESSION['tripal_chado_pub_search_form']['from_year'] = $from_year;
    $_SESSION['tripal_chado_pub_search_form']['to_year'] = $to_year;
    $_SESSION['tripal_chado_pub_search_form']['perform_search'] = 1;
  }
  if ($op == 'Reset') {
    unset($_SESSION['tripal_chado_pub_search_form']);
  }
}


/**
 * Ajax callback to update the form
 *
 * @param $form
 *   The form array
 * @param $form_state
 *   The form state array
 *
 * @ingroup tripal_pub
 */
function tripal_pubs_search_form_ajax_update($form, $form_state) {
  return $form['criteria'];
}

/**
 * Theme the tripal_chado_pub_search_setup_form form
 *
 * @ingroup tripal_pub
 */
function theme_tripal_chado_pub_search_setup_form_elements($variables) {
  $form = $variables['form'];

  $rows = [];
  // put each criteria element in a single table row
  foreach ($form as $i => $element) {
    if (is_numeric($i)) {
      $rows[] = [
        drupal_render($element["operation-$i"]),
        drupal_render($element["scope-$i"]),
        //drupal_render($element["mode-$i"]) .
        drupal_render($element["search_terms-$i"]),
        [
          'data' => drupal_render($element["add-$i"]) . drupal_render($element["remove-$i"]),
          'nowrap' => 'nowrap',
        ],
      ];
    }
  }

  // add in the from_year and to_year elements as the final row of the table
  $rows[] = [
    '&nbsp;',
    drupal_render($form['date']),
    [
      'data' =>
        "<div id=\"pub-search-form-dates-row\">
         <div id=\"pub-search-form-dates\"> " .
        drupal_render($form['from_year']) .
        drupal_render($form['to_year']) . "
         </div>
       </div>
      ",
    ],
    '',
  ];

  $headers = [];
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'id' => 'tripal-pub-search-form-table',
      'border' => '0',
      'class' => 'tripal-data-table',
    ],
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];
  $results = '<div id="tripal-pub-search-form-criteria">';
  $results .= theme_table($table);
  $results .= '</div>';
  return $results;
}


/**
 * Builds the SQL statement need to search Chado for the publications
 * that match the user supplied criteria.  Tpyically, this function is
 * called by the search form generated by the tripal_chado_pub_search_form()
 * function but this function is included in the API for calling by anyone.
 *
 * @param $search_array
 *   An array of search criteria provided by the user. The search array is
 *   an associative array with the following keys:
 *     'num_criteria': an integer indicating the number of search criteria
 *        supplied
 *     'from_year':    filters records by a start year
 *     'to_year':      filters records by an end year
 *     'criteria':     an array of criteria. Each criteria is an associative
 *                     array with the following keys:
 *                     'search_terms':   The text used for searching
 *                     'scope':          The cvterm_id of the property used for
 *                        filtering
 *                     'mode':           The operation (e.g. AND, OR or NOT)
 * @param $offset
 *   The offset for paging records.  The first record returned will be
 *   at the offset indicated here, and the next $limit number of records
 *   will be returned.
 *
 * @param $limit
 *   The number of records to retrieve
 *
 * @param total_records
 *   A value passed by reference. This value will get set to the total
 *   number of matching records
 *
 * @return
 *   a PDO database object of the query results.
 *
 * @ingroup tripal_pub
 */
function tripal_search_publications($search_array, $offset, $limit, &$total_records) {

  // Build the SQL based on the criteria provided by the user
  $select = "SELECT DISTINCT P.* ";
  $from = "FROM {pub} P INNER JOIN {cvterm} CVT on CVT.cvterm_id = P.type_id ";
  $where = "WHERE (NOT P.title = 'null') ";
  $order = "ORDER BY P.pyear DESC, P.title ASC";
  $args = [];
  $join = 0;

  $num_criteria = $search_array['num_criteria'];
  $from_year = $search_array['from_year'];
  $to_year = $search_array['to_year'];

  for ($i = 1; $i <= $num_criteria; $i++) {
    $value = $search_array['criteria'][$i]['search_terms'];
    $type_id = $search_array['criteria'][$i]['scope'];
    $mode = $search_array['criteria'][$i]['mode'];
    $op = $search_array['criteria'][$i]['operation'];

    // skip criteria with no values
    if (!$value) {
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
    $values = ['cvterm_id' => $type_id];
    $cvterm = chado_select_record('cvterm', ['name'], $values);
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
  if ($from_year and $to_year) {
    $where .= " AND (P.pyear ~ '....' AND to_number(P.pyear,'9999') >= :from$i AND to_number(P.pyear,'9999') <= :to$i) ";
    $args[":from$i"] = $from_year;
    $args[":to$i"] = $to_year;
  }
  $sql = "$select $from $where $order  LIMIT " . (int) $limit . ' OFFSET ' . (int) $offset;
  $count = "SELECT count(*) FROM ($select $from $where $order) as t1";

  // first get the total number of matches
  $total_records = chado_query($count, $args)->fetchField();
  $results = chado_query($sql, $args);

  return $results;
}
