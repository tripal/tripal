<?php

/**
 * 
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ALL_dbreferences_page($node) {
  $output = '';

  $output .= tripal_feature_implement_add_chado_properties_progress('db_references').'<br>';
  $output .= '<b>All Database References should strictly pertain to THE CURRENT Individual</b><br>';
  $output .= '<br><b>Current Database References</b><br>';
  $output .= list_dbreferences_for_node($node->db_references);
  $output .= '<br><br>';
  $output .= drupal_get_form('tripal_feature_add_ONE_dbreference_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_feature_implement_add_chado_properties_navigate', 'db_references', $node->nid);
  return $output;
}

/**
 * Implements Hook_form()
 * Handles adding of Database References to features
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_dbreference_form($form_state, $node) {

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  $form['feature_id'] = array(
    '#type' => 'hidden',
    '#value' => $node->feature->feature_id,
  );

  $form['add_dbreference'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Database References') . '<span class="form-optional" title="This field is optional">(optional)</span>',
  );

  $form['add_dbreference']['accession'] = array(
    '#type' => 'textfield',
    '#title' => t('Accession'),
    '#required' => TRUE,
  );

  $form['add_dbreference']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description of Reference') . '<span class="form-optional" title="This field is optional">+</span>',
    '#description' => t('Optionally enter a description about the database accession.'),
  );

  $db_options = tripal_db_get_db_options();
  $db_options[0] = 'Select a Database';
  ksort($db_options);
  $form['add_dbreference']['db_id'] = array(
    '#type' => 'select',
    '#title' => t('Database'),
    '#options' => $db_options,
    '#required' => TRUE,
  );

  $form['add_dbreference']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Database Reference')
  );

  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_dbreference_form_validate($form, &$form_state) {

   $db_id = $form_state['values']['db_id'];
   $accession = $form_state['values']['accession'];
   $description = $form_state['values']['description'];
   $feature_id = $form_state['values']['feature_id'];
   $nid = $form_state['values']['nid'];

   // Check database is valid db_id in chado
   $previous_db = tripal_db_set_active('chado');
   $tmp_obj = db_fetch_object(db_query("SELECT count(*) as count FROM db WHERE db_id=%d",$db_id));
   tripal_db_set_active($previous_db);    	
   if ($tmp_obj->count != 1) {
      form_set_error('database', 'The database you selected is not valid. Please choose another one.'); 
   }

   // Check Accession is unique for database
   $previous_db = tripal_db_set_active('chado');
   $sql = "SELECT count(*) as count FROM dbxref WHERE accession='%s' and db_id = %d";
   $tmp_obj = db_fetch_object(db_query($sql,$accession,$db_id));
   tripal_db_set_active($previous_db);

   if ($tmp_obj->count > 0) {
      form_set_error('accession', 'This accession has already been assigned to another feature in the selected database.'); 
   }

} 

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_add_ONE_dbreference_form_submit($form, &$form_state) {

   $db_id = $form_state['values']['db_id'];
   $accession = $form_state['values']['accession'];
   $description = $form_state['values']['description'];
   $feature_id = $form_state['values']['feature_id'];
   $nid = $form_state['values']['nid'];

   // create dbxref
   $previous_db = tripal_db_set_active('chado');
   $isql =  "INSERT INTO dbxref (db_id, accession, description) VALUES (%d, '%s', '%s')";
   db_query($isql,$db_id,$accession,$description);
   tripal_db_set_active($previous_db);

   //create feature_dbxref
   $dbxref = tripal_db_get_dbxref( array('db_id'=>array('type'=>'INT','value'=>$form_state['values']['db_id']), 
 											'accession'=>array('type'=>'STRING','exact'=>TRUE,'value'=>$form_state['values']['accession']) ) );

	if (!empty($dbxref->dbxref_id)) {
		$previous_db = tripal_db_set_active('chado');
      $isql = "INSERT INTO feature_dbxref (feature_id, dbxref_id) VALUES (%d, %d)";
 	   db_query($isql,$feature_id,$dbxref->dbxref_id);
		tripal_db_set_active($previous_db);
    	drupal_set_message('Successfully Added Database Reference');
      drupal_goto('node/'.$nid);
   } else {
 	   drupal_set_message('Database reference NOT successfully created...','error');
   } //end of if dbxref was created successfully

}


/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_dbreferences_page($node) {
  $output = '';

  $output .= drupal_get_form('tripal_feature_edit_ALL_db_references_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_feature_add_ONE_dbreference_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_feature_implement_back_to_feature_button', $node->nid);

  return $output;
}

/**                                           
 * Implements Hook_form()                      
 * Handles adding of DB References to Features
 *
 * @ingroup tripal_feature
 */                                        
function tripal_feature_edit_ALL_db_references_form($form_state, $node) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  $i=0;

  $feature = $node->feature;
  $references = tripal_feature_load_references ($feature->feature_id);

  // pre populate the database options
  $db_options = tripal_db_get_db_options();
  $db_options[0] = 'Select a Database';
  ksort($db_options);

  if (sizeof($references) != 0) {
     foreach ($references as $ref) {
       $i++;
       $form["num-$i"] = array(
         '#type' => 'fieldset',
         '#title' => t("Database Reference")." $i"
       );

       $form["num-$i"]["accession-$i"] = array(
         '#type' => 'textfield',
         '#title' => t('Accession'),
         '#size' => 30,
         '#required' => TRUE,
         '#default_value' => $ref->accession
       );

       $form["num-$i"]["db_id-$i"] = array( 
         '#type' => 'select', 
         '#title' => t('Database'),
         '#options' => $db_options,
         '#required' => TRUE, 
         '#default_value' => $ref->db_id
       );


       $form["num-$i"]["dbxref_id-$i"] = array(
         '#type' => 'hidden',
         '#value' => $ref->dbxref_id
       );

       $form["num-$i"]["delete-$i"] = array(
         '#type' => 'submit',
         '#value' => t("Delete"),
         '#name' => "delete-$i",
       );

     }

     $form['num_db_references'] = array(
       '#type' => 'hidden',
       '#value' => $i
     );

     $form["submit-edits"] = array(
       '#type' => 'submit',
       '#value' => t('Update All References')
     );
  } //end of foreach db ref
  return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_edit_ALL_db_references_form_submit($form, &$form_state) {

  $num_refs = $form_state['values']['num_db_references'];
  $action = $form_state['clicked_button']['#value'];
  $button = $form_state['clicked_button']['#name'];
  $nid = $form_state['values']['nid'];

  if (strcmp($action,'Update All References')==0) {
     for ($i=1; $i<=$num_refs; $i++) {
		 $dbxref_id = $form_state['values']["dbxref_id-$i"]; 
		 $db_id = $form_state['values']["db_id-$i"];
		 $accession = $form_state['values']["accession-$i"];
       tripal_feature_update_db_reference($dbxref_id,$db_id,$accession);
     }
     drupal_set_message("Updated all Database References");
     drupal_goto('node/'.$nid);
  } 
  elseif (strcmp($action,'Delete')==0){
     if(preg_match('/delete-(\d+)/', $button, $matches) ) {
        $i = $matches[1];
	     $dbxref_id = $form_state['values']["dbxref_id-$i"]; 
        tripal_feature_delete_db_reference($dbxref_id);
        drupal_set_message("Deleted Database Reference");
        drupal_goto('node/'.$nid);
     } else {
        drupal_set_message("Could not remove database reference: ",'error');
     }
  } 
  else {
    drupal_set_message("Unrecognized Button Pressed",'error');
  }

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_update_db_reference($dbxref_id, $db_id, $accession) {

  $previous_db = tripal_db_set_active('chado');
  $sql =  "UPDATE dbxref SET db_id=%d, accession='%s' WHERE dbxref_id=%d";
  db_query($sql,$db_id,$accession,$dbxref_id);
  tripal_db_set_active($previous_db);

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_delete_db_reference($dbxref_id) {

  $previous_db = tripal_db_set_active('chado');
  db_query(
    "DELETE FROM dbxref WHERE dbxref_id=%d",
    $dbxref_id
  );

  db_query(
    "DELETE FROM feature_dbxref WHERE dbxref_id=%d",
    $dbxref_id
  );
  tripal_db_set_active($previous_db);

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function theme_tripal_feature_edit_ALL_db_references_form ($form) {
  $output = '';

  $output .= '<br><fieldset>';
  $output .= '<legend>Edit Existing Database References<span class="form-optional" title="This field is optional">(optional)</span></legend>';
  $output .= '<p>Below is a list of already existing database references, one per line. When entering a database reference, the accession '
  	     .'is a unique identifier for this feature in the specified database.</p>';
  $output .= '<table>';
  $output .= '<tr><th>#</th><th>Database</th><th>Accession</th><th></th></tr>';

  for ($i=1; $i<=$form['num_db_references']['#value']; $i++) {
    $output .= '<tr><td>'.drupal_render($form["num-$i"]).'</td><td>'
    	       .drupal_render($form["database-$i"]).'</td><td>'
	       .drupal_render($form["accession-$i"]).'</td><td>'
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
 * @ingroup tripal_feature
 */
function list_dbreferences_for_node($db_references) {

  if (!empty($db_references) ) {
    $output = '<table>';
    $output .= '<tr><th>Database</th><th>Accession</th></tr>';

    foreach ($db_references as $db) {
        $output .= '<tr><td>'.$db->db_name.'</td><td>'.$db->accession.'</td></tr>';
    } // end of foreach db reference 

    $output .= '</table>'; 

  } else {
    $output = 'No Database References Added to the Current Feature';
  }              

  return $output;
}     
