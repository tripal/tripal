<?php
/**
 * @file
 * Administration of features
 */

/**
 * Launchpad for feature administration.
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_admin_feature_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Features', 'admin/tripal/legacy/tripal_feature');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_feature_admin_features', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Feature module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Features View', 'admin/tripal/legacy/tripal_feature/views/features/enable') . '</li>';
    $output .= '</ul>';
  }

  // Add a summary chart.
  //-----------------------------------
  $output .= theme('tripal_feature_bar_chart_type_organism_summary');
  drupal_add_js('
    Drupal.behaviors.tripalFeature_moveAdminSummaryChart = {
      attach: function (context, settings) {
        jQuery("#tripal-feature-admin-summary").insertBefore( jQuery(".view-filters") );
    }};
  ', 'inline');

  return $output;
}

/**
 * Feature Settings page
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_admin() {

  // FEATURE PAGE TITLES
  // Using the Chado Node: Title & Path API
  $details = [
    'module' => 'tripal_feature',
    'content_type' => 'chado_feature',
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '[feature.name]' => 'Feature Name Only',
      '[feature.uniquename]' => 'Feature Unique Name Only',
      // there should always be one options matching the unique constraint.
      '[feature.name], [feature.uniquename] ([feature.type_id>cvterm.name]) [feature.organism_id>organism.genus] [feature.organism_id>organism.species]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[feature.name], [feature.uniquename] ([feature.type_id>cvterm.name]) [feature.organism_id>organism.genus] [feature.organism_id>organism.species]',
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // FEATURE NODE URL
  // Using the Chado Node: Title & Path API
  $details = [
    'module' => 'tripal_feature',
    'content_type' => 'chado_feature',
    // An array of options to use under "Page URL"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '/feature/[feature.feature_id]' => 'Feature ID',
      // there should always be one options matching the unique constraint.
      '/feature/[feature.organism_id>organism.genus]/[feature.organism_id>organism.species]/[feature.type_id>cvterm.name]/[feature.uniquename]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name',
    ],
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  // FEATURE BROWSER
  $form['browser'] = [
    '#type' => 'fieldset',
    '#title' => t('Feature Browser'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];
  $form['browser']['browser_desc'] = [
    '#markup' => t('A feature browser can be added to an organism page to allow users to quickly ' .
      'access a feature.  This will most likely not be the ideal mechanism for accessing feature ' .
      'information, especially for large sites, but it will alow users exploring the site (such ' .
      'as students) to better understand the data types available on the site.'),

  ];
  $form['browser']['feature_types'] = [
    '#title' => t('Feature Types'),
    '#type' => 'textarea',
    '#description' => t("Enter the Sequence Ontology (SO) terms for the feature types that " .
      "will be shown in the feature browser."),
    '#default_value' => variable_get('chado_browser_feature_types', 'gene mRNA'),
  ];


  $form['browser']['set_browse_button'] = [
    '#type' => 'submit',
    '#value' => t('Set Browser'),
    '#weight' => 2,
  ];

  // FEATURE SUMMARY REPORT
  $form['summary'] = [
    '#type' => 'fieldset',
    '#title' => t('Feature Summary Report'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ];
  $form['summary']['feature_mapping'] = [
    '#title' => 'Map feature types',
    '#description' => t('You may specify which Sequence Ontology (SO) terms to show in the ' .
      'feature summary report by listing them in the following text area.   Enter one per line. ' .
      'If left blank, all SO terms for all features will be shown in the report. Only those terms ' .
      'listed below will be shown in the report. Terms will appear in the report in the same order listed. To rename a ' .
      'SO term to be more human readable form, use an \'=\' sign after the SO term (e.g. \'polypeptide = Protein\')'),
    '#type' => 'textarea',
    '#rows' => 15,
    '#default_value' => variable_get('tripal_feature_summary_report_mapping', ''),
  ];
  $form['summary']['set_summary_button'] = [
    '#type' => 'submit',
    '#value' => t('Set Summary'),
    '#weight' => 2,
  ];

  return system_settings_form($form);
}

/**
 * Validate the feature settings forms
 *
 * @ingroup tripal_legacy_feature
 */
function tripal_feature_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = [];

  variable_set('chado_browser_feature_types', $form_state['values']['feature_types']);

  switch ($form_state['values']['op']) {

    case t('Set Summary') :
      variable_set('tripal_feature_summary_report_mapping', $form_state['values']['feature_mapping']);
      break;

  }

}

/**
 * USort function for the admin summary chart.
 * Not meant to be called directly.
 */
function tripal_feature_admin_summary_sort($a, $b) {
  if ($a['total_features'] == $b['total_features']) {
    return 0;
  }
  return $b['total_features'] - $a['total_features'];
}
