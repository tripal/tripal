<?php

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_add_ALL_property_page($node) {
  $output = '';

  $output .= tripal_stock_add_chado_properties_progress('properties').'<br>';
  $output .= '<b>All Properties should strictly pertain to THE CURRENT Individual</b><br>';
  $output .= '<br>';
  $output .= theme('tripal_stock_properties', $node);
  $output .= '<br><br>';
  $output .= drupal_get_form('tripal_stock_add_ONE_property_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_stock_add_chado_properties_navigate', 'properties', $node->nid);
  return $output;
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_add_ONE_property_form($form_state, $node) {
  $form = array();
  $stock_id = $node->stock->stock_id;

  $form['add_properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Property') . '<span class="form-optional" title="This field is optional"> (optional)</span>',
  ); 

  $form['prop_nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

	$tmp_obj = tripal_cv_get_cvterm_by_name('synonym', variable_get('chado_stock_prop_types_cv', 'null'));
	$synonym_id = $tmp_obj->cvterm_id;
	
  $prop_type_options = tripal_cv_get_cvterm_options( variable_get('chado_stock_prop_types_cv', 'null') ); 
  $prop_type_options[0] = 'Select a Type';
  ksort($prop_type_options);
  $form['add_properties']['prop_type_id'] = array(
    '#type' => 'select',
    '#title' => t('Type of Property'),
    '#options' => $prop_type_options,
  );

  $form['add_properties']['prop_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value') . '<span class="form-optional" title="This field is optional">+</span>',
  );

  $form['add_properties']['preferred_synonym'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preferred Synonym (only applicable if type is synonym)'),
  );

  $form['add_properties']['prop_stock_id'] = array(
    '#type' => 'value',
    '#value' => $stock_id,
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
 * @ingroup tripal_stock
 */
function tripal_stock_add_ONE_property_form_validate($form, &$form_state) {

  // Only Require if Adding Property
  if ($form_state['clicked_button']['#value'] == t('Add Property') ) {

		// Check that there is a stock
		if ( $form_state['values']['prop_stock_id'] <= 0 ) {
			form_set_error('prop_stock_id', 'There is no associated stock.');
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

    // only check preferred synonym if type is synonym
    if ($form_state['values']['preferred_synonym'] == 1) {
      $tmp_obj = tripal_cv_get_cvterm_by_name('synonym', variable_get('chado_stock_prop_types_cv', 'null'));
      if ($form_state['values']['prop_type_id'] != $tmp_obj->cvterm_id) {
        form_set_error('preferred_synonym', 'Preferred Synonym Checkbox Only Applicable if Type of Property is Synonym');
      }
    }

  } // if add Property

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_add_ONE_property_form_submit($form, &$form_state) {
	
  // if there is a property add it (only won't be a property if clicked next step w/ no property)
  if ($form_state['values']['prop_type_id'] != 0) {
    //determine the rank for this property
    $max_rank = get_max_chado_rank('stockprop', 
    															array('stock_id'=>array('type'=>'INT','value'=>$form_state['values']['prop_stock_id']), 
    																		'type_id'=>array('type'=>'INT','value'=> $form_state['values']['prop_type_id']) ));
    if ($max_rank == -1) { $rank = 0; 
    } else { $rank = $max_rank+1; }
    
    $previous_db = tripal_db_set_active('chado');
    db_query(
      "INSERT INTO stockprop (stock_id, type_id, value, rank) VALUES (%d, %d, '%s', %d)",
      $form_state['values']['prop_stock_id'],
      $form_state['values']['prop_type_id'],
      $form_state['values']['prop_value'],
      $rank
    );
    tripal_db_set_active($previous_db);

    drupal_set_message("Successfully Added Property");

    // Set Preferred Synonym
    if ($form_state['values']['preferred_synonym'] == 1) {

      //use update node form so that both title and name get set
      $node = node_load($form_state['values']['prop_nid']);
      $node->title = $form_state['values']['prop_value'];
      $node_form_state = array( 
        'values' => array(
					'title' => $form_state['values']['prop_value'], 
					'op' => 'Save'
				) 
      );
      module_load_include('inc', 'node', 'node.pages');
      drupal_execute('chado_stock_node_form', $node_form_state, $node);

    }

  } //end of if property to add
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_edit_ALL_properties_page($node) {
  $output = '';

  $output .= drupal_get_form('tripal_stock_edit_ALL_properties_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_stock_add_ONE_property_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_stock_back_to_stock_button', $node->nid);

  return $output;
}

/**
 * Implements Hook_form(): Handles adding of Properties & Synonyms to Stocks
 *
 * @ingroup tripal_stock
 */
function tripal_stock_edit_ALL_properties_form($form_state, $node) {
  $form = array();

  // Add properties and synonyms
  $node = tripal_core_expand_chado_vars($node, 'table', 'stockprop');

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  $i=0;
  if (!$node->stock->stockprop) { 
    $node->stock->stockprop = array(); 
  } elseif (!is_array($node->stock->stockprop)) { 
    $node->stock->stockprop = array($node->stock->stockprop); 
  }
  if (sizeof($node->stock->stockprop) != 0) {
  foreach ($node->stock->stockprop as $property) {
    $i++;
    $form["num-$i"] = array(
      '#type' => 'item',
      '#value' => $i.'.'
    );

    $form["id-$i"] = array(
      '#type' => 'hidden',
      '#value' => $property->stockprop_id
    );

    $prop_type_options = tripal_cv_get_cvterm_options( variable_get('chado_stock_prop_types_cv', 'null') );
    ksort($prop_type_options);
    $form["type-$i"] = array(
      '#type' => 'select',
      //'#title' => t('Type of Property'),
      '#options' => $prop_type_options,
      '#default_value' => $property->type_id->cvterm_id
    );

    $form["value-$i"] = array(
      '#type' => 'textfield',
      //'#title' => t('Value'),
      '#default_value' => $property->value
    );

    $form["submit-$i"] = array(
      '#type' => 'submit',
      '#value' => t("Delete #$i")
    );

  }} //end of foreach property

  $form['num_properties'] = array(
    '#type' => 'hidden',
    '#value' => $i
  );

  $form["submit-edits"] = array(
    '#type' => 'submit',
    '#value' => t('Update Properties')
  );

  return $form;
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_edit_ALL_properties_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == t('Update Properties') ) {
     //Update all
     for ($i=1; $i<=$form_state['values']['num_properties']; $i++) {
       tripal_stock_update_property($form_state['values']["id-$i"], $form_state['values']["type-$i"], $form_state['values']["value-$i"], $form_state['values']["preferred-$i"], $form_state['values']["nid"]);
     }
     drupal_set_message("Updated all Properties");
     drupal_goto('node/'.$form_state['values']['nid']);
  } elseif ( preg_match('/Delete #(\d+)/', $form_state['clicked_button']['#value'], $matches) ) {
     $i = $matches[1];
     tripal_stock_delete_property($form_state['values']["id-$i"], $form_state['values']["type-$i"], $form_state['values']["value-$i"]);
     drupal_set_message("Deleted Property");
  } else {
    drupal_set_message("Unrecognized Button Pressed",'error');
  }

  
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_update_property($stockprop_id, $cvterm_id, $value, $preferred, $nid) {

	$previous_db = tripal_db_set_active('chado');
	$old_obj = db_fetch_object(db_query("SELECT * FROM stockprop WHERE stockprop_id=%d",$stockprop_id));
	tripal_db_set_active($previous_db);
	
	// if they changed the type need to check rank
	//   (if there is another property of the same type then rank needs to be increased to prevent collisions)
	if ($cvterm_id == $old_obj->type_id) {
	  $previous_db = tripal_db_set_active('chado');
  	db_query(
    	"UPDATE stockprop SET type_id=%d, value='%s' WHERE stockprop_id=%d",
    	$cvterm_id, 
    	$value,
    	$stockprop_id
  	);
  	tripal_db_set_active($previous_db);
  } else {
      //determine the rank for this property
    $max_rank = get_max_chado_rank('stockprop', 
    															array('stock_id'=>array('type'=>'INT','value'=> $old_obj->stock_id), 
    																		'type_id'=>array('type'=>'INT','value'=> $cvterm_id ) ));
    if ($max_rank == -1) { $rank = 0; 
    } else { $rank = $max_rank+1; }
	  $previous_db = tripal_db_set_active('chado');
  	db_query(
    	"UPDATE stockprop SET type_id=%d, value='%s', rank=%d WHERE stockprop_id=%d",
    	$cvterm_id, 
    	$value,
    	$rank,
    	$stockprop_id
  	);
  	tripal_db_set_active($previous_db);  	
  }

  // Set Preferred Synonym
  //use update node form so that both title and name get set                                                                                                                                                                       
  if ($preferred) {
    $node = node_load($nid);                                                                                                                                                                            
    $node->title = $value;                                                                                                                                                                              
    $node_form_state = array(                                                                                                                                                                                                        
      'values' => array(                                                                                                                                                                                                             
        'title' => $value,                                                                                                                                               
        'op' => 'Save'                                                                                                                                                                                
      )                                                                                                                                                                                                            
    );                                                                                                                                                                                                                               
    module_load_include('inc', 'node', 'node.pages');                                                                                                                                                                                
    drupal_execute('chado_stock_node_form', $node_form_state, $node);  
  }
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_delete_property($stockprop_id) {

  $previous_db = tripal_db_set_active('chado');
  db_query(
    "DELETE FROM stockprop WHERE stockprop_id=%d",
    $stockprop_id
  );
  tripal_db_set_active($previous_db);

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function theme_tripal_stock_edit_ALL_properties_form ($form) {
  $output = '';

  $output .= '<br><fieldset>';
  $output .= '<legend>Edit Already Existing Properties<span class="form-optional" title="This field is optional">(optional)</span></legend>';
  $output .= '<p>Below is a list of already existing properties for this stock, one property per line. The type refers to the type of '
  	     .'property and the value is the value for that property. For example, if this stock has a seed coat colour of green then '
	     .'the property type=sead coat colour and the value=green. When the type of property is synonym, there is an extra checkbox '
	     .'allowing you to specify which is the <b>Preferred Synonym</b>. This will change the current name of the stock.</p>';
  $output .= '<table>';
  $output .= '<tr><th>#</th><th>Type</th><th>Value</th><th></th></tr>';

  for ($i=1; $i<=$form['num_properties']['#value']; $i++) {
    $output .= '<tr><td>'.drupal_render($form["num-$i"]).'</td><td>'.drupal_render($form["type-$i"]).'</td><td>'.drupal_render($form["value-$i"]).drupal_render($form["preferred-$i"]).'</td><td>'.drupal_render($form["submit-$i"]).'</td></tr>';
  }

  $output .= '</table><br>';
  $output .= drupal_render($form);
  $output .= '</fieldset>';

  return $output;
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_list_properties_for_node($properties, $synonyms) {

  if (!empty($properties) OR !empty($synonyms) ) {
    $output = '<table>';
    $output .= '<tr><th>Type</th><th>Value</th></tr>';

    if (!empty($synonyms) ) {
      foreach ($synonyms as $s) {
        $output .= '<tr><td>synonym</td><td>'.$s.'</td></tr>';
      }
    }

    if (!empty($properties) ) {
      foreach ($properties as $p) {
        $output .= '<tr><td>'.$p->type.'</td><td>'.$p->value.'</td></tr>';
      } // end of foreach property
    }

    $output .= '</table>';

  } else {
    $output = 'No Properties Added to the Current Stock';
  }

  return $output;
}