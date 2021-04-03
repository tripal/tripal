<?php
/**
 * @file
 * Provides functions for hooking into bulk loader functionality.
 *
 * @ingroup tripal_bulk_loader
 */

/**
 * @defgroup tripal_bulk_loader_api Bulk Loader
 * @ingroup tripal_api
 * @{
 * All functions in this file provide an API to administrative management of
 * bulk loader templates.
 * @}
 */

/**
 * Validates an $options array for insert or update of a bulk loader record.
 *
 * @param $val_type
 *   The type of validation. Can be either 'insert' or 'update'.
 * @param $options
 *   An array of key/value pairs containing the following keys:
 *     template_name: The name of the template.
 *     template_array: The JSON array representing the template.
 *     Optional:
 *      strict: If set then only JSON formatted templates are allowed.
 * @param $errors
 *   An empty array where validation error messages will be set. The keys
 *   of the array will be name of the field from the options array and the
 *   value is the error message.
 * @param $warnings
 *   An empty array where validation warning messagges will be set. The
 *   warnings should not stop an insert or an update but should be provided
 *   to the user as information by a drupal_set_message() if appropriate. The
 *   keys of the array will be name of the field from the options array and the
 *   value is the error message.
 *
 * @return
 *   If validation fails then FALSE is returned.  Any options that do not pass
 *   validation checks will be added in the $errors array with the key being
 *   the option and the value being the error message.  If validation
 *   is successful then TRUE is returned.
 *
 * @ingroup tripal_bulk_loader_api
 */
function tripal_validate_bulk_loader_template($val_type, &$options, &$errors, &$warnings = []) {
  $template_array = trim($options['template_array']);
  $template_name = trim($options['template_name']);
  $strict = array_key_exists('strict', $options) ? $options['strict'] : FALSE;

  // Make sure the template array is one of the supported types
  // DEPRECATED: A stringified version of the array (causes security issues)
  if (preg_match('/^array/', $template_array)) {
    if ($strict) {
      $errors['template_array'] = t('Invalid template array. Please provide
        a JSON formatted array');
      return FALSE;
    }
    else {
      $warnings['template_array'] = t('Please note that import of
        bulk loader templates as PHP arrays as a stringified array is deprecated
        and will be removed in future versions of Tripal. Export and import
        format will be JSON.');
    }
  }
  // DEPRECATED: A serialized PHP array
  elseif (preg_match('/^a:/', $template_array)) {
    if ($strict) {
      $errors['template_array'] = t('Invalid template array. Please provide
        a JSON formatted array');
      return FALSE;
    }
    else {
      $warnings['template_array'] = t('Please note that import of
        bulk loader templates as PHP serialized arrays is deprecated and will
        be removed in future versions of Tripal. Export and import format will
        be JSON.');
    }
  }
  // JSON FORMAT
  elseif (json_decode($template_array)) {
    // This is correct!
  }
  else {
    $errors['template_array'] = t('The template array must be in
      JSON format (although PHP strigified arrays and PHP serialized
      arrays are temporarily supported for backwards compatibility).');
    return FALSE;
  }

  // Make sure the template name is unique
  $name_exists = db_select('tripal_bulk_loader_template', 'tblt')
    ->fields('tblt', ['template_id'])
    ->condition('name', $template_name)
    ->execute()
    ->fetchField();
  if ($name_exists) {
    $errors['template_name'] = t('The template name already exists. Please
      choose another name.');
    return FALSE;
  }
  return TRUE;
}

/**
 * Inserts a bulk loader template record.
 *
 * This function validates the options passed prior to insertion of the record.
 *
 * @param $options
 *   An array of key/value pairs containing the following keys:
 *     'template_name':   The name of the template.
 *     'template_array':  The JSON array representing the template.
 *   Optional:
 *     'strict':          If set then only JSON formatted templates are allowed.
 * @param $errors
 *   An empty array where validation error messages will be set. The keys
 *   of the array will be name of the field from the options array and the
 *   value is the error message.
 * @param $warnings
 *   An empty array where validation warning messagges will be set. The
 *   warnings should not stop an insert or an update but should be provided
 *   to the user as information by a drupal_set_message() if appropriate. The
 *   keys of the array will be name of the field from the options array and the
 *   value is the error message.
 *
 * @return
 *   TRUE for success and FALSE for failure.
 *
 * @ingroup tripal_bulk_loader_api
 */
function tripal_insert_bulk_loader_template($options, &$errors, &$warnings) {

  $success = tripal_validate_bulk_loader_template('insert', $options, $errors, $warnings);
  if (!$success) {
    foreach ($errors as $field => $message) {
      tripal_report_error('tripal_bulkldr', TRIPAL_ERROR, $message);
    }
    return FALSE;
  }

  // Insert the bulk loader template.
  $template_array = trim($options['template_array']);
  $template_name = trim($options['template_name']);

  // Previous version of Tripal would export the template as a PHP array.
  // This has security implications and is deprecated. This support should
  // be reomved in future versions of Tripal, but to support transfers of
  // templates between v1.1 and v2.x sites we support it.
  if (preg_match('/^array/', $template_array)) {
    $tarray = [];
    eval("\$tarray = $template_array;");
    $template_array = serialize($tarray);
  }
  // For a brief period, the bulk loader templates were exported as a PHP
  // serialized array. We have moved to exporting in JSON as JSON is more
  // user friendly. But we must support the serialized PHP array for
  // backwards compatibility of v2.0-rc1 sites and v2.x sites.
  elseif (preg_match('/^a:/', $template_array)) {
    // do nothing it's in PHP serialized format
  }
  // The typical format is JSON
  elseif (json_decode($template_array)) {
    $template_array = serialize(json_decode($template_array, TRUE));
  }
  else {
    $errors['template_array'] = t('Unrecognized array type.');
    return FALSE;
  }

  $record = [
    'name' => $template_name,
    'template_array' => $template_array,
    'created' => time(),
    'changed' => time(),
  ];
  if (!drupal_write_record('tripal_bulk_loader_template', $record)) {
    return FALSE;
  }
  return TRUE;
}

/**
 * Meant to be called from a form_validate function to ensure a newly added bulk
 * loader record name is unique and not empty.
 *
 * @param $new_record_name
 *   The record name to check for uniqueness
 * @param $template_id
 *   The template_id of the template to add the record to
 * @param $template_array
 *   The array describing the template. Optional -will be loaded using
 *   template_id if not provided
 * @param $current_priority
 *   The priority of the already existing record -checks that the name only
 *   occurs on this particular record
 *
 * @return
 *   TRUE if the record name is not empty and not already in the template_array;
 *   FALSE otherwise
 *
 * @ingroup tripal_bulk_loader_api
 */
function tripal_is_bulk_loader_record_name_unique($new_record_name, $template_id, $template_array = NULL, $current_priority = NULL) {

  // get the template array if it's not supplied
  if (empty($template_array)) {
    $template = db_query("SELECT * FROM {tripal_bulk_loader_template} WHERE template_id=:template", [':template' => $template_id])->fetchObject();
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
    drupal_set_message("Unable to delete record with a priority of $priority since the template was not supplied", 'error');
    return FALSE;
  }

  $new_template_array = [];
  $i = 0;
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
    drupal_set_message("Unable to delete record with a priority of $priority since the template was not supplied", 'error');
    return FALSE;
  }

  // Re-order the remaining fields of the same record to ensure that the indicies are
  // 0 to size and. If this is not done, weird behaviour may result
  $new_template_array = $template_array;
  $new_template_array[$priority]['fields'] = [];
  $i = 0;
  foreach ($template_array[$priority]['fields'] as $field_index => $field_details) {
    if ($field_index != $delete_field_index) {
      $new_template_array[$priority]['fields'][$i] = $field_details;
      $i++;
    }
  }

  // If this field was the only one in the current record, also delete the record
  if (empty($new_template_array[$priority]['fields'])) {
    $new_template_array = tripal_delete_bulk_loader_record($priority, $new_template_array);
  }

  return $new_template_array;
}
