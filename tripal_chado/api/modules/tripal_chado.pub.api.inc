<?php
/**
 * @file
 * Provides API functions specifically for managing publication
 * records in Chado.
 */

/**
 * @defgroup tripal_pub_api Chado Publication
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing publication
 * records in Chado.
 * @}
 */


/**
 * Retrieves a chado publication array.
 *
 * @param $identifier
 *   An array used to uniquely identify a publication. This array has the same
 *   format as that used by the chado_generate_var(). The following keys can be
 *   useful for uniquely identifying a publication as they should be unique:
 *    - pub_id: the chado pub.pub_id primary key.
 *    - nid: the drupal nid of the publication.
 *    - uniquename: A value to matach with the pub.uniquename field.
 *   There are also some specially handled keys. They are:
 *    - property: An array describing the property to select records for. It
 *      should at least have either a 'type_name' key (if unique across cvs) or
 *      'type_id' key. Other supported keys include: 'cv_id', 'cv_name'
 *      (of the type), 'value' and 'rank'
 *    - dbxref: The database cross reference accession.  It should be in the
 *      form DB:ACCESSION, where DB is the database name and ACCESSION is the
 *      unique publication identifier (e.g. PMID:4382934)
 *    - dbxref_id:  The dbxref.dbxref_id of the publication.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the pub record to
 * be returned.
 *
 * @return
 *   If a singe publication is retreived using the identifiers, then a
 *   publication array will be returned.  The array is of the same format
 *   returned by the chado_generate_var() function. Otherwise, FALSE will be
 *   returned.
 *
 * @ingroup tripal_pub_api
 */
function chado_get_publication($identifiers, $options = []) {

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: The identifier passed in is expected to be an array with the key
       matching a column name in the pub table (ie: pub_id or name). You passed in %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: You did not pass in anything to identify the publication you want. The identifier
       is expected to be an array with the key matching a column name in the pub table
       (ie: pub_id or name). You passed in %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }

  // If one of the identifiers is property then use
  // chado_get_record_with_property().
  if (array_key_exists('property', $identifiers)) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $pub = chado_get_record_with_property(
      ['table' => 'pub', 'base_records' => $identifiers],
      ['type_name' => $property],
      $options
    );
  }
  elseif (array_key_exists('dbxref', $identifiers)) {
    if (preg_match('/^(.*?):(.*?)$/', $identifiers['dbxref'], $matches)) {
      $dbname = $matches[1];
      $accession = $matches[2];

      // First make sure the dbxref is present.
      $values = [
        'accession' => $accession,
        'db_id' => [
          'name' => $dbname,
        ],
      ];
      $dbxref = chado_select_record('dbxref', ['dbxref_id'], $values);
      if (count($dbxref) == 0) {
        return FALSE;
      }
      $pub_dbxref = chado_select_record('pub_dbxref', ['pub_id'], ['dbxref_id' => $dbxref[0]->dbxref_id]);
      if (count($pub_dbxref) == 0) {
        return FALSE;
      }
      $pub = chado_generate_var('pub', ['pub_id' => $pub_dbxref[0]->pub_id], $options);
    }
    else {
      tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
        "chado_get_publication: The dbxref identifier is not correctly formatted.",
        ['%identifier' => print_r($identifiers, TRUE)]
      );
    }
  }
  elseif (array_key_exists('dbxref_id', $identifiers)) {
    // First get the pub_dbxref record.
    $values = ['dbxref_id' => $identifiers['dbxref_id']];
    $pub_dbxref = chado_select_record('pub_dbxref', ['pub_id'], $values);

    // Now get the pub.
    if (count($pub_dbxref) > 0) {
      $pub = chado_generate_var('pub', ['pub_id' => $pub_dbxref[0]->pub_id], $options);
    }
    else {
      return FALSE;
    }

  }
  // Else we have a simple case and we can just use chado_generate_var to get
  // the pub.
  else {
    // Try to get the pub.
    $pub = chado_generate_var('pub', $identifiers, $options);
  }

  // Ensure the pub is singular. If it's an array then it is not singular.
  if (is_array($pub)) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: The identifiers did not find a single unique record. Identifiers passed: %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }

  // Report an error if $pub is FALSE since then chado_generate_var has failed.
  elseif ($pub === FALSE) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: Could not find a publication using the identifiers
       provided. Check that the identifiers are correct. Identifiers passed: %identifier.",
      ['%identifier' => print_r($identifiers, TRUE)]
    );
  }

  // Else, as far we know, everything is fine so give them their pub :)
  else {
    return $pub;
  }
}

/**
 * The publication table of Chado only has a unique constraint for the
 * uniquename of the publication, but in reality a publication can be considered
 * unique by a combination of the title, publication type, published year and
 * series name (e.g. journal name or conference name). The site administrator
 * can configure how publications are determined to be unique.  This function
 * uses the configuration specified by the administrator to look for
 * publications that match the details specified by the $pub_details argument
 * and indicates if one ore more publications match the criteria.
 *
 * @param $pub_details
 *   An associative array with details about the publications. The expected
 *   keys
 *   are:
 *     'Title':              The title of the publication.
 *     'Year':               The published year of the publication.
 *     'Publication Type':   An array of publication types. A publication can
 *                           have more than one type.
 *     'Series Name':        The series name of the publication.
 *     'Journal Name':       An alternative to 'Series Name'.
 *     'Conference Name':    An alternative to 'Series Name'.
 *     'Citation':           The publication citation (this is the value saved
 *                           in the pub.uniquename field and must be unique).
 *
 *     If this key is present it will also be checked
 *     'Publication Dbxref': A database cross reference of the form
 *   DB:ACCESSION
 *                           where DB is the name of the database and ACCESSION
 *                           is the unique identifier (e.g PMID:3483139).
 *
 * @return
 *   An array containing the pub_id's of matching publications. Returns an
 *   empty array if no pubs match.
 *
 * @ingroup tripal_pub_api
 */
function chado_publication_exists($pub_details) {

  // First try to find the publication using the accession number if that key
  // exists in the details array.
  if (array_key_exists('Publication Dbxref', $pub_details)) {
    $pub = chado_get_publication(['dbxref' => $pub_details['Publication Dbxref']]);
    if ($pub) {
      return [$pub->pub_id];
    }
  }

  // Make sure the citation is unique.
  if (array_key_exists('Citation', $pub_details)) {
    $pub = chado_get_publication(['uniquename' => $pub_details['Citation']]);
    if ($pub) {
      return [$pub->pub_id];
    }
  }

  // Get the publication type (use the first publication type).
  if (array_key_exists('Publication Type', $pub_details)) {
    $type_name = '';
    if (is_array($pub_details['Publication Type'])) {
      $type_name = $pub_details['Publication Type'][0];
    }
    else {
      $type_name = $pub_details['Publication Type'];
    }
    $identifiers = [
      'name' => $type_name,
      'cv_id' => [
        'name' => 'tripal_pub',
      ],
    ];
    $pub_type = chado_get_cvterm($identifiers);
  }
  else {
    tripal_report_error('tripal_pub', TRIPAL_ERROR,
      "chado_publication_exists(): The Publication Type is a " .
      "required property but is missing", []);
    return [];
  }
  if (!$pub_type) {
    tripal_report_error('tripal_pub', TRIPAL_ERROR,
      "chado_publication_exists(): Cannot find publication type: '%type'",
      ['%type' => $pub_details['Publication Type'][0]]);
    return [];
  }

  // Get the series name.  The pub.series_name field is only 255 chars so we
  // must truncate to be safe.
  $series_name = '';
  if (array_key_exists('Series Name', $pub_details)) {
    $series_name = substr($pub_details['Series Name'], 0, 255);
  }
  if (array_key_exists('Journal Name', $pub_details)) {
    $series_name = substr($pub_details['Journal Name'], 0, 255);
  }
  if (array_key_exists('Conference Name', $pub_details)) {
    $series_name = substr($pub_details['Conference Name'], 0, 255);
  }

  // Make sure the publication is unique using the prefereed import
  // duplication check.
  $import_dups_check = variable_get('tripal_pub_import_duplicate_check', 'title_year_media');
  $pubs = [];
  switch ($import_dups_check) {
    case 'title_year':
      $identifiers = [
        'title' => $pub_details['Title'],
        'pyear' => $pub_details['Year'],
      ];
      $pubs = chado_select_record('pub', ['pub_id'], $identifiers);
      break;
    case 'title_year_type':
      $identifiers = [
        'title' => $pub_details['Title'],
        'pyear' => $pub_details['Year'],
        'type_id' => $pub_type->cvterm_id,
      ];
      $pubs = chado_select_record('pub', ['pub_id'], $identifiers);
      break;
    case 'title_year_media':
      $identifiers = [
        'title' => $pub_details['Title'],
        'pyear' => $pub_details['Year'],
        'series_name' => $series_name,
      ];
      $pubs = chado_select_record('pub', ['pub_id'], $identifiers);
      break;
  }

  $return = [];
  foreach ($pubs as $pub) {
    $return[] = $pub->pub_id;
  }

  return $return;
}


/**
 * Used for autocomplete in forms for identifying for publications.
 *
 * @param $field
 *   The field in the publication to search on.
 * @param $string
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_pub_api
 */
function chado_autocomplete_pub($string = '') {
  $items = [];
  $sql = "
    SELECT pub_id, title, uniquename
    FROM {pub}
    WHERE lower(title) like lower(:str)
    ORDER by title
    LIMIT 25 OFFSET 0
  ";
  $pubs = chado_query($sql, [':str' => $string . '%']);
  while ($pub = $pubs->fetchObject()) {
    $val = $pub->title . " [id:" . $pub->pub_id . "]";
    $items[$val] = $pub->title;
  }

  drupal_json_output($items);
}


/**
 * Imports a singe publication specified by a remote database cross reference.
 *
 * @param $pub_dbxref
 *   The unique database ID for the record to update.  This value must
 *   be of the format DB_NAME:ACCESSION where DB_NAME is the name of the
 *   database (e.g. PMID or AGL) and the ACCESSION is the unique identifier
 *   for the record in the database.
 * @param $do_contact
 *   Set to TRUE if authors should automatically have a contact record added
 *   to Chado.
 * @param $publish
 *   Set to TRUE if publications should be published after import.  For Tripal
 *   v3 this value can be set to the string 'sync' or 'both' in the event that
 *   the site is in "legacy" mode.  Setting this value to 'sync' will create
 *   nodes, setting to 'both' will create nodes and entities.  If set to TRUE
 *   only entities are created.
 * @param $do_update
 *   If set to TRUE then the publication will be updated if it already exists
 *   in the database.
 *
 * @ingroup tripal_pub_api
 */
function chado_import_pub_by_dbxref($pub_dbxref, $do_contact = FALSE,
                                    $publish = TRUE, $do_update = TRUE) {

  $num_to_retrieve = 1;
  $pager_id = 0;
  $page = 0;
  $num_pubs = 0;
  $pub_id = NULL;

  module_load_include('inc', 'tripal_chado', 'includes/loaders/tripal_chado.pub_importers');

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog but we do for the job and to
  // the terminal
  $message_type = 'pub_import';
  $message_opts = [
    'watchdog' == FALSE,
    'print' => TRUE,
  ];

  $message = "Importing of publications is performed using a database transaction. " .
    "If the load fails or is terminated prematurely then the entire set of " .
    "insertions/updates is rolled back and will not be found in the database";
  tripal_report_error($message_type, TRIPAL_INFO, $message, [], $message_opts);


  $transaction = db_transaction();
  try {
    if (preg_match('/^(.*?):(.*?)$/', $pub_dbxref, $matches)) {
      $dbname = $matches[1];
      $accession = $matches[2];

      $criteria = [
        'num_criteria' => 1,
        'remote_db' => $dbname,
        'criteria' => [
          '1' => [
            'search_terms' => "$dbname:$accession",
            'scope' => 'id',
            'operation' => '',
            'is_phrase' => 0,
          ],
        ],
      ];
      $remote_db = $criteria['remote_db'];
      $results = tripal_get_remote_pubs($remote_db, $criteria, $num_to_retrieve, $page);
      $pubs = $results['pubs'];
      $search_str = $results['search_str'];
      $total_records = $results['total_records'];
      tripal_pub_add_publications($pubs, $do_contact, $do_update);
    }

    // Publish as requested by the caller.
    _chado_execute_pub_importer_publish($publish, NULL, $message_type, $message_opts);

  } catch (Exception $e) {
    $transaction->rollback();
    print "\n"; // make sure we start errors on new line
    watchdog_exception('T_pub_import', $e);
    print "FAILED: Rolling back database changes...\n";
    return;
  }
}

/**
 * Imports all publications for all active import setups.
 *
 * @param $report_email
 *   A list of email address, separated by commas, that should be notified
 *   once importing has completed.
 * @param $publish
 *   Set to TRUE if publications should be published after import.  For Tripal
 *   v3 this value can be set to the string 'sync' or 'both' in the event that
 *   the site is in "legacy" mode.  Setting this value to 'sync' will create
 *   nodes, setting to 'both' will create nodes and entities.  If set to TRUE
 *   only entities are created.
 * @param $do_update
 *   If set to TRUE then publications that already exist in the Chado database
 *   will be updated, whereas if FALSE only new publications will be added.
 *
 * @ingroup tripal_pub_api
 */
function chado_execute_active_pub_importers($report_email = FALSE,
                                            $publish = TRUE, $do_update = FALSE) {

  $report = [];
  $report['error'] = [];
  $report['inserted'] = [];
  $report['skipped'] = [];
  $report['updated'] = [];

  // Get all of the loaders.
  $args = [];
  $sql = "SELECT * FROM {tripal_pub_import} WHERE disabled = 0 ";
  $importers = db_query($sql, $args);
  $do_contact = FALSE;
  while ($import = $importers->fetchObject()) {
    $importer_report = chado_execute_pub_importer($import->pub_import_id, $publish, $do_update);
    foreach ($importer_report as $action => $pubs) {
      $report[$action] = array_merge($report[$action], $pubs);
    }
  }

  $site_email = variable_get('site_mail', '');
  $params = [
    'report' => $report,
  ];
  drupal_mail('tripal_chado', 'import_report', $report_email, language_default(), $params, $site_email, TRUE);
  print "Done.\n";
}


/**
 * Imports all publications for a given publication import setup.
 *
 * @param $import_id
 *   The ID of the import setup to use
 * @param $publish
 *   Set to TRUE if publications should be published after import.  For Tripal
 *   v3 this value can be set to the string 'sync' or 'both' in the event that
 *   the site is in "legacy" mode.  Setting this value to 'sync' will create
 *   nodes, setting to 'both' will create nodes and entities.  If set to TRUE
 *   only entities are created.
 * @param $do_update
 *   If set to TRUE then publications that already exist in the Chado database
 *   will be updated, whereas if FALSE only new publications will be added.
 * @param $job
 *   The jobs management object for the job if this function is run as a job.
 *   This argument is added by Tripal during a job run and is not needed if
 *   this function is run directly.
 *
 * @return
 *   Returns an array containing the number of publications that were
 *   inserted, updated, skipped and which had an error during import.
 *
 * @ingroup tripal_pub
 */
function chado_execute_pub_importer($import_id, $publish = TRUE,
                                    $do_update = FALSE, $job = NULL) {

  // Holds the list of imported pubs which includes their ID and Citation.
  $report = [];
  $report['error'] = [];
  $report['inserted'] = [];
  $report['skipped'] = [];
  $report['updated'] = [];

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog but we do for the job and to
  // the terminal
  $message_type = 'pub_import';
  $message_opts = [
    'watchdog' == FALSE,
    'job' => $job,
    'print' => TRUE,
  ];

  $message = "Importing of publications for this importer is performed using a database transaction. " .
    "If the load fails or is terminated prematurely then the entire set of " .
    "insertions/updates is rolled back and will not be found in the database";
  tripal_report_error($message_type, TRIPAL_INFO, $message, [], $message_opts);


  // start the transaction
  $transaction = db_transaction();

  try {
    $page = 0;
    $do_contact = FALSE;
    $num_to_retrieve = 100;

    // get all of the loaders
    $args = [':import_id' => $import_id];
    $sql = "SELECT * FROM {tripal_pub_import} WHERE pub_import_id = :import_id ";
    $import = db_query($sql, $args)->fetchObject();

    tripal_report_error($message_type, TRIPAL_INFO,
      "Executing Importer: !name.", ['!name' => $import->name], $message_opts);

    $criteria = unserialize($import->criteria);
    $remote_db = $criteria['remote_db'];
    $total_pubs = 0;

    // Loop until we have a $pubs array that does not have
    // our requested numer of records.  This means we've hit the end
    do {
      // retrieve the pubs for this page. We'll retrieve 100 at a time
      $npages = isset($num_pubs)?(intval($num_pubs/$num_to_retrieve)+1):'?';  // will be 0 to 99 in last page
      tripal_report_error($message_type, TRIPAL_INFO,
        "Page ".($page+1)." of $npages. Querying !remote_db for up to !num pubs that match the criteria.",
        [
          '!num' => $num_to_retrieve,
          '!remote_db' => $remote_db,
        ], $message_opts);
      $results = tripal_get_remote_pubs($remote_db, $criteria, $num_to_retrieve, $page);
      $pubs = $results['pubs'];
      $num_pubs = $results['total_records'];
      $total_pubs += $num_pubs;
      tripal_report_error($message_type, TRIPAL_INFO,
        "Found %num publications.",
        ['%num' => $num_pubs], $message_opts);

      $subset_report = tripal_pub_add_publications($pubs, $import->do_contact, $do_update, $job);
      $countpubs = count($pubs);  // the following merge resets count($pubs) so save it
      foreach ($subset_report as $action => $pubs) {
        $report[$action] = array_merge($report[$action], $pubs);
      }
      $page++;
    } while ($countpubs == $num_to_retrieve);

    // Publish as requested by the caller.
    _chado_execute_pub_importer_publish($publish, $job, $message_type, $message_opts);

    if ($job) {
      $job->setProgress(100);
    }
  } catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('T_pub_import', $e);
    tripal_report_error($message_type, TRIPAL_ERROR,
      "Rolling back database changes... !message",
      ['!message' => $e->getMessage()], $message_opts);
    return FALSE;
  }

  tripal_report_error($message_type, TRIPAL_INFO,
    "Done.", [], $message_opts);

  return $report;
}


/**
 * A helper function to dermine if imported publications should be published.
 *
 * It supports backwards compatibility with Tripal v2 legacy mode.
 *
 * @param $publish
 *   Set to TRUE if publications should be published after import.  For Tripal
 *   v3 this value can be set to the string 'sync' or 'both' in the event that
 *   the site is in "legacy" mode.  Setting this value to 'sync' will create
 *   nodes, setting to 'both' will create nodes and entities.  If set to TRUE
 *   only entities are created.
 */
function _chado_execute_pub_importer_publish($publish, $job, $message_type, $message_opts) {

  // If the user wants to publish then do so.
  if ($publish === TRUE or $publish === 'both') {

    // Get the bundle for the Publication content type.
    $bundle_term = tripal_load_term_entity([
      'vocabulary' => 'TPUB',
      'accession' => '0000002',
    ]);
    $bundle = tripal_load_bundle_entity(['term_id' => $bundle_term->id]);
    if ($bundle) {
      tripal_report_error($message_type, TRIPAL_INFO,
        "Publishing publications with Drupal...", [], $message_opts);
      chado_publish_records(['bundle_name' => $bundle->name], $job);
    }
    // Note: we won't publish contacts as Tripal v2 did because there is
    // no consisten way to do that. Each site my use a different term for
    // different contact content types (e.g. all as one 'Contact' type or
    // specific such as 'Person', 'Organization', etc.).
  }

  // For backwords compatibility with legacy module do a sync.
  if ($publish === 'sync' or $publish === 'both') {
    if (module_exists('tripal_pub')) {
      tripal_report_error($message_type, TRIPAL_INFO,
        "Syncing publications with Drupal...", [], $message_opts);
      chado_node_sync_records('pub');
      if ($import->do_contact) {
        tripal_report_error($message_type, TRIPAL_INFO,
          "Syncing contacts with Drupal...", [], $message_opts);
        chado_node_sync_records('contact');
      }
    }
  }
}


/**
 * Updates publication records.
 *
 * Updates publication records that currently exist in the Chado pub table
 * with the most recent data in the remote database.
 *
 * @param $do_contact
 *   Set to TRUE if authors should automatically have a contact record added
 *   to Chado. Contacts are added using the name provided by the remote
 *   database.
 * @param $dbxref
 *   The unique database ID for the record to update.  This value must
 *   be of the format DB_NAME:ACCESSION where DB_NAME is the name of the
 *   database (e.g. PMID or AGL) and the ACCESSION is the unique identifier
 *   for the record in the database.
 * @param $db
 *   The name of the remote database to update.  If this value is provided and
 *   no dbxref then all of the publications currently in the Chado database
 *   for this remote database will be updated.
 * @param $publish
 *   Set to TRUE if publications should be published after import.  For Tripal
 *   v3 this value can be set to the string 'sync' or 'both' in the event that
 *   the site is in "legacy" mode.  Setting this value to 'sync' will create
 *   nodes, setting to 'both' will create nodes and entities.  If set to TRUE
 *   only entities are created.
 *
 * @ingroup tripal_pub_api
 */
function chado_reimport_publications($do_contact = FALSE, $dbxref = NULL,
                                     $db = NULL, $publish = TRUE) {

  // These are options for the tripal_report_error function. We do not
  // want to log messages to the watchdog but we do for the job and to
  // the terminal
  $message_type = 'pub_import';
  $message_opts = [
    'watchdog' == FALSE,
    'print' => TRUE,
  ];

  $message = "Importing of publications for this importer is performed using a database transaction. " .
    "If the load fails or is terminated prematurely then the entire set of " .
    "insertions/updates is rolled back and will not be found in the database";
  tripal_report_error($message_type, TRIPAL_INFO, $message, [], $message_opts);

  $transaction = db_transaction();
  try {

    // Get a list of all publications by their Dbxrefs that have supported
    // databases.
    $sql = "
      SELECT DB.name as db_name, DBX.accession
      FROM {pub} P
        INNER JOIN {pub_dbxref} PDBX ON P.pub_id = PDBX.pub_id
        INNER JOIN {dbxref} DBX      ON DBX.dbxref_id = PDBX.dbxref_id
        INNER JOIN {db} DB           ON DB.db_id = DBX.db_id
    ";
    $args = [];
    if ($dbxref and preg_match('/^(.*?):(.*?)$/', $dbxref, $matches)) {
      $dbname = $matches[1];
      $accession = $matches[2];
      $sql .= "WHERE DBX.accession = :accession and DB.name = :dbname ";
      $args[':accession'] = $accession;
      $args[':dbname'] = $dbname;
    }
    elseif ($db) {
      $sql .= " WHERE DB.name = :dbname ";
      $args[':dbname'] = $db;
    }
    $sql .= "ORDER BY DB.name, P.pub_id";
    $results = chado_query($sql, $args);

    $num_to_retrieve = 100;
    $i = 0;                 // count the number of IDs. When we hit $num_to_retrieve we'll do the query.
    $curr_db = '';          // keeps track of the current current database.
    $ids = [];         // the list of IDs for the database.
    $search = [];      // the search array passed to the search function.

    // Iterate through the pub IDs.
    while ($pub = $results->fetchObject()) {
      $accession = $pub->accession;
      $remote_db = $pub->db_name;

      // Here we need to only update publications for databases we support.
      $supported_dbs = variable_get('tripal_pub_supported_dbs', []);
      if (!in_array($remote_db, $supported_dbs)) {
        continue;
      }
      $search = [
        'num_criteria' => 1,
        'remote_db' => $remote_db,
        'criteria' => [
          '1' => [
            'search_terms' => "$remote_db:$accession",
            'scope' => 'id',
            'operation' => '',
            'is_phrase' => 0,
          ],
        ],
      ];
      $qresults = tripal_get_remote_pubs($remote_db, $search, 1, 0);
      $pubs = $qresults['pubs'];
      tripal_pub_add_publications($pubs, $do_contact, TRUE);

      $i++;
    }

    // Publish as requested by the caller.
    _chado_execute_pub_importer_publish($publish, NULL, $message_type, $message_opts);
  } catch (Exception $e) {
    $transaction->rollback();
    watchdog_exception('T_pub_import', $e);
    tripal_report_error($message_type, TRIPAL_ERROR,
      "Rolling back database changes... !message",
      ['!message' => $e->getMessage()], $message_opts);
    return;
  }
  print "Done.\n";
}

/**
 * Launch the Tripal job to generate citations.
 *
 * This function will recreate citations for all publications currently
 * loaded into Tripal.  This is useful to create a consistent format for
 * all citations.
 *
 * @param $options
 *  Options pertaining to what publications to generate citations for.
 *  One of the following must be present:
 *   - all: Create and replace citation for all pubs.
 *   - new: Create citation for pubs that don't already have one.
 *
 * @ingroup tripal_pub_api
 */
function chado_pub_create_citations($options) {
  $skip_existing = TRUE;
  $sql = "
    SELECT cvterm_id
    FROM {cvterm}
    WHERE
      name = 'Citation' AND
      cv_id = (SELECT cv_id FROM {cv} WHERE name = 'tripal_pub')
  ";
  $citation_type_id = chado_query($sql)->fetchField();

  // Create and replace citation for all pubs.
  if ($options == 'all') {
    $sql = "SELECT pub_id FROM {pub} P WHERE pub_id <> 1";
    $skip_existing = FALSE;
  }
  // Create citation for pubs that don't already have one.
  else {
    if ($options == 'new') {
      $sql = "
      SELECT pub_id
      FROM {pub} P
      WHERE
        (SELECT value
         FROM {pubprop} PB
         WHERE type_id = :type_id AND P.pub_id = PB.pub_id AND rank = 0) IS NULL
        AND  pub_id <> 1
    ";
      $skip_existing = TRUE;
    }
  }

  $result = chado_query($sql, [':type_id' => $citation_type_id]);
  $counter_updated = 0;
  $counter_generated = 0;
  while ($pub = $result->fetchObject()) {
    $pub_arr = tripal_pub_get_publication_array($pub->pub_id, $skip_existing);
    if ($pub_arr) {
      $citation = chado_pub_create_citation($pub_arr);
      print $citation . "\n\n";
      // Replace if citation exists. This condition is never TRUE if
      // $skip_existing is TRUE.
      if ($pub_arr['Citation']) {
        $sql = "
          UPDATE {pubprop} SET value = :value
          WHERE pub_id = :pub_id  AND type_id = :type_id AND rank = :rank
        ";
        chado_query($sql, [
          ':value' => $citation,
          ':pub_id' => $pub->pub_id,
          ':type_id' => $citation_type_id,
          ':rank' => 0,
        ]);
        $counter_updated++;
        // Generate a new citation.
      }
      else {
        $sql = "
          INSERT INTO {pubprop} (pub_id, type_id, value, rank)
          VALUES (:pub_id, :type_id, :value, :rank)
        ";
        chado_query($sql, [
          ':pub_id' => $pub->pub_id,
          ':type_id' => $citation_type_id,
          ':value' => $citation,
          ':rank' => 0,
        ]);
        $counter_generated++;
      }
    }
  }
  print "$counter_generated citations generated. $counter_updated citations updated.\n";
}


/**
 * This function generates citations for publications.  It requires
 * an array structure with keys being the terms in the Tripal
 * publication ontology.  This function is intended to be used
 * for any function that needs to generate a citation.
 *
 * @param $pub
 *   An array structure containing publication details where the keys
 *   are the publication ontology term names and values are the
 *   corresponding details.  The pub array can contain the following
 *   keys with corresponding values:
 *     - Publication Type:  an array of publication types. a publication can
 *       have more than one type.
 *     - Authors: a  string containing all of the authors of a publication.
 *     - Journal Name:  a string containing the journal name.
 *     - Journal Abbreviation: a string containing the journal name
 *   abbreviation.
 *     - Series Name: a string containing the series (e.g. conference
 *       proceedings) name.
 *     - Series Abbreviation: a string containing the series name abbreviation
 *     - Volume: the serives volume number.
 *     - Issue: the series issue number.
 *     - Pages: the page numbers for the publication.
 *     - Publication Date:  A date in the format "Year Month Day".
 *
 * @return
 *   A text string containing the citation.
 *
 * @ingroup tripal_pub_api
 */
function chado_pub_create_citation($pub) {
  $citation = '';
  $pub_type = '';

  // An article may have more than one publication type. For example,
  // a publication type can be 'Journal Article' but also a 'Clinical Trial'.
  // Therefore, we need to select the type that makes most sense for
  // construction of the citation. Here we'll iterate through them all
  // and select the one that matches best.
  if (is_array($pub['Publication Type'])) {
    foreach ($pub['Publication Type'] as $ptype) {
      if ($ptype == 'Journal Article') {
        $pub_type = $ptype;
        break;
      }
      else {
        if ($ptype == 'Conference Proceedings') {
          $pub_type = $ptype;
          break;
        }
        else {
          if ($ptype == 'Review') {
            $pub_type = $ptype;
            break;
          }
          else {
            if ($ptype == 'Book') {
              $pub_type = $ptype;
              break;
            }
            else {
              if ($ptype == 'Letter') {
                $pub_type = $ptype;
                break;
              }
              else {
                if ($ptype == 'Book Chapter') {
                  $pub_type = $ptype;
                  break;
                }
                else {
                  if ($ptype == "Research Support, Non-U.S. Gov't") {
                    $pub_type = $ptype;
                    // We don't break because if the article is also a Journal Article
                    // we prefer that type.
                  }
                }
              }
            }
          }
        }
      }
    }
    // If we don't have a recognized publication type, then just use the
    // first one in the list.
    if (!$pub_type) {
      $pub_type = $pub['Publication Type'][0];
    }
  }
  else {
    $pub_type = $pub['Publication Type'];
  }
  //----------------------
  // Journal Article
  //----------------------
  if ($pub_type == 'Journal Article') {
    if (array_key_exists('Authors', $pub)) {
      $citation = $pub['Authors'] . '. ';
    }

    $citation .= $pub['Title'] . '. ';

    if (array_key_exists('Journal Name', $pub)) {
      $citation .= $pub['Journal Name'] . '. ';
    }
    elseif (array_key_exists('Journal Abbreviation', $pub)) {
      $citation .= $pub['Journal Abbreviation'] . '. ';
    }
    elseif (array_key_exists('Series Name', $pub)) {
      $citation .= $pub['Series Name'] . '. ';
    }
    elseif (array_key_exists('Series Abbreviation', $pub)) {
      $citation .= $pub['Series Abbreviation'] . '. ';
    }
    if (array_key_exists('Publication Date', $pub)) {
      $citation .= $pub['Publication Date'];
    }
    elseif (array_key_exists('Year', $pub)) {
      $citation .= $pub['Year'];
    }
    if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
      $citation .= '; ';
    }
    if (array_key_exists('Volume', $pub)) {
      $citation .= $pub['Volume'];
    }
    if (array_key_exists('Issue', $pub)) {
      $citation .= '(' . $pub['Issue'] . ')';
    }
    if (array_key_exists('Pages', $pub)) {
      if (array_key_exists('Volume', $pub)) {
        $citation .= ':';
      }
      $citation .= $pub['Pages'];
    }
    $citation .= '.';
  }
  //----------------------
  // Review
  //----------------------
  else {
    if ($pub_type == 'Review') {
      if (array_key_exists('Authors', $pub)) {
        $citation = $pub['Authors'] . '. ';
      }

      $citation .= $pub['Title'] . '. ';

      if (array_key_exists('Journal Name', $pub)) {
        $citation .= $pub['Journal Name'] . '. ';
      }
      elseif (array_key_exists('Journal Abbreviation', $pub)) {
        $citation .= $pub['Journal Abbreviation'] . '. ';
      }
      elseif (array_key_exists('Series Name', $pub)) {
        $citation .= $pub['Series Name'] . '. ';
      }
      elseif (array_key_exists('Series Abbreviation', $pub)) {
        $citation .= $pub['Series Abbreviation'] . '. ';
      }
      if (array_key_exists('Publication Date', $pub)) {
        $citation .= $pub['Publication Date'];
      }
      elseif (array_key_exists('Year', $pub)) {
        $citation .= $pub['Year'];
      }
      if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
        $citation .= '; ';
      }
      if (array_key_exists('Volume', $pub)) {
        $citation .= $pub['Volume'];
      }
      if (array_key_exists('Issue', $pub)) {
        $citation .= '(' . $pub['Issue'] . ')';
      }
      if (array_key_exists('Pages', $pub)) {
        if (array_key_exists('Volume', $pub)) {
          $citation .= ':';
        }
        $citation .= $pub['Pages'];
      }
      $citation .= '.';
    }
    //----------------------
    // Research Support, Non-U.S. Gov't
    //----------------------
    elseif ($pub_type == "Research Support, Non-U.S. Gov't") {
      if (array_key_exists('Authors', $pub)) {
        $citation = $pub['Authors'] . '. ';
      }

      $citation .= $pub['Title'] . '. ';

      if (array_key_exists('Journal Name', $pub)) {
        $citation .= $pub['Journal Name'] . '. ';
      }
      if (array_key_exists('Publication Date', $pub)) {
        $citation .= $pub['Publication Date'];
      }
      elseif (array_key_exists('Year', $pub)) {
        $citation .= $pub['Year'];
      }
      $citation .= '.';
    }
    //----------------------
    // Letter
    //----------------------
    elseif ($pub_type == 'Letter') {
      if (array_key_exists('Authors', $pub)) {
        $citation = $pub['Authors'] . '. ';
      }

      $citation .= $pub['Title'] . '. ';
      if (array_key_exists('Journal Name', $pub)) {
        $citation .= $pub['Journal Name'] . '. ';
      }
      elseif (array_key_exists('Journal Abbreviation', $pub)) {
        $citation .= $pub['Journal Abbreviation'] . '. ';
      }
      elseif (array_key_exists('Series Name', $pub)) {
        $citation .= $pub['Series Name'] . '. ';
      }
      elseif (array_key_exists('Series Abbreviation', $pub)) {
        $citation .= $pub['Series Abbreviation'] . '. ';
      }
      if (array_key_exists('Publication Date', $pub)) {
        $citation .= $pub['Publication Date'];
      }
      elseif (array_key_exists('Year', $pub)) {
        $citation .= $pub['Year'];
      }
      if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
        $citation .= '; ';
      }
      if (array_key_exists('Volume', $pub)) {
        $citation .= $pub['Volume'];
      }
      if (array_key_exists('Issue', $pub)) {
        $citation .= '(' . $pub['Issue'] . ')';
      }
      if (array_key_exists('Pages', $pub)) {
        if (array_key_exists('Volume', $pub)) {
          $citation .= ':';
        }
        $citation .= $pub['Pages'];
      }
      $citation .= '.';
    }
    //-----------------------
    // Conference Proceedings
    //-----------------------
    elseif ($pub_type == 'Conference Proceedings') {
      if (array_key_exists('Authors', $pub)) {
        $citation = $pub['Authors'] . '. ';
      }

      $citation .= $pub['Title'] . '. ';
      if (array_key_exists('Conference Name', $pub)) {
        $citation .= $pub['Conference Name'] . '. ';
      }
      elseif (array_key_exists('Series Name', $pub)) {
        $citation .= $pub['Series Name'] . '. ';
      }
      elseif (array_key_exists('Series Abbreviation', $pub)) {
        $citation .= $pub['Series Abbreviation'] . '. ';
      }
      if (array_key_exists('Publication Date', $pub)) {
        $citation .= $pub['Publication Date'];
      }
      elseif (array_key_exists('Year', $pub)) {
        $citation .= $pub['Year'];
      }
      if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
        $citation .= '; ';
      }
      if (array_key_exists('Volume', $pub)) {
        $citation .= $pub['Volume'];
      }
      if (array_key_exists('Issue', $pub)) {
        $citation .= '(' . $pub['Issue'] . ')';
      }
      if (array_key_exists('Pages', $pub)) {
        if (array_key_exists('Volume', $pub)) {
          $citation .= ':';
        }
        $citation .= $pub['Pages'];
      }
      $citation .= '.';
    }
    //-----------------------
    // Default
    //-----------------------
    else {
      if (array_key_exists('Authors', $pub)) {
        $citation = $pub['Authors'] . '. ';
      }
      $citation .= $pub['Title'] . '. ';
      if (array_key_exists('Series Name', $pub)) {
        $citation .= $pub['Series Name'] . '. ';
      }
      elseif (array_key_exists('Series Abbreviation', $pub)) {
        $citation .= $pub['Series Abbreviation'] . '. ';
      }
      if (array_key_exists('Publication Date', $pub)) {
        $citation .= $pub['Publication Date'];
      }
      elseif (array_key_exists('Year', $pub)) {
        $citation .= $pub['Year'];
      }
      if (array_key_exists('Volume', $pub) or array_key_exists('Issue', $pub) or array_key_exists('Pages', $pub)) {
        $citation .= '; ';
      }
      if (array_key_exists('Volume', $pub)) {
        $citation .= $pub['Volume'];
      }
      if (array_key_exists('Issue', $pub)) {
        $citation .= '(' . $pub['Issue'] . ')';
      }
      if (array_key_exists('Pages', $pub)) {
        if (array_key_exists('Volume', $pub)) {
          $citation .= ':';
        }
        $citation .= $pub['Pages'];
      }
      $citation .= '.';
    }
  }

  return $citation;
}

/**
 * Retrieves the minimal information to uniquely describe any publication.
 *
 * The returned array is an associative array where the keys are
 * the controlled vocabulary terms in the form [vocab]:[accession].
 *
 * @param $pub
 *   A publication object as created by chado_generate_var().
 *
 * @return
 *   An array with the following keys:  'Citation', 'Abstract', 'Authors',
 *   'URL'. All keys are term names in the Tripal Publication Ontology :TPUB.
 *
 * @ingroup tripal_pub_api
 */
function chado_get_minimal_pub_info($pub) {
  if (!$pub) {
    return [];
  }

  // Chado has a null pub as default.  We don't return anything for this.
  if (isset($pub->uniquename) && $pub->uniquename == 'null') {
    return [];
  }

  // Expand the title.
  $pub = chado_expand_var($pub, 'field', 'pub.title');
  $pub = chado_expand_var($pub, 'field', 'pub.volumetitle');

  // Get the abstract.
  $values = [
    'pub_id' => $pub->pub_id,
    'type_id' => [
      'name' => 'Abstract',
    ],
  ];
  $options = [
    'include_fk' => [
    ],
  ];
  $abstract = chado_generate_var('pubprop', $values, $options);
  $abstract_text = '';
  if($abstract) {
    $abstract = chado_expand_var($abstract, 'field', 'pubprop.value');
    if ($abstract) {
      $abstract_text = htmlspecialchars($abstract->value);
    }
  }

  // Get the author list.
  $values = [
    'pub_id' => $pub->pub_id,
    'type_id' => [
      'name' => 'Authors',
    ],
  ];
  $options = [
    'include_fk' => [
    ],
  ];
  $authors = chado_generate_var('pubprop', $values, $options);
  $authors_list = 'N/A';
  if($authors) {
    $authors = chado_expand_var($authors, 'field', 'pubprop.value');
    if ($authors) {
      $authors_list = $authors->value;
    }
  }

  // Get the first database cross-reference with a url.
  $options = ['return_array' => 1];
  $pub = chado_expand_var($pub, 'table', 'pub_dbxref', $options);
  $dbxref = NULL;
  if ($pub->pub_dbxref) {
    foreach ($pub->pub_dbxref as $index => $pub_dbxref) {
      if ($pub_dbxref->dbxref_id->db_id->urlprefix) {
        $dbxref = $pub_dbxref->dbxref_id;
      }
    }
  }

  // Get the URL.
  $values = [
    'pub_id' => $pub->pub_id,
    'type_id' => [
      'name' => 'URL',
    ],
  ];
  $options = [
    'return_array' => 1,
    'include_fk' => [],
  ];
  $url = '';
  $urls = chado_generate_var('pubprop', $values, $options);
  if ($urls) {
    $urls = chado_expand_var($urls, 'field', 'pubprop.value');
    if (count($urls) > 0) {
      $url = $urls[0]->value;
    }
  }

  // Get the list of database cross references.
  $values = [
    'pub_id' => $pub->pub_id,
  ];
  $options = [
    'return_array' => 1,
  ];
  $pub_dbxrefs = chado_generate_var('pub_dbxref', $values, $options);
  $dbxrefs = [];
  foreach ($pub_dbxrefs as $pub_dbxref) {
    $dbxrefs[] = $pub_dbxref->dbxref_id->db_id->name . ':' . $pub_dbxref->dbxref_id->accession;
  }

  // Get the citation.
  $values = [
    'pub_id' => $pub->pub_id,
    'type_id' => [
      'name' => 'Citation',
    ],
  ];
  $options = [
    'include_fk' => [
    ],
  ];
  $citation = chado_generate_var('pubprop', $values, $options);
  if ($citation) {
    $citation = chado_expand_var($citation, 'field', 'pubprop.value');
    $citation = $citation->value;
  }
  else {
    $pub_info = [
      'Title' => $pub->title,
      'Publication Type' => $pub->type_id->name,
      'Authors' => $authors_list,
      'Series Name' => $pub->series_name,
      'Volume' => $pub->volume,
      'Issue' => $pub->issue,
      'Pages' => $pub->pages,
      'Publication Date' => $pub->pyear,
    ];
    $citation = chado_pub_create_citation($pub_info);
  }

  return [
    'TPUB:0000039' => $pub->title,
    'TPUB:0000003' => $citation,
    'TPUB:0000050' => $abstract_text,
    'TPUB:0000047' => $authors_list,
    'TPUB:0000052' => $url,
    'SBO:0000554' => $dbxrefs,
  ];
}
