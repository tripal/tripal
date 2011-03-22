<?php


/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ALL_relationships_page($node) {
  $output = '';

  $output .= tripal_feature_implement_add_chado_properties_progress('relationships').'<br>';
  $output .= '<b>All Relationships should include the CURRENT Individual ('.$node->uniquename.')</b><br>';
  $output .= '<br><b>Current Relationships</b><br>';
  $output .= list_relationships_for_node($node->uniquename, $node->subject_relationships, $node->object_relationships);
  $output .= '<br><br>';
  $output .= drupal_get_form('tripal_feature_add_ONE_relationship_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_feature_implement_add_chado_properties_navigate', 'relationships', $node->nid);
  return $output;
}

/**
 * Implements Hook_form()
 * Handles adding of Relationships to Features
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_relationship_form($form_state, $node) {

  $feature_id = $node->feature_id;
  $organism_id = $node->organism->organism_id;
  $_SESSION['organism'] = $organism_id; //needed for autocomplete enter feature to work

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
    '#value' => t('Relationships are specified as follows: (Subject) (Type of Relationship) (Object). For example, X part_of Y, where X & Y are genomic features.')
  );

  $form['add_relationships']['subject_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => 'The Uniquename, Name, Database Reference or Synonym of a Feature can be used here',
  );

  $cv = tripal_cv_get_cv_by_name('relationship');
  $type_options = tripal_cv_get_cvterm_options($cv->cv_id);
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
    '#description' => 'The Uniquename, Name, Database Reference or Synonym of a Feature can be used here',
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

  $form['add_relationships']['r_feature_id'] = array(
    '#type' => 'value',
    '#value' => $feature_id,
    '#required' => TRUE

  );

  $form['add_relationships']['r_feature_uniquename'] = array( 
    '#type' => 'value',
    '#value' => $node->uniquename,
    '#required' => TRUE
  );

  return $form;

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_relationship_form_validate($form, &$form_state) {

  //Require Validation if adding
  if ($form_state['clicked_button']['#value'] == t('Add a Relationship') ) {

    // check valid feature selected for subject
    $criteria = array('unknown' => array('value'=> $form_state['values']['subject_id'], 
      																		'columns'=>array('name','uniquename','accession','synonym') ));
    $subject_results = get_chado_stocks($criteria,'ANY',$_SESSION['organism']);
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
      $form_state['values']['subject_id'] = $subject_results[0]->stock_id;
    }

    // check valid stock selected for object
    $criteria = array('unknown' => array('value'=> $form_state['values']['object_id'], 
      																		'columns'=>array('name','uniquename','accession','synonym') ));
    $object_results = get_chado_stocks($criteria,'ANY',$_SESSION['organism']);
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
      $form_state['values']['object_id'] = $object_results[0]->stock_id;
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
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_relationship_form_submit($form, &$form_state) {

  if ($form_state['values']['subject_id'] > 0) {
    $previous_db = db_set_active('chado');
    db_query(
      "INSERT INTO stock_relationship (subject_id, type_id, object_id, value) VALUES (%d, %d, %d, '%s')",
      $form_state['values']['subject_id'],
      $form_state['values']['type_id'],
      $form_state['values']['object_id'],
      $form_state['values']['r_description']
    );
    db_set_active($previous_db);

    drupal_set_message('Successfully Added Relationship.');
  } //end of insert relationship

}

/** 
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_relationships_page($node) {
  $output = '';
	
  $output .= drupal_get_form('tripal_feature_edit_ALL_relationships_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_feature_add_ONE_relationship_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_feature_implement_back_to_feature_button', $node->nid);

  return $output;
}
                                                      
/**                                                 
 * Implements Hook_form()                             
 * Handles adding of Properties & Synonyms to Stocks  
 *
 * @ingroup tripal_feature
 */                                                   
function tripal_feature_edit_ALL_relationships_form($form_state, $node) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );
  
  $form['r_feature_uniquename'] = array(
    '#type' => 'hidden',
    '#value' => $node->uniquename
  );

  $i=0;

  $feature = $node->feature;
  $orelationships = tripal_feature_load_relationships ($feature->feature_id,'as_object');
  $srelationships = tripal_feature_load_relationships ($feature->feature_id,'as_subject');
  $relationships = array_merge($orelationships,$srelationships);

  if (sizeof($relationships) != 0) {
     foreach ($relationships as $r) {

       $i++;
       $form["num-$i"] = array(
         '#type' => 'fieldset',
         '#title' => "Relationship $i",
       );

       $form["num-$i"]["id-$i"] = array(
         '#type' => 'hidden',
         '#value' => $r->stock_relationship_id
       );

       //Enter relationship specific fields
       if ( !empty($r->subject_id) ) { 
         $default = $r->subject_uniquename;
         $description = l($r->subject_name, 'node/'.$r->subject_nid); 
       } else { 
          $default = $node->uniquename; 
          $description = "Current Feature"; 
       }
       $description .= " (".$r->subject_type.")";
       $form["num-$i"]["subject_id-$i"] = array(
         '#type' => 'textfield',      
         //'#title' => t('Subject'), 
         '#required'   => TRUE,
         '#size' => 30,
         '#default_value' => $default,
         '#description' => $description,
       ); 

       $cv = tripal_cv_get_cv_by_name('relationship');
       $type_options = tripal_cv_get_cvterm_options($cv->cv_id);
       ksort($type_options);          
       $form["num-$i"]["type_id-$i"] = array(  
         '#type' => 'select',    
         //'#title' => t('Type of Relationship'), 
         '#options' => $type_options,
         '#required' => TRUE,
         '#default_value' => $r->relationship_type_id
       );

       if (!empty($r->object_id) ) { 
         $default = $r->object_uniquename;
         $description = l($r->object_name, 'node/'.$r->object_nid);
       } else { 
         $default = $node->uniquename; 
         $description = 'Current Feature'; 
       }
       $description .= " (".$r->object_type.")";
       $form["num-$i"]["object_id-$i"] = array(
         '#type' => 'textfield',          
         //'#title' => t('Object'),      
         '#required'   => TRUE,
         '#size' => 30,
         '#default_value' => $default,
         '#description' => $description
       );

       $form["num-$i"]["delete-$i"] = array(
         '#type' => 'submit',
         '#value' => t("Delete"),
         '#name' => "delete-$i",
       );

     } //end of foreach relationship
     $form['num_relationships'] = array(
       '#type' => 'hidden',
       '#value' => $i
     );

     $form["submit-edits"] = array(
       '#type' => 'submit',
       '#value' => t('Update All Relationships')
     );
  } else {
     $form["info"] = array(
       '#type' => 'markup',
       '#value' => t('No relationships currently exist for this feature.')
     );
  } 



  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_relationships_form_validate($form, &$form_state) {

  // Only Require if Updating Relationships
  if ($form_state['clicked_button']['#value'] == t('Update All Relationships') ) {

    for ($i=1; $i<=$form_state['values']['num_relationships']; $i++) {
      
      // check valid stock selected for subject
      $criteria = array('unknown' => array('value'=>$form_state['values']["subject_id-$i"], 
      																		'columns'=>array('name','uniquename','accession','synonym') ));
      $subject_results = get_chado_stocks($criteria,'ANY',$_SESSION['organism']);
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
        $form_state['values']["subject_id-$i"] = $subject_results[0]->stock_id;
      } 

      // check valid stock selected for object
      $criteria = array('unknown' => array('value'=> $form_state['values']["object_id-$i"], 
      																		'columns'=>array('name','uniquename','accession','synonym') ));
      $object_results = get_chado_stocks($criteria,'ANY',$_SESSION['organism']);
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
        $form_state['values']["object_id-$i"] = $object_results[0]->stock_id;
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
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_relationships_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == t('Update Relationships') ) {
     //Update all
     for ($i=1; $i<=$form_state['values']['num_relationships']; $i++) {

       //process stock textfields
       tripal_feature_update_relationship(
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
     tripal_feature_delete_relationship($form_state['values']["id-$i"]);
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
 * @ingroup tripal_feature
 */
function tripal_feature_update_relationship ($stock_relationship_id, $subject_id, $cvterm_id, $object_id) {

  $previous_db = db_set_active('chado');
  db_query(
    "UPDATE stock_relationship SET subject_id=%d, type_id=%d, object_id=%d WHERE stock_relationship_id=%d",
    $subject_id,
    $cvterm_id, 
    $object_id,
    $stock_relationship_id
  );
  db_set_active($previous_db);

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_relationship ($stock_relationship_id) {

  $previous_db = db_set_active('chado');
  db_query(
    "DELETE FROM stock_relationship WHERE stock_relationship_id=%d",
    $stock_relationship_id
  );
  db_set_active($previous_db);

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function theme_tripal_feature_edit_ALL_relationships_form ($form) {
  $output = '';
  
  $output .= '<br><fieldset>';
  $output .= '<legend>Edit Already Existing Relationships<span class="form-optional" title="This field is optional">(optional)</span></legend>';
  $output .= '<p>Each relationship for this stock is listed below, one per line. The textboxes indicating '
  	    .'the subject and object of the relationship can contain the uniquename, name, database '
	    .'reference or synonym of a stock of the same organism.</p>';
  $output .= '<table>';
  $output .= '<tr><th>#</th><th>Subject</th><th>Type</th><th>Object</th><th></th></tr>';

  for ($i=1; $i<=$form['num_relationships']['#value']; $i++) {
    $output .= '<tr><td>'.drupal_render($form["num-$i"]).'</td>'.
      	      '<td>'.drupal_render($form["subject_id-$i"]).'</td>'.
	            '<td>'.drupal_render($form["type_id-$i"]).'</td>'.
     	         '<td>'.drupal_render($form["object_id-$i"]).'</td>'.
	            '<td>'.drupal_render($form["submit-$i"]).'</td></tr>';
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
function tripal_feature_list_relationships_for_node($feature_name, $subject_relationships, $object_relationships) {

  if (!empty($subject_relationships) OR !empty($object_relationships) ) {
    $output = '<table>';
    $output .= '<tr><th>Subject</th><th>Relationship Type</th><th>Object</th></tr>';

    if (!empty($subject_relationships) ) {
      foreach ($subject_relationships as $s) {
        $output .= '<tr><td>'.$s->subject_name.'</td><td>'.$s->relationship_type.'</td><td>'.$feature_name.'</td></tr>';
      }
    }

    if (!empty($object_relationships) ) {
      foreach ($object_relationships as $o) {
        $output .= '<tr><td>'.$feature_name.'</td><td>'.$o->relationship_type.'</td><td>'.$o->object_name.'</td></tr>';
      } // end of foreach property
    }

    $output .= '</table>';
  } else {
    $output = 'No Relationships For the Current Feature'; 
  }

  return $output;

}
