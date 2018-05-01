<?php

/**
 * @file
 * API to manage the Chado dbxref table for various Tripal Node Types
 *
 * How To Use:
 * @code
 *
 * function chado_example_form($form, &$form_state) {
 *
 * // Default values for form elements can come in the following ways:
 * //
 * // 1) as elements of the $node object.  This occurs when editing an existing
 *   node
 * // 2) in the $form_state['values'] array which occurs on a failed validation
 *   or
 * //    ajax callbacks when the ajax call originates from non-submit fields
 *   other
 * //    than button
 * // 3) in the $form_state['input'] array which occurs on ajax callbacks from
 *   submit
 * //    form elements (e.g. buttons) and the form is being rebuilt but has not
 *   yet
 * //    been validated
 * //
 * // The reference elements added by this function do use AJAX calls from
 *   buttons,
 * // therefore, it is important to check for form values in the
 *   $form_state['values']
 * // for case #2 above, and in the $form_state['input'] for case #3.
 * // See the chado analysis node form for an example.
 *
 *
 * // Next, add in all the form array definition particular to your node type
 *
 * // To add in the additional database reference form elements, you first need
 *   to prepare the arguments
 * // for the function call.
 *
 * $details = array(
 * 'linking_table' => 'example_dbxref',         // the name of the table
 *   linking additional dbxrefs to this node
 * 'base_foreign_key' => 'example_id',          // key to link to the chado
 *   content created by this node
 * 'base_key_value' => $example_id,             // the value of the above key
 * 'fieldset_title' => 'Additional References', // the non-translated title for
 *   this fieldset
 * 'additional_instructions' => ''              // a non-stranslated string
 *   providing additional instructions
 * );
 *
 * // Finally, and add the additional form elements to the form
 * chado_add_node_form_dbxrefs($form, $form_state, $details);
 *
 * return $form;
 * }
 *
 * function chado_example_insert($node) {
 *
 * // if there is an example_id in the $node object then this must be a sync so
 * // we can skip adding the chado_example as it is already there, although
 * // we do need to proceed with the rest of the insert
 * if (!property_exists($node, 'example_id')) {
 *
 * // Add record to chado example table
 *
 * // Add to any other tables needed
 *
 * // Add all additional database references
 * // This function will create new database references as needed and select
 *   existing ones.
 * // Existing _dbxref links will be cleared and then re-added
 * chado_update_node_form_dbxrefs(
 * $node,              // the node object passed in via hook_insert()
 * 'example_dbxref',   // the name of the _dbxref linking table
 * 'example_id',       // key to link to the chado content created by this node
 * $node->example_id   // value of the above key
 * );
 * }
 *
 * // Add record to chado_example linking example_id to new node
 *
 * }
 *
 * function chado_example_update($node) {
 *
 *
 * // Update record in chado example table
 *
 * // Update any other tables needed
 *
 * // Update all additional database references
 * // This function will create new database references as needed and select
 *   existing ones.
 * // Existing _dbxref links will be cleared and then re-added
 * chado_update_node_form_dbxrefs(
 * $node,              // the node object passed in via hook_insert()
 * 'example_dbxref',   // the name of the _dbxref linking table
 * 'example_id',       // key to link to the chado content created by this node
 * $node->example_id   // value of the above key
 * );
 *
 * // Don't need to update chado_example linking table since niether example_id
 *   or nid can be changed in update
 *
 * }
 * @endcode
 *
 * @ingroup tripal_legacy_chado_node_api
 */

/**
 * @section
 * Additional Database References Form to be added to node forms
 */

/**
 * Provides a form for adding to BASE_dbxref and dbxref tables
 *
 * @param $form
 *   The Drupal form array into which the dbxref elements will be added
 * @param $form_state
 *   The corresponding form_state array for the form
 * @param $details
 *   An array defining details needed by this form. Required Keys are:
 *     - linking_table: the name of the dbxref linking table (ie:
 *   feature_dbxref)
 *     - base_foreign_key: the name of the foreign key linking this table to
 *   the non-dbxref table (ie: feature_id)
 *     - base_key_value: the value of the base_foreign_key for the current form
 *   (ie: 999 if the feature_id=999) Optional keys include:
 *     - fieldset_title: the non-translated title for this fieldset
 *     - additional_instructions: a non-translated string providing additional
 *   instructions
 *     - select_options: must be an array where the [key] is a valid db_id and
 *       the [value] is the human-readable name of the option. This includes
 *   all databases in the chado db table by default
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_add_node_form_dbxrefs(&$form, &$form_state, $details) {

  // Set defaults for optional fields
  $details['fieldset_title'] = 'External References';
  $details['additional_instructions'] = '';

  // Get the list of databases to display in the dropdown.
  if (isset($details['select_options'])) {
    // the callee has provided a list
    $db_options = $details['select_options'];
  }
  else {
    // get the list of databases from the db table
    $db_options = [0 => 'Select a Database'];
    $options = ['order_by' => ['name' => 'ASC']];
    $select = chado_select_record('db', ['db_id', 'name'], [], $options);
    foreach ($select as $db) {
      $db_options[$db->db_id] = $db->name;
    }
  }

  // Determine the node type using the name of the foreign key.
  $details['nodetype'] = str_replace('_id', '', $details['base_foreign_key']);

  // Tell tripal administrators how to add terms to the property types drop down.
  $importance = (empty($db_options)) ? TRIPAL_WARNING : TRIPAL_INFO;
  $tripal_message = tripal_set_message(
    t('To add databases to the drop down list, you need to <a href="@dblink">add
      an external database reference</a>.',
      [
        '@dblink' => url('admin/tripal/legacy/tripal_db/add'),
      ]
    ),
    $importance,
    ['return_html' => TRUE]
  );

  // Group all of the chado node api fieldsets into vertical tabs.
  $form['chado_node_api'] = [
    '#type' => 'vertical_tabs',
    '#attached' => [
      'css' => [
        'chado-node-api' => drupal_get_path('module', 'tripal_core') . '/theme/css/chado_node_api.css',
      ],
    ],
  ];

  // the fieldset of the dbxref elements
  $instructions = 'To add an external reference, select the database you want to reference from the
    drop-down below. Then enter the name/accession (as it is shown in the external database) of this
    particular %nodetype into the text box before clicking "Add". The version can be used to
    indicate the version of the external database or the version of the reference
    depending upon what is available. To remove incorrect references, click the
    "Remove" button. Note: you cannot edit previously added references but instead
    need to remove and re-add them.';
  $form['addtl_dbxrefs'] = [
    '#type' => 'fieldset',
    '#title' => t($details['fieldset_title']),
    '#description' => t('<p><strong>Indicate that this %nodetype either originates from
      or is present in another database.</strong></p><p>' . $instructions . $details['additional_instructions'] . '</p>',
      ['%nodetype' => $details['nodetype']]),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'chado_node_api',
    '#weight' => 9,
    '#attributes' => ['class' => ['chado-node-api', 'dbxrefs']],
    '#attached' => [
      'js' => [
        'chado-node-api-vertical-tabs' => drupal_get_path('module', 'tripal_core') . '/theme/js/chadoNodeApi_updateVerticalTabSummary.js',
      ],
    ],
  ];

  // this form element is a tree, so that we don't puke all of the values into then node variable
  // it is set as a tree, and keeps them in the $form_state['values']['dbxref_table'] heading.
  $form['addtl_dbxrefs']['dbxref_table'] = [
    '#type' => 'markup',
    '#tree' => TRUE,
    '#prefix' => '<div id="tripal-generic-edit-addtl_dbxrefs-table">',
    '#suffix' => '</div>',
    '#theme' => 'chado_node_additional_dbxrefs_form_table',
  ];

  // We need to provide feedback to the user that changes made
  // are not saved until the node is saved.
  $form['addtl_dbxrefs']['dbxref_table']['save_warning'] = [
    '#type' => 'markup',
    '#prefix' => '<div id="dbxref-save-warning" class="messages warning" style="display:none;">',
    '#suffix' => '</div>',
    '#markup' => '* The changes to these references will not be saved until the "Save" button at the bottom of this form is clicked. <span class="specific-changes"></span>',
    '#attached' => [
      'js' => [
        'chado-node-api-unsaved' => drupal_get_path('module', 'tripal_core') . '/theme/js/chadoNodeApi_unsavedNotify.js',
      ],
    ],
  ];

  /* DBxrefs can come to us in two ways:
   *
   * 1) In the form state in the $form_state['chado_additional_dbxrefs']. Data is in this field
   *    when an AJAX call updates the form state or a validation error.
   *
   * 2) Directly from the database if the record already has dbxref's associated.  This
   *    data is only used the first time the form is loaded. On AJAX calls or validation
   *    errors the fields on the form are populated from the $form_state['chado_additional_dbxrefs']
   *    entry.
   */
  if (isset($form_state['chado_additional_dbxrefs'])) {
    $existing_dbxrefs = $form_state['chado_additional_dbxrefs'];
  }
  else {
    $existing_dbxrefs = chado_query(
      "SELECT
         db.name as db_name,
         db.db_id as db_id,
         dbxref.dbxref_id,
         dbxref.accession as accession,
         dbxref.description as description,
         dbxref.version
       FROM {dbxref} dbxref
         LEFT JOIN {db} db ON db.db_id = dbxref.db_id
         LEFT JOIN {" . $details['linking_table'] . "} linking_table ON linking_table.dbxref_id = dbxref.dbxref_id
       WHERE linking_table." . $details['base_foreign_key'] . "= :base_key_value
       ORDER BY db.name ASC, dbxref.version ASC",
      [':base_key_value' => $details['base_key_value']]
    );
  }
  /* The format of the $existing_dbxref's array is either:
   *
   * From the chado_additional_dbxrefs array:
   * $form_state['chado_additional_dbxrefs'] = array(
   *   '[db_id]-[version]' => array(
   *     'db_id' => [the db.db_id value]
   *     'db_name' => [the db.name value]
   *     'dbxref_id' => [the dbxref.dbxref_id value, or temporary value if it doesn't yet exists],
   *     'version' => [the dbxref.version value],
   *     'accession' => [the dbxref.accession value],
   *   ),
   * );
   *
   * OR
   * Populated from the database:
   * $existing_dbxref = array(
   *   0 => array(
   *     'dbxref_id' => [the dbxref.dbxref_id value],
   *     'db_name' => [the db.name value],
   *     'db_id' => [the db.db_id value],
   *     'accession' => [the dbxref.accession value],
   *     'description' => [the dbxref.description value],
   *     'version' => [the dbxref.versiion value],
   *   ),
   * );
   *
   * NOTE: The main difference is the key
   *
   * Loop on the array elements of the $existing_dbxrefs array and add
   * an element to the form for each one.
   */
  $num_dbxrefs = 0;
  foreach ($existing_dbxrefs as $dbxref) {
    if (array_key_exists($dbxref->db_id, $db_options)) {
      $num_dbxrefs++;

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id]['#type'] = 'markup';
      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id]['#value'] = '';

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['#type'] = 'markup';
      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['#value'] = '';
      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['#attributes'] = [
        'class' => ['dbxref', 'saved'],
      ];

      // Determine whether this dbxref is unsaved or not.
      // We can tell this by looking at the dbxref_id: if it's not
      // saved yet we will have entered a TEMP###.
      if (preg_match('/^TEMP/', $dbxref->dbxref_id)) {
        $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['#attributes'] = [
          'class' => ['dbxref', 'unsaved'],
        ];
      }

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['db_id'] = [
        '#type' => 'hidden',
        '#value' => $dbxref->db_id,
      ];

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['accession'] = [
        '#type' => 'hidden',
        '#value' => $dbxref->accession,
      ];

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['version'] = [
        '#type' => 'hidden',
        '#value' => $dbxref->version,
      ];

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['dbxref_id'] = [
        '#type' => 'hidden',
        '#value' => $dbxref->dbxref_id,
      ];

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['db'] = [
        '#type' => 'markup',
        '#markup' => $dbxref->db_name,
        '#prefix' => '<span class="row-unsaved-warning"></span>',
      ];

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['dbxref_version'] = [
        '#type' => 'markup',
        '#markup' => $dbxref->version,
      ];

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['dbxref_accession'] = [
        '#type' => 'markup',
        '#markup' => $dbxref->accession,
      ];
      // remove button
      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$dbxref->dbxref_id]['dbxref_action'] = [
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => "dbxrefs_remove-" . $dbxref->db_id . '-' . $dbxref->dbxref_id,
        '#ajax' => [
          'callback' => "chado_add_node_form_subtable_ajax_update",
          'wrapper' => 'tripal-generic-edit-addtl_dbxrefs-table',
          'effect' => 'fade',
          'method' => 'replace',
          'prevent' => 'click',
        ],
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default node form submit.  In the validate function we validate only the
        // additional dbxref fields and in the submit we remove the indicated dbxref
        // from the chado_additional_dbxrefs array. In order to keep validate errors
        // from the node form validate and Drupal required errors for non-dbxref fields
        // preventing the user from removing dbxrefs we set the #limit_validation_errors below
        '#validate' => ['chado_add_node_form_subtables_remove_button_validate'],
        '#submit' => ['chado_add_node_form_subtables_remove_button_submit'],
        // Limit the validation of the form upon clicking this button to the dbxref_table tree
        // No other fields will be validated (ie: no fields from the main form or any other api
        // added form).
        '#limit_validation_errors' => [
          ['dbxref_table']
          // Validate all fields within $form_state['values']['dbxref_table']
        ],
      ];
    }
  }

  // Quickly add a hidden field stating how many dbxrefs are currently added.
  $form['addtl_dbxrefs']['num_dbxrefs'] = [
    '#type' => 'hidden',
    '#value' => $num_dbxrefs,
    '#attributes' => ['class' => 'num-dbxrefs'],
  ];

  // Form elements for adding a new dbxref
  //---------------------------------------------
  $form['addtl_dbxrefs']['dbxref_table']['new'] = [
    '#type' => 'markup',
    '#prefix' => '<span class="addtl-dbxrefs-add-new-dbxref">',
    '#suffix' => '</span>',
  ];

  // add in the existing databases
  $form['addtl_dbxrefs']['dbxref_table']['new']['db'] = [
    '#type' => 'select',
    '#options' => $db_options,
  ];

  $form['addtl_dbxrefs']['dbxref_table']['new']['dbxref_accession'] = [
    '#type' => 'textfield',
  ];

  $form['addtl_dbxrefs']['dbxref_table']['new']['dbxref_version'] = [
    '#type' => 'textfield',
    '#size' => 10,
  ];

  // add button
  $form['addtl_dbxrefs']['dbxref_table']['new']['dbxref_action'] = [
    '#type' => 'submit',
    '#value' => t('Add'),
    '#name' => "dbxrefs-add",
    '#ajax' => [
      'callback' => "chado_add_node_form_subtable_ajax_update",
      'wrapper' => 'tripal-generic-edit-addtl_dbxrefs-table',
      'effect' => 'fade',
      'method' => 'replace',
      'prevent' => 'click',
    ],
    // When this button is clicked, the form will be validated and submitted.
    // Therefore, we set custom submit and validate functions to override the
    // default node form submit.  In the validate function we validate only the
    // additional dbxref fields and in the submit we add them to the chado_additional_dbxrefs
    // array. In order to keep validate errors from the node form validate and Drupal
    // required errors for non-dbxref fields preventing the user from adding dbxrefs we
    // set the #limit_validation_errors below
    '#validate' => ['chado_add_node_form_subtables_add_button_validate'],
    '#submit' => ['chado_add_node_form_subtables_add_button_submit'],
    // Limit the validation of the form upon clicking this button to the dbxref_table tree
    // No other fields will be validated (ie: no fields from the main form or any other api
    // added form).
    '#limit_validation_errors' => [
      ['dbxref_table']
      // Validate all fields within $form_state['values']['dbxref_table']
    ],
  ];

  $form['addtl_dbxrefs']['admin_message'] = [
    '#type' => 'markup',
    '#markup' => $tripal_message,
  ];
}

/**
 * Validate the user input for creating a new dbxref
 * Called by the add button in chado_add_node_form_dbxrefs
 */
function chado_add_node_form_dbxrefs_add_button_validate($form, &$form_state) {

  // Ensure the db_id is supplied & Valid
  $db = chado_select_record(
    'db',
    ['db_id', 'name'],
    ['db_id' => $form_state['values']['dbxref_table']['new']['db']]
  );
  if (!isset($db[0])) {
    form_set_error('dbxref_table][new][db', 'Please select a database before attempting to add a new database reference.');
  }
  else {
    $form_state['values']['dbxref_table']['new']['db_name'] = $db[0]->name;
  }

  // Ensure accession is supplied
  if (empty($form_state['values']['dbxref_table']['new']['dbxref_accession'])) {
    form_set_error('dbxref_table][new][dbxref_accession', 'You must enter the accession before attempting to add a new database reference.');
  }

}

/**
 * Called by the add button in chado_add_node_form_dbxrefs
 *
 * Create an array of additional dbxrefs in the form state. This array will
 * then be used to rebuild the form in subsequent builds
 *
 */
function chado_add_node_form_dbxrefs_add_button_submit($form, &$form_state) {

  // if the chado_additional_dbxrefs array is not set then this is the first time modifying the
  // dbxref table. this means we need to include all the dbxrefs from the db
  if (!isset($form_state['chado_additional_dbxrefs'])) {
    chado_add_node_form_dbxrefs_create_dbxref_formstate_array($form, $form_state);
  }

  // get details for the new dbxref
  $dbxref = [
    'db_id' => $form_state['values']['dbxref_table']['new']['db'],
    'db_name' => $form_state['values']['dbxref_table']['new']['db_name'],
    'dbxref_id' => 'TEMP' . uniqid(),
    'version' => $form_state['values']['dbxref_table']['new']['dbxref_version'],
    'accession' => $form_state['values']['dbxref_table']['new']['dbxref_accession'],
  ];
  $key = $dbxref['db_id'] . '-' . $dbxref['dbxref_id'];
  $form_state['chado_additional_dbxrefs'][$key] = (object) $dbxref;


  // we don't want the new element to pick up the values from the previous element so wipe them out
  unset($form_state['input']['dbxref_table']['new']['db']);
  unset($form_state['input']['dbxref_table']['new']['db_name']);
  unset($form_state['input']['dbxref_table']['new']['dbxref_version']);
  unset($form_state['input']['dbxref_table']['new']['dbxref_accession']);
}

/**
 * Called by the many remove buttons in chado_add_node_form_dbxrefs
 *
 */
function chado_add_node_form_dbxrefs_remove_button_validate($form, $form_state) {
  // No validation needed.
}

/**
 * Remove the correct dbxref from the form
 * Called by the many remove buttons in chado_add_node_form_dbxrefs
 *
 */
function chado_add_node_form_dbxrefs_remove_button_submit(&$form, &$form_state) {

  // if the chado_additional_dbxrefs array is not set then this is the first time modifying the
  // dbxref table. this means we need to include all the dbxrefs from the db
  if (!isset($form_state['chado_additional_dbxrefs'])) {
    chado_add_node_form_dbxrefs_create_dbxref_formstate_array($form, $form_state);
  }

  // remove the specified dbxref from the form dbxref table
  if (preg_match('/dbxrefs_remove-([^-]+-[^-]+)/', $form_state['triggering_element']['#name'], $match)) {
    $key = $match[1];
    if (array_key_exists($key, $form_state['chado_additional_dbxrefs'])) {
      unset($form_state['chado_additional_dbxrefs'][$key]);
    }
  }
}

/**
 * Creates an array in form_state containing the existing addtl_dbxrefs. This
 * array is then modified by the add/remove buttons and used as a source for
 * rebuilding the form. This function get's called at each button (add and
 * remove) button submits the first time one of the button's is clicked to
 * instantiates the $form_state['chado_additional_dbxrefs'] array
 *
 * $form_state['chado_additional_dbxrefs'] = array(
 *   '[db_id]-[version]' => array(
 *     'db_id' => [the db.db_id value]
 *     'db_name' => [the db.name value]
 *     'dbxref_id' => [the dbxref.dbxref_id value, or temporary value if it
 * doesn't yet exists],
 *     'version' => [the dbxref.version value],
 *     'accession' => [the dbxref.accession value],
 *   ),
 * );
 *
 */
function chado_add_node_form_dbxrefs_create_dbxref_formstate_array($form, &$form_state) {

  $form_state['chado_additional_dbxrefs'] = [];

  foreach (element_children($form['addtl_dbxrefs']['dbxref_table']) as $db_id) {
    if ($db_id != 'new') {
      foreach (element_children($form['addtl_dbxrefs']['dbxref_table'][$db_id]) as $version) {
        $element = $form['addtl_dbxrefs']['dbxref_table'][$db_id][$version];
        $dbxref = [
          'db_id' => $element['db_id']['#value'],
          'db_name' => $element['db']['#markup'],
          'dbxref_id' => $element['dbxref_id']['#value'],
          'version' => $element['dbxref_version']['#markup'],
          'accession' => $element['dbxref_accession']['#markup'],
        ];
        $key = $dbxref['db_id'] . '-' . $dbxref['dbxref_id'];
        $form_state['chado_additional_dbxrefs'][$key] = (object) $dbxref;
      }
    }
  }
}

/**
 * Function to theme the add/remove dbxrefs form into a table
 *
 */
function theme_chado_add_node_form_dbxrefs_table($variables) {
  $element = $variables['element'];

  $header = [
    'db' => t('Database'),
    'dbxref_accession' => t('Accession'),
    'dbxref_version' => t('Version'),
    'dbxref_action' => t('Actions'),
  ];

  $rows = [];
  foreach (element_children($element) as $db_id) {
    if ($db_id == 'new') {
      $row = [];

      $row['data'] = [];
      foreach ($header as $fieldname => $title) {
        $row['data'][] = drupal_render($element[$db_id][$fieldname]);
      }
      $rows[] = $row;
    }
    else {
      foreach (element_children($element[$db_id]) as $version) {
        $row = [];

        $row['data'] = [];
        $row['class'] = $element[$db_id][$version]['#attributes']['class'];
        foreach ($header as $fieldname => $title) {
          $row['data'][] = drupal_render($element[$db_id][$version][$fieldname]);
        }
        $rows[] = $row;
      }
    }
  }

  return render($element['save_warning']) . theme('table', [
      'header' => $header,
      'rows' => $rows,
    ]);
}

/**
 * This function is used in a hook_insert, hook_update for a node form
 * when the additional_dbxrefs form has been added to the form.  It retrieves
 * all of the dbxrefs and returns them in an array of the format:
 *
 *   $dbxefs[<db_id>][<version>][<dbxref_id>] = <accession>
 *
 * This array can then be used for inserting or updating dbxrefs using the API
 * call tripal_hook_insert_dbxref()
 *
 * @param $node
 *
 * @return
 *   A dbxref array
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_retrieve_node_form_dbxrefs($node) {
  $dbxrefs = [];

  if (isset($node->dbxref_table)) {
    foreach ($node->dbxref_table as $db_id => $elements) {
      if ($db_id != 'new') {
        foreach ($elements as $dbxref_id => $dbxref) {
          $version = (!empty($dbxref['version'])) ? $dbxref['version'] : 'NONE';
          $dbxrefs[$db_id][$version][$dbxref_id] = $dbxref['accession'];
        }
      }
    }
  }

  return $dbxrefs;
}

/**
 * This function is used in hook_insert or hook_update and handles inserting of
 * any new dbxrefs and creation of links between those dbxrefs and node content
 *
 * @param $node
 *    The node passed into hook_insert & hook_update
 * @param $details
 *   - linking_table: the name of the _dbxref linking table (ie:
 *   feature_dbxref)
 *   - foreignkey_name: the name of the foreign key used to link to the node
 *   content (ie: feature_id)
 *   - foreignkey_value: the value of the foreign key (ie: 445, if there exists
 *   a feature where feature_id=445)
 * @param $retrieved_dbxrefs
 *   An array of databa references from
 *   chado_retrieve_node_form_dbxrefs($node).
 *   This can be used if you need special handling for some of the database
 *   references
 *
 * @ingroup tripal_legacy_chado_node_api
 */
function chado_update_node_form_dbxrefs($node, $details, $retrieved_dbxrefs = FALSE) {

  $linking_table = $details['linking_table'];
  $foreignkey_name = $details['foreignkey_name'];
  $foreignkey_value = $details['foreignkey_value'];

  if (isset($node->dbxref_table) AND ($foreignkey_value > 0)) {
    // First remove existing dbxref links
    chado_delete_record($linking_table, [$foreignkey_name => $foreignkey_value]);

    // Add back in dbxref links and insert dbxrefs as needed
    if ($retrieved_dbxrefs) {
      $dbxrefs = $retrieved_dbxrefs;
    }
    else {
      $dbxrefs = chado_retrieve_node_form_dbxrefs($node);
    }
    foreach ($dbxrefs as $db_id => $versions) {
      foreach ($versions as $version => $elements) {
        foreach ($elements as $dbxref_id => $accession) {
          // If there is no dbxref then we have to create that first
          if (preg_match('/^TEMP/', $dbxref_id)) {
            $version = ($version == 'NONE') ? '' : $version;
            $success = tripal_insert_dbxref([
              'db_id' => $db_id,
              'accession' => $accession,
              'version' => $version,
              'description' => NULL,
            ]);
            if ($success) {
              $dbxref_id = $success->dbxref_id;
            }
            else {
              $dbxref_id = FALSE;
            }
          }

          // add _dbxref linker
          if ($dbxref_id) {
            if (preg_match('/(\w+)_dbxref/', $linking_table, $matches)) {
              $base_table = $matches[1];

              $success_link = tripal_associate_dbxref(
                $base_table,
                $foreignkey_value,
                ['dbxref_id' => $dbxref_id]
              );
            }
          }
        }
      }
    }
  }
}