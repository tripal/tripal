<?php

/**
 * @file
 *
 * Provides the API for loading d3js onto pages.
 */

/**
 * @defgroup tripal_d3js_api d3js
 * @ingroup tripal_api
 * @{
 * D3.js is a JavaScript library for producing dynamic, interactive data
 * visualizations in web browsers. It makes use of the widely implemented SVG,
 * HTML5, and CSS standards. For more information on how to use d3js please see
 * https://github.com/d3/d3.
 *
 * For an example of d3 usage in Tripal please review
 * tripal_chado/includes/tripal_chado.phylotree.inc and
 * tripal_chado/theme/js/d3.phylogram.js
 *
 * @}
 */


/**
 * Load D3.js related javascripts for the current page.
 *
 * @ingroup tripal_d3js_api
 */
function tripal_add_d3js() {
  $library = ['loaded' => FALSE];

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