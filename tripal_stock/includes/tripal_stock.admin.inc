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
  $form['title'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stock Page Titles'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['title']['desc'] = array(
    '#markup' => t('Each synced stock must have a unique page title, however, stocks
      may have the same name if they are of different types or from different
      organisms.  Therefore, we must be sure that the page titles can uniquely
      identify the stock being viewed.  Select an option below that will
      uniquely identify all stocks on your site.'),
  );
  $options = array(
    'stock_unique_name'  => 'Only stock unique name',
    'stock_name'         => 'Only stock name',
    'unique_constraint'  => 'Includes stock name, uniquename, type and species',
  );
  $form['title']['chado_stock_title'] = array(
    '#title'         => t('Stock Page Titles'),
    '#type'          => 'radios',
    '#description'   => t('Choose a title type  from the list above that is
      guaranteed to be unique for all stocks If in doubt it is safest to choose
      the last option as that guarantees uniqueness. Click the
      \'Save Configuration\' button at the bottom to save your selection.'),
    '#required'      => FALSE,
    '#options'       => $options,
    '#default_value' => variable_get('chado_stock_title', 'unique_constraint'),
  );
  

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

  variable_set('chado_stock_types_cv', $form_state['values']['stock_types_cv']);
  variable_set('chado_stock_prop_types_cv', $form_state['values']['stock_prop_types_cv']);
  variable_set('chado_stock_relationship_cv', $form_state['values']['stock_relationship_cv']);
  variable_set('chado_stock_url_string', $form_state['values']['chado_stock_url_string']);

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
