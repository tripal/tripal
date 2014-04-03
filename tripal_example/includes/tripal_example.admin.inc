<?php
/**
 * @file
 * This file contains the functions used for administration of the module
 *
 */

function tripal_example_admin_examples_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Examples', 'admin/tripal/chado/tripal_example');
  drupal_set_breadcrumb($breadcrumb);

  
  // EXPLANATION:  Typically for all Tripal modules the home administrative
  // page for the module contains a search form to help the adminstrator
  // locate records.  The following example code adds a default View to
  // the page
  /*
  // Add the view
  $view = views_embed_view('tripal_example_admin_examples','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Example Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Example Admin', 'admin/tripal/chado/tripal_example/views/examples/enable').'</li>';
    $output .= '</ul>';
  }
  */
  $output = 'Typically a search view goes here';  

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_example
 */
function tripal_example_admin() {
  $form = array();

  $form['nothing'] = array(
    '#markup' => t('There are currently no settings to configure.')
  );

  return system_settings_form($form);
}



/**
 *
 * @ingroup tripal_example
 */
function tripal_example_admin_validate($form, &$form_state) {
  
}

