<?php
/**
 * @file
 * Administration of organisms
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin_organism_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Organisms', 'admin/tripal/chado/tripal_organism');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_organism_admin_organisms','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Organism module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Organisms View', 'admin/tripal/chado/tripal_organism/views/organisms/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings for chado_orgnism
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin() {

  $form = array();

  $form['nothing'] = array(
    '#markup' => t('There are currently no settings to configure.')
  );

  return system_settings_form($form);
}


/**
 * Validate the organism settings form
 *
 * @ingroup tripal_organism
 */
function tripal_organism_admin_validate($form, &$form_state) {

}
