<?php
/**
 * @file
 * Provides administration of controlled vocabularies & their terms.
 */

/**
 * Provide landing page to the new admin pages
 *
 * @ingroup tripal_cv
 */
function tripal_cv_admin_cv_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Modules', 'admin/tripal/chado');
  $breadcrumb[] = l('Vocabularies', 'admin/tripal/chado/tripal_cv');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $cvs_view = views_embed_view('tripal_cv_admin_cvs','default');
  $cvterms_view = views_embed_view('tripal_cv_admin_cvterms','default');
  if (isset($cvs_view) && isset($cvterms_view)) {
    $output .= $cvs_view;
  }
  else {
    $output .= '<p>The Tripal Controlled Vocabulary module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    if (!isset($cvs_view)) {
      $output .= '<li>'.l('Tripal Vocabularies', 'admin/tripal/chado/tripal_cv/views/cvs/enable').'</li>';
    }
    if (!isset($cvterm_view)) {
      $output .= '<li>'.l('Tripal Vocabulary Terms', 'admin/tripal/chado/tripal_cv/views/cvterms/enable').'</li>';
    }
    $output .= '</ul>';
  }

  return $output;
}

/**
 * 
 */
function tripal_cv_admin_set_defaults_form() {
  $form = array();
  
  $form['instructions'] = array(
    '#markup' => 'use the following...'
  );
  
  return $form;
}