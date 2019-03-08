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
 *
 * @ingroup tripal_drush
 */
function tripal_pub_drush_help($command) {
  switch ($command) {
    case 'drush:tripal-import-pubs':
      return dt('Imports publications from remote databases using saved configuration settings.');
    case 'drush:tripal-update-pubs':
      return dt('Updates publication information for publications with a supported database cross-reference.');
  }
}

/**
 * Registers a drush command and constructs the full help for that command
 *
 * @return
 *   And array of command descriptions
 *
 * @ingroup tripal_drush
 */
function tripal_pub_drush_command() {
  $items = [];


  // Deprecated commands
  $items['tripal-pubs-import'] = [
    'description' => dt('DEPRECATED. Please see: trp-import-pubs.'),
    'options' => [
      'create_contacts' => dt('provide this option to create or update contacts for authors. By default contacts are not created or updated.'),
      'dbxref' => dt('An accession number for a publication from a remote database (e.g. PMID:23582642).'),
      'report' => dt("Set to the email address of the recipient who should receive an HTML report of the publications that have been added."),
      'update' => dt("Set to 'Y' to update existing pubs.  By default only new pubs are inserted."),
    ],
    'examples' => [
      'Standard example' => 'drush tripal-pubs-import',
      'Standard example' => 'drush -l http://[site url] tripal-pubs-import --report=[email]. Where [site url] is the URL of the website and [email] is the email address of the recipient to receive the HTML report',
      'Import single publication' => 'drush tripal-pub-import --dbxref=PMID:23582642',
    ],
    'aliases' => ['tpubs-import'],
  ];
  $items['tripal-pubs-update'] = [
    'description' => dt('DEPRECATED. Please see: trp-update-pubs.'),
    'options' => [
      'create_contacts' => dt('provide this option to create or update contacts for authors. By default contacts are not created or updated.'),
      'dbxref' => dt('An accession number for a publication from a remote database (e.g. PMID:23582642)'),
      'db' => dt('The database name (e.g. PMID or AGL)'),
    ],
    'examples' => [
      'Standard example' => 'drush tripal-pubs-update',
      'Create contacts during update' => 'drush tripal-pubs-update --create_contacts=1',
      'Update a single record' => 'drush tripal-pubs-update --dbxref=PMID:23582642',
      'Update all records for a single database' => 'drush tripal-pubs-update --db=PMID',
    ],
    'aliases' => ['tpubs-update'],
  ];

  return $items;
}


/**
 * DEPRECATED. Imports publications into Chado
 *
 * @ingroup tripal_drush
 */
function drush_tripal_pub_tripal_pubs_import() {
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
    "continue to work but please consider using the 'trp-import-pubs' command.\n\n");

  drush_tripal_pub_trp_import_pubs();
}


/**
 * DEPRECATED. Imports publications into Chado
 *
 * @ingroup tripal_drush
 */
function drush_tripal_pub_tripal_pubs_update() {
  drush_print("\n\nDEPRECATED: This drush command is outdated.\nIt will " .
    "continue to work but please consider using the 'trp-update-pubs' command.\n\n");

  drush_tripal_pub_trp_update_pubs();
}
