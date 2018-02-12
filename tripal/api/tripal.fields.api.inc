<?php

/**
 * @file
 * Provides an application programming interface (API) for working with
 * fields attached to TripalEntity content types (bundles).
 */

/**
 * @defgroup tripal_fields_api Tripal Fields
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with
 * fields attached to TripalEntity content types (bundles).
 * @}
 *
 */

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
 *
 * @ingroup tripal_fields_api
 */
function hook_field_storage_tquery($conditions, $orderBy) {
  // See the tripal_chado_field_storage_tquery() function for an example.
}

/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 *
 * @ingroup tripal_fields_api
 */
function hook_bundle_fields_info($entity_type, $bundle) {

}

/**
 *
 * @param unknown $entity_type
 * @param unknown $bundle
 *
 * @ingroup tripal_fields_api
 */
function hook_bundle_instances_info($entity_type, $bundle) {

}
/**
 *
 * @param unknown $info
 * @param unknown $bundle
 * @param unknown $term
 *
 * @ingroup tripal_fields_api
 */
function hook_bundle_fields_info_alter(&$info, $bundle, $term) {

}
/**
 *
 * @param unknown $info
 * @param unknown $bundle
 * @param unknown $term
 *
 * @ingroup tripal_fields_api
 */
function hook_bundle_instances_info_alter(&$info, $bundle, $term) {

}
/**
 * Retrieves a list of TripalField types.
 *
 * The TripalField classes can be added by a site developer and should be
 * placed in the [module]/includes/TripalFields directory.  Tripal will support
 * any field as long as it is in this directory and extends the TripalField
 * class.  To support dynamic inclusion of new fields this function
 * will look for TripalField class files and return a type for
 * each one.
 *
 * @return
 *   A list of TripalField names.
 *
 * @ingroup tripal_fields_api
 */
function tripal_get_field_types() {
  $types = array();

  $modules = module_list(TRUE);
  foreach ($modules as $module) {

    // Only run this for modules that are enabled.
    if (!module_exists($module)) {
      continue;
    }
    // Find all of the files in the tripal_chado/includes/TripalFields/ directory.
    $fields_path = drupal_get_path('module', $module) . '/includes/TripalFields';
    $field_files = file_scan_directory($fields_path, '/.*\.inc$/');
    // Iterate through the fields, include the file and run the info function.
    foreach ($field_files as $file) {
      // Ignore the formatter and widget classes for now.
      if (preg_match('/_formatter|_widget/', $file->name)) {
        continue;
      }
      $field_type = $file->name;
      module_load_include('inc', $module, 'includes/TripalFields/' . $field_type . '/' . $field_type);
      if (class_exists($field_type) and is_subclass_of($field_type, 'TripalField')) {
        $types[] = $field_type;
      }
    }
  }

  // If the libraries module is enabled then we want to look for a TripalFields
  // library folder and see if our field exist there.
  if (module_exists('libraries')) {
    $library_path = libraries_get_path('TripalFields');
    $fields_path = realpath(".") . '/' . $library_path;
    $field_files = file_scan_directory($fields_path, '/.*\.inc$/');
    foreach ($field_files as $file) {
      // Ignore the formatter and widget classes for now.
      if (preg_match('/_formatter|_widget/', $file->name)) {
        continue;
      }
      $field_type = $file->name;
      $file_path = realpath(".") . '/' . $library_path .'/' . $field_type . '/' . $field_type . '.inc';
      if (file_exists($file_path)) {
        require_once($file_path);
        if (class_exists($field_type) and is_subclass_of($field_type, 'TripalField')) {
          $types[] = $field_type;
        }
      }
    }
  }
  return $types;
}
/**
 * Retrieves a list of TripalFieldWidgets.
 *
 * The TripalFieldWidget classes can be added by a site developer and should be
 * placed in the [module]/includes/TripalFields directory.  Tripal will support
 * any widget as long as it is in this directory and extends the
 * TripalFieldWidget class.
 *
 * @return
 *   A list of TripalFieldWidget names.
 *
 * @ingroup tripal_fields_api
 */
function tripal_get_field_widgets() {
  $widgets = array();

  $modules = module_list(TRUE);
  foreach ($modules as $module) {

    // Only run this for modules that are enabled.
    if (!module_exists($module)) {
      continue;
    }

    // Find all of the files in the tripal_chado/includes/fields directory.
    $fields_path = drupal_get_path('module', $module) . '/includes/TripalFields';
    $field_files = file_scan_directory($fields_path, '/.*_widget\.inc$/');
    // Iterate through the fields, include the file and run the info function.
    foreach ($field_files as $file) {
      $widget_type = $file->name;
      $field_type = preg_replace('/(^.*)_widget/', '$1', $widget_type);
      module_load_include('inc', $module, 'includes/TripalFields/' . $field_type . '/' . $widget_type);
      if (class_exists($widget_type) and is_subclass_of($widget_type, 'TripalFieldWidget')) {
        $widgets[] = $widget_type;
      }
    }
  }

  // If the libraries module is enabled then we want to look for a TripalFields
  // library folder and see if our field exist there.
  if (module_exists('libraries')) {
    $library_path = libraries_get_path('TripalFields');
    $fields_path = realpath(".") . '/' . $library_path;
    $field_files = file_scan_directory($fields_path, '/.*_widget\.inc$/');
    foreach ($field_files as $file) {
      $widget_type = $file->name;
      $field_type = preg_replace('/(^.*)_widget/', '$1', $widget_type);
      $file_path = realpath(".") . '/' . $library_path .'/' . $field_type . '/' . $widget_type . '.inc';
      if (file_exists($file_path)) {
        require_once($file_path);
        if (class_exists($widget_type) and is_subclass_of($widget_type, 'TripalFieldWidget')) {
          $widgets[] = $widget_type;
        }
      }
    }
  }
  return $widgets;
}

/**
 * Retrieves a list of field formatters compatible with a given field.
 *
 * @param $field
 *   A field array as returned by the field_info_field() function.
 * @return
 *   A list of file formatter class names.
 */
function tripal_get_field_field_formatters($field) {
  $field_name = $field['field_name'];
  $field_type = $field['type'];

  $downloaders = array();

  // All fields should support the Tab and CSV downloaders.
  tripal_load_include_downloader_class('TripalTabDownloader');
  $downloaders['TripalTabDownloader'] = TripalTabDownloader::$label;
  tripal_load_include_downloader_class('TripalCSVDownloader');
  $downloaders['TripalCSVDownloader'] = TripalCSVDownloader::$label;

  if (tripal_load_include_field_class($field_type)) {
    $settings = $field_type::$default_instance_settings;
    if (array_key_exists('download_formatters', $settings)) {
      foreach ($settings['download_formatters'] as $class_name) {
        if (!array_key_exists($class_name, $downloaders)) {
          tripal_load_include_downloader_class($class_name);
          $downloaders[$class_name] = $class_name::$label;
        }
      }
    }
  }
  return $downloaders;
}

/**
 * Retrieves a list of TripalFieldFormatters.
 *
 * The TripalFieldFormatter classes can be added by a site developer and should
 * be placed in the [module]/includes/TripalFields directory.  Tripal will
 * support any widget as long as it is in this directory and extends the
 * TripalFieldFormatter class.
 *
 * @return
 *   A list of TripalFieldFormatter names.
 *
 * @ingroup tripal_fields_api
 */
function tripal_get_field_formatters() {
  $formatters = array();

  $modules = module_list(TRUE);
  foreach ($modules as $module) {

    // Only run this for modules that are enabled.
    if (!module_exists($module)) {
      continue;
    }

    // Find all of the files in the tripal_chado/includes/fields directory.
    $fields_path = drupal_get_path('module', $module) . '/includes/TripalFields';
    $field_files = file_scan_directory($fields_path, '/.*_formatter\.inc$/');
    // Iterate through the fields, include the file and run the info function.
    foreach ($field_files as $file) {
      $formatter_type = $file->name;
      $field_type = preg_replace('/(^.*)_formatter/', '$1', $formatter_type);
      module_load_include('inc', $module, 'includes/TripalFields/' . $field_type . '/' . $formatter_type);
      if (class_exists($formatter_type) and is_subclass_of($formatter_type, 'TripalFieldFormatter')) {
        $formatters[] = $formatter_type;
      }
    }
  }

  // If the libraries module is enabled then we want to look for a TripalFields
  // library folder and see if our field exist there.
  if (module_exists('libraries')) {
    $library_path = libraries_get_path('TripalFields');
    $fields_path = realpath(".") . '/' . $library_path;
    $field_files = file_scan_directory($fields_path, '/.*_formatter\.inc$/');
    foreach ($field_files as $file) {
      $formatter_type = $file->name;
      $field_type = preg_replace('/(^.*)_formatter/', '$1', $formatter_type);
      $file_path = realpath(".") . '/' . $library_path .'/' . $field_type . '/' . $formatter_type . '.inc';
      if (file_exists($file_path)) {
        require_once($file_path);
        if (class_exists($formatter_type) and is_subclass_of($formatter_type, 'TripalFieldFormatter')) {
          $formatters[] = $formatter_type;
        }
      }
    }
  }
  return $formatters;
}
/**
 * Loads the TripalField class file into scope.
 *
 * @param $class
 *   The class to include.  This can be a TripalField, TripalFieldWidget or
 *   TripalFieldFormatter class name.
 *
 * @return
 *   TRUE if the field type class file was found, FALSE otherwise.
 *
 * @ingroup tripal_fields_api
 */
function tripal_load_include_field_class($class) {

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    $field_type = preg_replace('/(^.*)_(formatter|widget)/', '$1', $class);
    $file_path = realpath(".") . '/' . drupal_get_path('module', $module) . '/includes/TripalFields/' . $field_type . '/' . $class . '.inc';
    if (file_exists($file_path)) {
      module_load_include('inc', $module, 'includes/TripalFields/' . $field_type . '/' . $class);
      if (class_exists($class)) {
        return TRUE;
      }
    }
  }

  // If the libraries module is enabled then we want to look for a TripalFields
  // library folder and see if our field exist there.
  if (module_exists('libraries')) {
    $library_path = libraries_get_path('TripalFields');
    $field_type = preg_replace('/(^.*)_(formatter|widget)/', '$1', $class);
    $file_path = realpath(".") . '/' . $library_path .'/' . $field_type . '/' . $class . '.inc';
    if (file_exists($file_path)) {
      require_once($file_path);
      if (class_exists($class)) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
 * Loads the TripalEntityDownloader file into scope.
 *
 * @param $class
 *   The name of the class to include.
 *
 * @return
 *   TRUE if the downloader class file was found, FALSE otherwise.
 *
 * @ingroup tripal_fields_api
 */
function tripal_load_include_downloader_class($class) {

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    $file_path = realpath(".") . '/' . drupal_get_path('module', $module) . '/includes/TripalFieldDownloaders/' . $class . '.inc';
    if (file_exists($file_path)) {
      module_load_include('inc', $module, 'includes/TripalFieldDownloaders/' . $class);
      if (class_exists($class)) {
        return TRUE;
      }
    }
  }

  // If the libraries module is enabled then we want to look for a
  // TripalFieldDownloader library folder and see if our field exist there.
  if (module_exists('libraries')) {
    $library_path = libraries_get_path('TripalFieldDownloaders');
    $file_path = realpath(".") . '/' . $library_path .'/' . $class . '.inc';
    if (file_exists($file_path)) {
      require_once($file_path);
      if (class_exists($class)) {
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
 *
 * @ingroup tripal_fields_api
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

/**
 * Formats an element of a TripalField for use by Drupal Views.
 *
 * Sometimes the value of TripalField can be more than just a single scalar. In
 * this case the value is an array of key value pairs where each key is a
 * controlled vocabulary term.  In order to support fields, filtering and
 * sorting by these sub elements using Drupal Views, the TripalField
 * implementation must provide some help to Views by describing these elements,
 * and then implementing a query() function to support them.  However, the
 * naming of sub elements must follow a set convention. This function
 * guarantees proper naming for sub elements.
 *
 * @param $field_name
 *   The name of the field to which the element belongs.
 * @param $term
 *   The term object as returned by tripal_get_term_details();
 *
 * @ingroup tripal_fields_api
 */
function tripal_format_views_field_element($field_name, $term) {
  $element_name = $term['vocabulary']['short_name'] . '__' . $term['accession'];
  return $field_name . '.' . $element_name;
}
