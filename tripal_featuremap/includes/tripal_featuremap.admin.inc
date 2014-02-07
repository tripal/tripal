<?php
/**
 * @file
 * Administration of feature maps
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_admin_featuremaps_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Maps', 'admin/tripal/chado/tripal_featuremap');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_featuremap_admin_featuremaps','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Feature Map Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Feature Map Admin', 'admin/tripal/chado/tripal_featuremap/views/featuremaps/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_featuremap
 */
function tripal_featuremap_admin() {
  $form = array();

  $form['nothing'] = array(
    '#markup' => t('There are currently no settings to configure.')
  );

  return system_settings_form($form);
}
