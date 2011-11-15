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
  $text .= "<ul>
             <li><a href=\"".url("admin/tripal/tripal_feature/configuration") . "\">Feature Configuration</a></li>
             <li><a href=\"".url("admin/tripal/tripal_feature/fasta_loader"). "\">Import a multi-FASTA file</a></li>
             <li><a href=\"".url("admin/tripal/tripal_feature/gff3_load"). "\">Import a GFF3 file</a></li>
             <li><a href=\"".url("admin/tripal/tripal_feature/sync") . "\">Sync Features</a></li>
             <li><a href=\"".url("admin/tripal/tripal_feature/delete") . "\">Delete Features</a></li>
           </ul>";
#             <li><a href=\"".url("admin/tripal/tripal_feature/aggregate"). "\">Feature Relationship Aggegators</a></li> 

  $text .= '<h3>Module Description:</h3>';
  $text .= '<p>This module provides an interface for the Chado feature module which stores information
            related to genomic features.  This module provides support for bulk loading of data in
            FASTA or GFF format, visualization of "feature" pages, editing and updating.  
            </p>';

  $text .= '<h3>Setup Instructions:</h3>';
  $text .= '<p>After installation of the feature module.  The following tasks should be performed
            <ol>
              <li><p><b>Set Permissions</b>: The feature module supports the Drupal user permissions interface for 
               controlling access to feature content and functions. These permissions include viewing, 
               creating, editing or administering of
               feature content. The default is that only the original site administrator has these 
               permissions.  You can <a href="'.url('admin/user/roles').'">add roles</a> for classifying users, 
               <a href="'.url('admin/user/user').'">assign users to roles</a> and
               <a href="'.url('admin/user/permissions').'">assign permissions</a> for the feature content to 
               those roles.  For a simple setup, allow anonymous users access to view organism content and 
               allow the site administrator all other permissions.</p></li>

               <li><p><b>Themeing</b>:  Before content from Chado can be visualized the Tripal base theme must
               be installed.  This should have been done prior to this point.  But is mentioned here in the event you
               follow the instructions below and cannot see content.  In this case, if you do not see content
               check that Tripal theming is properly installed</p></li>

               <li><p><b>Loading of Ontologies</b>:  If you
               used Tripal to create the Chado database, then you must load ontologies before proceeding.  Visit the
               page to <a href="'.url('admin/tripal/tripal_cv/obo_loader').'">load ontologies</a> and load at 
               least the following ontologies:  
               <ul>
                  <li>Chado Feature Properties</li>
                  <li>Relationship Ontology</li>
                  <li>Sequence Ontology</li>
                  <li>Gene Ontology (if loading GO terms for features)</li>
               </ul></p></li>

               <li><p><b>Create Organisms</b>:  Before adding feature data you must already have the 
               organisms loaded in the database.  See the 
               <a href="'.url('admin/tripal/tripal_organism').'">Tripal Organism Admin page</a> for 
               instructions for adding and Syncing organisms.</p></li>

               <li><p><b>Create Analysis</b>:  Tripal requires that feature data loaded using the Tripal loaders
               be associated with an analyis.  This provides a grouping for the feature data and can be used
               later to visualize data pipelines.  Before loading feature data through the FASTA or GFF loaders
               you will need to <a href="'.url('node/add').'">create an analysis</a> for the data.</p></li>

               <li><p><b>Create Referring Database Entries</b>:  If you would like to associate your feature data with an 
               external reference database, check to ensure that the <a href="'.url('admin/tripal/tripal_db/edit_db').'">
               database record already exists</a>.  If not you should <a href="'.url('admin/tripal/tripal_db/add_db').'">add a new database record</a> before importing
               feature data.</p></li>

               <li><p><b>Data Import</b>:  if you do not already have an existing Chado database with preloaded data 
               then you will want
               to import data.  You can do so using the Chado perl scripts that come with the normal 
               <a href="http://gmod.org/wiki/Chado">distribution of Chado</a> or you can use the <a href="'.url('admin/tripal/tripal_feature/fasta_loader').'">FASTA loader</a> and 
               <a href="'.url('admin/tripal/tripal_feature/gff3_load').'">GFF loader</a> provided here.  If you
               created the Chado database using Tripal then you\'ll most likely want to use the Tripal loaders.  If your data
               is not condusive for loading with these loaders you may have to write your own loaders. 
               </p></li>

               <li><p><b>Sync Features</b>:  After data is loaded you need to sync features.  This process is what
               creates the pages for viewing online.  Not all features need be synced.  For instance, if you
               have loaded whole genome sequence with fully defined gene models with several features to define
               a gene and its products (e.g. gene, mRNA, CDS, 5\'UTR, 3\'UTR, etc) you probably only want to create
               pages for genes or genes and mRNA.  You probably do not want a page for a 5\'UTR.  
               Using the <a href="'.url('admin/tripal/tripal_feature/configuration/sync').'">Feature Sync page</a> 
               you can sync (or create pages) for the desired feature types. </p></li>

               <li><p><b>Set Feature URL</b>:  It is often convenient to have a simple URL for each feature page. 
               For example, http://www.mygenomesite.org/[feature], where [feature] is a unique identifier for a feature page.
               With this, people can easily include links to feature pages of interest. Use the 
               <a href="'.url('admin/tripal/tripal_feature/configuration').'">Feature Configuration page</a> 
               to specify whether to use the feature name, unique name or internal ID as the [feature] portion of the
               URL.  Select the one that will guarantee a unique identifier for feature pages.</p></li>

               <li><p><b>Indexing</b>:  Once all data has been loaded (including analysis data--e.g. blast, interpro, etc.)
               you can index all feature pages for searching if you want to ues the Drupal default search mechanism. 
               Use the <a href="'.url('admin/tripal/tripal_feature/configuration').'">Feature Configuration page</a> 
               to either Index (for the first time) or "Reindex" (after adding new data)
               the feature pages for searching.  Once the site is 100% indexed the pages will be searchable using Drupal\'s
               full text searching.  You can find the percent indexed for the entire site by visiting the 
               <a href="'.url('admin/settings/search').'">Search settings page</a>. Indexing
               can take quite a while if you have a lot of data</p></li>

               <li><p><b>Set Taxonomy</b>:  Drupal provides a mechanism for categorizing content to allow 
               for advanced searching.  Drupal calls this "Taxonomy", but is essentially categorizing the pages.  
               You can categorize feature pages by their type (e.g. gene, mRNA, contig, EST, etc.) and by the 
               organism to which they belong.  This allows for filtering of search results by organism and feature type. 
               Use the <a href="'.url('admin/tripal/tripal_feature/configuration').'">Feature Configuration page</a> to
               set the Taxonomy.</p></li>
            </ol>
            </p>';
  
  
  $text .= '<h3>Features of this Module:</h3>';
  $text .= '<p>Aside from data loading and feature page setup (as described in the Setup section above), 
            The Tripal feature module also provides the following functionality
            <ul>
              <li><p><b>Feature Browser:</b>  The feature browser is a tabular list of features with links to their 
               feature pages which appears on the organism 
               page.  It was created to provide a mechanism to allow site visitors to quickly
               accesss feature pages when they do not know what to search for.  For sites with large numbers of features, this
               method for finding a specific pages is inadequate, but may still be included to aid new site 
               visitors.    This browser can be toggled on or off using the 
               <a href="'.url('admin/tripal/tripal_feature/configuration').'">Feature Configuration page</a></p></li>
 
              <li><p><b>Feature Summary:</b>  The feature summary is a pie chart that indicates the types and quantities
              of feature types (Sequence Ontology terms) that are loaded in the database. It appears on the organism 
              page.  The summary can be toggled on or off using the 
              <a href="'.url('admin/tripal/tripal_feature/configuration').'">Feature Configuration page</a></p></li>             

              <li><p><b>Integration with Drupal Views</b>: <a href="http://drupal.org/project/views">Drupal Views</a> is
              a powerful tool that allows the site administrator to create lists or basic searching forms of Chado content.
              It provides a graphical interface within Drupal to allow the site admin to directly query the Chado database
              and create custom lists without PHP programming or customization of Tripal source code.  Views can also
              be created to filter content that has not yet been synced with Druapl in order to protect access to non
              published data (only works if Chado was installed using Tripal).  You can see a list of available pre-existing
              Views <a href="'.url('admin/build/views/').'">here</a>, as well as create your own. </p></li>

              <li><p><b>Basic Feature Lookup View</b>: This module provides a basic <a href="'.url('features').'">feature search 
              tool</a> for finding or listing features in Chado. It does not require indexing for Drupal searching but relies
              on Drupal Views.  <a href="http://drupal.org/project/views">Drupal Views</a> must be installed. </p></li>              

              <li><p><b>Delete Features</b>: This module provides a <a href="'.url('admin/tripal/tripal_feature/delete').'">Delete Feature page</a>
              for bulk deltions of features. You may delete features using a list of feature names, or for a specific organism
              or for a specific feature type.</p></li>              

            </ul>
            </p>';

   $text .= '<h3>Page Customizations</h3>';
   $text .= '<p>There are several ways to customize the look-and-feel for the way Chado data is presented through Tripal. 
             Below is a description of several methods.  These methods may be used in conjunction with one another to
             provide fine-grained control. 
             <ul>

             <li><p><b>Integration with Drupal Panels</b>:  <a href="http://drupal.org/project/views">Drupal Panels</a> 
              allows for customization of a page layout if you don\'t want to do PHP/Javascript/CSS programming.  Tripal comes with pre-set layouts for feature pages.  However, 
              Panels become useful if you prefer a layout that is different from the pre-set layouts.  Chado content
              is provided to Panels in the form of Drupal "blocks" which you can then place anywhere on a page using the 
              Panel\'s GUI.</p></li>

             <li><p><b>Drupal\'s Content Construction Kit (CCK)</b>: the 
             <a href="http://drupal.org/project/cck">Content Construction Kit (CCK) </a> is a powerful way to add non-Chado content
             to any page without need to edit template files or knowing PHP.  You must first download and install CCK.
             With CCK, the site administartor can create a new field to appear on the page.  For example, currently,
             the Chado publication module is not yet supported by Tripal.  Therefore, the site administrator can add a text 
             field to the feature pages.  This content is not stored in Chado, but will appear on the feature page.  A field
             added by CCK will also appear in the form when editing a feature to allow users to manually enter the appropriate
             text.  If the default pre-set layout and themeing for Tripal is used, it is better to create the CCK element,
             indicate that it is not to be shown (using the CCK interface), then manually add the new content type 
             where desired by editing the templates (as described below).  If using Panels, the CCK field can be added to the
             location desired using the Panels interface.</p></li>

             <li><p><b>Drupal Node Templates</b>:  The Tripal packages comes with a "theme_tripal" directory that contains the
             themeing for Chado content.    The feature module has a template file for feature "nodes" (Tripal feature pages).  This file
             is named "node-chado_feature.tpl.php", and provides javascript, HTML and PHP code for display of the feature
             pages.  You can edit this file to control which types of information (or which feature "blocks") are displayed for features. Be sure to 
             copy these template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations. See the <a href="http://tripal.sourceforge.net/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template file.</p></li>

             <li><p><b>Feature "Block" Templates</b>:  In the "theme_tripal" directory is a subdirectory named "tripal_feature".
             Inside this directory is a set of templates that control distinct types of information for features.  For example,
             there is a "base" template for displaying of data directly from the Chado feature table, and a "references" 
             template for showing external site references for a feature (data from the feature_dbxref table).  These templates are used both by Drupal blocks
             for use in Drupal Panels (as described above) or for use in the default pre-set layout that the node template 
             provides (also desribed above).  You can customize this template as you desire.  Be sure to copy the
             template to your primary theme directory for editing. Do not edit them in the "theme_tripal" directory as
             future Tripal updates may overwrite your customizations.  See the <a href="http://tripal.sourceforge.net/">Tripal website </a>
             for instructions on how to access variables and other Chado content within the template files.</p></li>
             </li>

             <li><p><b>Adding Links to the "Resources" Sidebar</b>: If you use the pre-set default Tripal layout for theming, you
             will see a "Resources" sidebar on each page.  The links that appear on the sidebar are automatically generated
             using Javascript for all of the feature "Blocks" that appear on the page. If you want to add additional links 
             (e.g. a dynamic link to GBrowse for the feature) and you want that link to appear in the 
             "Resources" sidebar, simply edit the Drupal Node Template (as described above) and add the link to the 
             section at the bottom of the template file where the resources section is found.</p></li>

             </ul>
             </p>';

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

      get_tripal_feature_admin_form_url_set($form);

      $form['browser'] = array(
         '#type' => 'fieldset',
         '#title' => t('Feature Browser')
      );
      $allowedoptions1  = array (
        'show_feature_browser' => "Show the feature browser on the organism page. The browser loads when page loads. This may be slow for large sites.",
        'hide_feature_browser' => "Hide the feature browser on the organism page. Disables the feature browser completely.",
      );
//      $allowedoptions ['allow_feature_browser'] = "Allow loading of the feature browsing through AJAX. For large sites the initial page load will be quick with the feature browser loading afterwards.";

      $form['browser']['browser_desc'] = array(
         '#type'        => 'markup',
         '#value' => 'A feature browser can be added to an organism page to allow users to quickly '. 
            'access a feature.  This will most likely not be the ideal mechanism for accessing feature '.
            'information, especially for large sites, but it will alow users exploring the site (such '.
            'as students) to better understand the data types available on the site.',

      );
      $form['browser']['feature_types'] = array(
         '#title'       => t('Feature Types'),
         '#type'        => 'textarea',
         '#description' => t("Enter the Sequence Ontology (SO) terms for the feature types that ".
                             "will be shown in the feature browser."),
         '#default_value' => variable_get('chado_browser_feature_types','gene contig'),
      );


      $form['browser']['browse_features'] = array(
         '#title' => 'Feature Browser on Organism Page',
         '#type' => 'radios',
         '#options' => $allowedoptions1,
         '#default_value'=>variable_get('tripal_feature_browse_setting', 'show_feature_browser'),
      );
      $form['browser']['set_browse_button'] = array(
         '#type' => 'submit',
         '#value' => t('Set Browser'),
         '#weight' => 2,
      );


      $form['feature_edit'] = array(
         '#type' => 'fieldset',
         '#title' => t('Feature Editing')
      );

      $form['feature_edit']['browser_desc'] = array(
         '#type'        => 'markup',
         '#value' => 'When editing or creating a feature, a user must provide the feature type. '.
                     'The Sequence Ontology list is very large, therefore, to simply the list of types for the user, the following '.
                     'textbox allows you to specify which features types can be used. This list of terms will appear in the '.
                     'feature type drop down list of the feature creation/edit form.',

      );
      $form['feature_edit']['feature_edit_types'] = array(
         '#title'       => t('Feature Types'),
         '#type'        => 'textarea',
         '#description' => t("Enter the Sequence Ontology (SO) terms for the allowed feature types when creating or editing features."),
         '#default_value' => variable_get('chado_edit_feature_types','gene contig EST mRNA'),
      );

      $form['feature_edit']['set_feature_types'] = array(
         '#type' => 'submit',
         '#value' => t('Set Feature Types'),
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

   variable_set('chado_browser_feature_types',$form_state['values']['feature_types']);

   // if the user wants to sync up the chado features then
   // add the job to the management queue
   switch ($form_state['values']['op']){

      case  t('Sync all Features') :
         tripal_add_job('Sync all features','tripal_feature',
            'tripal_feature_sync_features',$job_args,$user->uid);
         break;

      case t('Set/Reset Taxonomy for all feature nodes') :
         tripal_add_job('Set all feature taxonomy','tripal_feature',
            'tripal_features_set_taxonomy',$job_args,$user->uid);
         break;

      case t('Reindex all feature nodes') :
         tripal_add_job('Reindex all features','tripal_feature',
            'tripal_features_reindex',$job_args,$user->uid);
         break;

      case t('Clean up orphaned features') :
         tripal_add_job('Cleanup orphaned features','tripal_feature',
            'tripal_features_cleanup',$job_args,$user->uid);
         break;

      case t('Set Browser') :
         variable_set('tripal_feature_browse_setting',$form_state['values']['browse_features']);
         break;

      case t('Set Feature Types') :
         variable_set('tripal_feature_type_setting',$form_state['values']['feature_edit_types']);
         break;

      case t('Set Summary') :
         variable_set('tripal_feature_summary_setting',$form_state['values']['feature_summary']);
         break;

      case t('Set Feature URLs') :
         variable_set('chado_feature_url',$form_state['values']['feature_url']);
         tripal_add_job('Set Feature URLs','tripal_feature',
            'tripal_feature_set_urls',$job_args,$user->uid);
         break;
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
      '#title' => t('Index/Reindex')
   );
   $form['reindex']['description'] = array(
       '#type' => 'item',
       '#value' => t("Indexing or reindexing of nodes is required for Drupal's full text searching. ".
          "Index features for the first time to allow for searching of content, and later when content for features ".
          "is updated.  Depending on the number of features this may take ".
          "quite a while. Click the button below to begin reindexing of ".
          "features. "),
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

function get_tripal_feature_admin_form_url_set (&$form) {

   $form['url'] = array(
      '#type' => 'fieldset',
      '#title' => t('Feature URL Path')
   );
   $form['url']['desc'] = array (
      '#type'        => 'markup',
      '#value' => t('Each synced feature will have a unique URL which consists of '.
                          'the site domain followed by a unique identifer: for '.
                          'example http://my-tripal-site.org/ID1034, where the '.
                          'element just after the final slash is the unique '.
                          'identifier for the feature.'),

   );

   $form['url']['chado_feature_url'] = array(
      '#title'       => t('Unique Identifier'),
      '#type'        => 'radios',
      '#description' => t('Choose an identifier type '.
                          'from the list above that is guaranteed to be unique in your synced '.
                          'dataset. If in doubt it is safest to coose the internal ID. '.
                          'The descrpitor need not be unique amont the total dataset. '.
                          'It only need be unique among the synced dataset.'),
      '#required'    => FALSE,
      '#options'     => array('internal ID' => 'internal ID (Chado feature_id)',
                              'feature unique name' => 'feature unique name', 
                              'feature name' => 'feature name'),
      '#default_value' => variable_get('chado_feature_url','internal ID'),
   );

   $form['url']['chado_feature_accession_prefix'] = array (
      '#title'       => t('ID Prefix'),
      '#type'        => t('textfield'),
      '#description' => t("If you choose an Internal ID above you must also enter an ID prefix.".
                          "this prefix will be prepended to the internal ID number (e.g. ID38294). ".
                          "if you chose to use the feature name or unique name then this prfix is not used"),
      '#required'    => TRUE,
      '#default_value' => variable_get('chado_feature_accession_prefix','ID'),
   );

   $form['url']['button'] = array(
      '#type' => 'submit',
      '#value' => t('Set Feature URLs'),
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
