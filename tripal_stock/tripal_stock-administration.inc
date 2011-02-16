<?php
// $Id$

/*************************************************************************
 * Purpose: Provide administration options for chado_stocks
 *
 * @return form array (as described by the drupal form api)
 */
function tripal_stock_admin() {
  $form = array();

   // before proceeding check to see if we have any
   // currently processing jobs. If so, we don't want
   // to give the opportunity to sync Stocks
   $active_jobs = FALSE;
   if(tripal_get_module_active_jobs('tripal_stock')){
      $active_jobs = TRUE;
   }
   if($active_jobs){
   
   $form['notice'] = array(
         '#type' => 'fieldset',
         '#title' => t('Stock Management Temporarily Unavailable')
      );

   $form['notice']['message'] = array(
         '#value' => t("Currently, stock management jobs are waiting or ".
            "are running. Managemment features have been hidden until these ".
            "jobs complete.  Please check back later once these jobs have ".
            "finished.  You can view the status of pending jobs in the Tripal ".
            "jobs page."),
      );

   } else {

   // SET Vocabularies -----------------------------------------------------------------------------------------
   $form['set_cv'] = array(
      '#type' => 'fieldset',
      '#title' => t('Set Stock Controlled Vocabularies'),
      '#weight' => -10
   );   
   
   $form['set_cv']['message'] = array(
         '#value' => t("This setting allows you to set which chado controlled vocabularies (cv)"
	              ." are used. Cvs are used to control user input for the type of stock,"
		      ." any properties they enter for a stock & the types of relationships"
		      ." between stocks. Only cvs already loaded into chado can be selected here.")
   );

   $cv_options = tripal_cv_get_cv_options();
   
   $form['set_cv']['stock_types_cv'] = array(
     '#type' => 'select',
     '#title' => t('Controlled Vocabulary governing Stock Types'),
     '#options' => $cv_options,
     '#default_value' => variable_get('chado_stock_types_cv', 0)
   );

   $form['set_cv']['stock_prop_types_cv'] = array(
     '#type' => 'select',
     '#title' => t('Controlled Vocabulary governing Types of Stock Properties'),
     '#description' => t("This cv must contain a cvterm entry where name='synonym'."),
     '#options' => $cv_options,
     '#default_value' => variable_get('chado_stock_prop_types_cv', 0)
   );

   $form['set_cv']['stock_relationship_cv'] = array(
     '#type' => 'select',
     '#title' => t('Controlled Vocabulary governing Types of Relationsips between Stocks'),
     '#options' => $cv_options,
     '#default_value' => variable_get('chado_stock_relationship_cv', 0)
   );

   $form['set_cv']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Set Controlled Vacabularies')
   );

   // SYNC STOCKS-----------------------------------------------------------------------------------------------
   $form['sync'] = array(
      '#type' => 'fieldset',
      '#title' => t('Sync Stocks'),
      '#weight' => -10
   );

   $form['sync']['description'] = array(
      '#type' => 'item',
      '#value' => t("Click the 'Sync all Germplasm' button to create Drupal ".
         "content for stocks in chado. Depending on the ".
         "number of stocks in the chado database this may take a long ".
         "time to complete. ")
   );

   $form['sync']['organisms'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Organisms for which Stocks should be sync\'d'),
      '#description' => t('Only sync\'d Organisms are listed. Leaving an organism unchecked does not delete already sync\'d Stocks.'),
      '#options' => tripal_organism_get_organism_options(),
      '#required'    => FALSE,
      '#prefix'      => '<div id="lib_boxes">',
      '#suffix'      => '</div>'
   );

   $form['sync']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Sync Stocks')
   );
}

  return system_settings_form($form);

}

/**
 * Implements hook_form_submit()
 *
 * Handles Submit actions of all subforms
 */
function tripal_stock_admin_validate($form, &$form_state) {
   global $user;  // we need access to the user info
   $job_args = array();

   // Sync Stocks
   if ($form_state['values']['op'] == t('Sync Stocks')) {
      // Array organism_id => organims common_name
      //  which only includes those organisms which the user wants to select stocks for
      $organisms_2b_syncd = $form_state['values']['organisms'];

      //for each organism selected submit job (handled by tripal_stock_sync_stock_set)
      //  which syncs all stocks with an organism_id equal to the selelcted organism
      foreach ( $organisms_2b_syncd as $organism_id ) {
        if($organism_id != 0) {
          $job_args[0] = $organism_id;
          tripal_add_job("Sync Stocks from Organism $organism_id",'tripal_stock',
            'tripal_stock_sync_stock_set',$job_args,$user->uid);
        }
      }
    }

    if ($form_state['values']['op'] == t('Set Controlled Vacabularies')) {
      variable_set('chado_stock_types_cv', $form_state['values']['stock_types_cv']);
      variable_set('chado_stock_prop_types_cv', $form_state['values']['stock_prop_types_cv']);
      variable_set('chado_stock_relationship_cv', $form_state['values']['stock_relationship_cv']);
    }
}

/**************************************************************************************************************/
/** 
 * Syncs all Stocks associated with an organism
 * Note: Handling of multiple organisms is done in tripal_stock_admin_validate()
 *
 * @params $organisms
 */
 function tripal_stock_sync_stock_set($organism_id, $job_id) {
	global $user;

	if(!$organism_id) {
		print '0 Stocks to Sync -No Organisms Selected.\n';
	} else {

		// Get list of stocks to sync
		$previous_db = tripal_db_set_active('chado');
    $result = db_query(
       "SELECT stock_id, uniquename, type_id, organism_id FROM stock WHERE organism_id=%d",
       $organism_id
    );
    tripal_db_set_active($previous_db);

    $stocks_created_count = 0; //keeps track of total number of stocks successfully created
    $stocks_attempted = 0;
    // foreach stock to be sync'd -> create node & add stock_id
    while ( $r = db_fetch_object($result) ) {
      // $r is the current stock to be sync'd
      $stocks_attempted++;

      print 'Processing '.$r->uniquename."\n";

      // check not already in drupal
      $in_drupal_query = db_query(
      	"SELECT * FROM {chado_stock} WHERE stock_id=%d",
	 			$r->stock_id
      );
      if ( !db_fetch_object($in_drupal_query) ) {

        //create new chado_stock node
        $new_node = new stdClass();
	 			$new_node->type = 'chado_stock';
        $new_node->uid = $user->uid;
	 			$new_node->title = $r->uniquename;
	 			$new_node->type_id = $r->type_id;
	 			$new_node->organism_id = $r->organism_id;
	 			$new_node->stock_id = $r->stock_id;
	 			//print 'New Node:';
	 			//print_r($new_node);
	 			
	 			node_validate($new_node);
	 			
        if(!form_get_errors()){
	    		//print 'Try to Create Node ';
        	$node = node_submit($new_node);
          node_save($node);
          if($node->nid){
          	$stocks_created_count++;
	 
						//Add stock id to chado_stock table
	 					db_query(
	   	  			"UPDATE {chado_stock} SET stock_id=%d WHERE nid=%d AND vid=%d",
	   	  			$r->stock_id,
	   	  			$node->nid,
	   	  			$node->vid
	 					);
          }
        } else {
        	print "\tCreate Stock Form Errors: ";
	 				print_r(form_get_errors());
        }
	 			print "\n\tNid=".$node->nid."\n";
			} else {
				print "\tSkipped $r->uniquename because it's already in drupal.\n";
      } //end of if not already in drupal
    } //end of while still stocks to be sync'd
  } //end of if organism_id not supplied
   
  if ($stocks_attempted == 0) {
		print "No stocks retrieved for organism (".$organism_id.")\n";
		return 1;
	} else {
		if ($stocks_created_count > 0) {
			print "$stocks_created_count Stocks Successfully Created\n";
			return 1;
		} else {
			return 0;
		}
	}
}