<?php
/**
 * @file
 * Administration of stocks
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_admin_stock_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = [];
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/legacy');
  $breadcrumb[] = l('Stocks', 'admin/tripal/legacy/tripal_stock');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_stock_admin_stocks', 'default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Stock module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    $output .= '<li>' . l('Stocks View', 'admin/tripal/legacy/tripal_stock/views/stocks/enable') . '</li>';
    $output .= '</ul>';
  }

  return $output;
}

/**
 * Provide administration options for chado_stocks
 *
 * @return
 *   Form array (as described by the drupal form api)
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_admin() {
  $form = [];


  // STOCK PAGE TITLES CONFIGURATION
  $details = [
    'module' => 'tripal_stock',
    'content_type' => 'chado_stock',
    'options' => [
      '[stock.name]' => 'Stock Name Only',
      '[stock.uniquename]' => 'Stock Unique Name Only',
      '[stock.name], [stock.uniquename] ([stock.type_id>cvterm.name]) [stock.organism_id>organism.genus] [stock.organism_id>organism.species]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name',
    ],
    'unique_option' => '[stock.name], [stock.uniquename] ([stock.type_id>cvterm.name]) [stock.organism_id>organism.genus] [stock.organism_id>organism.species]',
  ];
  chado_add_admin_form_set_title($form, $form_state, $details);

  // URL ALIAS
  $details = [
    'module' => 'tripal_stock',
    'content_type' => 'chado_stock',
    'options' => [
      '/stock/[stock.stock_id]' => 'Stock ID',
      '/stock/[stock.organism_id>organism.genus]/[stock.organism_id>organism.species]/[stock.type_id>cvterm.name]/[stock.uniquename]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name',
    ],
  ];
  // This call adds the configuration form to your current form
  // This sub-form handles it's own validation & submit
  chado_add_admin_form_set_url($form, $form_state, $details);

  return system_settings_form($form);

}

/**
 * Implements hook_form_validate(): Validates user input
 *
 * @param $form
 *   An array describing the form that was rendered
 * @param $form_state
 *   An array describing the current state of the form including user input
 *
 * @ingroup tripal_legacy_stock
 */
function tripal_stock_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = [];

  switch ($form_state['values']['op']) {
    case  t('Set Controlled Vacabularies') :
      break;

    case t('Clean up orphaned stocks') :
      tripal_add_job('Cleanup orphaned stocks', 'tripal_stock',
        'tripal_stock_cleanup', $job_args, $user->uid);
      break;

  }
}
