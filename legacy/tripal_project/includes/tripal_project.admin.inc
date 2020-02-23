<?php
/**
 * @file
 * Administration of projects
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_legacy_project
 */
function tripal_project_admin_project_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Projects', 'admin/tripal/legacy/tripal_project');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_project_admin_projects', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Project module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Projects View', 'admin/tripal/legacy/tripal_project/views/projects/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Project settings
 *
 * @ingroup tripal_legacy_project
 */
function tripal_project_admin($form, $form_state) {
  $form = [];

  // If your module is using the Chado Node: Title & Path API to allow custom titles
  // for your node type then you need to add the configuration form for this functionality.
  $details = [
    'module' => 'tripal_project',
    // the name of the MODULE implementing the content type
    'content_type' => 'chado_project',
    // the name of the content type
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '[project.name]' => 'project Name Only',
      // there should always be one options matching the unique constraint.
      '[project.name]' => 'Unique Contraint: The project name',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[project.name]',
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // If the module is using the "Chado Node: Title & Path API" to allow custom URLs
  // for your node type then you need to add the configuration form for this functionality.
  $details = [
    'module' => 'tripal_project',
    // the name of the MODULE implementing the content type
    'content_type' => 'chado_project',
    // the name of the content type
    // An array of options to use under "Page URLs"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '/project/[project.name]' => 'Project Name Only',
      // there should always be one options matching the unique constraint.
      // If you have a more human-readable constraint, then that is preferrable.
      // See the tripal feature module for a good example of this.
      '/project/[project.project_id]' => 'Unique Contraint: The Project ID',
    ],
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}
