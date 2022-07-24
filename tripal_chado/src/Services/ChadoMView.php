<?php

namespace Drupal\tripal_chado\Services;

class ChadoMView extends ChadoCustomTable {

  /**
   * The materialized view ID.
   *
   * @var int
   */
  private $mview_id;

  /**
   * Instantiates a new ChadoCustomTable object.
   */
  public function __construct() {
    parent::__construct();

    $this->custom_table = NULL;
    $this->mview_id = NULL;
  }

  /**
   * Initializes the service object with a table name.
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
   */
  public function init($table_name, string $chado_schema = NULL) {

    if (!$table_name) {
      throw new \Exception('ChadoMView::init(). Please provide a value for the $table_name argument');
    }
    parent::init($table_name, $chado_schema);

    $this->setMviewId();

    // If this table doesn't exist (i.e. it has no ID) then create
    // an empty record for it.
    if (!$this->mview_id) {
      $public = \Drupal::database();
      $insert = $public->insert('tripal_mviews');
      $insert->fields([
        'table_id' => $this->tableId(),
        'name' => $table_name,
        'query' => '',
      ]);
      $insert->execute();
      $this->setMviewId();
    }
  }

  /**
   * Sets the private table_id member variable.
   */
  private function setMviewId() {
    $mview_id = $this->getTableValue('mview_id');
    $this->mview_id = $mview_id;
  }

  public function mviewId() {
    return $this->mview_id;
  }

  /**
   * Retrieves the value from a specified column in the tripal_mviews table.
   *
   * @param string $column
   *   The name of the column to get the value for.
   * @return
   *   The value for the column.
   */
  private function getTableValue($column) {
    $public = \Drupal::database();
    $query = $public->select('tripal_mviews','tm');
    $query->fields('tm', [$column]);
    $query->condition('tm.table_id', $this->tableId());
    $results = $query->execute();
    if ($results) {
      return $results->fetchField();
    }
    return NULL;
  }

  /**
   * Sets the value for a specified column in the tripal_mviews table.
   *
   * @param string $column
   *   The column name to set the value for.
   * @param mixed $value
   *   The value to set.
   */
  private function setTableValue($column, $value) {
    $public = \Drupal::database();
    $update = $public->update('tripal_mviews');
    $update->fields([$column => $value]);
    $update->condition('table_id', $this->tableId());
    $update->execute();
  }

  /**
   * Sets the query used to populate the materialized view.
   *
   * @param string $query
   *   The SQL query used to populate the materialized view.
   */
  public function setSqlQuery(string $query) {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->tableId()) {
      $logger->error('Cannot set the SQL query for the materialized view. Please, first run the init() function.');
      return False;
    }
    return $this->setTableValue('query', $query);
  }

  /**
   * Retrieves the SQL query used to populate the materialized view.
   *
   * @return string
   */
  public function sqlQuery() {
    return $this->getTableValue('query');
  }

  /**
   * Sets the status after populating the materialized view.
   *
   * @param string status
   *   The status message.
   */
  private function setStatus($status) {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->tableId()) {
      $logger->error('Cannot set status for the materialized view. Please, first run the init() function.');
      return False;
    }
    return $this->setTableValue('status', $status);
  }

  /**
   * Retrieves the the status of the last time the mview was populated.
   *
   * @return string
   */
  public function status() {
    return $this->getTableValue('status');
  }

  /**
   * Retrieves the timestamp for the last time the mview was populated.
   *
   * @return int
   */
  public function lastUpdate() {
    return $this->getTableValue('last_update');
  }

  /**
   * Sets the comment for the materialized view.
   *
   * The comment should describe to others the puporse of the table and
   * perhaps the data used to populate it.
   *
   * @param string $comment
   *   The comment for the materialized view.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function setComment(string $comment) {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->tableId()) {
      $logger->error('Cannot set the comment for the materialized view. Please, first run the init() function.');
      return False;
    }
    return $this->setTableValue('comment', $comment);
  }

  /**
   * Retrieves the comment for materialized view.
   *
   * @return string
   */
  public function comment() {
    return $this->getTableValue('comment');
  }

  /**
   * Populates the materialized view with rows of data.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function populate() {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->tableId()) {
      $logger->error('Cannot populate the materialized view. Please, first run the init() function.');
      return False;
    }

    $public = \Drupal::database();
    $chado = $this->getChado();
    $transaction_chado = $chado->startTransaction();
    $transaction = $public->startTransaction();

    try {
      $chado->query("DELETE FROM {" . $this->tableName() . "}");
      $num_rows = $chado->query("INSERT INTO {" . $this->tableName() . "} ($this->sqlQuery())");
      $this->setStatus("Populated with " . $num_rows . " rows");
    }
    catch (Exception $e) {
      $transaction_chado->rollback();
      $transaction->rollback();
      $logger->error('ERROR populating "' . $this->tableName() . '": ' . $e->getMessage());
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Retrieve a list of all materialized views.
   *
   * @param string $chado_schema
   *   Optional. The chado schema from which to retrieve materialized views. If
   *   no schema is specified then the default schema is used.
   *
   * @return array
   *  An array of table names.
   */
  static public function allMViews(string $chado_schema = NULL) {
    $mviews = [];

    $public = \Drupal::database();
    $query = $public->select('tripal_mviews','tm');
    $query->fields('tm', ['name']);
    $query->join('tripal_custom_tables', 'ct', 'tm.table_id = ct.table_id');
    $query->condition('ct.chado', $chado_schema);
    $query->orderBy('tm.name');
    $results = $query->execute();
    while ($name = $results->fetchField()) {
      $mviews[] = $name;
    }
    return $mviews;
  }

  /**
   * Finds the Id of the materialized view that matches the given name.
   *
   * Only searches within the default Chado schema.
   *
   * @param string $table_name
   *
   * @param string $chado_schema
   *   Optional. The chado schema from which to retrieve materialized views. If
   *   no schema is specified then the default schema is used.
   *
   * @return int
   *   The materialized view ID if it exists.
   */
  static public function findMviewId(string $table_name, string $chado_schema = NULL) {
    $public = \Drupal::database();
    $query = $public->select('tripal_mviews','tm');
    $query->fields('tm', ['mview_id']);
    $query->join('tripal_custom_tables', 'ct', 'tm.table_id = ct.table_id');
    $query->condition('ct.chado', $chado_schema);
    $query->condition('tm.name', $table_name);
    return $query->execute()->fetchField();
  }

  /**
   * Loads the materialize view whose name matches the given ID.
   *
   * @param int $mview_id
   *   The ID of the materialized view.
   * @return \Drupal\tripal_chado\Services\ChadoMview.
   *   A ChadoMview object or NULL if not found.
   */
  static public function loadMView(int $mview_id) {
    $public = \Drupal::database();
    $query = $public->select('tripal_mviews','tm');
    $query->join('tripal_custom_tables', 'tct', 'tct.table_id = tm.table_id');
    $query->fields('tct', ['table_name']);
    $query->fields('tct', ['chado']);
    $query->condition('tm.mview_id', $mview_id);
    $results = $query->execute();
    if (!$results) {
      return NULL;
    }
    $record = $results->fetchAssoc();

    $mview = \Drupal::service('tripal_chado.materialized_view');
    $mview->init($record['table_name'], $record['chado']);
    return $mview;
  }

  /**
   * Destroyes the materialized view completely.
   *
   * Tripal will no longer know about the table and the table will be removed
   * from Chado. After this function is executed this object is no longer
   * usable.
   *
   * Note: if the materialized view exists in multiple Chado instnaces then it
   * will only be removed from the default instance and will not be removed
   * in any other instance. Be sure to call the setSchemaName() on the
   * ChadoConnection object to ensure the view is deleted in the
   * correct Chado instance.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function destroy() {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->table_id) {
      $logger->error('Cannot destroy the materialized view. Please, first run the init() function.');
      return False;
    }

    $public = \Drupal::database();
    $delete = $public->delete('tripal_mviews');
    $delete->condition('mview_id', $this->mview_id);
    $delete->execute();

    $this->mview_id = NULL;

    return parent::destroy();
  }
}