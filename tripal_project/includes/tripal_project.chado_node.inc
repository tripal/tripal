<?php

/**
 * Implementation of hook_form().
 *
 *  This form takes the Project Title information and description from the user.
 *
 *  @parm $node
 *    The initialized node
 *
 *  @parm $form_state
 *    The state of the form, that has the user entered information that is neccessary for adding
 *    information to the project
 *
 *  @return $form
 *    An array as described by the Drupal Form API
 *
 *
 * @ingroup tripal_project
 */
function chado_project_form(&$node, $form_state) {
  $form = array();
  
dpm("hi");
  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing project
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $project_id = null;
  $title = '';
  $description = '';
  
  // if we are editing an existing node then the project is already part of the node
  if (property_exists($node, 'project')) {
    $project = $node->project;
    // get the project default values.  When this module was first created
    // the project description was incorrectly stored in the $node->body field.
    // It is better to store it in the Chado tables.  However, the 'description'
    // field of the project table is only 255 characters.  So, we are going
    // to follow the same as the project module and store the description in
    // the projectprop table and leave the project.description field blank.
    // however, for backwards compatibitily, we check to see if the description
    // is in the $node->body field. If it is we'll use that.  When the node is
    // edited the text will be moved out of the body and into the projectprop
    // table where it should belong.
    if ($node->body) {
      $description = $node->body;
    }
    else {
      $description = $node->description;
    }
    if (!$description) {
      $projectprop = tripal_project_get_property($project->project_id, 'Project Description');
      $description = $projectprop->value;
    }
    
    // keep track of the project id if we have.  If we do have one then
    // this is an update as opposed to an insert.
    $form['project_id'] = array(
      '#type' => 'value',
      '#value' => $project->project_id,
    );
  }
  
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $title       = $form_state['values']['title'];
    $description = $form_state['values']['description'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $title       = $form_state['input']['title'];
    $description = $form_state['input']['description'];
  }

  $form['title']= array(
    '#type'          => 'textfield',
    '#title'         => t('Project Title'),
    '#description'   => t('Please enter the title for this project. This appears at the top of the project page.'),
    '#required'      => TRUE,
    '#default_value' => $node->title,
    '#weight'        => 1
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Project Description'),
    '#description'   => t('A brief description of the project'),
    '#required'      => TRUE,
    '#default_value' => $description,
    '#weight'        => 5
  );

  // get the project properties
  $properties = array();
  $properties[] = 'Select a Property';
  $sql = "
    SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition
    FROM  {cvterm} CVT
      INNER JOIN {cv} ON CVT.cv_id = CV.cv_id
    WHERE
      CV.name = 'project_property' AND
      NOT CVT.is_obsolete = 1
    ORDER BY CVT.name ASC
  ";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    // because we are using the project description property as
    // a replacement for the project.description field and we want the 
    // user to add a description in the field above, we remove the property
    // from the list.
    if (strcmp($prop->name, "Project Description")==0) {
      continue;
    }
    $properties[$prop->cvterm_id] = $prop->name;
  }
  
  // we want to exclude the project description from being loaded as a stored property 
  // because we want to use the property to replace the project.description field as it is
  // only 255 characters which isn't large enough. We don't want the user to set it 
  // as a property even though it will be stored as a property. 
  $exclude = array('Project Description');
  $include = array();
  $instructions = t('To add additional properties to the drop down. ' . l("Add terms to the project_property vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");
  tripal_core_properties_form($form, $form_state, 'projectprop', 'project_id', 'project_property',
    $properties, $project_id, $exclude, $include, $instructions, 'Properties');
  
  return $form;

}
/**
 *  validates submission of form when adding or updating a project node
 *
 * @ingroup tripal_project
 */
function chado_project_validate($node, $form, &$form_state) {
  
  $node->title = trim($node->title);
  $node->description = trim($node->description);
  
  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }
  
  // we are syncing if we do not have a node ID but we do have a project_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'project_id') and $node->project_id != 0) {
    return;
  }
  
  $project = 0;
  // check to make sure the name on the project is unique
  // before we try to insert into chado.
  if ($node->project_id) {
    $sql = "SELECT * FROM {project} WHERE name = :name AND NOT project_id = :project_id";
    $project = chado_query($sql, array(':name' => $node->title, ':project_id' => $node->project_id))->fetchObject();
  }
  else {
    $sql = "SELECT * FROM {project} WHERE name = :name";
    $project = chado_query($sql, array(':name' => $node->title))->fetchObject();
  }
  if ($project) {
    form_set_error('title', t('The unique project name already exists. Please choose another'));
  }
}
/**
 * Implementation of hook_insert().
 *
 *  @parm $node
 *    Then node that has the information stored within, accessed given the nid
 *
 *
 * @ingroup tripal_project
 */
function chado_project_insert($node) {

  $node->title = trim($node->title);
  $node->description = trim($node->description);
  
  // if there is an project_id in the $node object then this must be a sync so
  // we can skip adding the project as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'project_id')) {
    $values = array(
      'name' => $node->title,
      'description' => '',
    );
    $project = tripal_core_chado_insert('project', $values);
    if (!$project) {
      drupal_set_message(t('Unable to add project.', 'warning'));
      watchdog('tripal_project', 'Insert project: Unable to create project where values:%values',
      array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
      return;
    }
    $project_id = $project['project_id'];
    
    // now add in the properties
    $properties = tripal_core_properties_form_retreive($node, 'project_property');
    foreach ($properties as $property => $elements) {
      foreach ($elements as $rank => $value) {
    
        $status = tripal_project_insert_property($project_id, $property, $value, FALSE, 'project_property');
        if (!$status) {
          drupal_set_message("Error cannot add property: $property", "error");
          watchdog('t_project', "Error cannot add property: %prop",
          array('%property' => $property), WATCHDOG_ERROR);
        }
      }
    }
    
    // add in the description as a separate property
    tripal_project_insert_property($project_id, 'Project Description', $node->description, FALSE);
  }
  else {
    $project_id = $node->project_id;
  }

  // Make sure the entry for this project doesn't already exist in the
  // chado_project table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_for_node('project', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->project_id = $project_id;
    drupal_write_record('chado_project', $record);
  }
}

/**
 *
 * Implementation of hook_delete().
 *
 * @param $node
 * The node which is to be deleted, only chado project and chado_project need to be dealt with
 * since the drupal node is deleted automagically
 *
 *
 * @ingroup tripal_project
 */
function chado_project_delete($node) {

  $project_id = chado_get_id_for_node('project', $node->nid);

  // if we don't have a project id for this node then this isn't a node of
  // type chado_project or the entry in the chado_project table was lost.
  if (!$project_id) {
    return;
  }

  // Remove data from {chado_project}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_project} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vod";
  db_query($sql_del,  array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del,  array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from project and projectprop tables of chado database as well
  chado_query("DELETE FROM {projectprop} WHERE project_id = :project_id", array(':project_id' => $project_id));
  chado_query("DELETE FROM {project} WHERE project_id = :project_id", array(':project_id' => $project_id));
}

/**
 * Implements hook_update().
 *
 * @param $node
 *  The node which is to have its containing information updated when the user modifies information
 *  pertaining to the specific project
 *
 *
 * @ingroup tripal_project
 */
function chado_project_update($node) {
  
  $node->title = trim($node->title);
  $node->description = trim($node->description);

  // update the project and the description
  $project_id = chado_get_id_for_node('project', $node->nid) ;
  $match = array('project_id' => $project_id);
  $values = array(
    'name' => $node->title,
    'description' => '',
  );
  $status = tripal_core_chado_update('project', $match, $values);
  if (!$status) {
    drupal_set_message(t('Unable to update project.', 'warning'));
    watchdog('tripal_project', 'Update project: Unable to update project where values: %values',
    array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
  }
  
  // now add in the properties by first removing any the project
  // already has and adding the ones we have
  tripal_core_chado_delete('projectprop', array('project_id' => $project_id));
  $properties = tripal_core_properties_form_retreive($node, 'project_property');
  foreach ($properties as $property => $elements) {
    foreach ($elements as $rank => $value) {
      $status = tripal_project_insert_property($project_id, $property, $value, FALSE, 'project_property');
      if (!$status) {
        drupal_set_message("Error cannot add property: '$property'", "error");
        watchdog('t_project', "Error cannot add property: '%prop'",
        array('%prop' => $property), WATCHDOG_ERROR);
      }
    }
  }
  
  // add the project description as a property
  tripal_project_update_property($project_id, 'Project Description', $node->description, 1);
}

/**
 * Implementation of node_load().
 *
 * @param $node
 *   The node that is to have its containing information loaded
 *
 * @return $node
 *   The node, containing the loaded project with the current nid
 *
 *
 * @ingroup tripal_project
 */
function chado_project_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // get the feature details from chado
    $project_id = chado_get_id_for_node('project', $node->nid);
  
    $values = array('project_id' => $project_id);
    $project = tripal_core_generate_chado_var('project', $values);
  
    $nodes[$nid]->project = $project;
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
 * @ingroup tripal_project
 */
function chado_project_node_access($node, $op, $account) {

  if ($op == 'create') {
    if (!user_access('create chado_projects content', $account)) {
      return FALSE;
    }
    return TRUE;
  }
  if ($op == 'update') {
    if (!user_access('edit chado_projects content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'delete') {
    if (!user_access('delete chado_projects content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'view') {
    if (!user_access('access chado_projects content', $account)) {
      return FALSE;
    }
  }
  return NULL;
}