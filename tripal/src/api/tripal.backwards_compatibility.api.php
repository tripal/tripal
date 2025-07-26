<?php

/**
 * @file
 * Functions handling backwards compatibility.
 */

/**
 * Backwards compatibility fix for a new views css option.
 *
 * See https://www.drupal.org/node/3499943 for the new option.
 * Because this option requires a schema update, having it in the view for
 * Drupal versions < 11.2 will cause errors. This function removes the new
 * setting if the Drupal version is less than 11.2.
 *
 * @param string $config
 *   The view configuration.
 *
 * @return string
 *   The updated view configuration.
 */
function bc_11_2_view(string $config): string {
  if (floatval(\Drupal::VERSION) < 11.2) {
    // Removes the entire line from the config.
    $config = preg_replace("/ +class: ''[\r\n]+/", '', $config);
  }
  return $config;
}
