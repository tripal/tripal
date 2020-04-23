<?php
/**
 * @file
 * Provides an API for querying of chado including inserting, updating, deleting
 * and selecting from chado.
 */

/**
 * @defgroup tripal_chado_query_api Chado Query
 * @ingroup tripal_chado_api
 * @{
 * Provides an API for querying of chado including inserting, updating,
 *   deleting
 * and selecting from specific chado tables. There is also a generic function,
 * chado_query(), to execute and SQL statement on chado. It is ideal to use
 * these functions to interact with chado in order to keep your module
 * compatible with both local & external chado databases. Furthermore, it
 * ensures connection to the chado database is taken care of for you.
 *
 * Generic Queries to a specifc chado table:
 *
 * chado_select_record( [table name], [columns to select],
 * [specify record to select], [options*] ) This function allows you to select
 * various columns from the specified chado table. Although you can only select
 * from a single table, you can specify the record to select using values
 * from related tables through use of a nested array. For example, the
 *   following
 * code shows you how to select the name and uniquename of a feature based on
 * it's type and source organism.
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => 'sinensis',
 *      ),
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = chado_select_record(
 *      'feature',                      // table to select from
 *      array('name', 'uniquename'),    // columns to select
 *      $values                         // record to select (see variable defn.
 *                                                           above)
 *   );
 * @endcode
 *
 * chado_insert_record( [table name], [values to insert], [options*] )
 * This function allows you to insert a single record into a specific table.
 *   The
 * values to insert are specified using an associative array where the keys are
 * the column names to insert into and they point to the value to be inserted
 * into that column. If the column is a foreign key, the key will point to an
 * array specifying the record in the foreign table and then the primary key of
 * that record will be inserted in the column. For example, the following code
 * will insert a feature and for the type_id, the cvterm.cvterm_id of the
 *   cvterm
 * record will be inserted and for the organism_id, the organism.organism_id
 * of the organism_record will be inserted.
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => 'sinensis',
 *      ),
 *     'name' => 'orange1.1g000034m.g',
 *     'uniquename' => 'orange1.1g000034m.g',
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = chado_insert_record(
 *     'feature',             // table to insert into
 *     $values                // values to insert
 *   );
 * @endcode
 *
 * chado_update_record( [table name], [specify record to update],
 * [values to change], [options*] ) This function allows you to update records
 * in a specific chado table. The record(s) you wish to update are specified
 *   the
 * same as in the select function above and the values to be update are
 * specified the same as the values to be inserted were. For example, the
 * following code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be updated
 * with a new name, and the type changed from a gene to an mRNA.
 * @code
 * $umatch = array(
 *   'organism_id' => array(
 *     'genus' => 'Citrus',
 *     'species' => 'sinensis',
 *   ),
 *   'uniquename' => 'orange1.1g000034m.g7',
 *   'type_id' => array (
 *     'cv_id' => array (
 *       'name' => 'sequence',
 *     ),
 *     'name' => 'gene',
 *     'is_obsolete' => 0
 *   ),
 * );
 * $uvalues = array(
 *   'name' => 'orange1.1g000034m.g',
 *   'type_id' => array (
 *     'cv_id' => array (
 *       'name' => 'sequence',
 *     ),
 *     'name' => 'mRNA',
 *     'is_obsolete' => 0
 *   ),
 * );
 *   $result = chado_update_record('feature',$umatch,$uvalues);
 * @endcode
 *
 * chado_delete_record( [table name], [specify records to delete], [options*] )
 * This function allows you to delete records from a specific chado table. The
 * record(s) to delete are specified the same as the record to select/update
 *   was
 * above. For example, the following code will delete all genes from the
 * organism Citrus sinensis.
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => 'sinensis',
 *      ),
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = chado_select_record(
 *      'feature',                      // table to select from
 *      $values                         // records to delete (see variable
 *   defn.
 *                                                            above)
 *   );
 * @endcode
 *
 * Generic Queries for any SQL:
 *
 * Often it is necessary to select from more then one table in chado or to
 * execute other complex queries that cannot be handled efficiently by the
 *   above
 * functions. It is for this reason that the chado_query( [sql string],
 * [arguments to sub-in to the sql] ) function was created. This function
 *   allows
 * you to execute any SQL directly on the chado database and should be used
 *   with
 * care. If any user input will be used in the query make sure to put a
 * placeholder in your SQL string and then define the value in the arguments
 * array. This will make sure that the user input is santized and safe through
 * type-checking and escaping. The following code shows an example of how to
 * use user input resulting from a form and would be called withing the form
 * submit function.
 * @code
 * $sql = "SELECT F.name, CVT.name as type_name, ORG.common_name
 *          FROM feature F
 *          LEFT JOIN cvterm CVT ON F.type_id = CVT.cvterm_id
 *          LEFT JOIN organism ORG ON F.organism_id = ORG.organism_id
 *          WHERE
 *            F.uniquename = :feature_uniquename";
 * $args = array( ':feature_uniquename' => $form_state['values']['uniquename']
 *   );
 * $result = chado_query( $sql, $args );
 * foreach ($result as $r) { [Do something with the records here] }
 * @endcode
 *
 * If you are going to need more then a couple fields, you might want to use
 *   the
 * Chado Variables API (specifically chado_generate_var()) to select all
 * of the common fields needed including following foreign keys.
 *
 * Loading of Variables from chado data:
 *
 * These functions, chado_generate_var() and  chado_expand_var(), generate
 * objects containing the full details of a record(s) in chado. These should be
 * used in all theme templates.
 *
 * This differs from the objects returned by chado_select_record in so far as
 * all foreign key relationships have been followed meaning you have more
 * complete details. Thus this function should be used whenever you need a full
 * variable and chado_select_record should be used if you only case about a few
 * columns.
 *
 * The initial variable is generated by the
 * chado_generate_var([table], [filter criteria], [optional options])
 * function. An example of how to use this function is:
 * @code
 * $values = array(
 * 'name' => 'Medtr4g030710'
 * );
 * $features = chado_generate_var('feature', $values);
 * @endcode
 * This will return an object if there is only one feature with the name
 * Medtr4g030710 or it will return an array of feature objects if more than one
 * feature has that name.
 *
 * Some tables and fields are excluded by default. To have those tables &
 *   fields
 * added to your variable you can use the
 * chado_expand_var([chado variable], [type], [what to expand],
 * [optional options]) function. An example of how to use this function is:
 * @code
 *
 * Get a chado object to be expanded
 * $values = array(
 * 'name' => 'Medtr4g030710'
 * );
 * $features = chado_generate_var('feature', $values);
 * Expand the organism node
 * $feature = chado_expand_var($feature, 'node', 'organism');
 * Expand the feature.residues field
 * $feature = chado_expand_var($feature, 'field', 'feature.residues');
 * Expand the feature properties (featureprop table)
 * $feature = chado_expand_var($feature, 'table', 'featureprop');
 * @endcode
 */


/**
 * Get max rank for a given set of criteria.
 *
 * This function was developed with the many property tables in chado in mind
 * but will work for any table with a rank.
 *
 * @param tablename: the name of the chado table you want to select the max
 *            rank from this table must contain a rank column of type integer.
 * @param where_options: array(
 *   <column_name> => array(
 *     'type' => <type of column: INT/STRING>,
 *     'value' => <the value you want to filter on>,
 *     'exact' => <if TRUE use =; if FALSE use ~>,
 *    )
 *  )
 *  where options should include the id and type for that table to correctly
 *  group a set of records together where the only difference are the value and
 *  rank.
 *
 * @return
 *  The maximum rank.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_get_table_max_rank($tablename, $where_options) {

  $where_clauses = [];
  $where_args = [];

  //generate the where clause from supplied options
  // the key is the column name
  $i = 0;
  $sql = "
    SELECT max(rank) as max_rank, count(rank) as count
    FROM {" . $tablename . "}
    WHERE
  ";
  foreach ($where_options as $key => $value) {
    $where_clauses[] = "$key = :$key";
    $where_args[":$key"] = $value;
  }
  $sql .= implode($where_clauses, ' AND ');

  $result = chado_query($sql, $where_args)->fetchObject();
  if ($result->count > 0) {
    return $result->max_rank;
  }
  else {
    return -1;
  }
}

/**
 * Alter Chado connection settings.
 *
 * This hook is useful for multi-chado instances. Tripal core functions
 * call the chado_set_active() function (e.g. chado_query) but there is no
 * opportunity elsewhere to set the active database.  This is useful in two
 * cases:  1) Users are managed at the database level as in the case of
 * SouthGreen Bioinformatics Platform tools (e.g. Banana Genone Hub).
 * This allows custom modules to change the database connections on a per-user
 * basis, and each user permissions is managed at the database level.  Users
 * are managed at the database level to provid the same access restrictions
 * across various tools that use Chado (e,g, Artemis) 2) When there are
 * simply two Chado instances housed in different Chado databases and the
 * module needs to control which one is being used at any given time.
 *
 * @param $settings
 *   An array containing
 *
 * @see chado_set_active()
 *
 * @ingroup tripal_chado_query_api
 */
function hook_chado_connection_alter(&$settings) {
  // This example shows how we could make sure no table of the 'public' schema
  // would be allowed in the coming queries: to do so, the caller will call
  // "chado_set_active('chado_only');" and the hook will remove 'public' from
  // the search path.
  if ('chado_only' == $settings['dbname']) {
    $settings['new_active_db'] = 'chado';
    // We don't include 'public' in search path.
    $settings['new_search_path'] = 'chado';
  }
}

/**
 * Set the Tripal Database
 *
 * The chado_set_active function is used to prevent namespace collisions
 * when Chado and Drupal are installed in the same database but in different
 * schemas. It is also used when using Drupal functions such as
 * db_table_exists().
 *
 * The connection settings can be altered through the hook
 * hook_chado_connection_alter.
 *
 * Current active connection name is stored in the global variable
 * $GLOBALS['chado_active_db'].
 *
 * @see hook_chado_connection_alter()
 *
 * @param $dbname
 *
 * @return
 *  Global variable $GLOBALS['chado_active_db'].
 *
 * @ingroup tripal_chado_query_api
 */
function chado_set_active($dbname = 'default') {

  // Check if the chado_active_db has been set yet.
  if (!array_key_exists('chado_active_db', $GLOBALS)) {
    $GLOBALS['chado_active_db'] = 'default';
  }

  $previous_db = $active_db = $GLOBALS['chado_active_db'];
  $search_path = chado_get_schema_name('drupal');

  // Change only if 'chado' has been specified.
  // Notice that we leave the active_db set as chado but use the possibly
  // user-altered  schema name for the actual search path. This is to keep
  // outward facing mentions of chado as "chado" while still allowing the user
  // to alter the schema name used.
  if ($dbname == 'chado') {
    $active_db = 'chado';
    $search_path = chado_get_schema_name('chado') . ',' . chado_get_schema_name('drupal');
  }
  else {
    $active_db = $dbname;
  }

  $settings = [
    'dbname' => $dbname,
    'new_active_db' => &$active_db,
    'new_search_path' => &$search_path,
  ];

  // Will call all modules implementing hook_chado_search_path_alter
  // note: hooks can alter $active_db and $search_path.
  drupal_alter('chado_connection', $settings);

  // set chado_active_db to remember active db
  $GLOBALS['chado_active_db'] = $active_db;

  // set PostgreSQL search_path
  db_query('SET search_path TO ' . $search_path);

  return $previous_db;
}


/**
 * Provides a generic routine for inserting into any Chado table
 *
 * Use this function to insert a record into any Chado table.  The first
 * argument specifies the table for inserting and the second is an array
 * of values to be inserted.  The array is mutli-dimensional such that
 * foreign key lookup values can be specified.
 *
 * @param $table
 *  The name of the chado table for inserting
 * @param $values
 *  An associative array containing the values for inserting.
 * @param $options
 *  An array of options such as:
 *  - skip_validation: TRUE or FALSE. If TRUE will skip all the validation
 *   steps and just try to insert as is. This is much faster but results in
 *   unhandled non user-friendly errors if the insert fails.
 *  - return_record: by default, the function will return the record but with
 *     the primary keys added after insertion.  To simply return TRUE on
 *   success
 *     set this option to FALSE
 *
 * @return
 *  On success this function returns the inserted record with the new primary
 *   keys added to the returned array. On failure, it returns FALSE.
 *
 * Example usage:
 * @code
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => 'sinensis',
 *      ),
 *     'name' => 'orange1.1g000034m.g',
 *     'uniquename' => 'orange1.1g000034m.g',
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $result = chado_insert_record('feature',$values);
 * @endcode
 * The above code inserts a record into the feature table.  The $values array
 *   is
 * nested such that the organism is selected by way of the organism_id foreign
 * key constraint by specifying the genus and species.  The cvterm is also
 * specified using its foreign key and the cv_id for the cvterm is nested as
 * well.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_insert_record($table, $values, $options = []) {

  $print_errors = (isset($options['print_errors'])) ? $options['print_errors'] : FALSE;

  if (!is_array($values)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass non array as values for inserting.', [],
      ['print' => $print_errors]
    );
    return FALSE;
  }
  if (count($values) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass an empty array as values for inserting.',
      [], ['print' => $print_errors]
    );
    return FALSE;
  }

  // Set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements.
  if (!is_array($options)) {
    $options = [];
  }

  if (!array_key_exists('skip_validation', $options)) {
    $options['skip_validation'] = FALSE;
  }
  if (!array_key_exists('return_record', $options)) {
    $options['return_record'] = TRUE;
  }

  $insert_values = [];

  if (array_key_exists('skip_validation', $options)) {
    $validate = !$options['skip_validation'];
  }
  else {
    $validate = TRUE;
  }

  // Get the table description.
  $table_desc = chado_get_schema($table);
  if (!$table_desc) {
    tripal_report_error('tripal_chado', TRIPAL_WARNING,
      'chado_insert_record; There is no table description for !table_name',
      ['!table_name' => $table], ['print' => $print_errors]
    );
    return;
  }

  // Iterate through the values array and create a new 'insert_values' array
  // that has all the values needed for insert with all foreign relationsihps
  // resolved.
  foreach ($values as $field => $value) {
    // Make sure the field is in the table description. If not then return an
    // error message.
    if (!array_key_exists($field, $table_desc['fields'])) {
      tripal_report_error('tripal_chado', TRIPAL_ERROR,
        "chado_insert_record; The field '%field' does not exist " .
        "for the table '%table'.  Cannot perform insert. Values: %array",
        [
          '%field' => $field,
          '%table' => $table,
          '%array' => print_r($values, 1),
        ],
        ['print' => $print_errors]
      );
      return FALSE;
    }

    if (is_array($value)) {
      // Select the value from the foreign key relationship for this value.
      $results = chado_schema_get_foreign_key($table_desc, $field, $value);

      if (sizeof($results) > 1) {
        tripal_report_error('tripal_chado', TRIPAL_ERROR,
          'chado_insert_record: Too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
          ['!foreign_key' => $field, '!criteria' => print_r($value, TRUE)],
          ['print' => $print_errors]
        );
        return FALSE;
      }
      elseif (sizeof($results) < 1) {
        tripal_report_error('tripal_chado', TRIPAL_DEBUG,
          'chado_insert_record: no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)',
          ['!foreign_key' => $field, '!criteria' => print_r($value, TRUE)],
          ['print' => $print_errors]
        );
        return FALSE;
      }
      else {
        $insert_values[$field] = $results[0];
      }
    }
    else {
      $insert_values[$field] = $value;
    }
  }

  if ($validate) {

    // Check for violation of any unique constraints.
    $ukeys = [];
    if (array_key_exists('unique keys', $table_desc)) {
      $ukeys = $table_desc['unique keys'];
    }
    $ukselect_cols = [];
    $ukselect_vals = [];
    if ($ukeys) {
      foreach ($ukeys as $name => $fields) {
        foreach ($fields as $index => $field) {
          // Build the arrays for performing a select that will check the constraint.
          $ukselect_cols[] = $field;
          if (!array_key_exists($field, $insert_values)) {
            if (array_key_exists('default', $table_desc['fields'][$field])) {
              $ukselect_vals[$field] = $table_desc['fields'][$field]['default'];
            }
          }
          else {
            $ukselect_vals[$field] = $insert_values[$field];
          }
        }
        // Now check the constraint.
        if (chado_select_record($table, $ukselect_cols, $ukselect_vals)) {
          tripal_report_error('tripal_chado', TRIPAL_ERROR,
            "chado_insert_record; Cannot insert duplicate record into $table table: !values",
            ['!values' => print_r($values, TRUE)], ['print' => $print_errors]
          );
          return FALSE;
        }
      }
    }

    // If trying to insert a field that is the primary key, make sure it also
    // is unique.
    if (array_key_exists('primary key', $table_desc)) {
      $pkey = $table_desc['primary key'][0];
      if (array_key_exists($pkey, $insert_values)) {
        $coptions = [];
        if (chado_select_record($table, [$pkey], [$pkey => $insert_values[$pkey]], $coptions)) {
          tripal_report_error('tripal_chado', TRIPAL_ERROR,
            'chado_insert_record; Cannot insert duplicate primary key into !table table: !values',
            ['!table' => $table, '!values' => print_r($values, TRUE)],
            ['print' => $print_errors]
          );
          return FALSE;
        }
      }
    }

    // Make sure required fields have a value.
    if (!is_array($table_desc['fields'])) {
      $table_desc['fields'] = [];
      tripal_report_error('tripal_chado', TRIPAL_WARNING,
        "chado_insert_record; %table missing fields: \n %schema",
        ['%table' => $table, '%schema' => print_r($table_desc, 1)],
        ['print' => $print_errors]
      );
    }
    foreach ($table_desc['fields'] as $field => $def) {
      // A field is considered missing if it cannot be NULL and there is no
      // default value for it or it is of type 'serial'.
      if (array_key_exists('NOT NULL', $def) and
        !array_key_exists($field, $insert_values) and
        !array_key_exists('default', $def) and
        strcmp($def['type'], serial) != 0) {
        tripal_report_error('tripal_chado', TRIPAL_ERROR,
          "chado_insert_record; Field %table.%field cannot be NULL: %values",
          [
            '%table' => $table,
            '%field' => $field,
            '%values' => print_r($values, 1),
          ],
          ['print' => $print_errors]
        );
        return FALSE;
      }
    }
  }
  // End of validation.

  // Now build the insert SQL statement.
  $ifields = [];       // Contains the names of the fields.
  $itypes = [];       // Contains placeholders for the sql query.
  $ivalues = [];       // Contains the values of the fields.
  foreach ($insert_values as $field => $value) {
    $ifields[] = $field;
    if (strcmp($value, '__NULL__') == 0) {
      $itypes[] = "NULL";
    }
    else {
      $itypes[] = ":$field";
      $ivalues[":$field"] = $value;
    }
  }

  // Create the SQL.
  $sql = 'INSERT INTO {' . $table . '} (' . implode(", ", $ifields) . ") VALUES (" . implode(", ", $itypes) . ")";
  $result = chado_query($sql, $ivalues);

  // If we have a result then add primary keys to return array.
  if ($options['return_record'] == TRUE and $result) {
    if (array_key_exists('primary key', $table_desc) and is_array($table_desc['primary key'])) {
      foreach ($table_desc['primary key'] as $field) {
        $sql = "SELECT CURRVAL('{" . $table . "}_" . $field . "_seq')";
        $results = chado_query($sql);
        $value = $results->fetchField();
        if (!$value) {
          tripal_report_error('tripal_chado', TRIPAL_ERROR,
            "chado_insert_record; not able to retrieve primary key after insert: %sql",
            ['%sql' => $sql],
            ['print' => $print_errors]
          );
          return FALSE;
        }
        $values[$field] = $value;
      }
    }
    return $values;
  }
  elseif ($options['return_record'] == FALSE and $result) {
    return TRUE;
  }
  else {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'chado_insert_record; Cannot insert record into "%table": %values',
      ['%table' => $table, '%values' => print_r($values, 1)],
      ['print' => $print_errors]
    );
    return FALSE;
  }

  return FALSE;

}

/**
 * Provides a generic routine for updating into any Chado table.
 *
 * Use this function to update a record in any Chado table.  The first
 * argument specifies the table for inserting, the second is an array
 * of values to matched for locating the record for updating, and the third
 * argument give the values to update.  The arrays are mutli-dimensional such
 * that foreign key lookup values can be specified.
 *
 * @param $table
 *  The name of the chado table for inserting.
 * @param $match
 *  An associative array containing the values for locating a record to update.
 * @param $values
 *  An associative array containing the values for updating.
 * @param $options
 *  An array of options such as:
 *  - return_record: by default, the function will return the TRUE if the
 *   record
 *     was succesfully updated.  However, set this option to TRUE to return the
 *     record that was updated.  The returned record will have the fields
 *     provided but the primary key (if available for the table) will be added
 *     to the record.
 *
 * @return
 *  On success this function returns TRUE. On failure, it returns FALSE.
 *
 * Example usage:
 * @code
 * $umatch = array(
 *  'organism_id' => array(
 *    'genus' => 'Citrus',
 *    'species' => 'sinensis',
 *  ),
 *  'uniquename' => 'orange1.1g000034m.g7',
 *  'type_id' => array (
 *    'cv_id' => array (
 *      'name' => 'sequence',
 *    ),
 *    'name' => 'gene',
 *    'is_obsolete' => 0
 *  ),
 *);
 * $uvalues = array(
 *  'name' => 'orange1.1g000034m.g',
 *  'type_id' => array (
 *    'cv_id' => array (
 *      'name' => 'sequence',
 *
 *     ),
 *   'name' => 'mRNA',
 *     'is_obsolete' => 0
 *   ),
 * );
 *   $result = chado_update_record('feature',$umatch,$uvalues);
 * @endcode
 * The above code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be updated.
 * The organism_id is specified as a nested array that uses the organism_id
 * foreign key constraint to lookup the specified values to find the exact
 * organism_id. The same nested struture is also used for specifying the
 * values to update.  The function will find the record that matches the
 * columns specified and update the record with the avlues in the $uvalues
 *   array.
 *
 * @TODO: Support Complex filtering as is done in chado_select_record();
 *
 * @ingroup tripal_chado_query_api
 */
function chado_update_record($table, $match, $values, $options = NULL) {

  $print_errors = (isset($options['print_errors'])) ? $options['print_errors'] : FALSE;

  if (!is_array($values)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass non array as values for updating.',
      [], ['print' => $print_errors]
    );
    return FALSE;
  }
  if (count($values) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass an empty array as values for updating.',
      [], ['print' => $print_errors]
    );
    return FALSE;
  }

  if (!is_array($match)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass non array as values for matching.',
      [], ['print' => $print_errors]
    );
    return FALSE;
  }
  if (count($match) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass an empty array as values for matching.',
      [], ['print' => $print_errors]
    );
    return FALSE;
  }

  // Set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements.
  if (!is_array($options)) {
    $options = [];
  }

  if (!array_key_exists('return_record', $options)) {
    $options['return_record'] = FALSE;
  }

  $update_values = [];   // Contains the values to be updated.
  $update_matches = [];  // Contains the values for the where clause.

  // Get the table description.
  $table_desc = chado_get_schema($table);
  if (!$table_desc) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'The table name, %table, does not exist.',
      ['%table', $table], ['print' => $print_errors]
    );
    return FALSE;
  }

  // If the user wants us to return the record then we need to get the
  // unique primary key if one exists.  That way we can add it to the
  // values that get returned at the end of the function.
  $pkeys = [];
  if ($options['return_record'] == TRUE) {
    if (array_key_exists('primary key', $table_desc) and is_array($table_desc['primary key'])) {
      $columns = [];
      $stmt_suffix = '';
      foreach ($table_desc['primary key'] as $field) {
        $columns[] = $field;
        $stmt_suffix .= substr($field, 0, 2);
      }
      $options2 = [];
      $results = chado_select_record($table, $columns, $match, $options2);
      if (count($results) > 0) {
        foreach ($results as $index => $pkey) {
          $pkeys[] = $pkey;
        }
      }
    }
  }

  // Get the values needed for matching in the SQL statement.
  foreach ($match as $field => $value) {
    if (is_array($value)) {
      $results = chado_schema_get_foreign_key($table_desc, $field, $value);
      if (sizeof($results) > 1) {
        tripal_report_error('tripal_chado', TRIPAL_ERROR,
          'chado_update_record: When trying to find record to update, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
          ['!foreign_key' => $field, '!criteria' => print_r($value, TRUE)],
          ['print' => $print_errors]
        );
        return FALSE;
      }
      elseif (sizeof($results) < 1) {
        tripal_report_error('tripal_chado', TRIPAL_DEBUG,
          'chado_update_record: When trying to find record to update, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)',
          ['!foreign_key' => $field, '!criteria' => print_r($value, TRUE)],
          ['print' => $print_errors]
        );
        return FALSE;
      }
      else {
        $update_matches[$field] = $results[0];
      }
    }
    else {
      $update_matches[$field] = $value;
    }
  }

  // Get the values used for updating.
  foreach ($values as $field => &$value) {
    if (is_array($value)) {
      $foreign_options = [];
      // Select the value from the foreign key relationship for this value.
      $results = chado_schema_get_foreign_key($table_desc, $field, $value, $foreign_options);
      if (sizeof($results) > 1) {
        tripal_report_error('tripal_chado', TRIPAL_ERROR,
          'chado_update_record: When trying to find update values, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
          ['!foreign_key' => $field, '!criteria' => print_r($value, TRUE)],
          ['print' => $print_errors]
        );
        return FALSE;
      }
      elseif (sizeof($results) < 1) {
        tripal_report_error('tripal_chado', TRIPAL_DEBUG,
          'chado_update_record: When trying to find update values, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)',
          ['!foreign_key' => $field, '!criteria' => print_r($value, TRUE)],
          ['print' => $print_errors]
        );
        return FALSE;
      }
      else {
        $update_values[$field] = $results[0];
      }
    }
    else {
      $update_values[$field] = $value;
    }
  }

  // Now build the SQL statement.
  $sql = 'UPDATE {' . $table . '} SET ';
  $args = [];        // Arguments passed to chado_query.
  foreach ($update_values as $field => $value) {
    if (strcmp($value, '__NULL__') == 0) {
      $sql .= " $field = NULL, ";
    }
    else {
      $sql .= " $field = :$field, ";
      $args[":$field"] = $value;
    }
  }
  $sql = drupal_substr($sql, 0, -2);  // Get rid of the trailing comma & space.

  $sql .= " WHERE ";
  foreach ($update_matches as $field => $value) {
    if (strcmp($value, '__NULL__') == 0) {
      $sql .= " $field = NULL AND ";
    }
    else {
      $sql .= " $field = :old_$field AND ";
      $args[":old_$field"] = $value;
    }
  }
  $sql = drupal_substr($sql, 0, -4);  // Get rid of the trailing 'AND'.

  $result = chado_query($sql, $args);

  // If we have a result then add primary keys to return array.
  if ($options['return_record'] == TRUE and $result) {
    // Only if we have a single result do we want to add the primary keys to the
    // values array.  If the update matched many records we can't add the pkeys.

    if (count($pkeys) == 1) {
      foreach ($pkeys as $index => $pkey) {
        foreach ($pkey as $field => $fvalue) {
          $values[$field] = $fvalue;
        }
      }
    }
    return $values;
  }
  elseif ($options['return_record'] == FALSE and $result) {
    return TRUE;
  }
  else {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      "chado_update_record: Cannot update record in %table table.  \nMatch: %match \nValues: %values",
      [
        '%table' => table,
        '%match' => print_r($match, TRUE),
        '%values' => print_r($values, 1),
      ],
      ['print' => $print_errors]
    );
    return FALSE;
  }

  return FALSE;
}

/**
 * Provides a generic function for deleting a record(s) from any chado table.
 *
 * Use this function to delete a record(s) in any Chado table.  The first
 * argument specifies the table to delete from and the second is an array
 * of values to match for locating the record(s) to be deleted.  The arrays
 * are mutli-dimensional such that foreign key lookup values can be specified.
 *
 * @param $table
 *  The name of the chado table for inserting.
 * @param $match
 *  An associative array containing the values for locating a record to update.
 * @param $options
 *  Currently there are no options.
 *
 * @return
 *   On success this function returns TRUE. On failure, it returns FALSE.
 *
 * Example usage:
 * @code
 *$umatch = array(
 *  'organism_id' => array(
 *    'genus' => 'Citrus',
 *    'species' => 'sinensis',
 *  ),
 *  'uniquename' => 'orange1.1g000034m.g7',
 *  'type_id' => array (
 *    'cv_id' => array (
 *      'name' => 'sequence',
 *    ),
 *    'name' => 'gene',
 *    'is_obsolete' => 0
 *  ),
 *);
 *$uvalues = array(
 *  'name' => 'orange1.1g000034m.g',
 *  'type_id' => array (
 *    'cv_id' => array (
 *      'name' => 'sequence',
 *    ),
 *    'name' => 'mRNA',
 *    'is_obsolete' => 0
 *  ),
 *);
 *   $result = chado_update_record('feature', $umatch, $uvalues);
 * @endcode
 * The above code species that a feature with a given uniquename, organism_id,
 * and type_id (the unique constraint for the feature table) will be deleted.
 * The organism_id is specified as a nested array that uses the organism_id
 * foreign key constraint to lookup the specified values to find the exact
 * organism_id. The same nested struture is also used for specifying the
 * values to update.  The function will find all records that match the
 * columns specified and delete them.
 *
 * @TODO: Support Complex filtering as is done in chado_select_record();
 *
 * @ingroup tripal_chado_query_api
 */
function chado_delete_record($table, $match, $options = NULL) {

  $print_errors = (isset($options['print_errors'])) ? $options['print_errors'] : FALSE;

  if (!is_array($match)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass non array as values for matching.', []);
    return FALSE;
  }
  if (count($match) == 0) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'Cannot pass an empty array as values for matching.', []);
    return FALSE;
  }

  // Set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements.
  if (!is_array($options)) {
    $options = [];
  }

  $delete_matches = [];  // Contains the values for the where clause.

  // Get the table description.
  $table_desc = chado_get_schema($table);
  $fields = $table_desc['fields'];
  if (empty($table_desc)) {
    tripal_report_error('tripal_chado', TRIPAL_WARNING,
      'chado_delete_record; There is no table description for !table_name',
      ['!table_name' => $table], ['print' => $print_errors]
    );
  }

  // Get the values needed for matching in the SQL statement.
  foreach ($match as $field => $value) {
    if (is_array($value)) {
      // If the user has specified an array of values to delete rather than
      // FK relationships the keep those in our match.
      if (array_values($value) === $value) {
        $delete_matches[$field] = $value;
      }
      else {
        $results = chado_schema_get_foreign_key($table_desc, $field, $value);
        if (sizeof($results) > 1) {
          tripal_report_error('tripal_chado', TRIPAL_ERROR,
            'chado_delete_record: When trying to find record to delete, too many records match the criteria supplied for !foreign_key foreign key constraint (!criteria)',
            ['!foreign_key' => $field, '!criteria' => print_r($value, TRUE)]);
          return FALSE;
        }
        elseif (sizeof($results) < 1) {
          //tripal_report_error('tripal_chado', TRIPAL_ERROR, 'chado_delete_record: When trying to find record to delete, no record matches criteria supplied for !foreign_key foreign key constraint (!criteria)', array('!foreign_key' => $field, '!criteria' => print_r($value,TRUE)));
        }
        else {
          $delete_matches[$field] = $results[0];
        }
      }
    }
    else {
      $delete_matches[$field] = $value;
    }
  }

  // Now build the SQL statement.
  $sql = 'DELETE FROM {' . $table . '} WHERE ';
  $args = [];
  foreach ($delete_matches as $field => $value) {
    // If we have an array values then this is an "IN" clasue.

    if (is_array($value) and count($value) > 1) {
      $sql .= "$field IN (";
      $index = 0;
      foreach ($value as $v) {
        $sql .= ":$field" . $index . ", ";
        $args[":$field" . $index] = $v;
        $index++;
      }
      $sql = drupal_substr($sql, 0, -2); // Get rid of trailing ', '.
      $sql .= ") AND ";
    }
    else {
      if (strcmp($value, '__NULL__') == 0) {
        $sql .= " $field = NULL AND ";
      }
      else {
        $sql .= " $field = :$field AND ";
        $args[":$field"] = $value;
      }
    }
  }
  $sql = drupal_substr($sql, 0, -4);  // Get rid of the trailing 'AND'.

  // Finally perform the delete.  If successful, return the updated record.
  $result = chado_query($sql, $args);
  if ($result) {
    return TRUE;
  }
  else {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      "Cannot delete record in $table table.  Match:" . print_r($match, 1) . ". Values: " . print_r($values, 1), []);
    return FALSE;
  }
  return FALSE;
}

/**
 * Provides a generic routine for selecting data from a Chado table.
 *
 * Use this function to perform a simple select from any Chado table.
 *
 * @param $table
 *  The name of the chado table for inserting
 * @param $columns
 *  An array of column names
 * @param $values
 *  An associative array containing the values for filtering the results. In
 *   the
 *  case where multiple values for the same time are to be selected an
 *  additional entry for the field should appear for each value. If you need to
 *  filter results using more complex methods see the 'Complex Filtering'
 * section below.
 * @param $options
 *  An associative array of additional options where the key is the option
 *  and the value is the value of that option.
 *
 * Additional Options Include:
 *  - has_record
 *     Set this argument to 'TRUE' to have this function return a numeric
 *     value for the number of records rather than the array of records.  this
 *     can be useful in 'if' statements to check the presence of particula
 *     records.
 *  - return_sql
 *     Set this to 'TRUE' to have this function return an array where the first
 *     element is the sql that would have been run and the second is an array
 *   of
 *     arguments.
 *  - case_insensitive_columns
 *     An array of columns to do a case insensitive search on.
 *  - regex_columns
 *     An array of columns where the value passed in should be treated as a
 *     regular expression
 *  - order_by
 *     An associative array containing the column names of the table as keys
 *     and the type of sort (i.e. ASC, DESC) as the values.  The results in the
 *     query will be sorted by the key values in the direction listed by the
 *     value
 *  - is_duplicate: TRUE or FALSE.  Checks the values submited to see if
 *     they violate any of the unique constraints. If not, the record
 *     is returned, if so, FALSE is returned.
 *  - pager:  Use this option if it is desired to return only a subset of
 *     results so that they may be shown with in a Drupal-style pager. This
 *     should be an array with two keys: 'limit' and 'element'.  The value of
 *     'limit'  should specify the number of records to return and 'element' is
 *     a unique integer to differentiate between pagers when more than one
 *     appear on a page.  The 'element' should start with zero and increment by
 *     one for each pager.
 *  -limit:  Specifies the number of records to return.
 *  -offset:  Indicates the number of records to skip before returning records.
 *
 * @return
 *  An array of results, FALSE if the query was not executed
 *  correctly, an empty array if no records were matched, or the number of
 *  records in the dataset if $has_record is set.
 *  If the option 'is_duplicate' is provided and the record is a duplicate it
 *  will return the duplicated record.  If the 'has_record' option is provided
 *  a value of TRUE will be returned if a record exists and FALSE will bee
 *  returned if there are not records.
 *
 * Example usage:
 * @code
 *   $columns = array('feature_id', 'name');
 *   $values =  array(
 *     'organism_id' => array(
 *         'genus' => 'Citrus',
 *         'species' => array('sinensis', 'clementina'),
 *      ),
 *     'uniquename' => 'orange1.1g000034m.g',
 *     'type_id' => array (
 *         'cv_id' => array (
 *            'name' => 'sequence',
 *         ),
 *         'name' => 'gene',
 *         'is_obsolete' => 0
 *      ),
 *   );
 *   $options = array(
 *     'order_by' => array(
 *        'name' => 'ASC'
 *     ),
 *   );
 *   $result = chado_select_record('feature',$columns,$values,$options);
 * @endcode
 * The above code selects a record from the feature table using the three
 *   fields
 * that uniquely identify a feature.  The $columns array simply lists the
 *   columns to select. The $values array is nested such that the organism is
 *   identified by way of the organism_id foreign key constraint by specifying
 *   the genus and species.  The cvterm is also specified using its foreign key
 *   and the cv_id for the cvterm is nested as well.  In the example above, two
 *   different species are allowed to match
 *
 * Complex Filtering:
 *   All of the documentation above supports filtering based on 'is equal to'
 *   or 'is NULL'. If your criteria doesn't fall into one of these two
 *   categories then you need to provide an array with additional details such
 *   as the operator as well as the value. An example follows and will be
 *   discussed in detail.
 * @code
 *      $columns = array('feature_id', 'fmin', 'fmax');
 *     // Regular criteria specifying the parent feature to retrieve locations
 *   from.
 *     $values = array(
 *       'srcfeature_id' => array(
 *         'uniquename' => 'MtChr01'
 *         'type_id' => array(
 *           'name' => 'pseudomolecule'
 *         ),
 *       ),
 *     );
 *     // Complex filtering to specify the range to return locations from.
 *     $values['fmin'][] = array(
 *       'op' => '>',
 *       'data' => 15
 *     );
 *     $values['fmin'][] = array(
 *       'op' => '<',
 *       'data' => 100
 *     );
 *     $results = chado_select_record('featureloc', $columns, $values);
 * @endcode
 *   The above code example will return all of the name, start and end of all
 *   the features that start within MtChr1:15-100bp. Note that complex
 *   filtering
 *   can be used in conjunction with basic filtering and that multiple
 *   criteria,
 *   even for the same field can be entered.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_select_record($table, $columns, $values, $options = NULL) {
  // Set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements.
  if (!is_array($options)) {
    $options = [];
  }
  if (!array_key_exists('case_insensitive_columns', $options)) {
    $options['case_insensitive_columns'] = [];
  }
  if (!array_key_exists('regex_columns', $options)) {
    $options['regex_columns'] = [];
  }
  if (!array_key_exists('order_by', $options)) {
    $options['order_by'] = [];
  }
  if (!array_key_exists('return_sql', $options)) {
    $options['return_sql'] = FALSE;
  }
  if (!array_key_exists('has_record', $options)) {
    $options['has_record'] = FALSE;
  }
  if (!array_key_exists('is_duplicate', $options)) {
    $options['is_duplicate'] = FALSE;
  }
  $pager = [];
  if (array_key_exists('pager', $options)) {
    $pager = $options['pager'];
  }
  $print_errors = FALSE;
  if (isset($options['print_errors'])) {
    $print_errors = $options['print_errors'];
  }

  // Check that our columns and values arguments are proper arrays.
  if (!is_array($columns)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'chado_select_record; the $columns argument must be an array. Columns:%columns',
      ['%columns' => print_r($columns, TRUE)],
      ['print' => $print_errors]
    );
    return FALSE;
  }
  if (!is_array($values)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'chado_select_record; the $values argument must be an array. Values:%values',
      ['%values' => print_r($values, TRUE)],
      ['print' => $print_errors]
    );
    return FALSE;
  }

  // Get the table description.
  $table_desc = chado_get_schema($table);
  if (!is_array($table_desc)) {
    tripal_report_error('tripal_chado', TRIPAL_WARNING,
      'chado_insert_record; There is no table description for !table_name',
      ['!table_name' => $table], ['print' => $print_errors]
    );
    return FALSE;
  }

  $where = [];
  $args = [];

  if ($options['is_duplicate'] and array_key_exists('unique keys', $table_desc)) {
    $ukeys = $table_desc['unique keys'];
    $has_results = 0;

    // Iterate through the unique constraints and reset the values and columns
    // arrays to only include these fields.
    foreach ($ukeys as $cname => $fields) {
      if ($has_results) {
        continue;
      }
      $new_values = [];
      $new_columns = [];
      $new_options = [];
      $has_pkey = 0;

      // Include the primary key in the results returned.
      if (array_key_exists('primary key', $table_desc)) {
        $has_pkey = 1;
        $pkeys = $table_desc['primary key'];
        foreach ($pkeys as $index => $key) {
          array_push($new_columns, $key);
        }
      }

      // Recreate the $values and $columns arrays.
      foreach ($fields as $field) {
        if (array_key_exists($field, $values)) {
          $new_values[$field] = $values[$field];
          // If there is no primary key then use the unique constraint fields.
          if (!$has_pkey) {
            array_push($new_columns, $field);
          }
        }
        // If the field doesn't exist in the values array then
        // substitute any default values.
        elseif (array_key_exists('default', $table_desc['fields'][$field])) {
          $new_values[$field] = $table_desc['fields'][$field]['default'];
          if (!$has_pkey) {
            array_push($new_columns, $field);
          }
        }
        // If there is no value (default or otherwise) check if this field is
        // allowed to be null.
        elseif (!$table_desc['fields'][$field]['not null']) {
          $new_values[$field] = NULL;
          if (!$has_pkey) {
            array_push($new_columns, $field);
          }
        }
        // If the array key doesn't exist in the values given by the caller
        // and there is no default value then we cannot check if the record
        // is a duplicate so return FALSE.
        else {
          tripal_report_error('tripal_chado', TRIPAL_ERROR,
            'chado_select_record: There is no value for %field thus we cannot ' .
            'check if this record for table, %table, is unique. %values',
            [
              '%field' => $field,
              '%table' => $table,
              '%values' => print_r($values, TRUE),
            ],
            ['print' => $print_errors]);
          return FALSE;
        }
      }
      $results = chado_select_record($table, $new_columns, $new_values, $new_options);
      // If we have a duplicate record then return the results.
      if (count($results) > 0) {
        $has_results = 1;
      }
      unset($new_columns);
      unset($new_values);
      unset($new_options);
    }
    if ($options['has_record'] and $has_results) {
      return TRUE;
    }
    else {
      return $results;
    }
  }

  // Process the values array into where clauses and retrieve foreign keys. The
  // $where array should always be an integer-indexed array with each value
  // being an array with a 'field', 'op', and 'data' keys with all foreign keys
  // followed.
  foreach ($values as $field => $value) {

    // Require the field be in the table description.
    if (!array_key_exists($field, $table_desc['fields'])) {
      tripal_report_error('tripal_chado', TRIPAL_ERROR,
        'chado_select_record: The field "%field" does not exist in the table "%table".  Cannot perform query. Values: %array. Fields: %fields',
        [
          '%field' => $field,
          '%table' => $table,
          '%array' => print_r($values, 1),
          '%fields' => print_r($table_desc['fields'], 1),
        ],
        ['print' => $print_errors]
      );
      return [];
    }

    // CASE 1: We have an array for a value.
    if (is_array($value)) {

      // CASE 1a: If there is only one element in the array, treat it the same
      // as a non-array value.
      if (count($value) == 1 AND is_int(key($value))
        AND !(isset($value[0]['op']) && isset($value[0]['data']))) {

        $value = array_pop($value);
        $op = '=';
        chado_select_record_check_value_type($op, $value, $table_desc['fields'][$field]['type']);

        $where[] = [
          'field' => $field,
          'op' => $op,
          'data' => $value,
        ];
      }
      // CASE 1b: If there is a 'data' key in the array then we have the new
      // complex filtering format with a single criteria.
      elseif (isset($value['data']) AND isset($value['op'])) {

        $value['field'] = $field;
        $where[] = $value;
      }
      // CASE 1c: If we have an integer indexed array and the first element is
      // not an array then we have a simple array of values to be used for an
      // IN clause.
      elseif (is_int(key($value)) AND !is_array(current($value))) {

        $where[] = [
          'field' => $field,
          'op' => 'IN',
          'data' => $value,
        ];
      }
      // We have a multi-dimensional array: 2 cases...
      else {

        // CASE 1d: If there is a multi-dimensional array with each sub-array
        // containing a data key then we have the new complex filtering format
        // with multiple criteria.
        if (isset($value[0]['data']) AND isset($value[0]['op'])) {

          foreach ($value as $subvalue) {
            $subvalue['field'] = $field;
            $where[] = $subvalue;
          }
        }
        // CASE 1e: We have a multi-dimensional array that doesn't fit any of
        // the above cases then we have a foreign key definition to follow.
        else {

          // Select the value from the foreign key relationship for this value.
          $foreign_options = [
            'regex_columns' => $options['regex_columns'],
          ];
          $results = chado_schema_get_foreign_key($table_desc, $field, $value, $foreign_options);

          // Ensure that looking up the foreign key didn't fail in an error.
          if ($results === FALSE OR $results === NULL) {
            tripal_report_error('tripal_chado', TRIPAL_ERROR,
              'chado_select_record: could not follow the foreign key definition
              for %field where the definition supplied was %value',
              ['%field' => $field, '%value' => print_r($value, TRUE)]
            );
            return [];
          }
          // Ensure that there were results returned.
          elseif (count($results) == 0) {
            tripal_report_error('tripal_chado', TRIPAL_ERROR,
              'chado_select_record: the foreign key definition for \'%field\' on table \'%table\' ' .
              'returned no results where the definition supplied was %value',
              [
                '%field' => $field,
                '%table' => $table,
                '%value' => print_r($value, TRUE),
              ]
            );
            return [];
          }
          // If there was only a single resutlt then add it using an op of =.
          elseif (count($results) == 1) {
            $results = array_pop($results);
            $op = '=';
            chado_select_record_check_value_type($op, $results, $table_desc['fields'][$field]['type']);

            $where[] = [
              'field' => $field,
              'op' => $op,
              'data' => $results,
            ];
          }
          // Otherwise multiple results were returned so we want to form an
          // IN (x, y, z) expression.
          else {
            $where[] = [
              'field' => $field,
              'op' => 'IN',
              'data' => $results,
            ];
          }
        }
      }
    }
    // CASE 2: We have a single value.
    else {

      $op = '=';
      chado_select_record_check_value_type($op, $value, $table_desc['fields'][$field]['type']);

      $where[] = [
        'field' => $field,
        'op' => $op,
        'data' => $value,
      ];
    }

    // Support Deprecated method for regex conditions.
    $current_key = key($where);
    if (in_array($field, $options['regex_columns'])) {
      $where[$current_key]['op'] = '~*';
    }

  }

  // Now build the SQL.
  if (empty($where)) {
    // Sometimes want to select everything.
    $sql = "SELECT " . implode(', ', $columns) . " ";
    $sql .= 'FROM {' . $table . '} ';
  }
  else {
    $sql = "SELECT " . implode(', ', $columns) . " ";
    $sql .= 'FROM {' . $table . '} ';

    // If $values is empty then we want all results so no where clause.
    if (!empty($values)) {
      $sql .= "WHERE ";
    }
    foreach ($where as $clause_num => $value_def) {

      switch ($value_def['op']) {
        // Deal with 'field IN (x, y, z)' where clauses.
        case 'IN':
          $sql .= $value_def['field'] . " IN (";
          $index = 0;
          foreach ($value_def['data'] as $v) {
            $placeholder = ':' . $value_def['field'] . $clause_num . '_' . $index;
            $sql .= $placeholder . ', ';
            $args[$placeholder] = $v;
            $index++;
          }
          $sql = drupal_substr($sql, 0, -2); // remove trailing ', '
          $sql .= ") AND ";
          break;

        // Deal with IS NULL.
        case 'IS NULL':
          $sql .= $value_def['field'] . ' IS NULL AND ';
          break;

        // Default is [field] [op] [data].
        default:
          $placeholder = ':' . $value_def['field'] . $clause_num;

          // Support case insensitive columns.
          if (in_array($value_def['field'], $options['case_insensitive_columns'])) {
            $sql .= 'lower(' . $value_def['field'] . ') ' . $value_def['op'] . ' lower(' . $placeholder . ') AND ';
          }
          else {
            $sql .= $value_def['field'] . ' ' . $value_def['op'] . ' ' . $placeholder . ' AND ';
          }
          $args[$placeholder] = $value_def['data'];
      }
    } // End foreach item in where clause.
    $sql = drupal_substr($sql, 0, -4);  // Get rid of the trailing 'AND '
  } // End if (empty($where)){ } else {

  // Add any ordering of the results to the SQL statement.
  if (count($options['order_by']) > 0) {
    $sql .= " ORDER BY ";
    foreach ($options['order_by'] as $field => $dir) {
      $sql .= "$field $dir, ";
    }
    $sql = drupal_substr($sql, 0, -2);  // Get rid of the trailing ', '
  }

  // Limit the records returned.
  if (array_key_exists('limit', $options) and is_numeric($options['limit'])) {
    $sql .= " LIMIT " . $options['limit'];
    if (array_key_exists('offset', $options) and is_numeric($options['offset'])) {
      $sql .= " OFFSET " . $options['offset'];
    }
  }

  // If the caller has requested the SQL rather than the results then do so.
  if ($options['return_sql'] == TRUE) {
    return ['sql' => $sql, 'args' => $args];
  }
  if (array_key_exists('limit', $pager)) {
    $total_records = 0;
    $resource = chado_pager_query($sql, $args, $pager['limit'], $pager['element'], NULL, $total_records);
  }
  else {
    $resource = chado_query($sql, $args);
  }

  // Format results into an array.
  $results = [];
  foreach ($resource as $r) {
    $results[] = $r;
  }
  if ($options['has_record']) {
    return count($results);
  }

  return $results;
}

/**
 * Helper Function: check that the value is the correct type.
 *
 * This function is used by chado_select_record() when building the $where
 * clause array to ensure that any single values are the correct type based
 * on the table definition. Furthermore, it ensures that NULL's are caught
 * changing the operator to 'IS NULL'.
 *
 * @code
 *     $op = '=';
 *     chado_select_record_check_value_type($op, $value,
 *                                      $table_desc['fields'][$field]['type']);
 *
 *     $where[] = array(
 *       'field' => $field,
 *       'op' => $op,
 *       'data' => $value
 *     );
 * @endcode
 *
 * @param $op
 *   The operator being used. This is mostly passed in to allow it to be changed
 *   if a NULL value is detected.
 * @param $value
 *   The value to be checked and adjusted.
 * @param $type
 *   The type from the table definition that's used to determine the type of
 *   value.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_select_record_check_value_type(&$op, &$value, $type) {

  if ($value === NULL) {
    $op = 'IS NULL';
  }
  elseif ($type == 'int') {
    $value = (int) $value;
  }

}

/**
 * A substitute for db_query() when querying from Chado.
 *
 * This function is needed to avoid switching databases when making query to
 * the chado database.
 *
 * Will use a chado persistent connection if it already exists.
 *
 * @param $sql
 *   The sql statement to execute. When referencing tables in chado, table
 *   names
 *   should be surrounded by curly brackets (e.g. { and }). If Drupal tables
 *   need to be included in the query, surround those by sqaure brackets
 *   (e.g. [ and ]).  This follows Drupal conventions for resolving table
 *   names.
 *   It also supports a multi-chado installation.
 *
 * @param $args
 *   The array of arguments, with the same structure as passed to
 *   the db_query() function of Drupal.
 *
 * @return
 *   DatabaseStatementInterface A prepared statement object, already executed.
 *
 * Example usage:
 * @code
 * $sql = "SELECT F.name, CVT.name as type_name, ORG.common_name
 *          FROM {feature} F
 *          LEFT JOIN {cvterm} CVT ON F.type_id = CVT.cvterm_id
 *          LEFT JOIN {organism} ORG ON F.organism_id = ORG.organism_id
 *          WHERE
 *            F.uniquename = :feature_uniquename";
 * $args = array( ':feature_uniquename' => $form_state['values']['uniquename']
 *   );
 * $result = chado_query($sql, $args);
 * while ($r = $results->fetchObject()) {
 *   // Do something with the record object $r
 * }
 * @endcode
 *
 * @ingroup tripal_chado_query_api
 */
function chado_query($sql, $args = []) {
  $results = NULL;

  $is_local = isset($GLOBALS["chado_is_local"]) && $GLOBALS["chado_is_local"];

  // Args should be an array
  if (!is_array($args)) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      'chado_query; Need to pass an array to chado_query, "%value" passed instead. Query: %query',
      ['%value' => $args, '%query' => $sql]
    );
    return FALSE;
  }

  // if Chado is local to the database then prefix the Chado table
  // names with 'chado'.
  if ($is_local) {
    // Remove carriage returns from the SQL.
    $sql = preg_replace('/\n/', ' ', $sql);

    // Get the current default Chado and Drupal schema prefixes.
    $chado_schema_name = chado_get_schema_name('chado');
    $drupal_schema_name = chado_get_schema_name('drupal');

    // Prefix the tables with their correct schema.
    // Chado tables should be enclosed in curly brackets (ie: {feature} )
    // and Drupal tables should be enclosed in square brackets
    // (ie: [tripal_jobs] ).
    $matches = [];
    if (preg_match_all('/\{(.*?)\}/', $sql, $matches)) {
      $matches = $matches[1];
      $chado_tables = array_unique(array_keys(chado_get_table_names(TRUE)));
      foreach ($matches as $match) {
        if (in_array(strtolower($match), $chado_tables)) {
          $sql = preg_replace("/\{$match\}/", $chado_schema_name . '.' . $match, $sql);
        }
      }
    }

    // Now set the Drupal prefix if the table is surrounded by square brackets.
    if (preg_match_all('/\[(.*?)\]/', $sql, $matches)) {
      $matches = $matches[1];
      $drupal_tables = array_unique(array_keys(drupal_get_schema()));
      foreach ($matches as $match) {
        if (in_array(strtolower($match), $drupal_tables)) {
          $sql = preg_replace("/\[$match\]/", $drupal_schema_name . '.' . $match, $sql);
        }
      }
    }

    // Add an alter hook to allow module developers to change the query right
    // before it's  executed. Since all queriying of chado by Tripal eventually
    // goes through this function, we only need to provide an alter hook at this
    // point in order to ensure developers have complete control over the query
    // being executed. For example, a module developer might want to remove
    // schema prefixing from queries and rely on the search path. This alter
    // hook would allow them to do that by implementing
    // mymodule_chado_query_alter($sql, $args) and using a regular expression
    // to remove table prefixing from the query.
    // @see hook_chado_query_alter().
    drupal_alter('chado_query', $sql, $args);

    // The featureloc table has some indexes that use function that call other
    // functions and those calls do not reference a schema, therefore, any
    // tables with featureloc must automaticaly have the chado schema set as
    // active to find.
    if (preg_match('/' . $chado_schema_name . '.featureloc/i', $sql) or preg_match('/' . $chado_schema_name . '.feature/i', $sql)) {
      $previous_db = chado_set_active('chado');
      try {
        $results = db_query($sql, $args);
        chado_set_active($previous_db);
      } catch (Exception $e) {
        chado_set_active($previous_db);
        throw $e;
      }
    }
    // For all other tables we should have everything in scope so just run the
    // query.
    else {
      $results = db_query($sql, $args);
    }
  }
  // Check for any cross schema joins (ie: both drupal and chado tables
  // represented and if present don't execute the query but instead warn the
  // administrator.
  else {
    if (preg_match('/\[(\w*?)\]/', $sql)) {
      tripal_report_error('chado_query', TRIPAL_ERROR,
        'The following query does not support external chado databases. Please file an issue with the Drupal.org Tripal Project. Query: @query',
        ['@query' => $sql]
      );
      return FALSE;
    }
    // If Chado is not local to the Drupal database then we have to
    // switch to another database.
    else {
      $previous_db = chado_set_active('chado');
      $results = db_query($sql, $args);
      chado_set_active($previous_db);
    }
  }

  return $results;
}

/**
 * This hook provides a way for module developers to alter any/all queries on
 * the chado schema by Tripal.
 *
 * Example: a module developer might want to remove schema prefixing from
 * queries and rely on the search path. This alter hook would allow them to do
 * that by implementing mymodule_chado_query_alter($sql, $args) and using a
 * regular expression to remove table prefixing from the query.
 *
 * @param $sql
 *    A string describing the SQL query to be executed by Tripal. All parameters
 *    should be indicated by :tokens with values being in the $args array and
 *    all tables should be prefixed with the schema name described in
 *    chado_get_schema_name().
 * @param $args
 *    An array of arguments where the key is the token used in $sql
 *    (for example, :value) and the value is the value you would like
 *    substituted in.
 *
 * @ingroup tripal_chado_query_api
 */
function hook_chado_query_alter(&$sql, &$args) {

  // The following code is an example of how this alter function might be used.
  // Say you would like only a portion of node => feature connections available
  // for a period of time or under a specific condition. To "hide" the other
  // connections you might create a temporary view of the chado_feature table
  // that only includes the connections you would like to be available. In order
  // to ensure this view is used rather than the original chado_feature table
  // you could alter all Tripal queries referring to chado_feature to instead
  //refer to your view.
  if (preg_match('/(\w+)\.chado_feature/', $sql, $matches)) {

    $sql = str_replace(
      $matches[1] . '.chado_feature',
      'chado_feature_view',
      $sql
    );
  }
}

/**
 * Use this function instead of pager_query() when selecting a
 * subset of records from a Chado table.
 *
 * @param $query
 *   The SQL statement to execute, this is followed by a variable number of args
 *   used as substitution values in the SQL statement.
 * @param $args
 *   The array of arguments for the query. They keys are the placeholders
 * @param $limit
 *   The number of query results to display per page.
 * @param $element
 *   An numeric identifier used to distinguish between multiple pagers on one
 *   page.
 * @param $count_query
 *   An SQL query used to count matching records.
 *
 * @returns
 *   A database query result resource or FALSE if the query was not
 *   executed correctly
 *
 * @ingroup tripal_chado_query_api
 */
function chado_pager_query($query, $args, $limit, $element, $count_query = '') {
  // Get the page and offset for the pager.
  $page_arg = isset($_GET['page']) ? $_GET['page'] : '0';
  $pages = explode(',', $page_arg);
  $page = 0;
  if (count($pages) >= $element) {
    $page = key_exists($element, $pages) ? $pages[$element] : 0;
  }
  $offset = $limit * $page;
  $q = $_GET['q'];

  // Construct a count query if none was given.
  if (!isset($count_query)) {
    $count_query = preg_replace(['/SELECT.*?FROM /As', '/ORDER BY .*/'],
      ['SELECT COUNT(*) FROM ', ''], $query);
  }

  // We calculate the total of pages as ceil(items / limit).
  $results = chado_query($count_query, $args);
  if (!$results) {
    tripal_report_error('tripal_chado', TRIPAL_ERROR,
      "chado_pager_query(): Query failed: %cq", ['%cq' => $count_query]);
    return;
  }
  $total_records = $results->fetchField();

  // Set a session variable for storing the total number of records.
  $GLOBALS['chado_pager'][$q][$element]['total_records'] = $total_records;

  pager_default_initialize($total_records, $limit, $element);

  $query .= ' LIMIT ' . (int) $limit . ' OFFSET ' . (int) $offset;
  $results = chado_query($query, $args);
  return $results;
}

/**
 * A function to retrieve the total number of records for a pager that
 * was generated using the chado_pager_query() function.
 *
 * @param $element
 *   The $element argument that was passed to the chado_pager_query function.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_pager_get_count($element) {
  $q = $_GET['q'];

  if (array_key_exists($q, $GLOBALS['chado_pager']) and
    array_key_exists($element, $GLOBALS['chado_pager'][$q])) {
    return $GLOBALS['chado_pager'][$q][$element]['total_records'];
  }
  else {
    return 0;
  }
}

/**
 * Gets the value of a foreign key relationship.
 *
 * This function is used by chado_select_record, chado_insert_record,
 * and chado_update_record to iterate through the associate array of
 * values that gets passed to each of those routines.  The values array
 * is nested where foreign key constraints are used to specify a value that.
 * See documentation for any of those functions for further information.
 *
 * @param $table_desc
 *  A table description for the table with the foreign key relationship to be
 *  identified generated by hook_chado_<table name>_schema()
 * @param $field
 *  The field in the table that is the foreign key.
 * @param $values
 *  An associative array containing the values
 * @param $options
 *  An associative array of additional options where the key is the option
 *  and the value is the value of that option. These options are passed on to
 *  chado_select_record.
 *
 * Additional Options Include:
 *  - case_insensitive_columns
 *     An array of columns to do a case insensitive search on.
 *  - regex_columns
 *     An array of columns where the value passed in should be treated as a
 *     regular expression
 *
 * @return
 *  A string containg the results of the foreign key lookup, or FALSE if failed.
 *
 * Example usage:
 * @code
 *
 *   $values = array(
 *     'genus' => 'Citrus',
 *     'species' => 'sinensis',
 *   );
 *   $value = chado_schema_get_foreign_key('feature', 'organism_id',$values);
 *
 * @endcode
 * The above code selects a record from the feature table using the three fields
 * that uniquely identify a feature.  The $columns array simply lists the
 * columns to select. The $values array is nested such that the organism is
 * identified by way of the organism_id foreign key constraint by specifying the
 * genus and species.  The cvterm is also specified using its foreign key and
 * the cv_id for the cvterm is nested as well.
 *
 */
function chado_schema_get_foreign_key($table_desc, $field, $values, $options = NULL) {

  // Set defaults for options. If we don't set defaults then
  // we get memory leaks when we try to access the elements.
  if (!is_array($options)) {
    $options = [];
  }
  if (!array_key_exists('case_insensitive_columns', $options)) {
    $options['case_insensitive_columns'] = [];
  }
  if (!array_key_exists('regex_columns', $options)) {
    $options['regex_columns'] = [];
  }

  // Get the list of foreign keys for this table description and
  // iterate through those until we find the one we're looking for.
  $fkeys = '';
  if (array_key_exists('foreign keys', $table_desc)) {
    $fkeys = $table_desc['foreign keys'];
  }
  if ($fkeys) {
    foreach ($fkeys as $name => $def) {
      if (is_array($def['table'])) {
        // Foreign key was described 2X.
        $message = "The foreign key " . $name . " was defined twice. Please check modules "
          . "to determine if hook_chado_schema_<version>_" . $table_desc['table'] . "() was "
          . "implemented and defined this foreign key when it wasn't supposed to. Modules "
          . "this hook was implemented in: " . implode(', ',
            module_implements("chado_" . $table_desc['table'] . "_schema")) . ".";
        tripal_report_error('tripal_chado', $message);
        drupal_set_message(check_plain($message), 'error');
        continue;
      }
      $table = $def['table'];
      $columns = $def['columns'];

      // Iterate through the columns of the foreign key relationship.
      foreach ($columns as $left => $right) {
        // Does the left column in the relationship match our field?
        if (strcmp($field, $left) == 0) {
          // The column name of the foreign key matches the field we want
          // so this is the right relationship.  Now we want to select.
          $select_cols = [$right];
          $result = chado_select_record($table, $select_cols, $values, $options);
          $fields = [];
          if ($result and count($result) > 0) {
            foreach ($result as $obj) {
              $fields[] = $obj->$right;
            }
            return $fields;
          }
        }
      }
    }
  }
  else {
    // @todo: what do we do if we get to this point and we have a fk
    // relationship expected but we don't have any definition for one in the
    // table schema??
    $version = $GLOBALS["chado_version"];
    $message = t("There is no foreign key relationship defined for " . $field . " .
       To define a foreign key relationship, determine the table this foreign
       key referrs to (<foreign table>) and then implement
       hook_chado_chado_schema_v<version>_<foreign table>(). See
       tripal_chado_chado_v1_2_schema_feature for an example. Chado version: $version");
    tripal_report_error('tripal_chado', TRIPAL_ERROR, $message);
    drupal_set_message(check_plain($message), 'error');
  }

  return [];
}

/**
 * Retrieve the name of the PostgreSQL schema housing Chado or Drupal.
 *
 * @param $schema
 *   Wehter you want the schema name for 'chado' or 'drupal'. Chado is the
 *   default.
 *
 * @return
 *   The name of the PostgreSQL schema housing the $schema specified.
 *
 * @ingroup tripal_chado_query_api
 */
function chado_get_schema_name($schema = 'chado') {

  // First we will set our default. This is what will be returned in most cases.
  if ($schema == 'chado') {
    $schema_name = 'chado';
  }
  else {
    $schema_name = 'public';
  }

  // There are cases where modules or admin might need to change the default
  // names for the schema. Thus we provide an alter hook here to allow
  // the names to be changed and ensure that schema names are never hardcoded
  // directly into queries.
  $context = ['schema' => $schema];
  drupal_alter('chado_get_schema_name', $schema_name, $context);

  return $schema_name;
}

/**
 * Alter the name of the schema housing Chado and/or Drupal.
 *
 * This example implementation shows a solution for the case where your chado
 * database was well established in the "public" schema and you added Drupal
 * later in a "drupal" schema. Please note that this has not been tested and
 * while we can ensure that Tripal will work as expected, we have no control
 * over whether Drupal is compatible with not being in the public schema. That's
 * why we recommened the organization we have (ie: Chado in a "chado" schema and
 * Drupal in the "public schema).
 *
 * @param $schema_name
 *   The current name of the schema as known by Tripal. This is likely the
 *   default set in chado_get_schema_name() but in the case of multiple alter
 *   hooks, it might be different.
 * @param $context
 *   This is an array of items to provide context.
 *     - schema: this is the schema that was passed to chado_get_schema_name()
 *       and will be either "chado" or "drupal". This should be used to
 *       determine you are changing the name of the correct schema.
 *
 * @ingroup tripal_chado_query_api
 */
function hook_chado_get_schema_name_alter($schema_name, $context) {

  // First we check which schema was passed to chado_get_schema().
  // Notice that we do not use $schema_name since it may already have
  // been altered by another module.
  if ($context['schema'] == 'chado') {
    $schema_name = 'public';
  }
  // Notice that we use elseif to capture the second case rather than else. This
  // avoids the assumption that there is only one chado and one drupal schema.
  elseif ($context['schema'] == 'drupal') {
    $schema_name = 'drupal';
  }
}

/**
 * A replacement for db_select when querying Chado.
 *
 * Use this function instead of db_select when querying Chado tables.
 *
 * @param $table
 *   The base table for this query. May be a string or another SelectQuery
 *   object. If a query object is passed, it will be used as a subselect.
 * @param $alias
 *   The alias for the base table of this query.
 * @param $options
 *   An array of options to control how the query operates.
 *
 * @return
 *   A new SelectQuery object for this connection.
 *
 * @ingroup tripal_chado_query_api
 * @see \ChadoPrefixExtender::select()
 */
function chado_db_select($table, $alias = NULL, array $options = []) {
  return ChadoPrefixExtender::select($table, $alias, $options);
}
