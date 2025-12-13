#!/bin/bash

##
## Determine config file based on PHPunit version.
##
## This script should be run within a working site environment
## and within the same directory as this script.
##
## What will this script do?
##
## 1. Run phpunit --version to detect the current
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
configfile='phpunit.11.xml'

version_str=$(phpunit --version)
major_version=$(echo "$version_str" | sed -E 's/^PHPUnit ([0-9]+).*/\1/')
minor_version=$(echo "$version_str" | sed -E 's/^PHPUnit [0-9]+\.([0-9]+).*/\1/')
# Check for phpunit.MAJOR.MINOR.xml (e.g. phpunit.9.6.xml)
versioned_file="phpunit.${major_version}.${minor_version}.xml"
if [ -f "$versioned_file" ]; then
  echo "Option 1: $versioned_file --found!"
  configfile="$versioned_file"
  ln -s $versioned_file phpunit.xml
else
  echo "Option 1: $versioned_file --doesn't exist."
  # Check for phpunit.MAJOR.xml (e.g. phpunit.10.xml)
  versioned_file="phpunit.${major_version}.xml"
  if [ -f "$versioned_file" ]; then
    echo "Option 2: $versioned_file --found!"
      configfile="$versioned_file"
      ln -s $versioned_file phpunit.xml
  # Use the fallback version so we don't end up with nothing.\
  else
    echo "Option 2: $versioned_file --doesn't exist."
    echo "Final Option: RESORTING TO FALLBACK config file."
    ln -s $configfile phpunit.xml
  fi
fi
echo ""
echo "Using phpunit config file: $configfile"
