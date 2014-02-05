<?php
/**
 * Implementation of hook_tripal_contact_node_info().
 *
 * This node_info, is a simple node that describes the functionallity of the module.
 *
 */
function tripal_contact_node_info() {

  return array(
    'chado_contact' => array(
      'name' => t('Contact'),
      'base' => 'chado_contact',
      'description' => t('A contact from the Chado database'),
      'title_label' => t('Article Title'),
      'body_label' => t('Abstract'),
      'has_title' => TRUE,
      'has_body' => FALSE,
      'chado_node_api' => array(
        'base_table' => 'contact',
        'hook_prefix' => 'chado_contact',
        'record_type_title' => array(
          'singular' => t('Contact'),
          'plural' => t('Contacts')
        ),
        'sync_filters' => array(
          'type_id' => FALSE,
          'organism_id' => FALSE
        ),
      )
    ),
  );
}
/**
 * Implementation of tripal_contact_form().
 *
 *
 *
 *  @parm $node
 *    The node that is created when the database is initialized
 *
 *  @parm $form_state
 *    The state of the form, that has the user entered information that is neccessary for, setting
 *    up the database tables for the contact
 *
 *  @return $form
 *    The information that was enterd allong with
 *
 */
function chado_contact_form(&$node, $form_state) {
  $form = array();
  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing contact
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $contact_id  = null;
  $type_id     = 0;
  $title       = '';
  $description = '';

  // if we are editing an existing node then the contact is already part of the node
  if (property_exists($node, 'contact')) {
    $contact = $node->contact;
    $contact_id = $contact->contact_id;

    // get form defaults
    $type_id     = $contact->type_id->cvterm_id;
    $title       = $contact->name;

    // get the contact default values.  When this module was first created
    // the contact description was incorrectly stored in the $node->body field.
    // It is better to store it in the Chado tables.  However, the 'description'
    // field of the contact table is only 255 characters.  So, we are going
    // to follow the same as the contact module and store the description in
    // the contactprop table and leave the contact.description field blank.
    // however, for backwards compatibitily, we check to see if the description
    // is in the $node->body field. If it is we'll use that.  When the node is
    // edited the text will be moved out of the body and into the contactprop
    // table where it should belong.
    $description = '';
    if (property_exists($node, 'body')) {
      $description = $node->body;
    }
    else {
      $description = $contact->description;
    }
    if (!$description) {
      $contactprop = tripal_contact_get_property($contact->contact_id, 'contact_description');
      $description = (isset($contactprop->value)) ? $contactprop->value : '';
    }

    // set the contact_id in the form
    $form['contact_id'] = array(
      '#type' => 'value',
      '#value' => $contact->contact_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $type_id     = $form_state['values']['type_id'];
    $title       = $form_state['values']['title'];
    $description = $form_state['values']['description'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $type_id     = $form_state['input']['type_id'];
    $title       = $form_state['input']['title'];
    $description = $form_state['input']['description'];
  }

  // get the contact types. These are those that are part of the tripal_contact
  // vocabulary and are children of the term 'Contact Type', so we need
  // to join on the cvtermpath table and select those with a distance of 1
  $sql = "
    SELECT CVTS.cvterm_id, CVTS.name
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
      INNER JOIN {cv} CV       ON CVTO.cv_id = CV.cv_id
    WHERE
      CV.name = 'tripal_contact' AND
      CVTO.name = 'Contact Type' AND
      CVTP.pathdistance = 1
    ORDER BY CVTS.name ASC
  ";
  $results = chado_query($sql);
  $contact_types = array(3723 => 'Person');
  while ($contact_type = $results->fetchObject()) {
    $contact_types[$contact_type->cvterm_id] = $contact_type->name;
    if (strcmp($contact_type->name, "Person") == 0 and !$type_id) {
      $type_id = $contact_type->cvterm_id;
    }
  }
  $form['type_id'] = array(
    '#type' => 'select',
    '#title' => t('Contact Type'),
    '#options' => $contact_types,
    '#required' => TRUE,
    '#default_value' => $type_id,
  );

  $form['title']= array(
    '#type'          => 'textfield',
    '#title'         => t('Contact Name'),
    '#description'   => t('Enter the name of this contact'),
    '#required'      => TRUE,
    '#default_value' => $title,
    '#maxlength'     => 255,
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Contact Description'),
    '#description'   => t('A brief description of the contact'),
    '#required'      => TRUE,
    '#default_value' => $description,
  );

  // Properties Form
  // ----------------------------------
  // Need to pass in our own select_options since we use cvtermpath to filter ours
  $select_options = array();
  $select_options[] = 'Select a Property';
  $sql = "
   SELECT CVTS.cvterm_id, CVTS.name
   FROM {cvtermpath} CVTP
     INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
     INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
     INNER JOIN {cv} CV       ON CVTO.cv_id = CV.cv_id
   WHERE
     CV.name = 'tripal_contact' AND
     NOT CVTO.name = 'Contact Type'
   ORDER BY CVTS.name ASC";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    // add all properties except the Citation. That property is set via the uniquename field
    if ($prop->name != 'Citation') {
      if (!isset($select_options[$prop->cvterm_id])) {
        $select_options[$prop->cvterm_id] = $prop->name;
      }
    }
  }

  $details = array(
    'property_table' => 'contactprop',
    'base_foreign_key' => 'contact_id',
    'base_key_value' => $contact_id,
    'cv_name' => 'tripal_contact',
    'select_options' => $select_options
  );
  chado_node_properties_form($form, $form_state, $details);

  // RELATIONSHIPS FORM
  //---------------------------------------------
  // We want to use the contact_relationship_types cv if there are any terms available
  // and if not, to default to the relationship ontology
  $cv_result = tripal_core_chado_select('cv',array('cv_id'),array('name' => 'contact_relationship_types'));
  $cv_id = $cv_result[0]->cv_id;
  $select_options = tripal_cv_get_cvterm_options($cv_id);
  if (empty($select_options)) {
    $cv_result = tripal_core_chado_select('cv',array('cv_id'),array('name' => 'relationship'));
    $cv_id = $cv_result[0]->cv_id;
    $select_options = tripal_cv_get_cvterm_options($cv_id);
  }
  // D7 @TODO: tell tripal admin's about this

  $details = array(
    'relationship_table' => 'contact_relationship', // the name of the _relationship table
    'base_table' => 'contact',                      // the name of your chado base table
    'base_foreign_key' => 'contact_id',             // the name of the key in your base chado table
    'base_key_value' => $contact_id,                // the value of example_id for this record
    'nodetype' => 'contact',                        // the human-readable name of your node type
    'cv_name' => 'contact_relationship_types',      // the cv.name of the cv governing example_relationship.type_id
    'base_name_field' => 'name',                    // the base table field you want to be used as the name
    'select_options' => $select_options
  );
  // Adds the form elements to your current form
  chado_node_relationships_form($form, $form_state, $details);

  return $form;
}

/**
 *  validates submission of form when adding or updating a contact node
 *
 * @ingroup tripal_contact
 */
function chado_contact_validate($node, $form, &$form_state) {
  // remove surrounding white-space on submitted values
  $node->title          = trim($node->title);
  $node->description    = trim($node->description);

  // if this is a delete then don't validate
  if($node->op == 'Delete') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a contact_id. We don't
  // need to validate during syncing so just skip it.
  if (is_null($node->nid) and property_exists($node, 'contact_id') and $node->contact_id != 0) {
    return;
  }

  // Validating for an update
  if (property_exists($node, 'nid')) {
    // get the existing node
    $values = array('contact_id' => $node->contact_id);
    $result = tripal_core_chado_select('contact', array('*'), $values);
    $contact = $result[0];

    // if the name has changed make sure it doesn't conflict with an existing name
    if ($contact->name != $node->title) {
      $values = array('name' => $node->title);
      $result = tripal_core_chado_select('contact', array('contact_id'), $values);
      if ($result and count($result) > 0) {
        form_set_error('title', 'Cannot update the contact with this contact name. A contact with this name already exists.');
        return;
      }
    }
  }
  // Validating for an insert
  else {
    // The unique constraint for the chado contact table is: name
    $values = array(
      'name' => $node->title,
    );
    $contact = tripal_core_chado_select('contact', array('contact_id'), $values);
    if ($contact and count($contact) > 0) {
      form_set_error('title', 'Cannot add the contact with this name. A contact with these values already exists.');
      return;
    }
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
 */
function chado_contact_node_access($node, $op, $account ) {
  if ($op == 'create') {
    if (!user_access('create chado_contact content', $account)) {
      return FALSE;
    }
    return TRUE;
  }

  if ($op == 'update') {
    if (!user_access('edit chado_contact content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'delete') {
    if (!user_access('delete chado_contact content', $account)) {
      return FALSE;
    }
  }
  if ($op == 'view') {
    if (!user_access('access chado_contact content', $account)) {
      return FALSE;
    }
  }
  return NULL;
}



/**
 * Implementation of tripal_contact_insert().
 *
 * This function inserts user entered information pertaining to the contact instance into the
 * 'contactauthor', 'contactprop', 'chado_contact', 'contact' talble of the database.
 *
 *  @parm $node
 *    Then node which contains the information stored within the node-ID
 *
 *
 */
function chado_contact_insert($node) {

  // remove surrounding white-space on submitted values
  $node->title          = trim($node->title);
  $node->description    = trim($node->description);

  // if there is a contact_id in the $node object then this must be a sync so
  // we can skip adding the contact as it is already there, although
  // we do need to proceed with the rest of the insert
  if (!property_exists($node, 'contact_id')) {
    // insert and then get the newly inserted contact record
    $values = array(
      'name'           => $node->title,
      'description'    => '',
      'type_id'        => $node->type_id,
    );
    $contact = tripal_core_chado_insert('contact', $values);
    if (!$contact) {
      drupal_set_message(t('Unable to add contact.', 'warning'));
      watchdog('tripal_contact', 'Insert contact: Unable to create contact where values: %values',
      array('%values' => print_r($values, TRUE)), WATCHDOG_ERROR);
      return;
    }
    $contact_id = $contact['contact_id'];

      // Add the description property
    $properties = chado_node_properties_form_retreive($node);
    $contact_descrip_id = tripal_cv_get_cvterm_by_name('contact_description', NULL, 'tripal_contact');
    $properties[$contact_descrip_id->cvterm_id][0] = $node->description;

    // * Properties Form *
    $details = array(
      'property_table' => 'contactprop',
      'base_table' => 'contact',
      'foreignkey_name' => 'contact_id',
      'foreignkey_value' => $contact_id
    );
    chado_node_properties_form_update_properties($node, $details, $properties);

    // * Relationships Form *
    $details = array(
      'relationship_table' => 'contact_relationship',  // name of the _relationship table
      'foreignkey_value' => $contact_id                // value of the contact_id key
    );
    chado_node_relationships_form_update_relationships($node, $details);
  }
  else {
    $contact_id = $node->contact_id;
  }

  // Make sure the entry for this contact doesn't already exist in the
  // chado_contact table if it doesn't exist then we want to add it.
  $check_org_id = chado_get_id_for_node('contact', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->contact_id = $contact_id;
    drupal_write_record('chado_contact', $record);
  }
  return TRUE;
}

/*
 *
* Implements hook_update
*
* The purpose of the function is to allow the module to take action when an edited node is being
* updated. It updates any name changes to the database tables that werec reated upon registering a contact.
* As well, the database will be changed, so the user changed information will be saved to the database.
*
* @param $node
*   The node being updated
*
* @ingroup tripal_contact
*/
function chado_contact_update($node) {
  // remove surrounding white-space on submitted values
  $node->title          = trim($node->title);
  $node->description    = trim($node->description);

  $contact_id = chado_get_id_for_node('contact', $node->nid) ;

  // update the contact record
  $match = array(
    'contact_id' => $contact_id,
  );
  $values = array(
    'name' => $node->title,
    'description' => '',
    'type_id' => $node->type_id
  );
  $status = tripal_core_chado_update('contact', $match, $values);
  if (!$status) {
    drupal_set_message("Error updating contact", "error");
    watchdog('t_contact', "Error updating contact", array(), WATCHDOG_ERROR);
    return;
  }

  // Add the description property
  $properties = chado_node_properties_form_retreive($node);
  $contact_descrip_id = tripal_cv_get_cvterm_by_name('contact_description', NULL, 'tripal_contact');
  $properties[$contact_descrip_id->cvterm_id][0] = $node->description;

  // now add in the properties by first removing any the contact
  // already has and adding the ones we have
  $details = array(
    'property_table' => 'contactprop',
    'base_table' => 'contact',
    'foreignkey_name' => 'contact_id',
    'foreignkey_value' => $contact_id
  );
  chado_node_properties_form_update_properties($node, $details, $properties);

  // * Relationships Form *
  $details = array(
    'relationship_table' => 'contact_relationship',  // name of the _relationship table
    'foreignkey_value' => $contact_id                // value of the contact_id key
  );
  chado_node_relationships_form_update_relationships($node, $details);
}


/**
 * Implementation of tripal_contact_load().
 *
 *
 * @param $node
 *   The node that is to be accessed from the database
 *
 * @return $node
 *   The node with the information to be loaded into the database
 *
 */
function chado_contact_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // find the contact and add in the details
    $contact_id = chado_get_id_for_node('contact', $nid);

    // get the contact
    $values = array('contact_id' => $contact_id);
    $contact = tripal_core_generate_chado_var('contact', $values);

    // get the contact description from the contactprop table and replace
    // the contact.description field with this one (we don't use the contact.description
    // field because it is only 255 characters (too small)).
    $values = array(
      'contact_id' => $contact->contact_id,
      'type_id' => array(
        'name' => 'contact_description',
      ),
    );
    $options = array(
      'return_array' => 1,
      'include_fk' => array('type_id' => 1),
    );
    $description = tripal_core_generate_chado_var('contactprop', $values, $options);
    if (count($description) == 1) {
      $description = tripal_core_expand_chado_vars($description, 'field', 'contactprop.value');
      $contact->description = $description[0]->value;
    }

    $nodes[$nid]->contact = $contact;
  }
}

/**
 * Implementation of tripal_contact_delete().
 *
 * This function takes a node and if the delete button has been chosen by the user, the contact
 * and it's details will be removed.Following,given the node-ID, the instance will be deleted from
 * the 'chado_contact' table.
 *
 *  @parm $node
 *    Then node which contains the information stored within the node-ID
 *
 */
function chado_contact_delete(&$node) {

  $contact_id = chado_get_id_for_node('contact', $node->nid);

  // if we don't have a contact id for this node then this isn't a node of
  // type chado_contact or the entry in the chado_contact table was lost.
  if (!$contact_id) {
    return;
  }

  // Remove data from {chado_contact}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_contact} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from contact and contactprop tables of chado database as well
  chado_query("DELETE FROM {contactprop} WHERE contact_id = :contact_id", array(':contact_id' => $contact_id));
  chado_query("DELETE FROM {contact} WHERE contact_id = :contact_id", array(':contact_id' => $contact_id));
}


/**
 *
 * @ingroup tripal_feature
 */
function tripal_contact_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_contact':
      // Show feature browser and counts
      if ($view_mode == 'full') {
        $node->content['tripal_contact_base'] = array(
          '#value' => theme('tripal_contact_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Details',
          '#weight' => 0,
        );
        $node->content['tripal_contact_properties'] = array(
          '#value' => theme('tripal_contact_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_contact_publications'] = array(
          '#value' => theme('tripal_contact_publications', array('node' => $node)),
          '#tripal_toc_id'    => 'publications',
          '#tripal_toc_title' => 'Publications',
        );
        $node->content['tripal_contact_relationships'] = array(
          '#value' => theme('tripal_contact_relationships', array('node' => $node)),
          '#tripal_toc_id'    => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_contact_teaser'] = array(
          '#value' => theme('tripal_contact_teaser', array('node' => $node)),
        );
      }
      break;
  }
}

/**
 *
 * @param $node
 */
function tripal_contact_node_presave($node) {
  switch ($node->type) {
    case 'chado_contact':
      // for a form submission the fields part of the node object
      // but for a sync the feilds are in an object of the node
      if(property_exists($node, 'contact')) {
        // set the title
        $node->title = $node->contact->name;
      }
      else {
        // the title is an element of the form and does not need to be set
      }
      break;
  }
}