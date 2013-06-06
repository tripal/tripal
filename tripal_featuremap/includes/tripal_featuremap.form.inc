<?php
/**
 *  When editing or creating a new node of type 'chado_featuremap' we need
 *  a form.  This function creates the form that will be used for this.
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_form($node) {
  tripal_core_ahah_init_form();
  $form = array();
  
  $featuremap = $node->featuremap;
  $featuremap_id = $featuremap->featuremap_id;
  
  $d_title        = $form_state['values']['title']       ? $form_state['values']['title']       : $featuremap->name;
  $d_description  = $form_state['values']['description'] ? $form_state['values']['description'] : $featuremap->description;
  $d_unittype_id  = $form_state['values']['unittype_id'] ? $form_state['values']['unittype_id'] : $featuremap->unittype_id->cvterm_id;

  // on AHAH callbacks we want to keep a list of all the properties that have been removed
  // we'll store this info in a hidden field and retrieve it here
  $d_removed = $form_state['values']['removed'];

  // get the number of new fields that have been aded via AHAH callbacks
  $num_new = $form_state['values']['num_new'] ? $form_state['values']['num_new'] : 0;

  // initialze default properties array. This is where we store the property defaults
  $d_properties = array();
  
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
  
  // get the featuremap properties
  $properties_select = array();
  $properties_select[] = 'Select a Property';
  $properties_list = array();
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
  while ($prop = db_fetch_object($prop_types)) {
    $properties_select[$prop->cvterm_id] = $prop->name;
    $properties_list[$prop->cvterm_id] = $prop;
  }
  
  // keep track of the map id if we have.  If we do have one then
  // this is an update as opposed to an insert.
  $form['featuremap_id'] = array(
    '#type' => 'hidden',
    '#value' => $featuremap_id,
  );

  $form['title']= array(
    '#type'          => 'textfield',
    '#title'         => t('Map Name'),
    '#description'   => t('Please enter a name for this map'),
    '#required'      => TRUE,
    '#default_value' => $d_title,
    '#maxlength'     => 255
  );

  $form['description']= array(
    '#type'          => 'textarea',
    '#title'         => t('Map Description'),
    '#description'   => t('A description of the map.'),
    '#required'      => TRUE,
    '#default_value' => $d_description,
  );
  

  $form['unittype_id'] = array(
    '#title'       => t('Map Units'),
    '#type'        => t('select'),
    '#description' => t("Chose the units for this map"),
    '#required'    => TRUE,
    '#default_value' => $d_unittype_id,
    '#options'     => $units,
  );

  
  // add in the properties from the featuremapprop table
  $num_properties += chado_featuremap_node_form_add_featuremapprop_table_props($form, $form_state, $featuremap_id, $d_properties, $d_removed);

  // add in any new properties that have been added by the user through an AHAH callback
  $num_new = chado_featuremap_node_form_add_new_props($form, $form_state, $d_properties, $d_removed);

  // add an empty row of field to allow for addition of a new property
  chado_featuremap_node_form_add_new_empty_props($form, $properties_select);
  

  return $form;
}
/**
 *  validates submission of form when adding or updating a map node
 *
 * @ingroup tripal_featuremap
 */
function chado_featuremap_validate($node, &$form) {
  $name          = trim($node->title);
  $featuremap_id = trim($node->featuremap_id);
  $unittype_id   = trim($node->unittype_id);
  $description   = trim($node->description);
  $num_properties = $node->num_properties;
  $num_new = $node->num_new;
  
  $featuremap = 0;
  // check to make sure the unique name on the map is unique
  // before we try to insert into chado. If this is an update then we will
  // have a featuremap_id, therefore we want to look for another map with this
  // name but with a different featuremap_id. If this is an insert, just look
  // for a case where the name already exists.
  if ($node->featuremap_id) {
    $sql = "
      SELECT * FROM {featuremap} 
      WHERE name = '%s' AND NOT featuremap_id = %d
    ";
    $featuremap = db_fetch_object(chado_query($sql, $node->title, $node->featuremap_id));
  }
  else {
    $sql = "SELECT * FROM {featuremap} WHERE name = '%s'";
    $featuremap = db_fetch_object(chado_query($sql, $node->title));
  }
  if ($featuremap) {
    form_set_error('name', t('The unique map name already exists. Please choose another'));
  }
  
}
/*
 *
 */
function chado_featuremap_node_form_add_new_empty_props(&$form, $properties_select) {

  // add one more blank set of property fields
  $form['properties']['new']["new_id"] = array(
    '#type'          => 'select',
    '#options'       => $properties_select,
    '#ahah' => array(
      'path'    => "tripal_featuremap/properties/description",
      'wrapper' => 'tripal-featuremap-new_value-desc',
      'event'   => 'change',
      'method'  => 'replace',          
  ),
  );
  $form['properties']['new']["new_value"] = array(
    '#type'          => 'textarea',
    '#default_value' => '',
    '#cols'          => 5,
    '#rows'          => $rows,
    '#description'   => '<div id="tripal-featuremap-new_value-desc"></div>'
    );
    $form['properties']['new']["add"] = array(
    '#type'         => 'image_button',      
    '#value'        => t('Add'),
    '#src'          => drupal_get_path('theme', 'tripal') . '/images/add.png',
    '#ahah' => array(
      'path'    => "tripal_featuremap/properties/add",
      'wrapper' => 'tripal-featuremap-edit-properties-table',
      'event'   => 'click',
      'method'  => 'replace',          
    ),
    '#attributes' => array('onClick' => 'return false;'),
    );
}
/*
 *
 */
function chado_featuremap_node_form_add_new_props(&$form, $form_state, &$d_properties, &$d_removed) {
   
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
            'path'    => "tripal_featuremap/properties/minus/$new_id/$rank",
            'wrapper' => 'tripal-featuremap-edit-properties-table',
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
        'path'    => "tripal_featuremap/properties/minus/$new_id/$rank",
        'wrapper' => 'tripal-featuremap-edit-properties-table',
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
function chado_featuremap_node_form_add_featuremapprop_table_props(&$form, $form_state, $featuremap_id, &$d_properties, &$d_removed) {

  // get the properties for this featuremap
  $num_properties = 0;

  if(!$featuremap_id) {
    return $num_properties;
  }

  $sql = "
    SELECT CVT.cvterm_id, CVT.name, CVT.definition, PP.value, PP.rank
    FROM {featuremapprop} PP
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = PP.type_id
    WHERE PP.featuremap_id = %d
    ORDER BY CVT.name, PP.rank
  ";
  $featuremap_props = chado_query($sql, $featuremap_id);
  while ($prop = db_fetch_object($featuremap_props)) {

    $type_id = $prop->cvterm_id;
    $rank = count($d_properties[$type_id]);

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
        'path'    => "tripal_featuremap/properties/minus/$type_id/$rank",
        'wrapper' => 'tripal-featuremap-edit-properties-table',
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
function tripal_featuremap_theme_node_form_properties($form) {
  $rows = array();

  if ($form['properties']) {

    // first add in the properties derived from the featuremapprop table
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
  return theme('table', $headers, $rows, array('id'=> "tripal-featuremap-edit-properties-table"));
}

/*
 *
 */
function tripal_featuremap_property_add() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();

  // we only want to return the properties as that's all we'll replace with this AHAh callback
  $data = tripal_featuremap_theme_node_form_properties($form);

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
function tripal_featuremap_property_delete() {
  $status = TRUE;

  // prepare and render the form
  $form = tripal_core_ahah_prepare_form();

  // we only want to return the properties as that's all we'll replace with this AHAh callback
  $data = tripal_featuremap_theme_node_form_properties($form);

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
function tripal_featuremap_property_get_description() {
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
      'data'   => '<div id="tripal-featuremap-new_value-desc">' . $description . '</div>',
    )
  );
}
/*
 *
 */
function theme_chado_featuremap_node_form($form) {

  $properties_table = tripal_featuremap_theme_node_form_properties($form);

  $markup  = drupal_render($form['featuremap_id']);
  $markup .= drupal_render($form['title']);
  $markup .= drupal_render($form['unittype_id']);
  $markup .= drupal_render($form['description']);
  $markup .= "<b>Include Additional Details</b><br>You may add additional properties to this map by scrolling to the bottom of this table, selecting a property type from the dropdown and adding text.  You may add as many properties as desired by clicking the plus button on the right.  To remove a property, click the minus button";
  $markup .= $properties_table;
  $markup .= drupal_render($form['is_obsolete']);

  $form['properties'] = array(
    '#type' => 'markup',
    '#value' =>  $markup,
  );
  return drupal_render($form);
}