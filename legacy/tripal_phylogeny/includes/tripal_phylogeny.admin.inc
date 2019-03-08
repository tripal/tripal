<?php

/**
 * Administrative settings form
 *
 * @ingroup tripal_legacy_phylogeny
 */
function tripal_phylogeny_admin() {
  $form = [];


  // PHYLOTREE NODE TITLES
  // If your module is using the Chado Node: Title & Path API to allow
  // custom titles for your node type then you need to add the
  // configuration form for this functionality.
  $details = [
    'module' => 'tripal_phylogeny',
    'content_type' => 'chado_phylotree',
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '[phylotree.name]' => 'Tree Name Only',
      // there should always be one options matching the unique constraint.
      '[phylotree.phylotree_id]' => 'The Chado ID for Phylotrees',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[phylotree.phylotree_id]',
  ];

  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);


  // PHYLOTREE NODE URL
  // Using the Chado Node: Title & Path API
  $details = [
    'module' => 'tripal_phylogeny',
    'content_type' => 'chado_phylotree',
    // An array of options to use under "Page URL"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '/tree/[phylotree.name]' => 'Tree Name Only',
      // there should always be one options matching the unique constraint.
      '/tree/[phylotree.phylotree_id]' => 'The Chado ID for Phylotrees',
    ],
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);

}