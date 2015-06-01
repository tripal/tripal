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
  //-----------------------------------
  // We are using the organism_feature_count materialized view as the source for our data.
  // Thus grab all the records from this materialized view.
  $organism_feature_count = chado_select_record(
    'organism_feature_count',
    array('*'),
    array(),
    array('order_by' => array('genus' => 'ASC', 'species' => 'ASC', 'feature_type' => 'ASC', 'num_features' => 'DESC'))
  );

  // Initialize variables.
  $chart = array();
  $type_names = array();
  $organism_names = array();
  $max_bar_height = 0;

  // Process each row of the materialzied view into the chart array.
  // Note: it's first keyed by type since each type will be a bar. Each type will have
  // a "bars" array with the start (y0) and end (y1) height on the bar for a given
  // organism. Finally we keep a record of the names of the types & organisms
  // for axis' and legend generation respectively.
  foreach ($organism_feature_count as $row) {

    // Build up the easy details for the current row's type. These will be overridden
    // multiple times but that's more efficient than checking each time.
    $chart[$row->cvterm_id]['cvterm_id'] = $row->cvterm_id;
    $chart[$row->cvterm_id]['name'] = str_replace('_', ' ', $row->feature_type);

    // Save the name of the type and organism into their respective arrays
    // for generation of axis' and legends for the chart.
    $type_names[$row->cvterm_id] = $row->feature_type;
    $organism_names[$row->organism_id] = $row->genus . ' ' . $row->species;

    // Save information about the current organism. This isn't actually used by the
    // chart but can be used to debug the bar generation to follow.
    $chart[$row->cvterm_id]['organisms'][] = array(
      'name' => $row->genus . ' ' . $row->species,
      'value' => (int) $row->num_features
    );

    // Now to build the bar array with the start (y0) and end (y1) height on the
    // bar for a given organism.
    // NOTE: we cannot assume the types are all in order so store y0 & y1 in the
    // $chart[type] array.
    // If y0 has not yet been set for this type then we're starting with the first
    // chunk (organism) on the bar.
    if (!isset($chart[$row->cvterm_id]['y0'])) {
      $chart[$row->cvterm_id]['y0'] = 0;
      $chart[$row->cvterm_id]['y1'] = $row->num_features;
    }
    // Otherwise, add the next chunk (organism) on top of the pre-existing bar.
    else {
      $chart[$row->cvterm_id]['y0'] = $chart[$row->cvterm_id]['y1'];
      $chart[$row->cvterm_id]['y1'] = $chart[$row->cvterm_id]['y0'] + $row->num_features;
    }
    // Now save the bar chunk we just determined.
    $chart[$row->cvterm_id]['bars'][] = array(
      'name' => $row->genus . ' ' . $row->species,
      'y0' => $chart[$row->cvterm_id]['y0'],
      'y1' => $chart[$row->cvterm_id]['y1'],
    );

    // We also need to keep track of the total number of features for a single bar (Type).
    $chart[$row->cvterm_id]['total_features'] = (int) $chart[$row->cvterm_id]['y1'];
    // And the maximum "height" for all bars.
    if ($max_bar_height < $chart[$row->cvterm_id]['total_features']) {
      $max_bar_height = (int) $chart[$row->cvterm_id]['total_features'];
    }
  }

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

  // Save everything we just determined as a Drupal JS settings so that we have access to
  // it in our js script.
  $last_updated = $mview->last_update ? format_date($mview->last_update) : '';
  drupal_add_js(array('tripalFeature' => array('admin' => array(
    'summary' => $chart,
    'types' => $type_names,
    'organisms' => $organism_names,
    'legendPosition' => 'top',
    'maxBarHeight' => $max_bar_height,
    'mviewUrl' => url('admin/tripal/schema/mviews/update/' . $mview->mview_id),
    'mviewUable' => $mview->name,
    'mviewLastUpdate' => $last_updated,
    'figureDesc' => '<span class="figure-title">Feature Composition</span>: This figure depicts the type and source organism of features in your Tripal site. It is populated from the <em>'.$mview->name.'</em> materialized view which was last updated on <em>'.$last_updated.'</em>. <strong><em>To update this chart, <a href="'.url('admin/tripal/schema/mviews/update/' . $mview->mview_id).'">submit a job to update the materialized view</a></em></strong>.'
  ))), 'setting');

  // Finally add all the javascript and css needed to render the chart.
  tripal_add_d3js();
  drupal_add_css(drupal_get_path('module','tripal_feature') . '/theme/css/tripal_feature.css');
  drupal_add_js(drupal_get_path('module','tripal_feature') . '/theme/js/tripalFeature.adminChart.js');

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
