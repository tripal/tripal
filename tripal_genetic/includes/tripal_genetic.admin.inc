<?php
/**
 * @file
 * Provides administration of chado genetic data
 */

/**
 * Administration launchpad
 *
 * @ingroup tripal_genetic
 */
function tripal_genetic_admin_genetics_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Genetics', 'admin/tripal/chado/tripal_genetic');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_genetic_admin_genetics','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Genetic Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Genetic Admin', 'admin/tripal/chado/tripal_genetic/views/genetics/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}