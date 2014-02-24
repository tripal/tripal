<?php
/**
 * @file
 * Implement the project node content type
 */

/**
 * Implementation of hook_node_info().
 *
 * This node_info, is a simple node that describes the functionallity of the module. It specifies
 * that the title(Project Name) and body(Description) set to true so that they information can be
 * entered
 *
 * @ingroup tripal_project
 */
function tripal_project_node_info() {
  return array(
    'chado_project' => array(
      'name'        => t('Project'),
      'base'        => 'chado_project',
      'description' => t('A project from the Chado database'),
      'has_title'   => TRUE,
      'locked'      => TRUE,
      'chado_node_api' => array(
        'base_table' => 'project',
        'hook_prefix' => 'chado_project',
        'record_type_title' => array(
          'singular' => t('Project'),
          'plural' => t('Projects')
        ),
        'sync_filters' => array(
          'type_id' => FALSE,
          'organism_id' => FALSE
        ),
      ),
    ),
  );
}

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
    if (property_exists($node, 'body')) {
      $description = $node->body;
    }
    else {
      $description = $project->description;
    }
    if (!$description) {
      $projectprop = tripal_project_get_property($project->project_id, 'Project Description');
      $description = $projectprop->value;
    }

    $title = $project->name;
    $project_id = $project->project_id;

    // keep track of the project id if we have.  If we do have one then
    // this is an update as opposed to an insert.
    $form['project_id'] = array(
      '#type' => 'value',
      '#value' => $project_id,
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
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Project Description'),
    '#description'   => t('A brief description of the project'),
    '#required'      => TRUE,
    '#default_value' => $description,
  );

  // Properties Form
  // ----------------------------------
  $select_options = array();
  $prop_cv = tripal_get_default_cv('projectprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  if ($prop_cv = 'project_property') {
    // if this is the project_property CV then
    // we want to exclude the project description from being loaded as a stored property
    // because we want to use the property to replace the project.description field as it is
    // only 255 characters which isn't large enough. We don't want the user to set it
    // as a property even though it will be stored as a property.
    $cv_result = chado_select_record('cv',array('cv_id'),array('name' => 'project_property'));
    $cv_id = $cv_result[0]->cv_id;
    $select_options = tripal_cv_get_cvterm_options($cv_id);
    $descrip_id = array_search('Project Description', $select_options);
    unset($select_options[$descrip_id]);
  }

  $instructions = t('To add properties to the drop down list, you must ' . l("add terms to the project_property vocabulary", "admin/tripal/chado/tripal_cv/cvterm/add") . ".");
  $details = array(
    'property_table' => 'projectprop',
    'chado_id' => $project_id,
    'cv_id' => $cv_id,
    'additional_instructions' => $instructions,
    'select_options' => $select_options
  );
  chado_add_node_form_properties($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  $relationship_cv = tripal_get_default_cv('project_relationship', 'type_id');
  $cv_id = $relationship_cv ? $relationship_cv->cv_id : NULL;
  $details = array(
    'relationship_table' => 'project_relationship', // the name of the _relationship table
    'base_table' => 'project',                      // the name of your chado base table
    'base_foreign_key' => 'project_id',             // the name of the key in your base chado table
    'base_key_value' => $project_id,                // the value of example_id for this record
    'nodetype' => 'project',                        // the human-readable name of your node type
    'cv_id' => $cv_id,                              // the cv.cv_id of the cv governing example_relationship.type_id
    'base_name_field' => 'name',                    // the base table field you want to be used as the name
    'subject_field_name' => 'subject_project_id',
    'object_field_name' => 'object_project_id',
    'select_options' => $select_options
  );
  // Adds the form elements to your current form
  chado_add_node_form_relationships($form, $form_state, $details);

  return $form;

}

/**
 * Implements hook_validate().
 * Validates submission of form when adding or updating a project node
 *
 * @ingroup tripal_project
 */
function chado_project_validate($node, $form, &$form_state) {

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a project_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'project_id') and $node->project_id != 0) {
    return;
  }

  // trim white space from text fields
  $node->title = trim($node->title);
  $node->description = trim($node->description);
  
  $project = 0;
  // check to make sure the name on the project is unique
  // before we try to insert into chado.
  if (property_exists($node, 'project_id')) {
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
    $project = chado_insert_record('project', $values);
    if (!$project) {
      drupal_set_message(t('Unable to add project.', 'warning'));
      watchdog('tripal_project', 'Insert project: Unable to create project where values:%values',
      array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
      return;
    }
    $project_id = $project['project_id'];

    // * Properties Form *
    // Add the description property
    $properties = chado_retrieve_node_form_properties($node);
    $descrip_id = tripal_cv_get_cvterm_by_name('Project Description', NULL, 'project_property');
    $properties[$descrip_id->cvterm_id][0] = $node->description;

    $details = array(
      'property_table' => 'projectprop',
      'base_table' => 'project',
      'foreignkey_name' => 'project_id',
      'foreignkey_value' => $project_id
    );
    chado_update_node_form_properties($node, $details, $properties);

    // * Relationships Form *
    $details = array(
      'relationship_table' => 'project_relationship',  // name of the _relationship table
      'foreignkey_value' => $project_id                // value of the example_id key
    );
    chado_update_node_form_relationships($node, $details);

  }
  else {
    $project_id = $node->project_id;
  }

  // Make sure the entry for this project doesn't already exist in the
  // chado_project table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_from_nid('project', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->project_id = $project_id;
    drupal_write_record('chado_project', $record);
  }
}

/**
 * Implementation of hook_delete().
 *
 * @param $node
 *  The node which is to be deleted, only chado project and chado_project need to be dealt with
 *  since the drupal node is deleted automagically
 *
 * @ingroup tripal_project
 */
function chado_project_delete($node) {

  $project_id = chado_get_id_from_nid('project', $node->nid);

  // if we don't have a project id for this node then this isn't a node of
  // type chado_project or the entry in the chado_project table was lost.
  if (!$project_id) {
    return;
  }

  // Remove data from {chado_project}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_project} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
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
 * @ingroup tripal_project
 */
function chado_project_update($node) {

  $node->title = trim($node->title);
  $node->description = trim($node->description);

  // update the project and the description
  $project_id = chado_get_id_from_nid('project', $node->nid) ;
  $match = array('project_id' => $project_id);
  $values = array(
    'name' => $node->title,
    'description' => '',
  );
  $status = chado_update_record('project', $match, $values);
  if (!$status) {
    drupal_set_message(t('Unable to update project.', 'warning'));
    watchdog('tripal_project', 'Update project: Unable to update project where values: %values',
    array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
  }

  // * Properties Form *
  // Add the description property
  $properties = chado_retrieve_node_form_properties($node);
  $descrip_id = tripal_cv_get_cvterm_by_name('Project Description', NULL, 'project_property');
  $properties[$descrip_id->cvterm_id][0] = $node->description;

  $details = array(
    'property_table' => 'projectprop',
    'base_table' => 'project',
    'foreignkey_name' => 'project_id',
    'foreignkey_value' => $project_id
  );
  chado_update_node_form_properties($node, $details, $properties);

  // * Relationships Form *
  $details = array(
    'relationship_table' => 'project_relationship',  // name of the _relationship table
    'foreignkey_value' => $project_id                // value of the example_id key
  );
  chado_update_node_form_relationships($node, $details);
}

/**
 * Implementation of hook_load().
 *
 * @param $node
 *   The node that is to have its containing information loaded
 *
 * @ingroup tripal_project
 */
function chado_project_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // get the feature details from chado
    $project_id = chado_get_id_from_nid('project', $node->nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$project_id) {
      continue;
    }
    
    
    $values = array('project_id' => $project_id);
    $project = chado_generate_var('project', $values);

    $nodes[$nid]->project = $project;
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

  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'chado_project') {
    if ($op == 'create') {
      if (!user_access('create chado_project', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_project', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_project', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_project', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_node_view().
 *
 * @ingroup tripal_project
 */
function tripal_project_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_project':
      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_project_base'] = array(
          '#markup' => theme('tripal_project_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_project_contact'] = array(
          '#markup' => theme('tripal_project_contact', array('node' => $node)),
          '#tripal_toc_id'    => 'contacts',
          '#tripal_toc_title' => 'Contacts',
        );
        $node->content['tripal_project_properties'] = array(
          '#markup' => theme('tripal_project_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_project_publications'] = array(
          '#markup' => theme('tripal_project_publications', array('node' => $node)),
          '#tripal_toc_id'    => 'publications',
          '#tripal_toc_title' => 'Publications',
        );
        $node->content['tripal_project_relationships'] = array(
          '#markup' => theme('tripal_project_relationships', array('node' => $node)),
          '#tripal_toc_id'    => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_project_teaser'] = array(
          '#markup' => theme('tripal_project_teaser', array('node' => $node)),
        );
      }
      break;
  }
}
