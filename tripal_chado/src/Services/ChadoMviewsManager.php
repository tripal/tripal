<?php

namespace Drupal\tripal_chado\Services;

use Drupal\Core\Database\Database;
use Drupal\Core\Database\Connection;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoCustomTables\ChadoMview;

class ChadoMviewsManager extends ChadoCustomTableManager {


  /**
   * Instantiates a new ChadoCustomTableManager object.
   * 
   * @param \Drupal\Core\Database\Connection
   *  The database connection object.
   * @param Drupal\tripal_chado\Database\ChadoConnection
   *   The chado connection used to query chado.
   */
  public function __construct(Connection $connection, ChadoConnection $chado_connection) {
    $this->connection = $connection;
    $this->chado_connection = $chado_connection;
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
    $mview = new ChadoMview($table_name, $chado_schema);
    return $mview;
  }

  /**
   * Loads a materialized view whose Id matches the one provided.
   *
   * @param int $id
   *   The ID of the materialized view
   *
   * @return \Drupal\tripal_chado\ChadoCustomTables\ChadoMview
   *   A ChadoMview object or NULL if not found.
   */
  public function loadById(int $id) {
    $public = \Drupal::database();
    $query = $public->select('tripal_mviews','tm');
    $query->join('tripal_custom_tables', 'tct', 'tct.table_id = tm.table_id');
    $query->fields('tct', ['table_name']);
    $query->fields('tct', ['chado']);
    $query->condition('tm.mview_id', $id);
    $record = $query->execute()->fetchAssoc();
    if (!$record) {
      return NULL;
    }
    $mview = new ChadoMview($record['table_name'], $record['chado']);
    return $mview;
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
   * Finds the Id of the materialized view matching the name and Chado schema.
   *
   * @param string $table_name
   *   The name of the materialized view table to find the ID for.
   * @param string $chado_schema
   *   Optional. The chado schema from which to find materialized view. If no
   *   schema is specified then the default schema is used.
   *
   * @return int
   *   The id of the matching materialized view.
   */
  public function findByName(string $table_name, string $chado_schema = NULL) {
    $public = \Drupal::database();
    $query = $public->select('tripal_mviews','tm');
    $query->fields('tm', ['mview_id']);
    $query->join('tripal_custom_tables', 'tct', 'tm.table_id = tct.table_id');
    $query->condition('tct.chado', $chado_schema);
    $query->condition('tm.name', $table_name);
    return $query->execute()->fetchField();
  }

  /**
   * Retrieve a list of all the materialized views.
   *
   * @param string $chado_schema
   *   Optional. The chado schema from which to retrieve materialized views. If
   *   no schema is specified then the default schema is used.
   *
   * @return array
   *  An associatve array of the materialized views with the key being the id
   *  and the value the table name.
   */
  public function getTables(string $chado_schema = NULL) {
    $tables = [];

    $public = \Drupal::database();
    $query = $public->select('tripal_mviews','tm');
    $query->join('tripal_custom_tables', 'tct', 'tm.table_id = tct.table_id');
    $query->fields('tct', ['table_id', 'table_name']);
    $query->fields('tm', ['mview_id']);
    $query->condition('tct.chado', $chado_schema);
    $query->orderBy('tct.table_name');
    $results = $query->execute();
    while ($table = $results->fetchObject()) {
      $tables[$table->mview_id] = $table->table_name;
    }
    return $tables;
  }
}