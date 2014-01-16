<?php
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
      $description = $contactprop->value;
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
  $contact_types = array();
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

  // get the contact properties that the user can use for this form
  $properties = array();
  $properties[] = 'Select a Property';
  $sql = "
    SELECT CVTS.cvterm_id, CVTS.name
    FROM {cvtermpath} CVTP
      INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
      INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
      INNER JOIN {cv} CV       ON CVTO.cv_id = CV.cv_id
    WHERE
      CV.name = 'tripal_contact' AND
      NOT CVTO.name = 'Contact Type'
    ORDER BY CVTS.name ASC
  ";
  $prop_types = chado_query($sql);
  while ($prop = $prop_types->fetchObject()) {
    $properties[$prop->cvterm_id] = $prop->name;
  }
  
  $exclude = array('contact_description');
  tripal_core_properties_form($form, $form_state, 'contactprop', 'contact_id', 'tripal_contact',
    $properties, $contact_id, $exclude, '');
   
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
  if (!property_exists($node,'nid')) {    
    // get the existing node    
    $values = array('contact_id' => $node->contact_id);      
    $result = tripal_core_chado_select('contact', array('*'), $values);
    $contact = $result[0];
      
    // if the name has changed make sure it doesn't conflict with an existing name
    if ($contact->name != $node->title) {
      $values = array('name' => $node->title);
      $result = tripal_core_chado_select('contact', array('contact_id'), $values);
      if ($result and count($result) > 0) {
        form_set_error('title', 'Cannot update the contact with this contact name. An contact with this name already exists.');
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
      form_set_error('title', 'Cannot add the contact with this name. An contact with these values already exists.');
      return;
    }
  }
}
