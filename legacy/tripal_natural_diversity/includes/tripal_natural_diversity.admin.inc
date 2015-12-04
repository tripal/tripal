<?php
/**
 * @file
 * Administration of the natural diversity module
 */

/**
 * Admin Launchpad
 *
 * @ingroup tripal_natural_diversity
 */
function tripal_natural_diversity_admin_natdiv_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Diversity Experiments', 'admin/tripal/chado/tripal_natdiv');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_natural_diversity_admin_natdiv_exp','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Natural Diversity Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Natural Diversity Experiments Admin', 'admin/tripal/chado/tripal_natdiv/views/natdiv_exp/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}
