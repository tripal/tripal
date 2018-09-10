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
      $sql = "SELECT table_name FROM {tripal_custom_tables}";
      $resource = db_query($sql);

      foreach ($resource as $r) {
        $tables[$r->table_name] = $r->table_name;
      }
    }

    asort($tables);
    return $tables;

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

}
