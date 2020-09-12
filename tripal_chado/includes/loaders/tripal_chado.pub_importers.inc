<?php
/**
 * @file
 * Management of importers
 */

require_once('tripal_chado.pub_importer_AGL.inc');
require_once('tripal_chado.pub_importer_PMID.inc');


/**
 * A function to generate a table containing the list of publication importers
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importers_list() {

  // Check to make sure that the tripal_pub vocabulary is loaded. If not, then
  // warn the user that they should load it before continuing.
  $pub_cv = chado_select_record('cv', ['cv_id'], ['name' => 'tripal_pub']);
  if (count($pub_cv) == 0) {
    drupal_set_message(t('The Tripal Pub vocabulary is currently not loaded. ' .
      'This vocabulary is required to be loaded before importing of ' .
      'publications.  <br>Please !import',
      ['!import' => l('load the Tripal Publication vocabulary', 'aadmin/tripal/loaders/chado_vocabs/obo_loader')]), 'warning');
  }

  // clear out the session variable when we view the list.
  unset($_SESSION['tripal_pub_import']);

  $headers = [
    '',
    'Importer Name',
    'Database',
    'Search String',
    'Disabled',
    'Create Contact',
    '',
  ];
  $rows = [];
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

    $rows[] = [
      [
        'data' => l(t('Edit/Test'), "admin/tripal/loaders/pub/edit/$importer->pub_import_id") . '<br>' .
          l(t('Import Pubs'), "admin/tripal/loaders/pub/submit/$importer->pub_import_id"),
        'nowrap' => 'nowrap',
      ],
      $importer->name,
      $criteria['remote_db'],
      $criteria_str,
      $importer->disabled ? 'Yes' : 'No',
      $importer->do_contact ? 'Yes' : 'No',
      l(t('Delete'), "admin/tripal/loaders/pub/delete/$importer->pub_import_id"),
    ];
  }


  $page = "<ul class='action-links'>";
  $page .= '  <li>' . l('New Importer', 'admin/tripal/loaders/pub/new') . '</li>';
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
          importer to run peridically by adding a cron job. </li>
     </ol><br>");

  $form = drupal_get_form('tripal_pub_importer_ncbi_api_key_form');
  $page .= drupal_render($form);

  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
    ],
    'caption' => '',
    'sticky' => TRUE,
    'colgroups' => [],
    'empty' => 'There are no currently importers',
  ];

  $page .= theme_table($table);

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
  $values = ['name' => 'tripal_pub'];
  $tpub_cv = chado_select_record('cv', ['cv_id'], $values);
  if (count($tpub_cv) == 0) {
    drupal_set_message(t('Before importing publications you must first ') . l(t('load the Tripal Pub Ontology'), 'admin/tripal/loaders/chado_vocabs/obo_loader'), 'error');
  }
  $values = ['name' => 'tripal_contact'];
  $tpub_cv = chado_select_record('cv', ['cv_id'], $values);
  if (count($tpub_cv) == 0) {
    drupal_set_message(t('If you want to create contact pages for authors, you must first ') . l(t('load the Tripal Contact Ontology'), 'admin/tripal/loaders/chado_vocabs/obo_loader'), 'error');
  }

  if(!extension_loaded ('yaz')){
    drupal_set_message(t('<b>Note:</b> In order to create an importer using the USDA National Agricultural Library (AGL) you must install the yaz libraries. See the ') . l(t('Users Guide for Instructions'), 'https://tripal.readthedocs.io/en/latest/user_guide/example_genomics/pub_import.html#import-from-the-usda-national-agricultural-library') . ' for assistance.  If you do not want to use AGL you can ignore this warning.', 'warning');
  }

  // generate the search form
  $form = drupal_get_form('tripal_pub_importer_setup_form', $pub_import_id, $action);

  $output = l("Return to publication importers list", "admin/tripal/loaders/pub");
  $output .= drupal_render($form);

  // retrieve any results
  if (array_key_exists('tripal_pub_import', $_SESSION)) {
    $remote_db = array_key_exists('remote_db', $_SESSION['tripal_pub_import']) ? $_SESSION['tripal_pub_import']['remote_db'] : '';
    $num_criteria = array_key_exists('num_criteria', $_SESSION['tripal_pub_import']) ? $_SESSION['tripal_pub_import']['num_criteria'] : '';
    $days = array_key_exists('days', $_SESSION['tripal_pub_import']) ? $_SESSION['tripal_pub_import']['days'] : '';
    $latestyear = array_key_exists('latestyear', $_SESSION['tripal_pub_import']) ? $_SESSION['tripal_pub_import']['latestyear'] : '';

    $search_array = [];
    $search_array['remote_db'] = $remote_db;
    $search_array['num_criteria'] = $num_criteria;
    $search_array['days'] = $days;
    $search_array['latestyear'] = $latestyear;
    for ($i = 1; $i <= $num_criteria; $i++) {
      $search_array['criteria'][$i]['search_terms'] = $_SESSION['tripal_pub_import']['criteria'][$i]['search_terms'];
      $search_array['criteria'][$i]['scope'] = $_SESSION['tripal_pub_import']['criteria'][$i]['scope'];
      $search_array['criteria'][$i]['is_phrase'] = $_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase'];
      $search_array['criteria'][$i]['operation'] = $_SESSION['tripal_pub_import']['criteria'][$i]['operation'];
    }

    // if the form has been submitted with the 'test' button then get the results
    if ($_SESSION['tripal_pub_import']['perform_search']) {

      $limit = 25;

      // get the list of publications from the remote database using the search criteria.
      $page = isset($_GET['page']) ? $_GET['page'] : '0';
      $results = tripal_get_remote_pubs($remote_db, $search_array, $limit, $page);
      $total_records = $results['total_records'];
      $search_str = $results['search_str'];
      $pubs = $results['pubs'];

      // iterate through the results and construct the table displaying the publications
      $rows = [];
      $i = $page * $limit + 1;
      if (count($pubs) > 0) {
        foreach ($pubs as $pub) {
          $citation = array_key_exists('Citation', $pub) ? htmlspecialchars($pub['Citation']) : 'Unable to generate citation';
          $raw_link = '';
          if (array_key_exists('Publication Dbxref', $pub) and $pub['Publication Dbxref']) {
            $raw_link = l('raw', 'admin/tripal/loaders/pub/raw/' . $pub['Publication Dbxref'], ['attributes' => ['target' => '_blank']]);
          }
          // indicate those that will be excluded by AGL year filtering parameters
          if ((array_key_exists('passfilter', $pub)) and ($pub['passfilter'] == 0 )) {
            $citation = '<span style="text-decoration: line-through;">' . $citation . '</span>';
          }
          $rows[] = [
            number_format($i),
            $citation,
            $raw_link,
          ];
          $i++;
        }
      }

      if (count($rows) == 0) {
        $rows[] = [
          [
            'data' => 'No results found',
            'colspan' => 3,
          ],
        ];
      }

      $headers = ['', 'Publication', 'Raw Results'];
      $table = [
        'header' => $headers,
        'rows' => $rows,
        'attributes' => [
          'id' => 'tripal_pub-importer-test',
          'class' => 'tripal-data-table',
        ],
        'sticky' => FALSE,
        'caption' => '',
        'colgroups' => [],
        'empty' => '',
      ];
      // once we have our table array structure defined, we call Drupal's theme_table()
      // function to generate the table.
      $table = theme_table($table);

      // generate the pager
      pager_default_initialize($total_records, $limit);
      $pager = [
        'tags' => [],
        'element' => 0,
        'parameters' => [],
        'quantity' => $limit,
      ];
      $pager = theme_pager($pager);

      // because this is an ajax callback, the theme_pager will set the URL to be
      // "system/ajax", so we need to reset that
      $pager = str_replace($base_path . "system/ajax", "", $pager);

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
  $latestyear = '';
  $disabled = '';
  $do_contact = '';
  $num_criteria = 1;
  $loader_name = '';

  // if this is an edit then we are pulling an import object from the database
  if ($action == "edit") {
    $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :pub_import_id";
    $importer = db_query($sql, [':pub_import_id' => $pub_import_id])->fetchObject();

    $criteria = unserialize($importer->criteria);
    $remote_db = $criteria['remote_db'];
    $days = $criteria['days'];
    $latestyear = $criteria['latestyear'];
    $disabled = $criteria['disabled'];
    $do_contact = $criteria['do_contact'];
    $num_criteria = $criteria['num_criteria'];
    $loader_name = $criteria['loader_name'];
  }

  // if there are any session variables then use those
  if (array_key_exists('tripal_pub_import', $_SESSION)) {
    $remote_db = $_SESSION['tripal_pub_import']['remote_db'];
    $days = $_SESSION['tripal_pub_import']['days'];
    $latestyear = $_SESSION['tripal_pub_import']['latestyear'];
    $disabled = $_SESSION['tripal_pub_import']['disabled'];
    $do_contact = $_SESSION['tripal_pub_import']['do_contact'];
    $num_criteria = $_SESSION['tripal_pub_import']['num_criteria'];
    $loader_name = $_SESSION['tripal_pub_import']['loader_name'];

    // check if the pub_import_id in the session variable is not the same as the one we've been provided
    // if so, then clear the session variable
    if ($pub_import_id and $pub_import_id != $_SESSION['tripal_pub_import']['pub_import_id']) {
      unset($_SESSION['tripal_pub_import']);
    }
  }

  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $remote_db = $form_state['values']['remote_db'] ?? null;
    $days = $form_state['values']['days'] ?? null;
    $latestyear = $form_state['values']['latestyear'] ?? null;
    $disabled = $form_state['values']['disabled'] ?? null;
    $do_contact = $form_state['values']['do_contact'] ?? null;
    $num_criteria = $form_state['values']['num_criteria'] ?? null;
    $loader_name = $form_state['values']['loader_name'] ?? null;
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $remote_db = $form_state['input']['remote_db'] ?? null;
    $days = $form_state['input']['days'] ?? null;
    $latestyear = $form_state['input']['latestyear'] ?? null;
    $disabled = $form_state['input']['disabled'] ?? null;
    $do_contact = $form_state['input']['do_contact'] ?? null;
    $loader_name = $form_state['input']['loader_name'] ?? null;

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
  $form['num_criteria'] = [
    '#type' => 'value',
    '#value' => $num_criteria,
  ];
  $form['pub_import_id'] = [
    '#type' => 'value',
    '#value' => $pub_import_id,
  ];
  $form['action'] = [
    '#type' => 'value',
    '#value' => $action,
  ];

  // add in the elements that will be organized via a theme function
  $form['themed_element']['loader_name'] = [
    '#type' => 'textfield',
    '#title' => t('Loader Name'),
    '#description' => t('Please provide a name for this loader setup.'),
    '#default_value' => $loader_name,
    '#required' => TRUE,
  ];

  $supported_dbs = variable_get('tripal_pub_supported_dbs', ['PMID']);
  $remote_dbs = [];
  $values = [
    'name' => $supported_dbs,
  ];
  $dbs = chado_select_record('db', ['*'], $values);
  foreach ($dbs as $index => $db) {
    $remote_dbs[$db->name] = $db->description ? $db->description : $db->name;
  };
  // use PubMed as the default
  if (!$remote_db) {
    $remote_db = 'PMID';
  }

  $form['themed_element']['remote_db'] = [
    '#title' => t('Source'),
    '#type' => 'select',
    '#options' => $remote_dbs,
    '#default_value' => $remote_db,
    '#ajax' => [
      'callback' => "tripal_pubs_setup_form_ajax_update",
      'wrapper' => 'tripal-pubs-importer-setup',
      'effect' => 'fade',
      'method' => 'replace',
    ],
  ];
  $form['themed_element']['days'] = [
    '#type' => 'textfield',
    '#title' => t('Days since record modified'),
    '#description' => t('Limit the search to include pubs that have been added no more than this many days before today.'),
    '#default_value' => $days,
    '#size' => 5,
  ];
  // AGL only, this field will be removed for the pubmed loader
  $form['themed_element']['latestyear'] = [
    '#type' => 'textfield',
    '#title' => t('Latest year of publication'),
    '#description' => t('Filter returned publications for those that have been published no later than this year.'),
    '#default_value' => $latestyear,
    '#size' => 5,
  ];
  $form['themed_element']['disabled'] = [
    '#type' => 'checkbox',
    '#title' => t('Disabled'),
    '#description' => t('Check to disable this importer.'),
    '#default_value' => $disabled,
  ];
  $form['themed_element']['do_contact'] = [
    '#type' => 'checkbox',
    '#title' => t('Create Contact'),
    '#description' => t('Check to create an entry in the contact table for each author of a matching publication during import. This allows storage of
       additional information such as affilation, etc. Otherwise, only authors names are retrieved.'),
    '#default_value' => $do_contact,
  ];

  // add in the form for the criteria
  tripal_pub_importer_setup_add_criteria_fields($form, $form_state, $num_criteria, $criteria);

  // add in the buttons
  $form['save'] = [
    '#type' => 'submit',
    '#value' => t('Save Importer'),
  ];
  $form['test'] = [
    '#type' => 'submit',
    '#value' => t('Test Importer'),
  ];
  $form['delete'] = [
    '#type' => 'submit',
    '#value' => t('Delete Importer'),
    '#attributes' => ['style' => 'float: right;'],
  ];

  // add in the section where the test results will appear
  $form['results'] = [
    '#markup' => '<div id="tripal-pub-importer-test-section"></div>',
  ];

  // allow the selected remote database to make changes to the form if needed
  $callback = "tripal_pub_remote_alter_form_$remote_db";
  $form = call_user_func($callback, $form, $form_state, $num_criteria);

  $form['themed_element']['#theme'] = 'tripal_pub_importer_setup_form_elements';

  return $form;
}

/**
 * The form used for setting the optional NCBI API key.
 *
 * @param $form
 *   The form element to be populated.
 * @param $form_state
 *   The state of the form element to be populated.
 *
 * @return array
 *   The populated form element.
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importer_ncbi_api_key_form($form, $form_state) {
  $description = t('Tripal imports publications using NCBI\'s ')
    . l('EUtils API', 'https://www.ncbi.nlm.nih.gov/books/NBK25500/')
    . t(', which limits users and programs to a maximum of 3 requests per second without an API key. '
        . 'However, NCBI allows users and programs to an increased maximum of 10 requests per second if '
        . 'they provide a valid API key. This is particularly useful in speeding up large publication imports. '
        . 'For more information on NCBI API keys, please ')
    . l('see here', 'https://www.ncbi.nlm.nih.gov/books/NBK25497/#chapter2.Coming_in_December_2018_API_Key', array(
      'attributes' => array(
        'target' => 'blank',
      ),
    )) . '.';

  $form['ncbi_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('(Optional) NCBI API key:'),
    '#description' => $description,
    '#default_value' => variable_get('tripal_pub_importer_ncbi_api_key', NULL),
    '#ajax' => array(
      'callback' => 'tripal_pub_importer_set_ncbi_api_key',
      'wrapper' => 'ncbi_api_key',
    ),
    '#prefix' => '<div id="ncbi_api_key">',
    '#suffix' => '</div>',
  );

  return $form;
}

/**
 * This function saves the NCBI API key to the database.
 *
 * It is called when the user makes a change to the NCBI API key field and then
 * moves their cursor out of the field.
 *
 * @param $form
 *   The new form element.
 * @param $form_state
 *   The state of the new form element.
 *
 * @return array
 *   The new api key field.
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importer_set_ncbi_api_key($form, $form_state) {
  variable_set('tripal_pub_importer_ncbi_api_key', check_plain($form_state['values']['ncbi_api_key']));
  drupal_set_message('NCBI API key has been saved successfully!');
  return $form['ncbi_api_key'];
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
 * @return
 *  A form array
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importer_setup_add_criteria_fields(&$form, &$form_state, $num_criteria, $criteria) {

  // choices array
  $scope_choices = [
    'any' => 'Any Field',
    'abstract' => 'Abstract',
    'author' => 'Author',
    'id' => 'Accession',
    'title' => 'Title',
    'journal' => 'Journal Name',
  ];

  $first_op_choices = [
    '' => '',
    'NOT' => 'NOT',
  ];
  $op_choices = [
    'AND' => 'AND',
    'OR' => 'OR',
    'NOT' => 'NOT',
  ];

  for ($i = 1; $i <= $num_criteria; $i++) {
    $is_phrase = 1;

    $search_terms = '';
    $scope = '';
    $is_phrase = '';
    $operation = '';

    // if we have criteria supplied from the database then use that as the initial defaults
    if ($criteria) {
      if (array_key_exists('criteria', $criteria)) {
        if (array_key_exists($i, $criteria['criteria'])) {
          $search_terms = $criteria['criteria'][$i]['search_terms'];
          $scope = $criteria['criteria'][$i]['scope'];
          $is_phrase = $criteria['criteria'][$i]['is_phrase'];
          $operation = $criteria['criteria'][$i]['operation'];
        }
      }
    }

    // if the criteria comes the session
    if (array_key_exists('tripal_pub_import', $_SESSION)) {
      $search_terms = isset($_SESSION['tripal_pub_import']['criteria'][$i]['search_terms']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['search_terms'] : $search_terms;
      $scope = isset($_SESSION['tripal_pub_import']['criteria'][$i]['scope']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['scope'] : $scope;
      $is_phrase = isset($_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['is_phrase'] : $is_phrase;
      $operation = isset($_SESSION['tripal_pub_import']['criteria'][$i]['operation']) ? $_SESSION['tripal_pub_import']['criteria'][$i]['operation'] : $operation;
    }

    // If the form_state has variables then use those.  This happens when an error occurs on the form or the
    // form is resubmitted using AJAX
    if (array_key_exists('values', $form_state)) {
      $search_terms = $form_state['values']["search_terms-$i"] ?? null;
      $scope = $form_state['values']["scope-$i"] ?? null;
      $is_phrase = $form_state['values']["is_phrase-$i"] ?? null;
      $operation = $form_state['values']["operation-$i"] ?? null;
    }
    $form['themed_element']['criteria'][$i]["scope-$i"] = [
      '#type' => 'select',
      '#description' => t('Please select the fields to search for this term.'),
      '#options' => $scope_choices,
      '#default_value' => $scope,
    ];
    $form['themed_element']['criteria'][$i]["search_terms-$i"] = [
      '#type' => 'textfield',
      '#description' => t('<span style="white-space: normal">Please provide a list of words for searching. You may use
        conjunctions such as "AND" or "OR" to separate words if they are expected in
        the same scope, but do not mix ANDs and ORs.  Check the "Is Phrase" checkbox to use conjunctions as part of the text to search</span>'),
      '#default_value' => $search_terms,
      '#required' => TRUE,
      '#maxlength' => 2048,
    ];
    $form['themed_element']['criteria'][$i]["is_phrase-$i"] = [
      '#type' => 'checkbox',
      '#title' => t('Is Phrase?'),
      '#default_value' => $is_phrase,
    ];

    if ($i == 1) {
      /*
       $form['criteria'][$i]["operation-$i"] = array(
         '#type'          => 'select',
         '#options'       => $first_op_choices,
         '#default_value' => $operation,
       );*/
    }
    if ($i > 1) {
      $form['themed_element']['criteria'][$i]["operation-$i"] = [
        '#type' => 'select',
        '#options' => $op_choices,
        '#default_value' => $operation,
      ];
    }
    if ($i == $num_criteria) {
      if ($i > 1) {
        $form['themed_element']['criteria'][$i]["remove-$i"] = [
          '#type' => 'button',
          '#name' => 'remove',
          '#value' => t('Remove'),
          '#ajax' => [
            'callback' => "tripal_pubs_setup_form_ajax_update",
            'wrapper' => 'tripal-pubs-importer-setup',
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
          '#submit' => ['tripal_pub_setup_form_ajax_button_submit'],
          '#validate' => ['tripal_pub_setup_form_ajax_button_validate'],
          '#limit_validation_errors' => [],
        ];
      }
      $form['themed_element']['criteria'][$i]["add-$i"] = [
        '#type' => 'button',
        '#name' => 'add',
        '#value' => t('Add'),
        '#ajax' => [
          'callback' => "tripal_pubs_setup_form_ajax_update",
          'wrapper' => 'tripal-pubs-importer-setup',
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
        '#submit' => ['tripal_pub_setup_form_ajax_button_submit'],
        '#validate' => ['tripal_pub_setup_form_ajax_button_validate'],
        '#limit_validation_errors' => [],
      ];
    }
  }
}

/**
 * This function is used to rebuild the form if an ajax call is made vai a
 * button. The button causes the form to be submitted. We don't want this so we
 * override the validate and submit routines on the form button. Therefore,
 * this function only needs to tell Drupal to rebuild the form
 *
 * @ingroup tripal_pub
 */
function tripal_pub_setup_form_ajax_button_validate($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * This function is just a dummy to override the default form submit on ajax
 * calls for buttons
 *
 * @ingroup tripal_pub
 */
function tripal_pub_setup_form_ajax_button_submit($form, &$form_state) {
  // do nothing
}

/**
 * Validate the tripal_pub_importer_setup_form form
 *
 * @ingroup tripal_pub
 */
function tripal_pub_importer_setup_form_validate($form, &$form_state) {
  $num_criteria = $form_state['values']['num_criteria'];
  $remote_db = $form_state['values']["remote_db"];
  $days = trim($form_state['values']["days"]);
  $latestyear = trim($form_state['values']["latestyear"]);
  $disabled = $form_state['values']["disabled"];
  $do_contact = $form_state['values']["do_contact"];
  $loader_name = trim($form_state['values']["loader_name"]);

  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms = trim($form_state['values']["search_terms-$i"]);
    $scope = $form_state['values']["scope-$i"];
    $is_phrase = $form_state['values']["is_phrase-$i"];
    $operation = '';
    if ($i > 1) {
      $operation = $form_state['values']["operation-$i"];
    }

    if (!$is_phrase) {
      if (preg_match('/\sand\s/i', $search_terms) and preg_match('/\sor\s/i', $search_terms)) {
        form_set_error("search_terms-$i", "You may use 'AND' or 'OR' but cannot use both. Add a new entry below with the same scope for the other conunction.");
        $_SESSION['tripal_pub_import']['perform_search'] = 0;
      }
    }
  }

  if ($days and !is_numeric($days) or preg_match('/\./', $days)) {
    form_set_error("days", "Please enter a numeric, non decimal value, for the number of days.");
    $_SESSION['tripal_pub_import']['perform_search'] = 0;
  }
  if ($latestyear and !is_numeric($latestyear) or preg_match('/\./', $latestyear)) {
    form_set_error("latestyear", "Please enter a numeric, non decimal value, for latestyear.");
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
  $remote_db = $form_state['values']["remote_db"];
  $days = trim($form_state['values']["days"]);
  $latestyear = trim($form_state['values']["latestyear"]);
  $loader_name = trim($form_state['values']["loader_name"]);
  $disabled = $form_state['values']["disabled"];
  $do_contact = $form_state['values']["do_contact"];

  // set the session variables
  $_SESSION['tripal_pub_import']['remote_db'] = $remote_db;
  $_SESSION['tripal_pub_import']['days'] = $days;
  $_SESSION['tripal_pub_import']['latestyear'] = $latestyear;
  $_SESSION['tripal_pub_import']['num_criteria'] = $num_criteria;
  $_SESSION['tripal_pub_import']['loader_name'] = $loader_name;
  $_SESSION['tripal_pub_import']['disabled'] = $disabled;
  $_SESSION['tripal_pub_import']['do_contact'] = $do_contact;
  $_SESSION['tripal_pub_import']['pub_import_id'] = $pub_import_id;
  unset($_SESSION['tripal_pub_import']['criteria']);
  for ($i = 1; $i <= $num_criteria; $i++) {
    $search_terms = trim($form_state['values']["search_terms-$i"]);
    $scope = $form_state['values']["scope-$i"];
    $is_phrase = $form_state['values']["is_phrase-$i"];
    $operation = '';
    if ($i > 1) {
      $operation = $form_state['values']["operation-$i"];
    }

    $_SESSION['tripal_pub_import']['criteria'][$i] = [
      'search_terms' => $search_terms,
      'scope' => $scope,
      'is_phrase' => $is_phrase,
      'operation' => $operation,
    ];
  }

  // now perform the appropriate action for the button clicked
  if ($form_state['values']['op'] == 'Test Importer') {
    $_SESSION['tripal_pub_import']['perform_search'] = 1;
  }
  if ($form_state['values']['op'] == 'Save Importer' or
    $form_state['values']['op'] == 'Save & Import Now') {
    $record = [
      'name' => $loader_name,
      'criteria' => serialize($_SESSION['tripal_pub_import']),
      'disabled' => $disabled,
      'do_contact' => $do_contact,
    ];
    // first check to see if this pub_import_id is already present. If so,
    // do an update rather than an insert
    $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :pub_import_id";
    $importer = db_query($sql, [':pub_import_id' => $pub_import_id])->fetchObject();
    if ($importer) {
      // do the update
      $record['pub_import_id'] = $pub_import_id;
      if (drupal_write_record('tripal_pub_import', $record, 'pub_import_id')) {
        unset($_SESSION['tripal_pub_import']);
        drupal_set_message('Publication import settings updated.');
        drupal_goto('admin/tripal/loaders/pub');
      }
      else {
        drupal_set_message('Could not update publication import settings.', 'error');
      }
    }
    else {
      // do the insert
      if (drupal_write_record('tripal_pub_import', $record)) {
        unset($_SESSION['tripal_pub_import']);
        drupal_set_message('Publication import settings saved.');
        // if the user wants to do the import now then do it (may time out
        // for long jobs)
        if ($form_state['values']['op'] == 'Save & Import Now') {
          chado_execute_pub_importer($record['pub_import_id']);
        }
        drupal_goto('admin/tripal/loaders/pub');
      }
      else {
        drupal_set_message('Could not save publication import settings.', 'error');
      }
    }
  }
  if ($form_state['values']['op'] == 'Delete Importer') {
    $sql = "DELETE FROM {tripal_pub_import} WHERE pub_import_id = :pub_import_id";
    $success = db_query($sql, [':pub_import_id' => $pub_import_id]);
    if ($success) {
      drupal_set_message('Publication importer deleted.');
      drupal_goto('admin/tripal/loaders/pub');
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
  $markup = '';
  $markup .= '<div id="pub-search-form-row0">';
  $markup .= '  <div id="pub-search-form-row0-col1" style="float: left">' . drupal_render($form['remote_db']) . '</div>';
  $markup .= '  <div id="pub-search-form-row0-col2" style="float: left; margin-left: 10px">' . drupal_render($form['loader_name']) . '</div>';
  $markup .= '</div>';
  $markup .= '<div id="pub-search-form-row1" style="clear:both">';
  $markup .= '  <div id="pub-search-form-row1-col1">' . drupal_render($form['days']) . '</div>';
  // latest year field is used ony for AGL importer
  if ($variables['form']['remote_db']['#value'] == 'AGL') {
    $markup .= '  <div id="pub-search-form-row1-col2">' . drupal_render($form['latestyear']) . '</div>';
  }
  $markup .= '</div>';
  $markup .= '<div id="pub-search-form-row2">' . drupal_render($form['disabled']) . '</div>';
  $markup .= '<div id="pub-search-form-row3">' . drupal_render($form['do_contact']) . '</div>';

  // next render the criteria fields into a table format
  $rows = [];
  foreach ($form['criteria'] as $i => $element) {
    if (is_numeric($i)) {
      $rows[] = [
        drupal_render($element["operation-$i"]),
        drupal_render($element["scope-$i"]),
        drupal_render($element["search_terms-$i"]),
        drupal_render($element["is_phrase-$i"]),
        drupal_render($element["add-$i"]) . drupal_render($element["remove-$i"]),
      ];
    }
  }

  $headers = ['Operation', 'Scope', 'Search Terms', '', ''];
  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [
      'class' => ['tripal-data-table'],
    ],
    'sticky' => TRUE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];
  $criteria_table = theme_table($table);
  $markup .= $criteria_table;

  // add the rendered form
  $form = [
    '#markup' => $markup,
    '#prefix' => '<div id="tripal-pubs-importer-setup">',
    '#suffix' => '</div>',
  ];

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
  $args = [':import_id' => $import_id];
  $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :import_id ";
  $import = db_query($sql, $args)->fetchObject();

  $args = [$import_id, TRUE, FALSE];
  $includes = [];
  $includes[] = module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.pub_importers');
  tripal_add_job("Import publications $import->name", 'tripal_chado',
    'chado_execute_pub_importer', $args, $user->uid, 10, $includes);

  drupal_goto('admin/tripal/loaders/pub');
}

/**
 * Deletes a publication importer.
 *
 */
function tripal_pub_importer_delete($import_id) {

  $args = [':import_id' => $import_id];
  $sql = "DELETE FROM {tripal_pub_import} WHERE pub_import_id = :import_id";
  $success = db_query($sql, $args);

  if ($success) {
    drupal_set_message('Publication importer deleted.');
    drupal_goto('admin/tripal/loaders/pub');
  }
  else {
    drupal_set_message('Could not delete publication importer.', 'error');
  }
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
 * @param $job
 *   The jobs management object for the job if this function is run as a job.
 *   This argument is added by Tripal during a job run and is not needed if
 *   this function is run directly.
 *
 * @return
 *   Returns an array containing the number of publications that were
 *   inserted, updated, skipped and which had an error during import.
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_publications($pubs, $do_contact, $update = FALSE, $job = NULL) {

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog but we do for the job and to
  // the terminal
  $message_type = 'pub_import';
  $message_opts = [
    'watchdog' == TRUE,
    'job' => $job,
    'print' => TRUE,
  ];

  $report = [];
  $report['error'] = [];
  $report['inserted'] = [];
  $report['skipped'] = [];
  $report['updated'] = [];
  $total_pubs = count($pubs);

  // iterate through the publications and add each one
  $i = 1;
  foreach ($pubs as $pub) {
    $memory = number_format(memory_get_usage()) . " bytes";
    print "Processing $i of $total_pubs. Memory usage: $memory.\r";

    // implementation of year limits for AGL uses a 'passfilter' flag
    if ((!array_key_exists('passfilter', $pub)) or ($pub['passfilter'] == 1 )) {

      // add the publication to Chado
      $action = '';
      $pub_id = tripal_pub_add_publication($pub, $action, $do_contact, $update, $job);
      // $pub_id will be null if publication already existed
      if ($pub_id) {
        // add the publication cross reference (e.g. to PubMed)
        if ($pub_id and $pub['Publication Dbxref']) {
          $dbxref = [];
          if (preg_match('/^(.*?):(.*?)$/', trim($pub['Publication Dbxref']), $matches)) {
            $dbxref['db_name'] = $matches[1];
            $dbxref['accession'] = $matches[2];
          }
          else {
            tripal_report_error($message_type, TRIPAL_ERROR,
              'Unable to extract the dbxref to be associated with the publication (pub ID=@pub_id) from @dbxref. This reference should be [database-name]:[accession]',
              [
                '@pub_id' => $pub_id,
                '@dbxref' => $pub['Publication Dbxref'],
                $message_opts,
              ]
            );
          }
          $pub_dbxref = tripal_associate_dbxref('pub', $pub_id, $dbxref);
        }
        $pub['pub_id'] = $pub_id;
      }

      switch ($action) {
        case 'error':
          $report['error'][] = $pub['Citation'];
          break;
        case 'inserted':
          $report['inserted'][] = $pub['Citation'];
          break;
        case 'updated':
          $report['updated'][] = $pub['Citation'];
          break;
        case 'skipped':
          $report['skipped'][] = $pub['Citation'];
          break;
      }
    }
    // else pub failed AGL year filter
    else { $report['skipped'][] = $pub['Citation']; }
    $i++;
  }
  return $report;
}

/**
 * Adds a new publication to Chado.
 *
 * In addition, all properties and
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
 *   Optional. Set to TRUE if a contact entry should be added to the Chado
 *   contact table for authors of the publication.
 * @param $update_if_exists
 *   Optional.  If the publication already exists then this function will
 *   return without adding a new publication.  However, set this value to
 *   TRUE to force the function to pudate the publication using the
 *   $pub_details that are provided.
 * @param $job
 *   The jobs management object for the job if this function is run as a job.
 *   This argument is added by Tripal during a job run and is not needed if
 *   this function is run directly.
 *
 * @return
 *   If the publication already exists, is inserted or updated then the
 *   publication ID is returned, otherwise FALSE is returned. If the
 *   publication already exists and $update_if_exists is not TRUE then the
 *   $action variable is set to 'skipped'. If the publication already exists
 *   and $update_if_exists is TRUE and if the update was successful then
 *   $action is set to 'updated'.  Otherwise on successful insert the
 *   $action variable is set to 'inserted'.  If the function fails then the
 *   $action variable is set to 'error'
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_publication($pub_details, &$action, $do_contact = FALSE, $update_if_exists = FALSE, $job = NULL) {
  $pub_id = 0;

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog except for errors and to the job and
  // to the terminal
  $message_type = 'pub_import';
  $message_opts = [
    'watchdog' == FALSE,
    'job' => $job,
    'print' => TRUE,
  ];
  $error_opts = [
    'watchdog' == TRUE,
    'job' => $job,
    'print' => TRUE,
  ];

  if (!is_array($pub_details)) {
    return FALSE;
  }

  // Before proceeding check to see if the publication already exists. If there
  // is only one match and the $update_if_exists is NOT set then return FALSE.
  $pub_ids = chado_publication_exists($pub_details);

  if (count($pub_ids) == 1 and !$update_if_exists) {
    tripal_report_error($message_type, TRIPAL_NOTICE,
      "The following publication already exists on this site:  %title %dbxref (Matching Pub id: %ids). Skipping.",
      [
        '%title' => $pub_details['Citation'],
        '%dbxref' => $pub_details['Publication Dbxref'],
        '%ids' => implode(",", $pub_ids),
      ],
      $message_opts
    );
    $action = 'skipped';
    return FALSE;
  }

  // If we have more than one matching pub then return an error as we don't
  // know which to update even if update_if_exists is set to TRUE.
  if (count($pub_ids) > 1) {
    tripal_report_error($message_type, TRIPAL_NOTICE,
      "The following publication exists %num times on this site:  %title %dbxref (Matching Pub id: %ids). Skipping.",
      [
        '%num' => count($pub_ids),
        '%title' => $pub_details['Citation'],
        '%dbxref' => $pub_details['Publication Dbxref'],
        '%ids' => implode(",", $pub_ids),
      ],
      $message_opts
    );
    $action = 'skipped';
    return FALSE;
  }
  if (count($pub_ids) == 1 and $update_if_exists) {
    $pub_id = $pub_ids[0];
  }

  // Get the publication type (use the first publication type).
  if (array_key_exists('Publication Type', $pub_details)) {
    $pub_type = '';
    if (is_array($pub_details['Publication Type'])) {
      $pub_type = $pub_details['Publication Type'][0];
    }
    else {
      $pub_type = $pub_details['Publication Type'];
    }
    $identifiers = [
      'name' => $pub_type,
      'cv_id' => [
        'name' => 'tripal_pub',
      ],
    ];
    $pub_type = chado_get_cvterm($identifiers);
  }
  else {
    tripal_report_error($message_type, TRIPAL_ERROR,
      "The Publication Type is a required property but is missing", [], $error_opts);
    $action = 'error';
    return FALSE;
  }
  if (!$pub_type) {
    tripal_report_error($message_type, TRIPAL_ERROR,
      "Cannot find publication type: '%type'",
      ['%type' => $pub_details['Publication Type'][0]], $error_opts);
    $action = 'error';
    return FALSE;
  }

  // The series name field in the pub table is only 255 characters, so we
  // should trim just in case.
  $series_name = '';
  if (array_key_exists('Series_Name', $pub_details)) {
    $series_name = substr($pub_details['Series Name'], 0, 255);
  }
  if (array_key_exists('Journal Name', $pub_details)) {
    $series_name = substr($pub_details['Journal Name'], 0, 255);
  }

  // Build the values array for inserting or updating.
  $values = [
    'title' => $pub_details['Title'],
    'volume' => (isset($pub_details['Volume'])) ? $pub_details['Volume'] : '',
    'series_name' => $series_name,
    'issue' => (isset($pub_details['Issue'])) ? $pub_details['Issue'] : '',
    'pyear' => (isset($pub_details['Year'])) ? $pub_details['Year'] : '',
    'pages' => (isset($pub_details['Pages'])) ? $pub_details['Pages'] : '',
    'uniquename' => $pub_details['Citation'],
    'type_id' => $pub_type->cvterm_id,
  ];

  // If there is no pub_id then we need to do an insert.
  if (!$pub_id) {
    $options = ['statement_name' => 'ins_pub_tivoseispypaunty'];
    $pub = chado_insert_record('pub', $values, $options);
    if (!$pub) {
      tripal_report_error($message_type, TRIPAL_ERROR,
        "Cannot insert the publication with title: %title",
        ['%title' => $pub_details['Title']], $error_opts);
      $action = 'error';
      return FALSE;
    }
    $pub_id = $pub['pub_id'];
    $action = 'inserted';
  }

  // If there is a pub_id and we've been told to update, then do the update.
  else {
    if ($pub_id and $update_if_exists) {
      $match = ['pub_id' => $pub_id];
      $options = ['statement_name' => 'up_pub_tivoseispypaunty'];
      $success = chado_update_record('pub', $match, $values, $options);
      if (!$success) {
        tripal_report_error($message_type, TRIPAL_ERROR,
          "Cannot update the publication with title: %title",
          ['%title' => $pub_details['Title']], $error_opts);
        $action = 'error';
        return FALSE;
      }
      $action = 'updated';
    }
  }

  // Before we add any new properties we need to remove those that are there
  // if this is an update.  The only thing we don't want to remove are the
  // 'Publication Dbxref'.
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
    chado_query($sql, [':pub_id' => $pub_id]);
  }

  // Iterate through the properties and add them.
  foreach ($pub_details as $key => $value) {

    // The pub_details may have the raw search data (e.g. in XML from PubMed.
    // We'll irgnore this for now.
    if ($key == 'raw') {
      continue;
    }

    // Filtering flag for AGL, not a property to add here
    if ($key == 'passfilter') {
      continue;
    }

    // Since we're not updating the 'Publication Dbxref' on an update
    // skip this property.
    if ($update_if_exists and $key == 'Publication Dbxref') {
      continue;
    }

    // Get the cvterm by name.
    $identifiers = [
      'name' => $key,
      'cv_id' => [
        'name' => 'tripal_pub',
      ],
    ];
    $cvterm = chado_get_cvterm($identifiers);

    // If we could not find the cvterm by name then try by synonym.
    if (!$cvterm) {
      $identifiers = [
        'synonym' => [
          'name' => $key,
          'cv_name' => 'tripal_pub',
        ],
      ];
      $cvterm = chado_get_cvterm($identifiers);
    }
    if (!$cvterm) {
      tripal_report_error($message_type, TRIPAL_ERROR,
        "Cannot find term: '%prop'. Skipping.", ['%prop' => $key], $error_opts);
      continue;
    }

    // Skip details that won't be stored as properties.
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

        // If the key is an integer then this array is a simple list and
        // we will insert using the primary key. Otheriwse, use the new key.
        if (is_int($subkey)) {
          $success = chado_insert_property(
            ['table' => 'pub', 'id' => $pub_id],
            [
              'type_name' => $key,
              'cv_name' => 'tripal_pub',
              'value' => $subvalue,
            ]
          );
        }
        else {
          $success = chado_insert_property(
            ['table' => 'pub', 'id' => $pub_id],
            [
              'type_name' => $subkey,
              'cv_name' => 'tripal_pub',
              'value' => $subvalue,
            ]
          );
        }
      }
    }
    else {
      $success = chado_insert_property(
        ['table' => 'pub', 'id' => $pub_id],
        ['type_name' => $key, 'cv_name' => 'tripal_pub', 'value' => $value],
        ['update_if_present' => TRUE]
      );
    }
    if (!$success) {
      tripal_report_error($message_type, TRIPAL_ERROR,
        "Cannot add property '%prop' to pubprop table. Skipping.",
        ['%prop' => $key], $error_opts);
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
 *   Optional. Set to TRUE if a contact entry should be added to the Chado
 *   contact table for authors of the publication.
 *
 * @ingroup tripal_pub
 */
function tripal_pub_add_authors($pub_id, $authors, $do_contact) {
  $rank = 0;

  // First remove any of the existing pubauthor entires.
  $sql = "DELETE FROM {pubauthor} WHERE pub_id = :pub_id";
  chado_query($sql, [':pub_id' => $pub_id]);

  // Iterate through the authors and add them to the pubauthors and contact
  // tables of chado, then link them through the custom pubauthors_contact
  // table.
  foreach ($authors as $author) {
    // Skip invalid author entires.
    if (isset($author['valid']) AND $author['valid'] == 'N') {
      continue;
    }
    // remove the 'valid' property as we don't have a CV term for it
    unset($author['valid']);

    $values = [
      'pub_id' => $pub_id,
      'rank' => $rank,
    ];

    // construct the contact.name field using the author information
    $name = '';
    $type = 'Person';
    if (isset($author['Given Name'])) {
      $name .= $author['Given Name'];
      $values['givennames'] = $author['Given Name'];
    }
    if (isset($author['Surname'])) {
      $name .= ' ' . $author['Surname'];
      $values['surname'] = substr($author['Surname'], 0, 100);
    }
    if (isset($author['Suffix'])) {
      $name .= ' ' . $author['Suffix'];
      $values['suffix'] = $author['Suffix'];
    }
    if (isset($author['Collective'])) {
      $name = $author['Collective'];
      $type = 'Collective';
      if (!isset($author['Surname'])) {
        $values['surname'] = substr($author['Collective'], 0, 100);
      }
    }
    $name = trim($name);

    // add an entry to the pubauthors table
    $options = ['statement_name' => 'ins_pubauthor_idrasugisu'];
    $pubauthor = chado_insert_record('pubauthor', $values, $options);

    // if the user wants us to create a contact for each author then do it.
    if ($do_contact) {
      // Add the contact
      $contact = chado_insert_contact([
        'name' => $name,
        'description' => '',
        'type_name' => $type,
        'properties' => $author,
      ]);

      // if we have succesfully added the contact and the pubauthor entries then we want to
      // link them together
      if ($contact and $pubauthor) {

        // link the pubauthor entry to the contact
        $values = [
          'pubauthor_id' => $pubauthor['pubauthor_id'],
          'contact_id' => $contact['contact_id'],
        ];
        $options = ['statement_name' => 'ins_pubauthorcontact_puco'];
        $pubauthor_contact = chado_insert_record('pubauthor_contact', $values, $options);
        if (!$pubauthor_contact) {
          tripal_report_error('tripal_pub', TRIPAL_ERROR, "Cannot link pub authro and contact.", []);
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

  $options = ['return_array' => 1];

  // ---------------------------------
  // get the publication
  // ---------------------------------
  $values = ['pub_id' => $pub_id];
  $pub = chado_generate_var('pub', $values);

  // expand the title
  $pub = chado_expand_var($pub, 'field', 'pub.title');
  $pub = chado_expand_var($pub, 'field', 'pub.volumetitle');
  $pub = chado_expand_var($pub, 'field', 'pub.uniquename');
  $pub_array = [];
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
  $values = [
    'pub_id' => $pub->pub_id,
    'type_id' => [
      'name' => 'Citation',
    ],
  ];
  $citation = chado_generate_var('pubprop', $values);
  if ($citation) {
    $citation = chado_expand_var($citation, 'field', 'pubprop.value', $options);
    if (count($citation) > 1) {
      tripal_report_error('tripal_pub', TRIPAL_ERROR, "Publication has multiple citations already: %pub_id",
        ['%pub_id' => $pubid]);
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
  $values = [
    'pub_id' => $pub->pub_id,
    'type_id' => [
      'name' => 'Publication Type',
    ],
  ];
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
  $values = [
    'pub_id' => $pub->pub_id,
    'type_id' => [
      'name' => 'Authors',
    ],
  ];
  $authors = chado_generate_var('pubprop', $values);
  $authors = chado_expand_var($authors, 'field', 'pubprop.value', $options);
  if (count($authors) > 1) {
    tripal_report_error('tripal_pub', TRIPAL_ERROR, "Publication has multiple author lists. It should have only one list: %pub_id",
      ['%pub_id' => $pubid]);
    return FALSE;
  }
  else {
    if (trim($authors->value)) {
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
      $au = chado_query($sql, [':pub_id' => $pub_id])->fetchField();
      if ($au) {
        $pub_array['Authors'] = $au;
      }
    }
  }

  //Get other props
  $props = [
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
    'Original Title',
  ];
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
    $val = trim(chado_query($sql, [
      ':cvtname' => $prop,
      ':pub_id' => $pub->pub_id,
    ])->fetchField());
    if ($val) {
      $pub_array[$prop] = $val;
    }
  }
  return $pub_array;
}


/**
 * This function is used to perfom a query using one of the supported databases
 * and return the raw query results. This may be XML or some other format
 * as provided by the database.
 *
 * @param $dbxref
 *   The unique database ID for the record to retrieve.  This value must
 *   be of the format DB_NAME:ACCESSION where DB_NAME is the name of the
 *   database (e.g. PMID or AGL) and the ACCESSION is the unique identifier
 *   for the record in the database.
 *
 * @return
 *   Returns the publication array or FALSE if a problem occurs
 *
 * @ingroup tripal_pub
 */
function tripal_get_remote_pub($dbxref) {

  if (preg_match('/^(.*?):(.*?)$/', $dbxref, $matches)) {
    $remote_db = $matches[1];
    $accession = $matches[2];

    // check that the database is supported
    $supported_dbs = variable_get('tripal_pub_supported_dbs', ['PMID']);
    if (!in_array($remote_db, $supported_dbs)) {
      return FALSE;
    }

    $search = [
      'num_criteria' => 1,
      'remote_db' => $remote_db,
      'criteria' => [
        '1' => [
          'search_terms' => "$remote_db:$accession",
          'scope' => 'id',
          'operation' => '',
          'is_phrase' => 0,
        ],
      ],
    ];
    $pubs = tripal_get_remote_pubs($remote_db, $search, 1, 0);

    return $pubs['pubs'][0];
  }
  return FALSE;
}

/**
 * Retrieves a list of publications as an associated array where
 *  keys correspond directly with Tripal Pub CV terms.
 *
 * @param remote_db
 *    The name of the remote publication database to query. These names should
 *    match the name of the databases in the Chado 'db' table. Currently
 *    supported databass include
 *      'PMID':  PubMed
 *
 * @param search_array
 *    An associate array containing the search criteria. The following key
 *    are expected
 *      'remote_db':     Specifies the name of the remote publication database
 *      'num_criteria':  Specifies the number of criteria present in the search
 *   array
 *      'days':          The number of days to include in the search starting
 *   from today
 *      'criteria':      An associate array containing the search critiera.
 *   There should be no less than 'num_criteria' elements in this array.
 *
 *    The following keys are expected in the 'criteria' array
 *      'search_terms':  A list of terms to search on, separated by spaces.
 *      'scope':         The fields to search in the remote database. Valid
 *   values include: 'title', 'abstract', 'author' and 'any'
 *      'operation':     The logical operation to use for this criteria. Valid
 *                       values include: 'AND', 'OR' and 'NOT'.
 * @param $num_to_retrieve
 *    The number of records to retrieve.  In cases with large numbers of
 *    records to retrieve, the remote database may limit the size of each
 *    retrieval.
 * @param $page
 *    Optional.  If this function is called where the
 *    page for the pager cannot be set using the $_GET variable, use this
 *    argument to specify the page to retrieve.
 *
 * @return
 *   Returns an array of pubs where each element is
 *   an associative array where the keys are Tripal Pub CV terms.
 *
 * @ingroup tripal_pub
 */
function tripal_get_remote_pubs($remote_db, $search_array, $num_to_retrieve, $page = 0) {

  // now call the callback function to get the results
  $callback = "tripal_pub_remote_search_$remote_db";
  $pubs = [
    'total_records' => 0,
    'search_str' => '',
    'pubs' => [],
  ];
  if (function_exists($callback)) {
    $pubs = call_user_func($callback, $search_array, $num_to_retrieve, $page);
  }
  return $pubs;
}

/**
 * The admin form for submitting job to create citations
 *
 * @param $form_state
 *
 * @ingroup tripal_pub
 */
function tripal_pub_citation_form($form, &$form_state) {

  $form['instructions'] = [
    '#markup' => '<p>Use this form to unify publication citations. Citations are created automtically when
      importing publications but citations are set by the user when publications are added manually.
      Or publications added to the Chado database by tools other than the Tripal Publication Importer may
      not have citations set. If you are certain that all necessary information for all publications is present (e.g.
      authors, volume, issue, page numbers, etc.) but citations are not consistent, then you can
      choose to update all citations for all publications using the form below. Alternatively, you
      can update citations only for publication that do not already have one.</p>',
  ];

  $form['options'] = [
    '#type' => 'radios',
    '#options' => [
      'all' => 'Create citation for all publications. Replace the existing citation if it exists.',
      'new' => 'Create citation for publication only if it does not already have one.',
    ],
    '#default_value' => 'all',
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Submit'),
  ];

  return $form;
}

/**
 * Submit form. Create Tripal job for citations
 *
 * @param $form_state
 *
 * @ingroup tripal_pub
 */
function tripal_pub_citation_form_submit(&$form_state) {
  $options [0] = $form_state['options']['#value'];
  tripal_add_job("Create citations ($options[0])", 'tripal_pub', 'chado_pub_create_citations', $options, $user->uid);
}
