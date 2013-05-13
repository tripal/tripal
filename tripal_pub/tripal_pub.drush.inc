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
      'Import single publication' => 'drush tripal-pub-import --dbxref=PMID:23582642',  
    ),
    'options' => array(
      'create_contacts' => dt('provide this option to create or update contacts for authors. By default contacts are not created or updated.'),
      'dbxref' => dt('An accession number for a publication from a remote database (e.g. PMID:23582642)'),
    ),
    'aliases' => array('tpubs-import'),
  );
  $items['tripal-pubs-update'] = array(
    'description' => dt('Updates publication information for publications with a supported database cross-reference.'),
    'options' => array(
      'create_contacts' => dt('provide this option to create or update contacts for authors. By default contacts are not created or updated.'),
      'dbxref' => dt('An accession number for a publication from a remote database (e.g. PMID:23582642)'),
      'db' => dt('The database name (e.g. PMID or AGL)'),
    ),
    'examples' => array(
      'Standard example' => 'drush tripal-pubs-update',
      'Create contacts during update' => 'drush tripal-pubs-update --create_contacts=1',
      'Update a single record' => 'drush tripal-pubs-update --dbxref=PMID:23582642',
      'Update all records for a single database' => 'drush tripal-pubs-update --db=PMID'
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
  $create_contacts = drush_get_option('create_contacts');
  $dbxref = drush_get_option('dbxref');
  if ($dbxref) {
    tripal_pub_import_by_dbxref($dbxref, $create_contacts);  
  }
  else {
    tripal_pub_import_publications();
  }
}
/**
 * Imports publications into Chado
 *
 */
function drush_tripal_pub_tripal_pubs_update() {
  $create_contacts = drush_get_option('create_contacts');
  $dbxref = drush_get_option('dbxref');
  $db = drush_get_option('db');
    
  tripal_pub_update_publications($create_contacts, $dbxref, $db);
}
