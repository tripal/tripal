<?php
/**
 * Provide form to store information of other Tripal sites
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_form($form, &$form_state) {

  $form = [];
  $values = key_exists('values', $form_state) ? $form_state['values'] : NULL;
  $tripal_site = $values ? $values['tripal_site'] : 0;

  $results =
    db_select('tripal_sites', 'ts')
      ->fields('ts')
      ->execute();
  $headers = ['Name', 'URL', 'Version', 'Description', 'Action'];
  $rows = [];
  while ($site = $results->fetchObject()) {
    $rows[] = [
      $site->name,
      $site->url,
      $site->version,
      $site->description,
      [
        'data' => l('Edit', '/admin/tripal/storage/ws/tripal_sites/edit/' . $site->id) . ' | ' .
          l('Remove', '/admin/tripal/storage/ws/tripal_sites/remove/' . $site->id),
        'nowrap' => TRUE,
      ],
    ];
  }
  if (count($rows) == 0) {
    $rows[] = ['No configured Tripal site.', ['colspan' => 5]];
  }

  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => [],
    'empty' => '',
  ];
  $output = theme_table($table);

  $form['instructions'] = [
    '#type' => 'item',
    '#markup' => t('The following is a list of remote Tripal sites with which
        this site can communicate. You may add and remove site as needed.'),
  ];
  $form['table'] = [
    '#markup' => $output,
  ];

  return $form;
}

/**
 * Provide form to add/edit a Tripal site
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_edit_form($form, &$form_state, $tripal_site_id = NULL) {
  $id = NULL;
  $name = '';
  $url = '';
  $version = '';
  $description = '';
  if ($tripal_site_id) {
    $site = db_select('tripal_sites', 'ts')
      ->fields('ts')
      ->condition('id', $tripal_site_id)
      ->execute()
      ->fetchObject();
    if (is_object($site)) {
      $id = $site->id;
      $name = $site->name;
      $url = $site->url;
      $version = $site->version;
      $description = $site->description;
    }
  }
  $form = [];
  $form['tripal_site_info']['id'] = [
    '#type' => 'hidden',
    '#value' => $id,
  ];
  $form['tripal_site_info']['name'] = [
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#description' => t('Full name of the Tripal site.'),
    '#default_value' => $name,
    '#required' => TRUE,
  ];
  $form['tripal_site_info']['url'] = [
    '#title' => t('URL'),
    '#type' => 'textfield',
    '#description' => t('The URL of the Tripal site, including the "http://" or "https://" followed by the address for the site\'s home page.'),
    '#default_value' => $url,
    '#required' => TRUE,
  ];
  $form['tripal_site_info']['version'] = [
    '#title' => t('Version'),
    '#type' => 'textfield',
    '#description' => t('Web services version used by the Tripal site (example: v0.1)'),
    '#default_value' => $version,
  ];
  $form['tripal_site_info']['description'] = [
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $description,
  ];

  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
    '#name' => 'save',
  ];
  $form['cancel_button'] = [
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#name' => 'cancel_button',
    '#limit_validation_errors' => [],
  ];

  return $form;
}

/**
 * Implements hook_validate()
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_edit_form_validate($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state)) {
    if ($form_state['clicked_button']['#name'] == 'cancel_button') {
      drupal_goto('/admin/tripal/storage/ws/tripal_sites');
    }
    // Make sure URL does not already exist
    else {
      if ($form_state['clicked_button']['#name'] == 'save') {
        $url = $form_state['values']['url'];
        $version = $form_state['values']['version'];
        $name = $form_state['values']['name'];
        $check_url = NULL;
        $check_name = NULL;
        // When updating a record
        if ($form_state['values']['id']) {
          $check_url =
            db_select('tripal_sites', 'ts')
              ->fields('ts', ['id'])
              ->condition('url', $url)
              ->condition('version', $version)
              ->condition('id', $form_state['values']['id'], '<>')
              ->execute()
              ->fetchField();
          $check_name =
            db_select('tripal_sites', 'ts')
              ->fields('ts', ['id'])
              ->condition('name', $name)
              ->condition('id', $form_state['values']['id'], '<>')
              ->execute()
              ->fetchField();
        }
        // When inserting a record
        else {
          $check_url =
            db_select('tripal_sites', 'ts')
              ->fields('ts', ['id'])
              ->condition('url', $url)
              ->condition('version', $version)
              ->execute()
              ->fetchField();
          $check_name =
            db_select('tripal_sites', 'ts')
              ->fields('ts', ['id'])
              ->condition('name', $name)
              ->execute()
              ->fetchField();
        }
        if ($check_url) {
          form_set_error('url', t('The URL and version is used by another site.'));
          form_set_error('version');
        }
        if ($check_name) {
          form_set_error('name', t('The name is used by another site.'));
        }
      }
    }
  }
}

/**
 * Implements hook_submit()
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_edit_form_submit($form, &$form_state) {
  $id = $form_state['values']['id'];
  $name = $form_state['values']['name'];
  $url = $form_state['values']['url'];
  $version = $form_state['values']['version'];
  $description = $form_state['values']['description'];
  // If there is an 'id' do an update, otherwise do an insert
  if ($id) {
    db_update('tripal_sites')
      ->fields([
        'name' => $name,
        'url' => $url,
        'version' => $version,
        'description' => $description,
      ])
      ->condition('id', $id)
      ->execute();
    drupal_set_message(t('Tripal site \'' . $name . '\' has been updated.'));
  }
  else {
    db_insert('tripal_sites')
      ->fields([
        'name' => $name,
        'url' => $url,
        'version' => $version,
        'description' => $description,
      ])
      ->execute();
    drupal_set_message(t('Tripal site \'' . $name . '\' has been added.'));
  }
  drupal_goto('/admin/tripal/storage/ws/tripal_sites');
}

/**
 * Implements hook_form()
 * Reset term used by semantic web
 *
 * @param $form
 * @param $form_state
 * @param $table
 * @param $column
 *
 * @return $form
 */
function tripal_ws_tripal_sites_remove_form($form, &$form_state, $id = NULL) {
  $name = '';
  $record_id = '';
  if ($id) {
    $site = db_select('tripal_sites', 'ts')
      ->fields('ts')
      ->condition('id', $id)
      ->execute()
      ->fetchObject();
    if (is_object($site)) {
      $record_id = $site->id;
      $name = $site->name;
    }
  }
  $form['confirmation'] = [
    '#markup' => 'Really remove the \'' . $name . '\' Tripal site? ',
  ];
  $form['tripal_site_id'] = [
    '#type' => 'value',
    '#value' => $record_id,
  ];
  $form['tripal_site_name'] = [
    '#type' => 'value',
    '#value' => $name,
  ];

  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Remove'),
    '#name' => 'remove',
  ];

  $form['cancel_button'] = [
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#name' => 'cancel_button',
    '#limit_validation_errors' => [],
  ];

  return $form;
}

/**
 * Implements hook_form_validate()
 *
 * Validate function for resetting the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_remove_form_validate($form, &$form_state) {
  if (array_key_exists('clicked_button', $form_state)) {
    if ($form_state['clicked_button']['#name'] == 'cancel_button') {
      drupal_goto('/admin/tripal/storage/ws/tripal_sites');
    }
    else {
      if (!$form_state['values']['tripal_site_id']) {
        drupal_set_message(t('Invalid Tripal site id'), 'error');
      }
    }
  }
}

/**
 * Implements hook_form_submit()
 *
 * Submit function for editing the semantic web term
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function tripal_ws_tripal_sites_remove_form_submit($form, &$form_state) {
  $id = $form_state['values']['tripal_site_id'];
  $name = $form_state['values']['tripal_site_name'];
  if ($id) {
    db_delete('tripal_sites')
      ->condition('id', $id)
      ->execute();
    drupal_set_message('The Tripal site \'' . $name . '\' has been removed.');
  }
  drupal_goto('/admin/tripal/storage/ws/tripal_sites');
}