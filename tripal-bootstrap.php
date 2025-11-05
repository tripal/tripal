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
$drupal_ignore = '/var/www/drupal/web/core/.deprecation-ignore.txt';
$tripal_ignore = '/tmp/tripal-deprecation-ignore.txt';

// Append Tripal's phpunit deprecation exclusions to those already
// defined by Drupal.

$additional_ignores = '
# We are ignoring these phpunit deprecation notices under Drupal 11.3
# because they are in external modules, but we need to watch out for
# these in the future!

# Field Group Module
# /var/www/drupal/web/core/lib/Drupal/Core/Hook/HookCollectorPass.php:591
# field_group_module_implements_alter without a #[LegacyModuleImplementsAlter] attribute is deprecated in drupal:11.2.0 and removed in drupal:12.0.0. See https://www.drupal.org/node/3496788
# One of the tests that triggers this: testTripalEmptyContentTypes
/field_group_module_implements_alter without a #\\[LegacyModuleImplementsAlter\\] attribute/

# Drupal
# /var/www/drupal/vendor/symfony/error-handler/DebugClassLoader.php:347
# Method "Twig\Extension\ExtensionInterface::getFunctions()" might add "array" as a native return type declaration in the future. Do the same in implementation "Drupal\devel\Twig\Extension\Debug" now to avoid errors or add an explicit @return annotation to suppress this message.
# One of the tests that triggers this: testTripalEmptyContentTypes
/Method "Twig\\\\Extension\\\\ExtensionInterface::getFunctions\(\)" might add "array"/
';

copy($drupal_ignore, $tripal_ignore);
file_put_contents($tripal_ignore, $additional_ignores, FILE_APPEND);

// Tripal preprocessing is done, now call Drupal's bootstrap.php.
include '/var/www/drupal/web/core/tests/bootstrap.php';
