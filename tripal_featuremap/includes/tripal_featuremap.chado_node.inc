<?php
/**
 * @file
 * Hooks implementing the feature map node content type
 */

/**
 * Implements hook_node_info().
 *
 * Provide information to drupal about the node types that we're creating
 * in this module
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_node_info() {
  $nodes = array();
  $nodes['chado_featuremap'] = array(
    'name'        => t('Feature Map'),
    'base'        => 'chado_featuremap',
    'description' => t('A map of features from the chado database (e.g. genetic map)'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    'chado_node_api' => array(
      'base_table' => 'featuremap',
      'hook_prefix' => 'chado_featuremap',
      'record_type_title' => array(
        'singular' => t('Feature Map'),
        'plural' => t('Feature Maps')
      ),
      'sync_filters' => array(
        'type_id' => FALSE,
        'organism_id' => FALSE
      ),
    )
  );
  return $nodes;
}

/**
 *  When editing or creating a new node of type 'chado_featuremap' we need
 *  a form.  This function creates the form that will be used for this.
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_form($node, &$form_state) {
  $form = array();

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing library
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $featuremap_id = NULL;
  $fmapname = '';
  $description = '';
  $unittype_id = '';

  // if we are editing an existing node then the featuremap is already part of the node
  if (property_exists($node, 'featuremap')) {
    $featuremap = $node->featuremap;
    $featuremap = chado_expand_var($featuremap, 'field', 'featuremap.description');
    $featuremap_id = $featuremap->featuremap_id;

    // get form defaults
    $fmapname    = $featuremap->name;
    $description = $featuremap->description;
    $unittype_id = $featuremap->unittype_id->cvterm_id;

    // set the featuremap_id in the form
    $form['featuremap_id'] = array(
      '#type' => 'hidden',
      '#value' => $featuremap_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $fmapname       = $form_state['values']['fmapname'];
    $description = $form_state['values']['description'];
    $unittype_id = $form_state['values']['unittype_id'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $fmapname       = $form_state['input']['fmapname'];
    $description = $form_state['input']['description'];
    $unittype_id = $form_state['input']['unittype_id'];
  }

  $form['fmapname']= array(
    '#type'          => 'textfield',
    '#title'         => t('Map Name'),
    '#description'   => t('Please enter a name for this map'),
    '#required'      => TRUE,
    '#default_value' => $fmapname,
    '#maxlength'     => 255
  );
  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Map Description'),
    '#description'   => t('A description of the map.'),
    '#required'      => TRUE,
    '#default_value' => $description,
  );

  // get the list of unit types  
  $units = tripal_get_cvterm_default_select_options('featuremap', 'unittype_id', 'map unit types');
  $units[0] = 'Select a Type';
  
  $form['unittype_id'] = array(
    '#title'       => t('Map Units'),
    '#type'        => t('select'),
    '#description' => t("Chose the units for this map"),
    '#required'    => TRUE,
    '#default_value' => $unittype_id,
    '#options'     => $units,
  );

  // Properties Form
  // ----------------------------------
  $prop_cv = tripal_get_default_cv('featuremap_property', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $instructions = t('To add additional properties to the drop down. ' . l("Add terms to the featuremap_property vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");
  $details = array(
    'property_table' => 'featuremapprop',
    'chado_id' => $featuremap_id,
    'cv_id' => $cv_id,
    'fieldset_name' => 'Additional Details',
    'additional_instructions' => $instructions
  );
  // TODO: remove the 'Map Dbxref' from the list as that should now be handled
  // by the dbxref interface below
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'featuremap_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'featuremap_id',   // the name of the key in your base chado table
    'base_key_value' => $featuremap_id       // the value of featuremap_id for this record
  );
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  return $form;
}

/**
 * Validates submission of form when adding or updating a map node
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_validate($node, $form, &$form_state) {

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a featuremap_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'featuremap_id') and $node->featuremap_id != 0) {
    return;
  }

  // trim white space from text fields
  $node->fmapname       = trim($node->fmapname);
  $node->description    = trim($node->description);
  
  $featuremap = 0;
  // check to make sure the unique name on the map is unique
  // before we try to insert into chado. If this is an update then we will
  // have a featuremap_id, therefore we want to look for another map with this
  // name but with a different featuremap_id. If this is an insert, just look
  // for a case where the name already exists.
  if (property_exists($node, 'featuremap_id')) {
    $sql = "
      SELECT * FROM {featuremap}
      WHERE name = :name AND NOT featuremap_id = :featuremap_id
    ";
    $featuremap = chado_query($sql, array(':name' => $node->fmapname, ':featuremap_id' => $node->featuremap_id))->fetchObject();
  }
  else {
    $sql = "SELECT * FROM {featuremap} WHERE name = :name";
    $featuremap = chado_query($sql, array(':name' => $node->fmapname))->fetchObject();
  }
  if ($featuremap) {
    form_set_error('fmapname', t('The unique map name already exists. Please choose another'));
  }
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
 * @ingroup tripal_featuremap
 */
function chado_featuremap_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'chado_featuremap') {
    if ($op == 'create') {
      if (!user_access('create chado_featuremap', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_featuremap', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_featuremap', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_featuremap', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_insert().
 *
 * When a new chado_featuremap node is created we also need to add information
 *  to our chado_featuremap table.  This function is called on insert of a new node
 *  of type 'chado_featuremap' and inserts the necessary information.
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_insert($node) {

  $node->fmapname       = trim($node->fmapname);
  $node->description    = trim($node->description);

  // if there is an featuremap_id in the $node object then this must be a sync so
  // we can skip adding the featuremap as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'featuremap_id')) {

    $values = array(
      'name'        => $node->fmapname,
      'description' => $node->description,
      'unittype_id' => $node->unittype_id
    );
    $featuremap = chado_insert_record('featuremap', $values);
    if(!$featuremap) {
      drupal_set_message(t('Unable to add featuremap.', 'warning'));
      tripal_report_error('tripal_featuremap', TRIPAL_WARNING, 'Unable to create feature map where values: %values',
      array('%values' => print_r($values, TRUE)));
      return;
    }
    $featuremap_id = $featuremap['featuremap_id'];

    // now add in the properties
    $properties = chado_retrieve_node_form_properties($node);
    // We need to deal with the 'Map Dbxref' property specially
    $cvterm = chado_select_record(
      'cvterm',
      array('cvterm_id'),
      array('name' => 'Map Dbxref', 'cv_id' => array('name' => 'featuremap_property'))
    );
    $map_dbxref_cvterm_id = $cvterm[0]->cvterm_id;
    if (isset($properties[$map_dbxref_cvterm_id])) {
      foreach ($properties[$map_dbxref_cvterm_id] as $rank => $value) {
        $featuremap_dbxref = tripal_featuremap_add_featuremap_dbxref($featuremap_id, $value);
        if (!$featuremap_dbxref) {
          drupal_set_message("Error cannot add featuremap cross reference: $value", "error");
          tripal_report_error('t_featuremap', TRIPAL_ERROR, "Error cannot add featuremap cross reference: %ref",
          array('%ref' => $value));
        }
      }
      unset($properties[$map_dbxref_cvterm_id]);
    }
    $details = array(
      'property_table' => 'featuremapprop',
      'base_table' => 'featuremap',
      'foreignkey_name' => 'featuremap_id',
      'foreignkey_value' => $featuremap_id
    );
    chado_update_node_form_properties($node, $details, $properties);

    // * Additional DBxrefs Form *
    $details = array(
      'linking_table' => 'featuremap_dbxref',   // the name of your _dbxref table
      'foreignkey_name' => 'featuremap_id',     // the name of the key in your base table
      'foreignkey_value' => $featuremap_id      // the value of the featuremap_id key
    );
    chado_update_node_form_dbxrefs($node, $details);

  }
  else {
    $featuremap_id = $node->featuremap_id;
  }

  // Make sure the entry for this featuremap doesn't already exist in the
  // chado_featuremap table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('featuremap', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->featuremap_id = $featuremap_id;
    drupal_write_record('chado_featuremap', $record);
  }
}

/**
 * Implements hook_update(). Update nodes
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_update($node) {

  $node->fmapname          = trim($node->fmapname);
  $node->description    = trim($node->description);

  $featuremap_id = chado_get_id_from_nid('featuremap', $node->nid) ;

  // update the map record
  $match = array(
    'featuremap_id' => $featuremap_id,
  );
  $values = array(
    'name' => $node->fmapname,
    'description' => $node->description,
    'unittype_id' => $node->unittype_id
  );
  $status = chado_update_record('featuremap', $match, $values);
  if (!$status) {
    drupal_set_message("Error updating map", "error");
    tripal_report_error('t_featuremap', TRIPAL_ERROR, "Error updating map", array());
    return;
  }

  // Update the properties
  $properties = chado_retrieve_node_form_properties($node);
  // We need to deal with the 'Map Dbxref' property specially
  $cvterm = chado_select_record(
    'cvterm',
    array('cvterm_id'),
    array('name' => 'Map Dbxref', 'cv_id' => array('name' => 'featuremap_property'))
  );
  $map_dbxref_cvterm_id = $cvterm[0]->cvterm_id;
  if (isset($properties[$map_dbxref_cvterm_id])) {
    foreach ($properties[$map_dbxref_cvterm_id] as $rank => $value) {
      $featuremap_dbxref = tripal_featuremap_add_featuremap_dbxref($featuremap_id, $value);
      if (!$featuremap_dbxref) {
        drupal_set_message("Error cannot add featuremap cross reference: $value", "error");
        tripal_report_error('t_featuremap', TRIPAL_ERROR, "Error cannot add featuremap cross reference: %ref",
        array('%ref' => $value));
      }
    }
    unset($properties[$map_dbxref_cvterm_id]);
  }
  $details = array(
    'property_table' => 'featuremapprop',
    'base_table' => 'featuremap',
    'foreignkey_name' => 'featuremap_id',
    'foreignkey_value' => $featuremap_id
  );
  chado_update_node_form_properties($node, $details, $properties);

  // * Additional DBxrefs Form *
  $details = array(
    'linking_table' => 'featuremap_dbxref',   // the name of your _dbxref table
    'foreignkey_name' => 'featuremap_id',     // the name of the key in your base table
    'foreignkey_value' => $featuremap_id      // the value of the featuremap_id key
  );
  chado_update_node_form_dbxrefs($node, $details);

}

/**
 * Implements hook_load().
 *
 *  When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_load($nodes) {
  foreach ($nodes as $nid => $node) {
    // get the feature details from chado
    $featuremap_id = chado_get_id_from_nid('featuremap', $node->nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$featuremap_id) {
      continue;
    }
    
    $values = array('featuremap_id' => $featuremap_id);
    $featuremap = chado_generate_var('featuremap', $values);

    // expand the description field as it is needed by the form
    $featuremap = chado_expand_var($featuremap, 'field', 'featuremap.description');

    $nodes[$nid]->featuremap = $featuremap;
  }

}

/**
 * Implements hook_delete().
 *
 * Delete data from drupal and chado databases when a node is deleted
 * @ingroup tripal_featuremap
 */
function chado_featuremap_delete(&$node) {

  $featuremap_id = chado_get_id_from_nid('featuremap', $node->nid);

  // if we don't have a map id for this node then this isn't a node of
  // type chado_featuremap or the entry in the chado_featuremap table was lost.
  if (!$featuremap_id) {
    return;
  }

  // Remove data from {chado_featuremap}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_featuremap} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from map and mapprop tables of chado database as well
  chado_query("DELETE FROM {featuremapprop}    WHERE featuremap_id = :featuremap_id", array(':featuremap_id' => $featuremap_id));
  chado_query("DELETE FROM {featuremap_dbxref} WHERE featuremap_id = :featuremap_id", array(':featuremap_id' => $featuremap_id));
  chado_query("DELETE FROM {featuremap}        WHERE featuremap_id = :featuremap_id", array(':featuremap_id' => $featuremap_id));

}

/**
 * Implements hook_node_presave(). Acts on all content types.
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_node_presave($node) {
  switch ($node->type) {
    case 'chado_featuremap':
      // for a form submission the 'fmapname' field will be set,
      // for a sync, we must pull from the featuremap object
      if (property_exists($node, 'fmapname')) {
        // set the title
        $node->title = $node->fmapname;
      }
      else if (property_exists($node, 'featuremap')) {
        $node->title = $node->featuremap->name;
      }
      break;
  }
}

/**
 * Implements hook_node_view(). Acts on all content types.
 *
 * @ingroup tripal_feature
 */
function tripal_featuremap_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_featuremap':
      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_featuremap_base'] = array(
          '#markup' => theme('tripal_featuremap_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_featuremap_featurepos'] = array(
          '#markup' => theme('tripal_featuremap_featurepos', array('node' => $node)),
          '#tripal_toc_id'    => 'featurepos',
          '#tripal_toc_title' => 'Map Features',
        );
        $node->content['tripal_featuremap_properties'] = array(
          '#markup' => theme('tripal_featuremap_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_featuremap_publication'] = array(
          '#markup' => theme('tripal_featuremap_publication', array('node' => $node)),
          '#tripal_toc_id'    => 'publications',
          '#tripal_toc_title' => 'Publications',
        );
        $node->content['tripal_featuremap_references'] = array(
          '#markup' => theme('tripal_featuremap_references', array('node' => $node)),
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_featuremap_teaser'] = array(
          '#markup' => theme('tripal_featuremap_teaser', array('node' => $node)),
        );
      }
      break;
    case 'chado_feature':
      if ($view_mode == 'full') {
        $node->content['tripal_feature_featurepos'] = array(
          '#markup' => theme('tripal_feature_featurepos', array('node' => $node)),
          '#tripal_toc_id'    => 'featurepos',
          '#tripal_toc_title' => 'Maps',
        );
      }
      break;
  }
}