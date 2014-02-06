<?php

/**
 * @file
 * Bulk Loader Administration (Miscellaneous)
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * Landing page for bulk loader admin. Ensures views are enabled and if not provides links to do so.
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_admin_jobs_listing() {
  $output = '';

  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Chado Data Loaders', 'admin/tripal/loaders');
  $breadcrumb[] = l('Bulk Loader', 'admin/tripal/loaders/bulk');
  drupal_set_breadcrumb($breadcrumb);

  // Add the view
  $jobs_view = views_embed_view('tripal_bulk_loading_jobs','default');
  $template_view = views_embed_view('tripal_bulk_loader_templates','default');
  if (isset($jobs_view) && isset($template_view)) {
    $output .= $jobs_view;
  }
  else {
    $output .= '<p>The Tripal Bulk Loader uses primarily views to provide an '
      . 'administrative interface. Currently one or more views needed for this '
      . 'administrative interface are disabled. <strong>Click each of the following links to '
      . 'enable the pertinent views</strong>:</p>';
    $output .= '<ul>';
    if (!isset($jobs_view)) {
      $output .= '<li>'.l('Tripal Bulk Loading Jobs', 'admin/tripal/loaders/bulk/views/jobs/enable').'</li>';
    }
    if (!isset($template_view)) {
      $output .= '<li>'.l('Tripal Bulk Loader Templates', 'admin/tripal/loaders/bulk/views/templates/enable').'</li>';
    }
    $output .= '</ul>';
  }

  return $output;
}

/**
 * @section
 * Configuration Form
 */

/**
 * A Configuration form for this module
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_configuration_form($form_state = NULL) {
  $form = array();

  $form['space'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enable/Disable Functionality'),
  );

  $form['space']['keep_track_inserted'] = array(
    '#type' => 'checkbox',
    '#title' => t('Keep track of inserted record IDs'),
    '#description' => t('This enables the ability to revert an entire loading job even if '
      .'it completed successfully. Furthermore, it displays the number of records '
      .'successfully inserted into each table.'),
    '#default_value' => variable_get('tripal_bulk_loader_keep_track_inserted', FALSE),
  );

  $form['speed'] = array(
    '#type' => 'fieldset',
    '#title' => t('Possible Speed Improvements'),
  );

  $form['speed']['prepare'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Prepared Statements'),
    '#description' => t('SQL Prepared Statements allow for SQL queries which will be run '
      .'many times to be parsed, rewritten and planned only once rather then every time '
      .'the query is run. In the case of the bulk loader, this ensures that planning only '
      .'occurs once for each "record" in your bulk loading template.'),
    '#default_value' => variable_get('tripal_bulk_loader_prepare', TRUE),
  );

  $form['speed']['disable_triggers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delay Constraint Checking during loading job.'),
    '#description' => t('This delays the constraint checking until the end of the
    loading proccess.'),
    '#default_value' => variable_get('tripal_bulk_loader_disable_triggers', TRUE),
  );

  $form['speed']['no_validate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Skip Validation at the Tripal Core API level'),
    '#description' => t('If an error is encountered, the Tripal core API will try
      to provide informative error messages. With this turned off, you will not benifit
      from these more informative error messages; however, your job will load faster
      since it doesn\'t have to do the additional checking before inserting.'),
    '#default_value' => variable_get('tripal_bulk_loader_skip_validation', FALSE),
  );

  $form['speed']['transactions'] = array(
    '#type' => 'radios',
    '#title' => t('Transaction Rollback when an error is encountered'),
    '#options' => array(
      'all' => t('Rollback the last constant set.'
        .'<div class="description"If you added more then one constant set then the
        successfully loaded constant sets will not be rolled back. However, once an error
        is encountered no further constant sets will be loaded either.</div>'),
      'row' => t('Only Rollback the last line of the input file.'
        .'<div class="description">This option may allow you to restart the job after
        fixing the error (manual intervention needed).</div>'),
      'none' => t('Do not use transactions<div class="description">This is not recommended.</div>')
    ),
    '#default_value' => variable_get('tripal_bulk_loader_transactions',   'row')
  );

  $form['speed']['lock'] = array(
    '#type' => 'radios',
    '#title' => t('Lock Type'),
    '#description' => t('The type of lock used by the bulk loading jobs. The lock is '
      .'acquired at the beginning of the job and kept till the end. A lock of the type '
      .'selected will be acquired for every table being inserted into.'),
    '#options' => array(
      'ROW EXCLUSIVE' => t('ROW EXCLUSIVE: The default lock type for insert queries.'),
      'EXCLUSIVE' => t('EXCLUSIVE: Only Select Queries can access the table.'),
      'ACCESS EXCLUSIVE' => t('ACCESS EXCLUSIVE: No other queries can access the table.'),
    ),
    '#default_value' => variable_get('tripal_bulk_loader_lock', 'ROW EXCLUSIVE'),
  );

  $form['submit1'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );

  return $form;
}

/**
 * A Configuration form for this module (Submit)
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_configuration_form_submit($form, $form_state) {

  variable_set('tripal_bulk_loader_prepare', $form_state['values']['prepare']);
  variable_set('tripal_bulk_loader_disable_triggers', $form_state['values']['disable_triggers']);
  variable_set('tripal_bulk_loader_skip_validation', $form_state['values']['no_validate']);
  variable_set('tripal_bulk_loader_transactions', $form_state['values']['transactions']);
  variable_set('tripal_bulk_loader_lock', $form_state['values']['lock']);

  variable_set('tripal_bulk_loader_keep_track_inserted', $form_state['values']['keep_track_inserted']);

}
