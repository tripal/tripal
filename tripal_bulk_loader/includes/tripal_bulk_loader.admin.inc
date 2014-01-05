<?php

/**
 * @file
 * Bulk Loader Administration (Miscellaneous)
 */

function tripal_bulk_loader_admin_jobs_listing() {
  $output = '';

  // Add Tripal Admin View CSS
  $tripal_admin_view_css = drupal_get_path('module', 'tripal_views') . '/theme/tripal_views_admin_views.css';
  drupal_add_css($tripal_admin_view_css);

  // Add the view
  $view = views_get_view('tripal_bulk_loading_jobs');
  if (isset($view)) {
    $view->set_display('default');
    $output .= '<h2>' . $view->get_title() . '</h2>';
    $output .= $view->preview('page');
  }
  else {
    $output .= '<p>The "Tripal Bulk Loading Jobs" View should be displayed here. If you '
      . 'have disabled, please re-enable it in order to administrate this module.</p>';
  }

  return $output;
}

function tripal_bulk_loader_admin_template_listing() {
  $output = '';

  // Add Tripal Admin View CSS
  $tripal_admin_view_css = drupal_get_path('module', 'tripal_views') . '/theme/tripal_views_admin_views.css';
  drupal_add_css($tripal_admin_view_css);

  $view = views_get_view('tripal_bulk_loader_templates');
  if (isset($view)) {
    $view->set_display('default');
    $output .= '<h2>' . $view->get_title() . '</h2>';
    $output .= $view->preview('page');
  }
  else {
    $output .= '<p>The "Tripal Bulk Loader Templates" View should be displayed here. If you '
      . 'have disabled, please re-enable it in order to administrate this module.</p>';
  }

  return $output;
}

/**
 * Provides a description page and quick links for template management
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_admin_manage_templates() {
  $output = '';
  $output .= '<p>' . t('Templates, as the term is used for this module, refer to plans
  describing how the columns in the data file supplied to a bulk loading job map to tables
  and fields in chado. Templates are created independently of bulk loading jobs so that
  they can be re-used. Thus you only need one template to load any number of files of the
  same format.') . '</p>';

  return $output;
}

/**
 * Provides a listing of bulk loader jobs and links for administration
 *
 * @ingroup tripal_bulk_loader
 */
function tripal_bulk_loader_admin_jobs() {
  $output = '';
  $num_jobs_per_page = 50;

  $output .= '<p>' . t('Jobs are not automatically submitted to the tripal jobs management
  system when they are first created. Any jobs listed below with a status of "Initialized"
  will not have a Job ID until you go to the bulk loader page and submit the job.') . '</p>';

  $header = array(
    array('data' => 'Job ID', 'field' => 'job_id', 'sort' => 'DESC'),
    array('data' => 'Name', 'field' => 'loader_name'),
    array('data' =>  'Template', 'field' => 'template_name'),
    array('data' =>  'Status', 'field' => 'job_status'),
    array('data' => 'Progress', 'field' => 'progress'),
    '');
  $rows = array();
  $query = db_select('tripal_bulk_loader', 'n')->extend('TableSort');
  $query->join('tripal_bulk_loader_template', 't', 'cast(n.template_id as integer) = t.template_id');
  $query->join('tripal_jobs', 'j', 'n.job_id = j.job_id');
  $query->fields('n')
    ->fields('t', array('name'))
    ->fields('j', array('progress'))
    ->orderByHeader($header);
  $result = $query->execute();
  /**
  $resource = pager_query("SELECT n.*, t.name as template_name, j.progress
    FROM {tripal_bulk_loader} n
    LEFT JOIN {tripal_bulk_loader_template} t ON cast(n.template_id as integer) = t.template_id
    LEFT JOIN {tripal_jobs} j ON n.job_id = j.job_id"
    . tablesort_sql($header),
    $num_jobs_per_page);
    */
  foreach ($result as $n) {
    $rows[] = array(
      l($n->job_id, 'admin/tripal/tripal_jobs/view/' . $n->job_id),
      l($n->loader_name, 'node/' . $n->nid),
      l($n->template_name, 'admin/tripal/loaders/bulk/template/'.$n->template_id.'/edit'),
      $n->job_status,
      ($n->progress) ? $n->progress . '%' : '',
      l('View', 'node/' . $n->nid) . ' | ' .  l('Edit', 'node/' . $n->nid . '/edit')
    );
  }
  $output .= theme_table(array('header' => $header, 'rows' => $rows));

  $output .= theme('pager');

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
