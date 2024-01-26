<?php

namespace Drupal\tripal_chado\ChadoCustomTables;

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
    $this->table_name = NULL;
    $this->table_id = NULL;
    $this->chado_schema = NULL;

    if (!$table_name) {
      throw new \Exception('Please provide a value for the $table_name argument');
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
      $insert = $public->insert('tripal_custom_tables');
      $insert->fields([
        'table_name' => $this->table_name,
        'schema' => '',
        'chado' => $this->chado_schema,
      ]);
      $table_id = $insert->execute();
      if (!$table_id) {
        throw New \Exception('Could not add the custom table, "' . $this->table_name .
            '" for the Chado schema "' . $this->chado_schema .'".');
      }
      $this->setTableId();
    }
  }

  /**
   * Returns a ChadoConnection object with the correct schema set.
   *
   * This is just a helpder function for this class to make sure the
   * Chado schema is set as requested anytime the object is needec.
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
   * Sets the private table_id member variable.
   */
  private function setTableId() {
    $public = \Drupal::database();
    $query = $public->select('tripal_custom_tables','ct');
    $query->fields('ct', ['table_id']);
    $query->condition('ct.table_name', $this->table_name);
    $query->condition('ct.chado', $this->chado_schema);
    $results = $query->execute();
    $this->table_id = $results->fetchField();

  }

  /**
   * Retrieves the numeric ID of the custom table.
   *
   * @return integer
   */
  public function getTableId() {
    return $this->table_id;
  }

  /**
   * Retrieves the name of the custom table.
   *
   * @return string
   */
  public function getTableName() {
    return $this->table_name;
  }

  /**
   * Retrieves the name of the Chado schema in which this table lives.
   *
   * @return string
   */
  public function getChadoSchema() {
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
  public function getTableSchema() {
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
   * make sure the $force argument is set to False. But be careful. If the
   * schema does not properly match the table problems may occur when using
   * the table later.
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

    $public = \Drupal::database();
    $chado = $this->getChado();
    $transaction_chado = $chado->startTransaction();
    try {

      // Don't set the schema if it's not valid.
      $errors = ChadoCustomTable::validateTableSchema($table_schema);
      if (!empty($errors)) {
        return False;
      }

      // If the table name is the same and the user isn't forcing any changes then
      // create the table if it doesn't exist. If it does exist then leave the
      // table as is and the function will later update the saved schema.
      if ($force == False and $this->table_name == $table_schema['table']) {
        $table_exists = $chado->schema()->tableExists($this->table_name);
        if (!$table_exists) {
          $chado->schema()->createTable($this->table_name, $table_schema);
        }
      }

      // If the table name is the same and the user is forcing a change then
      // create the table if it doesn't exist. If it does exist then drop it
      // and recreate it.
      if ($force == True and $this->table_name == $table_schema['table']) {
        if ($chado->schema()->tableExists($this->table_name)) {
          $chado->schema()->dropTable($this->table_name);
        }
        $chado->schema()->createTable($this->table_name, $table_schema);
      }

      // If the table name is different in the provided schema but the user is not
      // forcing a change then this shouldn't be allowed. We don't want to update
      // the saved schema with a table name mismatch.
      if ($force == False and $this->table_name != $table_schema['table']) {
        $logger->error('Cannot change the name of the table in the schema without forcing it..');
        return False;
      }

      // If the table name is different and the force argument is true, then the
      // user is requesting a rename of the table. Make sure the name isn't
      // already taken. If not, then drop the old table and create the new one.
      if ($force == True and $this->table_name != $table_schema['table']) {

        // First check if the new table exists and if so return False.
        if ($chado->schema()->tableExists($table_schema['table'])) {
          $logger->error('Cannot rename the table as another table exists with the same name.');
          return False;
        }

        // Second, if the original table exists then delete it.
        if ($chado->schema()->tableExists($this->table_name)) {
          $chado->schema()->dropTable($this->table_name);
        }
        $chado->schema()->createTable($table_schema['table'], $table_schema);
      }

      $update = $public->update('tripal_custom_tables');
      $update->fields([
        'table_name' => $table_schema['table'],
        'schema' => serialize($table_schema)
      ]);
      $update->condition('table_id', $this->table_id);
      $update->condition('chado', $chado->getSchemaName());
      $update->execute();
    }
    catch (Exception $e) {
      $transaction_chado->rollback();
      $logger->error($e->getMessage());
      return False;
    }
    return True;
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
   * Destroyes the custom table completely.
   *
   * Tripal will no longer know about the table and the table will be removed
   * from Chado. After this function is executed this object is no longer
   * usable.
   *
   * @return bool
   *   True if successful. False otherwise.
   */
  public function delete() {
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
