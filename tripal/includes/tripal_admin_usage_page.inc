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
  drupal_add_library('system', 'drupal.collapse');
  $output = '<h2>Tripal Administrative Notifications and Info</h2>';
  return $output;
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
function tripal_admin_notification_import_field($field_name_note, $bundle_id, $module, $field_or_instance) {
  // Get the bundle object.
  $bundle = tripal_load_bundle_entity(array('name' => $bundle_id));
  if (!$bundle) {
    tripal_report_error('tripal', TRIPAL_ERROR, "Unrecognized bundle name '%bundle'.",
      array('%bundle' => $bundle_id));
    drupal_goto("admin/tripal/dashboard");
    return FALSE;
  }

  if($field_or_instance == 'field'){
    $function = $module . '_bundle_create_fields';
    $info = $function('TripalEntity', $bundle);
    foreach ($info as $field_name => $details) {
      if($details['field_name'] == $field_name_note) {
        // Create the field.
        $instance = field_create_field($details);
        drupal_set_message(t("Created field: %field", array('%field' => $info[ $field_name ]['label'])));

        if (!$instance) {
          tripal_set_message(t("Could not create new field: %field.",
            array('%field' =>  $field_name_note)), TRIPAL_ERROR);
        }
      }
    }
  }
  else if($field_or_instance == 'instance'){
    $function = $module . '_bundle_create_instances';
    $info = $function('TripalEntity', $bundle);
    foreach ($info as $field_name => $details) {
      if($details['field_name'] == $field_name_note) {
        // Create the field instance.
        $instance = field_create_instance($details);
        drupal_set_message(t("Created field: %field", array('%field' => $info[ $field_name ]['label'])));

        if (!$instance) {
          tripal_set_message(t("Could not create new field: %field.",
            array('%field' =>  $field_name_note)), TRIPAL_ERROR);
        }
      }
    }
  }

  $submitter_id = $field_name_note . '-' . $bundle_id . '-' . $module;
  if($instance){
    // Delete the notification table entry.
    db_delete('tripal_admin_notfications')
      ->condition('submitter_id', $submitter_id, '=')
      ->execute();
  }
  else {
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
