<?php
/**
 * @file
 * Provides an application programming interface (API) to manage organisms
 */

/**
 * @defgroup tripal_organism_api Organism API
 * @ingroup tripal_api
 * @{
 * Provides an application programming interface (API) to manage organisms
 * @}
 */

/**
 * Retrieves a chado organism variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only on of the
 *   following unique keys is required):
 *    - organism_id: the chado organism.organism_id primary key
 *    - genus & species: the chado organism.genus field & organism.species field
 *   There are also some specially handled keys. They are:
 *    - property: An array/object describing the property to select records for. It
 *      should at least have either a type_name (if unique across cvs) or type_id. Other
 *      supported keys include: cv_id/cv_name (of the type), value and rank
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function definition for
 *       additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values passed into
 *   chado_select_record(). It should fully specify the organism record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing the organism
 *   will be returned (will be a chado variable from chado_generate_var()). Otherwise,
 *   FALSE will be returned.
 *
 * @ingroup tripal_organism_api
 */
function tripal_get_organism($identifiers, $options = array()) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var not to follow any foreign keys
    $options['include_fk'] = array();
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "tripal_get_organism: The identifier passed in is expected to be an array with the key
        matching a column name in the organism table (ie: organism_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "tripal_get_organism: You did not pass in anything to identify the organism you want. The identifier
        is expected to be an array with the key matching a column name in the organism table
        (ie: organism_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // If one of the identifiers is property then use chado_get_record_with_property()
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $organism = chado_get_record_with_property('organism', $property, $identifiers, $options);
  }

  // Else we have a simple case and we can just use chado_generate_var to get the analysis
  else {

    // Try to get the organism
    $organism = chado_generate_var(
      'organism',
      $identifiers,
      $options
    );
  }

  // Ensure the organism is singular. If it's an array then it is not singular
  if (is_array($organism)) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "tripal_get_organism: The identifiers you passed in were not unique. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Report an error if $organism is FALSE since then chado_generate_var has failed
  elseif ($organism === FALSE) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "tripal_get_organism: chado_generate_var() failed to return a organism based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their organism :)
  else {
    return $organism;
  }
}

/**
 * Returns a list of organisms that are currently synced with Drupal to use in select lists
 *
 * @param $syncd_only
 *   Whether or not to return all chado organisms or just those sync'd with drupal. Defaults
 *   to TRUE (only sync'd organisms)
 * @return
 *   An array of organisms sync'd with Drupal where each value is the organism scientific
 *   name and the keys are organism_id's
 *
 * @ingroup tripal_organism_api
 */
function tripal_get_organism_select_options($syncd_only = TRUE) {

  if ($syncd_only) {
    // use this SQL for getting synced organisms
    $dsql =  "SELECT * FROM {chado_organism}";
    $orgs = db_query($dsql);

    // use this SQL statement for getting the organisms
    $csql =  "SELECT * FROM {organism} " .
             "WHERE organism_id = :organism_id";

    $org_list = array();

    // iterate through the organisms and build an array of those that are synced
    foreach ($orgs as $org) {
      $args = array(':organism_id' => $org->organism_id);
      $org = chado_query($csql, $args)->fetchObject();
      $org_list[$org->organism_id] = $org->genus . ' ' . $org->species;
    }
  }
  else {
    // use this SQL statement for getting the organisms
    $csql =  "SELECT * FROM {organism}";
    $orgs = chado_query($csql)->execute();

    $org_list = array();

    // iterate through the organisms and build an array of those that are synced
    foreach ($orgs as $org) {
      $org_list[$org->organism_id] = $org->genus . ' ' . $org->species;
    }
  }
  return $org_list;
}

/**
 * Return the URL for the organism image
 *
 * @param $organism
 *   An organism table record
 * @param $nid
 *   (Optional) the node id of the organism node. if not supplied it will be looked up
 *
 * @return
 *   The fully qualified url to the image
 */
function tripal_get_organism_image($organism, $nid = NULL) {
  $url = '';

  // first look for an image with the genus/species name.  This is old-style tripal
  // and we keep it for backwards compatibility.  If we don't find that file
  // then look for the image with the node ID in the name. If we don't find that then
  // no image tag is generated
  $base_path = realpath('.');
  $image_dir = tripal_get_files_dir('tripal_organism') . "/images";
  $image_name =  $organism->genus . "_" . $organism->species . ".jpg";

  if (file_exists("$base_path/$image_dir/$image_name")) {
    $url = file_create_url("$image_dir/$image_name");
  }
  else {
     $image_name = $nid . ".jpg";
     if (file_exists("$base_path/$image_dir/$image_name")) {
       $url = file_create_url("$image_dir/$image_name");
     }
  }
  return $url;
}
