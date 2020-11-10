<?php

/**
 * @file
 * Provides an application programming interface (API) for working with
 * fields attached to TripalEntity content types (bundles).
 *
 */

/**
 * @defgroup tripal_fields_api Tripal Fields
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) for working with
 * fields attached to TripalEntity content types (bundles).
 *
 * Fields:
 * A field is a reusable "data container" that is attached to a Bundle.
 * Programmatically, each field provides one or more primitive data types, with
 * validators and widgets for editing and formatters for display. Each field
 * independently manages the data to which it assigned.  Just like with Bundles,
 * Fields are also described using controlled vocabulary terms.  For example, a
 * gene Bundle has a field attached that provides the name of the gene.
 * This field only provides the name and nothing more.  Tripal uses the
 * schema:name vocabulary term to describe the field.
 *
 * Field Instances:
 * Fields describe "atomic" units of data that are associated with an entity.
 * For example, a "name" is an atomic unit of data about a Gene or Organism
 * entity. Fields can be reused for multiple Bundles. For example, gene, mRNA,
 * genetic markers and variants all have name data.  Despite that all of these
 * Bundles provides a "name", we only need one field to describe that this data
 * is a "name".  However, we may want to customize a field specific to each
 * bundle.  Therefore, an Instance of a field is attached to a bundle, and
 * field instances can then be customized differently.  The most important
 * customization is the one that defines the Chado table from which the data
 * for a field is retrieved.   Despite that field instances are attached to
 * bundles, they become visible with Entities.  When an entity is loaded for
 * display, Drupal examines all of the fields that are attached to the entity's
 * bundle, and then populates the fields instances with data specific to the
 * entity being loaded.
 * @}
 *
 */

/**
 * @section
 * Hooks.
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
 * Allows a module to return a bundles field info.
 *
 * @param $entity_type
 *  The name of the entity, like 'TripalEntity'.
 * @param $bundle
 *  The bundle object.
 *
 * @ingroup tripal_fields_api
 */
function hook_bundle_fields_info($entity_type, $bundle) {

}

/**
 * Allows a module to adjust a field's info settings.
 *
 * Modules should only adjust the info array for fields that
 * they manage and not fields that were created by other modules. For example,
 * if a field corresponds to a column in a custom table of Chado, then the
 * tripal_chado module will automatically create a field for it. This function
 * can be used to override the default settings created by Chado.
 *
 * @param $info
 *  The fields info array for all fields.
 * @param $bundle
 *  The bundle content type object that the field belongs to.
 * @param $term
 *  The bundle type term.
 */
function hook_bundle_fields_info_alter(&$info, $bundle, $term) {

}

/**
 * Allows a module to return the field instances of a bundle.
 *
 * @param $entity_type
 *  The name of the entity, most likely 'TripalEntity'.
 * @param $bundle
 *  The bundle object.
 *
 * @ingroup tripal_fields_api
 */
function hook_bundle_instances_info($entity_type, $bundle) {

}


/**
 * Allows a module to adjust field instances info settings.
 *
 * Modules should only adjust the info array for field instances that
 * they manage and not fields that were created by other modules. For example,
 * if a field corresponds to a column in a custom table of Chado, then the
 * tripal_chado module will automatically create a field for it. This function
 * can be used to override the default settings created by Chado.
 *
 * @param $info
 *  The field instance info array for all fields.
 * @param $bundle
 *  The bundle content type object that the field belongs to.
 * @param $term
 *  The bundle type term.
 */
function hook_bundle_instances_info_alter(&$info, $bundle, $term) {

}
/**
 * Indicate if a field has an empty value.
 *
 * By default, all field values are attached to an entity in the form
 * $entity->{field_name}[{language}][{delta}].   Typically a field will then
 * have a 'value' element:  $entity->{field_name}[{language}][{delta}]['value']
 * and if that value is empty then the field is considered empty by Tripal.
 * By default the tripal_field_is_empty() function is used to check all
 * fields to see if they are empty. However, this hook can be implemented by
 * any module to override that behavior.
 *
 * @param $field
 *   A field array.
 * @param $items
 *   The array of items attached to entity.
 * @param $delta
 *   The specific value to check.  For fields with cardinality greater than
 *   1 then each value can be checked. Defaults to 0 indicating it will check
 *   the first value.
 *
 * @return
 *   TRUE if the field value is empty for the given delta, and FALSE if not
 *   empty.
 *
 * @ingroup tripal_fields_api
 */
function tripal_field_is_empty($field, $items, $delta = 0) {

  // If the $items argument is empty then return TRUE.
  if (!$items) {
    return TRUE;
  }

  // If the field is a tripal field storage API field and there
  // is no value field then the field is empty.
  if (array_key_exists('storage', $field) and
    array_key_exists('tripal_storage_api', $field['storage']['settings']) and
    !array_key_exists('value', $items[$delta])) {
    return TRUE;
  }

  // If there is a value field but there's nothing in it, the the field is
  // empty.
  if (array_key_exists($delta, $items) and
    array_key_exists('value', $items[$delta]) and
    empty($items[$delta]['value'])) {
    return TRUE;
  }

  // Otherwise, the field is not empty.
  return FALSE;
}

/**
 * Retrieves a list of all fields implementing Tripal Fields/Formatters/Widgets.
 *
 * The TripalField classes can be added by the site developer and should be
 * placed in the [module]/includes/TripalFields directory. Tripal will support
 * any field as long as:
 *   - it's in the [modules]/includes/TripalFields directory,
 *   - extends TripalField, TripalWidget or TripalFormatter,
 *   - matches the [cvname]__[termname] convention.
 *
 * @return
 *   An array of files containing Tripal fields, widgets or formatters where
 *   each element has a name, filename, and uri. The URI is relative to your
 *   drupal root.
 */
function tripal_get_tripalfield_files() {
  $field_files = &drupal_static(__FUNCTION__);

  if (!isset($field_files)) {
    $field_files = [];

    // Check module directories.
    $modules = module_list(TRUE);
    foreach ($modules as $module) {

      // Only run this for modules that are enabled.
      if (!module_exists($module)) {
        continue;
      }

      // Find all of the files in the tripal_chado/includes/fields directory.
      $fields_path = drupal_get_path('module', $module) . '/includes/TripalFields';
      $tmp = file_scan_directory($fields_path, '/.*__.*\.inc$/');
      if (!empty($tmp)) {
        $field_files = array_merge($field_files, $tmp);
      }
    }

    // Check the library directory.
    if (module_exists('libraries')) {
      $library_path = libraries_get_path('TripalFields');
      $fields_path = $library_path;
      $tmp = file_scan_directory($fields_path, '/.*__.*\.inc$/');
      if (!empty($tmp)) {
        $field_files = array_merge($field_files, $tmp);
      }
    }
  }

  return $field_files;
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
  $types = [];

  $field_files = tripal_get_tripalfield_files();
  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    // Ignore the formatter and widget classes for now.
    if (preg_match('/_formatter|_widget/', $file->name)) {
      continue;
    }
    $field_type = $file->name;
    require_once($file->uri);
    if (class_exists($field_type) and is_subclass_of($field_type, 'TripalField')) {
      $types[] = $field_type;
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
  $widgets = [];

  $field_files = tripal_get_tripalfield_files();
  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    if (preg_match('/_widget/', $file->name)) {
      $widget_type = $file->name;
      require_once($file->uri);
      if (class_exists($widget_type) and is_subclass_of($widget_type, 'TripalFieldWidget')) {
        $widgets[] = $widget_type;
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
 * @param $instance
 *   A field instance array.
 *
 * @return
 *   A list of file formatter class names.
 */
function tripal_get_field_field_formatters($field, $instance) {
  $field_name = $field['field_name'];
  $field_type = $field['type'];

  $downloaders = [];

  // If the field type is a TripalField then get information about the formatter.
  if (tripal_load_include_field_class($field_type)) {
    $formatters = $field_type::$download_formatters;
    foreach ($formatters as $class_name) {
      if (!array_key_exists($class_name, $downloaders)) {
        tripal_load_include_downloader_class($class_name);
        $downloaders[$class_name] = $class_name::$full_label;
      }
    }
  }
  else {
    // For non TripalFields we'll assume TAB and CSV.
    tripal_load_include_downloader_class('TripalTabDownloader');
    tripal_load_include_downloader_class('TripalCSVDownloader');
    $downloaders['TripalTabDownloader'] = TripalTabDownloader::$full_label;
    $downloaders['TripalCSVDownloader'] = TripalCSVDownloader::$full_label;
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
  $formatters = [];

  $field_files = tripal_get_tripalfield_files();
  // Iterate through the fields, include the file and run the info function.
  foreach ($field_files as $file) {
    if (preg_match('/_formatter/', $file->name)) {
      $formatter_type = $file->name;
      require_once($file->uri);
      if (class_exists($formatter_type) and is_subclass_of($formatter_type, 'TripalFieldFormatter')) {
        $formatters[] = $formatter_type;
      }
    }
  }

  return $formatters;
}

/**
 * Loads the TripalField class file into scope.
 *
 * @param $class
 *   The class to include. This can be a TripalField, TripalFieldWidget or
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
    $file_path = realpath(".") . '/' . $library_path . '/' . $field_type . '/' . $class . '.inc';
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
    $file_path = realpath(".") . '/' . $library_path . '/' . $class . '.inc';
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
 * with their own values.  You can easily retrieve the value of these keys
 * using this function.  What makes this function useful is that you can
 * provide a default value to use if the key has no value.  This is useful
 * when developing a TripalField::widgetForm function and you need to
 * retrieve default values and you don't want to have to always check if the
 * value is set.
 *
 * @param $items
 *   The fields $items array. Compatible with that returned by field_get_items.
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
function tripal_get_field_item_keyval($items, $delta, $key, $default = '') {
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
