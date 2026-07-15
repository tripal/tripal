<?php

/**
 * @file
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
$tripal_ignore_file = __DIR__ . '/../../.tripal-deprecation-ignore.txt';
$combined_ignore_file = __DIR__ . '/../../.deprecation-ignore.txt';

print "\nTripal is ignoring the deprecation warnings defined in:\n";
print "- " . $drupal_ignore_file . "\n";
print "- " . $tripal_ignore_file . "\n\n";

// Set the SYMFONY_DEPRECATIONS_HELPER environment variable to point to the
// combined ignore file but only in Drupal versions less than 11.5.
// If we are on a dev version, increment minor version by 1 so that we can
// distinguish 11.x from 11.4 for example.
$drupal_version = \Drupal::VERSION;
if (version_compare($drupal_version, '11.4.2', '<=') && ($drupal_version !== '11.4-dev')) {
  print "We are using Symphony deprecation helper. This should only be used for Drupal versions less than and including 11.4.2.\n\n";
  putenv('SYMFONY_DEPRECATIONS_HELPER=ignoreFile=../../../modules/contrib/tripal/.deprecation-ignore.txt');
}

// Append Tripal's phpunit deprecation exclusions to those already
// defined by Drupal.
$exclude_text = file_get_contents($drupal_ignore_file);
$exclude_text .= file_get_contents($tripal_ignore_file);
file_put_contents($combined_ignore_file, $exclude_text);

print "Drupal $drupal_version by the Drupal Community.\n";
// Tripal preprocessing is done, now call Drupal's bootstrap.php.
include DRUPAL_ROOT . '/core/tests/bootstrap.php';
