<?php

/**
 * @file
 * Contains functions displaying administrative pages and forms.
 *
 * @ingroup tripal_legacy_analysis
 */

/**
 * Landing page for administration. Ensures Views are enabled & if not provides
 * links to do so.
 *
 * @ingroup tripal_legacy_analysis
 */
function tripal_analysis_admin_analysis_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Analysis', 'admin/tripal/legacy/tripal_analysis');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_analysis_admin_analyses', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Analysis module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Analysis View', 'admin/tripal/legacy/tripal_analysis/views/analyses/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administration page callbacks for the Tripal Analysis module
 *
 * @return
 *  A form API array describing an administrative form
 *
 * @ingroup tripal_legacy_analysis
 */
function tripal_analysis_admin() {
  // Create a new administrative form. We'll add main functions to the form
  // first (Sync, Reindex, Clean, Taxonify). Thereafter, any sub-module that
  // has a setting will be added.
  $form = [];

  // If your module is using the Chado Node: Title & Path API to allow custom titles
  // for your node type then you need to add the configuration form for this functionality.
  $details = [
    'module' => 'tripal_analysis',
    // the name of the MODULE implementing the content type
    'content_type' => 'chado_analysis',
    // the name of the content type
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '[analysis.name]' => 'Analysis Name Only',
      // there should always be one options matching the unique constraint.
      '[analysis.name] ([analysis.sourcename]) [analysis.program] version [analysis.programversion]' => 'Unique Contraint: Includes the name, source and program name/version',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[analysis.name] ([analysis.sourcename]) [analysis.program] version [analysis.programversion]',
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // URL ALIAS
  $details = [
    'module' => 'tripal_analysis',
    'content_type' => 'chado_analysis',
    'options' => [
      '/analysis/[analysis.analysis_id]' => 'Analysis ID',
      '/analysis/[analysis.program]/[analysis.programversion]/[analysis.sourcename]' => 'Unique Contraint: Includes the program name & version as well as the source name',
    ],
  ];

  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}

/**
 * Validate the administrative form
 *
 * @todo Stephen: Why is validate used rather then submit?
 *
 * @param $form
 *   The form API array of the form to be validated
 *
 * @form_state
 *   The user submitted values
 *
 * @ingroup tripal_legacy_analysis
 */
function tripal_analysis_admin_validate($form, &$form_state) {

}

