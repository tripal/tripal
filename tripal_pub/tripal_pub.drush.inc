<?php

/**
 * @file
 * Contains function relating to drush-integration of this module.
 */

/**
 * Describes each drush command implemented by the module
 *
 * @return
 *   The first line of description when executing the help for a given command
 */
function tripal_pub_drush_help($command) {
  switch ($command) {
    case 'drush:tripal-pub-import':
      return dt('Imports publications from remote databases using saved configuration settings.');
  }
}

/**
 * Registers a drush command and constructs the full help for that command
 *
 * @return
 *   And array of command descriptions
 */
function tripal_pub_drush_command() {
  $items = array();
  $items['tripal-pubs-import'] = array(
    'description' => dt('Imports publications from remote databases using saved configuration settings.'),
    'examples' => array(
      'Standard example' => 'drush tripal-pubs-import',
    ),
    'aliases' => array('tpubs-import'),
  );
  $items['tripal-pubs-update'] = array(
    'description' => dt('Updates publication information for publications with a supported database cross-reference.'),
    'examples' => array(
      'Standard example' => 'drush tripal-pubs-update',
    ),
    'aliases' => array('tpubs-update'),
  );
  return $items;
}

/**
 * Imports publications into Chado
 *
 */
function drush_tripal_pub_tripal_pubs_import() {
  tripal_pub_import_publications();
}

/**
 * Imports publications into Chado
 *
 */
function drush_tripal_pub_tripal_pubs_update() {
  tripal_pub_update_publications();
}
