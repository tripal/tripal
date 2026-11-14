#!/bin/bash

##
## Determine PHPunit config file based on Drupal or phpunit version.
##
## This script should be run within a working site environment
## and within the same directory as this script.
##
## What will this script do?
##
## 1. If the Drupal version is not passed on the command line,
##    run drush status to detect the current Drupal version installed.
## 2. Extract the first and second parts of the semantic
##    version and save as MAJOR and MINOR respectively.
## 3. Check for a file named phpunit_for_drupal_MAJOR.MINOR.xml.
##    If it exists then symlink it to phpunit.xml.
## 4. If not 3 then check for a file named phpunit_for_drupal_MAJOR.xml.
##    If it exists then symlink it to phpunit.xml.
## 5. If not 4 then fallback to the legacy mode of using the
##    version of phpunit, and from that extract MAJOR and MINOR.
## 6. Check for a file named phpunit.MAJOR.MINOR.xml.
##    If it exists then symlink it to phpunit.xml
## 7. If not 6 then Check for a file named phpunit.MAJOR.xml.
##    If it exists then symlink it to phpunit.xml
## 8. If not 7 then use a default based on the highest supported
##    version.

## FALLBACK
## Set to the highest config version.
configfile='phpunit_for_drupal_12.xml'

# Drupal version can be of the form "11.2.3", "11.2.x", or "11.2-dev".
# test-tripal-action will pass the version on the command line, otherwise
# look it up using drush.
drupal_version_str="$1"
if [[ -z "$drupal_version_str" ]] ; then
  drupal_version_str=$(drush status | grep 'Drupal version')
fi
drupal_major_version=$(echo "$drupal_version_str" | sed -E 's/^PHPUnit ([0-9]+).*/\1/')
drupal_minor_version=$(echo "$drupal_version_str" | sed -E 's/^PHPUnit [0-9]+\.([0-9]+).*/\1/')

# Get and parse phpunit version in case we need the legacy behavior.
phpunit_version_str=$(phpunit --version)
phpunit_major_version=$(echo "$phpunit_version_str" | sed -E 's/^PHPUnit ([0-9]+).*/\1/')
phpunit_minor_version=$(echo "$phpunit_version_str" | sed -E 's/^PHPUnit [0-9]+\.([0-9]+).*/\1/')

# Check for phpunit_for_drupal_MAJOR.MINOR.xml (e.g. phpunit_for_drupal_11.4.xml)
versioned_file="phpunit_for_drupal_${drupal_major_version}.${drupal_minor_version}.xml"
if [ -f "$versioned_file" ]; then
  echo "Option 1: $versioned_file --found!"
  configfile="$versioned_file"
else
  echo "Option 1: $versioned_file --doesn't exist."
  # Check for phpunit_for_drupal_MAJOR.xml (e.g. phpunit_for_drupal_12.xml)
  versioned_file="phpunit_for_drupal_${drupal_major_version}.xml"
  if [ -f "$versioned_file" ]; then
    echo "Option 2: $versioned_file --found!"
    configfile="$versioned_file"
  else
    echo "Option 2: $versioned_file --doesn't exist."
    # Check for phpunit.MAJOR.MINOR.xml (e.g. phpunit.9.6.xml).
    versioned_file="phpunit.${phpunit_major_version}.${phpunit_minor_version}.xml"
    if [ -f "$versioned_file" ]; then
      echo "Option 3: $versioned_file --found!"
      configfile="$versioned_file"
    else
      echo "Option 3: $versioned_file --doesn't exist."
      # Check for phpunit.MAJOR.xml (e.g. phpunit.11.xml).
      versioned_file="phpunit.${phpunit_major_version}.xml"
      if [ -f "$versioned_file" ]; then
        echo "Option 4: $versioned_file --found!"
        configfile="$versioned_file"
      else
        # Use the fallback version so we don't end up with nothing.
        echo "Option 4: $versioned_file --doesn't exist."
        echo "Final Option: RESORTING TO FALLBACK config file $configfile."
      fi
    fi
  fi
fi

# Make the symlink to the selected version.
rm -f phpunit.xml
ln -s $configfile phpunit.xml

# Set variable for test-tripal-action.
if [[ -n "$GITHUB_OUTPUT" ]] ; then
  echo "configfile=$configfile" >> $GITHUB_OUTPUT
fi

# Final status message.
echo ""
echo "Using phpunit config file: $configfile"
