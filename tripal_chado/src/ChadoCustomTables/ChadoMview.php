<?php

namespace Drupal\tripal_chado\ChadoCustomTables;

class ChadoMview extends ChadoCustomTable {

  /**
   * The materialized view ID.
   *
   * @var int
   */
  private $mview_id;

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
  public function __construct($table_name, string $chado_schema = NULL) {

    parent::__construct($table_name, $chado_schema);

    $this->mview_id = NULL;

    if (!$table_name) {
      throw new \Exception('Please provide a value for the $table_name argument');
    }

    $this->setMviewId();

    // If this table doesn't exist (i.e. it has no ID) then create
    // an empty record for it.
    if (!$this->mview_id) {
      $public = \Drupal::database();
      $insert = $public->insert('tripal_mviews');
      $insert->fields([
        'table_id' => $this->getTableId(),
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

  public function getMviewId() {
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
    $query->condition('tm.table_id', $this->getTableId());
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
    $update->condition('table_id', $this->getTableId());
    $update->execute();
  }

  /**
   * {@inheritDoc}
   * @see \Drupal\tripal_chado\ChadoCustomTables\ChadoCustomTable::setTableSchema()
   */
  public function setTableSchema($table_schema, $force = False) {
    $success = parent::setTableSchema($table_schema, $force);
    if ($success) {
      $this->setTableValue('name', $table_schema['table']);
    }
    return $success;
  }

  /**
   * Sets the query used to populate the materialized view.
   *
   * @param string $query
   *   The SQL query used to populate the materialized view.
   */
  public function setSqlQuery(string $query) {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->getTableId()) {
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
  public function getSqlQuery() {
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
    if (!$this->getTableId()) {
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
  public function getStatus() {
    return $this->getTableValue('status');
  }

  /**
   * Toggles the materialized view's locked status.
   * 
   * In some cases, a module will provide a materialized view and
   * in such cases it is beneficial to have these be locked from editing
   * by the site maintainer.
   * 
   * @param bool $lock
   *   Set to True to lock the materialized view. Set to False to allow the
   *   materialized view to be edited by the end-user (default).
   */
  // public function setLocked($lock = False) {
  //   $public = \Drupal::database();
  //   $update = $public->update('tripal_mviews');
  //   $update->fields(['locked' => $lock == TRUE ? 1 : 0]);
  //   $update->condition('mview_id', $this->mview_id);
  //   $update->execute();
  // }

  /**
   * Indicates if the materialized view is locked from editing by the end-user.
   * 
   * In some cases, a module will provide a materialized view and
   * in such cases it is beneficial to have these be locked from editing
   * by the site maintainer.
   * 
   * Because there is no reason to have the locked status of a custom table and 
   * its materialized view to differ, we will check the 'locked' column 
   * in the tripal_custom_tables table. 
   */
  // public function isLocked() {
  //   $public = \Drupal::database();
  //   $query = $public->select('tripal_mviews', 'tmv');
  //   $query->fields('tmv', ['locked']);
  //   $query->condition('tmv.mview_id', $this->mview_id);
  //   $locked = $query->execute()->fetchField();

  //   if ($locked == 1) {
  //     return True;
  //   }
  //   return False;
  // }

  /**
   * Retrieves the timestamp for the last time the mview was populated.
   *
   * @return int
   */
  public function getLastUpdate() {
    return $this->getTableValue('last_update');
  }

  /**
   * Sets the last update for the materialized view.
   *
   *
   * @param int $timestamp
   *   The UNIX timestamp.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function setLastUpdate(int $timestamp) {

    $logger = \Drupal::service('tripal.logger');
    if (!$this->getTableId()) {
      $logger->error('Cannot set the comment for the materialized view. Please, first run the init() function.');
      return False;
    }
    return $this->setTableValue('last_update', $timestamp);
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
    if (!$this->getTableId()) {
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
    if (!$this->getTableId()) {
      $logger->error('Cannot populate the materialized view. Please, first run the init() function.');
      return False;
    }

    $public = \Drupal::database();
    $chado = $this->getChado();
    $transaction_chado = $chado->startTransaction();
    $transaction = $public->startTransaction();

    try {
      $chado->query("DELETE FROM {1:" . $this->getTableName() . "}");
      $sql_query = $this->getSqlQuery();
      $chado->query("INSERT INTO {1:" . $this->getTableName() . "} ($sql_query)");
      $results = $chado->query("SELECT COUNT(*) as num_rows FROM {1:" . $this->getTableName() . "}");
      $num_rows = $results->fetchField();
      $this->setStatus("Populated with " . $num_rows . " rows");
      $this->setLastUpdate(time());
    }
    catch (Exception $e) {
      $transaction_chado->rollback();
      $transaction->rollback();
      $logger->error('ERROR populating "' . $this->getTableName() . '": ' . $e->getMessage());
      return FALSE;
    }
    return TRUE;
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
    if (!$this->getTableId()) {
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
