<?php
/**
 * Provides an application programming interface (API) for describing Chado tables.
 *
 * The functions provided in this documentation should not be called as is, but if you need
 * the Drupal-style array definition for any table, use the following function
 * call:
 *
 *   $table_desc = chado_get_schema($table)
 *
 * where the variable $table contains the name of the table you want to
 * retireve.  The chado_get_schema function determines the appropriate version of
 * Chado and uses the Drupal hook infrastructure to call the appropriate
 * hook function to retrieve the table schema.
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
   *   The current version for this site. E.g. "1.3". If a version is not provided, the
   *   version of the current database will be looked up.
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
   * Retrieves the list of tables in the Chado schema.  By default it only returns
   * the default Chado tables, but can return custom tables added to the
   * Chado schema if requested.
   *
   * @param $include_custom
   *   Optional.  Set as TRUE to include any custom tables created in the
   *   Chado schema. Custom tables are added to Chado using the
   *   tripal_chado_chado_create_table() function.
   *
   * @returns
   *   An associative array where the key and value pairs are the Chado table names.
   */
  public function getTableNames($include_custom = FALSE) {

    $tables = array();
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
    if ($this->version == '1.11' or $v == '1.11 or older') {
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
      $table_arr = chado_get_custom_table_schema($table);
    }

    return $table_arr;

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
   * @param sequence
   *   The name of the sequence
   * @return
   *   TRUE if the seqeuence exists in the chado schema and FALSE if it does not.
   *
   * @ingroup tripal_chado_schema_api
   */
  public function checkSequenceExists($sequence) {
    return chado_sequence_exists($sequence);
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
