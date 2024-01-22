<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable;

class ChadoCustomTableManager {


  /**
   * Instantiates a new ChadoCustomTableManager object.
   */
  public function __construct() {

  }

  /**
   * Creates a new ChadoCustomTable object.
   *
   * This object will work with the custom table in the default Chado schema.
   * Be sure to call the setSchemaName() on the ChadoConnection object to
   * ensure the custom table is managed in the correct Chado instance.
   *
   * @param string $table_name
   *   The name of the custom table.
   * @param string $chado_schema
   *   Optional. The chado schema where the custom table will live. If no
   *   schema is specified then the default schema is used.
   *
   * @return \Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable
   */
  public function create(string $table_name, string $chado_schema = NULL) {
    $custom_table = new ChadoCustomTable($table_name, $chado_schema);
    return $custom_table;
  }

  /**
   * Loads a custom table whose table Id matches the one provided.
   *
   * @param int $id
   *   The ID of the custom table.
   *
   * @return \Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable
   *   A ChadoCustomTable object or NULL if not found.
   */
  public function loadById(int $id) {
    $public = \Drupal::database();

    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['table_name', 'chado']);
    $query->condition('tct.table_id', $id);
    $record = $query->execute()->fetchAssoc();
    if (!$record) {
      return NULL;
    }

    $custom_table = new ChadoCustomTable($record['table_name'], $record['chado']);
    return $custom_table;
  }

  /**
   * Loads a custom table that matches the given name and Chado schema.
   *
   * @param string $table_name
   *   The name of the table to find the ID for.
   * @param string $chado_schema
   *   Optional. The chado schema from which to find a custom tables. If no
   *   schema is specified then the default schema is used.
   *
   * @return \Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable
   *   A ChadoCustomTable object or NULL if not found.
   */
  public function loadbyName(string $table_name, string $chado_schema = NULL) {
    $table_id = $this->findByName($table_name, $chado_schema);
    if (!$table_id) {
      return NULL;
    }
    return $this->loadById($table_id);
  }

  /**
   * Finds the Id of the  custom table that matches the name and Chado schema.
   *
   * @param string $table_name
   *   The name of the table to find the ID for.
   * @param string $chado_schema
   *   Optional. The chado schema from which to find a custom tables. If no
   *   schema is specified then the default schema is used.
   *
   * @return int
   *   The id of the matching custom table.
   */
  public function findByName(string $table_name, string $chado_schema = NULL) {

    // Retrieve the default name of the Chado schema if it's not provided.
    if ($chado_schema === NULL) {
      $chado_schema = chado_get_schema_name('chado');
    }
    $public = \Drupal::database();
    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['table_id']);
    $query->condition('tct.chado', $chado_schema);
    $query->condition('tct.table_name', $table_name);
    return $query->execute()->fetchField();
  }

  /**
   * Retrieve a list of all Chado custom table names.
   *
   * @param string $chado_schema
   *   Optional. The chado schema from which to retrieve custom tables. If no
   *   schema is specified then the default schema is used.
   *
   * @return array
   *  An associatve array of custom tables with the key being the id and
   *  the value the table name.
   */
  public function getTables(string $chado_schema = NULL) {
    $tables = [];

    // Retrieve the default name of the Chado schema if it's not provided.
    if ($chado_schema === NULL) {
      $chado_schema = chado_get_schema_name('chado');
    }
    $public = \Drupal::database();
    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['table_id', 'table_name']);
    $query->condition('tct.chado', $chado_schema);
    $query->orderBy('table_name');
    $results = $query->execute();
    while ($table = $results->fetchObject()) {
      $tables[$table->table_id] = $table->table_name;
    }
    return $tables;
  }
}