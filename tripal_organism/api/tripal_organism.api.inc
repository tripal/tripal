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
    $organism = chado_get_record_with_property(
      array('table' => 'organism', 'base_records' => $identifiers),
      array('type_name' => $property),
      $options
    );
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
  $org_list = array();
  $org_list[] = 'Select an organism';

  if ($syncd_only) {
    $sql = "
      SELECT *
      FROM public.chado_organism CO
        INNER JOIN {organism} O ON O.organism_id = CO.organism_id
      ORDER BY O.genus, O.species
    ";
    $orgs = chado_query($sql);

    // iterate through the organisms and build an array of those that are synced
    foreach ($orgs as $org) {
      $org_list[$org->organism_id] = $org->genus . ' ' . $org->species;
    }
  }
  else {
    // use this SQL statement for getting the organisms
    $csql =  "SELECT * FROM {organism} ORDER BY genus, species";
    $orgs = chado_query($csql);

    // iterate through the organisms and build an array of those that are synced
    foreach ($orgs as $org) {
      $org_list[$org->organism_id] = $org->genus . ' ' . $org->species;
    }
  }
  return $org_list;
}

/**
 * Return the path for the organism image.
 *
 * @param $organism
 *   An organism table record
 *
 * @return
 *   If the type parameter is 'url' (the default) then the fully qualified
 *   url to the image is returend. If no image is present then NULL is returned
 */
function tripal_get_organism_image_url($organism) {
  $url = '';

  if (!is_object($organism)) {
    return NULL;
  }

  // Get the organism's node
  $nid = chado_get_nid_from_id('organism', $organism->organism_id);

  // Look in the file_usage table of Drupal for the image file. This
  // is the current way for handling uploaded images. It allows the file to
  // keep it's proper name and extension.
  $fid = db_select('file_usage', 'fu')
    ->fields('fu', array('fid'))
    ->condition('module', 'tripal_organism')
    ->condition('type', 'organism_image')
    ->condition('id', $nid)
    ->execute()
    ->fetchField();
  if ($fid) {
    $file = file_load($fid);
    return file_create_url($file->uri);
  }

  // First look for an image with the genus/species name.  This is old-style tripal
  // and we keep it for backwards compatibility.
  $base_path = realpath('.');
  $image_dir = tripal_get_files_dir('tripal_organism') . "/images";
  $image_name =  $organism->genus . "_" . $organism->species . ".jpg";
  $image_path = "$base_path/$image_dir/$image_name";

  if (file_exists($image_path)) {
    $url = file_create_url("$image_dir/$image_name");
    return $url;
  }

  // If we don't find the file using the genus ans species then look for the
  // image with the node ID in the name. This method was used for Tripal 1.1
  // and 2.x-alpha version.
  $image_name = $nid . ".jpg";
  $image_path = "$base_path/$image_dir/$image_name";
  if (file_exists($image_path)) {
    $url = file_create_url("$image_dir/$image_name");
    return $url;
  }

  return NULL;
}

/**
 * This function is intended to be used in autocomplete forms
 * for searching for organisms that begin with the provided string
 *
 * @param $text
 *   The string to search for
 *
 * @return
 *   A json array of terms that begin with the provided string
 *
 * @ingroup tripal_organism_api
 */
function tripal_autocomplete_organism($text) {
  $matches = array();
  $genus = $text;
  $species = '';
  if (preg_match('/^(.*?) (.*)$/', $text, $matches)) {
    $genus = $matches[1];
    $species = $matches[2];
  }
  $sql = "SELECT * FROM {organism} WHERE lower(genus) like lower(:genus) ";
  $args = array();
  $args[':genus'] = $genus . '%';
  if ($species) {
    $sql .= "AND lower(species) like lower(:species) ";
    $args[':species'] =  $species . '%';
  }
  $sql .= "ORDER BY genus, species ";
  $sql .= "LIMIT 25 OFFSET 0 ";
  $results = chado_query($sql, $args);
  $items = array();
  foreach ($results as $organism) {
    $name = $organism->genus . ' ' .$organism->species;
    $items[$name] = $name;
  }
  drupal_json_output($items);
}