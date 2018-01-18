<?php
/**
 * @file
 *
 * This file provides the Tripal Web Services API: a set of functions for
 * interacting with the Tripal Web Services.
 */

/**
 * @defgroup tripal_ws_api Web Services
 *
 * @ingroup tripal_api
 * The Tripal Web Services API provides a set of functions for interacting
 * with the Tripal Web Services.
 *
 */

/**
 * Adjust the values of a field for display in web services.
 *
 * This hook should be used sparingly. It is meant primarily to adjust 3rd
 * Party (non Tripal) fields so that they work with web
 * services.  The caller should adjust the $items array as needed.
 * This change only affects the value displayed in web services.  Web services
 * expect that every field have a 'value' element for each of the items. If a
 * field for some reason does not have a 'value' element then this hook will
 * allow setting of that element.
 *
 * @param $items
 *   The list of items for the field.
 * @param $field
 *   The field array.
 * @param $instance
 *   The field instance array.
 *
 * @ingroup tripal_ws_api
 */

function hook_tripal_ws_value(&$items, $field, $instance) {
  // The image module doesn't properly set the 'value' field, so we'll do it
  // here.
  if($field['type'] == 'image' and $field['module'] == 'image') {
    foreach ($items as $delta => $details) {
      if ($items[$delta] and array_key_exists('uri', $items[$delta])) {
        $items[$delta]['value']['schema:url'] = file_create_url($items[$delta]['uri']);
      }
    }
  }
}

/**
 * Retrieves a list of TripalWebService implementations.
 *
 * The TripalWebService classes can be added by a site developer that wishes
 * to create a new Tripal compatible web serivce.  The class file should
 * be placed in the [module]/includes/TripalWebService directory.  Tripal will
 * support any service as long as it is in this directory and extends the
 * TripalWebService class.
 *
 * @return
 *   A list of TripalWebService names.
 */
function tripal_get_web_services() {
  $services = array();

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    // Find all of the files in the tripal_chado/includes/fields directory.
    $service_path = drupal_get_path('module', $module) . '/includes/TripalWebService';
    $service_files = file_scan_directory($service_path, '/.inc$/');
    // Iterate through the fields, include the file and run the info function.
    foreach ($service_files as $file) {
      $class = $file->name;
      module_load_include('inc', $module, 'includes/TripalWebService/' . $class);
      if (class_exists($class) and is_subclass_of($class, 'TripalWebService')) {
        $services[] = $class;
      }
    }
  }
  return $services;
}

/**
 * Loads the TripalWebService class file into scope.
 *
 * @param $class
 *   The TripalWebService class to include.
 *
 * @return
 *   TRUE if the field type class file was found, FALSE otherwise.
 */
function tripal_load_include_web_service_class($class) {

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    $file_path = realpath(".") . '/' . drupal_get_path('module', $module) . '/includes/TripalWebService/' . $class . '.inc';
    if (file_exists($file_path)) {
      module_load_include('inc', $module, 'includes/TripalWebService/' . $class);
      if (class_exists($class)) {
        return TRUE;
      }
    }
  }
  return FALSE;
}