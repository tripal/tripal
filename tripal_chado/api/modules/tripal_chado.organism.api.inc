<?php
/**
 * @file
 * Provides API functions specifically for managing feature
 * records in Chado.
 */

/**
 * @defgroup tripal_organism_api Chado Organism
 * @ingroup tripal_chado_api
 * @{
 * Provides API functions specifically for managing organism
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
 *    - genus & species: the chado organism.genus field & organism.species field.
 *    - scientific_name: Full taxonomic name, can include infraspecific nomenclature.
 *   There are also some specially handled keys. They are:
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

  // If the scientific_name identifier is used, we look up organism_id from that.
  if (isset($identifiers['scientific_name'])) {
    $scientific_name = $identifiers['scientific_name'];
    unset($identifiers['scientific_name']);
    $organism_ids = chado_get_organism_id_from_scientific_name($scientific_name, $options);
    if (count($organism_ids) == 1) {
      $identifiers['organism_id'] = $organism_ids[0];
    }
    else {
      tripal_report_error(
        'tripal_organism_api',
        TRIPAL_ERROR,
        "chado_get_organism: The specified scientific name did not uniquely identify an organism.
          You passed in %scientific_name.",
        [
          '%scientific_name' => $scientific_name,
        ]
      );
      return NULL;
    }
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
    // For db query where we explicitly supply infraspecific_type
    elseif (property_exists($organism, 'infraspecific_type')) {
      $rank = $organism->infraspecific_type;
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
 * Returns organism_id values of organisms matching the specified full
 * scientific name, abbreviation, or common name of an organism.
 *
 * @param $name
 *   The organism name to be queried. Infraspecific type can be abbreviated.
 *
 * @param $options
 *   An array of options. The following keys are available:
 *     - check_abbreviation: If TRUE and the $name did not match the
 *         scientific name, then check the abbreviation.
 *     - check_common_name: If TRUE and the $name did not match the
 *         scientific name, then check the common name.
 *     - case_sensitive: If TRUE then all searches should be case
 *         sensitive. Default is FALSE.
 *   If no options are specified, search is for a match of $name to
 *     the scientific_name only, case insensitive.
 *
 * @return
 *   Array of matching organism_id values.
 *
 * @ingroup tripal_organism_api
 */
function chado_get_organism_id_from_scientific_name($name, $options = []) {
  $organism_ids = [];

  // Handle missing $name by returning empty array.
  if (!$name) {
    return $organism_ids;
  }

  // By default, search is case insensitive because this function may
  // be used to handle input from users or from data files in loaders.
  $sql_for_lower = '';
  if (!in_array('case_sensitive', $options, true)) {
    $name = strtolower($name);
    $sql_for_lower = 'LOWER';
  }

  // Check scientific name first, and if a match is found, nothing
  // else specified by $options will be checked.
  // Scientific name is the combination of genus, species,
  // and optionally infraspecific nomenclature added with Chado 1.3
  // For Chado 1.2 and earlier, infraspecific nomenclature had to
  // be stored in the species column, so limit the split accordingly.
  // There is a unique constraint, so expect zero or one match here.
  $infra_present = false;
  $limit = 2;
  if (chado_column_exists('organism', 'infraspecific_name')) {
    $infra_present = true;
    $limit = 4;
  }
  $parts = preg_split('/\s+/', $name, $limit);
  // $name could be a single word, so make sure this is defined.
  if (!array_key_exists(1, $parts)) {
    $parts[1] = '';
  }
  $sql = 'SELECT organism_id FROM {organism} WHERE '.$sql_for_lower.'(genus) = :genus'
       . ' AND '.$sql_for_lower.'(species) = :species';
  $args = [ ':genus' => $parts[0], ':species' => $parts[1] ];
  if ($infra_present) {
    // When there is no infraspecific name, we can either use the "no_rank"
    // taxonomic term in the type_id column, or else use NULL.
    $sql .= ' AND ( type_id = (SELECT cvterm_id FROM {cvterm}'
         . ' WHERE '.$sql_for_lower.'(name) = :infraspecific_type'
         . ' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = :taxonomic_rank))';
    if (!array_key_exists(2, $parts)) {
      $parts[2] = 'no_rank';
      $sql .= " OR type_id IS NULL";
    }
    else {
      $parts[2] = chado_unabbreviate_infraspecific_rank($parts[2]);
    }
    $sql .= ")";
    $args[':infraspecific_type'] = $parts[2];
    $args[':taxonomic_rank'] = 'taxonomic_rank';

    // Infraspecific name, if present.
    if (array_key_exists(3, $parts)) {
      $sql .= ' AND '.$sql_for_lower.'(infraspecific_name) = :infraspecific_name';
      $args[':infraspecific_name'] = $parts[3];
    }
    else {
      // Infraspecific name not present, so this column
      // must be either an empty string or NULL.
      $sql .= " AND ( infraspecific_name = '' ) IS NOT FALSE";
    }
  }
  $results = chado_query($sql, $args);
  while ($organism = $results->fetchField()) {
    if (!in_array($organism, $organism_ids)) {
      $organism_ids[] = $organism;
    }
  }

  // Check other search modes only when no match was found for scientific name.
  if (empty($organism_ids)) {
    // Try to find $name in the abbreviation column. This does not
    // have a unique constraint, so there may be more than one match.
    if (in_array('check_abbreviation', $options, true)) {
      $sql = 'SELECT organism_id FROM {organism} WHERE '.$sql_for_lower.'(abbreviation) = :name';
      $args = [':name' => $name];
      $results = chado_query($sql, $args);
      while ($organism = $results->fetchField()) {
        $organism_ids[] = $organism;
      }
    }

    // Try to find $name in the common_name column. This does not
    // have a unique constraint, so there may be more than one match.
    if (in_array('check_common_name', $options, true)) {
      $sql = 'SELECT organism_id FROM {organism} WHERE '.$sql_for_lower.'(common_name) = :name';
      $args = [':name' => $name];
      $results = chado_query($sql, $args);
      while ($organism = $results->fetchField()) {
        if (!in_array($organism, $organism_ids)) {
          $organism_ids[] = $organism;
        }
      }
    }
  }

  return $organism_ids;
}

/**
 * Returns a list of organisms to use in select lists.
 *
 * @param $syncd_only
 *   Whether or not to return all chado organisms or just those sync'd with
 *   drupal. Defaults to TRUE (only sync'd organisms). For Tripal v3 you want FALSE here.
 *
 * @param $show_common_name
 *   When true, include the organism common name, if present, in parentheses.
 *
 * @return
 *   An array of organisms where each value is the organism
 *   scientific name and the keys are organism_id's.
 *
 * @ingroup tripal_organism_api
 */
function chado_get_organism_select_options($syncd_only = TRUE, $show_common_name = FALSE) {

  // This flag indicates whether infraspecific nomenclature (chado 1.3) is available on this site.
  $infra_present = chado_column_exists('organism', 'infraspecific_name');

  $org_list = [];
  $org_list[] = 'Select an organism';

  if ($syncd_only) {
    // Retrieve only synced organisms, relevant only for Tripal v2 sites
    $sql = "
      SELECT O.*
      FROM [chado_organism] CO
        INNER JOIN {organism} O ON O.organism_id = CO.organism_id
      ORDER BY O.genus, O.species
    ";
  }
  else {
    // Retrieve all organisms
    $sql = "
      SELECT *
      FROM {organism}
      ORDER BY genus, species
    ";
    if ($infra_present) {
      $sql = "
        SELECT organism_id, genus, species, type_id,
          (REPLACE ((SELECT name FROM {cvterm} CVT WHERE CVT.cvterm_id = type_id AND CVT.cv_id =
            (SELECT cv_id FROM {cv} WHERE name='taxonomic_rank')), 'no_rank', '')) AS infraspecific_type,
          infraspecific_name, common_name
        FROM {organism}
        ORDER BY genus, species, infraspecific_type, infraspecific_name
      ";
    }
  }
  $orgs = chado_query($sql);

  // Iterate through the organisms and build an array of their names.
  foreach ($orgs as $org) {
    $org_list[$org->organism_id] = $org->genus . ' ' . $org->species;
    // Include abbreviated infraspecific nomenclature in name when present,
    // e.g. subspecies becomes subsp.
    if ($infra_present and ($org->infraspecific_type or $org->infraspecific_name)) {
      $org_list[$org->organism_id] = chado_get_organism_scientific_name($org);
    }
    // Append common name when requested and when present.
    if ($show_common_name and $org->common_name) {
      $org_list[$org->organism_id] .= ' (' . $org->common_name . ')';
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
    case 'cultivar':
      $abb = 'cv.';
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

/**
 * A handy function to expand the infraspecific rank from an abbreviation.
 *
 * @param $rank
 *   The rank below species or its abbreviation.
 *   A period at the end of the abbreviation is optional.
 *
 * @return
 *   The proper unabbreviated form for the rank.
 *
 * @ingroup tripal_organism_api
 */
function chado_unabbreviate_infraspecific_rank($rank) {
  if (preg_match('/^subsp\.?$/', $rank)) {
    $rank = 'subspecies';
  }
  elseif (preg_match('/^ssp\.?$/', $rank)) {
    $rank = 'subspecies';
  }
  elseif (preg_match('/^var\.?$/', $rank)) {
    $rank = 'varietas';
  }
  elseif (preg_match('/^subvar\.?$/', $rank)) {
    $rank = 'subvarietas';
  }
  elseif (preg_match('/^cv\.?$/', $rank)) {
    $rank = 'cultivar';
  }
  elseif (preg_match('/^f\.?$/', $rank)) {
    $rank = 'forma';
  }
  elseif (preg_match('/^subf\.?$/', $rank)) {
    $rank = 'subforma';
  }
  // if none of the above matched, rank is returned unchanged
  return $rank;
}
