<?php

/**
 * Retrieves a list of TripalField class instances for a given module.
 *
 * The TripalField classes can be added by a site developer and should be
 * placed in the [module]/includes/fields directory.  Tripal will support
 * any field as long as it is in this directory and extends the TripalField
 * class.  To support dynamic inclusion of new fields this function
 * will look for TripalField class files and return a list of instantiated
 * objects for all of them.
 *
 * @param $module
 *   The module that provides fields.
 * @param $entity_type
 *   The type of entity (e.g. TripalEntity).
 * @param $bundle
 *   The bundle object.  This is the bundle that is being offered
 *   to the field for attachement. The function will determine if this
 *   field should be attached to this bundle.
 * @param $details
 *   An array of additional "details" that are required by the modue that
 *   implements this field. For example, an associative array of values
 *   needed for the field to work properly.
 * @return
 *   A list of TripalField objects.  If no field can be found in the
 *   module then an empty array is returned.
 */
function tripal_get_fields($module, $entity_type, $bundle, $details = array()) {

  $objects = array();

  // Find all of the files in the tripal_chado/includes/fields directory.
  $fields_path = drupal_get_path('module', $module) . '/includes/fields';
  $field_files = file_scan_directory($fields_path, '/.*\.inc$/');

  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    $field_type = $file->name;
    module_load_include('inc', $module, 'includes/fields/' . $field_type);
    if (class_exists($field_type)) {
      $objects[] = new $field_type($entity_type, $bundle, $details);
    }
  }

  return $objects;
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
function tripal_get_field_types($module) {
  $types = array();

  // Find all of the files in the tripal_chado/includes/fields directory.
  $fields_path = drupal_get_path('module', $module) . '/includes/fields';
  $field_files = file_scan_directory($fields_path, '/.*\.inc$/');

  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    $field_type = $file->name;
    module_load_include('inc', $module, 'includes/fields/' . $field_type);
    if (class_exists($field_type)) {
      $types[] = $field_type;
    }
  }

  return $types;
}
/**
 * Retreives an intance of a TripalField class.
 *
 * The TripalField classes can be added by a site developer and should be
 * placed in the [module]/includes/fields directory.  Tripal will support
 * any field as long as it is in this directory and extends the TripalField
 * class.  To support dynamic inclusion of new fields this function
 * serves as a wrapper for locating the appropriate class file and
 * instantiating the object.
 *
 * @param $field_type
 *   The type of field to instantiate
 * @param $module
 *   The module that provides this field.
 * @param $entity_type
 *   The type of entity (e.g. TripalEntity).
 * @param $bundle
 *   The bundle object.  This is the bundle that is being offered
 *   to the field for attachement. The function will determine if this
 *   field should be attached to this bundle.
 * @param $details
 *   An array of additional "details" that are required by the modue that
 *   implements this field. For example, an associative array of values
 *   needed for the field to work properly.
 */
function tripal_get_field($field_type, $module, $entity_type, $bundle, $details = array()) {

  module_load_include('inc', $module, 'includes/fields/' . $field_type);
  if (class_exists($field_type)) {
    return new $field_type($entity_type, $bundle, $details);
  }
  return NULL;
}