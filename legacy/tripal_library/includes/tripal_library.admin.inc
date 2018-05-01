<?php
/**
 * @file
 * Admin launchpad
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_admin_libraries_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Libraries', 'admin/tripal/legacy/tripal_library');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_library_admin_libraries', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Tripal Library Module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Library Admin', 'admin/tripal/legacy/tripal_library/views/libraries/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_legacy_library
 */
function tripal_library_admin() {
  $form = [];

  // If your module is using the Chado Node: Title & Path API to allow custom titles
  // for your node type then you need to add the configuration form for this functionality.
  $details = [
    'module' => 'tripal_library',
    // the name of the MODULE implementing the content type
    'content_type' => 'chado_library',
    // the name of the content type
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '[library.name]' => 'Library Name Only',
      // there should always be one options matching the unique constraint.
      '[library.name], [library.uniquename] ([library.type_id>cvterm.name]) [library.organism_id>organism.genus] [library.organism_id>organism.species]' => 'Unique Contraint: Includes the name, type and organism scientific name',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[library.name], [library.uniquename] ([library.type_id>cvterm.name]) [library.organism_id>organism.genus] [library.organism_id>organism.species]',
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // URL ALIAS
  $details = [
    'module' => 'tripal_library',
    'content_type' => 'chado_library',
    'options' => [
      '/library/[library.library_id]' => 'Library ID',
      '/library/[library.organism_id>organism.genus]/[library.organism_id>organism.species]/[library.type_id>cvterm.name]/[library.uniquename]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name',
    ],
  ];

  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}
