<?php

/**
 * @file
 * Bulk Loader Administration (Miscellaneous)
 */

/**
 * Provides a description page and quick links for the bulk loader
 */
function tripal_bulk_loader_admin_template() {
  $output = '';

  $output .= '<br /><h3>Quick Links:</h3>';
  $output .= '<ul>'
    . '<li>'
      . t('<a href="@link">Configure settings</a>',
        array('@link' => url('admin/tripal/tripal_bulk_loader_template/configure')))
      . '</li>'
    . '<li>'
      . t('List <a href="@link">Bulk Loader Jobs</a>',
      array('@link' => url('admin/tripal/tripal_bulk_loader_template/jobs')))
      . '</li>'
    . '<li>'
      . t('List <a href="@link">Manage Templates</a>',
      array('@link' => url('admin/tripal/tripal_bulk_loader_template/manage_templates')))
      . '</li>'
      . '<ul>'
      . '<li>'
        . t('<a href="@create">Create</a> a new template',
        array('@create' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/create')))
        . '</li>'
      . '<li>'
        . t('<a href="@edit">Edit</a> an existing template',
        array('@edit' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/edit')))
        . '</li>'
      . '<li>'
        . t('<a href="@delete">Delete</a> an existing template',
        array('@delete' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/delete')))
        . '</li>'
      . '<li>'
        . t('<a href="@import">Import</a> a new template',
        array('@import' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/import')))
        . '</li>'
      . '<li>'
        . t('<a href="@export">Export</a> an existing template',
        array('@export' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/export')))
        . '</li>'
      . '</ul>'
    . '</ul>';

  $output .= '<h3>Module Description:</h3>';
  $output .= '<p>This module provides the ability to create loading templates for any tab-delimited '
    . 'data file allowing it to be loaded into chado. The Loading Templates are a direct mapping '
    . 'between the columns in your file and the columns in chado tables. As such to use this tool '
    . 'you need to be very familar with the chado schema -See '
    . l(t('Chado -Getting Started'), 'http://gmod.org/wiki/Chado_-_Getting_Started')
    . '. The ability to add constants and specify foreign key contraints is also provided '
    . 'in order for the loader to fill chado columns which may be required but are not specified '
    . 'in your input file.</p>';
  $output .= '<br />';

  $output .= '<h3>Setup Instructions</h3>';
  $output .= '<p>After intallation of the bulk loader module, the following tasks should be performed:</p>';
  $output .= '<ol>';
  $output .= '<li><b>Install Theme:</b> In order for Bulk Loading pages to be displayed correctly, '
    .'the contents of the Tripal Bulk Loader theme directory ([drupal root]/sites/all/modules/tripal/tripal_bulk_loader/theme) '
    .'should be moved to the base directory of the Tripal theme ([drupal root]/sites/all/themes/tripal). '
    .'Finally the drupal cache should be cleared for the new theme to take effect -navigate to admin/settings/performance '
    .'and click the Clear Cached Data button.</li>';
  $output .= '</ol>';

  return $output;
}

/**
 * Provides a description page and quick links for template management
 */
function tripal_bulk_loader_admin_manage_templates() {
  $output = '';

  $output .= '<br /><h3>Quick Links:</h3>';
  $output .= '<ul>'
    . '<li>'
      . t('<a href="@create">Create</a> a new template',
      array('@create' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/create')))
      . '</li>'
    . '<li>'
      . t('<a href="@edit">Edit</a> an existing template',
      array('@edit' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/edit')))
      . '</li>'
    . '<li>'
      . t('<a href="@delete">Delete</a> an existing template',
      array('@delete' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/delete')))
      . '</li>'
    . '<li>'
      . t('<a href="@import">Import</a> a new template',
      array('@import' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/import')))
      . '</li>'
    . '<li>'
      . t('<a href="@export">Export</a> an existing template',
      array('@export' => url('admin/tripal/tripal_bulk_loader_template/manage_templates/export')))
      . '</li>'
    . '</ul>';

  $output .= '<p>' . t('Templates, as the term is used for this module, refer to plans
  describing how the columns in the data file supplied to a bulk loading job map to tables
  and fields in chado. Templates are created independently of bulk loading jobs so that
  they can be re-used. Thus you only need one template to load any number of files of the
  same format.') . '</p>';



  return $output;
}

/**
 * Provides a listing of bulk loader jobs and links for administration
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
    '');
  $rows = array();
  $resource = pager_query("SELECT n.*, t.name as template_name
    FROM {tripal_bulk_loader} n
    LEFT JOIN {tripal_bulk_loader_template} t ON cast(n.template_id as integer)=t.template_id"
    . tablesort_sql($header),
    $num_jobs_per_page);
  while ($n = db_fetch_object($resource)) {
    $rows[] = array(
      l($n->job_id, 'admin/tripal/tripal_jobs/view/' . $n->job_id),
      l($n->loader_name, 'node/' . $n->nid),
      l($n->template_name, 'admin/tripal/tripal_bulk_loader_template/manage_templates/edit', array('query' => 'template_id=' . $n->template_id)),
      $n->job_status,
      l('View', 'node/' . $n->nid) . ' | ' .  l('Edit', 'node/' . $n->nid . '/edit')
    );
  }
  $output .= theme_table($header, $rows);

  $output .= theme('pager');

  return $output;
}

/**
 * @section
 * Configuration Form
 */

/**
 * A Configuration form for this module
 */
function tripal_bulk_loader_configuration_form($form_state = NULL) {
  $form = array();

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
 */
function tripal_bulk_loader_configuration_form_submit($form, $form_state) {

  variable_set('tripal_bulk_loader_prepare', $form_state['values']['prepare']);
  variable_set('tripal_bulk_loader_disable_triggers', $form_state['values']['disable_triggers']);
  variable_set('tripal_bulk_loader_skip_validation', $form_state['values']['no_validate']);
  variable_set('tripal_bulk_loader_transactions', $form_state['values']['transactions']);
  variable_set('tripal_bulk_loader_lock', $form_state['values']['lock']);

}
