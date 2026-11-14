#!/bin/bash

##
## Determine PHPunit config file based on Drupal version.
##
## This script should be run within a working site environment
## and within the same directory as this script.
##
## What will this script do?
##
## 1. Run drush status to detect the current Drupal
##    version installed.
## 2. Extract the first and second parts of the semantic
##    version and save as MAJOR and MINOR respectively.
## 3. Check for a file named phpunit.MAJOR.MINOR.xml, if
##    it exists then symlink it to phpunit.xml
## 4. If not 3 then check for a file named phpunit.MAJOR.xml.
##    If it exists then symlink it to phpunit.xml
##

## FALLBACK
## Set to the highest config version.
configfile='phpunit.12.xml'

# Drupal version can either be of the form "11.2.3" or "11.2-dev".
# test-tripal-action will pass it on the command line, otherwise
# look it up using drush.
drupal_version_string="$1"
if [[ -n "$drupal_version_string" ]] ; then
  drupal_version_string=$(drush status | grep 'Drupal version' | sed -e 's/^.*: //' -e 's/[ -].*$//')
fi
IFS='.' read -ra drupal_version_parts <<< "$drupal_version_string"
# Check for phpunit_for_drupal_MAJOR.MINOR.xml (e.g. phpunit_for_drupal_11.4.xml)
versioned_file="phpunit_for_drupal_${drupal_version_parts[0]}.${drupal_version_parts[1]}.xml"
if [ -f "$versioned_file" ]; then
  echo "Option 1: $versioned_file --found!"
  configfile="$versioned_file"
else
  echo "Option 1: $versioned_file --doesn't exist."
  # Check for phpunit_for_drupal_MAJOR.xml (e.g. phpunit_for_drupal_12.xml)
  versioned_file="phpunit_for_drupal_${drupal_version_parts[0]}.xml"
  if [ -f "$versioned_file" ]; then
    echo "Option 2: $versioned_file --found!"
      configfile="$versioned_file"
  # Use the fallback version so we don't end up with nothing.\
  else
    echo "Option 2: $versioned_file --doesn't exist."
    echo "Final Option: RESORTING TO FALLBACK config file."
  fi
fi
rm -f phpunit.xml
ln -s $configfile phpunit.xml
echo ""
echo "Using phpunit config file: $configfile"
if [[ -n "$GITHUB_OUTPUT" ]] ; then
  echo "Using phpunit config file: $configfile" >> $GITHUB_OUTPUT
fi
