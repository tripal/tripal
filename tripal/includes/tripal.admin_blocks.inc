<?php

/**
 *
 * Import the field from the admin notification table on the dashboard.
 *
 * @param $field_or_instance
 *   The name of the field to be imported.
 *  * @param $bundle_id
 *   The ID of the bundle associated with that field.
 * @param $field_name_note
 * @param $module
 *
 *
 * @return bool
 */
function tripal_admin_notification_import_field($field_name_note, $bundle_id, $module, $field_or_instance) {
  // Get the bundle object.
  $bundle = tripal_load_bundle_entity(['name' => $bundle_id]);
  if (!$bundle) {
    tripal_report_error('tripal', TRIPAL_ERROR, "Unrecognized bundle name '%bundle'.",
      ['%bundle' => $bundle_id]);
    drupal_goto("admin/dashboard");
    return FALSE;
  }

  if ($field_or_instance == 'field') {
    $function = $module . '_bundle_fields_info';
    $info = $function('TripalEntity', $bundle);
    foreach ($info as $field_name => $details) {
      $field = field_info_field($field_name);
      if ($details['field_name'] == $field_name_note) {
        // Create the field.
        $instance = field_create_field($details);
        drupal_set_message(t("Created field: %field", ['%field' => $field['label']]));

        if (!$instance) {
          tripal_set_message(t("Could not create new field: %field.",
            ['%field' => $field_name_note]), TRIPAL_ERROR);
        }
      }
    }
  }
  else {
    if ($field_or_instance == 'instance') {
      $function = $module . '_bundle_instances_info';
      $info = $function('TripalEntity', $bundle);
      foreach ($info as $field_name => $details) {
        if ($details['field_name'] == $field_name_note) {
          // Create the field instance.
          $instance = field_create_instance($details);
          drupal_set_message(t("Created field: %field", ['%field' => $info[$field_name]['label']]));

          if (!$instance) {
            tripal_set_message(t("Could not create new field: %field.",
              ['%field' => $field_name_note]), TRIPAL_ERROR);
          }
        }
      }
    }
  }

  $submitter_id = $field_name_note . '-' . $bundle_id . '-' . $module;
  if ($instance) {
    // Delete the notification table entry.
    db_delete('tripal_admin_notfications')
      ->condition('submitter_id', $submitter_id, '=')
      ->execute();
  }
  else {
    drupal_set_message(t("There was a problem creating: %field", ['%field' => $info[$field_name]['label']]));
  }

  drupal_goto("admin/dashboard");
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
    ->fields([
      'enabled' => 0,
    ])
    ->condition('note_id', $note_id, '=')
    ->execute();
  if ($success) {
    drupal_set_message("That notification has been dismissed and will no longer appear.");
  }
  else {
    drupal_set_message("Could not dismiss notification.", 'error');
  }
  drupal_goto("admin/dashboard");
}
