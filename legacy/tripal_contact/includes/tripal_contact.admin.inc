<?php
/**
 * @file
 * Handle administration of contacts.
 *
 * @ingroup tripal_legacy_contact
 */

/**
 * Launchpage for contact administration. Makes sure views are enabled and if
 * not provides links to enable them.
 *
 * @ingroup tripal_legacy_contact
 */
function tripal_contact_admin_contact_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Contacts', 'admin/tripal/legacy/tripal_contact');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_contact_admin_contacts', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Contact module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Contacts View', 'admin/tripal/legacy/tripal_contact/views/contacts/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_legacy_contact
 */
function tripal_contact_admin() {
  $form = [];

  // If your module is using the Chado Node: Title & Path API to allow custom titles
  // for your node type then you need to add the configuration form for this functionality.
  $details = [
    'module' => 'tripal_contact',
    // the name of the MODULE implementing the content type
    'content_type' => 'chado_contact',
    // the name of the content type
    // An array of options to use under "Page Titles"
    // the key should be the token and the value should be the human-readable option
    'options' => [
      '[contact.name]' => 'Contact Name Only',
      // there should always be one options matching the unique constraint.
      '[contact.name]' => 'Unique Contraint: The name of the contact',
    ],
    // the token indicating the unique constraint in the options array
    'unique_option' => '[contact.name]',
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_title($form, $form_state, $details);

  // URL ALIAS
  $details = [
    'module' => 'tripal_contact',
    'content_type' => 'chado_contact',
    'options' => [
      '/contact/[contact.contact_id]' => 'Contact ID',
      '/contact/[contact.name]' => 'Unique Contraint: The name of the contact',
    ],
  ];

  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);
}


/**
 * Reindex nodes for drupal search
 *
 * @ingroup tripal_legacy_contact
 */
function get_tripal_contact_admin_form_reindex_set(&$form) {


}

/**
 * Validate the contact settings form.
 *
 * @ingroup tripal_legacy_contact
 */
function tripal_contact_admin_validate($form, &$form_state) {

}
