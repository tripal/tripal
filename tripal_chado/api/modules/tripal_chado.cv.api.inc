<?php
/**
 * @file
 * This module provides a set of functions to simplify working with
 * controlled vocabularies.
 *
 * @ingroup tripal_chado
 */

/**
 * @defgroup tripal_chado_cv_api Chado CV
 * @ingroup tripal_chado_api
 * @{
 * @}
 */

/**
 * Retrieves a chado controlled vocabulary variable
 *
 * @param $identifier
 *   An array with the key stating what the identifier is. Supported keys (only on of the
 *   following unique keys is required):
 *    - cv_id: the chado cv.cv_id primary key
 *    - name: the chado cv.name field (assume unique)
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function definition for
 *       additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values passed into
 *   chado_select_record(). It should fully specify the cv record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing the cv
 *   will be returned (will be a chado variable from chado_generate_var()). Otherwise,
 *   FALSE will be returned.
 *
 * @ingroup tripal_chado_cv_api
 */
function tripal_get_cv($identifiers, $options = array()) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var not to follow any foreign keys
    $options['include_fk'] = array();
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "tripal_get_cv: The identifier passed in is expected to be an array with the key
        matching a column name in the cv table (ie: cv_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "tripal_get_cv: You did not pass in anything to identify the cv you want. The identifier
        is expected to be an array with the key matching a column name in the cv table
        (ie: cv_id or name). You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Try to get the cv
  $cv = chado_generate_var(
    'cv',
    $identifiers,
    $options
  );

  // Ensure the cv is singular. If it's an array then it is not singular
  if (is_array($cv)) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "tripal_get_cv: The identifiers you passed in were not unique. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Report an error if $cv is FALSE since then chado_generate_var has failed
  elseif ($cv === FALSE) {
    tripal_report_error(
      'tripal_chado_api',
      TRIPAL_ERROR,
      "tripal_get_cv: chado_generate_var() failed to return a cv based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their cv :)
  else {
    return $cv;
  }
}

/**
 * Create an options array to be used in a form element which provides a
 * list of all chado cvs
 *
 * @return
 *   An array(cv_id => name) for each cv in the chado cv table
 *
 * @ingroup tripal_chado_cv_api
 */
function tripal_get_cv_select_options() {

  $results = chado_select_record('cv', array('cv_id', 'name'), array(), array('order_by' => array('name' => 'ASC')));

  $options = array();
  $options[] = 'Select a Vocabulary';

  foreach ($results as $r) {
    $options[$r->cv_id] = $r->name;
  }

  return $options;

}

/**
 * Retrieves a chado controlled vocabulary term variable
 *
 * @param $identifier
 *   An array apropriate for use with the chado_generate_var for uniquely
 *   identifying a cvterm record. Alternativley, there are also some specially
 *   handled keys. They are:
 *    - id: an ID for the term of the for [dbname]:[accession], where [dbname]
 *      is the short name of the vocabulary and accession is the unique ID.
 *    - cv_id:  an integer indicating the cv_id or an array with 'name' => the
 *      name of the cv.
 *    - synonym: an array with 'name' => the name of the synonym of the cvterm
 *      you want returned; 'cv_id' => the cv_id of the synonym; 'cv_name' =>
 *      the name of the cv of the synonym
 *    - property: An array/object describing the property to select records
 *      for. It should at least have either a type_name (if unique across cvs)
 *      or type_id. Other supported keys include: cv_id/cv_name (of the type),
 *      value and rank
 * @param $options
 *   An array of options. Supported keys include:
 *     - Any keys supported by chado_generate_var(). See that function
 *       definition for additional details.
 *
 * NOTE: the $identifier parameter can really be any array similar to $values
 *   passed into chado_select_record(). It should fully specify the cvterm
 *   record to be returned.
 *
 * @return
 *   If unique values were passed in as an identifier then an object describing
 *   the cvterm will be returned (will be a chado variable from
 *   chado_generate_var()). Otherwise, FALSE will be returned.
 *
 * @ingroup tripal_chado_cv_api
 */
function tripal_get_cvterm($identifiers, $options = array()) {

  // Set Defaults
  if (!isset($options['include_fk'])) {
    // Tells chado_generate_var to only get the cv
    $options['include_fk'] = array('cv_id' => TRUE);
  }

  // Error Checking of parameters
  if (!is_array($identifiers)) {
    tripal_report_error('tripal_cv_api', TRIPAL_ERROR,
      "tripal_get_cvterm: The identifier passed in is expected to be an array with the key
        matching a column name in the cvterm table (ie: cvterm_id or name). You passed in %identifier.",
      array('%identifier'=> print_r($identifiers, TRUE))
    );
  }
  elseif (empty($identifiers)) {
    tripal_report_error('tripal_cv_api', TRIPAL_ERROR,
      "tripal_get_cvterm: You did not pass in anything to identify the cvterm you want. The identifier
        is expected to be an array with the key matching a column name in the cvterm table
        (ie: cvterm_id or name). You passed in %identifier.",
      array('%identifier'=> print_r($identifiers, TRUE))
    );
  }

  // If synonym was passed in, then process this first before calling chado_generate_var()
  if (isset($identifiers['synonym'])) {
    $synonym = $identifiers['synonym']['name'];

    $values = array('synonym' => $synonym);
    if (isset($identifiers['synonym']['cv_id'])) {
      $values['cvterm_id'] = array('cv_id' => $identifiers['synonym']['cv_id']);
    }
    if (isset($identifiers['synonym']['cv_name'])) {
      $values['cvterm_id'] = array('cv_id' => array('name' => $identifiers['synonym']['cv_name']));
    }
    $options = array(
      'case_insensitive_columns' => array('name')
    );
    $result = chado_select_record('cvtermsynonym', array('cvterm_id'), $values, $options);

    // if the synonym doens't exist or more than one record is returned then return false
    if (count($result) == 0) {
      return FALSE;
    }
    if (count($result) > 1) {
      return FALSE;
    }

    $identifiers = array('cvterm_id' => $result[0]->cvterm_id);
  }

  // If one of the identifiers is property then use chado_get_record_with_property()
  if (isset($identifiers['property'])) {
    $property = $identifiers['property'];
    unset($identifiers['property']);
    $cvterm = chado_get_record_with_property(
      array('table' => 'cvterm', 'base_records' => $identifiers),
      array('type_name' => $property),
      $options
    );
  }
  if (isset($identifiers['id'])) {
    list($db_name, $accession) = preg_split('/:/', $identifiers['id']);
    $cvterm = chado_generate_var('cvterm',array(
      'dbxref_id' => array(
        'db_id' => array(
          'name' => $db_name,
        ),
        'accession' => $accession,
      )
    ));
  }

  // Else we have a simple case and we can just use chado_generate_var to get the cvterm
  else {
    // Try to get the cvterm
    $cvterm = chado_generate_var('cvterm', $identifiers, $options);
  }

  // Ensure the cvterm is singular. If it's an array then it is not singular
  if (is_array($cvterm)) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cvterm: The identifiers you passed in were not unique. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Report an error if $cvterm is FALSE since then chado_generate_var has failed
  elseif ($cvterm === FALSE) {
    tripal_report_error(
      'tripal_cv_api',
      TRIPAL_ERROR,
      "tripal_get_cvterm: chado_generate_var() failed to return a cvterm based on the identifiers
        you passed in. You should check that your identifiers are correct, as well as, look
        for a chado_generate_var error for additional clues. You passed in %identifier.",
      array(
        '%identifier'=> print_r($identifiers, TRUE)
      )
    );
  }

  // Else, as far we know, everything is fine so give them their cvterm :)
  else {
    return $cvterm;
  }

}

/**
 * Create an options array to be used in a form element
 *   which provides a list of all chado cvterms
 *
 * @param $cv_id
 *   The chado cv_id; only cvterms with the supplied cv_id will be returned
 * @param $rel_type
 *   Set to TRUE if the terms returned should only be relationship types in
 *   the vocabulary.  This is useful for creating drop-downs of terms
 *   used for relationship linker tables.
 *
 * @return
 *   An associative array with the cvterm_id's as keys. The first
 *   element in the array has a key of '0' and a value of 'Select a Type'
 *
 * @ingroup tripal_chado_cv_api
 */
function tripal_get_cvterm_select_options($cv_id, $rel_type = FALSE) {
  $columns = array('cvterm_id', 'name');
  $values = array('cv_id' => $cv_id);
  if ($rel_type) {
    $values['is_relationshiptype'] = 1;
  }
  $s_options = array('order_by' => array('name' => 'ASC'));

  $cvterms = chado_select_record('cvterm', $columns, $values, $s_options);

  $options = array();
  $options[0] = 'Select a Type';
  foreach ($cvterms as $cvterm) {
    $options[$cvterm->cvterm_id] = $cvterm->name;
  }

  return $options;

}

/**
 * Updates the cvtermpath table of Chado for the specified CV.
 *
 * @param $cv_id
 *   The chado cv_id;
 * @param $job_id
 *   This function is intended to be used with the Tripal Jobs API.
 *   When this function is called as a job the $job_id is automatically
 *   passed to this function.
 * @return
 *   TRUE on success FALSE on failure
 *
 * @ingroup tripal_chado_cv_api
 */
function tripal_update_cvtermpath_old($cv_id, $job_id = NULL) {
  // TODO: need better error checking in this function

  // first get the controlled vocabulary name:
  $sql = "SELECT * FROM {cv} WHERE cv_id = :cv_id";
  $cv = chado_query($sql, array(':cv_id' => $cv_id))->fetchObject();

  print "\nUpdating cvtermpath for $cv->name...\n";

  // We need to set the chado schema as active because some of the
  // functions call other functions which would not be in scope.
  $previous = chado_set_active('chado');
  try {
    $sql = "SELECT * FROM fill_cvtermpath(:name)";

    db_query($sql, array(':name' => $cv->name));
    chado_set_active($previous);
  }
  catch (Exception $e) {
    chado_set_active($previous);
    $error = $e->getMessage();
    tripal_report_error('tripal_chado', TRIPAL_ERROR, "Could not fill cvtermpath table: @error", array('@error' => $error));
    return FALSE;
  }
  return TRUE;
}

/**
 * Duplicate of fill_cvtermpath() stored procedure in Chado.
 *
 * Identifies all of the root terms of the controlled vocabulary. These
 * root terms are then processed by calling the
 * tripal_update_cvtermpath_root_loop() function on each one.
 *
 * @param $cvid
 *   The controlled vocabulary ID from the cv table of Chado (i.e. cv.cv_id).
 * @param $job_id
 *
 * @ingroup tripal_chado_cv_api
 */
function tripal_update_cvtermpath($cv_id, $job_id = NULL){
  // TODO: there's a function to determine the current Chado instance.
  // we should use that.
  $prev_db = chado_set_active('chado');
  try {
    $result = db_query('
      SELECT DISTINCT t.*
      FROM cvterm t
        LEFT JOIN cvterm_relationship r ON (t.cvterm_id = r.subject_id)
        INNER JOIN cvterm_relationship r2 ON (t.cvterm_id = r2.object_id)
      WHERE t.cv_id = :cvid AND r.subject_id is null',
      array(':cvid' => $cv_id)
    );

    // Iterate through each root level term.
    $record = $result->fetchAll();
    $roots = array();
    foreach ($record as $item){
      tripal_update_cvtermpath_root_loop($item->cvterm_id, $item->cv_id, $roots);
    }
  }
  catch (Exception $e) {
    // If there's an exception we have to set the database back. So, do that
    // and then rethrow the error.
    chado_set_active($prev_db);
    throw $e;
  }
  chado_set_active($prev_db);
}

/**
 *  Duplicate of _fill_cvtermpath4root() stored procedure in Chado.
 *
 *  This function process a "branch" of the ontology.  Initially, the
 *  "root" starts at the top of the tree. But, as the cvtermpath is populated
 *  the "root" becomes terms deeper in the tree.
 *
 * @param $rootid
 *   The term ID from the cvterm table of Chado (i.e. cvterm.cvterm_id).
 * @param $cvid
 *   The controlled vocabulary ID from the cv table of Chado (i.e. cv.cv_id).
 *
 * @ingroup tripal_chado_cv_api
 */
function tripal_update_cvtermpath_root_loop($rootid, $cvid, &$roots) {

  // Get's the cvterm record for this "root".
  $ttype = db_select('cvterm', 'cv')
          ->fields('cv', array('cvterm_id'));
  $db_or = db_or();
  $db_or->condition('cv.name', "isa", '=');
  $db_or->condition('cv.name', "is_a", '=');
  $ttype->condition($db_or);
  $result = $ttype->execute()->fetchObject();

  $term_id = $rootid . '|' . $rootid . '|' . $cvid . '|' . $result->cvterm_id;
  // If the child_id matches any other id in the array then we've hit a loop.
  foreach ($roots as $element_id) {
    if ($element_id == $term_id) {
      return;
    }
  }
  // Then add that new entry to the $tree_path.
  $roots[] = $term_id;

  // Descends through the branch starting at this "root" term.
  $tree_path = array();
  $matched_rows = array();
  $possible_start_of_loop = array();
  $depth = 0;
  tripal_update_cvtermpath_loop($rootid, $rootid, $cvid, $result->cvterm_id, $depth,
                                0, $tree_path, FALSE, $matched_rows, $possible_start_of_loop, FALSE);

  // Get's the children terms of this "root" term and then recursively calls
  // this function making each child root.
  $cterm = db_query(
    'SELECT *
      FROM cvterm_relationship
      WHERE object_id = :rootid
    ',
    array(':rootid' => $rootid)
  );
  while ($cterm_result = $cterm->fetchAssoc()) {
    tripal_update_cvtermpath_root_loop($cterm_result['subject_id'], $cvid, $roots);
  }
}

/**
 *
 * @param $origin
 *   The root terms cvterm_id.
 * @param $child_id
 *   The cvterm_id of the current child term.  The child term is a descendent
 *   of the origin.
 * @param $cv_id
 *   The controlled vocabulary ID from the cv table of Chado (i.e. cv.cv_id).
 * @param $type_id
 *   The relationship type between the origin term and the child.
 * @param $depth
 *   The depth of the recursion.
 * @param $increment_of_depth.
 *   An integer ??
 * @param $tree_path.
 *   The array of every term between the current child and the origin. Each
 *   element in the array is an associative array with the keys:
 *     -build_id: an string identifier for the child that combines the origin,
 *      child cvterm_id,cv_id, and the type_id.
 *     -depth: the depth that a child was inserted into the cvtermpath table.
 * @param $possible_loop
 *    A boolean flag.
 * @param $matched_row
 *    An array of rows that are currently in the cvtermpath table that match the
 *    build_id of the current term trying to be written to the table
 * @param $possible_start_of_ loop
 *    The array of the possible loop item between the current child and the origin.
 *    Each element in the array is an associative array with the keys:
 *     - cvid : $cv_id
 *     - subject_id:
 *     - child_id : $child_id,
 *     - type_id : $type_id,
 *     - depth : $depth,
 * @param $no_loop_skip_test
 *     A boolean used when the possible loop has been ruled out as a loop.
 * @return multitype:
 */
function tripal_update_cvtermpath_loop(
  $origin,
  $child_id,
  $cv_id,
  $type_id,
  $depth,
  $increment_of_depth,
  $tree_path,
  $possible_loop,
  $matched_rows,
  $possible_start_of_loop,
  $no_loop_skip_test) {

  // We have not detected a loop, so it's safe to insert the term.
  $new_match_rows = array();
  if (!empty($possible_start_of_loop)) {
    // Go through each matched_row.
    if (count($matched_rows) === 1) {
      // Get the cvtermpath_id and then increment down one row.
      $cvtermpath_id = (int) $matched_rows[0]->cvtermpath_id;
      $cvtermpath_id = $cvtermpath_id + 1;
      chado_set_active('chado');
      $next_row = db_query(
        'SELECT *
          FROM cvtermpath
          WHERE cvtermpath_id = :cvtermpath_id
        ',
        array(':cvtermpath_id' => $cvtermpath_id)
      );
      $next_row = $next_row->fetchObject();

      // If the next row matches the values passed we can't rule out a loop.
      if (($next_row->type_id === $type_id) &&
          ($next_row->subject_id === $child_id) &&
          ($next_row->object_id === $origin) &&
          ($next_row->cv_id === $cv_id)) {
        $new_match_rows[] = $next_row;
      }
      elseif (($next_row->type_id === $possible_start_of_loop['type_id']) &&
              ($next_row->subject_id === $possible_start_of_loop['subject_id']) &&
              ($next_row->object_id === $possible_start_of_loop['object_id']) &&
              ($next_row->cv_id === $possible_start_of_loop['cv_id'])) {
        // The next_row is equal to start of loop, so we've reached the end
        // and confirmed that this is a loop.
        $possible_loop == FALSE;
        $matched_rows = array();
        tripal_update_cvtermpath_loop_increment($origin, $child_id, $cv_id,
        $type_id, $depth + 1, $increment_of_depth, $tree_path, $possible_loop,
        $new_match_rows, $possible_start_of_loop, $no_loop_skip_test);

      }
    }
    else {
      foreach ($matched_rows as $matched_row) {
        // Get the cvtermpath_id and then increment down one row.
        $cvtermpath_id = (int) $match_row->cvtermpath_id;
        // Get the cvtermpath_id and then increment down one row.
        $cvtermpath_id = $cvtermpath_id + 1;
        chado_set_active('chado');
        $next_row = db_query(
          'SELECT *
            FROM cvtermpath
            WHERE cvtermpath_id = :cvtermpath_id
          ',
          array(':cvtermpath_id' => $cvtermpath_id)
        );
        $next_row = $next_row->fetchObject();

        // If the next row matches the values passed we can't rule out a loop.
        if (($next_row->type_id === $type_id) &&
            ($next_row->subject_id === $child_id) &&
            ($next_row->object_id === $origin) &&
            ($next_row->cv_id === $cv_id)) {
          $new_match_rows[] = $next_row;
        }
        elseif (($next_row->type_id === $possible_start_of_loop['type_id']) &&
                ($next_row->subject_id === $possible_start_of_loop['subject_id']) &&
                ($next_row->object_id === $possible_start_of_loop['object_id']) &&
                ($next_row->cv_id === $possible_start_of_loop['cv_id'])) {
          // The next_row is equal to start of loop, so we've reached the end
          // and confirmed that this is a loop.
          $possible_loop == FALSE;
          $matched_rows = array();
          tripal_update_cvtermpath_loop_increment($origin, $child_id, $cv_id,
          $type_id, $depth + 1, $increment_of_depth, $tree_path, $possible_loop,
          $new_match_rows, $possible_start_of_loop, $no_loop_skip_test);

        }
      }
    }
    // If $match_rows is empty there is no loop.
    if (empty($new_match_rows)) {
      $possible_loop == FALSE;
      $matched_rows = array();
      unset($new_match_rows);
      $no_loop_skip_test = TRUE;
      // There is not loop so pass it back the possible_start_of_loop info
      // and a flag telling it to skip the loop check.
      tripal_update_cvtermpath_loop_increment($possible_start_of_loop->subject_id,
      $possible_start_of_loop->child_id, $possible_start_of_loop->cvid,
      $possible_start_of_loop->type_id, $possible_start_of_loop->depth,
      $increment_of_depth, $tree_path, $possible_loop, $matched_rows,
      $possible_start_of_loop, $no_loop_skip_test);
    }
    // If $match_rows is not empty we need to keep trying rows.
    else {
      tripal_update_cvtermpath_loop_increment($origin, $child_id, $cv_id,
      $type_id, $depth + 1, $increment_of_depth, $tree_path, $possible_loop,
      $match_rows, $possible_start_of_loop, $no_loop_skip_test);
    }
  }
  elseif ($possible_loop === FALSE) {
    tripal_update_cvtermpath_loop_increment($origin, $child_id, $cv_id,
    $type_id, $depth + 1, $increment_of_depth, $tree_path, $possible_loop,
    $matched_rows, $possible_start_of_loop, $no_loop_skip_test);
  }
}

/**
 *
 * @param $origin
 *   The root terms cvterm_id.
 * @param $child_id
 *   The cvterm_id of the current child term.  The child term is a descendent
 *   of the origin.
 * @param $cv_id
 *   The controlled vocabulary ID from the cv table of Chado (i.e. cv.cv_id).
 * @param $type_id
 *   The relationship type between the origin term and the child.
 * @param $depth
 *   The depth of the recursion.
 * @param $increment_of_depth.
 *   An integer ??
 * @param $tree_path.
 *   The array of every term between the current child and the origin. Each
 *   element in the array is an associative array with the keys:
 *     -build_id: an string identifier for the child that combines the origin,
 *      child cvterm_id,cv_id, and the type_id.
 *     -depth: the depth that a child was inserted into the cvtermpath table.
 * @param $possible_loop
 *    A boolean flag.
 * @param $matched_row
 *    An array of rows that are currently in the cvtermpath table that match the
 *    build_id of the current term trying to be written to the table
 * @param $possible_start_of_ loop
 *    The array of the possible loop item between the current child and the origin.
 *    Each element in the array is an associative array with the keys:
 *     - cvid : $cv_id
 *     - subject_id:
 *     - child_id : $child_id,
 *     - type_id : $type_id,
 *     - depth : $depth,
 * @param $no_loop_skip_test
 *     A boolean used when the possible loop has been ruled out as a loop.
 * @return multitype:
 */
function tripal_update_cvtermpath_loop_increment(
  $origin,
  $child_id,
  $cv_id,
  $type_id,
  $depth,
  $increment_of_depth,
  $tree_path,
  $possible_loop,
  $matched_rows,
  &$possible_start_of_loop,
  $no_loop_skip_test) {

  // Check to see if a row with these values already exists.
  if ($possible_loop === FALSE && empty($possible_start_of_loop)) {
    chado_set_active('chado');
    $count = db_query(
      'SELECT *
        FROM cvtermpath
        WHERE cv_id = :cvid
        AND object_id = :origin
        AND subject_id = :child_id
        AND pathdistance = :depth
      ',
      array(
        ':cvid' => $cv_id,
        ':origin' => $origin,
        ':child_id' => $child_id,
        ':depth' => $depth
      )
    );
    $count_total = $count->rowCount();
    if ($count_total > 0) {
      return $count;
    }
    // Build the ID.
    $term_id = $origin . '|' . $child_id . '|' . $cv_id . '|' . $type_id;

    if ($no_loop_skip_test === FALSE) {
      // Now check if the most recent entry already exists in the array.
      if ($increment_of_depth != 0) {
        // Search the $tree_path for the new $child_id in the build_id column.
        foreach ($tree_path as $parent) {
          // If this child is the same as a parent term that has already been
          // processed then we have a potential loop.
          if ($parent['build_id'] == $term_id) {
            // Tell the function this is a possible loop and to stop writing to the table.
            $possible_loop = TRUE;
            // Find all the results in the table that might be the start of the loop.
            $matching_rows = db_query(
              'SELECT *
              FROM cvtermpath
              WHERE cv_id = :cvid
              AND object_id = :origin
              AND subject_id = :child_id
              AND type_id = :type_id
              ',
              array(
                ':cvid' => $cv_id,
                ':origin' => $origin,
                ':child_id' => $child_id,
                ':type_id' => $type_id
              )
            );
            $matched_rows = $matching_rows->fetchAll();
            $possible_start_of_loop = array(
              'cvid' => $cv_id,
              'subject_id' => $origin,
              'child_id' => $child_id,
              'type_id' => $type_id,
              'depth' => $depth,
            );
          }
        }
      }

      $query = db_insert('cvtermpath')
        ->fields([
          'object_id' => $origin,
          'subject_id' => $child_id,
          'cv_id' => $cv_id,
          'type_id' => $type_id,
          'pathdistance' => $depth,
        ]);
      $rows = $query->execute();

      // Then add that new entry to the $tree_path.
      $tree_path[$increment_of_depth] = [
        'build_id' => $term_id,
        'depth' => $depth
      ];
    }
    // Reset to FALSE  and empty variable if passed in as TRUE.
    $no_loop_skip_test == FALSE;
    $possible_start_of_loop = array();

    // Get all of the relationships of this child term, and recursively
    // call the tripal_update_cvtermpath_loop() function to continue
    // descending down the tree.
    $query = db_select('cvterm_relationship', 'cvtr')
      ->fields('cvtr')
      ->condition('cvtr.object_id', $child_id, '=')
      ->execute();
    $cterm_relationships = $query->fetchAll();

    foreach ($cterm_relationships as $item) {
      $increment_of_depth++;
      tripal_update_cvtermpath_loop_increment($origin, $item->subject_id, $cv_id,
        $item->type_id, $depth + 1, $increment_of_depth, $tree_path, $possible_loop,
        $matched_rows, $possible_start_of_loop, $no_loop_skip_test);
    }
  }
  elseif ($possible_loop === FALSE && !empty($possible_start_of_loop)) {
    // This means a loop has been identified and the recursive call can move
    // on to the next item and skip the rest of this run.
    return $possible_start_of_loop;
  }
  elseif ($possible_loop === TRUE) {
    // Get all of the relationships of this child term, and recursively
    // call the tripal_update_cvtermpath_loop() function to continue
    // descending down the tree.
    $query = db_select('cvterm_relationship', 'cvtr')
      ->fields('cvtr')
      ->condition('cvtr.object_id', $child_id, '=')
      ->execute();
    $cterm_relationships = $query->fetchAll();
    foreach ($cterm_relationships as $item) {
      $increment_of_depth++;
      tripal_update_cvtermpath_loop($origin, $item->subject_id, $cv_id,
        $item->type_id, $depth + 1, $increment_of_depth, $tree_path, $possible_loop,
        $matched_rows, $possible_start_of_loop, $no_loop_skip_test);
    }
  }
}

/**
 * Adds a controlled vocabular to the CV table of Chado.
 *
 * @param $name
 *   The name of the controlled vocabulary. These are typically all lower case
 *   with no special characters other than an undrescore (for spaces).
 * @param $comment
 *   A description or definition of the vocabulary.
 *
 * @return
 *   An object populated with fields from the newly added database.
 *
 * @ingroup tripal_chado_api
 */
function tripal_insert_cv($name, $definition) {

  // insert/update values
  $ins_values = array(
    'name'       => $name,
    'definition' => $definition
  );

  // see if the CV default exists already in the database
  $sel_values = array('name' => $name);
  $results = chado_select_record('cv', array('*'), $sel_values);

  // if it does not exists then add it
  if (count($results) == 0) {
    $success = chado_insert_record('cv', $ins_values);
    if (!$success) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to create the CV record", NULL);
      return FALSE;
    }
    $results = chado_select_record('cv', array('*'), $sel_values);
  }
  // if it already exists then do an update
  else {
    $success = chado_update_record('cv', $sel_values, $ins_values);
    if (!$success) {
      tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to update the CV record", NULL);
      return FALSE;
    }
    $results = chado_select_record('cv', array('*'), $sel_values);
  }

  // return the cv object
  return $results[0];
}

/**
 *  Add's a controlled vocabulary term to Chado.
 *
 *  This function will add a cvterm record (and a dbxref record if appropriate
 *  values are provided). If the parent vocabulary does not exist then
 *  that also is added to the cv table.  If the cvterm is a relationship term
 *  then the 'is_relationship' value should be set.  All
 *  terms must also have a corresponding database.  This is specified in the
 *  term's ID just before the colon (e.g. GO:003824).  If the database does not
 *  exist in the DB table then it will be added automatically.  The accession
 *  (the value just after the colon in the term's ID) will be added to the
 *  dbxref table.  If the CVterm already exists and $update is set (default)
 *  then the cvterm is updated.  If the CVTerm already exists and $update is
 *  not set, then no changes are made and the CVTerm object is returned.
 *
 * @param $term
 *   An associative array with the following keys:
 *    - id: the term accession. must be of the form <DB>:<ACCESSION>, where
 *      <DB> is the name of the database to which the cvterm belongs and the
 *      <ACCESSION> is the term's accession number in the database.
 *    - name: the name of the term. usually meant to be human-readable.
 *    - is_obsolete: is present and set to 1 if the term is defunct
 *    - definition: the definition of the term
 *    - cv_name: The CV name to which the term belongs.  If this arugment is
 *        null or not provided then the function tries to find a record in the
 *        CV table with the same name provided in the $term[namespace].  If
 *        this field is provided then it overrides what the value in
 *        $term[namespace]
 *    - is_relationship: If this term is a relationship term then this value
 *        should be 1.
 *    - db_name: In some cases the database name will not be part of the
 *        $term['id'] and it needs to be explicitly set.  Use this argument
 *        only if the database name cannot be specififed in the term ID
 *        (e.g. <DB>:<ACCESSION>).
 * @param $options
 *   An associative array with the following keys:
 *    - update_existing: By default this is TRUE.  If the term exists it is
 *      automatically updated.
 *
 * @return
 *   A cvterm object
 *
 * @ingroup tripal_chado_api
 */
function tripal_insert_cvterm($term, $options = array()) {

  // get the term properties
  $id = (isset($term['id'])) ? $term['id'] : '';
  $name = '';
  $cvname = '';
  $definition = '';
  $is_obsolete = 0;
  $accession = '';

  // Set Defaults
  if (isset($term['cv_name'])) {
    $cvname = $term['cv_name'];
  }
  else {
    $cvname = 'local';
  }
  // Namespace is deprecated but must be supported for backwards
  // compatability
  if (array_key_exists('namespace', $term)) {
    $cvname = $term['namespace'];
  }

  if (isset($term['is_relationship'])) {
    $is_relationship = $term['is_relationship'];
  }
  else {
    $is_relationship = 0;
  }

  if (isset($term['db_name'])) {
    $dbname = $term['db_name'];
  }
  else {
    $dbname = 'internal';
  }

  if (isset($options['update_existing'])) {
    $update = $options['update_existing'];
  }
  else {
    $update = 1;
  }

  if (array_key_exists('name', $term)) {
    $name = $term['name'];
  }
  else {
    $name = $id;
  }


  if (array_key_exists('definition', $term)) {
    $definition = preg_replace('/^\"(.*)\"/', '\1', $term['definition']);
  }
  else {
    $definition = '';
  }
  if (array_key_exists('is_obsolete', $term)) {
    $is_obsolete = $term['is_obsolete'];
    if (strcmp($is_obsolete, 'true') == 0) {
      $is_obsolete = 1;
    }
  }
  if (!$name and !$id) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find cvterm without 'id' or 'name'", NULL);
    return 0;
  }
  if (!$id) {
    $id = $name;
  }

  // Get the accession and the database from the cvterm id.
  if ($dbname) {
    $accession = $id;
  }

  if (preg_match('/^.+?:.*$/', $id)) {
    $accession = preg_replace('/^.+?:(.*)$/', '\1', $id);
    $dbname = preg_replace('/^(.+?):.*$/', '\1', $id);
  }

  // Check that we have a database name, give a different message if it's a
  // relationship.
  if ($is_relationship and !$dbname) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "A database name is not provided for this relationship term: $id", NULL);
    return 0;
  }
  if (!$is_relationship and !$dbname) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "A database identifier is missing from the term: $id", NULL);
    return 0;
  }

  // Make sure the CV name exists
  $cv = tripal_get_cv(array('name' => $cvname));
  if (!$cv) {
    $cv = tripal_insert_cv($cvname, '');
  }
  if (!$cv) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find namespace '$cvname' when adding/updating $id", NULL);
    return 0;
  }

  // This SQL statement will be used a lot to find a cvterm so just set it
  // here for easy reference below.  Because CV terms can change their names
  // but accessions don't change, the following SQL finds cvterms based on
  // their accession rather than the name.
  $cvtermsql = "
    SELECT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name as cvname,
      DB.name as dbname, DB.db_id, DBX.accession
    FROM {cvterm} CVT
      INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
      INNER JOIN {db} DB on DBX.db_id = DB.db_id
      INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
    WHERE DBX.accession = :accession and DB.name = :name
  ";

  // Add the database. The function will just return the DB object if the
  // database already exists.
  $db = tripal_get_db(array('name' => $dbname));
  if (!$db) {
    $db = tripal_insert_db(array('name' => $dbname));
  }
  if (!$db) {
    tripal_report_error('tripal_cv', TRIPAL_WARNING, "Cannot find database '$dbname' in Chado.", NULL);
    return 0;
  }

  // The cvterm table has two unique dependencies. We need to check both.
  // first check the (name, cv_id, is_obsolete) constraint.
  $values = array(
    'name' => $name,
    'is_obsolete' => $is_obsolete,
    'cv_id' => array(
      'name' => $cvname,
    ),
  );
  $result = chado_select_record('cvterm', array('*'), $values);
  if (count($result) == 1) {
    $cvterm = $result[0];

    // Get the dbxref record.
    $values = array('dbxref_id' => $cvterm->dbxref_id);
    $result = chado_select_record('dbxref', array('*'), $values);
    $dbxref = $result[0];
    if (!$dbxref) {
      tripal_report_error('tripal_cv', TRIPAL_ERROR,
          'Unable to access the dbxref record for the :term cvterm. Term Record: !record',
          array(':term' => $name, '!record' => print_r($cvterm, TRUE))
      );
      return FALSE;
    }

    // Get the db.
    $values = array('db_id' => $dbxref->db_id);
    $result = chado_select_record('db', array('*'), $values);
    $db_check = $result[0];

    //     // The database name for this existing term does not match that of the
    //     // one provided to this function.  The CV name matches otherwise we
    //     // wouldn't have made it this far. So, let's swap the database for
    //     // this term.
    //     if ($db_check->name != $db->name) {

    //       // Look to see if the correct dbxref record already exists for this
    //       // database.
    //       $values = array(
    //         'db_id' => $db->db_id,
    //         'accession' => $accession,
    //       );
    //       $result = chado_select_record('dbxref', array('*'), $values);

    //       // If we already have a good dbxref then we want to update our cvterm
    //       // to use this dbxref.
    //       if (count($result) > 0) {
    //         $dbxref = $result[0];
    //         $match = array('cvterm_id' => $cvterm->cvterm_id);
    //         $values = array('dbxref_id' => $dbxref->dbxref_id);
    //         $success = chado_update_record('cvterm', $match, $values);
    //         if (!$success) {
    //           tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to correct the dbxref id for the cvterm " .
    //             "'$name' (id: $accession), for database $dbname", NULL);
    //           return 0;
    //         }
    //       }
    //       // If we don't have the dbxref then we want to delete our cvterm and let
    //       // the code below recreate it with the correct info.
    //       else {
    //         $match = array('cvterm_id' => $cvterm->cvterm_id);
    //         chado_delete_record('cvterm', $match);
    //       }
    //     }

    // Check that the accession matches.  Sometimes an OBO can define a term
    // multiple times but with different accessions.  If this is the case we
    // can't do an insert or it will violate the constraint in the cvterm table.
    // So we'll need to add the record to the cvterm_dbxref table instead.
    if ($dbxref->accession != $accession) {

      // Get/add the dbxref for his term.
      $dbxref_new =  tripal_insert_dbxref(array(
        'db_id' => $db->db_id,
        'accession' => $accession
      ));
      if (!$dbxref_new) {
        tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
            "$name (id: $accession), for database $dbname", NULL);
        return 0;
      }

      // Check to see if the cvterm_dbxref record already exists.
      $values = array(
        'cvterm_id' => $cvterm->cvterm_id,
        'dbxref_id' => $dbxref_new->dbxref_id,
        'is_for_definition' => 1,
      );
      $result = chado_select_record('cvterm_dbxref', array('*'), $values);

      // if the cvterm_dbxref record does not exists then add it
      if (count($result)==0) {
        $options = array(
          'return_record' => FALSE,
        );
        $success = chado_insert_record('cvterm_dbxref', $values, $options);
        if (!$success) {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the cvterm_dbxref record for a " .
              "duplicated cvterm:  $name (id: $accession), for database $dbname", NULL);
          return 0;
        }
      }
      // get the original cvterm with the same name and return that.
      $result = chado_query($cvtermsql, array(':accession' => $dbxref->accession, ':name' => $dbname));
      $cvterm = $result->fetchObject();
      return $cvterm;
    }
    // Continue on, we've fixed the record if the db_id did not match.
    // We can now perform and updated if we need to.
  }

  // get the CVterm record
  $result = chado_query($cvtermsql, array(':accession' => $accession, ':name' => $dbname));
  $cvterm = $result->fetchObject();
  if (!$cvterm) {

    // Check to see if the dbxref exists if not, add it.
    $dbxref =  tripal_insert_dbxref(array(
      'db_id' => $db->db_id,
      'accession' => $accession
    ));
    if (!$dbxref) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
          "$name (id: $accession), for database $dbname", NULL);
      return 0;
    }

    // Check to see if the dbxref already has an entry in the cvterm table.
    $values = array('dbxref_id' => $dbxref->dbxref_id);
    $check = chado_select_record('cvterm', array('cvterm_id'), $values);
    if (count($check) == 0) {
      // now add the cvterm
      $ins_values = array(
        'cv_id'                => $cv->cv_id,
        'name'                 => $name,
        'definition'           => $definition,
        'dbxref_id'            => $dbxref->dbxref_id,
        'is_obsolete'          => $is_obsolete,
        'is_relationshiptype'  => $is_relationship,
      );
      $success = chado_insert_record('cvterm', $ins_values);
      if (!$success) {
        if (!$is_relationship) {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to insert the term: $name ($dbname)", NULL);
          return 0;
        }
        else {
          tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to insert the relationship term: $name (cv: " . $cvname . " db: $dbname)", NULL);
          return 0;
        }
      }
    }
    // This dbxref already exists in the cvterm table.
    else {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "The dbxref already exists for another cvterm record: $name (cv: " . $cvname . " db: $dbname)", NULL);
      return 0;
    }
    $result = chado_query($cvtermsql, array(':accession' => $accession, ':name' => $dbname));
    $cvterm = $result->fetchObject();
  }
  // Update the cvterm.
  elseif ($update) {

    // First, basic update of the term.
    $match = array('cvterm_id' => $cvterm->cvterm_id);
    $upd_values = array(
      'name'                => $name,
      'definition'          => $definition,
      'is_obsolete'         => $is_obsolete,
      'is_relationshiptype' => $is_relationship,
    );
    $success = chado_update_record('cvterm', $match, $upd_values);
    if (!$success) {
      tripal_report_error('tripal_cv', TRIPAL_WARNING, "Failed to update the term: $name", NULL);
      return 0;
    }

    // Second, check that the dbxref has not changed and if it has then update it.
    $checksql = "
      SELECT cvterm_id
      FROM {cvterm} CVT
        INNER JOIN {dbxref} DBX on CVT.dbxref_id = DBX.dbxref_id
        INNER JOIN {db} DB on DBX.db_id = DB.db_id
        INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
      WHERE DBX.accession = :accession and DB.name = :dbname and CVT.name = :term and CV.name = :cvname
    ";
    $check = chado_query($checksql, array(':accession' => $accession, ':dbname' => $dbname, ':term' => $name, ':cvname' => $cvname))->fetchObject();
    if (!$check) {

      // check to see if the dbxref exists if not, add it.
      $dbxref =  tripal_insert_dbxref(array(
        'db_id' => $db->db_id,
        'accession' => $accession
      ));
      if (!$dbxref) {
        tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to find or insert the dbxref record for cvterm, " .
            "$name (id: $accession), for database $dbname", NULL);
        return 0;
      }

      $match = array('cvterm_id' => $cvterm->cvterm_id);
      $upd_values = array(
        'dbxref_id' => $dbxref->dbxref_id,
      );
      $success = chado_update_record('cvterm', $match, $upd_values);
      if (!$success) {
        tripal_report_error('tripal_chado', TRIPAL_WARNING, "Failed to update the term $name with new accession $db:$accession", NULL);
        return 0;
      }
    }

    // Finally grab the updated details.
    $result = chado_query($cvtermsql, array(':accession' => $accession, ':name' => $dbname));
    $cvterm = $result->fetchObject();
  }
  else {
    // do nothing, we have the cvterm but we don't want to update
  }
  // return the cvterm
  return $cvterm;
}


/**
 * TODO: deprecate this function
 *
 * This function allows other modules to programatically
 * submit an ontology for loading into Chado.
 *
 * This function will add a job to the Jobs subsystem for parsing the ontology.
 * You can either pass a known OBO ID to the function or the URL
 * or full path the the ontology file.  If a URL or file name is
 * passed then the $obo_name argument must also be provided.  If
 * this is the first time the ontology has been provided to Tripal
 * then it will be added to the database and will be assigned a
 * unique OBO ID.
 *
 * @param $obo_id
 *   If the ontology is already loaded into the Tripal tables then
 *   use this argument to specify the unique ID for the ontology
 *   that will be loaded
 * @param $obo_name
 *   If the OBO has not been added before then use this argument
 *   to specify the human readable name of the ontology.
 * @param $obo_url
 *   If the OBO to be loaded is located on a remote server then
 *   use this argument to provide the URL.
 * @param $obo_file
 *   If the OBO is housed on the local file system of the server then
 *   use this argument to specify the full path.
 *
 * @return
 *   returns the job_id of the submitted job or FALSE if the job was not added
 *
 * @ingroup tripal_chado_api
 */
function tripal_submit_obo_job($obo) {
  global $user;

  // Set Defaults
  $obo['obo_id'] = (isset($obo['obo_id'])) ? $obo['obo_id'] : NULL;
  $obo['name']   = (isset($obo['name']))   ? $obo['name']   : NULL;
  $obo['url']    = (isset($obo['url']))    ? $obo['url']    : NULL;
  $obo['file']   = (isset($obo['file']))   ? $obo['file']   : NULL;

  $includes = array(
    drupal_get_path('module', 'tripal_chado') . '/includes/tripal_chado.cv.inc',
  );

  if ($obo['obo_id']) {
    $sql = "SELECT * FROM {tripal_cv_obo} WHERE obo_id = :obo_id";
    $result = db_query($sql, array(':obo_id' => $obo['obo_id']))->fetchObject();

    $args = array($result->obo_id);
    return tripal_add_job("Load OBO " . $result->name, 'tripal_chado',
       "tripal_cv_load_obo", $args, $user->uid, 10, $includes);
  }
  else {
    if ($obo['url']) {
      $sql = "SELECT * FROM {tripal_cv_obo} WHERE path = :url";
      $result = db_query($sql, array(':url' => $obo['url']))->fetchObject();

      $args = array($result->obo_id);
      return tripal_add_job("Load OBO " . $result->name, 'tripal_chado',
        "tripal_cv_load_obo", $args, $user->uid, 10, $includes);
    }
    elseif ($obo['file']) {
      $sql = "SELECT * FROM {tripal_cv_obo} WHERE path = :file";
      $result = db_query($sql, array(':url' => $obo['file']))->fetchObject();

      $args = array($result->obo_id);
      return tripal_add_job("Load OBO " . $result->name, 'tripal_chado',
        "tripal_cv_load_obo", $args, $user->uid, 10, $includes);
    }
  }
  return FALSE;
}

/**
 * Add the OBO to the tripal_cv_obo table in the Drupal database.
 *
 * If the OBO name already exists in the table then the path is updated.
 *
 * @param $name
 *   The human readable name of this ontology
 * @param $path
 *   The file path or URL of the ontology
 *
 * @return
 *   Returns the ontology ID
 *
 * @ingroup tripal_chado_api
 */
function tripal_insert_obo($name, $path) {
  // make sure an OBO with the same name doesn't already exist
  $obo_id = db_select('tripal_cv_obo', 'tco')
    ->fields('tco', array('obo_id'))
    ->condition('name', $name)
    ->execute()
    ->fetchField();

  if ($obo_id) {
    db_update('tripal_cv_obo')
      ->fields(array(
        'path' => $path,
      ))
      ->condition('name', $name)
      ->execute();
    return $obo_id;
  }
  else {
    $obo_id = db_insert('tripal_cv_obo')
      ->fields(array(
        'name' => $name,
        'path' => $path,
      ))
      ->execute();
    return $obo_id;
  }
}

/**
 * Retrieves an OBO record.
 *
 * @param $values
 *   An associate array with the following allowed keys: obo_id, name
 *
 * @return
 *   An instance of an OBO record object.
 */
function tripal_get_obo($values) {
  $query = db_select('tripal_cv_obo', 'tco')
    ->fields('tco');

  if (array_key_exists('name', $values)) {
    $query->condition('tco.name', $values['name']);
  }
  if (array_key_exists('obo_id', $values)) {
    $query->condition('tco.obo_id', $values['obo_id']);
  }
  return $query->execute()->fetchObject();
}
/**
 * This function is intended to be used in autocomplete forms.
 *
 * This function searches for a matching controlled vobulary name.
 *
 * @param $string
 * The string to search for
 *
 * @return
 * A json array of terms that begin with the provided string
 *
 * @ingroup tripal_chado_api
 */
function tripal_autocomplete_cv($string = '') {
  $sql = "
    SELECT CV.cv_id, CV.name
    FROM {cv} CV
    WHERE lower(CV.name) like lower(:name)
    ORDER by CV.name
    LIMIT 25 OFFSET 0
  ";
  $results = chado_query($sql, array(':name' => $string . '%'));
  $items = array();
  foreach ($results as $cv) {
    $items[$cv->name] = $cv->name;
  }

  drupal_json_output($items);
}
/**
 * This function is intended to be used in autocomplete forms
 * for searching for CV terms that begin with the provided string
 *
 * @param $cv_id
 * The CV ID in which to search for the term
 * @param $string
 * The string to search for
 *
 * @return
 * A json array of terms that begin with the provided string
 *
 * @ingroup tripal_chado_api
 */
function tripal_autocomplete_cvterm($cv_id, $string = '') {
  if ($cv_id) {
    $sql = "
      SELECT CVT.cvterm_id, CVT.name
      FROM {cvterm} CVT
      WHERE CVT.cv_id = :cv_id and lower(CVT.name) like lower(:name)
      UNION
      SELECT CVT2.cvterm_id, CVTS.synonym as name
      FROM {cvterm} CVT2
        INNER JOIN {cvtermsynonym} CVTS ON CVTS.cvterm_id = CVT2.cvterm_id
      WHERE CVT2.cv_id = :cv_id and lower(CVTS.synonym) like lower(:name)
      ORDER by name
      LIMIT 25 OFFSET 0
    ";
    $results = chado_query($sql, array(':cv_id' => $cv_id, ':name' => $string . '%'));
    $items = array();
    foreach ($results as $term) {
      $items[$term->name] = $term->name;
    }
  }
  // If a CV wasn't provided then search all of them, and include the cv
  // in the results.
  else {
    $sql = "
      SELECT CVT.cvterm_id, CVT.name, CV.name as cvname, CVT.cv_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CVT.cv_id = CV.cv_id
      WHERE lower(CVT.name) like lower(:name)
      UNION
      SELECT CVT2.cvterm_id, CVTS.synonym as name, CV2.name as cvname, CVT2.cv_id
      FROM {cvterm} CVT2
        INNER JOIN {cv} CV2 on CVT2.cv_id = CV2.cv_id
        INNER JOIN {cvtermsynonym} CVTS ON CVTS.cvterm_id = CVT2.cvterm_id
      WHERE lower(CVTS.synonym) like lower(:name)
      ORDER by name
      LIMIT 25 OFFSET 0
    ";
    $results = chado_query($sql, array(':name' => $string . '%'));
    $items = array();
    foreach ($results as $term) {
      $items[$term->name] = $term->name;
    }
  }

  drupal_json_output($items);
}

/**
 * Add a record to a cvterm linking table (ie: feature_cvterm)
 *
 * @param $basetable
 *   The base table to which the cvterm should be linked/associated. Thus to associate a
 *   cvterm to a feature the basetable=feature and cvterm_id is added to the feature_cvterm table.
 * @param $record_id
 *   The primary key of the basetable to associate the cvterm with. This should be in integer.
 * @param $cvterm
 *   An associative array describing the cvterm. Valid keys include:
 *     - name: the name for the cvterm,
 *     - cv_name: the name of the cv the cvterm belongs to.
 *     - cv_id: the primary key of the cv the cvterm belongs to.
 * @param $options
 *   An associative array of options. Valid keys include:
 *     - insert_cvterm: Insert the cvterm if it doesn't already exist. FALSE is the default
 *
 * @ingroup tripal_chado_api
 */
function tripal_associate_cvterm($basetable, $record_id, $cvterm, $options = array()) {
  $linking_table = $basetable . '_cvterm';
  $foreignkey_name = $basetable . '_id';

  // Default Values
  $options['insert_cvterm'] = (isset($options['insert_cvterm'])) ? $options['insert_cvterm'] : FALSE;

  // If the cvterm_id is not set then find the cvterm record using the name and cv_id
  if (!isset($cvterm['cvterm_id'])) {
    $values = array(
      'name' => $cvterm['name'],
    );
    if (isset($cvterm['cv_id'])) {
      $values['cv_id'] = $cvterm['cv_id'];
    }
    elseif (isset($cvterm['cv_name'])) {
      $values['cv_id'] = array(
        'name' => $cvterm['cv_name']
      );
    }
    else {
      tripal_report_error('tripal_chado_api', TRIPAL_WARNING,
        "tripal_associate_cvterm: The cvterm needs to have either the cv_name or cv_id
          supplied. You were trying to associate a cvterm with the %base %record_id
          and supplied the cvterm values: %cvterm.",
        array('%base' => $basetable, '%record_id' => $record_id, '%cvterm' => print_r($cvterm,TRUE))
      );
      return FALSE;
    }

    // Get the cvterm. If it doesn't exist then add it if the option
    // 'insert_cvterm' is set.
    $select = chado_select_record('cvterm', array('*'), $values);
    if ($select) {
      $cvterm['cvterm_id'] = $select[0]->cvterm_id;
    }
    elseif ($options['insert_cvterm']) {
      // Insert the cvterm
      $insert = tripal_insert_cvterm($values);
      if (isset($insert->cvterm_id)) {
        $cvterm['cvterm_id'] = $insert->cvterm_id;
      }
      else {
        tripal_report_error('tripal_chado_api', TRIPAL_WARNING,
          "tripal_associate_cvterm: Unable to insert the cvterm using the cvterm values: %cvterm.",
          array('%cvterm' => print_r($cvterm,TRUE))
        );
        return FALSE;
      }
    }
    else {
      tripal_report_error('tripal_api', TRIPAL_WARNING,
        "tripal_associate_cvterm: The cvterm doesn't already exist. You supplied the cvterm values: %cvterm.",
        array('%cvterm' => print_r($cvterm,TRUE))
      );
      return FALSE;
    }
  }

  // Now add the link between the record & cvterm
  if ($cvterm['cvterm_id'] > 0) {
    $values = array(
      'cvterm_id' => $cvterm['cvterm_id'],
      $foreignkey_name => $record_id,
      'pub_id' => 1,
    );

    // Check if the cvterm is already associated. If so, don't re-add it.
    $result = chado_select_record($linking_table, array('*'), $values);
    if (!$result) {
      $success = chado_insert_record($linking_table, $values);
      if (!$success) {
        tripal_report_error('tripal_api', TRIPAL_WARNING,
          "Failed to insert the %base record %term",
          array('%base' => $linking_table, '%term' => $cvterm['name'])
        );
        return FALSE;
      }
      $result = chado_select_record($linking_table, array('*'), $values);
    }

    if (isset($result[0])) {
      return $result[0];
    }
    else {
      return FALSE;
    }
  }

  return FALSE;
}


