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
 * @ingroup tripal_library
 */
function tripal_library_node_info() {
  $nodes = array();
  $nodes['chado_library'] = array(
    'name'        => t('Library'),
    'base'        => 'chado_library',
    'description' => t('A library from the chado database'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    'chado_node_api' => array(
      'base_table' => 'library',
      'hook_prefix' => 'chado_library',
      'record_type_title' => array(
        'singular' => t('Library'),
        'plural' => t('Libraries')
      ),
      'sync_filters' => array(
        'type_id' => TRUE,
        'organism_id' => TRUE
      ),
    )
  );
  return $nodes;
}

/**
 *  Implements hook_form().
 *
 * When editing or creating a new node of type 'chado_library' we need
 *  a form.  This function creates the form that will be used for this.
 *
 * @ingroup tripal_library
 */
function chado_library_form($node, &$form_state) {
  $form = array();

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

    $libraryname  = $library->name;
    $uniquename   = $library->uniquename;
    $library_type = $library->type_id->cvterm_id;
    $organism_id  = $library->organism_id->organism_id;

    $libprop = tripal_library_get_property($library->library_id, 'Library Description');
    $description = $libprop->value;

    // keep track of the library id if we have.  If we do have one then
    // this is an update as opposed to an insert.
    $form['library_id'] = array(
      '#type' => 'value',
      '#value' => $library_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $libraryname  = $form_state['values']['libraryname'];
    $uniquename   = $form_state['values']['uniquename'];
    $library_type = $form_state['values']['library_type'];
    $organism_id  = $form_state['values']['organism_id'];
    $description  = $form_state['values']['description'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $libraryname  = $form_state['input']['libraryname'];
    $uniquename   = $form_state['input']['uniquename'];
    $library_type = $form_state['input']['library_type'];
    $organism_id  = $form_state['input']['organism_id'];
    $description  = $form_state['input']['description'];
  }

  $form['libraryname']= array(
    '#type'          => 'textfield',
    '#title'         => t('Library Name'),
    '#description'   => t('Please enter the name for this library. Library names should be recognizable but do not need to be unique.'),
    '#required'      => TRUE,
    '#default_value' => $libraryname,
  );

  $form['uniquename']= array(
    '#type'          => 'textfield',
    '#title'         => t('Unique Name'),
    '#description'   => t('Please enter a unique name for this library. This can be any value used to uniquely identify a library.'),
    '#required'      => TRUE,
    '#default_value' => $uniquename,
  );

  // get the list of library types  
  $types = tripal_get_cvterm_default_select_options('library', 'type_id', 'library types');
  $types[0] = 'Select a Type';

  $form['library_type'] = array(
    '#title'       => t('Library Type'),
    '#type'        => t('select'),
    '#description' => t("Choose the library type."),
    '#required'    => TRUE,
    '#default_value' => $library_type,
    '#options'     => $types,
  );

  // get the list of organisms
  $sql = "SELECT * FROM {organism}";
  $org_rset = chado_query($sql);

  $organisms = array();
  $organisms[''] = '';
  while ($organism = $org_rset->fetchObject()) {
    $organisms[$organism->organism_id] =
    "$organism->genus $organism->species ($organism->common_name)";
  }

  $form['organism_id'] = array(
    '#title'       => t('Organism'),
    '#type'        => t('select'),
    '#description' => t("Choose the organism with which this library is associated."),
    '#required'    => TRUE,
    '#default_value' => $organism_id,
    '#options'     => $organisms,
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Library Description'),
    '#description'   => t('A brief description of the library'),
    '#required'      => TRUE,
    '#default_value' => $description,
  );

  // PROPERTIES FORM
  //---------------------------------------------
  $select_options = array();
  $prop_cv = tripal_get_default_cv('libraryprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  // if the default is the 'library_property' vocabulary then we want
  // to exclude the 'Library Description' term since it has it's own form element above
  if ($prop_cv->name == 'library_property') {
    // Generate our own select list so we can exclude the description element
    $cv_result = chado_select_record('cv',array('cv_id'),array('name' => 'library_property'));
    $cv_id = $cv_result[0]->cv_id;
    $select_options = tripal_cv_get_cvterm_options($cv_id);
    $descrip_id = array_search('Library Description', $select_options);
    unset($select_options[$descrip_id]);
  }
  $details = array(
    'property_table' => 'libraryprop',      // the name of the prop table
    'chado_id' => $library_id,              // the value of library_id for this record
    'cv_id' => $cv_id,                      // the cv.cv_id of the cv governing libraryprop.type_id
    'select_options' => $select_options
  );
  // Adds the form elements to your current form
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'library_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'library_id',   // the name of the key in your base chado table
    'base_key_value' => $library_id       // the value of library_id for this record
  );
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  return $form;
}

/**
 * Implements hook_validate().
 *
 * Validates submission of form when adding or updating a library node
 *
 * @ingroup tripal_library
 */
function chado_library_validate($node, $form, &$form_state) {
  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }
  
  // we are syncing if we do not have a node ID but we do have a featuremap_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'library_id') and $node->library_id != 0) {
    return;
  }
  
  // trim white space from text fields
  $node->libraryname = trim($node->libraryname);
  $node->uniquename  = trim($node->uniquename);
  $node->description = trim($node->description);

  $lib = 0;
  // check to make sure the unique name on the library is unique
  // before we try to insert into chado.
  if (property_exists($node, 'library_id')) {
    $sql = "
      SELECT *
      FROM {library}
      WHERE uniquename = :uname AND NOT library_id = :library_id
    ";
    $lib = chado_query($sql, array(':uname' => $node->uniquename, ':library_id' => $node->library_id))->fetchObject();
  }
  else {
    $sql = "SELECT * FROM {library} WHERE uniquename = :uname";
    $lib = chado_query($sql, array(':uname' => $node->uniquename))->fetchObject();
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
 * @ingroup tripal_library
 */
function chado_library_insert($node) {

  $node->libraryname = trim($node->libraryname);
  $node->uniquename  = trim($node->uniquename);
  $node->description = trim($node->description);

  // if there is an library_id in the $node object then this must be a sync so
  // we can skip adding the library as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'library_id')) {
    $values = array(
      'name' => $node->libraryname,
      'uniquename' => $node->uniquename,
      'organism_id' => $node->organism_id,
      'type_id' => $node->library_type,
    );
    $library = chado_insert_record('library', $values);
    if (!$library) {
      drupal_set_message(t('Unable to add library.', 'warning'));
      watchdog('tripal_library', 'Insert library: Unable to create library where values: %values',
        array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
      return;
    }
    $library_id = $library['library_id'];

    // * Properties Form *
    // add the description property
    $properties = chado_retrieve_node_form_properties($node);
    $descrip_id = tripal_cv_get_cvterm_by_name('Library Description', NULL, 'library_property');
    $properties[$descrip_id->cvterm_id][0] = $node->description;

    $details = array(
      'property_table' => 'libraryprop',   // the name of the prop table
      'base_table' => 'library',           // the name of your chado base table
      'foreignkey_name' => 'library_id',   // the name of the key in your base table
      'foreignkey_value' => $library_id    // the value of the library_id key
    );
    chado_update_node_form_properties($node, $details, $properties);

    // * Additional DBxrefs Form *
    $details = array(
      'linking_table' => 'library_dbxref',   // the name of your _dbxref table
      'foreignkey_name' => 'library_id',     // the name of the key in your base table
      'foreignkey_value' => $library_id      // the value of the library_id key
    );
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
 * @ingroup tripal_library
 */
function chado_library_update($node) {

  $node->libraryname = trim($node->libraryname);
  $node->uniquename  = trim($node->uniquename);
  $node->description = trim($node->description);

  // update the library record
  $library_id = chado_get_id_from_nid('library', $node->nid);
  $match = array(
    'library_id' => $library_id,
  );

  $values = array(
    'name'        => $node->libraryname,
    'uniquename'  => $node->uniquename,
    'organism_id' => $node->organism_id,
    'type_id'     => $node->library_type,
  );
  $status = chado_update_record('library', $match, $values);
  if (!$status) {
    drupal_set_message(t('Unable to update library.', 'warning'));
    watchdog('tripal_library', 'Update library: Unable to update library where values: %values',
    array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
  }

  // * Properties Form *
  // add the description property
  $properties = chado_retrieve_node_form_properties($node);
  $descrip_id = tripal_cv_get_cvterm_by_name('Library Description', NULL, 'library_property');
  $properties[$descrip_id->cvterm_id][0] = $node->description;

  $details = array(
    'property_table' => 'libraryprop',   // the name of the prop table
    'base_table' => 'library',           // the name of your chado base table
    'foreignkey_name' => 'library_id',   // the name of the key in your base table
    'foreignkey_value' => $library_id    // the value of the library_id key
  );
  chado_update_node_form_properties($node, $details, $properties);

  // * Additional DBxrefs Form *
  $details = array(
    'linking_table' => 'library_dbxref',   // the name of your _dbxref table
    'foreignkey_name' => 'library_id',     // the name of the key in your base table
    'foreignkey_value' => $library_id      // the value of the library_id key
  );
  chado_update_node_form_dbxrefs($node, $details);

}

/**
 *  Implements hook_load().
 *
 *  When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_library
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
    
    $values = array('library_id' => $library_id);
    $library = chado_generate_var('library', $values);

    // the uniquename field is a text field so we need to expand it
    $library = chado_expand_var($library, 'field', 'library.uniquename');

    $nodes[$nid]->library = $library;
  }
}

/**
 * Implements hook_delete().
 *
 * Delete data from drupal and chado databases when a node is deleted
 *
 * @ingroup tripal_library
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
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from library and libraryprop tables of chado database as well
  chado_query("DELETE FROM {libraryprop} WHERE library_id = :library_id", array(':library_id' => $library_id));
  chado_query("DELETE FROM {library} WHERE library_id = :library_id", array(':library_id' => $library_id));
}

/**
 * Implement hook_node_access().
 *
 * This hook allows node modules to limit access to the node types they define.
 *
 *  @param $node
 *  The node on which the operation is to be performed, or, if it does not yet exist, the
 *  type of node to be created
 *
 *  @param $op
 *  The operation to be performed
 *
 *  @param $account
 *  A user object representing the user for whom the operation is to be performed
 *
 *  @return
 *  If the permission for the specified operation is not set then return FALSE. If the
 *  permission is set then return NULL as this allows other modules to disable
 *  access.  The only exception is when the $op == 'create'.  We will always
 *  return TRUE if the permission is set.
 *
 * @ingroup tripal_library
 */
function chado_library_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'chado_library') {
    if ($op == 'create') {
      if (!user_access('create chado_library', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_library', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_library', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_library', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_node_view(). Acts on all content types
 *
 * @ingroup tripal_library
 */
function tripal_library_node_view($node, $view_mode, $langcode) {

  switch ($node->type) {
    case 'chado_library':
      if ($view_mode == 'full') {
        $node->content['tripal_library_base'] = array(
          '#markup' => theme('tripal_library_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_library_properties'] = array(
          '#markup' => theme('tripal_library_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_library_publications'] = array(
          '#markup' => theme('tripal_library_publications', array('node' => $node)),
          '#tripal_toc_id'    => 'publications',
          '#tripal_toc_title' => 'Publications',
        );
        $node->content['tripal_library_references'] = array(
          '#markup' => theme('tripal_library_references', array('node' => $node)),
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
        $node->content['tripal_library_synonyms'] = array(
          '#markup' => theme('tripal_library_synonyms', array('node' => $node)),
          '#tripal_toc_id'    => 'synonyms',
          '#tripal_toc_title' => 'Synonyms',
        );
        $node->content['tripal_library_terms'] = array(
          '#markup' => theme('tripal_library_terms', array('node' => $node)),
          '#tripal_toc_id'    => 'terms',
          '#tripal_toc_title' => 'Annotated Terms',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_library_teaser'] = array(
          '#markup' => theme('tripal_library_teaser', array('node' => $node)),
        );
      }
      break;
    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_organism.libraries'] = array(
          '#markup' => theme('tripal_organism.libraries', array('node' => $node)),
          '#tripal_toc_id'    => 'libraries',
          '#tripal_toc_title' => 'Libraries',
        );
      }
      break;
    case 'chado_feature':
      if ($view_mode == 'full') {
        $node->content['tripal_feature.libraries'] = array(
          '#markup' => theme('tripal_feature.libraries', array('node' => $node)),
          '#tripal_toc_id'    => 'libraries',
          '#tripal_toc_title' => 'Libraries',
        );
      }
      break;
  }
}

/**
 * Implements hook_node_presave(). Acts on all node content types.
 *
 * @ingroup tripal_library
 */
function tripal_library_node_presave($node) {

  switch ($node->type) {
    case 'chado_library':
      // for a form submission the 'libraryname' field will be set,
      // for a sync, we must pull from the library object
      if (property_exists($node, 'libraryname')) {
        // set the title
        $node->title = $node->libraryname;
      }
      else if (property_exists($node, 'library')) {
        $node->title = $node->library->name;
      }
      break;
  }
}
