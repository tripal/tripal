<?php

/**
 * Purpose: Provide Guidance to new Tripal Admin
 *
 * @return 
 *   HTML Formatted text
 *
 * @ingroup tripal_feature
 */
function tripal_feature_module_description_page() {

  $text .= '<h3>Tripal Feature Administrative Tools Quick Links:</h3>';
  $text .= "<ul>";
  $text .= "<li><a href=\"".url("admin/tripal/tripal_feature/configuration") . "\">Feature Configuration</a></li>";
  $text .= "<li><a href=\"".url("admin/tripal/tripal_feature/fasta_loader"). "\">Import a multi-FASTA file</a></li>";
  $text .= "<li><a href=\"".url("admin/tripal/tripal_feature/gff3_load"). "\">Import a GFF3 file</a></li>";
  $text .= "<li><a href=\"".url("admin/tripal/tripal_feature/aggregate"). "\">Feature Relationship Aggegators</a></li>";
  $text .= "</ul>";
 

  $text .= '<h3>Module Description:</h3>';
  $text .= '<p>TODO: Basic Description of this module including mention/link to the chado module</p>';

  $text .= '<h3>Post Installation Instructions:</h3>';
  $text .= '<p>TODO: Describe any post installation intructions here. You shouldalways include setting user permissions.</p>';
  
  
  $text .= '<h3>Features of this Module:</h3>';
  $text .= '<p>TODO: Discuss the Features of this module including links. Some features to consider are creating content, details pages/node content, editing/deleteing, basic listings and vies integration. See admin/tripal/tripal_stock for an example.</p>';
  
  return $text;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin () {

   // before proceeding check to see if we have any
   // currently processing jobs. If so, we don't want
   // to give the opportunity to sync libraries
   $active_jobs = FALSE;
   if(tripal_get_module_active_jobs('tripal_feature')){
      $active_jobs = TRUE;
   }
   if(!$active_jobs){

      $form['chado_feature_accession_prefix'] = array (
         '#title'       => t('Internal ID Prefix'),
         '#type'        => t('textfield'),
         '#description' => t("Internal ID numbers for features consist of the ".
            "chado feature_id and a site specific prefix.  Set the prefix that ".
            "will be incorporated in front of each feature_id to form a unique ".
            "accession number for this site."),
         '#required'    => TRUE,
         '#default_value' => variable_get('chado_feature_accession_prefix','ID'),
      );

      $form['chado_feature_types'] = array(
         '#title'       => t('Feature Types'),
         '#type'        => 'textarea',
         '#description' => t('Enter the names of the sequence types that the ".
            "site will support with independent pages.  Pages for these data ".
            "types will be built automatically for features that exist in the ".
            "chado database.  The names listed here should be spearated by ".
            "spaces or entered separately on new lines. The names must match ".
            "exactly (spelling and case) with terms in the sequence ontology'),
         '#required'    => TRUE,
         '#default_value' => variable_get('chado_feature_types','EST contig'),
      );

      $form['browser'] = array(
         '#type' => 'fieldset',
         '#title' => t('Feature Browser')
      );
      $allowedoptions1  = array (
        'show_feature_browser' => "Show the feature browser on the organism page. The browser loads when page loads. This may be slow for large sites.",
        'hide_feature_browser' => "Hide the feature browser on the organism page. Disables the feature browser completely.",
      );
//      $allowedoptions ['allow_feature_browser'] = "Allow loading of the feature browsing through AJAX. For large sites the initial page load will be quick with the feature browser loading afterwards.";

      $form['browser']['browse_features'] = array(
         '#title' => 'Feature Browser on Organism Page',
         '#description' => 'A feature browser can be added to an organism page to allow users to quickly '. 
            'access a feature.  This will most likely not be the ideal mechanism for accessing feature '.
            'information, especially for large sites, but it will alow users exploring the site (such '.
            'as students) to better understand the data types available on the site.',
         '#type' => 'radios',
         '#options' => $allowedoptions1,
         '#default_value'=>variable_get('tripal_feature_browse_setting', 'show_feature_browser'),
      );
      $form['browser']['set_browse_button'] = array(
         '#type' => 'submit',
         '#value' => t('Set Browser'),
         '#weight' => 2,
      );

      $form['summary'] = array(
         '#type' => 'fieldset',
         '#title' => t('Feature Summary')
      );
      $allowedoptions2 ['show_feature_summary'] = "Show the feature summary on the organism page. The summary loads when page loads.";
      $allowedoptions2 ['hide_feature_summary'] = "Hide the feature summary on the organism page. Disables the feature summary.";

      $form['summary']['feature_summary'] = array(
         '#title' => 'Feature Summary on Organism Page',
         '#description' => 'A feature summary can be added to an organism page to allow users to see the '.
            'type and quantity of features available for the organism.',
         '#type' => 'radios',
         '#options' => $allowedoptions2,
         '#default_value'=>variable_get('tripal_feature_summary_setting', 'show_feature_summary'),
      );
      $form['summary']['set_summary_button'] = array(
         '#type' => 'submit',
         '#value' => t('Set Summary'),
         '#weight' => 2,
      );

      get_tripal_feature_admin_form_sync_set($form);
      get_tripal_feature_admin_form_taxonomy_set($form);
      get_tripal_feature_admin_form_reindex_set($form);
      get_tripal_feature_admin_form_cleanup_set($form);
   } else {
      $form['notice'] = array(
         '#type' => 'fieldset',
         '#title' => t('Feature Management Temporarily Unavailable')
      );
      $form['notice']['message'] = array(
         '#value' => t('Currently, feature management jobs are waiting or ".
            "are running. Managemment features have been hidden until these ".
            "jobs complete.  Please check back later once these jobs have ".
            "finished.  You can view the status of pending jobs in the Tripal ".
            "jobs page.'),
      );
   }
   return system_settings_form($form);
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin_validate($form, &$form_state) {
   global $user;  // we need access to the user info
   $job_args = array();

   // if the user wants to sync up the chado features then
   // add the job to the management queue
   if ($form_state['values']['op'] == t('Sync all Features')) {
      tripal_add_job('Sync all features','tripal_feature',
         'tripal_feature_sync_features',$job_args,$user->uid);
   }

   if ($form_state['values']['op'] == t('Set/Reset Taxonomy for all feature nodes')) {
      tripal_add_job('Set all feature taxonomy','tripal_feature',
         'tripal_features_set_taxonomy',$job_args,$user->uid);
   }

   if ($form_state['values']['op'] == t('Reindex all feature nodes')) {
      tripal_add_job('Reindex all features','tripal_feature',
         'tripal_features_reindex',$job_args,$user->uid);
   }

   if ($form_state['values']['op'] == t('Clean up orphaned features')) {
      tripal_add_job('Cleanup orphaned features','tripal_feature',
         'tripal_features_cleanup',$job_args,$user->uid);
   }

   if ($form_state['values']['op'] == t('Set Browser')) {
      variable_set('tripal_feature_browse_setting',$form_state['values']['browse_features']);
   }

   if ($form_state['values']['op'] == t('Set Summary')) {
      variable_set('tripal_feature_summary_setting',$form_state['values']['feature_summary']);
   }
}
/**
 *
 *
 * @ingroup tripal_feature
 */
function get_tripal_feature_admin_form_cleanup_set(&$form) {
   $form['cleanup'] = array(
      '#type' => 'fieldset',
      '#title' => t('Clean Up')
   );
   $form['cleanup']['description'] = array(
       '#type' => 'item',
       '#value' => t("With Drupal and chado residing in different databases ".
          "it is possible that nodes in Drupal and features in Chado become ".
          "\"orphaned\".  This can occur if a feature node in Drupal is ".
          "deleted but the corresponding chado feature is not and/or vice ".
          "versa.  The Cleanup function will also remove nodes for features ".
          "that are not in the list of allowed feature types as specified ".
          "above.  This is helpful when a feature type needs to be ".
          "removed but was previously present as Drupal nodes. ".
          "Click the button below to resolve these discrepancies."),
       '#weight' => 1,
   );
   $form['cleanup']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Clean up orphaned features'),
      '#weight' => 2,
   );
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function get_tripal_feature_admin_form_reindex_set(&$form) {
   $form['reindex'] = array(
      '#type' => 'fieldset',
      '#title' => t('Reindex')
   );
   $form['reindex']['description'] = array(
       '#type' => 'item',
       '#value' => t("Reindexing of nodes is important when content for nodes ".
          "is updated external to drupal, such as external uploads to chado. ".
          "Features need to be reindexed to ensure that updates to features ".
          "are searchable. Depending on the number of features this may take ".
          "quite a while. Click the button below to begin reindexing of ".
          "features."),
       '#weight' => 1,
   );
   $form['reindex']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Reindex all feature nodes'),
      '#weight' => 2,
   );
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function get_tripal_feature_admin_form_taxonomy_set (&$form) {


   $form['taxonomy'] = array(
      '#type' => 'fieldset',
      '#title' => t('Set Taxonomy')
   );

   $form['taxonomy']['description'] = array(
       '#type' => 'item',
       '#value' => t("Drupal allows for assignment of \"taxonomy\" or ".
          "catagorical terms to nodes. These terms allow for advanced ".
          "filtering during searching."),
       '#weight' => 1,
   );
   $tax_options = array (
      'organism' => t('Organism name'),
      'feature_type'  => t('Feature Type (e.g. EST, mRNA, etc.)'),
      'analysis' => t('Analysis Name'),
      'library'  => t('Library Name'),
   );
   $form['taxonomy']['tax_classes'] = array (
     '#title'       => t('Available Taxonomic Classes'),
     '#type'        => t('checkboxes'),
     '#description' => t("Please select the class of terms to assign to ".
        "chado features"),
     '#required'    => FALSE,
     '#prefix'      => '<div id="taxclass_boxes">',
     '#suffix'      => '</div>',
     '#options'     => $tax_options,
     '#weight'      => 2,
     '#default_value' => variable_get('tax_classes',''),
   );
   $form['taxonomy']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Set/Reset Taxonomy for all feature nodes'),
      '#weight' => 3,
   );

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function get_tripal_feature_admin_form_sync_set (&$form) {

  
   // get the list of organisms which will be synced.
   $feature_sql = "SELECT * FROM {Feature} WHERE uniquename = '%s' and organism_id = %d";
   $previous_db = tripal_db_set_active('chado');
   $feature = db_fetch_object(db_query($feature_sql,$node->title,$node->organism_id));
   tripal_db_set_active($previous_db);

   // define the fieldsets
   $form['sync'] = array(
      '#type' => 'fieldset',
      '#title' => t('Sync Features')
   );

   $form['sync']['description'] = array(
      '#type' => 'item',
      '#value' => t("Click the 'Sync all Features' button to create Drupal ".
         "content for features in chado. Only features of the types listed ".
         "above in the Feature Types box will be synced. Depending on the ".
         "number of features in the chado database this may take a long ".
         "time to complete. "),
      '#weight' => 1,
   );

   $orgs = tripal_organism_get_synced();   
   $org_list = '';
   foreach($orgs as $org){
      $org_list .= "$org->genus $org->species, ";
   }
   $form['sync']['description2'] = array(
      '#type' => 'item',
      '#value' => "Only features for the following organisms will be synced: ".
         " $org_list",
      '#weight' => 1,
   );

   $form['sync']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Sync all Features'),
      '#weight' => 3,
   );

}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_aggregator_page(){
   $add_url = url("admin/tripal/tripal_feature/aggregate/new");
   $output = "<a href=\"$add_url\">Add a new aggregator</a>"; 
   $output .= drupal_get_form('tripal_feature_aggregator_select_form');
   $output .= '<div id="db-edit-div">Please select an aggregator base type to view or edit</div>';
   return $output;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_aggregator_select_form(){

	// get a list of base terms from chado for user to choose
	$sql = "SELECT DISTINCT type_id FROM {tripal_feature_relagg} ORDER BY type_id ";
	$results = db_query ($sql);
   
   $sql = "SELECT * FROM {cvterm} WHERE cvterm_id = %d";
   $previous_db = tripal_db_set_active('chado');
	$types = array();
   $types[] = '';
   while($base = db_fetch_object($results)){
      $term = db_fetch_object(db_query($sql,$base->type_id));
		$types[$base->type_id] = $term->name;
   }
   tripal_db_set_active($previous_db);

	$form['type_id'] = array(
      '#title' => t('Aggregator Base Type'),
      '#type' => 'select',
      '#options' => $types,
      '#ahah' => array(
         'path' => 'admin/tripal/tripal_feature/aggregate/edit/js',
         'wrapper' => 'db-edit-div',
         'effect' => 'fade',
         'event' => 'change',
         'method' => 'replace',
      ),
	);

   return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_aggregator_form(&$form_state = NULL,$type_id = NULL){

   // get this requested database
   if($type_id){
      $sql = "SELECT * FROM {tripal_feature_relagg} WHERE type_id = %d ";
      $tsql = "SELECT * FROM {cvterm} WHERE cvterm_id = %d ";

      // get the default list of terms
      $results = db_query($sql,$type_id);
      while($type = db_fetch_object($results)){
         $previous_db = tripal_db_set_active('chado');
         $term = db_fetch_object(db_query($tsql,$type->rel_type_id));
         tripal_db_set_active($previous_db);
         $default_others .= $term->name . " ";
      }
      $default_base = $base->name;     
      $action = 'Update';
      $form['type_id'] = array(
         '#type' => 'hidden',
         '#value' => $type_id
      );
   } else {
      $action = 'Add';
      $form['base']= array(
         '#type'          => 'textfield',
         '#title'         => t('Base Feature type'),
         '#description'   => t('Please enter the Sequence Ontology (SO) term for the base feature type for this aggregator.'),
         '#default_value' => $default_base,
         '#required'      => TRUE,
         '#weight'        => 1
      );
   }

   $form['others']= array(
      '#type'          => 'textarea',
      '#title'         => t('Aggregate these types if a relationship exists'),
      '#description'   => t('Please enter the Sequence Ontology (SO) terms that should be aggregated with the base feature type listed above.  Separate each by a space or newline'),
      '#default_value' => $default_others,
      '#required'      => TRUE,
      '#weight'        => 2
   );
 
   if(strcmp($action,'Update')==0){
      $form['update'] = array (
        '#type'         => 'submit',
        '#value'        => t('Update'),
        '#weight'       => 5,
        '#executes_submit_callback' => TRUE,
      );
      $form['delete'] = array (
        '#type'         => 'submit',
        '#value'        => t('Delete'),
        '#weight'       => 6,
        '#executes_submit_callback' => TRUE,
      );
   } else {
      $form['add'] = array (
        '#type'         => 'submit',
        '#value'        => t('Add'),
        '#weight'       => 5,
        '#executes_submit_callback' => TRUE,
      );
   }
   $form['#redirect'] = 'admin/tripal/tripal_feature/aggregate';


   return $form;
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_aggregator_form_validate($form, &$form_state){
   $type_id  = $form_state['values']['type_id'];
   $base     = $form_state['values']['base'];
   $others   = $form_state['values']['others'];
   $op      =  $form_state['values']['op'];

   // split apart the feature types to be aggregated
   $types = preg_split('/\s+/',$others);

   // the SQL for finding the term
   $tsql = "
      SELECT * 
      FROM {cvterm} CVT 
         INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id 
      WHERE CVT.name = '%s' and CV.name = 'sequence'";


   // make sure the base type exists
   if($base){
      $previous_db = tripal_db_set_active('chado');
      $term = db_fetch_object(db_query($tsql,$base));
      tripal_db_set_active($previous_db);
      if(!$term){
         form_set_error('base',t('The specified base type is not a valid SO term'));
      }

      // make sure this type doesn't already in the table
      $sql = "SELECT * FROM {tripal_feature_relagg} WHERE type_id = %d ";
      $agg = db_fetch_object(db_query($sql,$term->cvterm_id));
      if($agg){
         form_set_error('base',t('The specified base type is already used as a base type for another aggregator and cannot be readded.'));
      }      
   }

   // iterate through each type to be aggregated and make sure they are valid
   foreach($types as $type){
      $previous_db = tripal_db_set_active('chado');
      $term = db_fetch_object(db_query($tsql,$type));
      tripal_db_set_active($previous_db);
      if(!$term){
         form_set_error('others',t('The specified type ') . $type . t(' is not a valid SO term'));
      }
   }   
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_aggregator_form_submit($form, &$form_state){

   $type_id  = $form_state['values']['type_id'];
   $base     = $form_state['values']['base'];
   $others   = $form_state['values']['others'];
   $op      =  $form_state['values']['op'];

   // split apart the feature types to be aggregated
   $types = preg_split('/\s+/',$others);

   // the SQL for finding the term
   $tsql = "
      SELECT * 
      FROM {cvterm} CVT 
         INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id 
      WHERE CVT.name = '%s' and CV.name = 'sequence'";

   // the SQL for deleting an aggregator
   $dsql = "
      DELETE FROM {tripal_feature_relagg}
      WHERE type_id = %d
   ";

   // if this is an insert then we have a base type name.  We
   // need to get the corresponding term id
   if($base){
      $previous_db = tripal_db_set_active('chado');
      $term = db_fetch_object(db_query($tsql,$base));
      tripal_db_set_active($previous_db);
      $type_id = $term->cvterm_id;
   }

   if(strcmp($op,'Delete')==0){
      $result = db_query($dsql,$type_id);
      if($result){
        drupal_set_message("Aggregator deleted");
      } else {
        drupal_set_message("Failed to delete mailing list.");
      }
   }
   else {
      // for an update, first remove all the current aggregator settings
      // and we'll add them again
      $result = db_query($dsql,$type_id);

      // the SQL for inserting the aggregated types
      $isql = "
         INSERT INTO {tripal_feature_relagg}
          (type_id,rel_type_id)
         VALUES 
          (%d,%d)
      ";

      // iterate through each type to be aggregated and add an entry in the table
      foreach($types as $type){
         $previous_db = tripal_db_set_active('chado');
         $term = db_fetch_object(db_query($tsql,$type));
         tripal_db_set_active($previous_db);
         $result = db_query($isql,$type_id,$term->cvterm_id);
      }
      drupal_set_message("Aggregator added");
   } 
   return '';
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_aggregator_ajax_edit (){ 
   // get the database id, build the form and then return the JSON object
   $type_id = $_POST['type_id'];
   $form = drupal_get_form('tripal_feature_aggregator_form',$type_id);
   drupal_json(array('status' => TRUE, 'data' => $form));
}
