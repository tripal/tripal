#!/usr/bin/env bash

# Select the appropriate version of the phpunit.xml
# file to use. Drupal 10.x uses an earlier version.

drupalversion=$(drush core-status --field='Drupal version')
phpunit_config="/var/www/drupal/web/modules/contrib/tripal/phpunit.xml"
if $(dpkg --compare-versions "${drupalversion}" "lt" "10.6"); then
  phpunit_config="/var/www/drupal/web/modules/contrib/tripal/phpunit.9.6.xml"
fi

echo -n "$phpunit_config"
