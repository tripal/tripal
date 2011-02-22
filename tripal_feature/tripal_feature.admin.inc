<?php

/************************************************************************
 *
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
         '#title'       => t('Accession Prefix'),
         '#type'        => t('textfield'),
         '#description' => t("Accession numbers for features consist of the ".
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

/************************************************************************
 *
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
/************************************************************************
 *
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
/************************************************************************
 *
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
/************************************************************************
 *
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
/************************************************************************
 *
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
