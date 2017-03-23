<?php

/**
 * Create the admin page for the dashboard.
 *
 */
function tripal_admin_usage_page() {
  // set the breadcrumb
  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  drupal_set_breadcrumb($breadcrumb);
  tripal_add_d3js();
  //drupal_add_js(drupal_get_path ('module', 'tripal') . '/theme/js/tripal_galaxy.dashboard.js');
  drupal_add_css(drupal_get_path ('module', 'tripal') . '/theme/css/tripal.dashboard.css');

  // Prepare table header
  $header = array(
    'title' => array('data' => t('Title')),
    'details' => array('data' => t('Details')),
    'type' => array('data' => t('Type'), 'field' => 'tan.type'),
    'operations' => array('date' => t('Operations'))
  );

  $query = db_select('tripal_admin_notfications', 'tan')
    ->extend('TableSort');

  $results = $query->fields('tan')
    ->condition('enabled', 1, '=')
    ->orderByHeader($header)
    ->execute()->fetchAll();
  $rows = array();

  foreach($results as $result){
    $data['operation'] = ' | ';
    $data['operation'] .= l(t('Dismiss Notification'), 'admin/disable/notification/' . $result->note_id);

    $actions = unserialize($result->actions);
    foreach($actions as $action){
      $label = key($actions);
      $link = $action;
    }

    $rows[] = array(
      'Title' => $result->title,
      'Details' => $result->details,
      'Type' => $result->type,
      'Operations' => l(t($label), $link) . $data['operation'],
      );
  }
  if(!empty($rows)) {
    //Number of records shown in per page
    $per_page = 20;
    $current_page = pager_default_initialize(count($rows), $per_page);
    $chunks = array_chunk($rows, $per_page, TRUE);

    // Output of table with the paging
    $output = '<h2>Notifications</h2>';
    $output .= theme('table',
      array(
        "header" => $header,
        "rows" => $chunks[ $current_page ],
        "attributes" => array(),
        "sticky" => TRUE,
        "caption" => "",
        "colgroups" => array(),
        "empty" => t("No notifications.")
      )
    );
    //return pager with limited number of records.
    return $output .= theme('pager', array('quantity', count($rows)));
  }
  else {
    $output = 'There are no notifications at this time.';
    return $output;
  }
}

/**
 * Import the field from the admin notification table on
 * the dashboard.
 *
 * @param $field_name
 *   The name of the field to be imported.
 *
 *  * @param $bundle_id
 *   The ID of the bundle associated with that field.
 *
 */
function tripal_admin_notification_import_field($field_name_note, $bundle_id, $module) {
  // Get the bundle object.
  $bundle = tripal_load_bundle_entity(array('name' => $bundle_id));
  if (!$bundle) {
    tripal_report_error('tripal', TRIPAL_ERROR, "Unrecognized bundle name '%bundle'.",
      array('%bundle' => $bundle_id));
    return FALSE;
  }

  $function = $module . '_bundle_create_fields';
  $info = $function('TripalEntity', $bundle);
  foreach ($info as $field_name => $details) {
    if($details['field_name'] == $field_name_note) {
      // Create the field.
      $field = field_create_field($details);
      if (!$field) {
        tripal_set_message(t("Could not create new field: %field.",
          array('%field' =>  $field_name_note)), TRIPAL_ERROR);
      }
    }
  }
  $function = $module . '_bundle_create_instances';
  $info = $function('TripalEntity', $bundle);
  foreach ($info as $field_name => $details) {
    if($details['field_name'] == $field_name_note) {
      // Create the field instance.
      $instance = field_create_instance($details);
      drupal_set_message(t("Created field: %field", array('%field' => $info[ $field_name ]['label'])));
    }
  }
  $submitter_id = $field_name_note . '-' . $bundle_id . '-' . $module;
  if($instance){
    // Delete the notification table entry.
    db_delete('tripal_admin_notfications')
      ->condition('submitter_id', $submitter_id, '=')
      ->execute();
  }
  else{
    drupal_set_message(t("There was a problem creating: %field", array('%field' => $info[ $field_name ]['label'])));

  }
  drupal_goto("admin/tripal/dashboard");
}

/**
 * Disable the notification of the field on the dashboard.
 *
 * @param $note_id
 *   The ID of the note in the tripal_admin_notifications table
 * that will be dismissed.
 */
function tripal_disable_admin_notification($note_id) {
  $success = db_update('tripal_admin_notfications')
          ->fields(array(
            'enabled' => 0,
          ))
          ->condition('note_id', $note_id, '=')
          ->execute();
  if($success){
    drupal_set_message("That notification has been dismissed and will no longer appear.");
  }
  else {
    drupal_set_message("Could not dismiss notification.", 'error');
  }
  drupal_goto("admin/tripal/dashboard");
}
