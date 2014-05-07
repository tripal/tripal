<?php
/**
 * @file
 * Administration of stocks
 */

/**
 * Admin launchpad
 *
 * @ingroup tripal_stock
 */
function tripal_stock_admin_stock_view() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado', 'admin/tripal/chado');
  $breadcrumb[] = l('Stocks', 'admin/tripal/chado/tripal_stock');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $view = views_embed_view('tripal_stock_admin_stocks','default');
  if (isset($view)) {
    $output .= $view;
  }
  else {
    $output .= '<p>The Stock module uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
      $output .= '<li>'.l('Stocks View', 'admin/tripal/chado/tripal_stock/views/stocks/enable').'</li>';
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
 * @ingroup tripal_stock
 */
function tripal_stock_admin() {
  $form = array();


  // STOCK PAGE TITLES CONFIGURATION

  $details = array(
    'module' => 'tripal_stock',
    'content_type' => 'chado_stock',
    'options' => array(
      '[stock.name]' => 'Stock Name Only',
      '[stock.uniquename]' => 'Stock Unique Name Only',
      '[stock.name], [stock.uniquename] ([stock.type_id>cvterm.name]) [stock.organism_id>organism.genus] [stock.organism_id>organism.species]' => 'Unique Contraint: Includes the name, uniquename, type and scientific name'
    ),
    'unique_option' => '[stock.name], [stock.uniquename] ([stock.type_id>cvterm.name]) [stock.organism_id>organism.genus] [stock.organism_id>organism.species]'
  );
  chado_add_admin_form_set_title($form, $form_state, $details);

  // STOCK URL CONFIGURATION
  $form['url'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stock URL Path'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $options = array(
    'SID[id]'      => '[id]:' . t('The Chado stock_id'),
    'stock'        => 'stock:' . t('Chado table name'),
    '[genus]'      => '[genus]:' . t('Genus to which the stock belongs'),
    '[species]'    => '[species]:' . t('Species to which the stock belongs'),
    '[type]'       => '[type]:' . t('The type of stock'),
    '[uniquename]' => '[uniquename]:' . t('The stock unique name'),
    '[name]'       => '[name]:' . t('The stock name'),
    'reset'        => t('Reset'),
  );


  $form['url']['chado_stock_url_string'] = array(
    '#title' => 'URL Syntax',
    '#type' => 'textfield',
    '#description' => t('You may rearrange elements in this text box to
      customize the URLs.  The available tags include: [id],
      [uniquename]. [name], [species], [genus], [type]. You can separate or
      include any text between the tags. Click the "Set Stock URLs" button to
      reset the URLs for all stock pages.  Click the "Save Configuration" button to
      simply save this setup. <b>Important</b>: be sure that whatever you choose will always be unique even considering
      future data that may be added.  If you include the Chado table name, genus, species, type
      and uniquename you are guaranteed to have a unique URL. For example stock/[genus]/[species]/[type]/[uniquename]'),
    '#size' => 150,
    '#default_value' => variable_get('chado_stock_url_string', '/stock/[genus]/[species]/[type]/[uniquename]'),
  );

  $form['url']['chado_stock_url'] = array(
    '#title'         => t('URL components'),
    '#type'          => 'checkboxes',
    '#required'      => FALSE,
    '#options'       => $options,
    '#description'   => t('Click the item above to make it appear in the URL Syntax box'),
    '#attributes'    => array(
      'onclick' => '
        box = $(\'#edit-chado-stock-url-string\');
        if (this.value == \'reset\') {
          box.val(\'\');
        }
        else {
          box.val(box.val() + "/" + this.value);
        }
        this.checked = false;
      ',
    ),
  );

  $form['url']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Set Stock URLs'),
  );

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
 * @ingroup tripal_stock
 */
function tripal_stock_admin_validate($form, &$form_state) {
  global $user;  // we need access to the user info
  $job_args = array();

  switch ($form_state['values']['op']) {
    case  t('Set Controlled Vacabularies') :
      break;

    case t('Clean up orphaned stocks') :
      tripal_add_job('Cleanup orphaned stocks', 'tripal_stock',
         'tripal_stock_cleanup', $job_args, $user->uid);
      break;

    case t('Set Stock URLs') :
      tripal_add_job('Set Stock URLs', 'tripal_stock',
        'tripal_stock_set_urls', $job_args, $user->uid);
      break;
  }
}
