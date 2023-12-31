<?php

namespace Drupal\tripal_chado\TripalStorage;

use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Services\ChadoFieldDebugger;
use Symfony\Component\Validator\ConstraintViolation;

/**
 * A helper class for use by the ChadoStorage Plugin.
 *
 */
class ChadoRecords  {

  /**
   * An associative array that holds the information needed to
   * perform a variety of queries for the ChadoStorage plugin
   *
   * @var array
   */
  protected $records = [];

  /**
   * A collection of the primary key values for various base tables.
   *
   * This is populated as records are loaded/inserted and then used to later
   * update foreign key values for dependant tables.
   *
   * @var array
   *   key is the table alias and value is the primary key value for the
   *   record with that table alias. If the ID is not yet known then the
   *   value is NULL.
   */
  protected $base_record_ids = [];

  /**
   * Holds the violations during validatin.
   *
   * @var array
   */
  protected $violations = [];


  /**
   * A service to provide debugging for fields to developers.
   *
   * @var \Drupal\tripal_chado\Services\ChadoFieldDebugger
   */
  protected $field_debugger;


  /**
   * The values array passed to CharoStorage. An Associative array 5-levels deep
   *
   *   The 1st level is the field name (e.g. ChadoOrganismDefault).
   *   The 2nd level is the delta value (e.g. 0).
   *   The 3rd level is a field key name (i.e. record_id + value).
   *   The 4th level must contain the following three keys/value pairs
   *   - "value": a \Drupal\tripal\TripalStorage\StoragePropertyValue object
   *   - "type": a\Drupal\tripal\TripalStorage\StoragePropertyType object
   *   - "definition": a \Drupal\Field\Entity\FieldConfig object
   *
   * @var array
   */
  protected $values;

  /**
   * The database connection for querying Chado.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  protected $connection;


  /**
   * Constructor
   *
   * @param ChadoFieldDebugger $field_debugger
   */
  public function __construct(ChadoFieldDebugger $field_debugger, array $values, ChadoConnection $connection) {
    $this->values = $values;
    $this->field_debugger = $field_debugger;
    $this->connection = $connection;
  }

  /**
   *
   * @param array $elements
   * @param string $key
   * @param string $method
   * @throws \Exception
   */
  protected function checkElement($elements, $key, $method, $what) {
    if (!array_key_exists($key, $elements)) {
      throw new \Exception(t('@method a ChadoRecord @what without a "@key" element: @elements',
          ['@method' => $method, '@what' => $what, '@key' => $key, '@elements' => print_r($elements, TRUE)]));
    }
  }

  /**
   *
   * @param array $elements
   * @throws \Exception
   */
  protected function initTable($elements) {

    // Make sure all of the required elements are preesent
    $this->checkElement($elements, 'chado_table', 'Initializing', 'table');
    $this->checkElement($elements, 'table_alias', 'Initializing', 'table');
    $this->checkElement($elements, 'delta', 'Initializing', 'table');

    // Get the items needed to initalize a table.
    $chado_table = $elements['chado_table'];
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];

    // if the table has not been initialized then do so.
    if (!array_key_exists($table_alias, $this->records)) {
      $this->records[$table_alias] = [];
      $this->records[$table_alias]['chado_table'] = $chado_table;
      $this->records[$table_alias]['items'] = [];
    }

    // Intialize the delta if it doesn't exist.
    if (!array_key_exists($delta, $this->records[$table_alias]['items'])) {
      $this->records[$table_alias]['items'][$delta] = [
        'fields' => [],
        'conditions' => [],
        'joins' => [],
        'delete_if_empty' => []
      ];
    }
  }

  /**
   *
   * @param array $elements
   * @throws \Exception
   */
  public function setField(array $elements) {

    // Make sure the table is initalized.
    $this->initTable($elements);

    // Make sure all of the required elements are preesent.
    $this->checkElement($elements, 'chado_column', 'Setting', 'field');
    $this->checkElement($elements, 'column_alias', 'Setting', 'field');
    $this->checkElement($elements, 'value', 'Setting', 'field');

    // Get the elements needed to add a field.
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];
    $chado_column = $elements['chado_column'];
    $column_alias = $elements['column_alias'];
    $value = $elements['value'];

    // Add the field.
    $this->records[$table_alias]['items'][$delta]['fields'][$column_alias] = $value;

    // Add the optional delete_if_empty.
    if (array_key_exists('delete_if_empty', $elements) and $elements['delete_if_empty'] === TRUE) {
      $this->checkElement($elements, 'empty_value', 'Adding', 'field');
      $this->records[$table_alias]['items'][$delta]['delete_if_empty'][] = [
        'chado_column' => $chado_column,
        'empty_value' => $elements['empty_value']
      ];
    }
  }


  /**
   * Sets the values of a given field.
   *
   * The field must have already been added using the setField() function.
   * This function should be used with care.  The values set should not be
   * more or less than the values currently in the field.
   *
   * @param array $elements
   */
  public function setFieldValues(array $elements) {

    // Make sure all of the required elements are preesent.
    $this->checkElement($elements, 'table_alias', 'Setting', 'field value');
    $this->checkElement($elements, 'delta', 'Setting', 'field value');
    $this->checkElement($elements, 'values', 'Setting', 'field value');

    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];
    $values = $elements['values'];

    if (!array_key_exists($table_alias, $this->records)) {
      throw new \Exception(t('Cannot update fields in a ChadoRecord for a field that has not been added yet: @elements',
          ['@elements' => print_r($elements, TRUE)]));
    }

    // @todo: we should make sure that all of the fields in the $values array match
    // those that are already present for the field.

    $this->records[$table_alias]['items'][$delta]['fields'] = $values;
  }

  /**
   *
   * @param $elements
   * @throws \Exception
   */
  public function setCondition(array $elements) {

    // Make sure the table is initalized.
    $this->initTable($elements);

    // Make sure all of the required elements are preesent
    $this->checkElement($elements, 'table_alias', 'Setting', 'condition');
    $this->checkElement($elements, 'delta', 'Setting', 'condition');
    $this->checkElement($elements, 'column_alias', 'Setting', 'condition');
    $this->checkElement($elements, 'value', 'Setting', 'condition');

    // Get the elements needed to add a condition.
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];
    $column_alias = $elements['column_alias'];
    $value = $elements['value'];
    $operation = array_key_exists('operation', $elements) ? $elements['operation'] : '=';

    // Add the condition.
    $this->records[$table_alias]['items'][$delta]['conditions'][$column_alias] = ['value' => $value, 'operation' => $operation];
  }

  /**
   *
   * @param array $elements
   */
  public function setJoin(array $elements) {

    // Make sure the table is initalized.
    $this->initTable($elements);

    // Make sure all of the required elements are preesent
    $this->checkElement($elements, 'join_path', 'Setting', 'join');
    $this->checkElement($elements, 'join_type', 'Setting', 'join');
    $this->checkElement($elements, 'left_table', 'Setting', 'join');
    $this->checkElement($elements, 'left_column', 'Setting', 'join');
    $this->checkElement($elements, 'right_table', 'Setting', 'join');
    $this->checkElement($elements, 'right_column', 'Setting', 'join');
    $this->checkElement($elements, 'left_alias', 'Setting', 'join');
    $this->checkElement($elements, 'right_alias', 'Setting', 'join');

    // Get the elements needed to add a join..
    $chado_table = $elements['chado_table'];
    $delta = $elements['delta'];
    $join_path = $elements['join_path'];
    $join_type = $elements['join_type'];
    $left_table = $elements['left_table'];
    $left_column = $elements['left_column'];
    $right_table = $elements['right_table'];
    $right_column = $elements['right_column'];
    $left_alias = $elements['left_alias'];
    $right_alias = $elements['right_alias'];

    // Add the join.
    $this->records[$chado_table]['items'][$delta]['joins'][$join_path]['on'] = [
      'type' => $join_type,
      'left_table' => $left_table,
      'left_column' => $left_column,
      'right_table' => $right_table,
      'right_column' => $right_column,
      'left_alias' => $left_alias,
      'right_alias' => $right_alias,
    ];
  }

  /**
   *
   * @param array $elements
   */
  public function setJoinCols(array $elements) {

    // Make sure the table is initalized.
    $this->initTable($elements);

    // Make sure all of the required elements are preesent
    $this->checkElement($elements, 'join_path', 'Setting', 'join column');
    $this->checkElement($elements, 'chado_column', 'Setting', 'join column');
    $this->checkElement($elements, 'column_alias', 'Setting', 'join column');
    $this->checkElement($elements, 'field_name', 'Setting', 'join column');
    $this->checkElement($elements, 'property_key', 'Setting', 'join column');

    // Get the elements needed to add a join column.
    $chado_table = $elements['chado_table'];
    $delta = $elements['delta'];
    $join_path = $elements['join_path'];
    $chado_column = $elements['chado_column'];
    $colum_alias = $elements['column_alias'];
    $field_name = $elements['field_name'];
    $property_key = $elements['property_key'];

    // Add the join column.
    $this->records[$chado_table]['items'][$delta]['joins'][$join_path]['columns'][] = [
      $chado_column,
      $colum_alias,
      $field_name,
      $property_key
    ];
  }

  /**
   * Replaces aall IDs if they are known.
   */
  public function setIDs() {

    foreach ($this->getTables() as $table_name) {
      foreach ($this->records[$table_name]['items'] as $delta => $record) {

        // First for all the fields...
        if (array_key_exists('fields', $record)) {
          foreach ($record['fields'] as $chado_column => $val) {
            if (is_array($val) and $val[0] == 'REPLACE_BASE_RECORD_ID') {
              $core_table = $val[1];

              // If the core table is set in the base record ids array and the
              // value is not 0 then we can set this chado field now!
              if (array_key_exists($core_table, $this->base_record_ids) and $this->base_record_ids[$core_table] != 0) {
                $this->records[$table_name][$delta]['fields'][$chado_column] = $this->base_record_ids[$core_table];
              }
            }
          }
        }
        // All records should have conditions set!
        if (!array_key_exists('conditions', $record)) {
          throw new \Exception('All Chado records built should have a conditions array set by this point. However, the following record does not: ' . print_r($record, TRUE));
        }
        foreach ($record['conditions'] as $chado_column => $val) {
          if (is_array($val['value']) and $val['value'][0] == 'REPLACE_BASE_RECORD_ID') {
            $core_table = $val['value'][1];

            // If the core table is set in the base record ids array and the
            // value is not 0 then we can set this condition now!
            if (array_key_exists($core_table, $this->base_record_ids) and $this->base_record_ids[$core_table] != 0) {
              $this->records[$table_name][$delta]['conditions'][$chado_column]['value'] = $this->base_record_ids[$core_table];
            }
          }
        }
      }
    }
  }

  /**
   *
   * @param string $base_table
   * @param int $record_id
   */
  public function setBaseRecord(string $base_table, int $record_id) {
    if (!array_key_exists($base_table, $this->base_record_ids) and $record_id > 0) {
      $this->base_record_ids[$base_table] = $record_id;
    }
  }

  /**
   * Returns the list of base tables.
   *
   * @return array
   */
  public function getBaseTables() {
    return array_keys($this->base_record_ids);
  }

  /**
   * Returns the list of tables currently handled by this object.
   *
   * @return array
   */
  public function getTables() {
    return array_keys($this->records);
  }


  /**
   *
   * @param string $table_alias
   * @param int $delta
   * @param string $column_alias
   */
  public function getValue(string $table_alias, int $delta, $column_alias) {
    return $this->records[$table_alias]['items'][$delta]['fields'][$column_alias];
  }

  /**
   *
   * @param string $table_alias
   * @param int  $delta
   * @param string $column_alias
   * @param mixed $value
   */
  public function setValue(string $table_alias, int $delta, $column_alias, $value) {
    $this->records[$table_alias]['items'][$delta]['fields'][$column_alias] = $value;
  }
  /**
   *
   * @param string $base_table
   * @return NULL
   */
  public function getBaseRecordID(string $base_table) {
    if (array_key_exists($base_table, $this->base_record_ids)) {
      return $this->base_record_ids[$base_table];
    }
    return NULL;
  }

  /***
   *
   * @return array
   */
  public function getBaseRecords() {
    return $this->base_record_ids;
  }

  /**
   *
   * @return array
   */
  public function getRecords() {
    return $this->records;
  }

  /**
   *
   */
  public function setRecords($records) {
    $this->records = $records;
  }

  /**
   *  Provides a series of validation checks on the ChadoRecord records.
   *
   *  If any of the records do not pass a validation check then these are
   *  returned as an array of violoations.
   *
   *  @return  array of ConstraintViolation
   */
  public function validate() {

    // Reset the violations list.
    $this->violations = [];

    // We only need to validate the base table properties because
    // the linker table values get completely replaced on an update and
    // should not exist for an insert.
    foreach ($this->base_record_ids as $base_table => $record_id) {
      foreach ($this->records[$base_table] as $delta => $record) {
        $record = $this->records[$base_table][$delta];
        $this->validateRequired($base_table, $record_id, $record);
        $this->validateTypes($base_table, $record_id, $record);
        $this->validateSize($base_table, $record_id, $record);

        // Don't do the SQL checks if there are previous problems.
        if (count($this->violations) == 0) {
          $this->validateUnique($base_table, $record_id, $record);
          $this->validateFKs($base_table, $record_id, $record);
        }
      }
    }

    return $this->violations;
  }

  /**
   * Checks that foreign key fields exist in the record for the given table.
   *
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   */
  private function validateFKs($chado_table, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_fks = [];
    if (!array_key_exists('foreign keys', $table_def)) {
      return;
    }
    $fkeys = $table_def['foreign keys'];
    foreach ($fkeys as $fk_table => $info) {
      foreach ($info['columns'] as $lcol => $rcol) {

        // If the FK is not set in the record then skip it.
        if (!array_key_exists($lcol, $record['fields'])) {
          continue;
        }

        // If an FK allows nulls and the value is null then skip this one.
        $col_val = $record['fields'][$lcol];
        if ($table_def['fields'][$lcol]['not null'] == FALSE and $col_val === NULL) {
          continue;
        }

        // Check if the id is present in the FK table.
        $query = $this->connection->select($fk_table, 'fk');
        $query->fields('fk', [$rcol]);
        $query->condition($rcol, $col_val);
        $fk_id = $query->execute()->fetchField();
        if (!$fk_id) {
          $bad_fks[] = $lcol;
        }
      }
    }

    if (count($bad_fks) > 0) {
      // Documentation for how to create a violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values have a missing linked record in the data store: ';
      $params = [];
      foreach ($bad_fks as $col) {
        $message .=  ucfirst($col) . ", ";
      }
      $message = substr($message, 0, -1) . '.';
      $this->violations[] = new ConstraintViolation(t($message, $params)->render(),
          $message, $params, '', NULL, '', 1, 0, NULL, '');
    }
  }

  /**
   * Checks that foreign key values exist.
   *
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   */
  private function validateTypes($chado_table, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_types = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Skip fields without values. If they are required
      // but missing then the validateRequired() function will check those.
      if (!$col_val) {
        continue;
      }

      if ($info['type'] == 'integer' or $info['type'] == 'bigint' or
          $info['type'] == 'smallint' or $info['type'] == 'serial') {
        if (!preg_match('/^\d+$/', $col_val)) {
          $bad_types[$col] = 'Integer';
        }
      }
      else if ($info['type'] == 'boolean') {
        if (!is_bool($col_val) and !preg_match('/^[01]$/', $col_val)) {
          $bad_types[$col] = 'Boolean';
        }
      }
      else if ($info['type'] == 'timestamp without time zone' or $info['type'] == 'date') {
        if (!is_integer($col_val)) {
          $bad_types[$col] = 'Timestamp';
        }
      }
      else if ($info['type'] == 'character varying' or $info['type'] == 'character' or
        $info['type'] == 'text') {
        // Do nothing.
      }
      else if ($info['type'] == 'double precision' or $info['type'] == 'real') {
        if (!is_numeric($col_val)) {
          $bad_types[$col] = 'Number';
        }
      }

      if (count($bad_types) > 0) {
        // Documentation for how to create a violation is here
        // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
        $message = 'The item cannot be saved because the following values are of the wrong type: ';
        $params = [];
        foreach ($bad_types as $col => $col_type) {
          $message .=  ucfirst($col) . " should be $col_type. " ;
        }
        $this->violations[] = new ConstraintViolation(t($message, $params)->render(),
            $message, $params, '', NULL, '', 1, 0, NULL, '');
      }
    }
  }


  /**
   * Checks that size of the value isn't too large
   *
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   */
  private function validateSize($chado_table, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    $bad_sizes = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Skip fields without values. If they are required
      // but missing then the validateRequired() function will check those.
      if (!$col_val) {
        continue;
      }

      // If the column has a size then check it.
      if (array_key_exists('size', $info)) {

        // If this is a string type column.
        if ($info['type'] == 'character varying' or
            $info['type'] == 'character' or
            $info['type'] == 'text') {
              if (strlen($col_val) > $info['size']) {
                $bad_sizes[$col] = $info['size'];
              }
            }
      }
    }

    if (count($bad_sizes) > 0) {
      // Documentation for how to create a violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values are too large. ';
      $params = [];
      foreach ($bad_sizes as $col => $size) {
        $message .=  ucfirst($col) . " should be less than $size characters long. " ;
      }
      $this->violations[] = new ConstraintViolation(t($message, $params)->render(),
          $message, $params, '', NULL, '', 1, 0, NULL, '');
    }
  }

  /**
   * Checks the unique constraint of the table.
   *
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   */
  private function validateUnique($chado_table, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);

    // Check if we are violating a unique constraint (if it's an insert)
    if (array_key_exists('unique keys',  $table_def)) {
      $pkey = $table_def['primary key'];

      // Iterate through the unique constraints and see if the record
      // violates it.
      $ukeys = $table_def['unique keys'];
      foreach ($ukeys as $ukey_name => $ukey_cols) {
        $ukey_cols = explode(',', $ukey_cols);
        $query = $this->connection->select('1:'. $chado_table, $chado_table);
        $query->fields($chado_table);
        foreach ($ukey_cols as $col) {
          $col = trim($col);
          $col_val = NULL;
          if (array_key_exists($col, $record['fields'])) {
            $col_val = $record['fields'][$col];
          }
          // If there is not a NOT NULL constraint on this column,
          // and it is of a string type, then we need to handle
          // empty values specially, since they might be stored
          // as either NULL or as an empty string in the database
          // table. Create a condition that checks for both. For
          // other types, e.g. integer, just check for null.
          if ($table_def['fields'][$col]['not null'] == FALSE and !$col_val) {
            if (in_array($table_def['fields'][$col]['type'],
                ['character', 'character varying', 'text'])) {
                  $query->condition($query->orConditionGroup()
                      ->condition($col, '', '=')
                      ->isNull($col));
                }
                else {
                  $query->isNull($col);
                }
          }
          else {
            $query->condition($col, $col_val);
          }
        }

        // If we have matching record, check for a unique constraint
        // violation.
        $match = $query->execute()->fetchObject();
        if ($match) {

          // Add a constraint violation if we have a match and the
          // record_id is 0. This would be an insert but a record already
          // exists. Or, if the record_id isn't the same as the  matched
          // record. This is an update that conflicts with an existing
          // record.
          if (($record_id == 0) or ($record_id != $match->$pkey)) {
            // Documentation for how to create a violation is here
            // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
            $message = 'The item cannot be saved as another already exists with the following values. ';
            $params = [];
            foreach ($ukey_cols as $col) {
              $col = trim($col);
              $col_val = NULL;
              if (array_key_exists($col, $record['fields'])) {
                $col_val = $record['fields'][$col];
              }
              if ($table_def['fields'][$col]['not null'] == FALSE and !$col_val) {
                continue;
              }
              $message .=  ucfirst($col) . ": '@$col'. ";
              $params["@$col"] = $col_val;
            }
            $this->violations[] = new ConstraintViolation(t($message, $params)->render(),
                $message, $params, '', NULL, '', 1, 0, NULL, '');
          }
        }
      }
    }
  }


  /**
   * Checks that required fields have values.
   *
   * @param string $chado_table
   *   The name of the table
   * @param int $record_id
   *   The record ID of the record.
   * @param array $record
   *   The record to validate
   */
  private function validateRequired($chado_table, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    $missing = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['fields'])) {
        $col_val = $record['fields'][$col];
      }

      // Don't check the pkey
      if ($col == $pkey) {
        continue;
      }

      // If the field requires a value but doesn't have one then it may be
      // a problem.
      if ($info['not null'] == TRUE and (!isset($col_val) or ($col_val == ''))) {
        // If the column  has a default value then it's not a problem.
        if (array_key_exists('default', $info)) {
          continue;
        }
        $missing[] = $col;
      }
    }

    if (count($missing) > 0) {
      // Documentation for how to create a violation is here
      // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
      $message = 'The item cannot be saved because the following values are missing. ';
      $params = [];
      foreach ($missing as $col) {
        $message .=  ucfirst($col) . ", ";
      }
      $message = substr($message, 0, -2) . '.';
      $this->violations[] = new ConstraintViolation(t($message, $params)->render(),
          $message, $params, '', NULL, '', 1, 0, NULL, '');
    }
  }


  /**
   * Inserts all records for a single Chado table.
   *
   * @param string $table_alias
   * @throws \Exception
   */
  public function insertTable($table_alias) {

    // Get informatino about this Chado table.
    $chado_table = $this->records[$table_alias]['chado_table'];
    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    // Iterate through each item of the table and perform an insert.
    foreach ($this->records[$table_alias]['items'] as $delta => $record) {

      // Don't insert any records if any of the columns have field that
      // are marked as "delete if empty".
      if (array_key_exists('delete_if_empty', $record)) {
        $skip_record = FALSE;
        foreach ($record['delete_if_empty'] as $del_record) {
          if ($record['fields'][$del_record['chado_column']] == $del_record['empty_value']) {
            $skip_record = TRUE;
          }
        }
        if ($skip_record) {
          continue;
        }
      }

      // Replace fields that link to base tables with values.
      if (!array_key_exists($table_alias, $this->base_record_ids)) {
        foreach ($record['fields'] as $column => $val) {
          if (is_array($val) and $val[0] == 'REPLACE_BASE_RECORD_ID') {
            $base_table_alias = $val[1];
            $record_id = $this->base_record_ids[$base_table_alias];
            $this->records[$base_table_alias][$delta]['fields'][$column] = $record_id;
            $record['fields'][$column] = $record_id;
          }
        }
      }

      // Insert the record.
      $insert = $this->connection->insert('1:' . $chado_table);
      $insert->fields($record['fields']);
      $this->field_debugger->reportQuery($insert, "Insert Query for $chado_table ($delta)");
      $record_id = $insert->execute();
      if (!$record_id) {
        throw new \Exception(t('Failed to insert a record in the Chado "@table" table. Alias: @alias, Record: @record',
            ['@alias' => $table_alias, '@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Update the field with the record id.
      $elements = [
        'chado_table' => $chado_table,
        'table_alias' => $table_alias,
        'delta' => $delta,
        'chado_column' => $pkey,
        'column_alias' => $pkey,
        'value' => $record_id
      ];
      $this->setField($elements);

      // If this is the base table record_id then store it for later use.
      if (array_key_exists($table_alias, $this->base_record_ids)) {
        $this->base_record_ids[$table_alias] = $record_id;
      }
    }
  }

  /**
   * Queries for multiple records in Chado for a given table..
   *
   * @param string $table_alias
   *
   * @throws \Exception
   */
  public function findTable($table_alias) {

    $found_records = [];

    // Get informatino about this Chado table.
    $chado_table = $this->records[$table_alias]['chado_table'];

    // Iterate through each item of the table and perform an insert.
    foreach ($this->records[$table_alias]['items'] as $delta => $record) {

      // Select the fields in the chado table.
      $select = $this->connection->select('1:' . $chado_table, $table_alias);
      $select->fields($table_alias, array_keys($record['fields']));

      // Add in any joins.
      if (array_key_exists('joins', $record)) {
        $j_index = 0;
        foreach ($record['joins'] as $join_path => $join_info) {
          $right_table = $join_info['on']['right_table'];
          $right_alias = $join_info['on']['right_alias'];
          $right_colmn = $join_info['on']['right_column'];
          $left_alias = $join_info['on']['left_alias'];
          $left_column = $join_info['on']['left_column'];

          $select->leftJoin('1:' . $right_table, $right_alias, $left_alias . '.' .  $left_column . '=' .  $right_alias . '.' . $right_colmn);

          foreach ($join_info['columns'] as $column) {
            $sel_col = $column[0];
            $sel_col_as = $ralias . '_' . $column[1];
            $field_name = $column[2];
            $property_key = $column[3];
            $this->join_column_alias[$field_name][$property_key][$column[1]] = $sel_col_as;
            $select->addField($right_alias, $sel_col, $sel_col_as);
          }
          $j_index++;
        }
      }

      // Add the select condition
      foreach ($record['conditions'] as $chado_column => $value) {
        // If we don't have a primary key for the base table then skip the condition.
        if (is_array($value['value']) and in_array('REPLACE_BASE_RECORD_ID', array_values($value['value']))) {
          continue;
        }
        if (!empty($value)) {
          $select->condition($chado_table_alias . '.' . $chado_column, $value['value'], $value['operation']);
        }
      }

      $this->field_debugger->reportQuery($select, "Select Query for $chado_table ($delta)");
      // @debug print "Query in findChadoRecord(): " . strtr((string) $select, $select->arguments());

      // Execute the query.
      $results = $select->execute();
      if (!$results) {
        throw new \Exception(t('Failed to select record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      while ($match = $results->fetchAssoc()) {
        // @debug print "\t\tWorking on Query Record: " . print_r($match, TRUE);

        // We start by cloning the records array
        // (includes all tables, not just the current $base_table)
        $new_record = new ChadoRecords($this->field_debugger, $this->values);
        $new_record->setRecords($this->records);

        // Update the values in the new record.
        $elements = [
          'table_alias' => $table_alias,
          'delta' => $delta,
          'values' => $match
        ];
        $new_record->setFieldValues($elements);

        // Save the new record object. to be returned later.
        $found_records[] = $new_record;
      }
    }
    return $found_records;
  }

  /**
   * Updates all records for a single Chado table.
   *
   * @throws \Exception
   */
  private function updateTable($table_alias) {

    $chado_table = $this->records[$table_alias]['chado_table'];


    // Iterate through each item of the table and perform an insert.
    foreach ($this->records[$table_alias]['items'] as $delta => $record) {

      // If this is the base table then do an update.
      if (!array_key_exists('conditions', $record)) {
        throw new \Exception(t('Cannot update record in the Chado "@table" table due to missing conditions. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Don't update if we don't have any conditions set.
      if (!$this->hasValidConditions($record)) {
        throw new \Exception(t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Skip records that are empty..
      if ($this->isEmptyRecord($record)) {
        continue;
      }



      $update = $this->connection->update('1:'. $chado_table);
      $update->fields($record['fields']);
      foreach ($record['conditions'] as $chado_column => $cond_value) {
        $update->condition($chado_column, $cond_value['value']);
      }

      $this->field_debugger->reportQuery($update, "Update Query for $chado_table ($delta). Note: arguments may only include the conditional ones, see Drupal Issue #2005626.");

      $rows_affected = $update->execute();
      if ($rows_affected == 0) {
        throw new \Exception(t('Failed to update record in the Chado "@table" table. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }
      if ($rows_affected > 1) {
        throw new \Exception(t('Incorrectly tried to update multiple records in the Chado "@table" table. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }
    }
  }

  /**
   * Deletes record for a given table.
   *
   * @param string $table_alias
   * @throws \Exception
   */
  private function deleteChadoRecord($table_alias) {

    $chado_table = $this->records[$table_alias]['chado_table'];


    // Iterate through each item of the table and perform an insert.
    foreach ($this->records[$table_alias]['items'] as $delta => $record) {

      $schema = $this->connection->schema();
      $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
      $pkey = $table_def['primary key'];

      // Don't delete if we don't have any conditions set.
      if (!$this->hasValidConditions($record)) {
        throw new \Exception($this->t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      $delete = $this->connection->delete('1:'. $chado_table);
      foreach ($record['conditions'] as $chado_column => $cond_value) {
        $delete->condition($chado_column, $cond_value['value']);
      }

      $this->field_debugger->reportQuery($delete, "Delete Query for $chado_table ($delta)");

      $rows_affected = $delete->execute();
      if ($rows_affected == 0) {
        // @debug print "\n" . strtr((string) $delete, $delete->arguments()) . "\n";
        throw new \Exception($this->t('Failed to delete a record in the Chado "@table" table. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }
      if ($rows_affected > 1) {
        throw new \Exception($this->t('Incorrectly tried to delete multiple records in the Chado "@table" table. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Unset the record Id for this deleted record.
      $this->setValue($table_alias, $delta, $pkey, 0);
    }
  }

  /**
   * Selects a single record from Chado.
   *
   * @param array $records
   * @param string $chado_table_alias
   * @param integer $delta
   * @param array $record
   *
   * @throws \Exception
   */
  public function selectTable($table_alias) {

    $chado_table = $this->records[$table_alias]['chado_table'];

    // Iterate through each item of the table and perform an insert.
    foreach ($this->records[$table_alias]['items'] as $delta => $record) {

      if (!array_key_exists('conditions', $record)) {
        throw new \Exception(t('Cannot select record in the Chado "@table" table due to missing conditions. Record: @record',
            ['@table' => $table_alias, '@record' => print_r($record, TRUE)]));
      }

      // If we are selecting on the base table and we don't have a proper
      // condition then throw an error.
      if (!$this->hasValidConditions($record)) {
        throw new \Exception(t('Cannot select record in the Chado "@table" table due to unset conditions. Record: @record',
            ['@table' => $table_alias, '@record' => print_r($record, TRUE)]));
      }

      // Select the fields in the chado table.
      $select = $this->connection->select('1:' . $chado_table, $table_alias);
      if (array_key_exists('fields', $record)) {
        $select->fields($table_alias, array_keys($record['fields']));
      }

      // Add in any joins.
      if (array_key_exists('joins', $record)) {
        $j_index = 0;
        foreach ($record['joins'] as $join_path => $join_info) {
          $right_table = $join_info['on']['right_table'];
          $right_alias = $join_info['on']['right_alias'];
          $right_colmn = $join_info['on']['right_column'];
          $left_alias = $join_info['on']['left_alias'];
          $left_column = $join_info['on']['left_column'];

          $select->leftJoin('1:' . $right_table, $right_alias, $left_alias . '.' .  $left_column . '=' .  $right_alias . '.' . $right_colmn);

          foreach ($join_info['columns'] as $column) {
            $sel_col = $column[0];
            $sel_col_as = $right_alias . '_' . $column[1];
            $field_name = $column[2];
            $property_key = $column[3];
            $this->join_column_alias[$field_name][$property_key][ $column[1] ] = $sel_col_as;
            $select->addField($right_alias, $sel_col, $sel_col_as);
          }
          $j_index++;
        }
      }

      // Add the select condition
      foreach ($record['conditions'] as $chado_column => $value) {
        if (!empty($value['value'])) {
          $select->condition($table_alias . '.' . $chado_column, $value['value'], $value['operation']);
        }
      }

      $this->field_debugger->reportQuery($select, "Select Query for $chado_table ($delta)");

      // Execute the query.
      $results = $select->execute();
      if (!$results) {
        throw new \Exception(t('Failed to select record in the Chado "@table" table. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }
      $elements = [
        'table_alias' => $table_alias,
        'delta' => $delta,
        'values' => $results->fetchAssoc()
      ];
      $this->setFieldValues($elements);
    }
  }

  /**
   * Indicates if the record has any valid conditions.
   *
   * For the record to have valid conditions it must first have at least
   * one condition, and the value on which that condition relies is not empty.
   *
   * @param array $records
   * @return boolean
   */
  private function hasValidConditions($record) {
    $num_conditions = 0;
    foreach ($record['conditions'] as $chado_column => $cond_value) {
      if (!empty($cond_value['value'])) {
        $num_conditions++;
      }
    }
    if ($num_conditions == 0) {
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Indicates if we should keep this record for inserts/updates.
   *
   * @param array $record
   *
   * @return boolean
   */
  private function isEmptyRecord($record) {
    if (array_key_exists('delete_if_empty', $record)) {
      foreach ($record['delete_if_empty'] as $del_record) {
        if ($record['fields'][$del_record['chado_column']] == $del_record['empty_value']) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }
}
