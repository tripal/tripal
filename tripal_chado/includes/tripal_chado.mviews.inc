<?php
/**
 * @file
 * Contains functions for viewing and editing of Materialized Views
 * on a Tripal website.
 */

/**
 * Provides a landing page for tripal jobs admin
 */
function tripal_mview_admin_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Schema', 'admin/tripal/storage/chado');
  $breadcrumb[] = l('Materialized Views', 'admin/tripal/storage/chado/mviews');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_admin_mviews', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Materalized View management system uses Drupal Views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('MViews View', 'admin/tripal/storage/chado/mviews/views/mviews/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * A template function which returns markup to display details for the current
 * materialized view
 *
 * @param $mview_id
 *  The unique ID of the materialized view to render
 */
function tripal_mview_report($mview_id) {
  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Schema', 'admin/tripal/storage/chado');
  $breadcrumb[] = l('Materialied Views', 'admin/tripal/storage/chado/mviews');
  drupal_set_breadcrumb($breadcrumb);

  // get this mview details
  $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :mview_id";
  $results = db_query($sql, [':mview_id' => $mview_id]);
  $mview = $results->fetchObject();

  $rows = [];

  // create a table with each row containig stats for
  // an individual job in the results set.
  $output = "<p>Details for <b>$mview->name</b>:</p>";

  // build the URLs using the url function so we can handle installations where
  // clean URLs are or are not used
  $update_url = url("admin/tripal/storage/chado/mviews/action/update/$mview->mview_id");
  $delete_url = url("admin/tripal/storage/chado/mviews/action/delete/$mview->mview_id");
  $edit_url = url("admin/tripal/storage/chado/mviews/edit/$mview->mview_id");
  $export_url = url("admin/tripal/storage/chado/mviews/export/$mview->mview_id");
  $rows[] = [
    'Actions',
    "<a href='$update_url'>Populate</a>, <a href='$edit_url'>Edit</a>,  <a href='$delete_url'>Delete</a>",
  ];

  if ($mview->last_update > 0) {
    $update = format_date($mview->last_update);
  }
  else {
    $update = 'Not yet populated';
  }
  $rows[] = ['Last Update', $update];
  if ($mview->name) {
    $rows[] = ['View Name', $mview->name];
  }
  if ($mview->modulename) {
    $rows[] = ['Module Name', $mview->modulename];
  }
  if ($mview->mv_table) {
    $rows[] = ['Table Name', $mview->mv_table];
  }
  if ($mview->mv_specs) {
    $rows[] = ['Table Field Definitions', $mview->mv_specs];
  }
  if ($mview->query) {
    $rows[] = [
      'Query',
      "<textarea rows=\"15\" cols=\"120\" style=\"font-family:Consolas,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New, monospace;\">" . $mview->query . "</textarea>",
    ];
  }
  if ($mview->indexed) {
    $rows[] = ['Indexed Fields', $mview->indexed];
  }
  if ($mview->special_index) {
    $rows[] = ['Special Indexed Fields', $mview->special_index];
  }
  if ($mview->mv_schema) {
    $rows[] = [
      'Drupal Schema API Definition',
      "<textarea rows=\"20\" cols=\"120\" style=\"font-family:Consolas,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New, monospace;\">" . $mview->mv_schema . "</textarea>",
    ];
  }

  $header = ['Detail', 'Value'];
  $table = [
    'header' => $header,
    'rows' => $rows,
    'attributes' => ['class' => 'tripal-data-table'],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => 'There are no materialized views',
  ];
  $table = theme_table($table);
  $output .= $table;

  return $output;
}

/**
 * A Form to Create/Edit a Materialized View
 *
 * @param $form_state
 *   The current state of the form (Form API)
 * @param $mview_id
 *   The unique ID of the Materialized View to Edit or NULL if creating a new
 *   materialized view
 *
 * @return
 *   A form array (Form API)
 */
function tripal_mviews_form($form, &$form_state = NULL, $mview_id = NULL) {

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Schema', 'admin/tripal/storage/chado');
  $breadcrumb[] = l('Materialied Views', 'admin/tripal/storage/chado/mviews');
  drupal_set_breadcrumb($breadcrumb);


  if (!$mview_id) {
    $action = 'Add';
  }
  else {
    $action = 'Edit';
  }

  // set defaults for collapsed fieldsets
  $schema_collapsed = 0;
  $traditional_collapsed = 1;

  $default_name = '';
  $default_mv_table = '';
  $default_mv_specs = '';
  $default_indexed = '';
  $default_mvquery = '';
  $default_special_index = '';
  $default_comment = '';
  $default_modulename = '';
  $default_schema = '';

  // if the view is the older style legacy view then this value get's set to 1
  $is_legacy = 0;


  // get this requested view
  if (strcmp($action, 'Edit') == 0) {
    $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :mview_id ";
    $mview = db_query($sql, [':mview_id' => $mview_id])->fetchObject();

    // set the default values.  If there is a value set in the
    // form_state then let's use that, otherwise, we'll pull
    // the values from the database
    if (array_key_exists('values', $form_state)) {
      $default_name = $form_state['values']['name'];
      $default_mv_table = $form_state['values']['mv_table'];
      $default_mv_specs = $form_state['values']['mv_specs'];
      $default_indexed = $form_state['values']['indexed'];
      $default_mvquery = $form_state['values']['mvquery'];
      $default_special_index = $form_state['values']['special_index'];
      $default_comment = $form_state['values']['comment'];
      $default_modulename = $form_state['values']['modulename'];
    }

    if (!$default_name) {
      $default_name = $mview->name;
    }
    if (!$default_mv_table) {
      $default_mv_table = $mview->mv_table;
    }
    if (!$default_mv_specs) {
      $default_mv_specs = $mview->mv_specs;
    }
    if (!$default_indexed) {
      $default_indexed = $mview->indexed;
    }
    if (!$default_mvquery) {
      $default_mvquery = $mview->query;
    }
    if (!$default_special_index) {
      $default_special_index = $mview->special_index;
    }
    if (!$default_comment) {
      $default_comment = $mview->comment;
    }
    if (!$default_schema) {
      $default_schema = $mview->mv_schema;
    }
    if (!$default_modulename) {
      $default_modulename = $mview->modulename ? $mview->modulename : 'tripal_chado';
    }

    if ($mview->mv_specs) {
      $is_legacy = 1;
    }

    // the mv_table column of the tripal_mviews table always has the table
    // name even if it is a custom table. However, for the sake of the form,
    // we do not want this to show up as the mv_table is needed for the
    // traditional style input.  We'll blank it out if we have a custom
    // table and it will get reset in the submit function using the
    // 'table' value from the schema array
    if ($default_schema) {
      $default_mv_table = '';
    }
    // set which fieldset is collapsed
    if (!$default_schema) {
      $schema_collapsed = 1;
      $traditional_collapsed = 0;
    }
  }

  // Build the form
  $form['action'] = [
    '#type' => 'value',
    '#value' => $action,
  ];

  $form['is_legacy'] = [
    '#type' => 'value',
    '#value' => $is_legacy,
  ];

  $form['mview_id'] = [
    '#type' => 'value',
    '#value' => $mview_id,
  ];

  $form['modulename'] = [
    '#type' => 'value',
    '#value' => $default_modulename,
  ];

  $form['instructions'] = [
    '#type' => 'fieldset',
    '#title' => 'Instructions',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];
  $form['instructions']['text'] = [
    '#type' => 'item',
    '#markup' => t('Materialized views are used to help speed data
      querying, particularly for searching.  A materialized view is essentially
      a database table that is pre-populated with the desired data to search on.
      Rows in the materialized view are typically a combination of data from
      multiple tables with indexes on searchable columns. The table structure
      for materialized views is defined using the ' .
      l('Drupal Schema API', 'https://api.drupal.org/api/drupal/includes!database!schema.inc/group/schemaapi/7',
        ['attributes' => ['target' => '_blank']]) . '. ' . t('Additionally,
      an SQL statement is provided that populates the table with data. ' .
        'Please note that table names should be all lower-case.')
    ),
  ];
  $form['instructions']['example_schema'] = [
    '#type' => 'item',
    '#markup' => "An example Schema API definition for a materialized view: <pre>
array (
  'description' => 'Stores the type and number of features per organism',
  'table' => 'organism_feature_count',
  'fields' => array (
    'organism_id' => array (
      'type' => 'int',
      'not null' => true,
    ),
    'genus' => array (
      'type' => 'varchar',
      'length' => '255',
      'not null' => true,
    ),
    'species' => array (
      'type' => 'varchar',
      'length' => '255',
      'not null' => true,
    ),
    'common_name' => array (
      'type' => 'varchar',
      'length' => '255',
      'not null' => false,
    ),
    'num_features' => array (
      'type' => 'int',
      'not null' => true,
    ),
    'cvterm_id' => array (
      'type' => 'int',
      'not null' => true,
    ),
    'feature_type' => array (
      'type' => 'varchar',
      'length' => '255',
      'not null' => true,
    ),
  ),
  'indexes' => array (
    'organism_id_idx'  => array ('organism_id'),
    'cvterm_id_idx'    => array ('cvterm_id'),
    'feature_type_idx' => array ('feature_type'),
  ),
)
</pre>",
  ];
  $form['instructions']['example_sql'] = [
    '#type' => 'item',
    '#markup' => "An example SQL statement to populate the table: <pre>
SELECT
    O.organism_id, O.genus, O.species, O.common_name,
    count(F.feature_id) as num_features,
    CVT.cvterm_id, CVT.name as feature_type
 FROM organism O
    INNER JOIN feature F  ON O.Organism_id = F.organism_id
    INNER JOIN cvterm CVT ON F.type_id     = CVT.cvterm_id
 GROUP BY
    O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
    </pre>",
  ];


  $form['name'] = [
    '#type' => 'textfield',
    '#title' => t('View Name'),
    '#description' => t('Please enter the name for this materialized view.'),
    '#required' => TRUE,
    '#default_value' => $default_name,
  ];

  $form['comment'] = [
    '#type' => 'textarea',
    '#title' => t('MView Description'),
    '#description' => t('Optional.  Please provide a description of the purpose for this materialized vieww.'),
    '#required' => FALSE,
    '#default_value' => $default_comment,
  ];

  // add a fieldset for the Drupal Schema API
  $form['schema'] = [
    '#type' => 'fieldset',
    '#title' => 'Table Schema',
    '#description' => t('Use a ' . l("Drupal Schema API", "https://api.drupal.org/api/drupal/includes!database!schema.inc/group/schemaapi/7", ['attributes' => ['target' => '_blank']]) .
      ' array to describe the table. See the bottom of this page for an example.'),
    '#collapsible' => 1,
    '#collapsed' => $schema_collapsed,
  ];

  $form['schema']['schema'] = [
    '#type' => 'textarea',
    '#title' => t('Schema Array'),
    '#description' => t('Please enter the ' . l("Drupal Schema API", "https://api.drupal.org/api/drupal/includes!database!schema.inc/group/schemaapi/7", ['attributes' => ['target' => '_blank']]) .
      ' compatible array that defines the table. There must also be a "table" key with the name of the table as the value. See the example at the bottom of this page.'),
    '#required' => FALSE,
    '#default_value' => $default_schema,
    '#rows' => 25,
    '#attributes' => [
      'style' => "font-family:Consolas,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New, monospace;",
    ],
  ];

  // only let folks edit legacy MViews, not create new ones
  if ($is_legacy) {
    // add a fieldset for the Original Table Description fields
    $form['traditional'] = [
      '#type' => 'fieldset',
      '#title' => 'Legacy MViews Setup',
      '#description' => t('<font color="red">Tripal no longer supports editing of legacy style materialized views. </font> This view will continue to function and you can populate it, however, to update you must convert this view to the newer Drupal Schema API format using the "Table Schema" section above.  Unfortunately, after converting this view to the Schema API and saving, the materialized view be recreated and emptied. You will need to re-populate it. Therefore, you may want to schedule update of this or any other legacy materialized during your next site maintenance.'),
      '#collapsible' => 1,
      '#collapsed' => $traditional_collapsed,
    ];

    $form['traditional']['mv_table'] = [
      '#type' => 'textfield',
      '#title' => t('Table Name'),
      '#description' => t('Please enter the table name that this view will generate in the database.  You can use the schema and table name for querying the view'),
      '#required' => FALSE,
      '#default_value' => $default_mv_table,
      '#attributes' => ['disabled' => 'disabled'],
    ];

    $form['traditional']['mv_specs'] = [
      '#type' => 'textarea',
      '#title' => t('Table Definition'),
      '#description' => t('Please enter the field definitions for this view. Each field should be separated by a comma or enter each field definition on each line.'),
      '#required' => FALSE,
      '#default_value' => $default_mv_specs,
      '#attributes' => ['disabled' => 'disabled'],
    ];

    $form['traditional']['indexed'] = [
      '#type' => 'textarea',
      '#title' => t('Indexed Fields'),
      '#description' => t('Please enter the field names (as provided in the table definition above) that will be indexed for this view.  Separate by a comma or enter each field on a new line.'),
      '#required' => FALSE,
      '#default_value' => $default_indexed,
      '#attributes' => ['disabled' => 'disabled'],
    ];

    /**
     * $form['traditional']['special_index']= array(
     * '#type'          => 'textarea',
     * '#title'         => t('View Name'),
     * '#description'   => t('Please enter the name for this materialized view.'),
     * '#required'      => TRUE,
     * '#default_value' => $default_special_index,
     * );
     */
  }

  $form['mvquery'] = [
    '#type' => 'textarea',
    '#title' => t('Query'),
    '#description' => t('Please enter the SQL statement used to populate the table.'),
    '#required' => TRUE,
    '#default_value' => $default_mvquery,
    '#rows' => 25,
    '#attributes' => [
      'style' => "font-family:Consolas,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New, monospace;",
    ],
  ];

  if ($action == 'Edit') {
    $value = 'Save';
  }
  if ($action == 'Add') {
    $value = 'Add';
  }
  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t($value),
    '#executes_submit_callback' => TRUE,
  ];
  $form['cancel'] = [
    '#type' => 'markup',
    '#markup' => l('Cancel', 'admin/tripal/storage/chado/mviews'),
  ];


  return $form;
}

/**
 * Validate the Create/Edit Materialized View Form
 * Implements hook_form_validate().
 */
function tripal_mviews_form_validate($form, &$form_state) {


  $action = $form_state['values']['action'];
  $mview_id = $form_state['values']['mview_id'];
  $name = trim($form_state['values']['name']);
  $is_legacy = $form_state['values']['is_legacy'];
  $query = $form_state['values']['mvquery'];

  // if this is a legacy materialized view (no longer supported in Tripal v2.0
  // but code left just in case)
  if ($is_legacy) {
    $mv_table = trim($form_state['values']['mv_table']);
    $mv_specs = $form_state['values']['mv_specs'];
    $indexed = $form_state['values']['indexed'];
    $special_index = '';//$form_state['values']['special_index'];
  }
  else {
    $mv_table = '';
    $mv_specs = '';
    $indexed = '';
    $special_index = '';
  }
  $comment = trim($form_state['values']['comment']);
  $schema = $form_state['values']['schema'];

  // validate the contents of the array
  $schema_array = [];
  $success = eval("\$schema_array = $schema;");
  $error = chado_validate_custom_table_schema($schema_array);
  if ($error) {
    form_set_error('schema', $error);
  }

  // if both the schema and the older fields for the legacy view are populated then
  // this is an error and we need to let the user know.
  if ($schema and ($mv_table or $mv_specs or $indexed or $special_index)) {
    form_set_error($form_state['values']['schema'],
      t('You can create an MView using the Drupal Schema API method or the ' .
        'traditional method but not both.'));
  }

  // if we don't have a schema and are missing fields for the legacy views then
  // inform the user.
  if (!$schema) {
    if (!$mv_specs) {
      form_set_error($form_state['values']['mv_specs'],
        t('The Table Definition field is required.'));
    }
    if (!$mv_table) {
      form_set_error($form_state['values']['mv_table'],
        t('The Table Name field is required.'));
    }
  }

  // make sure the array is valid
  if ($schema) {
    $success = eval("\$schema_array = $schema;");
    if ($success === FALSE) {
      $error = error_get_last();
      form_set_error($form_state['values']['schema'],
        t("The schema array is improperly formatted. Parse Error : " . $error["message"]));
    }
    if (!array_key_exists('table', $schema_array)) {
      form_set_error($form_state['values']['schema'],
        t("The schema array must have key named 'table'"));
    }
    // TODO: add in more validation checks of the array to help the user
  }
}

/**
 * Submit the Create/Edit Materialized View Form
 * Implements hook_form_submit().
 */
function tripal_mviews_form_submit($form, &$form_state) {

  $ret = [];

  $action = $form_state['values']['action'];
  $mview_id = $form_state['values']['mview_id'];
  $name = trim($form_state['values']['name']);
  $is_legacy = $form_state['values']['is_legacy'];
  $query = $form_state['values']['mvquery'];
  $comment = trim($form_state['values']['comment']);
  $schema = $form_state['values']['schema'];
  $modulename = trim($form_state['values']['modulename']);
  $mv_table = '';
  $mv_specs = '';
  $indexed = '';
  $special_index = '';

  // if this is a legacy materialized view (no longer supported in Tripal v2.0
  // but code left just in case)
  if ($is_legacy) {
    $mv_table = $form_state['values']['mv_table'];
    $mv_specs = $form_state['values']['mv_specs'];
    $indexed = $form_state['values']['indexed'];
    $special_index = '';//$form_state['values']['special_index'];
  }

  if (!$modulename) {
    $modulename = 'tripal_chado';
  }

  // if this is an edit action
  if (strcmp($action, 'Edit') == 0) {
    chado_edit_mview($mview_id, $name, $modulename, $mv_table, $mv_specs,
      $indexed, $query, $special_index, $comment, $schema);
  }
  // else an add action
  elseif (strcmp($action, 'Add') == 0) {
    // convert the schema into a PHP array
    $schema_arr = [];
    eval("\$schema_arr = $schema;");
    tripal_add_mview($name, $modulename, $schema_arr, $query, $comment);
    drupal_goto("admin/tripal/storage/chado/mviews");
  }
  else {
    drupal_set_message(t("No action performed."));
  }

  return '';
}


/**
 * Just a simple form for confirming deletion of a custom table
 */
function tripal_mviews_delete_form($form, &$form_state, $mview_id) {

  // get details about this table entry
  $sql = "SELECT * FROM {tripal_mviews} WHERE mview_id = :mview_id";
  $results = db_query($sql, [':mview_id' => $mview_id]);
  $entry = $results->fetchObject();

  $form = [];
  $form['mview_id'] = [
    '#type' => 'value',
    '#value' => $mview_id,
  ];

  $form['sure'] = [
    '#type' => 'markup',
    '#markup' => '<p>Are you sure you want to delete the "' . $entry->name . '" materialized view?</p>',
  ];
  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Delete',
  ];
  $form['cancel'] = [
    '#type' => 'submit',
    '#value' => 'Cancel',
  ];
  return $form;
}

/**
 * form submit hook for the tripal_custom_tables_delete_form form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_mviews_delete_form_submit($form, &$form_state) {
  $action = $form_state['clicked_button']['#value'];
  $mview_id = $form_state['values']['mview_id'];

  if (strcmp($action, 'Delete') == 0) {
    chado_delete_mview($mview_id);
  }
  else {
    drupal_set_message(t("No action performed."));
  }
  drupal_goto("admin/tripal/storage/chado/mviews");
}

/**
 * A wrapper for the chado_refresh_mview() API function, which
 * then redirects back to the admin page for mviews.
 *
 * @param $mview_id
 */
function tripal_mviews_add_populate_job($mview_id) {

  chado_refresh_mview($mview_id);
  drupal_goto("admin/tripal/storage/chado/mviews");
}
