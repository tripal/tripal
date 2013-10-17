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

  /*
  // before proceeding check to see if we have any
  // currently processing jobs. If so, we don't want
  // to give the opportunity to sync libraries
  $active_jobs = FALSE;
  if (tripal_get_module_active_jobs('tripal_feature')) {
    $active_jobs = TRUE;
  }
  if (!$active_jobs) {
*/
    get_tripal_feature_admin_form_title_set($form);
    get_tripal_feature_admin_form_url_set($form);

    $form['browser'] = array(
       '#type' => 'fieldset',
       '#title' => t('Feature Browser'),
       '#collapsible' => TRUE,
       '#collapsed' => TRUE,
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

    $form['summary'] = array(
       '#type' => 'fieldset',
       '#title' => t('Feature Summary Report'),
       '#collapsible' => TRUE,
       '#collapsed' => TRUE,
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
/*  }
  else {
    $form['notice'] = array(
       '#type' => 'fieldset',
       '#title' => t('Feature Management Temporarily Unavailable'),
       '#collapsible' => FALSE,
       '#collapsed' => FALSE,
    );
    $form['notice']['message'] = array(
       '#value' => t('Currently, feature management jobs are waiting or ".
          "are running. Managemment features have been hidden until these ".
          "jobs complete.  Please check back later once these jobs have ".
          "finished.  You can view the status of pending jobs in the Tripal ".
          "jobs page.'),
    );
  }
  */
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
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
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
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
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
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
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
   '#default_value' => variable_get('tax_classes', array()),
  );
  $form['taxonomy']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set/Reset Taxonomy for all feature nodes'),
    '#weight' => 3,
  );

}

/**
 * 
 * @param $form
 */
function get_tripal_feature_admin_form_title_set(&$form) {

  $form['title'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature Page Titles'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['title']['desc'] = array(
    '#type'        => 'markup',
    '#value' => t('Each synced feature must have a unique page title, however, features
                   may have the same name if they are of different types or from
                   different organisms.  Therefore, we must be sure that the 
                   page titles can uniquely identify the feature being viewed.  Select
                   an option below that will uniquely identify all features on your site.'),
  );
  $options = array(
    'feature_unique_name'  => 'Feature unique name',
    'feature_name'         => 'Feature name',
    'unique_constraint'    => 'Feature Name, uniquename, type and species',
  );
  $form['title']['chado_feature_title'] = array(
    '#title'         => t('Feature Page Titles'),
    '#type'          => 'radios',
    '#description'   => t('Choose a title type  from the list above that is 
      guaranteed to be unique for all features.  If in doubt it is safest to 
      choose the last option as that guarantees uniqueness. Click the 
      \'Save Configuration\' button at the bottom to save your selection.'),
    '#required'      => FALSE,
    '#options'       => $options,
    '#default_value' => variable_get('chado_feature_title', 'unique_constraint'),
  );
}
/**
 * 
 * @param $form
 */
function get_tripal_feature_admin_form_url_set(&$form) {

 
  $form['url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature URL Path'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $options = array(
    'SID[id]'      => '[id]:' . t('The Chado feature_id'),
    'feature'      => 'feature:' . t('Chado table name'),
    '[genus]'      => '[genus]:' . t('Genus to which the feature belongs'),
    '[species]'    => '[species]:' . t('Species to which the feature belongs'),
    '[type]'       => '[type]:' . t('The type of feature'),
    '[uniquename]' => '[uniquename]:' . t('The feature unique name'),
    '[name]'       => '[name]:' . t('The feature name'),
    'reset'        => t('Reset'),
  );
  

  $form['url']['chado_feature_url_string'] = array(
    '#title' => 'URL Syntax',
    '#type' => 'textfield',
    '#description' => t('You may rearrange elements in this text box to
      customize the URLs.  The available tags include: [id],
      [uniquename]. [name], [species], [genus], [type]. You can separate or
      include any text between the tags. Click the "Set Feature URLs" button to 
      reset the URLs for all feature pages.  Click the "Save Configuration" button to
      simply save this setup. <b>Important</b>: be sure that whatever you choose will always be unique even considering
      future data that may be added.  If you include the Chado table name, genus, species, type 
      and uniquename you are guaranteed to have a unique URL. For example feature/[genus]/[species]/[type]/[uniquename]'),
    '#size' => 150,
    '#default_value' => variable_get('chado_feature_url_string', '/feature/[genus]/[species]/[type]/[uniquename]'), 
  );
  
  $form['url']['chado_feature_url'] = array(
    '#title'         => t('URL components'),
    '#type'          => 'checkboxes',
    '#required'      => FALSE,
    '#options'       => $options,
    '#description'   => t('Click the item above to make it appear in the URL Syntax box'),
    '#attributes'    => array(
      'onclick' => '
        box = $(\'#edit-chado-feature-url-string\');
        if (this.value == \'reset\') {
          box.val(\'\');
        }
        else {        
          box.val(box.val() + "/" + this.value);          
        }
        this.checked = false;
      ',
    ),
  );  
  
  $form['url']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Feature URLs'),
  );
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_features_set_taxonomy($max_sync = 0, $job_id = NULL) {

  // make sure our vocabularies are cleaned and reset before proceeding
  tripal_feature_del_vocabulary();
  tripal_feature_set_vocabulary();

  // iterate through all drupal feature nodes and set the taxonomy
  $results = db_query("SELECT * FROM {chado_feature}");
  $nsql =  "SELECT * FROM {node} ".
          "WHERE nid = %d";
  $i = 0;

  // load into ids array
  $count = 0;
  $chado_features = array();
  while ($chado_feature = db_fetch_object($results)) {
    $chado_features[$count] = $chado_feature;
    $count++;
  }

  // Iterate through features that need to be synced
  $interval = intval($count * 0.01);
  foreach ($chado_features as $chado_feature) {

  // update the job status every 1% features
  if ($job_id and $i % $interval == 0) {
    tripal_job_set_progress($job_id, intval(($i/$count)*100));
  }
  print "$i of $count: ";
  $node = db_fetch_object(db_query($nsql, $chado_feature->nid));
  tripal_feature_set_taxonomy($node, $chado_feature->feature_id);

  $i++;
  }
}

/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_set_taxonomy($node, $feature_id) {

  // iterate through the taxonomy classes that have been
  // selected by the admin user and make sure we only set those
  $tax_classes = variable_get('tax_classes', array());
  $do_ft = 0;
  $do_op = 0;
  $do_lb = 0;
  $do_an = 0;
  foreach ($tax_classes as $class) {
    if (strcmp($class , 'organism')==0) {
      $do_op = 1;
    }
    if (strcmp($class, 'feature_type')==0) {
      $do_ft = 1;
    }
    if (strcmp($class, 'library')==0) {
      $do_lb = 1;
    }
    if (strcmp($class, 'analysis')==0) {
      $do_an = 1;
    }
  }


  // get the list of vocabularies and find our two vocabularies of interest
  $vocabularies = taxonomy_get_vocabularies();
  $ft_vid = NULL;
  $op_vid = NULL;
  $lb_vid = NULL;
  $an_vid = NULL;
  foreach ($vocabularies as $vocab) {
    if ($vocab->name == 'Feature Type') {
      $ft_vid = $vocab->vid;
    }
    if ($vocab->name == 'Organism') {
      $op_vid = $vocab->vid;
    }
    if ($vocab->name == 'Library') {
      $lb_vid = $vocab->vid;
    }
    if ($vocab->name == 'Analysis') {
      $an_vid = $vocab->vid;
    }
  }

  // get the cvterm and the organism for this feature
  $sql = "SELECT CVT.name AS cvname, O.genus, O.species ".
        "FROM {CVTerm} CVT ".
        "  INNER JOIN {Feature} F on F.type_id = CVT.cvterm_id ".
        "  INNER JOIN {Organism} O ON F.organism_id = O.organism_id ".
        "WHERE F.feature_id = $feature_id";
  $feature = db_fetch_object(chado_query($sql));

  // Set the feature type for this feature
  if ($do_ft && $ft_vid) {
    $tags["$ft_vid"] = "$feature->cvname";
  }
  // Set the organism for this feature type
  if ($do_op && $op_vid) {
    $tags["$op_vid"] = "$feature->genus $feature->species";
  }

  // get the library that this feature may belong to and add it as taxonomy
  if ($do_lb && $lb_vid) {
    $sql = "SELECT L.name ".
           "FROM {Library} L ".
           "  INNER JOIN {Library_feature} LF ON LF.library_id = L.library_id ".
           "WHERE LF.feature_id = %d ";
    $library = db_fetch_object(chado_query($sql, $feature_id));
    $tags["$lb_vid"] = "$library->name";
  }

  // now add the taxonomy to the node
  $terms['tags'] = $tags;
  taxonomy_node_save($node, $terms);
  //   print "Setting $node->name: " . implode(", ",$tags) . "\n";

  // get the analysis that this feature may belong to and add it as taxonomy
  // We'll add each one individually since there may be more than one analysis
  if ($do_an && $an_vid) {
    $sql = "SELECT A.name ".
           "FROM {Analysis} A ".
           "  INNER JOIN {Analysisfeature} AF ON AF.analysis_id = A.analysis_id ".
           "WHERE AF.feature_id = $feature_id ";
    $results = chado_query($sql);
    $analysis_terms = array();
    while ($analysis=db_fetch_object($results)) {
      $tags2["$an_vid"] = "$analysis->name";
      $terms['tags'] = $tags2;
      taxonomy_node_save($node, $terms);
    }
  }

}

/**
 *
 * Remove orphaned drupal nodes
 *
 * @param $dummy
 *   Not Used -kept for backwards compatibility
 * @param $job_id
 *   The id of the tripal job executing this function
 *
 * @ingroup tripal_feature
 */
function tripal_features_cleanup($dummy = NULL, $job_id = NULL) {

  return tripal_core_clean_orphaned_nodes('feature', $job_id);
 
}
/**
 *  This function is an extension of the chado_feature_view by providing
 *  the markup for the feature object THAT WILL BE INDEXED.
 *
 * @ingroup tripal_feature
 */
function theme_tripal_feature_search_index($node) {
  $feature = $node->feature;
  $content = '';

  // get the accession prefix
  $aprefix = variable_get('chado_feature_accession_prefix', 'FID');

  $content .= "<h1>$feature->uniquename</h1>. ";
  $content .= "<strong>$aprefix$feature->feature_id.</strong> ";
  $content .= "$feature->cvname ";
  $content .= "$feature->common_name ";

  // add the synonyms of this feature to the text for searching
  $synonyms = $node->synonyms;
  if (count($synonyms) > 0) {
    foreach ($synonyms as $result) {
      $content .= "$result->name ";
    }
  }

  return $content;
}

/**
 *  This function is an extension of the chado_feature_view by providing
 *  the markup for the feature object THAT WILL BE INDEXED.
 *
 * @ingroup tripal_feature
 */
function theme_tripal_feature_search_results($node) {
  $feature = $node->feature;
  $content = '';

  // get the accession prefix
  $aprefix = variable_get('chado_feature_accession_prefix', 'FID');

  $content .= "Feature Name: <h1>$feature->uniquename</h1>. ";
  $content .= "<strong>Accession: $aprefix$feature->feature_id.</strong>";
  $content .= "Type: $feature->cvname. ";
  $content .= "Organism: $feature->common_name. ";

  // add the synonyms of this feature to the text for searching
  $synonyms = $node->synonyms;
  if (count($synonyms) > 0) {
    $content .= "Synonyms: ";
    foreach ($synonyms as $result) {
      $content .= "$result->name, ";
    }
  }

  return $content;
}


/**
 *
 *
 * @ingroup tripal_feature
 */
function tripal_feature_set_vocabulary() {

  //include the file containing the required functions for adding taxonomy vocabs
  module_load_include('inc', 'taxonomy', 'taxonomy.admin');

  // get the vocabularies so that we make sure we don't recreate
  // the vocabs that already exist
  $vocabularies = taxonomy_get_vocabularies();
  $ft_vid = NULL;
  $op_vid = NULL;
  $lb_vid = NULL;
  $an_vid = NULL;

  // These taxonomic terms are hard coded because we
  // konw we have these relationships in the chado tables
  // through foreign key relationships.  The tripal
  // modules that correspond to these chado "modules" don't
  // need to be installed for the taxonomy to work.
  foreach ($vocabularies as $vocab) {
    if ($vocab->name == 'Feature Type') {
      $ft_vid = $vocab->vid;
    }
    if ($vocab->name == 'Organism') {
      $op_vid = $vocab->vid;
    }
    if ($vocab->name == 'Library') {
      $lb_vid = $vocab->vid;
    }
    if ($vocab->name == 'Analysis') {
      $an_vid = $vocab->vid;
    }
  }

  if (!$ft_vid) {
    $form_state = array();
    $values = array(
       'name' => t('Feature Type'),
       'nodes' => array('chado_feature' => 'chado_feature'),
       'description' => t('The feature type (or SO cvterm for this feature).'),
       'help' => t('Select the term that matches the feature'),
       'tags' => 0,
       'hierarchy' => 1,
       'relations' => 1,
       'multiple' => 0,
       'required' => 0,
       'weight' => 1,
    );
    drupal_execute('taxonomy_form_vocabulary', $form_state, $values);
    drupal_execute('taxonomy_form_vocabulary', $form_state);
  }

  if (!$op_vid) {
    $form_state = array();
    $values = array(
       'name' => t('Organism'),
       'nodes' => array('chado_feature' => 'chado_feature'),
       'description' => t('The organism to which this feature belongs.'),
       'help' => t('Select the term that matches the feature'),
       'tags' => 0,
       'hierarchy' => 1,
       'relations' => 1,
       'multiple' => 0,
       'required' => 0,
       'weight' => 2,
    );
    drupal_execute('taxonomy_form_vocabulary', $form_state, $values);
    drupal_execute('taxonomy_form_vocabulary', $form_state);
  }

  if (!$lb_vid) {
    $form_state = array();
    $values = array(
       'name' => t('Library'),
       'nodes' => array('chado_feature' => 'chado_feature'),
       'description' => t('Chado features associated with a library are assigned the term associated with the library'),
       'help' => t('Select the term that matches the feature'),
       'tags' => 0,
       'hierarchy' => 1,
       'relations' => 1,
       'multiple' => 0,
       'required' => 0,
       'weight' => 3,
    );
    drupal_execute('taxonomy_form_vocabulary', $form_state, $values);
    drupal_execute('taxonomy_form_vocabulary', $form_state);
  }

  if (!$an_vid) {
    $form_state = array();
    $values = array(
       'name' => t('Analysis'),
       'nodes' => array('chado_feature' => 'chado_feature'),
       'description' => t('Any analysis to which this feature belongs.'),
       'help' => t('Select the term that matches the feature'),
       'tags' => 0,
       'hierarchy' => 1,
       'relations' => 1,
       'multiple' => 1,
       'required' => 0,
       'weight' => 4,
    );
    drupal_execute('taxonomy_form_vocabulary', $form_state, $values);
    drupal_execute('taxonomy_form_vocabulary', $form_state);
  }
}