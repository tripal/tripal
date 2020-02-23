<?php
/**
 * @file
 * Implements the library node content type
 */

/**
 * Implements hook_node_info().
 *
 * Provide information to drupal about the node types that we're creating
 * in this module
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_node_info() {
  $nodes = [];
  $nodes['chado_library'] = [
    'name' => t('Library (Tripal v2 legacy)'),
    'base' => 'chado_library',
    'description' => t('A library from the chado database'),
    'has_title' => TRUE,
    'locked' => TRUE,
    'chado_node_api' => [
      'base_table' => 'library',
      'hook_prefix' => 'chado_library',
      'record_type_title' => [
        'singular' => t('Library'),
        'plural' => t('Libraries'),
      ],
      'sync_filters' => [
        'type_id' => TRUE,
        'organism_id' => TRUE,
      ],
    ],
  ];
  return $nodes;
}

/**
 *  Implements hook_form().
 *
 * When editing or creating a new node of type 'chado_library' we need
 *  a form.  This function creates the form that will be used for this.
 *
 * @ingroup tripal_legacy_library
 */
function chado_library_form($node, &$form_state) {
  $form = [];

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing library
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $library_id = NULL;
  $libraryname = '';
  $uniquename = '';
  $library_type = '';
  $organism_id = '';
  $description = '';

  // if we are editing an existing node then the library is already part of the node
  if (property_exists($node, 'library')) {
    $library = $node->library;
    $library_id = $library->library_id;

    $libraryname = $library->name;
    $uniquename = $library->uniquename;
    $library_type = $library->type_id->cvterm_id;
    $organism_id = $library->organism_id->organism_id;

    $libprop = chado_get_property(
      ['table' => 'library', 'id' => $library->library_id],
      ['type_name' => 'Library Description', 'cv_name' => 'library_property']
    );
    $description = $libprop->value;

    // keep track of the library id if we have.  If we do have one then
    // this is an update as opposed to an insert.
    $form['library_id'] = [
      '#type' => 'value',
      '#value' => $library_id,
    ];
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $libraryname = $form_state['values']['libraryname'];
    $uniquename = $form_state['values']['uniquename'];
    $library_type = $form_state['values']['library_type'];
    $organism_id = $form_state['values']['organism_id'];
    $description = $form_state['values']['description'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $libraryname = $form_state['input']['libraryname'];
    $uniquename = $form_state['input']['uniquename'];
    $library_type = $form_state['input']['library_type'];
    $organism_id = $form_state['input']['organism_id'];
    $description = $form_state['input']['description'];
  }

  $form['libraryname'] = [
    '#type' => 'textfield',
    '#title' => t('Library Name'),
    '#description' => t('Please enter the name for this library. Library names should be recognizable but do not need to be unique.'),
    '#required' => TRUE,
    '#default_value' => $libraryname,
  ];

  $form['uniquename'] = [
    '#type' => 'textfield',
    '#title' => t('Unique Name'),
    '#description' => t('Please enter a unique name for this library. This can be any value used to uniquely identify a library.'),
    '#required' => TRUE,
    '#default_value' => $uniquename,
  ];

  // get the list of library types
  $lt_cv = tripal_get_default_cv("library", "type_id");
  $types = tripal_get_cvterm_default_select_options('library', 'type_id', 'library types');
  $types[0] = 'Select a Type';
  $lt_message = tripal_set_message("To add additional items to the library type drop down list,
     add a term to the " .
    l($lt_cv->name . " controlled vocabulary",
      "admin/tripal/loaders/chado_vocabs/chado_cv/" . $lt_cv->cv_id . "/cvterm/add",
      ['attributes' => ['target' => '_blank']]
    ),
    TRIPAL_INFO, ['return_html' => TRUE]
  );

  $form['library_type'] = [
    '#title' => t('Library Type'),
    '#type' => t('select'),
    '#description' => t("Choose the library type."),
    '#required' => TRUE,
    '#default_value' => $library_type,
    '#options' => $types,
    '#suffix' => $lt_message,
  ];

  // get the list of organisms
  $sql = "SELECT * FROM {organism}";
  $org_rset = chado_query($sql);

  $organisms = [];
  $organisms[''] = '';
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] =
      "$organism->genus $organism->species ($organism->common_name)";
  }

  $form['organism_id'] = [
    '#title' => t('Organism'),
    '#type' => t('select'),
    '#description' => t("Choose the organism with which this library is associated."),
    '#required' => TRUE,
    '#default_value' => $organism_id,
    '#options' => $organisms,
  ];

  $form['description'] = [
    '#type' => 'text_format',
    '#title' => t('Library Description'),
    '#description' => t('A brief description of the library'),
    '#required' => TRUE,
    '#default_value' => $description,
  ];

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('libraryprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;

  $details = [
    // The name of the prop table.
    'property_table' => 'libraryprop',
    // The value of library_id for this record.
    'chado_id' => $library_id,
    // The cv.cv_id of the cv governing libraryprop.type_id.
    'cv_id' => $cv_id,
  ];

  // If the default is the 'library_property' vocabulary then we want
  // to exclude the 'Library Description' term since it has it's own form
  // element above
  if ($prop_cv->name == 'library_property') {
    // Generate our own select list so we can exclude the description element
    $select_options = [];
    $cv_result = chado_select_record('cv', ['cv_id'], ['name' => 'library_property']);
    $cv_id = $cv_result[0]->cv_id;
    $select_options = tripal_get_cvterm_select_options($cv_id);
    $descrip_id = array_search('Library Description', $select_options);
    unset($select_options[$descrip_id]);
    $details['select_options'] = $select_options;
  }

  // Adds the form elements to your current form
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = [
    // The name of the _dbxref table.
    'linking_table' => 'library_dbxref',
    // The name of the key in your base chado table.
    'base_foreign_key' => 'library_id',
    // The value of library_id for this record.
    'base_key_value' => $library_id,
  ];
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  return $form;
}

/**
 * Implements hook_validate().
 *
 * Validates submission of form when adding or updating a library node
 *
 * @ingroup tripal_legacy_library
 */
function chado_library_validate($node, $form, &$form_state) {

  // We only want to validate when the node is saved.
  // Since this validate can be called on AJAX and Deletion of the node
  // we need to make this check to ensure queries are not executed
  // without the proper values.
  if (property_exists($node, "op") and $node->op != 'Save') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a featuremap_id. We don't
  // need to validate during syncing so just skip it.
  if (!property_exists($node, 'nid') and property_exists($node, 'library_id') and $node->library_id != 0) {
    return;
  }

  // trim white space from text fields
  $node->libraryname = property_exists($node, 'libraryname') ? trim($node->libraryname) : '';
  $node->uniquename = property_exists($node, 'uniquename') ? trim($node->uniquename) : '';

  $lib = 0;
  // check to make sure the unique name on the library is unique
  // before we try to insert into chado.
  if (property_exists($node, 'library_id')) {
    $sql = "
      SELECT *
      FROM {library}
      WHERE uniquename = :uname AND NOT library_id = :library_id
    ";
    $lib = chado_query($sql, [
      ':uname' => $node->uniquename,
      ':library_id' => $node->library_id,
    ])->fetchObject();
  }
  else {
    $sql = "SELECT * FROM {library} WHERE uniquename = :uname";
    $lib = chado_query($sql, [':uname' => $node->uniquename])->fetchObject();
  }
  if ($lib) {
    form_set_error('uniquename', t('The unique library name already exists. Please choose another'));
  }
}

/**
 *  Implements hook_insert().
 *
 *  When a new chado_library node is created we also need to add information
 *  to our chado_library table.  This function is called on insert of a new node
 *  of type 'chado_library' and inserts the necessary information.
 *
 * @ingroup tripal_legacy_library
 */
function chado_library_insert($node) {

  $library_id = '';

  // if there is an library_id in the $node object then this must be a sync so
  // we can skip adding the library as it is already there, although
  // we do need to proceed with insertion into the chado/drupal linking table.
  if (!property_exists($node, 'library_id')) {
    $node->libraryname = trim($node->libraryname);
    $node->uniquename = trim($node->uniquename);
    $node->description = trim($node->description['value']);

    $values = [
      'name' => $node->libraryname,
      'uniquename' => $node->uniquename,
      'organism_id' => $node->organism_id,
      'type_id' => $node->library_type,
    ];
    $library = chado_insert_record('library', $values);
    if (!$library) {
      drupal_set_message(t('Unable to add library.', 'warning'));
      watchdog('tripal_library', 'Insert library: Unable to create library where values: %values',
        ['%values' => print_r($values, TRUE)], WATCHDOG_ERROR);
      return;
    }
    $library_id = $library['library_id'];

    // * Properties Form *
    // add the description property
    $properties = chado_retrieve_node_form_properties($node);
    $descrip_id = tripal_get_cvterm([
      'name' => 'Library Description',
      'cv_id' => ['name' => 'library_property'],
    ]);
    $properties[$descrip_id->cvterm_id][0] = $node->description;

    $details = [
      'property_table' => 'libraryprop',
      // the name of the prop table
      'base_table' => 'library',
      // the name of your chado base table
      'foreignkey_name' => 'library_id',
      // the name of the key in your base table
      'foreignkey_value' => $library_id
      // the value of the library_id key
    ];
    chado_update_node_form_properties($node, $details, $properties);

    // * Additional DBxrefs Form *
    $details = [
      'linking_table' => 'library_dbxref',
      // the name of your _dbxref table
      'foreignkey_name' => 'library_id',
      // the name of the key in your base table
      'foreignkey_value' => $library_id
      // the value of the library_id key
    ];
    chado_update_node_form_dbxrefs($node, $details);

  }
  else {
    $library_id = $node->library_id;
  }

  // Make sure the entry for this library doesn't already exist in the
  // chado_library table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('library', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->library_id = $library_id;
    drupal_write_record('chado_library', $record);
  }
}

/**
 * Implements hook_update().
 *
 * @ingroup tripal_legacy_library
 */
function chado_library_update($node) {

  $node->libraryname = trim($node->libraryname);
  $node->uniquename = trim($node->uniquename);
  $node->description = trim($node->description['value']);

  // update the library record
  $library_id = chado_get_id_from_nid('library', $node->nid);
  $match = [
    'library_id' => $library_id,
  ];

  $values = [
    'name' => $node->libraryname,
    'uniquename' => $node->uniquename,
    'organism_id' => $node->organism_id,
    'type_id' => $node->library_type,
  ];
  $status = chado_update_record('library', $match, $values);
  if (!$status) {
    drupal_set_message(t('Unable to update library.', 'warning'));
    watchdog('tripal_library', 'Update library: Unable to update library where values: %values',
      ['%values' => print_r($values, TRUE)], WATCHDOG_ERROR);
  }

  // * Properties Form *
  // add the description property
  $properties = chado_retrieve_node_form_properties($node);
  $descrip_id = tripal_get_cvterm([
    'name' => 'Library Description',
    'cv_id' => ['name' => 'library_property'],
  ]);
  $properties[$descrip_id->cvterm_id][0] = $node->description;

  $details = [
    'property_table' => 'libraryprop',
    // the name of the prop table
    'base_table' => 'library',
    // the name of your chado base table
    'foreignkey_name' => 'library_id',
    // the name of the key in your base table
    'foreignkey_value' => $library_id
    // the value of the library_id key
  ];
  chado_update_node_form_properties($node, $details, $properties);

  // * Additional DBxrefs Form *
  $details = [
    'linking_table' => 'library_dbxref',
    // the name of your _dbxref table
    'foreignkey_name' => 'library_id',
    // the name of the key in your base table
    'foreignkey_value' => $library_id
    // the value of the library_id key
  ];
  chado_update_node_form_dbxrefs($node, $details);

}

/**
 *  Implements hook_load().
 *
 *  When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_legacy_library
 */
function chado_library_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // get the feature details from chado
    $library_id = chado_get_id_from_nid('library', $node->nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$library_id) {
      continue;
    }

    $values = ['library_id' => $library_id];
    $library = chado_generate_var('library', $values);

    // the uniquename field is a text field so we need to expand it
    $library = chado_expand_var($library, 'field', 'library.uniquename');

    $nodes[$nid]->library = $library;

    // Now get the title
    $node->title = chado_get_node_title($node);
  }
}

/**
 * Implements hook_delete().
 *
 * Delete data from drupal and chado databases when a node is deleted
 *
 * @ingroup tripal_legacy_library
 */
function chado_library_delete(&$node) {

  $library_id = chado_get_id_from_nid('library', $node->nid);

  // if we don't have a library id for this node then this isn't a node of
  // type chado_library or the entry in the chado_library table was lost.
  if (!$library_id) {
    return;
  }

  // Remove data from {chado_library}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_library} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);

  // Remove data from library and libraryprop tables of chado database as well
  chado_query("DELETE FROM {libraryprop} WHERE library_id = :library_id", [':library_id' => $library_id]);
  chado_query("DELETE FROM {library} WHERE library_id = :library_id", [':library_id' => $library_id]);
}

/**
 * Implement hook_node_access().
 *
 * This hook allows node modules to limit access to the node types they define.
 *
 * @param $node
 *  The node on which the operation is to be performed, or, if it does not yet
 *   exist, the type of node to be created
 *
 * @param $op
 *  The operation to be performed
 *
 * @param $account
 *  A user object representing the user for whom the operation is to be
 *   performed
 *
 * @return
 *  If the permission for the specified operation is not set then return FALSE.
 *   If the permission is set then return NULL as this allows other modules to
 *   disable access.  The only exception is when the $op == 'create'.  We will
 *   always return TRUE if the permission is set.
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if ($node_type == 'chado_library') {
    if ($op == 'create') {
      if (!user_access('create chado_library content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_library content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_library content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_library content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_node_view(). Acts on all content types
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_node_view($node, $view_mode, $langcode) {

  switch ($node->type) {
    case 'chado_library':
      if ($view_mode == 'full') {
        $node->content['tripal_library_base'] = [
          '#theme' => 'tripal_library_base',
          '#node' => $node,
          '#tripal_toc_id' => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        ];
        $node->content['tripal_library_features'] = [
          '#theme' => 'tripal_library_features',
          '#node' => $node,
          '#tripal_toc_id' => 'features',
          '#tripal_toc_title' => 'Features',
        ];
        $node->content['tripal_library_properties'] = [
          '#theme' => 'tripal_library_properties',
          '#node' => $node,
          '#tripal_toc_id' => 'properties',
          '#tripal_toc_title' => 'Properties',
        ];
        $node->content['tripal_library_publications'] = [
          '#theme' => 'tripal_library_publications',
          '#node' => $node,
          '#tripal_toc_id' => 'publications',
          '#tripal_toc_title' => 'Publications',
        ];
        $node->content['tripal_library_references'] = [
          '#theme' => 'tripal_library_references',
          '#node' => $node,
          '#tripal_toc_id' => 'references',
          '#tripal_toc_title' => 'Cross References',
        ];
        $node->content['tripal_library_synonyms'] = [
          '#theme' => 'tripal_library_synonyms',
          '#node' => $node,
          '#tripal_toc_id' => 'synonyms',
          '#tripal_toc_title' => 'Synonyms',
        ];
        $node->content['tripal_library_terms'] = [
          '#theme' => 'tripal_library_terms',
          '#node' => $node,
          '#tripal_toc_id' => 'terms',
          '#tripal_toc_title' => 'Annotated Terms',
        ];
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_library_teaser'] = [
          '#theme' => 'tripal_library_teaser',
          '#node' => $node,
        ];
      }
      break;
    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_organism_libraries'] = [
          '#theme' => 'tripal_organism_libraries',
          '#node' => $node,
          '#tripal_toc_id' => 'libraries',
          '#tripal_toc_title' => 'Libraries',
        ];
      }
      break;
    case 'chado_feature':
      if ($view_mode == 'full') {
        $node->content['tripal_feature_libraries'] = [
          '#theme' => 'tripal_feature_libraries',
          '#node' => $node,
          '#tripal_toc_id' => 'libraries',
          '#tripal_toc_title' => 'Libraries',
        ];
      }
      break;
  }
}

/**
 * Implements hook_node_presave(). Acts on all node content types.
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_node_presave($node) {

  switch ($node->type) {
    // This step is for setting the title for the Drupal node.  This title
    // is permanent and thus is created to be unique.  Title changes provided
    // by tokens are generated on the fly dynamically, but the node title
    // seen in the content listing needs to be set here. Do not call
    // the chado_get_node_title() function here to set the title as the node
    // object isn't properly filled out and the function will fail.
    case 'chado_library':
      // for a form submission the 'libraryname' field will be set,
      // for a sync, we must pull from the library object
      if (property_exists($node, 'libraryname')) {
        // set the title
        $node->title = $node->libraryname;
      }
      else {
        if (property_exists($node, 'library')) {
          $node->title = $node->library->name;
        }
      }
      break;
  }
}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_node_insert($node) {

  switch ($node->type) {
    case 'chado_library':

      $library_id = chado_get_id_from_nid('library', $node->nid);
      $values = ['library_id' => $library_id];
      $library = chado_generate_var('library', $values);
      $library = chado_expand_var($library, 'field', 'library.uniquename');
      $node->library = $library;

      // Now get the title
      $node->title = chado_get_node_title($node);

      // Now use the API to set the path.
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements hook_node_update().
 * Acts on all content types.
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_node_update($node) {

  switch ($node->type) {
    case 'chado_library':

      // Now get the title
      $node->title = chado_get_node_title($node);

      // Now use the API to set the path.
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements [content_type]_chado_node_default_title_format().
 *
 * Defines a default title format for the Chado Node API to set the titles on
 * Chado library nodes based on chado fields.
 */
function chado_library_chado_node_default_title_format() {
  return '[library.name], [library.uniquename] ([library.type_id>cvterm.name])';
}

/**
 * Implements hook_chado_node_default_url_format().
 *
 * Designates a default URL format for library nodes.
 */
function chado_library_chado_node_default_url_format() {
  return '/library/[library.organism_id>organism.genus]/[library.organism_id>organism.species]/[library.type_id>cvterm.name]/[library.uniquename]';
}
