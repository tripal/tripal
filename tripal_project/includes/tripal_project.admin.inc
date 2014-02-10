<?php
/**
 * @file
 * Administration of projects
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_project
 */
function tripal_project_admin_project_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Projects', 'admin/tripal/chado/tripal_project');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_project_admin_projects','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Project module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Projects View', 'admin/tripal/chado/tripal_project/views/projects/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}
/**
 * Project settings
 *
 * @ingroup tripal_project
 */
function tripal_project_admin($form, $form_state) {
  $form = array();

  $form['nothing'] = array(
    '#markup' => t('There are currently no settings to configure.')
  );

  return system_settings_form($form);
}

/**
 * Validate project settings
 *
 * @ingroup tripal_project
 */
function tripal_project_admin_validate($form, &$form_state) {

}
