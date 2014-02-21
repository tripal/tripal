<?php

/**
 * @file
 * API to manage the chado _dbxref table for various Tripal Node Types
 *
 * How To Use:
 * @code

  function chado_example_form($form, $form_state) {

    // Default values for form elements can come in the following ways:
    //
    // 1) as elements of the $node object.  This occurs when editing an existing node
    // 2) in the $form_state['values'] array which occurs on a failed validation or
    //    ajax callbacks when the ajax call originates from non-submit fields other
    //    than button
    // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
    //    form elements (e.g. buttons) and the form is being rebuilt but has not yet
    //    been validated
    //
    // The reference elements added by this function do use AJAX calls from buttons,
    // therefore, it is important to check for form values in the $form_state['values']
    // for case #2 above, and in the $form_state['input'] for case #3.
    // See the chado analysis node form for an example.


    // Next, add in all the form array definition particular to your node type

    // To add in the additional database reference form elements, you first need to prepare the arguments
    // for the function call.

    $details = array(
      'linking_table' => 'example_dbxref',         // the name of the table linking additional dbxrefs to this node
      'base_foreign_key' => 'example_id',          // key to link to the chado content created by this node
      'base_key_value' => $example_id,             // the value of the above key
      'fieldset_title' => 'Additional References', // the non-translated title for this fieldset
      'additional_instructions' => ''              // a non-stranslated string providing additional instructions
    );

    // Finally, and add the additional form elements to the form
    chado_add_node_form_dbxrefs($form, $form_state, $details);

    return $form;
  }

  function chado_example_insert($node) {

    // if there is an example_id in the $node object then this must be a sync so
    // we can skip adding the chado_example as it is already there, although
    // we do need to proceed with the rest of the insert
    if (!property_exists($node, 'example_id')) {

      // Add record to chado example table

      // Add to any other tables needed

      // Add all additional database references
      // This function will create new database references as needed and select existing ones.
      // Existing _dbxref links will be cleared and then re-added
      chado_update_node_form_dbxrefs(
        $node,              // the node object passed in via hook_insert()
        'example_dbxref',   // the name of the _dbxref linking table
        'example_id',       // key to link to the chado content created by this node
        $node->example_id   // value of the above key
      );
    }

    // Add record to chado_example linking example_id to new node

  }

  function chado_example_update($node) {


      // Update record in chado example table

      // Update any other tables needed

      // Update all additional database references
      // This function will create new database references as needed and select existing ones.
      // Existing _dbxref links will be cleared and then re-added
      chado_update_node_form_dbxrefs(
        $node,              // the node object passed in via hook_insert()
        'example_dbxref',   // the name of the _dbxref linking table
        'example_id',       // key to link to the chado content created by this node
        $node->example_id   // value of the above key
      );

    // Don't need to update chado_example linking table since niether example_id or nid can be changed in update

  }

 * @endcode
 *
 * @ingroup tripal_chado_node_api
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
 *     - linking_table: the name of the dbxref linking table (ie: feature_dbxref)
 *     - base_foreign_key: the name of the foreign key linking this table to the non-dbxref table (ie: feature_id)
 *     - base_key_value: the value of the base_foreign_key for the current form (ie: 999 if the feature_id=999)
 *   Optional keys include:
 *     - fieldset_title: the non-translated title for this fieldset
 *     - additional_instructions: a non-translated string providing additional instructions
 *     - select_options: must be an array where the [key] is a valid db_id and
 *       the [value] is the human-readable name of the option. This includes all databases
 *       in the chado db table by default
 *
 * @ingroup tripal_chado_node_api
 */
function chado_add_node_form_dbxrefs(&$form, &$form_state, $details) {

  // Set defaults for optional fields
  $details['fieldset_title'] = 'Additional Database References';
  $details['additional_instructions'] = '';

  // Get the list of databases to display in the dropdown. 
  if (isset($details['select_options'])) {
    // the callee has provided a list
    $db_options = $details['select_options'];
  }
  else {
    // get the list of databases from the db table
    $db_options = array(0 => 'Select a Database');
    $options = array('order_by' => array('name' => 'ASC'));
    $select = chado_select_record('db', array('db_id','name'), array(), $options);
    foreach($select as $db) {
      $db_options[$db->db_id] = $db->name;
    }
  }

  // Tell tripal administrators how to add terms to the property types drop down.
  $importance = (empty($db_options)) ? TRIPAL_WARNING : TRIPAL_INFO;
  $tripal_message = tripal_set_message(
    t('To add databases to the drop down list, you need to <a href="@dblink">add
      an external database reference</a>.',
      array(
        '@dblink' => url('admin/tripal/chado/tripal_db/add')
      )
    ),
    $importance,
    array('return_html' => TRUE)
  );

  // the fieldset of the dbxref elements
  $form['addtl_dbxrefs'] = array(
    '#type' => 'fieldset',
    '#title' => t($details['fieldset_title']),
    '#description' => t('You may add additional database references by
      selecting a database reference type from the dropdown and adding text.  You may add
      as many database references as desired by clicking the add button on the right.  To
      remove a database reference, click the remove button. ' . $details['additional_instructions']),
    '#prefix' => "<div id='addtl-dbxrefs-fieldset'>",
    '#suffix' => '</div>',
    '#weight'      => 9
  );

  $form['addtl_dbxrefs']['admin_message'] = array(
    '#type' => 'markup',
    '#markup' => $tripal_message
  );

  // this form element is a tree, so that we don't puke all of the values into then node variable
  // it is set as a tree, and keeps them in the $form_state['values']['dbxref_table'] heading.
  $form['addtl_dbxrefs']['dbxref_table'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#prefix' => '<div id="tripal-generic-edit-addtl_dbxrefs-table">',
    '#suffix' => '</div>',
    '#theme' => 'chado_node_additional_dbxrefs_form_table'
  );

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
         db.name as db_name, db.db_id as db_id,
         dbxref.dbxref_id, dbxref.accession as accession, 
         dbxref.description as description, dbxref.version
       FROM {dbxref} dbxref
         LEFT JOIN {db} db ON db.db_id = dbxref.db_id
         LEFT JOIN {".$details['linking_table']."} linking_table ON linking_table.dbxref_id = dbxref.dbxref_id
       WHERE linking_table.".$details['base_foreign_key']."= :base_key_value
       ORDER BY db.name ASC, dbxref.version ASC",
       array(':base_key_value' => $details['base_key_value'])
    );
  }
  /* The format of the $existing_dbxref's array is either:
   *
   * From the chado_additional_dbxrefs array:
   * $form_state['chado_additional_dbxrefs'] = array(
   *   '[db_id]-[version]' => array(
   *     'db_id' => [the db.db_id value]
   *     'db_name' => [the db.name value]
   *     'dbxref_id' => [the dbxref.dbxref_id value, or NULL if it doesn't yet exists],
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
  foreach ($existing_dbxrefs as $dbxref) {
    
    if (array_key_exists($dbxref->db_id, $db_options)) {

      /* Since the dbxref version is part of the unique constraint, when it is 
       * empty we need to use something in the key to indicate this case. Otherwise, 
       * you wouldn't be able to select those elements from the array 
       * (ie: $form['addtl_dbxrefs']['dbxref_table'][9999][''] doesn't work as 
       * expected whereas $form['addtl_dbxrefs']['dbxref_table'][9999][NONE] 
       * is much better)
       */
      $version = (!empty($dbxref->version)) ? $dbxref->version : 'NONE';

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id] = array(
        '#type' => 'markup',
        '#value' => ''
      );

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version] = array(
        '#type' => 'markup',
        '#value' => ''
      );

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version]['db_id'] = array(
        '#type' => 'hidden',
        '#value' => $dbxref->db_id
      );

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version]['accession'] = array(
        '#type' => 'hidden',
        '#value' => $dbxref->accession
      );

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version]['dbxref_id'] = array(
        '#type' => 'hidden',
        '#value' => $dbxref->dbxref_id
      );

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version]['db'] = array(
        '#type' => 'markup',
        '#markup' => $dbxref->db_name
      );

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version]['dbxref_version'] = array(
        '#type' => 'markup',
        '#markup' => $dbxref->version,
      );

      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version]['dbxref_accession'] = array(
        '#type' => 'markup',
        '#markup' => $dbxref->accession
      );
      // remove button
      $form['addtl_dbxrefs']['dbxref_table'][$dbxref->db_id][$version]['dbxref_action'] = array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => "dbxref_remove-".$dbxref->db_id.'-'.$version,
        '#ajax' => array(
          'callback' => "chado_add_node_form_dbxrefs_ajax_update",
          'wrapper' => 'tripal-generic-edit-addtl_dbxrefs-table',
          'effect'   => 'fade',
          'method'   => 'replace',
          'prevent'  => 'click'
        ),
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default node form submit.  In the validate function we validate only the
        // additional dbxref fields and in the submit we remove the indicated dbxref
        // from the chado_additional_dbxrefs array. In order to keep validate errors
        // from the node form validate and Drupal required errors for non-dbxref fields
        // preventing the user from removing dbxrefs we set the #limit_validation_errors below
        '#validate' => array('chado_add_node_form_dbxrefs_remove_button_validate'),
        '#submit' => array('chado_add_node_form_dbxrefs_remove_button_submit'),
        // Limit the validation of the form upon clicking this button to the dbxref_table tree
        // No other fields will be validated (ie: no fields from the main form or any other api
        // added form).
        '#limit_validation_errors' => array(
          array('dbxref_table')  // Validate all fields within $form_state['values']['dbxref_table']
        )
      );
    }
  }

  // Form elements for adding a new dbxref
  //---------------------------------------------
  $form['addtl_dbxrefs']['dbxref_table']['new'] = array(
    '#type' => 'markup',
    '#prefix' => '<span class="addtl-dbxrefs-add-new-dbxref">',
    '#suffix' => '</span>'
  );

  // add in the existing databases
  $form['addtl_dbxrefs']['dbxref_table']['new']['db'] = array(
    '#type' => 'select',
    '#options' => $db_options,
  );

  $form['addtl_dbxrefs']['dbxref_table']['new']['dbxref_accession'] = array(
    '#type' => 'textfield',
  );

  $form['addtl_dbxrefs']['dbxref_table']['new']['dbxref_version'] = array(
    '#type' => 'textfield',
    '#size' => 10,
  );

  // add button
  $form['addtl_dbxrefs']['dbxref_table']['new']['dbxref_action'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#name' => "dbxref-add",
    '#ajax' => array(
      'callback' => "chado_add_node_form_dbxrefs_ajax_update",
      'wrapper' => 'tripal-generic-edit-addtl_dbxrefs-table',
      'effect'   => 'fade',
      'method'   => 'replace',
      'prevent'  => 'click'
    ),
    // When this button is clicked, the form will be validated and submitted.
    // Therefore, we set custom submit and validate functions to override the
    // default node form submit.  In the validate function we validate only the
    // additional dbxref fields and in the submit we add them to the chado_additional_dbxrefs
    // array. In order to keep validate errors from the node form validate and Drupal
    // required errors for non-dbxref fields preventing the user from adding dbxrefs we
    // set the #limit_validation_errors below
    '#validate' => array('chado_add_node_form_dbxrefs_add_button_validate'),
    '#submit' => array('chado_add_node_form_dbxrefs_add_button_submit'),
    // Limit the validation of the form upon clicking this button to the dbxref_table tree
    // No other fields will be validated (ie: no fields from the main form or any other api
    // added form).
    '#limit_validation_errors' => array(
      array('dbxref_table')  // Validate all fields within $form_state['values']['dbxref_table']
    )
  );

}

/**
 * Validate the user input for creating a new dbxref
 * Called by the add button in chado_add_node_form_dbxrefs
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_dbxrefs_add_button_validate($form, &$form_state) {

  // Ensure the db_id is supplied & Valid
  $db = chado_select_record(
    'db',
    array('db_id', 'name'),
    array('db_id' => $form_state['values']['dbxref_table']['new']['db'])
  );
  if (!isset($db[0])) {
    form_set_error('dbxref_table][new][db', 'Please select a database before attempting to add a new database reference.');
  }
  else {
    $form_state['values']['dbxref_table']['new']['db_name'] = $db[0]->name;
  }

  // Ensure accession is supplied
  if (empty($form_state['values']['dbxref_table']['new']['dbxref_accession'])) {
    form_set_error('dbxref_table][new][dbxref_accession','You must enter the accession before attempting to add a new database reference.');
  }
}

/**
 * Called by the add button in chado_add_node_form_dbxrefs
 *
 * Create an array of additional dbxrefs in the form state. This array will then be
 * used to rebuild the form in subsequent builds
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_dbxrefs_add_button_submit(&$form, &$form_state) {

  // if the chado_additional_dbxrefs array is not set then this is the first time modifying the
  // dbxref table. this means we need to include all the dbxrefs from the db
  if (!isset($form_state['chado_additional_dbxrefs'])) {
    chado_add_node_form_dbxrefs_create_dbxref_formstate_array($form, $form_state);
  }

  // get details for the new dbxref
  $dbxref = array(
    'db_id' => $form_state['values']['dbxref_table']['new']['db'],
    'db_name' => $form_state['values']['dbxref_table']['new']['db_name'],
    'dbxref_id' => NULL,
    'version' => $form_state['values']['dbxref_table']['new']['dbxref_version'],
    'accession' => $form_state['values']['dbxref_table']['new']['dbxref_accession'],
  );
  $version = (!empty($dbxref['version'])) ? $dbxref['version'] : 'NONE';
  $key = $dbxref['db_id'] . '-' . $version;
  $form_state['chado_additional_dbxrefs'][$key] = (object) $dbxref;

  
  // we don't want the new element to pick up the values from the previous element so wipe them out
  unset($form_state['input']['dbxref_table']['new']['db']);
  unset($form_state['input']['dbxref_table']['new']['db_name']);
  unset($form_state['input']['dbxref_table']['new']['dbxref_version']);
  unset($form_state['input']['dbxref_table']['new']['dbxref_accession']);
  
  $form_state['rebuild'] = TRUE;
}

/**
 * There is no user input for the remove buttons so there is no need to validate
 * However, both a submit & validate need to be specified so this is just a placeholder
 *
 * Called by the many remove buttons in chado_add_node_form_dbxrefs
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_dbxrefs_remove_button_validate($form, $form_state) {
  // No Validation needed for remove
}

/**
 * Remove the correct dbxref from the form
 * Called by the many remove buttons in chado_add_node_form_dbxrefs
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_dbxrefs_remove_button_submit(&$form, &$form_state) {

  // if the chado_additional_dbxrefs array is not set then this is the first time modifying the
  // dbxref table. this means we need to include all the dbxrefs from the db
  if (!isset($form_state['chado_additional_dbxrefs'])) {
    chado_add_node_form_dbxrefs_create_dbxref_formstate_array($form, $form_state);
  }

  // remove the specified dbxref from the form dbxref table
  if(preg_match('/dbxref_remove-([^-]+-[^-]+)/',$form_state['triggering_element']['#name'],$match)) {
    $key = $match[1];
    if (array_key_exists($key, $form_state['chado_additional_dbxrefs'])) {
      unset($form_state['chado_additional_dbxrefs'][$key]);
    }
  }

  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax function which returns the section of the form to be re-rendered
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_dbxrefs_ajax_update($form, $form_state) {
  return $form['addtl_dbxrefs']['dbxref_table'];
}

/**
 * Creates an array in form_state containing the existing addtl_dbxrefs. This array is
 * then modified by the add/remove buttons and used as a source for rebuilding the form.
 * This function get's called at each button (add and remove) button submits the first
 * time one of the button's is clicked to instantiates the $form_state['chado_additional_dbxrefs'] array
 *
 * $form_state['chado_additional_dbxrefs'] = array(
 *   '[db_id]-[version]' => array(
 *     'db_id' => [the db.db_id value]
 *     'db_name' => [the db.name value]
 *     'dbxref_id' => [the dbxref.dbxref_id value, or NULL if it doesn't yet exists],
 *     'version' => [the dbxref.version value],
 *     'accession' => [the dbxref.accession value],
 *   ),
 * );
 *
 * @ingroup tripal_core
 */
function chado_add_node_form_dbxrefs_create_dbxref_formstate_array($form, &$form_state) {

  $form_state['chado_additional_dbxrefs'] = array();

  foreach (element_children($form['addtl_dbxrefs']['dbxref_table']) as $db_id) {
    if ($db_id != 'new') {
      foreach (element_children($form['addtl_dbxrefs']['dbxref_table'][$db_id]) as $version) {
          $element = $form['addtl_dbxrefs']['dbxref_table'][$db_id][$version];
          $dbxref = array(
            'db_id' => $element['db_id']['#value'],
            'db_name' => $element['db']['#markup'],
            'dbxref_id' => $element['dbxref_id']['#value'],
            'version' => $element['dbxref_version']['#markup'],
            'accession' => $element['dbxref_accession']['#markup'],
          );
          $version = (!empty($dbxref['version'])) ? $dbxref['version'] : 'NONE';
          $key = $dbxref['db_id'] . '-' . $version;
          $form_state['chado_additional_dbxrefs'][$key] = (object) $dbxref;
      }
    }
  }
}

/**
 * Function to theme the add/remove dbxrefs form into a table
 *
 * @ingroup tripal_chado_node_api
 */
function theme_chado_add_node_form_dbxrefs_table($variables) {
  $element = $variables['element'];

  $header = array(
    'db' => t('Database'),
    'dbxref_accession' => t('Accession'),
    'dbxref_version' => t('Version'),
    'dbxref_action' => t('Actions'),
  );

  $rows = array();
  foreach (element_children($element) as $db_id) {
    if ($db_id == 'new') {
      $row = array();

        $row['data'] = array();
        foreach ($header as $fieldname => $title) {
          $row['data'][] = drupal_render($element[$db_id][$fieldname]);
        }
        $rows[] = $row;
    }
    else {
      foreach (element_children($element[$db_id]) as $version) {
        $row = array();

        $row['data'] = array();
        foreach ($header as $fieldname => $title) {
          $row['data'][] = drupal_render($element[$db_id][$version][$fieldname]);
        }
        $rows[] = $row;
      }
    }
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows
  ));
}

/**
 * This function is used in a hook_insert, hook_update for a node form
 * when the additional_dbxrefs form has been added to the form.  It retrieves all of the dbxrefs
 * and returns them in an array of the format:
 *
 *   $dbxefs[<db_id>][<version>][<dbxref_id>] = <accession>
 *
 * This array can then be used for inserting or updating dbxrefs using the API call
 * tripal_hook_insert_dbxref()
 *
 * @param $node
 *
 * @return
 *   A dbxref array
 *
 * @ingroup tripal_chado_node_api
 */
function chado_retrieve_node_form_dbxrefs($node) {
  $dbxrefs = array();

  if (isset($node->dbxref_table)) {
    foreach ($node->dbxref_table as $db_id => $elements) {
      if ($db_id != 'new') {
        foreach ($elements as $version => $dbxref) {
          $dbxref_id = (!empty($dbxref['dbxref_id'])) ? $dbxref['dbxref_id'] : 'NONE';
          $dbxrefs[$db_id][$version][$dbxref_id] = $dbxref['accession'];
        }
      }
    }
  }

  return $dbxrefs;
}

/**
 * This function is used in hook_insert or hook_update and handles inserting of any new
 * dbxrefs and creation of links between those dbxrefs and node content
 *
 * @param $node
 *    The node passed into hook_insert & hook_update
 * @param $details
 *   - linking_table: the name of the _dbxref linking table (ie: feature_dbxref)
 *   - foreignkey_name: the name of the foreign key used to link to the node content (ie: feature_id)
 *   - foreignkey_value: the value of the foreign key (ie: 445, if there exists a feature where feature_id=445)
 * @param $retrieved_dbxrefs
 *   An array of databa references from chado_retrieve_node_form_dbxrefs($node).
 *   This can be used if you need special handling for some of the database references
 *
 * @ingroup tripal_chado_node_api
 */
function chado_update_node_form_dbxrefs($node, $details, $retrieved_dbxrefs = FALSE) {

  $linking_table = $details['linking_table'];
  $foreignkey_name = $details['foreignkey_name'];
  $foreignkey_value = $details['foreignkey_value'];

  if (isset($node->dbxref_table) AND ($foreignkey_value > 0)) {
    // First remove existing dbxref links
    chado_delete_record($linking_table, array($foreignkey_name => $foreignkey_value));

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
          if ($dbxref_id == 'NONE') {
            $version = ($version == 'NONE') ? '' : $version;
            $success = tripal_db_add_dbxref(
              $db_id,
              $accession,
              $version,
              NULL
            );
            if ($success) {
              $dbxref_id = $success->dbxref_id;
            }
            else {
              $dbxref_id = FALSE;
            }
          }

          // add _dbxref linker
          if ($dbxref_id) {
            $success_link = tripal_db_add_dbxref_link(
              $linking_table,
              $dbxref_id,
              $foreignkey_name,
              $foreignkey_value
            );
          }
        }
      }
    }
  }
}