<?php
/**
 * @file
 * Provides functions for managing analysis'.
 *
 * @ingroup tripal_analysis
 */

/**
 * @defgroup tripal_analysis_api Analysis Module API
 * @ingroup tripal_api
 * @{
 * Provides an interface for specialized analysis modules to extend the basic functionality.
 * @}
 */

/**
 * Register tripal_analysis_api sub-modules
 *
 * @param $modulename
 *  The name of the module to be registered as a tripal analysis submodule
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_register_child($modulename) {
  $sql = "SELECT * FROM {tripal_analysis} WHERE modulename = :modname";
  if (!db_query($sql, array(':modname' => $modulename))->fetchField()) {
    $sql = "INSERT INTO {tripal_analysis} (modulename) VALUES (:modname)";
    db_query($sql, array(':modname' => $modulename));
  }
}

/**
 * Un-register a tripal analysis sub-module
 *
 * @param $modulename
 *  The name of the module to un-register
 *
 * @ingroup tripal_analysis_api
 */
function tripal_analysis_unregister_child($modulename) {
  if (db_table_exists('tripal_analysis')) {
      $sql = "DELETE FROM {tripal_analysis} WHERE modulename = :modname";
      db_query($sql, array(':modname' => $modulename));
  }
}
