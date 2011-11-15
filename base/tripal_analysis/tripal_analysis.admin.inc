<?php
//
// Copyright 2009 Clemson University
//

/**
 * Purpose: Provide Guidance to new Tripal Admin
 *
 * @return 
 *   HTML Formatted text
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_module_description_page() {

  $text .= '<h3>Tripal Analysis Administrative Tools Quick Links:</h3>';
  $text .= "<ul>
             <li><a href=\"".url("admin/tripal/tripal_analysis/configuration") . "\">Analysis Configuration</a></li>
           </ul>";
 

  $text .= '<h3>Module Description:</h3>';
  $text .= '<p>The Tripal Analysis module provides a generic analysis content type that is intended to be
            used when a more specialized analysis module does not exist.  Because it is generic, it does not
            provide any visualization or data loading tools for analysis data.  Specialized analysis modules,
            such as the Tripal Analysis Blast or Tripal Analysis KEGG modules provide loading and custom
            visualizations for the analysis results.  These modules must be installed separately.
            </p>';

  $text .= '<h3>Setup Instructions:</h3>';
  $text .= '<p>After installation of the analysis module or any specialized analysis module.  The following tasks should be performed
            <ol>
              <li><p><b>Set Permissions</b>: Each analysis module supports the Drupal user permissions interface for 
               controlling access to the content and functions. These permissions include viewing, 
               creating, editing or administering of
               analysis content. The default is that only the original site administrator has these 
               permissions.  You can <a href="'.url('admin/user/roles').'">add roles</a> for classifying users, 
               <a href="'.url('admin/user/user').'">assign users to roles</a> and
               <a href="'.url('admin/user/permissions').'">assign permissions</a> for the analysis content to 
               those roles.  For a simple setup, allow anonymous users access to view organism content and 
               allow the site administrator all other permissions.</p></li>


               <li><p><b>Create Analysis</b>:  An analysis should be <a href="'.url('node/add').'">created</a> before data is imported into
               chado.  The generic analysis type should only be used when a more specialized analysis module
               (e.g. Tripal Analysis Blast module) does not already exists.  All data imported into 
               Chado should be associated with an analysis.    

               <li><p><b>Sync Analyses</b>:  If Chado has preloaded analyses then you can sync those.  This process is what
               creates the pages for viewing an analysis on the site.  Analyses can be synced using the 
               <a href="'.url('admin/tripal/tripal_analysis/configuration').'">Analysis Configuration page</a>. 
               However, syncing an analyses will always create a generic analysis content type.  If you would like
               to use a specialized analysis module for visualization of data then do not sync the analysis but recreate it
               using the appropriate specialized analysis content type.</p></li>
 
            </ol>
            </p>';
  
  
  $text .= '<h3>Features of this Module:</h3>';
  $text .= '<p>Aside from providing a generic content type the Tripal Analysis module also provides the following functionality
            <ul>
            
              <li><p><b>Basic Analysis Lookup View</b>: This module provides a basic <a href="'.url('analyses').'">analysis search 
              tool</a> for finding or listing analyses in Chado. It does not require indexing for Drupal searching but relies
              on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a> must be installed. </p></li>              

            </ul>
            </p>';

   $text .= '<h3>Page Customizations</h3>';
   $text .= '<p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal. 
             Below is a description of several methods.  These methods may be used in conjunction with one another to
             provide fine-grained control. 
             <ul>

             <li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a> 
              allows for customization of a page layout if you don\'t want to do PHP/Javascript/CSS programming.  
              Tripal comes with pre-set layouts for analysis pages.  However, 
              Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
              is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the 
              Panel\'s GUI.</p></li>

             <li><p><b>Drupal\'s Content Construction Kit (CCK)</b>: the 
             <a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
             to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
             With CCK, the site administartor can create a new field to appear on the page.  For example, currently,
             the Chado publication module is not yet supported by Tripal.  Therefore, the site administrator can add a text 
             field to the analysis pages.  This content is not stored in Chado, but will appear on the analysis page.  A field
             added by CCK will also appear in the form when editing a analysis to allow users to manually enter the appropriate
             text.  If the default pre-set layout and themeing for Tripal is used, it is better to create the CCK element,
             indicate that it is not to be shown (using the CCK interface), then manually add the new content type 
             where desired by editing the templates (as described below).  If using Panels, the CCK field can be added to the
             location desired using the Panels interface.</p></li>

             <li><p><b>Drupal Node Templates</b>:  The Tripal packages comes with a "theme_tripal" directory that contains the
             themeing for Chado content.    The analysis module has a template file for analysis "nodes" (Tripal analysis pages).  This file
             is named "node-chado_analysis.tpl.php", and provides javascript, HTML and PHP code for display of the analysis
             pages.  Specialized analysis modules will have their own template files as well, such as "node-chado_analysis-blast.tpl.php" for the
             Tripal Analysis Blast module.  You can edit the template file to control which types of information (or which analysis "blocks") are displayed 
             for analysis. Be sure to 
             copy these template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations. See the <a href="http://tripal.sourceforge.net/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template file.</p></li>

             <li><p><b>Analysis "Block" Templates</b>:  In the "theme_tripal" directory are subdirectories named after each tripal module (e.g. "tripal_feature", "tripal_library", etc.).
             Inside each directory is a set of templates that control distinct types of information for each content type.  For example,
             there is a "base" template for displaying of data directly from the Chado feature table, and a "references" 
             template for showing external site references for a feature (data from the feature_dbxref table). 
              These templates are used both by Drupal blocks
             for use in Drupal Panels (as described above) or for use in the default pre-set layout that the node template 
             provides (also desribed above).  Analyses block templates can exist in any of these directories.  For example, the Tripal Analysis Unigene
             module uses templates in the tripal_analysis_unigene, tripal_organism, and tripal_feature directories.  Content for a unigene is then
             cusotmizable within each of these contexts.
             You can customize block template as you desire.  Be sure to copy the
             template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations.  See the <a href="http://tripal.sourceforge.net/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template files.</p></li>
             </li>

             <li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
             will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
             using Javascript for all of the analysis "Blocks" that appear on the page. If you want to add additional links 
             (e.g. a dynamic link to GBrowse for the analysis) and you want that link to appear in the 
             "Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the 
             section at the bottom of the template file where the resources section is found.</p></li>

             </ul>
             </p>';

  return $text;
}

/*******************************************************************************
 * Administration page callbacks for the Tripal Analysis module
 * We have defined a hook_get_settings() function. When a sub-module
 * is enabled, we'll look for this function to provide a form for the
 * administrative setting.
 *
 * @ingroup tripal_analysis
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

/**
 *
 * @ingroup tripal_analysis
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
/** 
 *
 * @ingroup tripal_analysis
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
/** 
 *
 * @ingroup tripal_analysis
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
/** 
 *
 * @ingroup tripal_analysis
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
/**
 *
 * @ingroup tripal_analysis
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

