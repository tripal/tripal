<?php

/**
 * Overrides the DatabaseConnection_pgsql.
 *
 * The primary purpose of this class is to allow for prefixing of Chado tables.
 * By default the only way to support this is to add an array to the 'prefix'
 * key of the settings.php file.  But this is problematic. For example, what
 * if there is a contact table in the Drupal database as well as one in Chado.
 * The default prefix replacement would always rewrite it to be the one in
 * Chado.  This class is intended to be used when the Chado tables
 * are needed.
 */
class ChadoDatabaseConnection extends SelectQueryExtender {
  /**
   * A replacement constructor for DatabaseConnection_pgsql::__construct.
   *
   * The primary purpose for overiding the constructor is to dynamically add
   * a set of prefixes for replacing. This will allow Chado tables to be
   * prefixed with the 'chado.' schema prefix.  The alternative to overridding
   * the DatabaseConnection_pgsql is to ask the end-user to add a prefix
   * entry for every Chado table and custom table they create.  That's not
   * very manageable.
   */
  public function __construct() {

  }

  public function select($table, $alias = NULL, array $options = []) {
    $table = $this->prefixTables($table);

    $this->query->select($table, $alias, $options);

    return $this;
  }

  public function join($table, $alias = NULL, $condition = NULL, $arguments = []) {
    $table = $this->prefixTables($table);

    $this->query->join($table, $alias, $condition, $arguments);

    return $this;
  }

  public function prefixTables($sql) {
    $chado_schema_name = chado_get_schema_name('chado');
    $drupal_schema_name = chado_get_schema_name('drupal');
    $sql = preg_replace('/\{(.*?)\}/', $chado_schema_name.'.$1', $sql);
    $sql = preg_replace('/\[(\w+)\]/', $drupal_schema_name.'.$1', $sql);

    return $sql;
  }
}
