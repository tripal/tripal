<?php
/**
 * @file
 * Provides functions for hooking into bulk loader functionality.
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * @defgroup tripal_bulk_loader_api Tripal Bulk Loader Module API
 * @ingroup tripal_api
 * @{
 * All functions in this file provide an API to administrative management of bulk loader templates
 * @}
 */

/**
 * Meant to be called from a form_validate function to ensure a newly added bulk loader record
 * name is unique and not empty.
 *
 * @param $new_record_name
 *   The record name to check for uniqueness
 * @param $template_id
 *   The template_id of the template to add the record to
 * @param $template_array
 *   The array describing the template. Optional -will be loaded using template_id if not provided
 * @param $current_priority
 *   The priority of the already existing record -checks that the name only occurs on this particular record
 *
 * @return
 *   TRUE if the record name is not empty and not already in the template_array; FALSE otherwise
 *
 * @ingroup tripal_bulk_loader_api
 */
function tripal_is_bulk_loader_record_name_unique($new_record_name, $template_id, $template_array = NULL, $current_priority = NULL) {

  // get the template array if it's not supplied
  if (empty($template_array)) {
    $template = db_query("SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template", array(':template' => $template_id))->fetchObject();
    $template_array = unserialize($template->template_array);
    if (!is_array($template_array)) {
      return TRUE;
    }
  }

  // Check that the new record name is not empty
  if (empty($new_record_name)) {
    return FALSE;
  }

  // Check the new record name is unique
  foreach ($template_array as $priority => $t) {
    if (strcmp($t['record_id'], $new_record_name) == 0) {
      if (($priority != $current_priority) AND ($current_priority !== NULL)) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
 * An API function to delete a record from a template array
 *
 * @param $delete_priority
 *   The priority of the record to be deleted
 * @param $template_array
 *   The array describing the template
 *
 * @return
 *   The modified template array
 *
 * @ingroup tripal_bulk_loader_api
 */
function tripal_delete_bulk_loader_record($delete_priority, $template_array) {

  if (empty($template_array)) {
    drupal_set_message("Unable to delete record with a priority of $priority since the template was not supplied",'error');
    return FALSE;
  }

  $new_template_array = array();
  $i=0;
  foreach ($template_array as $priority => $record) {
    if ($priority != $delete_priority) {
      $new_template_array[$i] = $record;
      $i++;
    }
  }

  return $new_template_array;
}

/**
 * An API function to delete a field from a template array
 *
 * @param $priority
 *   The priority of the record containing the field
 * @param $delete_field_index
 *   The index of the field to be deleted
 * @param $template_array
 *   The array describing the template
 *
 * @return
 *   The modified template array
 *
 * @ingroup tripal_bulk_loader_api
 */
function tripal_delete_bulk_loader_field($priority, $delete_field_index, $template_array) {

  if (empty($template_array)) {
    drupal_set_message("Unable to delete record with a priority of $priority since the template was not supplied",'error');
    return FALSE;
  }

  // Re-order the remaining fields of the same record to ensure that the indicies are
  // 0 to size and. If this is not done, weird behaviour may result
  $new_template_array = $template_array;
  $new_template_array[$priority]['fields'] = array();
  $i=0;
  foreach ($template_array[$priority]['fields'] as $field_index => $field_details) {
    if ($field_index != $delete_field_index) {
      $new_template_array[$priority]['fields'][$i] = $field_details;
      $i++;
    }
  }

  // If this field was the only one in the current record, also delete the record
  if (empty($new_template_array[$priority]['fields'])) {
    $new_template_array = tripal_bulk_loader_delete_record($priority, $new_template_array);
  }

  return $new_template_array;
}
