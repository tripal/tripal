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
   * The name of the Chado schema to use.
   *
   * @var string
   */
  private $chado_schema;


  /**
   * Instantiates a new ChadoCustomTable object.
   */
  public function __construct() {
    $this->table_name = NULL;
    $this->table_id = NULL;
    $this->chado_schema = NULL;
  }

  /**
   * Returns a ChadoConnection object with the correct schema set.
   *
   * @return \Drupal\tripal_chado\Database\ChadoConnection
   */
  protected function getChado() {
    $chado = \Drupal::service('tripal_chado.database');
    if ($this->chado_schema) {
      $chado->setSchemaName($this->chado_schema);
    }
    return $chado;
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
      throw new \Exception('ChadoCustomTable::init(). Please provide a value for the $table_name argument');
    }

    $this->table_name = $table_name;
    $this->chado_schema = $chado_schema;
    if (!$chado_schema) {
      $chado = \Drupal::service('tripal_chado.database');
      $this->chado_schema = $chado->getSchemaName();
    }
    $this->setTableId();

    // If this table doesn't exist (i.e. it has no ID) then create
    // an empty record for it.
    if (!$this->table_id) {
      $public = \Drupal::database();
      $chado = $this->getChado();
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
    $query = $public->select('tripal_custom_tables','ct');
    $query->fields('ct', ['table_id']);
    $query->condition('ct.table_name', $this->table_name);
    $query->condition('ct.chado', $this->chado_schema);
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
   * Retrieves the name of the Chado schema in which this table lives.
   *
   * @return string
   */
  public function chadoSchema() {
    return $this->chado_schema;
  }

  /**
   * Toggles the custom table's hidden stuats.
   *
   * Tables that are hidden are meant to be managed internally by the
   * Tripal module that created it and should not be changed or deleted by
   * the end-user.
   *
   * @param bool $hide
   *   Set to True to hide the table. Set to False to show the table to the
   *   end-user.
   */
  public function setHidden($hide = False) {
    $public = \Drupal::database();
    $update = $public->update('tripal_custom_tables');
    $update->fields(['hidden' => $hide == TRUE ? 1 : 0]);
    $update->condition('table_name', $this->table_name);
    $update->condition('chado', $this->chado_schema);
    $update->execute();
  }

  /**
   * Indicates if the custom table is hidden from the end-user.
   *
   * Tables that are hidden are meant to be managed internally by the
   * Tripal module that created it and should not be changed or deleted by
   * the end-user.
   */
  public function isHidden() {
    $public = \Drupal::database();
    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['hidden']);
    $query->condition('ct.table_name', $this->table_name);
    $query->condition('ct.chado', $this->chado_schema);
    $hidden = $query->execute()->fetchField();

    if ($hidden == 1) {
      return True;
    }
    return False;
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
    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['schema']);
    $query->condition('tct.table_name', $this->table_name);
    $query->condition('tct.chado', $this->chado_schema);
    $table_schema = $query->execute()->fetchField();
    if (!$table_schema) {
      return [];
    }
    return unserialize($table_schema);
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
   * @param array $table_schema
   *   The Drupal table schema array defining the table.
   * @return boolean
   *   Returns True if the schema was updated. False otherwise.
   */
  public function fixTableSchema($table_schema) {

    $logger = \Drupal::service('tripal.logger');
    if (!$this->table_id) {
      $logger->error('Cannot fix the custom table schema. Please, first run the init() function.');
      return False;
    }

    $public = \Drupal::database();
    $chado = $this->getChado();

    // Don't set the schema if it's not valid.
    $errors = ChadoCustomTable::validateTableSchema($table_schema);
    if (!empty($errors)) {
      return False;
    }

    $update = $public->update('tripal_custom_tables');
    $update->fields(['schema' => serialize($table_schema)]);
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
  public function setTableSchema($table_schema, $force = False) {

    $logger = \Drupal::service('tripal.logger');
    if (!$this->table_id) {
      $logger->error('Cannot set the schema for the custom table. Please, first run the init() function.');
      return False;
    }

    $chado = $this->getChado();

    // Don't set the schema if it's not valid.
    $errors = ChadoCustomTable::validateTableSchema($table_schema);
    if (!empty($errors)) {
      return False;
    }

    // If the table already exists then we're doing an update.
    $success = False;
    $table_exists = $chado->schema()->tableExists($this->table_name);
    if (!$table_exists) {
      $success = $this->createCustomTable($table_schema);
    }
    else {
      if ($force === True) {
        $success = $this->editCustomTable($table_schema);
      }
    }

    // If the create or update were successful then we want to set the
    // schema for the table. The fixTableSchema function already does this
    // so we'll call it rather than recode that.
    if (!$success) {
      return False;
    }
    return $this->fixTableSchema($table_schema);
  }

  /**
   * Ensures that the table schema is correctly formatted.
   *
   * Returns a list of messages indicating if any errors are present.
   *
   * @param string $table_schema
   *   The Drupal table schema array defining the table.
   *
   * @return array
   *   A list of error message strings indicating what is wrong with the
   *   schema. If the array is empty then no errors were detected.
   */
  static public function validateTableSchema($table_schema) {

    $messages = [];
    $logger = \Drupal::service('tripal.logger');
    if (!$table_schema) {
      $message = 'The custom table schema is empty.';
      $messages[] = $message;
      $logger->error($message);
      return $messages;
    }
    if ($table_schema and !is_array($table_schema)) {
      $message = 'The custom table schema is not an array';
      $messages[] = $message;
      $logger->error($message);
      return $messages;
    }

    if (is_array($table_schema) and !array_key_exists('table', $table_schema)) {
      $message = "The schema array must have key named 'table'";
      $messages[] = $message;
      $logger->error($message);
    }

    if (preg_match('/[ABCDEFGHIJKLMNOPQRSTUVWXYZ]/', $table_schema['table'])) {
      $message = "Postgres will automatically change the table name to lower-case. To prevent unwanted side-effects, please rename the table with all lower-case characters.";
      $messages[] = $message;
      $logger->error($message);
    }

    // Check index length.
    if (array_key_exists('indexes', $table_schema)) {
      foreach (array_keys($table_schema['indexes']) as $index_name) {
        if (strlen($table_schema['table'] . '_' . $index_name) > 60) {
          $message = "One or more index names appear to be too long. For example: '" .
              $table_schema['table'] . '_' . $index_name . ".'  Index names are created by " .
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
   * @param array $table_schema
   *   The Drupal table schema array defining the table.
   * @return bool
   *   True if successful. False otherwise.
   */
  private function createCustomTable($table_schema) {

    $chado = $this->getChado();
    $logger = \Drupal::service('tripal.logger');

    $table_exists = $chado->schema()->tableExists($this->table_name);
    if ($table_exists) {
      return False;
    }

    $transaction_chado = $chado->startTransaction();
    try {
      $chado->schema()->createTable($this->table_name, $table_schema);
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
   * @param array $table_schema
   *   The Drupal table schema array defining the table.
   * @return bool
   *   True if successful. False otherwise.
   */
  private function editCustomTable($table_schema) {

    $chado = $this->getChado();
    $logger = \Drupal::service('tripal.logger');

    $table_exists = $chado->schema()->tableExists($this->table_name);
    if (!$table_exists) {
      return False;
    }

    $transaction_chado = $chado->startTransaction();
    try {
      $chado->schema()->dropTable($this->table_name);
      $chado->schema()->createTable($this->table_name, $table_schema);
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
   * @param string $chado_schema
   *   Optional. The chado schema from which to retrieve custom tables. If no
   *   schema is specified then the default schema is used.
   *
   * @return array
   *  An array of table names.
   */
  static public function allCustomTables($chado_schema = NULL) {
    $tables = [];

    $public = \Drupal::database();
    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['table_name']);
    $query->condition('tct.chado', $chado_schema);
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
   * @param string $table_name
   *   The name of the table to find the ID for.
   * @param string $chado_schema
   *   Optional. The chado schema from which to find a custom tables. If no
   *   schema is specified then the default schema is used.
   *
   * @return int
   *   The custom table ID if it exists.
   */
  static public function findCustomTableId(string $table_name, string $chado_schema = NULL) {
    $public = \Drupal::database();
    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['table_id']);
    $query->condition('tct.chado', $chado_schema);
    $query->condition('tct.table_name', $table_name);
    return $query->execute()->fetchField();
  }

  /**
   * Loads the custom table whose name matches the given ID.
   *
   * @param int $table_id
   *   The ID of the custom table.
   * @return \Drupal\tripal_chado\Services\ChadoCustomTable.
   *   A ChadoCustomTable object or NULL if not found.
   */
  static public function load(int $table_id) {
    $public = \Drupal::database();

    $query = $public->select('tripal_custom_tables','tct');
    $query->fields('tct', ['table_name', 'chado']);
    $query->condition('tct.table_id', $table_id);
    $record = $query->execute()->fetchAssoc();
    if (!$record) {
      return NULL;
    }

    $custom_table = \Drupal::service('tripal_chado.custom_table');
    $custom_table->init($record['table_name'], $record['chado']);
    return $custom_table;
  }

  /**
   * Destroyes the custom table completely.
   *
   * Tripal will no longer know about the table and the table will be removed
   * from Chado. After this function is executed this object is no longer
   * usable.
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
    $chado = $this->getChado();
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