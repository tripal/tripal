<?php

namespace Drupal\tripal_chado\Services;

class ChadoCustomTable {

  /**
   * The name of the table.
   *
   * @var string
   */
  private $table_name;

  /**
   * The ID of the table.
   *
   * @var int
   */
  private $table_id;


  /**
   * Instantiates a new ChadoCustomTable object.
   */
  public function __construct() {
    $this->table_name = NULL;
    $this->table_id = NULL;
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
   */
  public function init($table_name) {
    if (!$table_name) {
      throw new \Exception('ChadoCustomTable::init(). Please provide a value for the $table_name argument');
    }

    $this->table_name = $table_name;
    $this->setTableId();

    // If this table doesn't exist (i.e. it has no ID) then create
    // an empty record for it.
    if (!$this->table_id) {
      $public = \Drupal::database();
      $chado = \Drupal::service('tripal_chado.database');
      $insert = $public->insert('tripal_custom_tables');
      $insert->fields([
        'table_name' => $table_name,
        'schema' => '',
        'chado' => $chado->getSchemaName(),
      ]);
      $insert->execute();
      $this->setTableId();
    }
  }

  /**
   * Sets the private table_id member variable.
   */
  private function setTableId() {
    $public = \Drupal::database();
    $chado = \Drupal::service('tripal_chado.database');
    $query = $public->select('tripal_custom_tables','ct');
    $query->fields('ct', ['table_id']);
    $query->condition('ct.table_name', $this->table_name);
    $query->condition('ct.chado', $chado->getSchemaName());
    $results = $query->execute();
    if ($results) {
      $custom_table = $results->fetchObject();
      $this->table_id = $custom_table->table_id;
    }
  }

  /**
   * Retrieves the numeric ID of the custom table.
   *
   * @return integer
   */
  public function tableId() {
    return $this->table_id;
  }

  /**
   * Retrieves the name of the custom table.
   *
   * @return string
   */
  public function tableName() {
    return $this->table_name;
  }

  /**
   * Retrieves the schema for the custom table.
   *
   * If return value is empty then it means the table schema has not yet
   * been provided or the init() function has not been called.  Use
   * the setTableSchema() function to provide one.
   *
   * @return array
   */
  public function tableSchema() {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->table_id) {
      $logger->error('Cannot get the the custom table schema. Please, first run the init() function.');
      return [];
    }

    $public = \Drupal::database();
    $chado = \Drupal::service('tripal_chado.database');

    $query = $public->select('tripal_custom_tables','ct');
    $query->fields('ct', ['table_id']);
    $query->condition('ct.table_name', $this->table_name);
    $query->condition('ct.chado', $chado->getSchemaName());
    $results = $query->execute();
    if ($results) {
      $custom_table = $results->fetchObject();
      if (empty($custom_table->schema)){
        return [];
      }
      return $custom_table->schema;
    }
    return [];
  }

  /**
   * Corrects a schema definition for a custom table.
   *
   * In the rare case where a custom table exists but the schema definition
   * is not correct (this can happen if a custom table was added outside
   * of Tripal), then the setTableSchema() function should not be used
   * as it will not update the schema or will recreate the table (if $force is
   * True). This function simply replaces what Tripal thinks the table schema
   * is.
   *
   * WARNING: providing a table schema that does not match the underlying
   * custom table design will create unknown problems.
   *
   * @param array $schema
   *   The Drupal table schema array defining the table.
   * @return boolean
   *   Returns True if the schema was updated. False otherwise.
   */
  public function fixTableSchema($schema) {

    $logger = \Drupal::service('tripal.logger');
    if (!$this->table_id) {
      $logger->error('Cannot fix the custom table schema. Please, first run the init() function.');
      return False;
    }

    $public = \Drupal::database();
    $chado = \Drupal::service('tripal_chado.database');

    // Don't set the schema if it's not valid.
    $errors = ChadoCustomTable::validateTableSchema($schema);
    if (!empty($errors)) {
      return False;
    }

    $update = $public->update('tripal_custom_tables');
    $update->fields(['schema' => serialize($schema)]);
    $update->condition('table_id', $this->table_id);
    $update->condition('chado', $chado->getSchemaName());
    $update->execute();
    return True;
  }


  /**
   * Sets the table schema.
   *
   * When setting the table schema, the table will be created in the Chado
   * schema if it doesn't exist. If the table does exist then the $force
   * argument must be set to True and the table will be dropped and recreated.
   * If not set to True then no change is made to the schema or the custom
   * table. The force argument is to prevent accidental deletion and recreation
   * of tables that may have data.
   *
   * If a mistake was made in the schema definition and it needs correction
   * use the fixTableSchema() function. This will adjust the schema but
   * will not drop and recreate the table.
   *
   *
   * @param array $schema
   *   The Drupal table schema array defining the table.
   * @param boolean $force
   *   True if the custom table should be dropped and recreated if it already
   *   exists
   * @return boolean
   *   True on successfu
   */
  public function setTableSchema($schema, $force = False) {

    $logger = \Drupal::service('tripal.logger');
    if (!$this->table_id) {
      $logger->error('Cannot set the schema for the custom table. Please, first run the init() function.');
      return False;
    }

    $chado = \Drupal::service('tripal_chado.database');

    // Don't set the schema if it's not valid.
    $errors = ChadoCustomTable::validateTableSchema($schema);
    if (!empty($errors)) {
      return False;
    }

    // If the table already exists then we're doing an update.
    $success = False;
    $table_exists = $chado->schema()->tableExists($this->table_name);
    if (!$table_exists) {
      $success = $this->createCustomTable($schema);
    }
    else {
      if ($force === True) {
        $success = $this->editCustomTable($schema);
      }
    }

    // If the create or update were successful then we want to set the
    // schema for the table. The fixTableSchema function already does this
    // so we'll call it rather than recode that.
    if (!$success) {
      return False;
    }
    return $this->fixTableSchema($schema);
  }

  /**
   * Ensures that the table schema is correctly formatted.
   *
   * Returns a list of messages indicating if any errors are present.
   *
   * @param string $schema
   *   The Drupal table schema array defining the table.
   *
   * @return array
   *   A list of error message strings indicating what is wrong with the
   *   schema. If the array is empty then no errors were detected.
   */
  static public function validateTableSchema($schema) {

    $messages = [];
    $logger = \Drupal::service('tripal.logger');
    if (!$schema) {
      $message = 'The custom table schema is missing.';
      $messages[] = $message;
      $logger->error($message);
    }
    if ($schema and !is_array($schema)) {
      $message = 'The custom table schema is not an array';
      $messages[] = $message;
      $logger->error($message);
    }

    if (is_array($schema) and !array_key_exists('table', $schema)) {
      $message = "The schema array must have key named 'table'";
      $messages[] = $message;
      $logger->error($message);
    }

    if (preg_match('/[ABCDEFGHIJKLMNOPQRSTUVWXYZ]/', $schema['table'])) {
      $message = "Postgres will automatically change the table name to lower-case. To prevent unwanted side-effects, please rename the table with all lower-case characters.";
      $messages[] = $message;
      $logger->error($message);
    }

    // Check index length.
    if (array_key_exists('indexes', $schema)) {
      foreach (array_keys($schema['indexes']) as $index_name) {
        if (strlen($schema['table'] . '_' . $index_name) > 60) {
          $message = "One or more index names appear to be too long. For example: '" .
            $schema['table'] . '_' . $index_name . ".'  Index names are created by " .
            "concatenating the table name with the index name provided " .
            "in the 'indexes' array of the schema. Please alter any indexes that " .
            "when combined with the table name are longer than 60 characters.";
          $messages[] = $message;
          $logger->error($message);
        }
      }
    }
    return $messages;
  }

  /**
   * Creates the custom table in Chado.
   *
   * @param array $schema
   *   The Drupal table schema array defining the table.
   * @return bool
   *   True if successful. False otherwise.
   */
  private function createCustomTable($schema) {

    $chado = \Drupal::service('tripal_chado.database');
    $logger = \Drupal::service('tripal.logger');

    $table_exists = $chado->schema()->tableExists($this->table_name);
    if ($table_exists) {
      return False;
    }

    $transaction_chado = $chado->startTransaction();
    try {
      $chado->schema()->createTable($this->table_name, $schema);
    }
    catch (Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return False;
    }
    return True;
  }

  /**
   * Edits the custom table in Chado.
   *
   * @param array $schema
   *   The Drupal table schema array defining the table.
   * @return bool
   *   True if successful. False otherwise.
   */
  private function editCustomTable($schema) {

    $chado = \Drupal::service('tripal_chado.database');
    $logger = \Drupal::service('tripal.logger');

    $table_exists = $chado->schema()->tableExists($this->table_name);
    if (!$table_exists) {
      return False;
    }

    $transaction_chado = $chado->startTransaction();
    try {
      $chado->schema()->dropTable($this->table_name);
      $chado->schema()->createTable($this->table_name, $schema);
    }
    catch (Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return False;
    }
    return True;
  }

  /**
   * Retrieve a list of all Chado custom table names.
   *
   * This function will only return the custom tables in the current
   * default Chado instance.
   *
   * @return array
   *  An array of table names.
   */
  static public function allCustomTables() {
    $tables = [];

    $public = \Drupal::database();
    $chado = \Drupal::service('tripal_chado.database');

    $query = $public->select('tripal_custom_tables','ct');
    $query->fields('ct', ['table_name']);
    $query->condition('ct.chado', $chado->getSchemaName());
    $query->orderBy('table_name');
    $results = $query->execute();
    while ($table_name = $results->fetchField()) {
      $tables[] = $table_name;
    }
    return $tables;
  }

  /**
   * Finds the Id of the custom table that matches the given name.
   *
   * Only searches within the default Chado schema.
   *
   * @param string $table_name
   *
   * @return int
   *   The custom table ID if it exists.
   */
  static public function findTableId(string $table_name) {
    $public = \Drupal::database();
    $chado = \Drupal::service('tripal_chado.database');

    $query = $public->select('tripal_custom_tables','ct');
    $query->fields('tm', ['table_id']);
    $query->condition('ct.chado', $chado->getSchemaName());
    $query->condition('ct.table_name', $table_name);
    return $query->execute()->fetchField();
  }

  /**
   * Finds the name of the custom table whose name matches the given ID.
   *
   * Only searches within the default Chado schema.
   *
   * @param int $table_id
   *
   * @return string
   *   The custom table table name if it exists.
   */
  static public function findTableName(int $table_id) {
    $public = \Drupal::database();
    $chado = \Drupal::service('tripal_chado.database');

    $query = $public->select('tripal_mviews','tm');
    $query->fields('tm', ['table_name']);
    $query->condition('ct.chado', $chado->getSchemaName());
    $query->condition('tm.table_id', $table_id);
    return $query->execute()->fetchField();
  }

  /**
   * Destroyes the custom table completely.
   *
   * Tripal will no longer know about the table and the table will be removed
   * from Chado. After this function is executed this object is no longer
   * usable.
   *
   * Note: if the custom table exists in multiple Chado instnaces then it
   * will only be removed from the default instance and will not be removed
   * in any other instance. Be sure to call the setSchemaName() on the
   * ChadoConnection object to ensure the custom table is deleted in the
   * correct Chado instance.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function destroy() {
    $logger = \Drupal::service('tripal.logger');
    if (!$this->table_id) {
      $logger->error('Cannot destroy the custom table. Please, first run the init() function.');
      return False;
    }
    $public = \Drupal::database();
    $delete = $public->delete('tripal_custom_tables');
    $delete->condition('table_id', $this->table_id);
    $delete->execute();

    $this->deleteCustomTable();

    $this->table_id = NULL;
    $this->table_name = NULL;
    return True;
  }

  /**
   * Deletes the table in Chado.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  private function deleteCustomTable() {
    $logger = \Drupal::service('tripal.logger');
    $chado = \Drupal::service('tripal_chado.database');
    $transaction_chado = $chado->startTransaction();

    $table_exists = $chado->schema()->tableExists($this->table_name);
    if (!$table_exists) {
      return True;
    }

    try {
      $chado->schema()->dropTable($this->table_name);
      if ($chado->schema()->tableExists($this->table_name)) {
        $logger->error('Could not delete the ' . $this->table_name . ' table. Check the database server logs.');
        return False;
      }
    }
    catch (Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return False;
    }
    return True;
  }
}