<?php
/**
 * @file
 * Provides API functions specificially for managing feature
 * records in Chado.
 */

/**
 * @defgroup tripal_organism_api Chado Organism
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specificially for managing organism
 * records in Chado.
 * @}
 */

/**
 * Retrieves a chado organism variable.
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only
 *   one of the following unique keys is required):
 *    - organism_id: the chado organism.organism_id primary key.
 *    - genus & species: the chado organism.genus field & organism.species
 *   field. There are also some specially handled keys. They are:
 *    - property: An array/object describing the property to select records
 *   for.
 *      It should at least have either a type_name (if unique across cvs) or
 *      type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank.
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *      definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 * passed into chado_select_record(). It should fully specify the organism
 * record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the organism will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, NULL will be returned.
 *
 * @ingroup tripal_organism_api
 */
function chado_get_organism($identifiers, $options = []) {

  // Set Defaults.
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to not follow any foreign keys.
    $options['include_fk'] = [];
  }

  // Error Checking of parameters.
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "chado_get_organism: The identifier passed in is expected to be an array with the key
        matching a column name in the organism table (ie: organism_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "chado_get_organism: You did not pass in anything to identify the organism you want. The identifier
        is expected to be an array with the key matching a column name in the organism table
        (ie: organism_id or name). You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // If one of the identifiers is property then use chado_get_record_with_property().
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $organism = chado_get_record_with_property(
      ['table' => 'organism', 'base_records' => $identifiers],
      ['type_name' => $property],
      $options
    );
  }

  // Else we have a simple case and we can just use chado_generate_var to get 
  // the organism.
  else {

    // Try to get the organism
    $organism = chado_generate_var(
      'organism',
      $identifiers,
      $options
    );
  }

  // Ensure the organism is singular. If it's an array then it is not singular.
  if (is_array($organism)) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "chado_get_organism: The identifiers you passed in were not unique. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Report an error if $organism is FALSE since then chado_generate_var has 
  // failed.
  elseif ($organism === FALSE) {
    tripal_report_error(
      'tripal_organism_api',
      TRIPAL_ERROR,
      "chado_get_organism: chado_generate_var() failed to return an organism based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      [
        '%identifier' => print_r($identifiers, TRUE),
      ]
    );
  }

  // Else, as far we know, everything is fine so give them their organism :)
  else {
    return $organism;
  }
}

/**
 * Returns the full scientific name of an organism.
 *
 * @param $organism
 *   An organism object.
 *
 * @return
 *   The full scientific name of the organism.
 *
 * @ingroup tripal_organism_api
 */
function chado_get_organism_scientific_name($organism) {
  $name = $organism->genus . ' ' . $organism->species;

  // For Chado v1.3 we have a type_id and infraspecific name.
  if (property_exists($organism, 'type_id')) {
    $rank = '';
    // For organism objects created using chado_generate_var.
    if (is_object($organism->type_id)) {
      if ($organism->type_id) {
        $rank = $organism->type_id->name;
      }
    }
    else {
      $rank_term = chado_get_cvterm(['cvterm_id' => $organism->type_id]);
      if ($rank_term) {
        $rank = $rank_term->name;
      }
    }

    if ($rank) {
      $rank = chado_abbreviate_infraspecific_rank($rank);
      // rank will now be an empty string if it had been the "no_rank" cv term
      if ($rank) {
        $name .= ' ' . $rank;
      }
    }
    if ($organism->infraspecific_name) {
      $name .= ' ' . $organism->infraspecific_name;
    }
  }
  return $name;
}

/**
 * Returns a list of organisms that are currently synced with Drupal to use in
 * select lists.
 *
 * @param $syncd_only
 *   Whether or not to return all chado organisms or just those sync'd with
 *   drupal. Defaults to TRUE (only sync'd organisms).
 *
 * @return
 *   An array of organisms sync'd with Drupal where each value is the organism
 *   scientific name and the keys are organism_id's.
 *
 * @ingroup tripal_organism_api
 */
function chado_get_organism_select_options($syncd_only = TRUE) {
  $org_list = [];
  $org_list[] = 'Select an organism';

  if ($syncd_only) {
    $sql = "
      SELECT *
      FROM [chado_organism] CO
        INNER JOIN {organism} O ON O.organism_id = CO.organism_id
      ORDER BY O.genus, O.species
    ";
    // if present in this site's version of chado, include infraspecific nomenclature in sorting
    if (chado_column_exists('organism', 'infraspecific_name')) {
      $sql .= ", REPLACE ((SELECT name FROM {cvterm} CVT WHERE CVT.cvterm_id = O.type_id
                AND CVT.cv_id = (SELECT cv_id FROM {cv} WHERE name='taxonomic_rank')), 'no_rank', ''),
                O.infraspecific_name";
    }
    $orgs = chado_query($sql);

    // Iterate through the organisms and build an array of those that are synced.
    foreach ($orgs as $org) {
      $org_list[$org->organism_id] = chado_get_organism_scientific_name($org);
    }
  }
  else {
    // use this SQL statement for getting the organisms
    $csql = "SELECT * FROM {organism} ORDER BY genus, species";
    // if present in this site's version of chado, include infraspecific nomenclature in sorting
    if (chado_column_exists('organism', 'infraspecific_name')) {
      $csql .= ", REPLACE ((SELECT name FROM {cvterm} CVT WHERE CVT.cvterm_id = type_id" .
               " AND CVT.cv_id = (SELECT cv_id FROM {cv} WHERE name='taxonomic_rank')), 'no_rank', '')," .
               " infraspecific_name";
    }
    $orgs = chado_query($csql);

    // Iterate through the organisms and build an array of those that are synced.
    foreach ($orgs as $org) {
      $org_list[$org->organism_id] = chado_get_organism_scientific_name($org);
    }
  }
  return $org_list;
}

/**
 * Return the path for the organism image.
 *
 * @param $organism
 *   An organism table record.
 *
 * @return
 *   If the type parameter is 'url' (the default) then the fully qualified
 *   url to the image is returend. If no image is present then NULL is returned.
 *
 * @ingroup tripal_organism_api
 */
function chado_get_organism_image_url($organism) {
  $url = '';

  if (!is_object($organism)) {
    return NULL;
  }

  // Get the organism's node.
  $nid = chado_get_nid_from_id('organism', $organism->organism_id);

  // Look in the file_usage table of Drupal for the image file. This
  // is the current way for handling uploaded images. It allows the file to
  // keep its proper name and extension.
  $fid = db_select('file_usage', 'fu')
    ->fields('fu', ['fid'])
    ->condition('module', 'tripal_organism')
    ->condition('type', 'organism_image')
    ->condition('id', $nid)
    ->execute()
    ->fetchField();
  if ($fid) {
    $file = file_load($fid);
    return file_create_url($file->uri);
  }

  // First look for an image with the genus/species name.  This is old-style 
  // tripal and we keep it for backwards compatibility.
  $base_path = realpath('.');
  $image_dir = tripal_get_files_dir('tripal_organism') . "/images";
  $image_name = $organism->genus . "_" . $organism->species . ".jpg";
  $image_path = "$base_path/$image_dir/$image_name";

  if (file_exists($image_path)) {
    $url = file_create_url("$image_dir/$image_name");
    return $url;
  }

  // If we don't find the file using the genus and species then look for the
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
 * for searching for organisms that begin with the provided string.
 *
 * @param $text
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_organism_api
 */
function chado_autocomplete_organism($text) {
  $matches = [];
  $genus = $text;
  $species = '';
  if (preg_match('/^(.*?)\s+(.*)$/', $text, $matches)) {
    $genus = $matches[1];
    $species = $matches[2];
  }
  $sql = "SELECT * FROM {organism} WHERE lower(genus) like lower(:genus) ";
  $args = [];
  $args[':genus'] = $genus . '%';
  if ($species) {
    $sql .= "AND lower(species) like lower(:species) ";
    $args[':species'] = $species . '%';
  }
  $sql .= "ORDER BY genus, species";
  // if present in this site's version of chado, include infraspecific nomenclature in sorting
  if (chado_column_exists('organism', 'infraspecific_name')) {
    $sql .= ", REPLACE ((SELECT name FROM {cvterm} CVT WHERE CVT.cvterm_id = type_id" .
            " AND CVT.cv_id = (SELECT cv_id FROM {cv} WHERE name='taxonomic_rank')), 'no_rank', '')," .
            " infraspecific_name";
  }
  $sql .= " LIMIT 25 OFFSET 0";
  $results = chado_query($sql, $args);
  $items = [['args' => [$sql => $args]]];
  foreach ($results as $organism) {
    $name = chado_get_organism_scientific_name($organism);
    $items["$name [id: $organism->organism_id]"] = $name;
  }
  drupal_json_output($items);
}

/**
 * A handy function to abbreviate the infraspecific rank.
 *
 * @param $rank
 *   The rank below species.
 *
 * @return
 *   The proper abbreviation for the rank.
 *
 * @ingroup tripal_organism_api
 */
function chado_abbreviate_infraspecific_rank($rank) {
  $abb = '';
  switch ($rank) {
    case 'no_rank':
      $abb = '';
      break;
    case 'subspecies':
      $abb = 'subsp.';
      break;
    case 'varietas':
      $abb = 'var.';
      break;
    case 'variety':
      $abb = 'var.';
      break;
    case 'subvarietas':
      $abb = 'subvar.';
      break;
    case 'subvariety':
      $abb = 'subvar.';
      break;
    case 'forma':
      $abb = 'f.';
      break;
    case 'subforma':
      $abb = 'subf.';
      break;
    default:
      $abb = $rank;
  }
  return $abb;
}
