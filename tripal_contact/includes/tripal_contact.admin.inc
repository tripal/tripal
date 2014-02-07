<?php
/**
 * @file
 * Handle administration of contacts.
 *
 * @ingroup tripal_contact
 */

/**
 * Launchpage for contact administration. Makes sure views are enabled and if not provides
 * links to enable them.
 *
 * @ingroup tripal_contact
 */
function tripal_contact_admin_contact_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Contacts', 'admin/tripal/chado/tripal_contact');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_contact_admin_contacts','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Contact module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Contacts View', 'admin/tripal/chado/tripal_contact/views/contacts/enable').'</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Administrative settings form
 *
 * @ingroup tripal_contact
 */
function tripal_contact_admin() {
  $form = array();

  $form['nothing'] = array(
    '#markup' => t('There are currently no settings to configure.')
  );

  return system_settings_form($form);
}


/**
 * Reindex nodes for drupal search
 *
 * @ingroup tripal_contact
 */
function get_tripal_contact_admin_form_reindex_set(&$form) {


}

/**
 * Validate the contact settings form.
 *
 * @ingroup tripal_contact
 */
function tripal_contact_admin_validate($form, &$form_state) {

}
