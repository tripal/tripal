<?php

/**
 * @file
 * @todo Add file header description
 */

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin() {

  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_feature')) {
    $active_jobs = TRUE;
  }
  if (!$active_jobs) {

    get_tripal_feature_admin_form_url_set($form);

    $form['browser'] = array(
       '#type' => 'fieldset',
       '#title' => t('Feature Browser'),
       '#collapsible' => 1,
       '#collapsed' => 1 ,
    );
    $allowedoptions1  = array(
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
       '#description' => t("Enter the Sequence Ontology (SO) terms for the feature types that " .
                           "will be shown in the feature browser."),
       '#default_value' => variable_get('chado_browser_feature_types', 'gene contig'),
    );


    $form['browser']['browse_features'] = array(
       '#title' => 'Feature Browser on Organism Page',
       '#type' => 'radios',
       '#options' => $allowedoptions1,
       '#default_value' => variable_get('tripal_feature_browse_setting', 'show_feature_browser'),
    );

    $form['browser']['browse_features_library'] = array(
      '#title' => 'Feature Browser on Library Page',
      '#type' => 'radios',
      '#options' => array(
        'show_feature_browser' => "Show the feature browse on the library page. The browser loads when page loads. This may be slow for large sites.",
        'hide_feature_browser' => "Hide the feature browser on the library page. Disables the feature browser completely.",
      ),
      '#default_value' => variable_get('tripal_library_feature_browse_setting', 'show_feature_browser'),
    );

    $form['browser']['browse_features_analysis'] = array(
      '#title' => 'Feature Browser on Analysis Page',
      '#type' => 'radios',
      '#options' => array(
        'show_feature_browser' => "Show the feature browse on the analysis page. The browser loads when page loads. This may be slow for large sites.",
        'hide_feature_browser' => "Hide the feature browser on the analysis page. Disables the feature browser completely.",
      ),
     '#default_value' => variable_get('tripal_analysis_feature_browse_setting', 'show_feature_browser'),
    );

    $form['browser']['set_browse_button'] = array(
       '#type' => 'submit',
       '#value' => t('Set Browser'),
       '#weight' => 2,
    );


    $form['feature_edit'] = array(
       '#type' => 'fieldset',
       '#title' => t('Feature Editing'),
       '#collapsible' => 1,
       '#collapsed' => 1 ,
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
       '#default_value' => variable_get('chado_edit_feature_types', 'gene contig EST mRNA'),
    );

    $form['feature_edit']['set_feature_types'] = array(
       '#type' => 'submit',
       '#value' => t('Set Feature Types'),
    );

    $form['summary'] = array(
       '#type' => 'fieldset',
       '#title' => t('Feature Summary Report'),
       '#collapsible' => 1,
       '#collapsed' => 1 ,
    );
    $allowedoptions2 ['show_feature_summary'] = "Show the feature summary on the organism page. The summary loads when page loads.";
    $allowedoptions2 ['hide_feature_summary'] = "Hide the feature summary on the organism page. Disables the feature summary.";

    $form['summary']['feature_summary'] = array(
       '#title' => 'Feature Summary on Organism Page',
       '#description' => 'A feature summary can be added to an organism page to allow users to see the '.
          'type and quantity of features available for the organism.',
       '#type' => 'radios',
       '#options' => $allowedoptions2,
       '#default_value' => variable_get('tripal_feature_summary_setting', 'show_feature_summary'),
    );
    $form['summary']['feature_mapping'] = array(
       '#title' => 'Map feature types',
       '#description' => t('You may specify which Sequence Ontology (SO) terms to show in the '.
          'feature summary report by listing them in the following text area.   Enter one per line. '.
          'If left blank, all SO terms for all features will be shown in the report. Only those terms '.
          'listed below will be shown in the report. Terms will appear in the report in the same order listed. To rename a '.
          'SO term to be more human readable form, use an \'=\' sign after the SO term (e.g. \'polypeptide = Protein\')'),
       '#type' => 'textarea',
       '#rows' => 15,
       '#default_value' => variable_get('tripal_feature_summary_report_mapping', ''),
    );
    $form['summary']['set_summary_button'] = array(
       '#type' => 'submit',
       '#value' => t('Set Summary'),
       '#weight' => 2,
    );

    get_tripal_feature_admin_form_taxonomy_set($form);
    get_tripal_feature_admin_form_reindex_set($form);
    get_tripal_feature_admin_form_cleanup_set($form);
  }
  else {
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

  variable_set('chado_browser_feature_types', $form_state['values']['feature_types']);

  // if the user wants to sync up the chado features then
  // add the job to the management queue
  switch ($form_state['values']['op']) {

    case  t('Sync all Features') :
      tripal_add_job('Sync all features', 'tripal_feature',
        'tripal_feature_sync_features', $job_args, $user->uid);
    break;

    case t('Set/Reset Taxonomy for all feature nodes') :
    tripal_add_job('Set all feature taxonomy', 'tripal_feature',
      'tripal_features_set_taxonomy', $job_args, $user->uid);
    break;

    case t('Reindex all feature nodes') :
    tripal_add_job('Reindex all features', 'tripal_feature',
      'tripal_features_reindex', $job_args, $user->uid);
    break;

    case t('Clean up orphaned features') :
    tripal_add_job('Cleanup orphaned features', 'tripal_feature',
      'tripal_features_cleanup', $job_args, $user->uid);
    break;

    case t('Set Browser') :
      variable_set('tripal_feature_browse_setting', $form_state['values']['browse_features']);
      variable_set('tripal_library_feature_browse_setting', $form_state['values']['browse_features_library']);
      variable_set('tripal_analysis_feature_browse_setting', $form_state['values']['browse_features_analysis']);
    break;

    case t('Set Feature Types') :
      variable_set('tripal_feature_type_setting', $form_state['values']['feature_edit_types']);
    break;

    case t('Set Summary') :
      variable_set('tripal_feature_summary_setting', $form_state['values']['feature_summary']);
      variable_set('tripal_feature_summary_report_mapping', $form_state['values']['feature_mapping']);
    break;

    case t('Set Feature URLs') :
      variable_set('chado_feature_url', $form_state['values']['feature_url']);
      tripal_add_job('Set Feature URLs', 'tripal_feature',
        'tripal_feature_set_urls', $job_args, $user->uid);
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
    '#title' => t('Clean Up'),
    '#collapsible' => 1,
    '#collapsed' => 1 ,
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
    '#title' => t('Index/Reindex'),
    '#collapsible' => 1,
    '#collapsed' => 1 ,
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
function get_tripal_feature_admin_form_taxonomy_set(&$form) {

  $form['taxonomy'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set Taxonomy'),
    '#collapsible' => 1,
    '#collapsed' => 1 ,
  );

  $form['taxonomy']['description'] = array(
     '#type' => 'item',
     '#value' => t("Drupal allows for assignment of \"taxonomy\" or ".
        "catagorical terms to nodes. These terms allow for advanced ".
        "filtering during searching."),
     '#weight' => 1,
  );
  $tax_options = array(
    'organism' => t('Organism name'),
    'feature_type'  => t('Feature Type (e.g. EST, mRNA, etc.)'),
    'analysis' => t('Analysis Name'),
    'library'  => t('Library Name'),
  );
  $form['taxonomy']['tax_classes'] = array(
   '#title'       => t('Available Taxonomic Classes'),
   '#type'        => t('checkboxes'),
   '#description' => t("Please select the class of terms to assign to ".
      "chado features"),
   '#required'    => FALSE,
   '#prefix'      => '<div id="taxclass_boxes">',
   '#suffix'      => '</div>',
   '#options'     => $tax_options,
   '#weight'      => 2,
   '#default_value' => variable_get('tax_classes', ''),
  );
  $form['taxonomy']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set/Reset Taxonomy for all feature nodes'),
    '#weight' => 3,
  );

}

function get_tripal_feature_admin_form_url_set(&$form) {

  $form['url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature URL Path')
  );
  $form['url']['desc'] = array(
    '#type'        => 'markup',
    '#value' => t('Each synced feature will have a unique URL which consists of '.
                        'the site domain followed by a unique identifer: for '.
                        'example http://my-tripal-site.org/ID1034, where the '.
                        'element just after the final slash is the unique '.
                        'identifier for the feature.'),

  );

  $options = array(
    'internal ID'          => 'Internal ID (Chado feature_id)',
    'feature unique name'  => 'Feature unique name',
    'feature name'         => 'Feature name',
    'genus_species_uqname' => 'Genus + species + unique name (e.g. http://your.site.url/[genus]/[genus]_[species]/[unique_name]',
    'genus_species_name'   => 'Genus + species + name (e.g. http://your.site.url/[genus]/[genus]_[species]/[name]',
  );
  
  $form['url']['chado_feature_url'] = array(
    '#title'         => t('Unique Identifier'),
    '#type'          => 'radios',
    '#description'   => t('Choose an identifier type '.
                        'from the list above that is guaranteed to be unique in your synced '.
                        'dataset. If in doubt it is safest to coose the internal ID. '.
                        'The descrpitor need not be unique amont the total dataset. '.
                        'It only need be unique among the synced dataset.'),
    '#required'      => FALSE,
    '#options'       => $options,
    '#default_value' => variable_get('chado_feature_url', 'internal ID'),
  );

  $form['url']['chado_feature_accession_prefix'] = array(
    '#title'       => t('ID Prefix'),
    '#type'        => t('textfield'),
    '#description' => t("If you choose an Internal ID above you must also enter an ID prefix.".
                        "this prefix will be prepended to the internal ID number (e.g. ID38294). ".
                        "if you chose to use the feature name or unique name then this prfix is not used"),
    '#required'    => TRUE,
    '#default_value' => variable_get('chado_feature_accession_prefix', 'ID'),
  );

  $form['url']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Feature URLs'),
    '#weight' => 3,
  );
}

