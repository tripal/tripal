<?php
//
// Copyright 2009 Clemson University
//

/*************************************************************************
 * Purpose: Provide Guidance to new Tripal Admin
 *
 * @return HTML Formatted text
 */
function tripal_analysis_module_description_page() {
  $text = '';
  
  $text .= '<h3>Description:</h3>';
  $text .= '<p>TODO: Basic Description of this module including mention/link to the chado module</p>';

  $text .= '<h3>Post Installation Instructions:</h3>';
  $text .= '<p>TODO: Describe any post installation intructions here. You shouldalways include setting user permissions.</p>';
  
  
  $text .= '<h3>Features of this Module:</h3>';
  $text .= '<p>TODO: Discuss the Features of this module including links. Some features to consider are creating content, details pages/node content, editing/deleteing, basic listings and vies integration. See admin/tripal/tripal_stock for an example.</p>';
  
  return $text;
}

/*******************************************************************************
 * Administration page callbacks for the Tripal Analysis module
 * We have defined a hook_get_settings() function. When a sub-module
 * is enabled, we'll look for this function to provide a form for the
 * administrative setting.
 */
function tripal_analysis_admin() {
	// Create a new administrative form. We'll add main functions to the form
	// first (Sync, Reindex, Clean, Taxonify). Thereafter, any sub-module that
	// has a setting will be added.
	$form = array();

	// before proceeding check to see if we have any
	// currently processing jobs. If so, we don't want
	// to give the opportunity to sync libraries
	$active_jobs = FALSE;
	if(tripal_get_module_active_jobs('tripal_organism')){
		$active_jobs = TRUE;
	}

	// add the field set for syncing libraries
	if(!$active_jobs){
		// add the field set for syncing analyses
		get_tripal_analysis_admin_form_sync_set($form);
//		get_tripal_analysis_admin_form_reindex_set($form);
//		get_tripal_analysis_admin_form_taxonomy_set($form);
		get_tripal_analysis_admin_form_cleanup_set($form);
	} else {
		$form['notice'] = array(
		   '#type' => 'fieldset',
		   '#title' => t('Analysis Management Temporarily Unavailable')
		);
		$form['notice']['message'] = array(
          '#value' => t('Currently, analysis management jobs are waiting or are running. . Managemment features have been hidden until these jobs complete.  Please check back later once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.'),
		);
	}

	// Add sub-module settings. Pull all sub-module information from
	// {tripal_analysis} table
	$sql = "SELECT modulename FROM {tripal_analysis}";
	$result = db_query($sql);
	$counter = 0;  //keep track of the number of sub-modules
	while ($data = db_fetch_object($result)) {

		// Check if the hook_get_settings() function is already defined.
		$func = $data->modulename."_get_settings";
		$functions = get_defined_functions();
		$settings;
		foreach($functions['user'] as $function) {
			if ($function == $func) {
				$settings = $func();
			}
		}
		
		// Add sub-module's specific settings to the administrative view
		if ($settings) {
			// Define a fieldset for the sub-module
			$form["field$counter"] = array(
            '#type' => 'fieldset',
            '#title' => "$settings->title",
            '#collapsible' => TRUE
			);
			$form["field$counter"]["$settings->title"] = $settings->form;
		}
		$counter ++;
	}
	return system_settings_form($form);
}

/************************************************************************
 *
 */
function get_tripal_analysis_admin_form_taxonomy_set(&$form) {
	$form['taxonify'] = array(
      '#type' => 'fieldset',
      '#title' => t('Assign Drupal Taxonomy to Analysis Features')
	);

	// get the list of analyses
	$sql = "SELECT * FROM {Analysis} ORDER BY name";
	$previous_db = tripal_db_set_active('chado');  // use chado database
	$lib_rset = db_query($sql);
	tripal_db_set_active($previous_db);  // now use drupal database

	// iterate through all of the libraries
	$lib_boxes = array();
	while($analysis = db_fetch_object($lib_rset)){
		$lib_boxes[$analysis->analysis_id] = "$analysis->name";
	}

	$form['taxonify']['description'] = array(
       '#type' => 'item',
       '#value' => t("Drupal allows for assignment of \"taxonomy\" or catagorical terms to " .
          "nodes. These terms allow for advanced filtering during searching. This option allows ".
          "for setting taxonomy only for features that belong to the selected analyses below.  All other features will be unaffected.  To set taxonomy for all features in the site see the Feature Administration page."),
		 '#weight' => 1,
	);

	$form['taxonify']['tx-analyses'] = array (
     '#title'       => t('Analyses'),
     '#type'        => t('checkboxes'),
     '#description' => t("Check the analyses whose features you want to reset taxonomy.  Note: this list contains all analyses, even those that may not be synced."),
     '#required'    => FALSE,
     '#prefix'      => '<div id="lib_boxes">',
     '#suffix'      => '</div>',
     '#options'     => $lib_boxes,
     '#weight'      => 2
	);
	$form['taxonify']['tx-button'] = array(
      '#type' => 'submit',
      '#value' => t('Set Feature Taxonomy'),
      '#weight'      => 3
	);
}
/************************************************************************
 *
 */
function get_tripal_analysis_admin_form_reindex_set(&$form) {
	// define the fieldsets
	$form['reindex'] = array(
      '#type' => 'fieldset',
      '#title' => t('Reindex Analysis Features')
	);

	// get the list of libraries
	$sql = "SELECT * FROM {Analysis} ORDER BY name";
	$previous_db = tripal_db_set_active('chado');  // use chado database
	$lib_rset = db_query($sql);
	tripal_db_set_active($previous_db);  // now use drupal database

	// iterate through all of the libraries
	$lib_boxes = array();
	while($analysis = db_fetch_object($lib_rset)){
		$lib_boxes[$analysis->analysis_id] = "$analysis->name";
	}
	$form['reindex']['description'] = array(
       '#type' => 'item',
       '#value' => t("This option allows for reindexing of only those features that belong to the selected analyses below. All other features will be unaffected.  To reindex all features in the site see the Feature Administration page."),
		 '#weight' => 1,
	);

	$form['reindex']['re-analyses'] = array (
     '#title'       => t('Libraries'),
     '#type'        => t('checkboxes'),
     '#description' => t("Check the analyses whoee features you want to reindex. Note: this list contains all analyses, even those that may not be synced."),
     '#required'    => FALSE,
     '#prefix'      => '<div id="lib_boxes">',
     '#suffix'      => '</div>',
     '#options'     => $lib_boxes,
     '#weight' => 2,
	);
	$form['reindex']['re-button'] = array(
      '#type' => 'submit',
      '#value' => t('Reindex Features'),
      '#weight' => 3,
	);
}
/************************************************************************
 *
 */
function get_tripal_analysis_admin_form_cleanup_set(&$form) {
	$form['cleanup'] = array(
      '#type' => 'fieldset',
      '#title' => t('Clean Up')
	);
	$form['cleanup']['description'] = array(
       '#type' => 'item',
       '#value' => t("With Drupal and chado residing in different databases ".
          "it is possible that nodes in Drupal and analyses in Chado become ".
          "\"orphaned\".  This can occur if an analysis node in Drupal is ".
          "deleted but the corresponding chado analysis is not and/or vice ".
          "versa. Click the button below to resolve these discrepancies."),
       '#weight' => 1,
	);
	$form['cleanup']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Clean up orphaned analyses'),
      '#weight' => 2,
	);
}
/************************************************************************
 *
 */
function get_tripal_analysis_admin_form_sync_set (&$form) {
	// define the fieldsets
	$form['sync'] = array(
      '#type' => 'fieldset',
      '#title' => t('Sync Analyses')
	);

	// before proceeding check to see if we have any
	// currently processing jobs. If so, we don't want
	// to give the opportunity to sync analyses
	$active_jobs = FALSE;
	if(tripal_get_module_active_jobs('tripal_analysis')){
		$active_jobs = TRUE;
	}

	if(!$active_jobs){
			
		// get the list of analyses
		$sql = "SELECT * FROM {analysis} ORDER BY name";
		$previous_db = tripal_db_set_active('chado');  // use chado database
		$ana_rset = db_query($sql);
		tripal_db_set_active($previous_db);  // now use drupal database
			
		// if we've added any analyses to the list that can be synced
		// then we want to build the form components to allow the user
		// to select one or all of them.  Otherwise, just present
		// a message stating that all analyses are currently synced.
		$ana_boxes = array();
		$added = 0;
		while($analysis = db_fetch_object($ana_rset)){
			// check to see if the analysis is already present as a node in drupal.
			// if so, then skip it.
			$sql = "SELECT * FROM {chado_analysis} WHERE analysis_id = %d";
			if(!db_fetch_object(db_query($sql,$analysis->analysis_id))){
				$ana_boxes[$analysis->analysis_id] = "$analysis->name";
				$added++;
			}
		}

		// if we have analyses we need to add to the checkbox then
		// build that form element
		if($added > 0){
			$ana_boxes['all'] = "All analyses";

			$form['sync']['analyses'] = array (
           '#title'       => t('Available analyses'),
           '#type'        => t('checkboxes'),
           '#description' => t("Check the analyses you want to sync.  Drupal ".
              "content will be created for each of the analyses listed above. ".
              "Select 'All analyses' to sync all of them."),
           '#required'    => FALSE,
           '#prefix'      => '<div id="ana_boxes">',
           '#suffix'      => '</div>',
           '#options'     => $ana_boxes,
			);
			$form['sync']['button'] = array(
            '#type' => 'submit',
            '#value' => t('Submit Sync Job')
			);
		}
		// we don't have any analyses to select from
		else {
			$form['sync']['value'] = array(
            '#value' => t('All analyses in Chado are currently synced with Drupal.')
			);
		}
	}
	// we don't want to present a form since we have an active job running
	else {
		$form['sync']['value'] = array(
          '#value' => t('Currently, jobs exist related to chado analyses. Please check back later for analyses that can by synced once these jobs have finished.  You can view the status of pending jobs in the Tripal jobs page.')
		);
	}
}
/************************************************************************
 *
 */
function tripal_analysis_admin_validate($form, &$form_state) {
	global $user;  // we need access to the user info
	$job_args = array();

	if ($form_state['values']['op'] == t('Submit Sync Job')) {

		// check to see if the user wants to sync chado and drupal.  If
		// so then we need to register a job to do so with tripal
		$analyses = $form_state['values']['analyses'];
		$do_all = FALSE;
		$to_sync = array();

		foreach ($analyses as $analysis_id){
			if(preg_match("/^all$/i",$analysis_id)){
				$do_all = TRUE;
			}
			if($analysis_id and preg_match("/^\d+$/i",$analysis_id)){
				// get the list of analyses
				$sql = "SELECT * FROM {analysis} WHERE analysis_id = %d";
				$previous_db = tripal_db_set_active('chado');  // use chado database
				$analysis = db_fetch_object(db_query($sql,$analysis_id));
				tripal_db_set_active($previous_db);  // now use drupal database
				$to_sync[$analysis_id] = $analysis->name;
			}
		}

		// submit the job the tripal job manager
		if($do_all){
			tripal_add_job('Sync all analyses','tripal_analysis','tripal_analysis_sync_analyses',$job_args,$user->uid);
		}
		else{
			foreach($to_sync as $analysis_id => $name){
				$job_args[0] = $analysis_id;
				tripal_add_job("Sync analysis: $name",'tripal_analysis','tripal_analysis_sync_analyses',$job_args,$user->uid);
			}
		}
	}
	// -------------------------------------
	// Submit the Reindex Job if selected
	if ($form_state['values']['op'] == t('Reindex Features')) {
		global $user;  // we need access to the user info
		$job_args = array();
		$analyses = $form_state['values']['re-analyses'];
		foreach ($analyses as $analysis_id){
			if($analysis_id and preg_match("/^\d+$/i",$analysis_id)){
				// get the analysis info
				$sql = "SELECT * FROM {analysis} WHERE analysis_id = %d";
				$previous_db = tripal_db_set_active('chado');  // use chado database
				$analysis = db_fetch_object(db_query($sql,$analysis_id));
				tripal_db_set_active($previous_db);  // now use drupal database
				$job_args[0] = $analysis_id;
				tripal_add_job("Reindex features for analysis: $analysis->name",'tripal_analysis',
             'tripal_analysis_reindex_features',$job_args,$user->uid);
			}
		}
	}

	// -------------------------------------
	// Submit the Taxonomy Job if selected
	if ($form_state['values']['op'] == t('Set Feature Taxonomy')) {
		global $user;  // we need access to the user info
		$job_args = array();
		$analyses = $form_state['values']['tx-analyses'];
		foreach ($analyses as $analysis_id){
			if($analysis_id and preg_match("/^\d+$/i",$analysis_id)){
				// get the analysis info
				$sql = "SELECT * FROM {analysis} WHERE analysis_id = %d";
				$previous_db = tripal_db_set_active('chado');  // use chado database
				$analysis = db_fetch_object(db_query($sql,$analysis_id));
				tripal_db_set_active($previous_db);  // now use drupal database
				$job_args[0] = $analysis_id;
				tripal_add_job("Set taxonomy for features in analysis: $analysis->name",'tripal_analysis',
             'tripal_analysis_taxonify_features',$job_args,$user->uid);
			}
		}
	}

	// -------------------------------------
	// Submit the Cleanup Job if selected
	if ($form_state['values']['op'] == t('Clean up orphaned analyses')) {
		tripal_add_job('Cleanup orphaned analyses','tripal_analysis',
         'tripal_analyses_cleanup',$job_args,$user->uid);
	}

	// -------------------------------------
	// Save blast regular expression settings
	if ($form_state['values']['op'] == t('Save settings')) {
		$db_id = $form_state['values']['blastdb'];
		$name = $form_state['values']['displayname'];
		$gbstyle = $form_state['values']['gb_style_parser'];
		$reg1 = $form_state['values']['hit_id'];
		$reg2 = $form_state['values']['hit_def'];
		$reg3 = $form_state['values']['hit_accession'];
		
		// Check if the blast settings exists
		
		$sql = "SELECT db_id FROM {tripal_analysis_blast} WHERE db_id=%d";
		$check = db_result(db_query($sql, $db_id));

		// If setting exists, update it
		if ($check) {
			$sql = "UPDATE {tripal_analysis_blast} ".
   	          "SET displayname = '%s', ".
   	          "    regex_hit_id = '%s', ".
   	          "    regex_hit_def = '%s', ".
   	          "    regex_hit_accession = '%s', ".
			       "    genbank_style = %d ".
   	          "WHERE db_id=%d";
			db_query($sql, $name, $reg1, $reg2, $reg3, $gbstyle, $db_id);
			// Otherwise, insert a new setting for the db
		} else {
			$sql = "INSERT INTO {tripal_analysis_blast} (db_id, displayname, ".
   		       "           regex_hit_id, regex_hit_def, regex_hit_accession, genbank_style) ".
   		       "VALUES (%d, '%s', '%s', '%s', '%s', %d) ";
			db_query($sql, $db_id, $name, $reg1, $reg2, $reg3, $gbstyle);
		}
	}
}

