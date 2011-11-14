<?php

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_add_ALL_relationships_page($node) {
  $output = '';

  $output .= tripal_stock_add_chado_properties_progress('relationships').'<br>';
  $output .= '<b>All Relationships should include the CURRENT Individual ('.$node->stock->uniquename.')</b><br>';
  $output .= '<br>';
  $output .= theme('tripal_stock_relationships', $node);
  $output .= '<br><br>';
  $output .= drupal_get_form('tripal_stock_add_ONE_relationship_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_stock_add_chado_properties_navigate', 'relationships', $node->nid);
  return $output;
}

/**
 * Implements Hook_form(): Handles adding of Relationships to Stocks
 *
 * @ingroup tripal_stock
 */
function tripal_stock_add_ONE_relationship_form($form_state, $node) {

  $stock_id = $node->stock->stock_id;
  $organism_id = $node->stock->organism_id->organism_id;
  $_SESSION['organism'] = $organism_id; //needed for autocomplete enter stock to work

  $form['rel_nid'] = array(
    '#type' => 'hidden', 
    '#value' => $node->nid
  ); 

  $form['add_relationships'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Relationships') . '<span class="form-optional" title="This field is optional">(optional)</span>',
  );

  $form['add_relationships']['description'] = array(
    '#type' => 'item',
    '#value' => t('Relationships are specified as follows: (Subject) (Type of Relationship) (Object). For example, Fred is_the_paternal_parent_of Matty, where Fred & Matty are both stocks.')
  );

  $form['add_relationships']['subject_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => 'The Uniquename, Name, Database Reference or Synonym of a Stock can be used here',
  );

  $type_options = tripal_cv_get_cvterm_options( variable_get('chado_stock_relationship_cv', 'null') );
  $type_options[0] = 'Select a Type';
  ksort($type_options);
  $form['add_relationships']['type_id'] = array(
    '#type' => 'select',
    '#title' => t('Type of Relationship'),
    '#options' => $type_options

  );

  $form['add_relationships']['object_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Object'),
    '#description' => 'The Uniquename, Name, Database Reference or Synonym of a Stock can be used here',
  );

  $form['add_relationships']['r_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Notes on the relationship') . '<span class="form-optional" title="This field is optional">+</span>',
    '#description' => t('Should not include Genotypes and Phenotypes'),
  );

  $form['add_relationships']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add a Relationship')
  );

  $form['add_relationships']['r_stock_id'] = array(
    '#type' => 'value',
    '#value' => $stock_id,
    '#required' => TRUE

  );

  $form['add_relationships']['r_stock_uniquename'] = array( 
    '#type' => 'value',
    '#value' => $node->stock->uniquename,
    '#required' => TRUE
  );

  return $form;

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_add_ONE_relationship_form_validate($form, &$form_state) {

  //Require Validation if adding
  if ($form_state['clicked_button']['#value'] == t('Add a Relationship') ) {
    // check valid stock selected for subject
    $subject_results = tripal_stock_get_stock_by_name_identifier( $form_state['values']['subject_id'], $_SESSION['organism']);
    if (sizeof($subject_results) > 1) {
      $links= array();
      for ($i=0; $i<sizeof($subject_results); $i++) { $links[] = l($i+1, "node/".$subject_results[$i]->nid); }
      $message = "Too many stocks match '".$form_state['values']['subject_id']."'! "
      	       	 . " Please refine your input to match ONLY ONE stock. <br>"
		 . "To aid in this process, here are the stocks that match your initial input: "
		 .join(', ',$links);
      form_set_error('subject_id', $message);
    } elseif (sizeof($subject_results) < 1) {
      form_set_error('subject_id', "There are no stocks matching your input. Please check your input for typos and/or lookup the stock ".l('here', 'stocks'));
    } elseif (sizeof($subject_results) == 1) {
      $form_state['values']['subject_id'] = $subject_results[0]->stock->stock_id;
    }

    // check valid stock selected for object
    $object_results = tripal_stock_get_stock_by_name_identifier( $form_state['values']['object_id'], $_SESSION['organism']);
    if (sizeof($object_results) > 1) {
      $links= array();
      for ($i=0; $i<sizeof($object_results); $i++) { $links[] = l($i+1, "node/".$object_results[$i]->nid); } 
      $message = "Too many stocks match '".$form_state['values']['object_id']."'! "
                 . "Please refine your input to match ONLY ONE stock. <br>"
                 . "To aid in this process, here are the stocks that match your initial input: "
                 .join(', ',$links);
      form_set_error('object_id', $message);
    } elseif (sizeof($object_results) < 1) {
      form_set_error('object_id', "There are no stocks matching your input. Please check your input for typos and/or lookup the stock ".l('here', 'stocks'));                                                                         
    } elseif (sizeof($object_results) == 1) {
      $form_state['values']['object_id'] = $object_results[0]->stock->stock_id;
    }

    // check valid type selected
    if ($form_state['values']['type_id'] == 0) {
      form_set_error('type_id', 'Please select a type of relationship.');
    } else {
    	$previous_db = tripal_db_set_active('chado');
    	$tmp_obj = db_fetch_object(db_query("SELECT count(*) as count FROM cvterm WHERE cvterm_id=%d",$form_state['values']['type_id']));
    	tripal_db_set_active($previous_db);
    	
      if ($tmp_obj->count != 1) {
        form_set_error('type_id', 'The type you selected is not valid. Please choose another one.');
      }
    }

    // check either subject or object is the current stock
    if ( $subject_results[0]->nid != $form_state['values']['rel_nid'] ) {
      if ( $object_results[0]->nid != $form_state['values']['rel_nid'] ) {
        form_set_error('subject_id', 'Either Subject or Object must be the current stock ('.$form_state['values']['r_stock_uniquename'].').');
      }
    }
  } //end of require validation if adding relationship
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_add_ONE_relationship_form_submit($form, &$form_state) {
  
  if ($form_state['values']['subject_id'] > 0) {
    $previous_db = tripal_db_set_active('chado');
    db_query(
      "INSERT INTO stock_relationship (subject_id, type_id, object_id, value) VALUES (%d, %d, %d, '%s')",
      $form_state['values']['subject_id'],
      $form_state['values']['type_id'],
      $form_state['values']['object_id'],
      $form_state['values']['r_description']
    );
    tripal_db_set_active($previous_db);

    drupal_set_message('Successfully Added Relationship.');
  } //end of insert relationship

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_edit_ALL_relationships_page($node) {
  $output = '';
	
  $output .= drupal_get_form('tripal_stock_edit_ALL_relationships_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_stock_add_ONE_relationship_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_stock_back_to_stock_button', $node->nid);

  return $output;
}
                                                      
/**                                                   
 * Implements Hook_form()                             
 *
 * @ingroup tripal_stock
 */                                                   
function tripal_stock_edit_ALL_relationships_form($form_state, $node) {
  $form = array();
  
  // All Stock Relationships
  $node = tripal_core_expand_chado_vars($node, 'table', 'stock_relationship');    
   
  // compile all relationships into one variable
  $relationships = array();
  if (is_array($node->stock->stock_relationship->subject_id)) {
    foreach ($node->stock->stock_relationship->subject_id as $r) {
      $relationships[$r->stock_relationship_id] = $r;
    }
  } elseif (is_object($node->stock->stock_relationship->subject_id)) { 
    $relationships[$node->stock->stock_relationship->subject_id->stock_relationship_id] = $node->stock->stock_relationship->subject_id;
  }

  if (is_array($node->stock->stock_relationship->object_id)) {
    foreach ($node->stock->stock_relationship->object_id as $r) {
      $relationships[$r->stock_relationship_id] = $r;
    }
  } elseif (is_object($node->stock->stock_relationship->object_id)) { 
    $relationships[$node->stock->stock_relationship->object_id->stock_relationship_id] = $node->stock->stock_relationship->object_id;
  }
    
  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );
  
  $form['r_stock_uniquename'] = array(
    '#type' => 'hidden',
    '#value' => $node->stock->uniquename
  );

  $i=0;
  if (sizeof($relationships) != 0) {
  foreach ($relationships as $r) {

    $i++;
    $form["num-$i"] = array(
      '#type' => 'item',
      '#value' => $i.'.'
    );

    $form["id-$i"] = array(
      '#type' => 'hidden',
      '#value' => $r->stock_relationship_id
    );

    //Enter relationship specific fields
    if ( $node->stock->stock_id != $r->subject_id->stock_id ) { 
      $default = $r->subject_id->uniquename;
      $description = l($r->subject_id->name, 'node/'.$r->subject_id->nid); 
    } else { $default = $node->stock->uniquename; $description = 'Current Stock'; }
    $form["subject_id-$i"] = array(
      '#type' => 'textfield',      
      //'#title' => t('Subject'), 
      '#required'   => TRUE,
      '#size' => 30,
      '#default_value' => $default,
      '#description' => $description,
    ); 

    $type_options = tripal_cv_get_cvterm_options( variable_get('chado_stock_relationship_cv', 'null') );
    ksort($type_options);          
    $form["type_id-$i"] = array(  
      '#type' => 'select',    
      //'#title' => t('Type of Relationship'), 
      '#options' => $type_options,
      '#required' => TRUE,
      '#default_value' => $r->type_id->cvterm_id
    );

    if ( $node->stock->stock_id != $r->object_id->stock_id ) { 
      $default = $r->object_id->uniquename;
      $description = l($r->object_id->name, 'node/'.$r->object_id->nid);
    } else { $default = $node->stock->uniquename; $description = 'Current Stock'; }
    $form["object_id-$i"] = array(
      '#type' => 'textfield',          
      //'#title' => t('Object'),      
      '#required'   => TRUE,
      '#size' => 30,
      '#default_value' => $default,
      '#description' => $description
    );

    $form["submit-$i"] = array(
      '#type' => 'submit',
      '#value' => t("Delete #$i")
    );

  }} //end of foreach relationship

  $form['num_relationships'] = array(
    '#type' => 'hidden',
    '#value' => $i
  );

  $form["submit-edits"] = array(
    '#type' => 'submit',
    '#value' => t('Update Relationships')
  );

  return $form;
}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_edit_ALL_relationships_form_validate($form, &$form_state) {

  // Only Require if Updating Relationships
  if ($form_state['clicked_button']['#value'] == t('Update Relationships') ) {

    for ($i=1; $i<=$form_state['values']['num_relationships']; $i++) {
      
      // check valid stock selected for subject
      $subject_results = tripal_stock_get_stock_by_name_identifier( $form_state['values']["subject_id-$i"], $_SESSION['organism']);
      if (sizeof($subject_results) > 1) {
        $links= array();
        for ($j=0; $j<sizeof($subject_results); $j++) { $links[] = l($j+1, "node/".$subject_results[$j]->nid); }
        $message = "Too many stocks match '".$form_state['values']["subject_id-$i"]."'! "
                 . "Please refine your input to match ONLY ONE stock. <br>" 
                 . "To aid in this process, here are the stocks that match your initial input: "
                 .join(', ',$links);
        form_set_error("subject_id-$i", $message);
      } elseif (sizeof($subject_results) < 1) { 
        form_set_error("subject_id-$i", "There are no stocks matching your input. Please check your input for typos and/or lookup the stock ".l('here', 'stocks'));
      } elseif (sizeof($subject_results) == 1) {
        $form_state['values']["subject_id-$i"] = $subject_results[0]->stock->stock_id;
      } 

      // check valid stock selected for object
      $object_results = tripal_stock_get_stock_by_name_identifier( $form_state['values']["object_id-$i"], $_SESSION['organism']);
      if (sizeof($object_results) > 1) {
        $links= array();
        for ($j=0; $j<sizeof($object_results); $j++) { $links[] = l($j+1, "node/".$object_results[$j]->nid); }
        $message = "Too many stocks match '".$form_state['values']["object_id-$i"]."'! "
                 . "Please refine your input to match ONLY ONE stock. <br>" 
                 . "To aid in this process, here are the stocks that match your initial input: "
                 .join(', ',$links);
        form_set_error("object_id-$i", $message);
      } elseif (sizeof($object_results) < 1) {
        form_set_error("object_id-$i", "There are no stocks matching your input. Please check your input for typos and/or lookup the stock ".l('here', 'stocks'));
      } elseif (sizeof($object_results) == 1) {
        $form_state['values']["object_id-$i"] = $object_results[0]->stock->stock_id;
      } 

      // check valid type selected
      if ($form_state['values']["type_id-$i"] == 0) {
        form_set_error('type_id', 'Please select a type of relationship.');
      } else {
    		$previous_db = tripal_db_set_active('chado');
    		$tmp_obj = db_fetch_object(db_query("SELECT count(*) as count FROM cvterm WHERE cvterm_id=%d",$form_state['values']["type_id-$i"]));
    		tripal_db_set_active($previous_db);
    	
      	if ($tmp_obj->count != 1) {
          form_set_error("type_id-$i", 'The type you selected is not valid. Please choose another one.');
        }
      }

      // check either subject or object is the current stock
      if ( $subject_results[0]->nid != $form_state['values']['nid'] ) {
        if ( $object_results[0]->nid != $form_state['values']['nid'] ) {
          form_set_error("subject_id-$i", 'Either Subject or Object must be the current stock ('.$form_state['values']['r_stock_uniquename'].').');
        }
      }

    } // end of for each relationship
  } //end of if updating relationships

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_edit_ALL_relationships_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == t('Update Relationships') ) {
     //Update all
     for ($i=1; $i<=$form_state['values']['num_relationships']; $i++) {

       //process stock textfields
       tripal_stock_update_relationship(
				$form_state['values']["id-$i"], 
				$form_state['values']["subject_id-$i"],
				$form_state['values']["type_id-$i"], 
				$form_state['values']["object_id-$i"]
			);
     }
     drupal_set_message("Updated all Relationships");
     drupal_goto('node/'.$form_state['values']['nid']);

  } elseif ( preg_match('/Delete #(\d+)/', $form_state['clicked_button']['#value'], $matches) ) {

     $i = $matches[1];
     tripal_stock_delete_relationship($form_state['values']["id-$i"]);
     drupal_set_message("Deleted Relationship");

  } elseif ($form_state['clicked_button']['#value'] == t('Back to Stock') ) {
    drupal_goto('node/'.$form_state['values']['nid']);
  } else {
    drupal_set_message("Unrecognized Button Pressed",'error');
  }

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_update_relationship ($stock_relationship_id, $subject_id, $cvterm_id, $object_id) {

  $previous_db = tripal_db_set_active('chado');
  db_query(
    "UPDATE stock_relationship SET subject_id=%d, type_id=%d, object_id=%d WHERE stock_relationship_id=%d",
    $subject_id,
    $cvterm_id, 
    $object_id,
    $stock_relationship_id
  );
  tripal_db_set_active($previous_db);

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function tripal_stock_delete_relationship ($stock_relationship_id) {

  $previous_db = tripal_db_set_active('chado');
  db_query(
    "DELETE FROM stock_relationship WHERE stock_relationship_id=%d",
    $stock_relationship_id
  );
  tripal_db_set_active($previous_db);

}

/**
 * 
 *
 * @ingroup tripal_stock
 */
function theme_tripal_stock_edit_ALL_relationships_form ($form) {
  $output = '';
  
  $output .= '<br><fieldset>';
  $output .= '<legend>Edit Already Existing Relationships<span class="form-optional" title="This field is optional">(optional)</span></legend>';
  $output .= '<p>Each relationship for this stock is listed below, one per line. The textboxes indicating '
  	    .'the subject and object of the relationship can contain the uniquename, name, database '
	    .'reference or synonym of a stock of the same organism.</p>';
  $output .= '<table>';
  $output .= '<tr><th>#</th><th>Subject</th><th>Type</th><th>Object</th><th></th></tr>';

  for ($i=1; $i<=$form['num_relationships']['#value']; $i++) {
    $output .= '<tr><td>'.drupal_render($form["num-$i"]).'</td><td>'
    	       .drupal_render($form["subject_id-$i"]).'</td><td>'
	       .drupal_render($form["type_id-$i"]).'</td><td>'
	       .drupal_render($form["object_id-$i"]).'</td><td>'
	       .drupal_render($form["submit-$i"]).'</td></tr>';
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
function tripal_stock_list_relationships_for_node($stock_name, $subject_relationships, $object_relationships) {

  if (!empty($subject_relationships) OR !empty($object_relationships) ) {
    $output = '<table>';
    $output .= '<tr><th>Subject</th><th>Relationship Type</th><th>Object</th></tr>';

    if (!empty($subject_relationships) ) {
      foreach ($subject_relationships as $s) {
        $output .= '<tr><td>'.$s->subject_name.'</td><td>'.$s->relationship_type.'</td><td>'.$stock_name.'</td></tr>';
      }
    }

    if (!empty($object_relationships) ) {
      foreach ($object_relationships as $o) {
        $output .= '<tr><td>'.$stock_name.'</td><td>'.$o->relationship_type.'</td><td>'.$o->object_name.'</td></tr>';
      } // end of foreach property
    }

    $output .= '</table>';
  } else {
    $output = 'No Relationships Involving the Current Stock'; 
  }

  return $output;

}