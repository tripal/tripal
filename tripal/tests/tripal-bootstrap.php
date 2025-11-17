<?php

/**
 * Tripal preprocessing of bootstrap.php.
 *
 * The purpose of this file is to append a few additional phpunit deprecation
 * exclusions to those already defined by Drupal, because they are in external
 * modules. Because there can only be one phpunit exclusion file, we first
 * copy Drupal's exclusions, and then append the additional ones we define
 * here. We can then proceed to call Drupal's bootstrap.php.
 */

// Define exclusion file names.
$drupal_ignore_file = DRUPAL_ROOT . '/core/.deprecation-ignore.txt';
$tripal_ignore_file = __DIR__ . '/tripal-deprecation-ignore.txt';
$combined_ignore_file = __DIR__ . '/../../.deprecation-ignore.txt';

// Append Tripal's phpunit deprecation exclusions to those already
// defined by Drupal.
$drupal_exclude_text = file_get_contents($drupal_ignore_file);
$tripal_exclude_text = file_get_contents($tripal_ignore_file);
 file_put_contents($combined_ignore_file, $drupal_exclude_text . $tripal_exclude_text);

// Tripal preprocessing is done, now call Drupal's bootstrap.php.
include DRUPAL_ROOT . '/core/tests/bootstrap.php';
