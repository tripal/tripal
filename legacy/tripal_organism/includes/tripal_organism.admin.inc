<?php
/**
 * @file
 * Administration of organisms
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_admin_organism_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Organisms', 'admin/tripal/legacy/tripal_organism');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_organism_admin_organisms', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Organism module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Organisms View', 'admin/tripal/legacy/tripal_organism/views/organisms/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings for chado_orgnism
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_admin() {

  $form = [];

  // If your module is using the Chado Node: Title & Path API to allow custom titles
  // for your node type then you need to add the configuration form for this functionality.
  $details = [
    'module' => 'tripal_organism',
    // the name of the MODULE implementing the content type
    'content_type' => 'chado_organism',
    // the name of the content type
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '[organism.name]' => 'Organism Name Only',
      // there should always be one options matching the unique constraint.
      '[organism.genus] [organism.species]' => 'Unique Contraint: The scientific name',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[organism.genus] [organism.species]',
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // URL ALIAS
  $details = [
    'module' => 'tripal_organism',
    'content_type' => 'chado_organism',
    'options' => [
      '/organism/[organism.organism_id]' => 'Organism ID',
      '/organism/[organism.abbreviation]' => 'The abbreviation for the organism',
      '/organism/[organism.common_name]' => 'The common name of the organism',
      '/organism/[organism.genus]/[organism.species]' => 'Unique Contraint: the scientific name.',
    ],
  ];

  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}


/**
 * Validate the organism settings form
 *
 * @ingroup tripal_legacy_organism
 */
function tripal_organism_admin_validate($form, &$form_state) {

}
