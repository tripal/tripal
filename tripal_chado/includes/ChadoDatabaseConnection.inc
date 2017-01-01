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
 *
 */
class ChadoDatabaseConnection extends DatabaseConnection_pgsql {

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
  function __construct(array $connection_options = array()) {
    parent::__construct($connection_options);


    // Get the list of prefix search and replace that are set in the
    // settings.php file. We'll need those later.
    $psearch = $this->prefixSearch;
    $preplace = $this->prefixReplace;

    // Reset the prefix serach and replace
    $this->prefixSearch = array();
    $this->prefixReplace = array();

    $tables = chado_get_table_names(TRUE);
    foreach ($tables as $table) {
      $this->prefixSearch[] = '{' . $table . '}';
      $this->prefixReplace[] = 'chado.' . $table;
    }
    $this->prefixSearch = array_merge($this->prefixSearch, $psearch);
    $this->prefixReplace = array_merge($this->prefixReplace, $preplace);
  }

  public function prefixTables($sql) {
    $sql = str_replace($this->prefixSearch, $this->prefixReplace, $sql);
    //print_r($sql);
    return $sql;
  }


}