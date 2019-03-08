<?php
/**
 * @file
 * Administration of feature maps
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_legacy_featuremap
 */
function tripal_featuremap_admin_featuremaps_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Maps', 'admin/tripal/legacy/tripal_featuremap');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_featuremap_admin_featuremaps', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Feature Map Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Feature Map Admin', 'admin/tripal/legacy/tripal_featuremap/views/featuremaps/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_legacy_featuremap
 */
function tripal_featuremap_admin() {
  $form = [];

  // If your module is using the Chado Node: Title & Path API to allow custom titles
  // for your node type then you need to add the configuration form for this functionality.
  $details = [
    'module' => 'tripal_featuremap',
    // the name of the MODULE implementing the content type
    'content_type' => 'chado_featuremap',
    // the name of the content type
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      // there should always be one options matching the unique constraint.
      '[featuremap.name]' => 'Unique Contraint: The name of the map',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[featuremap.name]',
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // URL ALIAS
  $details = [
    'module' => 'tripal_featuremap',
    'content_type' => 'chado_featuremap',
    'options' => [
      '/featuremap/[featuremap.featuremap_id]' => 'Feature Map ID',
      '/featuremap/[featuremap.name]' => 'Unique Contraint: The name of the Feature Map',
    ],
  ];

  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}
