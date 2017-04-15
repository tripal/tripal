<?php
/**
 * Retrieves a list of TripalLoader class names.
 *
 * The TripalLoader classes can be added by a site developer and should be
 * placed in the [module]/includes/TripalLoader directory.  Tripal will support
 * any loader as long as it is in this directory and extends the TripalLoader
 * class.  To support dynamic inclusion of new loaders this function
 * will look for TripalLoader class files and return them all.
 *
 * @return
 *   A list of TripalLoader class names.
 */
function tripal_get_data_loaders() {
  $loaders = array();

  $modules = module_list(TRUE);
  foreach ($modules as $module) {
    // Find all of the files in the tripal_chado/includes/TripalLoader/ directory.
    $loader_path = drupal_get_path('module', $module) . '/includes/TripalLoader';
    $loader_files = file_scan_directory($fields_path, '/.*\.inc$/');
    // Iterate through the loaders, include the file and run the info function.
    foreach ($loader_files as $file) {
      $loader = preg_replace('/\.inc$/', '', $file->name);
      module_load_include('inc', $module, 'includes/TripalLoader/' . $loader . '.inc');
      if (class_exists($loader) and is_subclass_of($field_type, 'TripalLoader')) {
        $loaders[] = $loader;
      }
    }
  }
  return $loaders;
}