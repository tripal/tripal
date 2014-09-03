<?php
/**
 * @file
 * Provides an application programming interface (API) to manage chado publications
 */

/**
 * @defgroup tripal_pub_api Publication Module API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage chado publications
 *
 * @stephen add documentation here for how to add a new importer.
 *
 * @}
 */


/**
 * Retrieves a chado publication array
 *
 * @param $identifier
 *   An array used to uniquely identify a publication. This array has the same
 *   format as that used by the chado_generate_var(). The following keys can be
 *   useful for uniquely identifying a publication as they should be unique:
 *    - pub_id: the chado pub.pub_id primary key
 *    - nid: the drupal nid of the publication
 *    - uniquename: A value to matach with the pub.uniquename field
 *   There are also some specially handled keys. They are:
 *    - property: An array describing the property to select records for. It
 *      should at least have either a 'type_name' key (if unique across cvs) or
 *      'type_id' key. Other supported keys include: 'cv_id', 'cv_name' (of the type),
 *      'value' and 'rank'
 *    - dbxref: The database cross reference accession.  It should be in the form
 *        DB:ACCESSION, where DB is the database name and ACCESSION is the
 *        unique publication identifier (e.g. PMID:4382934)
 *    - dbxref_id:  The dbxref.dbxref_id of the publication.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function definition for
 *       additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values passed into
 *   chado_select_record(). It should fully specify the pub record to be returned.
 *
 * @return
 *   If a singe publication is retreived using the identifiers, then a publication
 *   array will be returned.  The array is of the same format returned by the
 *   chado_generate_var() function. Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_pub_api
 */
function tripal_get_publication($identifiers, $options = array()) {

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: The identifier passed in is expected to be an array with the key
       matching a column name in the pub table (ie: pub_id or name). You passed in %identifier.",
      array('%identifier'=> print_r($identifiers, TRUE))
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: You did not pass in anything to identify the publication you want. The identifier
       is expected to be an array with the key matching a column name in the pub table
       (ie: pub_id or name). You passed in %identifier.",
      array('%identifier'=> print_r($identifiers, TRUE))
    );
  }

  // If one of the identifiers is property then use chado_get_record_with_property()
  if (array_key_exists('property', $identifiers)) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $pub = chado_get_record_with_property(
      array('table' => 'pub', 'base_records' => $identifiers),
      array('type_name' => $property), 
      $options
    );
  }
  elseif (array_key_exists('dbxref', $identifiers)) {
    if(preg_match('/^(.*?):(.*?)$/', $identifiers['dbxref'], $matches)) {
      $dbname = $matches[1];
      $accession = $matches[2];

      $values = array(
        'dbxref_id' => array (
          'accession' => $accession,
          'db_id' => array(
            'name' => $dbname
          ),
        ),
      );
      $pub_dbxref = chado_select_record('pub_dbxref', array('pub_id'), $values);
      if (count($pub_dbxref) > 0) {
        $pub = chado_generate_var('pub', array('pub_id' => $pub_dbxref[0]->pub_id), $options);
      }
      else {
        return FALSE;
      }
    }
    else {
      tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
        "chado_get_publication: The dbxref identifier is not correctly formatted.",
        array('%identifier'=> print_r($identifiers, TRUE))
      );
    }
  }
  elseif (array_key_exists('dbxref_id', $identifiers)) {
    // first get the pub_dbxref record
    $values = array('dbxref_id' => $identifiers['dbxref_id']);
    $pub_dbxref = chado_select_record('pub_dbxref', array('pub_id'), $values);

    // now get the pub
    if (count($pub_dbxref) > 0) {
      $pub = chado_generate_var('pub', array('pub_id' => $pub_dbxref[0]->pub_id), $options);
    }
    else {
      return FALSE;
    }

  }
  // Else we have a simple case and we can just use chado_generate_var to get the pub
  else {
    // Try to get the pub
    $pub = chado_generate_var('pub', $identifiers, $options);
  }

  // Ensure the pub is singular. If it's an array then it is not singular
  if (is_array($pub)) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: The identifiers did not find a single unique record. Identifiers passed: %identifier.",
      array('%identifier'=> print_r($identifiers, TRUE))
    );
  }

  // Report an error if $pub is FALSE since then chado_generate_var has failed
  elseif ($pub === FALSE) {
    tripal_report_error('tripal_pub_api', TRIPAL_ERROR,
      "chado_get_publication: Could not find a publication using the identifiers
       provided. Check that the identifiers are correct. Identifiers passed: %identifier.",
      array('%identifier'=> print_r($identifiers, TRUE))
    );
  }

  // Else, as far we know, everything is fine so give them their pub :)
  else {
    return $pub;
  }
}
/**
 * The publication table of Chado only has a unique constraint for the
 * uniquename of the publiation, but in reality a publication can be considered
 * unique by a combination of the title, publication type, published year and
 * series name (e.g. journal name or conference name). The site administrator
 * can configure how publications are determined to be unique.  This function
 * uses the configuration specified by the administrator to look for publications
 * that match the details specified by the $pub_details argument
 * and indicates if one ore more publications match the criteria.
 *
 * @param $pub_details
 *   An associative array with details about the publications. The expected keys
 *   are:
 *     'Title':              The title of the publication
 *     'Year':               The published year of the publication
 *     'Publication Type':   An array of publication types. A publication can have more than one type.
 *     'Series Name':        The series name of the publication
 *     'Journal Name':       An alternative to 'Series Name'
 *     'Conference Name':    An alternative to 'Series Name'
 *     'Citation':           The publication citation (this is the value saved in the pub.uniquename field and must be unique)
 *   If this key is present it will also be checked
 *     'Publication Dbxref': A database cross reference of the form DB:ACCESSION where DB is the name
 *                           of the database and ACCESSION is the unique identifier (e.g PMID:3483139)
 *
 * @return
 *   An array containing the pub_id's of matching publications. Returns an
 *   empty array if no pubs match
 *
 * @ingroup tripal_pub_api
 */
function tripal_publication_exists($pub_details) {

  // first try to find the publication using the accession number if that key exists in the details array
  if (array_key_exists('Publication Dbxref', $pub_details)) {
    $pub = tripal_get_publication(array('dbxref' => $pub_details['Publication Dbxref']));
    if($pub) {
      return array($pub->pub_id);
    }
  }

  // make sure the citation is unique
  if (array_key_exists('Citation', $pub_details)) {
    $pub = tripal_get_publication(array('uniquename' => $pub_details['Citation']));
    if($pub) {
      return array($pub->pub_id);
    }
  }

  // get the publication type (use the first publication type)
  if (array_key_exists('Publication Type', $pub_details)) {
    $type_name = '';
    if(is_array($pub_details['Publication Type'])) {
      $type_name = $pub_details['Publication Type'][0];
    }
    else {
      $type_name = $pub_details['Publication Type'];
    }
    $identifiers = array(
      'name' => $type_name,
      'cv_id' => array(
        'name' => 'tripal_pub',
      ),
    );
    $pub_type = tripal_get_cvterm($identifiers);
  }
  else {
    tripal_report_error('tripal_pub', TRIPAL_ERROR, 
      "tripal_publication_exists(): The Publication Type is a " .
      "required property but is missing", array());
    return array();
  }
  if (!$pub_type) {
    tripal_report_error('tripal_pub', TRIPAL_ERROR, 
     "tripal_publication_exists(): Cannot find publication type: '%type'",
      array('%type' => $pub_details['Publication Type'][0]));
    return array();
  }

  // get the series name.  The pub.series_name field is only 255 chars so we must truncate to be safe
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
  
  // make sure the publication is unique using the prefereed import duplication check
  $import_dups_check = variable_get('tripal_pub_import_duplicate_check', 'title_year_media');
  $pubs = array();
  switch ($import_dups_check) {
    case 'title_year':
      $identifiers = array(
        'title' => $pub_details['Title'],
        'pyear' => $pub_details['Year']
      );
      $pubs = chado_select_record('pub', array('pub_id'), $identifiers);
      break;
    case 'title_year_type':
      $identifiers = array(
        'title'   => $pub_details['Title'],
        'pyear'   => $pub_details['Year'],
        'type_id' => $pub_type->cvterm_id,
      );
      $pubs = chado_select_record('pub', array('pub_id'), $identifiers);
      break;
    case 'title_year_media':
      $identifiers = array(
        'title'       => $pub_details['Title'],
        'pyear'       => $pub_details['Year'],
        'series_name' => $series_name,
      );
      $pubs = chado_select_record('pub', array('pub_id'), $identifiers);
      break;
  }

  $return = array();
  foreach ($pubs as $pub) {
    $return[] = $pub->pub_id;
  }

  return $return;
}
