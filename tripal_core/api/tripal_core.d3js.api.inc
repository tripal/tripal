<?php
/**
 *
 */

/**
 * Implements hook_libraries_info().
 */
function tripal_core_libraries_info() {
  $libraries = array();
  $libraries['d3'] = array(
    'name' => 'D3.js',
    'vendor url' => 'http://d3js.org/',
    'download url' => 'https://github.com/mbostock/d3',
    'version arguments' => array(
      'file' => 'd3.js',
      'pattern' => '/\s*version: "(\d+\.\d+\.\d+)"/',
    ),
    'files' => array(
      'js' => array(
        'd3.min.js',
      ),
    ),
  );

  return $libraries;
}

/**
 * Load D3.js releated javascripts for the current page.
 */
function tripal_add_d3js() {
  $library = array('loaded' => FALSE);

  // First try to load d3.js using the libraries API.
  // This will work if the site admin has saved d3.js in their libraries folder.
  if (module_exists('libraries_api')) {
    $library = libraries_load('d3');
  }

  // If we were not able to load d3.js using the libraries API
  // then revert to loading the remote files manually.
  if (!isset($library['loaded']) OR !$library['loaded']) {

    // If SSL is being used then use a secure CDN for d3.js
    if (isset($_SERVER['HTTPS'])) {
      drupal_add_js('https://cdnjs.cloudflare.com/ajax/libs/d3/3.5.5/d3.min.js');
    }
    else {
      drupal_add_js('http://d3js.org/d3.v3.min.js');
    }
  }
}