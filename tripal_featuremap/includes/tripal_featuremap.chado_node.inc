<?php
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
  $title = '';
  $description = '';
  $unittype_id = '';
  
  // if we are editing an existing node then the featuremap is already part of the node
  if (property_exists($node, 'featuremap')) {
    $featuremap = $node->featuremap;
    $featuremap = tripal_core_expand_chado_vars($featuremap, 'field', 'featuremap.description');
    $featuremap_id = $featuremap->featuremap_id;
    
    // get form defaults
    $title       = $featuremap->name;
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
    $title       = $form_state['values']['title'];
    $description = $form_state['values']['description'];
    $unittype_id = $form_state['values']['unittype_id'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $title       = $form_state['input']['title'];
    $description = $form_state['input']['description'];
    $unittype_id = $form_state['input']['unittype_id'];
  }
  
  $form['title']= array(
    '#type'          => 'textfield',
    '#title'         => t('Map Name'),
    '#description'   => t('Please enter a name for this map'),
    '#required'      => TRUE,
    '#default_value' => $title,
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
  $values = array(
    'cv_id' => array(
      'name' => 'featuremap_units',
    )
  );
  $columns = array('cvterm_id','name');
  $options = array('order_by' => array('name' => 'ASC'));
  $featuremap_units = tripal_core_chado_select('cvterm', $columns, $values, $options);
  $units = array();
  $units[''] = '';
  foreach($featuremap_units as $unit) {
    $units[$unit->cvterm_id] = $unit->name;
  }
  $form['unittype_id'] = array(
    '#title'       => t('Map Units'),
    '#type'        => t('select'),
    '#description' => t("Chose the units for this map"),
    '#required'    => TRUE,
    '#default_value' => $unittype_id,
    '#options'     => $units,
  );

  // get the featuremap properties
  $properties = array();
  $properties[] = 'Select a Property';
  $sql = "
    SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition
    FROM  {cvterm} CVT
      INNER JOIN {cv} ON CVT.cv_id = CV.cv_id
    WHERE 
      CV.name = 'featuremap_property' AND 
      NOT CVT.is_obsolete = 1
    ORDER BY CVT.name ASC 
  ";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    $properties[$prop->cvterm_id] = $prop->name;
  }

  $exclude = array();
  $include = array();
  $instructions = t('To add additional properties to the drop down. ' . l("Add terms to the featuremap_property vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");
  tripal_core_properties_form($form, $form_state, 'featuremapprop', 'featuremap_id', 'featuremap_property',
    $properties, $featuremap_id, $exclude, $include, $instructions, 'Properties');

  return $form;
}
/**
 *  validates submission of form when adding or updating a map node
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_validate($node, $form, &$form_state) {
  $node->title          = trim($node->title);
  $node->description    = trim($node->description);
  
  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }
  
  // we are syncing if we do not have a node ID but we do have a featuremap_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'featuremap_id') and $node->featuremap_id != 0) {
    return;
  }

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
    $featuremap = chado_query($sql, array(':name' => $node->title, ':featuremap_id' => $node->featuremap_id))->fetchObject();
  }
  else {
    $sql = "SELECT * FROM {featuremap} WHERE name = :name";
    $featuremap = chado_query($sql, array(':name' => $node->title))->fetchObject();
  }
  if ($featuremap) {
    form_set_error('title', t('The unique map name already exists. Please choose another'));
  }
}


/**
 * Implement hook_access().
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
  if ($op == 'create') {
    if (!user_access('create chado_featuremap content', $account)) {
      return FALSE;
    }
    return TRUE;
  }

  if ($op == 'update') {
    if (!user_access('edit any chado_featuremap content', $account) &&
    !user_access('edit own chado_featuremap content', $account)) {
      return FALSE;
    }
    if (user_access('edit own chado_featuremap content', $account) &&
    $account->uid != $node->uid) {
      return FALSE;
    }
  }

  if ($op == 'delete') {
    if (!user_access('delete any chado_featuremap content', $account) &&
    !user_access('delete own chado_featuremap content', $account)) {
      return FALSE;
    }
    if (user_access('delete own chado_featuremap content', $account) &&
    $account->uid != $node->uid) {
      return FALSE;
    }
  }
  return NULL;
}

/**
 *  When a new chado_featuremap node is created we also need to add information
 *  to our chado_featuremap table.  This function is called on insert of a new node
 *  of type 'chado_featuremap' and inserts the necessary information.
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_insert($node) {

  $node->title          = trim($node->title);
  $node->description    = trim($node->description);
  
  // if there is an featuremap_id in the $node object then this must be a sync so
  // we can skip adding the featuremap as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'featuremap_id')) {
    
    $values = array(
      'name'        => $node->title,
      'description' => $node->description,
      'unittype_id' => $node->unittype_id
    );
    $featuremap = tripal_core_chado_insert('featuremap', $values);
    if(!$featuremap) {
      drupal_set_message(t('Unable to add featuremap.', 'warning'));
      watchdog('tripal_featuremap', 'Unable to create feature map where values: %values',
      array('%values' => print_r($values, TRUE)), WATCHDOG_WARNING);
      return;
    }
    $featuremap_id = $featuremap['featuremap_id'];

    // get the properties from the form
    $properties = tripal_core_properties_form_retreive($node, 'featuremap_property');
    
    // now add in the properties
    $properties = tripal_core_properties_form_retreive($node, 'featuremap_property');
    foreach ($properties as $property => $elements) {
      foreach ($elements as $rank => $value) {
        // if the property name is 'Map Dbxref' then save this as a Dbxref record not a property
        if ($property == 'Map Dbxref') {
          $featuremap_dbxref = tripal_featuremap_add_featuremap_dbxref($featuremap_id, $value);
          if (!$featuremap_dbxref) {
            drupal_set_message("Error cannot add featuremap cross reference: $value", "error");
            watchdog('t_featuremap', "Error cannot add featuremap cross reference: %ref",
            array('%ref' => $value), WATCHDOG_ERROR);
          }
        }
        // this is a property so add it 
        else {
          $status = tripal_featuremap_insert_property($featuremap_id, $property, $value, FALSE, 'featuremap_property');
          if (!$status) {
            drupal_set_message("Error cannot add property: $property", "error");
            watchdog('t_featuremap', "Error cannot add property: %prop",
            array('%property' => $property), WATCHDOG_ERROR);
          }
        }
      }
    }
  }
  else {
    $featuremap_id = $node->featuremap_id;
  }

  // Make sure the entry for this featuremap doesn't already exist in the
  // chado_featuremap table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_for_node('featuremap', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->featuremap_id = $featuremap_id;
    drupal_write_record('chado_featuremap', $record);
  }
}
/**
 * Update nodes
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_update($node) {
  
  $node->title          = trim($node->title);
  $node->description    = trim($node->description);
  
  $featuremap_id = chado_get_id_for_node('featuremap', $node->nid) ;

  // update the map record
  $match = array(
    'featuremap_id' => $featuremap_id,
  );
  $values = array(
    'name' => $node->title,
    'description' => $node->description,
    'unittype_id' => $node->unittype_id
  );
  $status = tripal_core_chado_update('featuremap', $match, $values);
  if (!$status) {
    drupal_set_message("Error updating map", "error");
    watchdog('t_featuremap', "Error updating map", array(), WATCHDOG_ERROR);
    return;
  }

  // get the properties from the form
  $properties = tripal_core_properties_form_retreive($node, 'featuremap_property');
  
  tripal_core_chado_delete('featuremapprop', array('featuremap_id' => $featuremap_id));
  foreach ($properties as $property => $elements) {
    foreach ($elements as $rank => $value) {
      // if the property name is 'Map Dbxref' then save this as a Dbxref record not a property
      if ($property == 'Map Dbxref') {
        $featuremap_dbxref = tripal_featuremap_add_featuremap_dbxref($featuremap_id, $value);
        if (!$featuremap_dbxref) {
          drupal_set_message("Error cannot add featuremap cross reference: $value", "error");
          watchdog('t_featuremap', "Error cannot add featuremap cross reference: %ref",
          array('%ref' => $value), WATCHDOG_ERROR);
        }
      }
      // this is a property so add it
      else {
        $status = tripal_featuremap_insert_property($featuremap_id, $property, $value, FALSE, 'featuremap_property');
        if (!$status) {
          drupal_set_message("Error cannot add property: $property", "error");
          watchdog('t_featuremap', "Error cannot add property: %prop",
          array('%property' => $property), WATCHDOG_ERROR);
        }
      }
    }
  }
}
/**
 *  When a node is requested by the user this function is called to allow us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_load($nodes) {
  foreach ($nodes as $nid => $node) {
    // get the feature details from chado
    $featuremap_id = chado_get_id_for_node('featuremap', $node->nid);
  
    $values = array('featuremap_id' => $featuremap_id);
    $featuremap = tripal_core_generate_chado_var('featuremap', $values);
  
    // expand the description field as it is needed by the form
    $featuremap = tripal_core_expand_chado_vars($featuremap, 'field', 'featuremap.description');
  
    $nodes[$nid]->featuremap = $featuremap;
  }

}
/**
 * Delete data from drupal and chado databases when a node is deleted
 * @ingroup tripal_featuremap
 */
function chado_featuremap_delete(&$node) {

  $featuremap_id = chado_get_id_for_node('featuremap', $node->nid);

  // if we don't have a map id for this node then this isn't a node of
  // type chado_featuremap or the entry in the chado_featuremap table was lost.
  if (!$featuremap_id) {
    return;
  }

  // Remove data from {chado_featuremap}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_featuremap} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revisions} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from map and mapprop tables of chado database as well
  chado_query("DELETE FROM {featuremapprop}    WHERE featuremap_id = :featuremap_id", array(':featuremap_id' => $featuremap_id));
  chado_query("DELETE FROM {featuremap_dbxref} WHERE featuremap_id = :featuremap_id", array(':featuremap_id' => $featuremap_id));
  chado_query("DELETE FROM {featuremap}        WHERE featuremap_id = :featuremap_id", array(':featuremap_id' => $featuremap_id));
  
}