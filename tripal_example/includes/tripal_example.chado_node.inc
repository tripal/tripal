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
  //  'name'        => t('Example'),
  //  'base'        => 'chado_example',
  //  'description' => t('A example from the chado database'),
  //  'has_title'   => TRUE,
  //  'locked'      => TRUE
  // This section of the node type array specifies how Tripal will sync the node
  // types with data in Chado. When Drupal creates a node it has no way of
  // coordinating which node belongs to which record in Chado. Therefore,
  // Tripal maintains tables in the Drupal schema that maps Drupal nodes
  // to recrords in Chado.  Syncing is the process of creating Drupal nodes
  // and linking them to the appropriate record.
  //  'chado_node_api' => array(
  //    'base_table' => 'example',       // the base table name (e.g. feature, example, contact)
  //    'hook_prefix' => 'chado_example',// the node type hook prefix
  //    'record_type_title' => array(
  //      'singular' => t('Library'),    // how to refer to the record
  //      'plural' => t('Libraries')     // how to refer to the record in plurals
  //    ),
  //    'sync_filters' => array(
  //      'type_id' => TRUE,             // if the record has a type_id set to TRUE
  //      'organism_id' => TRUE          // if the record has an organism_id set to TRUE
  //    ),
  //  )
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
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  
  if($node_type == 'chado_example') {
    if ($op == 'create') {
      if (!user_access('create chado_example content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_example content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_example content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_example content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
  }
  return NODE_ACCESS_IGNORE;
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
    // $example = chado_expand_var($example, 'field', 'example.residues');
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
  
  // NOTE: if the table has a 'type_id' column, be sure to use the 
  // tripal_get_default_cv('example', 'type_id') to get the site's 
  // default vocabulary.
  


  // PROPERTIES FORM
  //---------------------------------------------
  // If there is a exampleprop table and you want to allow users to add/remove entries
  // from it through your node form then add this section to your own node form
  $prop_cv = tripal_get_default_cv('exampleprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'property_table' => 'exampleprop',      // the name of the prop table
    'chado_id' => $example_id,              // the value of example_id for this record
    'cv_id' => $cv_id                       // the cv.cv_id of the cv governing exampleprop.type_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  // If there is a example_dbxref table and you want to allow users to add/remove entries
  // from it through your node form then add this section to your own node form
  $details = array(
    'linking_table' => 'example_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'example_id',   // the name of the key in your base chado table
    'base_key_value' => $example_id       // the value of example_id for this record
  );
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  // If there is a example_relationship table and you want to allow users to add/remove entries
  // from it through your node form then add this section to your own node form
  $rels_cv = tripal_get_default_cv('example_relationship', 'type_id');
  $cv_id = $rels_cv ? $rels_cv->cv_id : NULL;
  $details = array(
    'relationship_table' => 'example_relationship', // the name of the _relationship table
    'base_table' => 'example',                      // the name of your chado base table
    'base_foreign_key' => 'example_id',             // the name of the key in your base chado table
    'base_key_value' => $example_id,                // the value of example_id for this record
    'nodetype' => 'example',                        // the human-readable name of your node type
    'cv_id' => $cv_id                               // the cv.cv_id of the cv governing example_relationship.type_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_relationships($form, $form_state, $details);

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

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a example_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'example_id') and $node->example_id != 0) {
    return;
  }

  // be sure to always trim text fields
  // $node->uniquename   = trim($node->uniquename);
  
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

    // ADD TO CHADO

    // * Example Table *
    // perform the insert using the tripal_core_chado_insert function();
    //$values = array(
    //  'uniquename' => $node->uniquename,
    //  'residues' => $residues,
    //);
    //$example = chado_select_record('example', array('*'), $values);
    //if (!$example) {
    //  drupal_set_message(t('Unable to add example.'), 'warning');
    //  tripal_report_error('tripal_example', TRIPAL_WARNING, 'Insert example: Unable to create example where values: %values',
    //    array('%values' => print_r($values, TRUE)));
    //  return;
    //}

    // get the example_id for linking Drupal node with Chado data
    // $example_id = $example->example_id;

    // Only add to other chado tables if the base record was inserted properly
    if ($example_id > 0) {

      // * Properties Form *
      // If you implemented the properties form in chado_example_form then you need to
      // handle inserting these properties into your chado prop table.
      // $details = array(
      //   'property_table' => 'exampleprop',   // the name of the prop table
      //   'base_table' => 'example',           // the name of your chado base table
      //   'foreignkey_name' => 'example_id',   // the name of the key in your base table
      //   'foreignkey_value' => $example_id    // the value of the example_id key
      // );
      // chado_update_node_form_properties($node, $details);

      // * Additional DBxrefs Form *
      // If you implemented the dbxrefs form in chado_example_form then you need to
      // handle inserting these database references into your chado _dbxref table.
      // $details = array(
      //   'linking_table' => 'example_dbxref',   // the name of your _dbxref table
      //   'foreignkey_name' => 'example_id',     // the name of the key in your base table
      //   'foreignkey_value' => $example_id      // the value of the example_id key
      // );
      // chado_update_node_form_dbxrefs($node, $details);

      // * Relationships Form *
      // If you implemented the relationships form in chado_example_form then you need to
      // handle inserting these relationships into your chado _relationship table.
      // $details = array(
      //   'relationship_table' => 'example_relationship',  // name of the _relationship table
      //   'foreignkey_value' => $example_id                // value of the example_id key
      // );
      // chado_update_node_form_relationships($node, $details);
    }
  }
  else {
    // the node has an example_id so get it for linking Drupal node with Chado data
    // $example_id = $node->example_id;
  }

  // Make sure the entry for this example doesn't already exist in the
  // chado_example table if it doesn't exist then we want to add it.
  // $check_org_id = chado_get_id_from_nid('example', $node->nid);
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

  // use the chado_update_record() function to update the record
  //$match = array(
  //'example_id' => $example_id,
  //);
  //$values = array(
  //  'uniquename' => $node->uniquename,
  //);
  //$options = array('return_record' => TRUE);
  //$status = chado_update_record('example', $match, $values, $options);

  //if (!$status) {
  //  drupal_set_message(t('Unable to update example.'), 'warning');
  //  tripal_report_error('tripal_example', TRIPAL_WARNING, 'Update example: Unable to update example where values: %values',
  //    array('%values' => print_r($values, TRUE)));
  //}

  // * Properties Form *
  // If you implemented the properties form in chado_example_form then you need to
  // handle updating these properties into your chado prop table.
  // $details = array(
  //   'property_table' => 'exampleprop',   // the name of the prop table
  //   'base_table' => 'example',           // the name of your chado base table
  //   'foreignkey_name' => 'example_id',   // the name of the key in your base table
  //   'foreignkey_value' => $example_id    // the value of the example_id key
  // );
  // chado_update_node_form_properties($node, $details);

  // * Additional DBxrefs Form *
  // If you implemented the dbxrefs form in chado_example_form then you need to
  // handle updating these database references into your chado _dbxref table.
  // $details = array(
  //   'linking_table' => 'example_dbxref',   // the name of your _dbxref table
  //   'foreignkey_name' => 'example_id',     // the name of the key in your base table
  //   'foreignkey_value' => $example_id      // the value of the example_id key
  // );
  // chado_update_node_form_dbxrefs($node, $details);

  // * Relationships Form *
  // If you implemented the relationships form in chado_example_form then you need to
  // handle updating these relationships into your chado _relationship table.
  // $details = array(
  //   'relationship_table' => 'example_relationship',  // name of the _relationship table
  //   'foreignkey_value' => $example_id                // value of the example_id key
  // );
  // chado_update_node_form_relationships($node, $details);

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
  //$example_id  = chado_get_id_from_nid('example', $node->nid);

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
    //$example_id = chado_get_id_from_nid('example', $nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    //if (!$example_id) {
    //  continue;
    //}
    
    // build the example variable by using the chado_generate_var() function
    //$values = array('example_id' => $example_id);
    //$example = chado_generate_var('example', $values);

    // for fields in the table that are of type 'text' you may want to include those
    // by default, the tripal_core_generate_chado_var does not include text fields as
    // they may be very large and including a large text field can slow the page load.
    // If you know a text field will never be large and it is important for the
    // other functions that will see the node to have access to a field you can
    // include it here using the chado_expand_var() function.  In most
    // cases it is probably best to let the end-user decide if text fields should
    // be included by using this function in the templates.
    //$example = chado_expand_var($example, 'field', 'example.residues');

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

  /*
  // set the node title
  switch ($node->type) {
    case 'chado_example':
      // for a form submission the 'examplename' field will be set,
      // for a sync, we must pull from the example object
      if (property_exists($node, 'examplename')) {
        // set the title
        $node->title = $node->examplename;
      }
      else if (property_exists($node, 'example')) {
        $node->title = $node->example->name;
      }
      break;
  }
  */
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
        //  '#markup' => theme('tripal_example_base', array('node' => $node)),
        //  '#tripal_toc_id'    => 'base',
        //  '#tripal_toc_title' => 'Overview',
        //  '#weight' => -100,
        //);
        // we can add other templates as well.
        //$node->content['tripal_example_properties'] = array(
        //  '#markup' => theme('tripal_example_properties', array('node' => $node)),
        //  '#tripal_toc_id'    => 'properties',
        //  '#tripal_toc_title' => 'Properties',
        //);
        //$node->content['tripal_example_publications'] = array(
        //  '#markup' => theme('tripal_example_publications', array('node' => $node)),
        //  '#tripal_toc_id'    => 'publications',
        //  '#tripal_toc_title' => 'Publications',
        //);
        //$node->content['tripal_example_references'] = array(
        //  '#markup' => theme('tripal_example_references', array('node' => $node)),
        //  '#tripal_toc_id'    => 'references',
        //  '#tripal_toc_title' => 'Cross References',
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
