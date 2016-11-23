<?php

/**
 * Adds a new field type to Tripal.
 *
 * Before a new field can be added to an entity, Drupal must know about the
 * field types.  This function is used to add new Tripal field types.
 *
 * @param $term
 *   The term that this field is associated with.  This should be an array as
 *   returned by tripal_get_term_details()
 * @param $options
 */
function tripal_add_field_type($term, $options) {
  // TODO: add checks to make sure we have properly formatted options.

  // Add the term in case it doesn't already exist
  tripal_add_term($term);

  $field_type = $term['vocab']['short_name'] . '__' . $term['accession'];

  // Make sure that the field type doesn't already exist
  $type_id = db_select('tripal_field_types', 'TFT')
    ->fields('TFT', array('term'))
    ->condition('term', $field_type)
    ->execute()
    ->fetchField();
  if ($type_id) {
    return FALSE;
  }
  $type_id = db_insert('tripal_field_types')
    ->fields(array(
      'term' => $field_type,
      'label' => $options['label'],
      'description' => $options['description'],
      'settings' => serialize($options['settings']),
      'instance_settings' => serialize($options['instance_settings']),
      'default_widget' => $options['default_widget'],
      'default_formatter' => $options['default_formatter'],
      'storage' => serialize($options['storage']),
    ))
    ->execute();
}
/**
 *
 */
function tripal_get_field_type_info() {
  // Make sure that the field type doesn't already exist
  $records = db_select('tripal_field_types', 'TFT')
    ->fields('TFT')
    ->execute();
  $info = array();
  while($record = $records->fetchObject()) {
    $info[$record->term] = array(
      'label' => $record->label,
      'description' => $record->description,
      'settings' => unserialize($record->settings),
      'instance_settings' => unserialize($record->instance_settings),
      'default_widget' => $record->default_widget,
      'default_formatter' => $record->default_formatter,
      'storage' => unserialize($record->storage),
      // Tripal fields should only be added to entities programmatically
      // and not through the UI.
      'no_ui' => TRUE,
    );
  }
  return $info;
}

/**
 * Executes a TripalFieldQuery using the provided conditions.
 *
 * This hook is called to find the entities having certain field
 * conditions and sort them in the given field order.
 *
 * @param $conditions
 *   An array of filter representing the conditions to be applied to the query.
 *   Each filter is an associative array whose keys include the following:
 *   - field: an array representing the field identical to the output of the
 *     field_info_field() function.
 *   - filter: the name of the field on which the filter should be applied.
 *   - value: the value of the filter.
 *   - operator:  the operation to apply: '=', '<>', '>', '>=', '<', '<=',
 *     'STARTS_WITH', 'CONTAINS': These operators expect $value to be a
 *     literal of the same type as the column. 'IN', 'NOT IN': These operators
 *     expect $value to be an array of literals of the same type as the column.
 * @param $orderBy
 *   An array of sorting instructions.  Each sort is an associative array with
 *   the following keys:
 *   - field: an array representing the field identical to the output of the
 *     field_info_field() function.
 *   - orderBy: the name of the field on which the filter should be applied.
 *   - direction: either the string 'ASC' (for ascending sort) or 'DESC' (for
 *     descending).
 */
function hook_field_storage_tquery($conditions, $orderBy) {
  // See the tripal_chado_field_storage_tquery() function for an example.
}
/**
 * Retrieves a list of TripalField types.
 *
 * The TripalField classes can be added by a site developer and should be
 * placed in the [module]/includes/fields directory.  Tripal will support
 * any field as long as it is in this directory and extends the TripalField
 * class.  To support dynamic inclusion of new fields this function
 * will look for TripalField class files and return a type for
 * each one.
 *
 * @param $module
 *   The module that provides fields.
 * @return
 *   A list of TripalField field type.
 */
function tripal_get_field_types() {
  $types = array();

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    // Find all of the files in the tripal_chado/includes/fields directory.
    $fields_path = drupal_get_path('module', $module) . '/includes/TripalFields';
    $field_files = file_scan_directory($fields_path, '/.*\.inc$/');
    // Iterate through the fields, include the file and run the info function.
    foreach ($field_files as $file) {
      $field_type = $file->name;
      module_load_include('inc', $module, 'includes/TripalFields/' . $field_type);
      if (class_exists($field_type)) {
        $types[] = $field_type;
      }
    }
  }
  return $types;
}
/**
 * Loads the TripalField class file into scope.
 *
 * @param $field_type
 *   The type of field to instantiate
 *
 * @return
 *   TRUE if the field type class file was found, FALSE otherwise.
 */
function tripal_load_include_field_type($field_type) {

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    $file_path = realpath(".") . '/' . drupal_get_path('module', $module) . '/includes/TripalFields/' . $field_type . '.inc';
    if (file_exists($file_path)) {
      module_load_include('inc', $module, 'includes/TripalFields/' . $field_type);
      if (class_exists($field_type)) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * More easily get the value of a single item from a field's items array.
 *
 * A Drupal field attached to an entity may have multiple items (i.e. it has
 * a cardinality > 1).  Each of these items will always have a 'value' key that
 * contains the data for that field. However, fields can also have other keys
 * with their own values.  You can easily retreive the value of these keys
 * using this function.  What makes this function useful is that you can
 * provide a default value to use if the key has no value.  This is useful
 * when developing a TripalField::widgetForm function and you need to
 * retreive default values and you don't want to have to always check if the
 * value is set.
 *
 * @param $items
 *   The fields $items array. Compatbile with that returned by field_get_items.
 * @param $delta
 *   The index of the item.
 * @parm $key
 *   The name of the key within the item.
 * @param $default
 *   A default value to return if the key is not set. By default the empty
 *   string is returned.
 *
 * @return
 *   The value assigned to the item's key; FALSE if the key doesn't exist or
 *   the $default argument if no value is associated with the key.
 */
function tripal_get_field_item_keyval($items, $delta, $key, $default='') {
  if (!array_key_exists($delta, $items)) {
    return FALSE;
  }
  if (!array_key_exists($key, $items[$delta])) {
    return FALSE;
  }
  if (!$items[$delta][$key]) {
    return $default;
  }
  return $items[$delta][$key];
}


