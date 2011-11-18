<?php

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_properties_page($node) {
  $output = '';

  $properties = tripal_core_chado_select(
    'featureprop', 
    array('featureprop_id','type_id','value','rank'), 
    array(
      'feature_id' => $node->feature->feature_id
    ),
    array('order_by' => array('type_id' => 'ASC','rank'=>'ASC'))
  );
  $expand_add = (sizeof($properties)) ? FALSE : TRUE;
  
  $output .= drupal_get_form('tripal_feature_add_ONE_property_form', $node, $expand_add);
  $output .= drupal_get_form('tripal_feature_edit_ALL_properties_form', $node, $properties);
  $output .= drupal_get_form('tripal_feature_implement_back_to_feature_button', $node->nid);

  return $output;
}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_property_form($form_state, $node, $expand) {
  $form = array();
  $feature_id = $node->feature->feature_id;

  $form['add_properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Property'),
    '#collapsible' => TRUE,
    '#collapsed' => ($expand) ? FALSE : TRUE,
  ); 

  $form['prop_nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );
	
  $prop_type_options = array();
  $results = tripal_core_chado_select('cvterm',array('cvterm_id','name'), array('cv_id' => array('name' => 'feature_property')));
  foreach ($results as $r) {
    $prop_type_options[$r->cvterm_id] = $r->name;
  }
  $form['add_properties']['prop_type_id'] = array(
    '#type' => 'select',
    '#title' => t('Type of Property'),
    '#options' => $prop_type_options,
  );

  $form['add_properties']['prop_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value'),
  );

  $form['add_properties']['prop_feature_id'] = array(
    '#type' => 'value',
    '#value' => $feature_id,
    '#required' => TRUE
  );

  $form['add_properties']['submit-add'] = array(
    '#type' => 'submit',         
    '#value' => t('Add Property')
  );

  return $form;
}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_property_form_validate($form, &$form_state) {

  // Only Require if Adding Property
  if ($form_state['clicked_button']['#value'] == t('Add Property') ) {

		// Check that there is a feature
		if ( $form_state['values']['prop_feature_id'] <= 0 ) {
			form_set_error('prop_feature_id', 'There is no associated feature.');
		}
		
    // Check that Selected a type
    if ( $form_state['values']['prop_type_id'] == 0) {
      form_set_error('prop_type_id', 'Please select a type of property.');
    } else {
      // Check that type is in chado
      $previous_db = tripal_db_set_active('chado');
      $num_rows = db_fetch_object(db_query("SELECT count(*) as count FROM cvterm WHERE cvterm_id=%d", $form_state['values']['prop_type_id']));
      tripal_db_set_active($previous_db);
      if ( $num_rows->count != 1) {
        form_set_error('prop_type_id', "The type you selected is not valid. Please choose another one. (CODE:$num_rows)");
      } // end of if more or less than 1 row
    } // if no prop type



  } // if add Property

}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_property_form_submit($form, &$form_state) {
	
  // if there is a property add it (only won't be a property if clicked next step w/ no property)
  if ($form_state['values']['prop_type_id'] != 0) {
    //determine the rank for this property
    $max_rank = get_max_chado_rank('featureprop', 
       array('feature_id'=>array('type'=>'INT','value'=>$form_state['values']['prop_feature_id']), 
       'type_id'=>array('type'=>'INT','value'=> $form_state['values']['prop_type_id']) ));
    if ($max_rank == -1) { $rank = 0; 
    } else { $rank = $max_rank+1; }
    
    $previous_db = tripal_db_set_active('chado');
    db_query(
      "INSERT INTO featureprop (feature_id, type_id, value, rank) VALUES (%d, %d, '%s', %d)",
      $form_state['values']['prop_feature_id'],
      $form_state['values']['prop_type_id'],
      $form_state['values']['prop_value'],
      $rank
    );
    tripal_db_set_active($previous_db);

    drupal_set_message("Successfully Added Property");

  } //end of if property to add
}

/**
 * Implements Hook_form()
 * Handles adding of Properties for features
 *
 * @ingroup tripal_feature
 */ 
function tripal_feature_edit_ALL_properties_form($form_state, $node, $properties) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );
  
  if (sizeof($properties)) {
    
    $prop_type_options = array();
    $results = tripal_core_chado_select('cvterm',array('cvterm_id','name'), array('cv_id' => array('name' => 'feature_property')));
    foreach ($results as $r) {
      $prop_type_options[$r->cvterm_id] = $r->name;
    }
      
    foreach ($properties as $i => $property) {
    
      $form["num-$i"] = array(
       '#type' => 'fieldset',
       '#value' => "Property $i"
      );
      
      $form["num-$i"]["id-$i"] = array(
       '#type' => 'hidden',
       '#value' => $property->featureprop_id
      );
      
      $default = array_search($property->type, $prop_type_options);  
      $form["num-$i"]["type-$i"] = array(
       '#type' => 'select',
       //'#title' => t('Type of Property'),
       '#options' => $prop_type_options,
       '#default_value' => $property->type_id
      );
      
      $form["num-$i"]["value-$i"] = array(
       '#type' => 'textfield',
       //'#title' => t('Value'),
       '#default_value' => $property->value
      );
      
      $form["num-$i"]["delete-$i"] = array(
       '#type' => 'submit',
       '#value' => t("Delete"),
       '#name' => "delete-$i",
      );
    }//end of foreach property
  
    $form['num_properties'] = array(
      '#type' => 'hidden',
      '#value' => $i
    );
  
    $form["submit-edits"] = array(
      '#type' => 'submit',
      '#value' => t('Update All Properties')
    );
  }
  
  return $form;
}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_properties_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == t('Update All Properties') ) {
     //Update all
     for ($i=1; $i<=$form_state['values']['num_properties']; $i++) {
       tripal_feature_update_property($form_state['values']["id-$i"], $form_state['values']["type-$i"], $form_state['values']["value-$i"], $form_state['values']["preferred-$i"], $form_state['values']["nid"]);
     }
     drupal_set_message("Updated all Properties");
     drupal_goto('node/'.$form_state['values']['nid']);
  } elseif ( preg_match('/delete-(\d+)/', $form_state['clicked_button']['#name'], $matches) ) {
     $i = $matches[1];
     tripal_feature_delete_property($form_state['values']["id-$i"], $form_state['values']["type-$i"], $form_state['values']["value-$i"]);
     drupal_set_message("Deleted Property");
  } else {
    drupal_set_message("Unrecognized Button Pressed",'error');
  }

  
}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_update_property($featureprop_id, $cvterm_id, $value, $preferred, $nid) {

	$previous_db = tripal_db_set_active('chado');
	$old_obj = db_fetch_object(db_query("SELECT * FROM featureprop WHERE featureprop_id=%d",$featureprop_id));
	tripal_db_set_active($previous_db);
	
	// if they changed the type need to check rank
	//   (if there is another property of the same type then rank needs to be increased to prevent collisions)
	if ($cvterm_id == $old_obj->type_id) {
	  $previous_db = tripal_db_set_active('chado');
  	db_query(
    	"UPDATE featureprop SET type_id=%d, value='%s' WHERE featureprop_id=%d",
    	$cvterm_id, 
    	$value,
    	$featureprop_id
  	);
  	tripal_db_set_active($previous_db);
  } else {
      //determine the rank for this property
    $max_rank = get_max_chado_rank('featureprop', 
       array('feature_id'=>array('type'=>'INT','value'=> $old_obj->feature_id), 
    	 'type_id'=>array('type'=>'INT','value'=> $cvterm_id ) ));
    if ($max_rank == -1) { $rank = 0; 
    } else { $rank = $max_rank+1; }
	  $previous_db = tripal_db_set_active('chado');
  	db_query(
    	"UPDATE featureprop SET type_id=%d, value='%s', rank=%d WHERE featureprop_id=%d",
    	$cvterm_id, 
    	$value,
    	$rank,
    	$featureprop_id
  	);
  	tripal_db_set_active($previous_db);  	
  }
                                                                                                                                                                                                                          
    module_load_include('inc', 'node', 'node.pages');                                                                                                                                                                                
    drupal_execute('chado_feature_node_form', $node_form_state, $node);  

}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_property($featureprop_id) {

  $previous_db = tripal_db_set_active('chado');
  db_query(
    "DELETE FROM featureprop WHERE featureprop_id=%d",
    $featureprop_id
  );
  tripal_db_set_active($previous_db);

}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function theme_tripal_feature_edit_ALL_properties_form ($form) {
  $output = '';
  
  $output .= '<br><fieldset>';
  $output .= '<legend>Edit Already Existing Properties<span class="form-optional" title="This field is optional">(optional)</span></legend>';
  $output .= '<p>Below is a list of already existing properties for this feature, one property per line. The type refers to the type of '
  	     .'property and the value is the value for that property. </p>';
  $output .= '<table>';
  $output .= '<tr><th>#</th><th>Type</th><th>Value</th><th></th></tr>';

  for ($i=0; $i<=$form['num_properties']['#value']; $i++) {
    if (isset($form["num-$i"])) {
      $output .= '<tr><td>'.($i+1).'</td><td>'.drupal_render($form["num-$i"]["type-$i"]).'</td><td>'.drupal_render($form["num-$i"]["value-$i"]).'</td><td>'.drupal_render($form["num-$i"]["delete-$i"]).'</td></tr>';
      unset($form["num-$i"]);
    }
  }

  $output .= '</table><br>';
  $output .= drupal_render($form);
  $output .= '</fieldset>';

  return $output;
}

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_list_properties_for_node($properties) {

  if (!empty($properties)) {
    $output = '<table>';
    $output .= '<tr><th>Type</th><th>Value</th></tr>';


    if (!empty($properties) ) {
      foreach ($properties as $p) {
        $output .= '<tr><td>'.$p->type.'</td><td>'.$p->value.'</td></tr>';
      } // end of foreach property
    }

    $output .= '</table>';

  } else {
    $output = 'No properties exist for the current feature';
  }

  return $output;
}
