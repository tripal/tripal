<?php

/*************************************************************************************************************
 * @section 
 * Deals with Adding DB references
 */

function tripal_stock_add_ALL_dbreferences_page($node) {
  $output = '';

  $output .= implement_add_chado_properties_progress('db_references').'<br>';
  $output .= '<b>All Database References should strictly pertain to THE CURRENT Individual</b><br>';
  $output .= '<br><b>Current Database References</b><br>';
  $output .= list_dbreferences_for_node($node->db_references);
  $output .= '<br><br>';
  $output .= drupal_get_form('tripal_stock_add_ONE_dbreference_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('implement_add_chado_properties_navigate', 'db_references', $node->nid);
  return $output;
}


/**
 * Implements Hook_form()
 * Handles adding of Database References to Stocks
 */
function tripal_stock_add_ONE_dbreference_form($form_state, $node) {

  $stock_id = $node->stock_id;

  $form['db_nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  $form['add_dbreference'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Database References') . '<span class="form-optional" title="This field is optional">(optional)</span>',
  );

  $form['add_properties']['db_stock_id'] = array(
    '#type' => 'value',
    '#value' => $stock_id,
    '#required' => TRUE

  );

  $form['add_dbreference']['accession'] = array(
    '#type' => 'textfield',
    '#title' => t('Accession Number'),
  );

  $form['add_dbreference']['db_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description of Database Reference') . '<span class="form-optional" title="This field is optional">+</span>',
    '#description' => t('Optionally enter a description about the database accession.')
  );

  $db_options = get_chado_db_options();
  $db_options[0] = 'Select a Database';
  ksort($db_options);
  $form['add_dbreference']['database'] = array(
    '#type' => 'select',
    '#title' => t('Database'),
    '#options' => $db_options,
  );

  $form['add_dbreference']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Database Reference')
  );

  return $form;
}

function tripal_stock_add_ONE_dbreference_form_validate($form, &$form_state) {

  // Only ensure db reference valid if adding
  if ($form_state['clicked_button']['#value'] == t('Add Database Reference') ) {
    //Do work of required validators
    if ($form_state['values']['accession'] == '') {
      form_set_error('accession', 'Accession field is Required.');
    }

    // Check database is valid db_id in chado
    if ( $form_state['values']['database'] > 0) {
    	$previous_db = tripal_db_set_active('chado');
    	$tmp_obj = db_fetch_object(db_query("SELECT count(*) as count FROM db WHERE db_id=%d",$form_state['values']['database']));
    	tripal_db_set_active($previous_db);
    	
      if ($tmp_obj->count != 1) {
        form_set_error('database', 'The database you selected is not valid. Please choose another one.'); 
      }
    } else {
      form_set_error('database', 'Please select a database');
    }

    // Check Accession is unique for database
    $previous_db = tripal_db_set_active('chado');
    $tmp_obj = db_fetch_object(db_query("SELECT count(*) as count FROM dbxref WHERE accession='%s'",$form_state['values']['accession']));
    tripal_db_set_active($previous_db);
    	
    if ($tmp_obj->count > 0) {
      form_set_error('accession', 'This accession has already been assigned to another stock.'); 
    }

  } //end of if adding

} 

function tripal_stock_add_ONE_dbreference_form_submit($form, &$form_state) {

  // FIX: Sometimes on programatic submission of form (drupal_execute) values in the form state get lost
  // 	  however, the post values always seem to be correct
  if (empty($form_state['values']['db_stock_id'])) { $form_state['values']['db_stock_id'] = $form_state['clicked_button']['#post']['db_stock_id']; }

  // Only Create if valid
  if ($form_state['values']['database'] > 0) {
    

    // create dbxref
    $previous_db = tripal_db_set_active('chado');
    db_query(
      "INSERT INTO dbxref (db_id, accession, description) VALUES (%d, '%s', '%s')",
      $form_state['values']['database'],
      $form_state['values']['accession'],
      $form_state['values']['db_description']
    );
    tripal_db_set_active($previous_db);

    //create stock_dbxref
    $dbxref = get_chado_dbxref( array('db_id'=>array('type'=>'INT','value'=>$form_state['values']['database']), 
    																	'accession'=>array('type'=>'STRING','exact'=>TRUE,'value'=>$form_state['values']['accession']) ) );
		if (!empty($dbxref->dbxref_id)) {
			$previous_db = tripal_db_set_active('chado');
    	db_query(
      	"INSERT INTO stock_dbxref (stock_id, dbxref_id) VALUES (%d, %d)",
      	$form_state['values']['db_stock_id'],
      	$dbxref->dbxref_id
    	);
			tripal_db_set_active($previous_db);

    	drupal_set_message('Successfully Added Database Reference');
    } else {
    	drupal_set_message('Database reference NOT successfully created...','error');
    } //end of if dbxref was created successfully
  } //end of if valid db reference

}

/************************************************************************************************************* 
 * @section                                    
 * Deals with Editing and Deleting Properties  
 */                                            

function tripal_stock_edit_ALL_dbreferences_page($node) {
  $output = '';

  $output .= drupal_get_form('tripal_stock_edit_ALL_db_references_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('tripal_stock_add_ONE_dbreference_form', $node);
  $output .= '<br>';
  $output .= drupal_get_form('implement_back_to_stock_button', $node->nid);

  return $output;
}
                                               
/**                                            
 * Implements Hook_form()                      
 * Handles adding of Properties & Synonyms to Stocks
 */                                        
function tripal_stock_edit_ALL_db_references_form($form_state, $node) {
  $form = array();

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid
  );

  $i=0;
  if (sizeof($node->db_references) != 0) {
  foreach ($node->db_references as $ref) {
    $i++;
    $form["num-$i"] = array(
      '#type' => 'item',
      '#value' => $i.'.'
    );

    $form["accession-$i"] = array(
      '#type' => 'textfield',
      //'#title' => t('Accession'),
      '#size' => 30,
      '#required' => TRUE,
      '#default_value' => $ref->accession
    );

    $db_options = get_chado_db_options();
    $db_options[0] = 'Select a Database';
    ksort($db_options);
    $form["database-$i"] = array( 
      '#type' => 'select', 
      //'#title' => t('Database'),
      '#options' => $db_options, 
      '#default_value' => $ref->db_id
    );


    $form["id-$i"] = array(
      '#type' => 'hidden',
      '#value' => $ref->dbxref_id
    );

    $form["submit-$i"] = array(
      '#type' => 'submit',
      '#value' => t("Delete #$i")
    );

  }} //end of foreach db ref

  $form['num_db_references'] = array(
    '#type' => 'hidden',
    '#value' => $i
  );

  $form["submit-edits"] = array(
    '#type' => 'submit',
    '#value' => t('Update DB References')
  );

  return $form;
}

function tripal_stock_edit_ALL_db_references_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == t('Update DB References') ) {

     //Update all
     for ($i=1; $i<=$form_state['values']['num_db_references']; $i++) {
       tripal_stock_update_db_reference(
					$form_state['values']["id-$i"], 
					$form_state['values']["database-$i"], 
					$form_state['values']["accession-$i"]
			);
     }
     drupal_set_message("Updated all Database References");
     drupal_goto('node/'.$form_state['values']['nid']);

  } elseif ( preg_match('/Delete #(\d+)/', $form_state['clicked_button']['#value'], $matches) ) {

     $i = $matches[1];
     tripal_stock_delete_db_reference($form_state['values']["id-$i"]);
     drupal_set_message("Deleted Database Reference");

  } else {
    drupal_set_message("Unrecognized Button Pressed",'error');
  }

}

function tripal_stock_update_db_reference($dbxref_id, $database_id, $accession) {

  $previous_db = db_set_active('chado');
  db_query(
    "UPDATE dbxref SET db_id=%d, accession='%s' WHERE dbxref_id=%d",
    $database_id, 
    $accession,
    $dbxref_id
  );
  db_set_active($previous_db);

}

function tripal_stock_delete_db_reference($dbxref_id) {

  $previous_db = db_set_active('chado');
  db_query(
    "DELETE FROM dbxref WHERE dbxref_id=%d",
    $dbxref_id
  );

  db_query(
    "DELETE FROM stock_dbxref WHERE dbxref_id=%d",
    $dbxref_id
  );
  db_set_active($previous_db);

}

function theme_tripal_stock_edit_ALL_db_references_form ($form) {
  $output = '';

  $output .= '<br><fieldset>';
  $output .= '<legend>Edit Existing Database References<span class="form-optional" title="This field is optional">(optional)</span></legend>';
  $output .= '<p>Below is a list of already existing database references, one per line. When entering a database reference, the accession '
  	     .'is a unique identifier for this stock in the specified database.</p>';
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

/*************************************************************************************************************
 * @section
 * Supplimentary functions
 */

/**
 * 
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
    $output = 'No Database References Added to the Current Stock';
  }              

  return $output;
}     