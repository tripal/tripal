<?php

/**
 * @file
 * Contains functions displaying administrative pages and forms.
 *
 * @ingroup tripal_analysis
 */

/**
 * Landing page for administration. Ensures Views are enabled & if not provides links to do so.
 *
 * @ingroup tripal_analysis
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

}

