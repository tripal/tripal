<?php

/**
 * @file
 * Contains functions displaying administrative pages and forms
 */

/**
 *
 */
function tripal_analysis_admin_analysis_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Analysis', 'admin/tripal/chado/tripal_analysis');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_analysis_admin_analyses','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Analysis module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Analysis View', 'admin/tripal/chado/tripal_analysis/views/analyses/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administration page callbacks for the Tripal Analysis module
 *
 * We have defined a hook_get_settings() function. When a sub-module
 * is enabled, we'll look for this function to provide a form for the
 * administrative setting.
 *
 * @return
 *  A form API array describing an administrative form
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_admin() {
  // Create a new administrative form. We'll add main functions to the form
  // first (Sync, Reindex, Clean, Taxonify). Thereafter, any sub-module that
  // has a setting will be added.
  $form = array();

  // Add sub-module settings. Pull all sub-module information from
  // {tripal_analysis} table
  $sql = "SELECT modulename FROM {tripal_analysis}";
  $result = db_query($sql);
  $counter = 0;  //keep track of the number of sub-modules
  while ($data = $result->fetchObject()) {

    // Check if the hook_get_settings() function is already defined.
    $func = $data->modulename . "_get_settings";
    $functions = get_defined_functions();
    $settings;
    foreach ($functions['user'] as $function) {
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
    $counter++;
  }

  if($counter == 0) {
    $form['nothing'] = array(
      '#markup' => t('There are currently no settings to configure. However, analysis extension modules may add items here when they are installed.')
    );
  }
  return system_settings_form($form);
}

/**
 * Displays the Set Drupal Taxonomy for Analysis Features From
 *
 * @param $form
 *  The administrative form as it is currently
 *
 * @return
 *  A form API array describing an administrative form
 *
 * @ingroup tripal_analysis
 */
function get_tripal_analysis_admin_form_taxonomy_set(&$form) {
  $form['taxonify'] = array(
      '#type' => 'fieldset',
      '#title' => t('Assign Drupal Taxonomy to Analysis Features')
  );

  // get the list of analyses
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $lib_rset = chado_query($sql);

  // iterate through all of the libraries
  $lib_boxes = array();
  while ($analysis = $lib_rset->fetchObject()) {
    $lib_boxes[$analysis->analysis_id] = "$analysis->name";
  }

  $form['taxonify']['description'] = array(
       '#type' => 'item',
       '#value' => t("Drupal allows for assignment of \"taxonomy\" or catagorical terms to " .
          "nodes. These terms allow for advanced filtering during searching. This option allows " .
          "for setting taxonomy only for features that belong to the selected analyses below.  All other features will be unaffected.  To set taxonomy for all features in the site see the Feature Administration page."),
     '#weight' => 1,
  );

  $form['taxonify']['tx-analyses'] = array(
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
 * The "Reindex Analysis Nodes" form
 *
 * @param $form
 *  The administrative form as it is currently
 *
 * @return
 *  A form API array describing an administrative form
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
  $sql = "SELECT * FROM {analysis} ORDER BY name";
  $lib_rset = chado_query($sql);

  // iterate through all of the libraries
  $lib_boxes = array();
  while ($analysis = $lib_rset->fetchObject()) {
    $lib_boxes[$analysis->analysis_id] = "$analysis->name";
  }
  $form['reindex']['description'] = array(
       '#type' => 'item',
       '#value' => t("This option allows for reindexing of only those features that belong to the selected analyses below. All other features will be unaffected.  To reindex all features in the site see the Feature Administration page."),
     '#weight' => 1,
  );

  $form['reindex']['re-analyses'] = array(
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
 * Validate the administrative form
 * @todo Stephen: Why is validate used rather then submit?
 *
 * @param $form
 *   The form API array of the form to be validated
 * @form_state
 *   The user submitted values
 *
 * @ingroup tripal_analysis
 */
function tripal_analysis_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  // -------------------------------------
  // Submit the Reindex Job if selected
  if ($form_state['values']['op'] == t('Reindex Features')) {
    global $user;  // we need access to the user info
    $job_args = array();
    $analyses = $form_state['values']['re-analyses'];
    foreach ($analyses as $analysis_id) {
      if ($analysis_id and preg_match("/^\d+$/i", $analysis_id)) {
        // get the analysis info
        $sql = "SELECT * FROM {analysis} WHERE analysis_id = :analysis_id";
        $analysis = chado_query($sql, array(':analysis_id' => $analysis_id))->fetchObject();
        $job_args[0] = $analysis_id;
        tripal_add_job("Reindex features for analysis: $analysis->name", 'tripal_analysis',
             'tripal_analysis_reindex_features', $job_args, $user->uid);
      }
    }
  }

  // -------------------------------------
  // Submit the Taxonomy Job if selected
  if ($form_state['values']['op'] == t('Set Feature Taxonomy')) {
    global $user;  // we need access to the user info
    $job_args = array();
    $analyses = $form_state['values']['tx-analyses'];
    foreach ($analyses as $analysis_id) {
      if ($analysis_id and preg_match("/^\d+$/i", $analysis_id)) {
        // get the analysis info
        $sql = "SELECT * FROM {analysis} WHERE analysis_id = :analysis_id";
        $analysis = chado_query($sql, array(':analysis_id' => $analysis_id))->fetchObject();
        $job_args[0] = $analysis_id;
        tripal_add_job("Set taxonomy for features in analysis: $analysis->name", 'tripal_analysis',
             'tripal_analysis_taxonify_features', $job_args, $user->uid);
      }
    }
  }
}
