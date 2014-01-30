<?php

/**
 * @file
 * This file should contain all Drupal hooks for interacting with nodes. 
 *
 */

/**
 *  Implementation of hook_node_info().  This hook provides information to drupal 
 *  about any node types that are being created by this module. If your module
 *  does not create any node types then this function is not required.
 *
 * @ingroup tripal_example
 */
function tripal_example_node_info() {
  $nodes = array();

  //$nodes['chado_example'] = array(
  //  'name'        => t('example'),
  //  'base'        => 'chado_example',
  //  'description' => t('A example from the chado database'),
  //  'title_label' => t('example'),
  //  'has_title'   => FALSE,
  //  'has_body'    => FALSE,
  //  'locked'      => TRUE
  //);
  
  return $nodes;
}

/**
 * Implement hook_access().  This hook provides instructions to
 * drupal for which users can access the custom content types
 * created in the function above.  The available permissions
 * are set in the chado_example_permissions() hook in the
 * tripal_example.module file.  This hook is not needed
 * if no node types were defined in the hook_node_info() hook.
 * 
 * @return
 * This function should return null if it does not specificially
 * deny access. This allows for other mechanisms to to deny
 * or reject access. If the return value is TRUE then access
 * is granted regardless of any other rules that might be implemented
 * by other modules.  
 */
function chado_example_node_access($node, $op, $account) {
  if ($op == 'create') {
    if (!user_access('create chado_example content', $account)) {
      return FALSE;
    }
    return TRUE;
  }
  if ($op == 'update') {
    if (!user_access('edit chado_example content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'delete') {
    if (!user_access('delete chado_example content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'view') {
    if (!user_access('access chado_example content', $account)) {
      return FALSE;
    }
  }
  return NULL;
}

/**
 * Implementation of hook_form() when a node type of chado_example is defined.
 * If a node type is not defined then this function is not needed. The table
 * name in chado for this example module is named 'example' so there is a
 * corresponding example_id in that table (similar to feature.feature_id, 
 * contact.contact_id, etc).
 *
 * @ingroup tripal_example
 */
function chado_example_form($node, &$form_state) {

  $form = array();

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing example
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults


  // if we are editing an existing node then the example is already part of the node
  if (property_exists($node, 'example')) {
    // $example = $node->example;
    // $example = tripal_core_expand_chado_vars($example, 'field', 'example.residues');
    // $example_id   = $example->example_id;
    // $uniquename   = $example->uniquename;

    // keep track of the example id
    //$form['example_id'] = array(
    //  '#type' => 'value',
    //  '#value' => $example_id,
    //);
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    // $uniquename   = $form_state['values']['uniquename'];

  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    // $uniquename   = $form_state['input']['uniquename'];

  }
  
  // add form elements here.
  

  // return the form
  return $form;
}

/**
 * Implementation of hook_validate
 *
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both drupal and chado
 *   CASE B: Synchronizing a node from chado to drupal
 *   CASE C: Inserting a new node that exists in niether drupal nor chado
 *
 * @param $node
 *
 *
 * @ingroup tripal_example
 */
function chado_example_validate($node, $form, &$form_state) {
  // be sure to always trim text fields
  // $node->uniquename   = trim($node->uniquename);

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a example_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'example_id') and $node->example_id != 0) {
    return;
  }

  // Validating for an update. If the 'nid' property is present in the node then
  // this is an update and validation can be different for updates
  if (property_exists($node, 'nid')) {

    // if there is a problem with a field then you can set an error on the form
    // form_set_error('uniquename', t("example update cannot proceed. The example name '$node->uniquename' is not unique for this organism. Please provide a unique name for this example."));
  }
  // Validating for an insert
  else {

    // if there is a problem with a field then you can set an error on the form
    // form_set_error('uniquename', t("example insert cannot proceed. The example name '$node->uniquename' already exists for this organism. Please provide a unique name for this example."));
  }
}

/**
 *  Implementation of hook_insert(). This function is called after the
 *  node is inserted into the database. We need it so that we can insert
 *  appropriate fields as provided by the user into the database. And so that
 *  we can link the new Drupal node to the data in Chado via the chado_example
 *  linking table. We can get to this function also during "syncing". 
 *  With syncing, however, the data already exists in Chado and we do not want 
 *  to try to re-add it. But we do need to add an entry to the chado_example table
 *  to link the Drupal node with the data in the 'example' table of Chado. 
 *  
 *  This function is not required if the hook_node_info() does not define
 *  any custom node types.
 *
 * @ingroup tripal_example
 */
function chado_example_insert($node) {
  // be sure to always trim text fields
  //$node->uniquename   = trim($node->uniquename);

  // if there is an example_id in the $node object then this must be a sync so
  // we can skip adding the example as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'example_id')) {
    
    // perform the insert using the tripal_core_chado_insert function();
    //$values = array(
    //  'uniquename' => $node->uniquename,
    //  'residues' => $residues,
    //);
    //$example = tripal_core_chado_select('example', array('*'), $values);
    //if (!$example) {
    //  drupal_set_message(t('Unable to add example.'), 'warning');
    //  watchdog('tripal_example', 'Insert example: Unable to create example where values: %values',
    //  array('%values' => print_r($values, TRUE)), WATCHDOG_WARNING);
    //  return;
    //}
    
    // get the example_id for linking Drupal node with Chado data
    // $example_id = $example->example_id;
  }
  else {
    // the node has an example_id so get it for linking Drupal node with Chado data
    // $example_id = $node->example_id;
  }

  // Make sure the entry for this example doesn't already exist in the
  // chado_example table if it doesn't exist then we want to add it.
  // $check_org_id = chado_get_id_for_node('example', $node->nid);
  //if (!$check_org_id) {
  //  $record = new stdClass();
  //  $record->nid = $node->nid;
  //  $record->vid = $node->vid;
  //  $record->example_id = $example_id;
  //  drupal_write_record('chado_example', $record);
  //}
}

/**
 * Implementation of hook_update().  This function runs after the
 * node has been inserted into the Drupal schema and allows us to
 * update the record in Chado.
 * 
 *  This function is not required if the hook_node_info() does not define
 *  any custom node types.
 *
 * @ingroup tripal_example
 */
function chado_example_update($node) {
  // be sure to always trim text fields
  // $node->uniquename   = trim($node->uniquename);

  // use the tripal_core_chado_update() function to update the record
  //$match = array(
  //'example_id' => $example_id,
  //);
  //$values = array(
  //  'uniquename' => $node->uniquename,
  //);
  //$options = array('return_record' => TRUE);
  //$status = tripal_core_chado_update('example', $match, $values, $options);

  //if (!$status) {
  //  drupal_set_message(t('Unable to update example.'), 'warning');
  //  watchdog('tripal_example', 'Update example: Unable to update example where values: %values',
  //    array('%values' => print_r($values, TRUE)), WATCHDOG_WARNING);
  //}

}
/**
 * Implementation of hook_delete().  This function runs after the 
 * node has been deleted from the Drupal schema and allows us to 
 * delete the corresponding recrod in Chado.
 *
 * This function is not required if the hook_node_info() does not define
 * any custom node types.
 *  
 * @ingroup tripal_example
 */
function chado_example_delete($node) {

  // get the example id from the node
  //$example_id  = chado_get_id_for_node('example', $node->nid);

  // if we don't have a example id for this node then this isn't a node of
  // type chado_example or the entry in the chado_example table was lost.
  if (!$example_id) {
    return;
  }

  // remove the entry in the chado_exapmle table linking the deleted
  // Drupal node with the data in chado
  // $sql_del = "DELETE FROM {chado_example} WHERE nid = :nid AND vid = :vid";
  // db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from example tables of chado database.  This will
  // cause a cascade delete and remove all data in referencing tables
  // for this example
  // chado_query("DELETE FROM {example} WHERE example_id = :example_id", array(':example_id' => $example_id));

  // inform the user that the data was deleted
  drupal_set_message(t("The example and all associated data were removed from Chado"));

}

/**
 * Implementation of hook_load().  This function is necessary to load
 * into the $node object the fields of the table form Chado. For example
 * for the feature table, the chado_feature_load() function adds in
 * a feature object which contains all of the fields and sub objects
 * for data in tables with foreign key relationships.
 *  
 * This function is not required if the hook_node_info() does not define
 * any custom node types.
 *
 * @ingroup tripal_example
 */
function chado_example_load($nodes) {

  // there may be multiple nodes that get passed in so we have to iterate through
  // them all
  foreach ($nodes as $nid => $node) {
    // find the example and add in the details
    //$example_id = chado_get_id_for_node('example', $nid);

    // build the example variable by using the tripal_core_generate_chado_var() function
    //$values = array('example_id' => $example_id);
    //$example = tripal_core_generate_chado_var('example', $values);
    
    // for fields in the table that are of type 'text' you may want to include those
    // by default, the tripal_core_generate_chado_var does not include text fields as
    // they may be very large and including a large text field can slow the page load.
    // If you know a text field will never be large and it is important for the 
    // other functions that will see the node to have access to a field you can 
    // include it here using the tripal_core_expand_chado_vars() function.  In most
    // cases it is probably best to let the end-user decide if text fields should
    // be included by using this function in the templates.
    //$example = tripal_core_expand_chado_vars($example, 'field', 'example.residues');
        
    // add the new example object to this node.
    //$nodes[$nid]->example = $example;
  }
}

/**
 * Implementation of hook_node_presave().  This node is useful for
 * making changes to the node prior to it being saved to the database.
 * One useful case for this is to set the title of a node. In some cases
 * such as for the organism module, the title will be set depending on
 * what genus and species is provided. This hook can allow the title to
 * be set using user supplied data before the node is saved.  In practice
 * any change can be made to any fields in the node object.
 * 
 * This function is not required. You probably won't need it if you
 * don't define a custom node type in the hook_node_info() function. But
 * it is node type agnostic, so you can use this function to change the
 * contents of any node regardless of it's type.
 * 
 * @ingroup tripal_example
 */
function tripal_example_node_presave($node) {

  // set the title to ensure it is always unique
  //switch ($node->type) {
  //  case 'chado_example':
  //    $node->title = $node->uniquename;
  //    break;
  //}
}

/**
 * Implementation of hook node_insert().  This function is used 
 * after any a node is inserted into the database.  It is different
 * from the hook_insert() function above in that it is called after
 * any node is saved, regardlesss of it's type. This function is useful
 * for making changes to the database after a node is inserted when you
 * can't edit the hook_insert() function of a node not defined by this
 * module, or to access values of a node when have not yet been saved.
 * An example comes from the tripal_feature module where the URL alias
 * of a node cannot be set in the hook_insert() function. Therefore
 * the tripal_feature module uses this function to set the url path
 * of a newly inserted feature node.
 * 
 * This function is not required. You probably won't need it if you
 * don't define a custom node type in the hook_node_info() function. But
 * it is node type agnostic, so you can use this function to do any
 * activity after insert of a node.
 * 
 * @ingroup tripal_example
 */
function tripal_example_node_insert($node) {

  // set the URL path after inserting.  We do it here because we do not know the 
  // example_id in the presave and cannot do it in the hook_insert()
  //switch ($node->type) {
  //  case 'chado_example':
  //    if (!$node->example_id) {
  //      $sql = "SELECT * FROM {chado_example} WHERE nid = :nid";
  //      $chado_example = db_query($sql, array(':nid' => $node->nid))->fetchObject();
  //      $node->example_id = $chado_example->example_id;
  //    }

  //    // remove any previous alias
  //    db_query("DELETE FROM {url_alias} WHERE source = :source", array(':source' => "node/$node->nid"));

  //    // set the URL for this example page
  //    $url_alias = tripal_example_get_example_url($node);
  //    $path_alias = array("source" => "node/$node->nid", "alias" => $url_alias);
  //    path_save($path_alias);
  //    break;
  //}
}
/**
 * Implementation of hook node_update().  This function is used 
 * after any a node is updated in the database.  It is different
 * from the hook_update() function above in that it is called after
 * any node is updated, regardlesss of it's type. This function is useful
 * for making changes to the database after a node is updated when you
 * can't perform changes in the hook_upate() function of a node not defined by this
 * module, or to access values of a node when have not yet been updated.
 * An example comes from the tripal_feature module where the URL alias
 * of a node cannot be set in the hook_update() function. Therefore
 * the tripal_feature module uses this function to reset the url path
 * of an updated feature node.
 * 
 * This function is not required. You probably won't need it if you
 * don't define a custom node type in the hook_node_info() function. But
 * it is node type agnostic, so you can use this function to do any
 * activity after insert of a node.
 * 
 */
function tripal_example_node_update($node) {

  // add items to other nodes, build index and search results
  switch ($node->type) {
    case 'chado_example':
      // remove any previous alias
      //db_query("DELETE FROM {url_alias} WHERE source = :source", array(':source' => "node/$node->nid"));

      // set the URL for this example page
      //$url_alias = tripal_example_get_example_url($node);
      //$path_alias = array("source" => "node/$node->nid", "alias" => $url_alias);
      //path_save($path_alias);
      break;
  }
}
/**
 * Implementation of hook_node_view().  This function allows you to
 * add custom content to any node page.  It is node type agnostic.
 * Here we typically use it to add content to our custom node type or
 * to other Tripal node types. Typically for Tripal, a content "block" 
 * (e.g. feature properties, feature dbxref, feature pub) has a corresponding
 * template file.  Those template files are first defined to Drupal using 
 * the hook_theme() function defined in the tripal_example.module file.  Here
 * we can add items to a node's content by calling those templates as needed. 
 * 
 * @ingroup tripal_example
 */
function tripal_example_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_example':
      // there are different ways a node can be viewed. Primarily Tripal
      // supports full page view and teaser view.
      if ($view_mode == 'full') {
        // there is always a base template.  This is the template that 
        // is first shown when the example node type is first displayed
        //$node->content['tripal_example_base'] = array(
        //  '#value' => theme('tripal_example_base', array('node' => $node)),
        //);
         
        // we can add other templates as well.
        //$node->content['tripal_example_properties'] = array(
        //  '#value' => theme('tripal_example_properties', array('node' => $node)),
        //);
        //$node->content['tripal_example_publications'] = array(
        //  '#value' => theme('tripal_example_publications', array('node' => $node)),
        //);
        //$node->content['tripal_example_references'] = array(
        //  '#value' => theme('tripal_example_references', array('node' => $node)),
        //);
      }
      // set the content for the teaser view
      if ($view_mode == 'teaser') {
        //$node->content['tripal_example_teaser'] = array(
        //  '#value' => theme('tripal_example_teaser', array('node' => $node)),
        //);
      }
      break;
    // you can add custom content to any tripal node type by adding
    // content to the node in the same way as above.
    case 'chado_organism':
      break;
    case 'chado_library':
      break;
    case 'chado_stock':
      break;
    case 'chado_analysis':
      break;
    // ... etc
  }
}
