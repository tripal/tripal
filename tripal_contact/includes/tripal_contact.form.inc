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
  tripal_core_ahah_init_form();
  $form = array();

  $contact = $node->contact;
  $contact_id = $contact->contact_id;
    
  $d_title        = $form_state['values']['title']       ? $form_state['values']['title']       : $contact->name;
  $d_description  = $form_state['values']['description'] ? $form_state['values']['description'] : $contact->description;
  $d_type_id      = $form_state['values']['type_id']     ? $form_state['values']['type_id']     : $contact->type_id->cvterm_id;
  
  // on AHAH callbacks we want to keep a list of all the properties that have been removed
  // we'll store this info in a hidden field and retrieve it here
  $d_removed = $form_state['values']['removed'];

  // get the number of new fields that have been aded via AHAH callbacks
  $num_new = $form_state['values']['num_new'] ? $form_state['values']['num_new'] : 0;

  // initialze default properties array. This is where we store the property defaults
  $d_properties = array();

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
  if ($node->body) {
    $description = $node->body;
  }
  else {
    $description = $node->description;
  }
  if (!$description) {
    $contactprop = tripal_contact_get_property($contact->contact_id, 'contact_description');
    $description = $contactprop->value;
  }

  // keep track of the contact id if we have.  If we do have one then
  // this is an update as opposed to an insert.
  $form['contact_id'] = array(
    '#type' => 'hidden',
    '#value' => $contact_id,
  );;
  
  // get the contact type
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
  while ($contact_type = db_fetch_object($results)) {
    $contact_types[$contact_type->cvterm_id] = $contact_type->name;
    if (strcmp($contact_type->name,"Person") == 0 and !$d_type_id) {
      $d_type_id = $contact_type->cvterm_id;
    }
  }
  
  // get the contact properties 
  $properties_select = array();
  $properties_select[] = 'Select a Property';
  $properties_list = array();
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
  while ($prop = db_fetch_object($prop_types)) {
    $properties_select[$prop->cvterm_id] = $prop->name;
    $properties_list[$prop->cvterm_id] = $prop;
  }
  
  $form['type_id'] = array(
    '#type' => 'select',
    '#title' => t('Contact Type'),
    '#options' => $contact_types,
    '#required' => TRUE,
    '#default_value' => $d_type_id,
  );
  
  $form['title']= array(
    '#type'          => 'textfield',
    '#title'         => t('Contact Name'),
    '#description'   => t('Enter the name of this contact'),
    '#required'      => TRUE,
    '#default_value' => $d_title,
    '#weight'        => 1,
    '#maxlength'     => 255,
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Contact Description'),
    '#description'   => t('A brief description of the contact'),
    '#required'      => TRUE,
    '#default_value' => $d_description,
    '#weight'        => 5
  );
  
  // add in the properties from the contactprop table
  $num_properties += chado_contact_node_form_add_contactprop_table_props($form, $form_state, $contact_id, $d_properties, $d_removed);

  // add in any new properties that have been added by the user through an AHAH callback
  $num_new = chado_contact_node_form_add_new_props($form, $form_state, $d_properties, $d_removed);

  // add an empty row of field to allow for addition of a new property
  chado_contact_node_form_add_new_empty_props($form, $properties_select);
  

  return $form;

}

/**
 *  validates submission of form when adding or updating a contact node
 *
 * @ingroup tripal_contact
 */
function chado_contact_validate($node, &$form) {
  $title          = trim($node->title);
  $contact_id     = trim($node->contact_id);
  $unittype_id    = trim($node->unittype_id);
  $description    = trim($node->description);
  $num_properties = $node->num_properties;
  $num_new = $node->num_new;
  
  $contact = 0;
  // check to make sure the name on the contact is unique
  // before we try to insert into chado. If this is an update then we will
  // have a contact_id, therefore we want to look for another contact with this
  // name but with a different contact_id. If this is an insert, just look
  // for a case where the name already exists.
  if ($node->contact_id) {
    $sql = "
      SELECT * FROM {contact} 
      WHERE name = '%s' AND NOT contact_id = %d
    ";
    $contact = db_fetch_object(chado_query($sql, $title, $contact_id));
  }
  else {
    $sql = "SELECT * FROM {contact} WHERE name = '%s'";
    $contact = db_fetch_object(chado_query($sql, $title));
  }
  if ($contact) {
    form_set_error('title', t('The contact name already exists. Please choose another'));
  }
  
}
/*
 *
 */
function chado_contact_node_form_add_new_empty_props(&$form, $properties_select) {

  // add one more blank set of property fields
  $form['properties']['new']["new_id"] = array(
    '#type'          => 'select',
    '#options'       => $properties_select,
    '#ahah' => array(
      'path'    => "tripal_contact/properties/description",
      'wrapper' => 'tripal-contact-new_value-desc',
      'event'   => 'change',
      'method'  => 'replace',          
  ),
  );
  $form['properties']['new']["new_value"] = array(
    '#type'          => 'textarea',
    '#default_value' => '',
    '#cols'          => 5,
    '#rows'          => $rows,
    '#description'   => '<div id="tripal-contact-new_value-desc"></div>'
    );
    $form['properties']['new']["add"] = array(
    '#type'         => 'image_button',      
    '#value'        => t('Add'),
    '#src'          => drupal_get_path('theme', 'tripal') . '/images/add.png',
    '#ahah' => array(
      'path'    => "tripal_contact/properties/add",
      'wrapper' => 'tripal-contact-edit-properties-table',
      'event'   => 'click',
      'method'  => 'replace',          
    ),
    '#attributes' => array('onClick' => 'return false;'),
    );
}
/*
 *
 */
function chado_contact_node_form_add_new_props(&$form, $form_state, &$d_properties, &$d_removed) {
   
  // first, add in all of the new properties that were added through a previous AHAH callback
  $j = 0;
  $num_properties++;

  // we need to find the
  if ($form_state['values']) {
    foreach ($form_state['values'] as $element_name => $value) {
      if (preg_match('/new_value-(\d+)-(\d+)/', $element_name, $matches)) {
        $new_id = $matches[1];
        $rank = $matches[2];

        // skip any properties that the user requested to delete through a previous
        // AHAH callback or through the current AHAH callback
        if($d_removed["$new_id-$rank"]) {
          continue;
        }
        if($form_state['post']['remove-' . $new_id . '-' . $rank]) {
          $d_removed["$new_id-$rank"] = 1;
          continue;
        }

        // get this new_id information
        $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), array('cvterm_id' => $new_id));

        // add it to the $d_properties array
        $d_properties[$new_id][$rank]['name']  = $cvterm->name;
        $d_properties[$new_id][$rank]['id']    = $new_id;
        $d_properties[$new_id][$rank]['value'] = $value;
        $d_properties[$new_id][$rank]['definition']  = $cvterm->definition;
        $num_properties++;

        // determine how many rows we need in the textarea
        $rows = 1;


        // add the new fields
        $form['properties']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
          '#type'          => 'item',
          '#value'         => $cvterm[0]->name
        );
        $form['properties']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
          '#type'          => 'textarea',
          '#default_value' => $value,
          '#cols'          => 50,
          '#rows'          => $rows,
          '#description'   => $cvterm->definition,
        );

        $form['properties']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
          '#type'         => 'image_button',
          '#value'        => t('Remove'),
          '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
          '#ahah' => array(
            'path'    => "tripal_contact/properties/minus/$new_id/$rank",
            'wrapper' => 'tripal-contact-edit-properties-table',
            'event'   => 'click',
            'method'  => 'replace',
        ),
          '#attributes' => array('onClick' => 'return false;'),
        );
      }
    }
  }


  // second add in any new properties added during this callback
  if($form_state['post']['add']) {
    $new_id = $form_state['values']['new_id'];
    $new_value = $form_state['values']['new_value'];

    // get the rank by counting the number of entries
    $rank = count($d_properties[$new_id]);

    // get this new_id information
    $cvterm = tripal_core_chado_select('cvterm', array('name', 'definition'), array('cvterm_id' => $new_id));

    // add it to the $d_properties array
    $d_properties[$new_id][$rank]['name']  = $cvterm->name;
    $d_properties[$new_id][$rank]['id']    = $new_id;
    $d_properties[$new_id][$rank]['value'] = $value;
    $d_properties[$new_id][$rank]['definition']  = $cvterm->definition;
    $num_properties++;

    // determine how many rows we need in the textarea
    $rows = 1;
    if (preg_match('/Abstract/', $cvterm[0]->name)) {
      $rows = 10;
    }
    if ($cvterm[0]->name == 'Authors') {
      $rows = 2;
    }

    // add the new fields
    $form['properties']['new'][$new_id][$rank]["new_id-$new_id-$rank"] = array(
      '#type'          => 'item',
      '#value'         => $cvterm[0]->name
    );
    $form['properties']['new'][$new_id][$rank]["new_value-$new_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $new_value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => $cvterm->definition,
    );

    $form['properties']['new'][$new_id][$rank]["remove-$new_id-$rank"] = array(
      '#type'         => 'image_button',
      '#value'        => t('Remove'),
      '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
      '#ahah' => array(
        'path'    => "tripal_contact/properties/minus/$new_id/$rank",
        'wrapper' => 'tripal-contact-edit-properties-table',
        'event'   => 'click',
        'method'  => 'replace',
    ),
      '#attributes' => array('onClick' => 'return false;'),
    );

  }

  return $num_properties;
}
/*
 *
 */
function chado_contact_node_form_add_contactprop_table_props(&$form, $form_state, $contact_id, &$d_properties, &$d_removed) {

  // get the properties for this contact
  $num_properties = 0;

  if(!$contact_id) {
    return $num_properties;
  }

  $sql = "
    SELECT CVT.cvterm_id, CVT.name, CVT.definition, PP.value, PP.rank
    FROM {contactprop} PP
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = PP.type_id
    WHERE PP.contact_id = %d
    ORDER BY CVT.name, PP.rank
  ";
  $contact_props = chado_query($sql, $contact_id);
  while ($prop = db_fetch_object($contact_props)) {

    $type_id = $prop->cvterm_id;
    $rank = count($d_properties[$type_id]);
    
    // skip the description as we have a separate form element for that
    if($prop->name == "contact_description") {
      continue;
    }

    // skip any properties that the user requested to delete through a previous
    // AHAH callback or through the current AHAH callback
    if($d_removed["$type_id-$rank"]) {
      continue;
    }
    if($form_state['post']['remove-' . $type_id . '-' . $rank]) {
      $d_removed["$type_id-$rank"] = 1;
      continue;
    }

    $d_properties[$type_id][$rank]['name']  = $prop->name;
    $d_properties[$type_id][$rank]['id']    = $type_id;
    $d_properties[$type_id][$rank]['value'] = $prop->value;
    $d_properties[$type_id][$rank]['definition']  = $prop->definition;
    $num_properties++;

    $form['properties'][$type_id][$rank]["prop_id-$type_id-$rank"] = array(
      '#type'          => 'item',
      '#value'         => $prop->name,
    );
    $form['properties'][$type_id][$rank]["prop_value-$type_id-$rank"] = array(
      '#type'          => 'textarea',
      '#default_value' => $prop->value,
      '#cols'          => 50,
      '#rows'          => $rows,
      '#description'   => $prop->definition,
    );

    $form['properties'][$type_id][$rank]["remove-$type_id-$rank"] = array(
      '#type'         => 'image_button',
      '#value'        => t('Remove'),
      '#src'          => drupal_get_path('theme', 'tripal') . '/images/minus.png',
      '#ahah' => array(
        'path'    => "tripal_contact/properties/minus/$type_id/$rank",
        'wrapper' => 'tripal-contact-edit-properties-table',
        'event'   => 'click',
        'method'  => 'replace',
    ),
      '#attributes' => array('onClick' => 'return false;'),
    );
  }
  return $num_properties;
}
/*
 *
 */
function tripal_contact_theme_node_form_properties($form) {
  $rows = array();

  if ($form['properties']) {

    // first add in the properties derived from the contactprop table
    // the array tree for these properties looks like this:
    // $form['properties'][$type_id][$rank]["prop_id-$type_id-$rank"]
    foreach ($form['properties'] as $type_id => $elements) {
      // there are other fields in the properties array so we only
      // want the numeric ones those are our type_id
      if (is_numeric($type_id)) {
        foreach ($elements as $rank => $element) {
          if (is_numeric($rank)) {
            $rows[] = array(
            drupal_render($element["prop_id-$type_id-$rank"]),
            drupal_render($element["prop_value-$type_id-$rank"]),
            drupal_render($element["remove-$type_id-$rank"]),
            );
          }
        }
      }
    }

    // second, add in any new properties added by the user through AHAH callbacks
    // the array tree for these properties looks like this:
    // $form['properties']['new'][$type_id][$rank]["new_id-$new_id-$rank"]
    foreach ($form['properties']['new'] as $type_id => $elements) {
      if (is_numeric($type_id)) {
        foreach ($elements as $rank => $element) {
          if (is_numeric($rank)) {
            $rows[] = array(
            drupal_render($element["new_id-$type_id-$rank"]),
            drupal_render($element["new_value-$type_id-$rank"]),
            drupal_render($element["remove-$type_id-$rank"]),
            );
          }
        }
      }
    }

    // finally add in a set of blank field for adding a new property
    $rows[] = array(
    drupal_render($form['properties']['new']['new_id']),
    drupal_render($form['properties']['new']['new_value']),
    drupal_render($form['properties']['new']['add']),
    );
  }

  $headers = array('Property Type','Value', '');
  return theme('table', $headers, $rows, array('id'=> "tripal-contact-edit-properties-table"));
}

/*
 *
 */
function tripal_contact_property_add() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();

  // we only want to return the properties as that's all we'll replace with this AHAh callback
  $data = tripal_contact_theme_node_form_properties($form);

  // bind javascript events to the new objects that will be returned
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
  array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
  )
  );
}
/*
 *
 */
function tripal_contact_property_delete() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();

  // we only want to return the properties as that's all we'll replace with this AHAh callback
  $data = tripal_contact_theme_node_form_properties($form);

  // bind javascript events to the new objects that will be returned
  // so that AHAH enabled elements will work.
  $settings = tripal_core_ahah_bind_events();

  // return the updated JSON
  drupal_json(
    array(
      'status'   => $status, 
      'data'     => $data,
      'settings' => $settings,
    )
  );
}
/*
 *
 */
function tripal_contact_property_get_description() {
  $new_id = $_POST['new_id'];

  $values = array('cvterm_id' => $new_id);
  $cvterm = tripal_core_chado_select('cvterm', array('definition'), $values);

  $description = '&nbsp;';
  if ($cvterm[0]->definition) {
    $description = $cvterm[0]->definition;
  }
  drupal_json(
    array(
      'status' => TRUE,
      'data'   => '<div id="tripal-contact-new_value-desc">' . $description . '</div>',
    )
  );
}
/*
 *
 */
function theme_chado_contact_node_form($form) {

  $properties_table = tripal_contact_theme_node_form_properties($form);

  $markup  = drupal_render($form['contact_id']);
  $markup .= drupal_render($form['title']);
  $markup .= drupal_render($form['type_id']);
  $markup .= drupal_render($form['description']);
  $markup .= "<b>Include Additional Details</b><br>You may add additional properties to this contact by scrolling to the bottom of this table, selecting a property type from the dropdown and adding text.  You may add as many properties as desired by clicking the plus button on the right.  To remove a property, click the minus button";
  $markup .= $properties_table;

  $form['properties'] = array(
    '#type' => 'markup',
    '#value' =>  $markup,
  );
  return drupal_render($form);
}
