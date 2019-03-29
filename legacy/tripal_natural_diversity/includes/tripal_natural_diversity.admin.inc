<?php
/**
 * @file
 * Administration of the natural diversity module
 */

/**
 * Admin Launchpad
 *
 * @ingroup tripal_legacy_natural_diversity
 */
function tripal_natural_diversity_admin_natdiv_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Diversity Experiments', 'admin/tripal/legacy/tripal_natdiv');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_natural_diversity_admin_natdiv_exp', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Natural Diversity Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Natural Diversity Experiments Admin', 'admin/tripal/legacy/tripal_natdiv/views/natdiv_exp/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}
