<?php
/**
 * @file
 * Administration of features
 */

/**
 * Launchpad for feature administration.
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin_feature_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Features', 'admin/tripal/chado/tripal_feature');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_feature_admin_features','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Feature module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Features View', 'admin/tripal/chado/tripal_feature/views/features/enable').'</li>';
    $output .= '</ul>';
  }

  // Add a summary chart.
  $organism_feature_count = chado_select_record(
    'organism_feature_count',
    array('*'),
    array(),
    array('order_by' => array('genus' => 'ASC', 'species' => 'ASC', 'feature_type' => 'ASC', 'num_features' => 'DESC'))
  );
  $summary = array();
  $organisms = array();
  $types = array();
  $chart = array();
  foreach ($organism_feature_count as $row) {

    $summary[$row->organism_id][$row->cvterm_id] = $row->num_features;

    $organisms[$row->organism_id]['organism_id'] = $row->organism_id;
    $organisms[$row->organism_id]['genus'] = $row->genus;
    $organisms[$row->organism_id]['species'] = $row->species;
    $organisms[$row->organism_id]['common_name'] = $row->common_name;
    $organisms[$row->organism_id]['scientific_name'] = $row->genus . ' ' . $row->species;
    $organisms[$row->organism_id]['total_features'] = (isset($organisms[$row->organism_id]['total_features'])) ? $organisms[$row->organism_id]['total_features'] + $row->num_features : $row->num_features;

    $types[$row->cvterm_id]['cvterm_id'] = $row->cvterm_id;
    $types[$row->cvterm_id]['name'] = $row->feature_type;
    $types[$row->cvterm_id]['total_features'] = (isset($types[$row->cvterm_id]['total_features'])) ? $types[$row->cvterm_id]['total_features'] + $row->num_features : $row->num_features;


    $chart[$row->cvterm_id]['cvterm_id'] = $row->cvterm_id;
    $chart[$row->cvterm_id]['name'] = $row->feature_type;
    $chart[$row->cvterm_id]['max_features'] = (isset($types[$row->cvterm_id]['max_features'])) ? max($types[$row->cvterm_id]['max_features'], $row->num_features) : $row->num_features;
    $chart[$row->cvterm_id]['organisms'][] = array(
      'name' => $row->genus . ' ' . $row->species,
      'value' => (int) $row->num_features
    );
    $type_names[$row->cvterm_id] = $row->feature_type;
    $organism_names[$row->organism_id] = $row->genus . ' ' . $row->species;
  }

  // Quick processing of the organisms array per type
  // to build-up the stacked bars.
  foreach ($chart as $type_id => $bar) {
    $y0 = 0;
    $y1 = 0;
    $chart[$type_id]['bars'] = array();
    foreach ($bar['organisms'] as $k => $org) {
      $y0 = $y1;
      $y1 = $y0 + $org['value'];
      $chart[$type_id]['bars'][$k]['name'] = $org['name'];
      $chart[$type_id]['bars'][$k]['y0'] = $y0;
      $chart[$type_id]['bars'][$k]['y1'] = $y1;
    }
    $chart[$type_id]['total_features'] = $y1;
  }

  $variables['organisms'] = $organisms;
  $variables['types'] = $types;

  // Sort based on the total number of features.
  // NOTE: This changes the keys so it's no longer the organism/type_id.
  usort($chart, 'tripal_feature_admin_summary_sort');
  sort($type_names);
  sort($organism_names);

  // We also need to add information about the materialized views
  // so that admin can update it and know how recent the data is.
  $mview = db_query('
    SELECT mview_id, name, last_update
    FROM tripal_mviews
    WHERE mv_table=:mv_table',
    array(':mv_table' => 'organism_feature_count')
  )->fetchObject();

  tripal_add_d3js();
  drupal_add_css(drupal_get_path('module','tripal_feature') . '/theme/css/tripal_feature.css');
  drupal_add_js(drupal_get_path('module','tripal_feature') . '/theme/js/tripalFeature.adminChart.js');
  drupal_add_js(array('tripalFeature' => array('admin' => array(
    'summary' => $chart,
    'types' => $type_names,
    'organisms' => $organism_names,
    'mviewUrl' => url('admin/tripal/schema/mviews/update/' . $mview->mview_id),
    'mviewUable' => $mview->name,
    'mviewLastUpdate' => format_date($mview->last_update),
  ))), 'setting');


  return $output;
}

/**
 * Feature Settings page
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin() {

  // FEATURE PAGE TITLES
  // Using the Chado Node: Title & Path API
  $details = array(
    'module' => 'tripal_feature',
    'content_type' => 'chado_feature',
      // An array of options to use under "Page Titles"
      // the key should be the token and the value should be the human-readable option
    'options' => array(
      '[feature.name]' => 'Feature Name Only',
      '[feature.uniquename]' => 'Feature Unique Name Only',
      // there should always be one options matching the unique constraint.
      '[feature.name], [feature.uniquename] ([feature.type_id>cvterm.name]) [feature.organism_id>organism.genus] [feature.organism_id>organism.species]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name'
    ),
    // the token indicating the unique constraint in the options array
    'unique_option' => '[feature.name], [feature.uniquename] ([feature.type_id>cvterm.name]) [feature.organism_id>organism.genus] [feature.organism_id>organism.species]'
  );
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // FEATURE NODE URL
  // Using the Chado Node: Title & Path API
  $details = array(
    'module' => 'tripal_feature',
    'content_type' => 'chado_feature',
      // An array of options to use under "Page URL"
      // the key should be the token and the value should be the human-readable option
    'options' => array(
      '/feature/[feature.feature_id]' => 'Feature ID',
        // there should always be one options matching the unique constraint.
      '/feature/[feature.organism_id>organism.genus]/[feature.organism_id>organism.species]/[feature.type_id>cvterm.name]/[feature.uniquename]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name'
    )
  );
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  // FEATURE BROWSER
  $form['browser'] = array(
     '#type' => 'fieldset',
     '#title' => t('Feature Browser'),
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
  );
  $form['browser']['browser_desc'] = array(
     '#markup' => t('A feature browser can be added to an organism page to allow users to quickly ' .
        'access a feature.  This will most likely not be the ideal mechanism for accessing feature ' .
        'information, especially for large sites, but it will alow users exploring the site (such ' .
        'as students) to better understand the data types available on the site.'),

  );
  $form['browser']['feature_types'] = array(
     '#title'       => t('Feature Types'),
     '#type'        => 'textarea',
     '#description' => t("Enter the Sequence Ontology (SO) terms for the feature types that " .
                         "will be shown in the feature browser."),
     '#default_value' => variable_get('chado_browser_feature_types', 'gene mRNA'),
  );


  $form['browser']['set_browse_button'] = array(
     '#type' => 'submit',
     '#value' => t('Set Browser'),
     '#weight' => 2,
  );

  // FEATURE SUMMARY REPORT
  $form['summary'] = array(
     '#type' => 'fieldset',
     '#title' => t('Feature Summary Report'),
     '#collapsible' => TRUE,
     '#collapsed' => TRUE,
  );
  $form['summary']['feature_mapping'] = array(
     '#title' => 'Map feature types',
     '#description' => t('You may specify which Sequence Ontology (SO) terms to show in the ' .
        'feature summary report by listing them in the following text area.   Enter one per line. ' .
        'If left blank, all SO terms for all features will be shown in the report. Only those terms ' .
        'listed below will be shown in the report. Terms will appear in the report in the same order listed. To rename a ' .
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

  return system_settings_form($form);
}

/**
 * Validate the feature settings forms
 *
 * @ingroup tripal_feature
 */
function tripal_feature_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

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
  if ($a['total_features'] == $b['total_features']) return 0;
  return $b['total_features'] - $a['total_features'];
}