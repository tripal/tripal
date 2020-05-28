<?php

/**
 * Provides an application programming interface (API) for describing Chado
 * tables.
 *
 * If you need the Drupal-style array definition for any table, use the
 * following:
 *
 * @code
 *
 * $chado_schema = new \ChadoSchema();
 * $table_schema = $chado_schema->getTableSchema($table_name);
 * @endcode
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The getTableSchema method determines the appropriate version of
 * Chado and uses the Drupal hook infrastructure to call the appropriate
 * hook function to retrieve the table schema.
 *
 * Additionally, here are some other examples of how to use this class:
 * @code
 *
 * // Retrieve the schema array for the organism table in chado 1.2
 * $chado_schema = new \ChadoSchema('1.2');
 * $table_schema = $chado_schema->getTableSchema('organism');
 *
 * // Retrieve all chado tables.
 * $chado_schema = new \ChadoSchema();
 * $tables = $chado_schema->getTableNames();
 * $base_tables = $chado_schema->getbaseTables();
 *
 * // Check the feature.type_id foreign key constraint
 * $chado_schema = new \ChadoSchema();
 * $exists = $chado_schema ->checkFKConstraintExists('feature','type_id');
 *
 * // Check Sequence exists
 * $chado_schema = new \ChadoSchema();
 * $exists = $chado_schema->checkSequenceExists('organism','organism_id');
 * // Or just check the primary key directly
 * $compliant = $chado_schema->checkPrimaryKey('organism');
 * @endcode
 */
class ChadoSchema {

  /**
   * @var string
   *   The current version for this site. E.g. "1.3".
   */
  protected $version = '';

  /**
   * @var string
   *   The name of the schema chado was installed in.
   */
  protected $schema_name = 'chado';

  /**
   * The ChadoSchema constructor.
   *
   * @param string $version
   *   The current version for this site. E.g. "1.3". If a version is not
   *   provided, the version of the current database will be looked up.
   */
  public function __construct($version = NULL, $schema_name = NULL) {

    // Set the version of the schema.
    if ($version === NULL) {
      $this->version = chado_get_version(TRUE);
    }
    else {
      $this->version = $version;
    }

    // Set the name of the schema.
    if ($schema_name === NULL) {
      $this->schema_name = chado_get_schema_name('chado');
    }
    else {
      $this->schema_name = $schema_name;
    }

    // Check functions require the chado schema be local and installed...
    // So lets check that now...
    if (!chado_is_local()) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_NOTICE,
        'The ChadoSchema class requires chado be installed within the drupal database
          in a separate schema for any compliance checking functionality.'
      );
    }
    if (!chado_is_installed()) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_NOTICE,
        'The ChadoSchema class requires chado be installed
          for any compliance checking functionality.'
      );
    }
  }

  /**
   * Returns the version number of the Chado this object references.
   *
   * @returns
   *   The version of Chado
   */
  public function getVersion() {
    return $this->version;
  }

  /**
   * Retrieve the name of the PostgreSQL schema housing Chado.
   *
   * @return
   *   The name of the schema.
   */
  public function getSchemaName() {
    return $this->schema_name;
  }

  /**
   * Retrieves the list of tables in the Chado schema.  By default it only
   * returns the default Chado tables, but can return custom tables added to
   * the Chado schema if requested.
   *
   * @param $include_custom
   *   Optional.  Set as TRUE to include any custom tables created in the
   *   Chado schema. Custom tables are added to Chado using the
   *   tripal_chado_chado_create_table() function.
   *
   * @returns
   *   An associative array where the key and value pairs are the Chado table
   *   names.
   */
  public function getTableNames($include_custom = FALSE) {

    $tables = [];
    if ($this->version == '1.3') {
      $tables_v1_3 = tripal_chado_chado_get_v1_3_tables();
      foreach ($tables_v1_3 as $table) {
        $tables[$table] = $table;
      }
    }
    if ($this->version == '1.2') {
      $tables_v1_2 = tripal_chado_chado_get_v1_2_tables();
      foreach ($tables_v1_2 as $table) {
        $tables[$table] = $table;
      }
    }
    if ($this->version == '1.11' or $this->version == '1.11 or older') {
      $tables_v1_11 = tripal_chado_chado_get_v1_11_tables();
      foreach ($tables_v1_11 as $table) {
        $tables[$table] = $table;
      }
    }

    // now add in the custom tables too if requested
    if ($include_custom) {
      $sql = "SELECT table FROM {tripal_custom_tables}";
      $resource = db_query($sql);

      foreach ($resource as $r) {
        $tables[$r->table] = $r->table;
      }
    }

    asort($tables);
    return $tables;

  }

  /**
   * Retrieves the chado tables Schema API array.
   *
   * @param $table
   *   The name of the table to retrieve.  The function will use the appopriate
   *   Tripal chado schema API hooks (e.g. v1.11 or v1.2).
   *
   * @returns
   *   A Drupal Schema API array defining the table.
   */
  public function getTableSchema($table) {

    // first get the chado version.
    $v = $this->version;

    // get the table array from the proper chado schema
    $v = preg_replace("/\./", "_", $v); // reformat version for hook name

    // Call the module_invoke_all.
    $hook_name = "chado_schema_v" . $v . "_" . $table;
    $table_arr = module_invoke_all($hook_name);

    // If the module_invoke_all returned nothing then let's make sure there isn't
    // An API call we can call directly.  The only time this occurs is
    // during an upgrade of a major Drupal version and tripal_core is disabled.
    if ((!$table_arr or !is_array($table_arr)) and
      function_exists('tripal_chado_' . $hook_name)) {
      $api_hook = "tripal_chado_" . $hook_name;
      $table_arr = $api_hook();
    }

    // if the table_arr is empty then maybe this is a custom table
    if (!is_array($table_arr) or count($table_arr) == 0) {
      $table_arr = $this->getCustomTableSchema($table);
    }

    return $table_arr;

  }

  /**
   * Retrieves the schema array for the specified custom table.
   *
   * @param $table
   *   The name of the table to create.
   *
   * @return
   *   A Drupal-style Schema API array definition of the table. Returns
   *   FALSE on failure.
   */
  public function getCustomTableSchema($table) {

    $sql = "SELECT schema FROM {tripal_custom_tables} WHERE table_name = :table_name";
    $results = db_query($sql, [':table_name' => $table]);
    $custom = $results->fetchObject();
    if (!$custom) {
      return FALSE;
    }
    else {
      return unserialize($custom->schema);
    }
  }

  /**
   *  Returns all chado base tables.
   *
   *  Base tables are those that contain the primary record for a data type.
   * For
   *  example, feature, organism, stock, are all base tables.  Other tables
   *  include linker tables (which link two or more base tables), property
   * tables, and relationship tables.  These provide additional information
   * about primary data records and are therefore not base tables.  This
   * function retreives only the list of tables that are considered 'base'
   * tables.
   *
   * @return
   *    An array of base table names.
   *
   * @ingroup tripal_chado_schema_api
   */
  function getBaseTables() {

    // Initialize the base tables with those tables that are missing a type.
    // Ideally they should have a type, but that's for a future version of Chado.
    $base_tables = [
      'organism',
      'project',
      'analysis',
      'biomaterial',
      'eimage',
      'assay',
    ];

    // We'll use the cvterm table to guide which tables are base tables. Typically
    // base tables (with a few exceptions) all have a type.  Iterate through the
    // referring tables.
    $schema = $this->getTableSchema('cvterm');
    $referring = $schema['referring_tables'];
    foreach ($referring as $tablename) {

      // Ignore the cvterm tables, relationships, chadoprop tables.
      if ($tablename == 'cvterm_dbxref' || $tablename == 'cvterm_relationship' ||
        $tablename == 'cvtermpath' || $tablename == 'cvtermprop' || $tablename == 'chadoprop' ||
        $tablename == 'cvtermsynonym' || preg_match('/_relationship$/', $tablename) ||
        preg_match('/_cvterm$/', $tablename) ||
        // Ignore prop tables
        preg_match('/prop$/', $tablename) || preg_match('/prop_.+$/', $tablename) ||
        // Ignore nd_tables
        preg_match('/^nd_/', $tablename)) {
        continue;
      }
      else {
        array_push($base_tables, $tablename);
      }
    }

    // Remove any linker tables that have snuck in.  Linker tables are those
    // whose foreign key constraints link to two or more base table.
    $final_list = [];
    foreach ($base_tables as $i => $tablename) {
      // A few tables break our rule and seems to look
      // like a linking table, but we want to keep it as a base table.
      if ($tablename == 'biomaterial' or $tablename == 'assay' or $tablename == 'arraydesign') {
        $final_list[] = $tablename;
        continue;
      }

      // Remove the phenotype table. It really shouldn't be a base table as
      // it is meant to store individual phenotype measurements.
      if ($tablename == 'phenotype') {
        continue;
      }
      $num_links = 0;
      $schema = $this->getTableSchema($tablename);
      $fkeys = $schema['foreign keys'];
      foreach ($fkeys as $fkid => $details) {
        $fktable = $details['table'];
        if (in_array($fktable, $base_tables)) {
          $num_links++;
        }
      }
      if ($num_links < 2) {
        $final_list[] = $tablename;
      }
    }

    // Now add in the cvterm table to the list.
    $final_list[] = 'cvterm';

    // Sort the tables and return the list.
    sort($final_list);
    return $final_list;

  }

  /**
   * Get information about which Chado base table a cvterm is mapped to.
   *
   * Vocbulary terms that represent content types in Tripal must be mapped to
   * Chado tables.  A cvterm can only be mapped to one base table in Chado.
   * This function will return an object that contains the chado table and
   * foreign key field to which the cvterm is mapped.  The 'chado_table'
   * property of the returned object contains the name of the table, and the
   * 'chado_field' property contains the name of the foreign key field (e.g.
   * type_id), and the
   * 'cvterm' property contains a cvterm object.
   *
   * @params
   *   An associative array that contains the following keys:
   *     - cvterm_id:  the cvterm ID value for the term.
   *     - vocabulary: the short name for the vocabulary (e.g. SO, GO, PATO)
   *     - accession:  the accession for the term.
   *     - bundle_id:  the ID for the bundle to which a term is associated.
   *   The 'vocabulary' and 'accession' must be used together, the 'cvterm_id'
   *   can be used on it's own.
   *
   * @return
   *   An object containing the chado_table and chado_field properties or NULL
   *   if if no mapping was found for the term.
   */
  public function getCvtermMapping($params) {
    return chado_get_cvterm_mapping($params);
  }

  /**
   * Check that any given Chado table exists.
   *
   * This function is necessary because Drupal's db_table_exists() function will
   * not look in any other schema but the one where Drupal is installed
   *
   * @param $table
   *   The name of the chado table whose existence should be checked.
   *
   * @return
   *   TRUE if the table exists in the chado schema and FALSE if it does not.
   */
  public function checkTableExists($table) {
    return chado_table_exists($table);
  }

  /**
   * Check that any given column in a Chado table exists.
   *
   * This function is necessary because Drupal's db_field_exists() will not
   * look in any other schema but the one were Drupal is installed
   *
   * @param $table
   *   The name of the chado table.
   * @param $column
   *   The name of the column in the chado table.
   *
   * @return
   *   TRUE if the column exists for the table in the chado schema and
   *   FALSE if it does not.
   *
   * @ingroup tripal_chado_schema_api
   */
  public function checkColumnExists($table, $column) {
    return chado_column_exists($table, $column);
  }

  /**
   * Check that any given column in a Chado table exists.
   *
   * This function is necessary because Drupal's db_field_exists() will not
   * look in any other schema but the one were Drupal is installed
   *
   * @param $table
   *   The name of the chado table.
   * @param $column
   *   The name of the column in the chado table.
   * @param $type
   *   (OPTIONAL) The PostgreSQL type to check for. If not supplied it will be
   *   looked up via the schema (PREFERRED).
   *
   * @return
   *   TRUE if the column type matches what we expect and
   *   FALSE if it does not.
   *
   * @ingroup tripal_chado_schema_api
   */
  public function checkColumnType($table, $column, $expected_type = NULL) {

    // Ensure this column exists before moving forward.
    if (!$this->checkColumnExists($table, $column)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_WARNING,
        'Unable to check the type of !table!column since it doesn\'t appear to exist in your site database.',
        ['!column' => $column, '!table' => $table]
      );
      return FALSE;
    }

    // Look up the type using the Schema array.
    if ($expected_type === NULL) {
      $schema = $this->getTableSchema($table, $column);

      if (is_array($schema) AND isset($schema['fields'][$column])) {
        $expected_type = $schema['fields'][$column]['type'];
      }
      else {
        tripal_report_error(
          'ChadoSchema',
          TRIPAL_WARNING,
          'Unable to check the type of !table!column due to being unable to find the schema definition.',
          ['!column' => $column, '!table' => $table]
        );
        return FALSE;
      }
    }

    // There is some flexibility in the expected type...
    // Fix that here.
    switch ($expected_type) {
      case 'int':
        $expected_type = 'integer';
        break;
      case 'serial':
        $expected_type = 'integer';
        break;
      case 'varchar':
        $expected_type = 'character varying';
        break;
      case 'datetime':
        $expected_type = 'timestamp without time zone';
        break;
      case 'char':
        $expected_type = 'character';
        break;
    }

    // Grab the type from the current database.
    $query = 'SELECT data_type
              FROM information_schema.columns
              WHERE
                table_name = :table AND
                column_name = :column AND
                table_schema = :schema
              ORDER  BY ordinal_position
              LIMIT 1';
    $type = db_query($query,
      [
        ':table' => $table,
        ':column' => $column,
        ':schema' => $this->schema_name,
      ])->fetchField();

    // Finally we do the check!
    if ($type === $expected_type) {
      return TRUE;
    }
    elseif (($expected_type == 'float') AND (($type == 'double precision') OR ($type == 'real'))) {
      return TRUE;
    }
    elseif ($type == 'smallint' AND $expected_type == 'integer') {
      return TRUE;
    }
    elseif ($type == 'bigint' AND $expected_type == 'integer') {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Check that any given sequence in a Chado table exists.
   *
   * @param table
   *   The name of the table the sequence is used in.
   * @param column
   *   The name of the column the sequence is used to populate.
   *
   * @return
   *   TRUE if the seqeuence exists in the chado schema and FALSE if it does
   *   not.
   *
   * @ingroup tripal_chado_schema_api
   */
  public function checkSequenceExists($table, $column) {

    $prefixed_table = $this->schema_name . '.' . $table;
    $sequence_name = db_query('SELECT pg_get_serial_sequence(:table, :column);',
      [':table' => $prefixed_table, ':column' => $column])->fetchField();


    // Remove prefixed table from sequence name
    $sequence_name = str_replace($this->schema_name . '.', '', $sequence_name);

    return chado_sequence_exists($sequence_name);
  }

  /**
   * Check that the primary key exists, has a sequence and a constraint.
   *
   * @param $table
   *   The table you want to check the primary key for.
   * @param $column
   *   (OPTIONAL) The name of the primary key column.
   *
   * @return
   *   TRUE if the primary key meets all the requirements and false otherwise.
   */
  public function checkPrimaryKey($table, $column = NULL) {

    // If they didn't supply the column, then we can look it up.
    if ($column === NULL) {
      $table_schema = $this->getTableSchema($table);
      $column = $table_schema['primary key'][0];
    }

    // If there is no primary key then we can't check it.
    // It neither passes nore fails validation.
    if (empty($column)) {
      tripal_report_error(
        'ChadoSchema',
        TRIPAL_NOTICE,
        'Cannot check the validity of the primary key for "!table" since there is no record of one.',
        ['!table' => $table]
      );
      return NULL;
    }

    // Check the column exists.
    $column_exists = $this->checkColumnExists($table, $column);
    if (!$column_exists) {
      return FALSE;
    }

    // First check that the sequence exists.
    $sequence_exists = $this->checkSequenceExists($table, $column);
    if (!$sequence_exists) {
      return FALSE;
    }

    // Next check the constraint is there.
    $constraint_exists = chado_query(
      "SELECT 1
      FROM information_schema.table_constraints
      WHERE table_name=:table AND constraint_type = 'PRIMARY KEY'",
      [':table' => $table])->fetchField();
    if (!$constraint_exists) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Check that the constraint exists.
   *
   * @param $table
   *   The table the constraint applies to.
   * @param $constraint_name
   *   The name of the constraint you want to check.
   * @param $type
   *   The type of constraint. Should be one of "PRIMARY KEY", "UNIQUE", or
   *   "FOREIGN KEY".
   *
   * @return
   *   TRUE if the constraint exists and false otherwise.
   */
  function checkConstraintExists($table, $constraint_name, $type) {

    // Next check the constraint is there.
    $constraint_exists = chado_query(
      "SELECT 1
      FROM information_schema.table_constraints
      WHERE table_name=:table AND constraint_type = :type AND constraint_name = :name",
      [
        ':table' => $table,
        ':name' => $constraint_name,
        ':type' => $type,
      ])->fetchField();
    if (!$constraint_exists) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Check the foreign key constrain specified exists.
   *
   * @param $base_table
   *   The name of the table the foreign key resides in. E.g. 'feature' for
   *     the feature.type_id => cvterm.cvterm_id foreign key.
   * @param $base_column
   *   The name of the column that is a foreign key in. E.g. 'type_id' for
   *     the feature.type_id => cvterm.cvterm_id foreign key.
   *
   * @return
   *   TRUE if the constraint exists and false otherwise.
   */
  function checkFKConstraintExists($base_table, $base_column) {


    // Since we don't have a constraint name, we have to use the known pattern for
    // creating these names in order to make this check.
    // This is due to PostgreSQL not storing column information for constraints
    // in the information_schema tables.
    $constraint_name = $base_table . '_' . $base_column . '_fkey';

    return $this->checkConstraintExists($base_table, $constraint_name, 'FOREIGN KEY');
  }

  /**
   * A Chado-aware replacement for the db_index_exists() function.
   *
   * @param $table
   *   The table to be altered.
   * @param $name
   *   The name of the index.
   */
  function checkIndexExists($table, $name) {
    return chado_index_exists($table, $name);
  }
}
