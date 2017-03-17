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
  $results = db_select('tripal_admin_notfications', 'tan')
    ->fields('tan')
    ->condition('enabled', 1, '=')
    ->execute()->fetchAll();
  $rows = array();

  foreach($results as $result){
    $data['operation'] = l(t('Import'), 'admin/import/field/' . $result->title . '/' .$result->bundle_id . '/' . $result->module);
    $data['operation'] .= "|";
    $data['operation'] .= l(t('Dismiss Notification'), 'admin/disable/notification/' . $result->note_id);
    $rows[] = array(
      'Title' => $result->title,
      'Details' => $result->info,
      'Operations' => $data['operation'],
      );
  }
  //Number of records shown in per page
  $per_page = 20;
  $current_page = pager_default_initialize(count($rows), $per_page);
  $chunks = array_chunk($rows, $per_page, TRUE);
  // Prepare table header
  $header = array(t('Title'), t('Details'), t('Operations'));
  // Output of table with the paging
  $output = theme('table',
    array(
      "header" => $header,
      "rows" => $chunks[$current_page],
      "attributes" => array(),
      "sticky" => TRUE,
      "caption" => "",
      "colgroups" => array(),
      "empty" => t("No Fields for Import")
    )
  );
  //return pager with limited number of records.
  return $output .= theme('pager', array('quantity', count($rows)));
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
          array('%field' =>  $details['field_name'])), TRIPAL_ERROR);
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
  if($instance){
    // Delete the notification table entry.
    db_delete('tripal_admin_notfications')
      ->condition('bundle_id', $bundle_id)
      ->condition('title', $field_name_note)
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
