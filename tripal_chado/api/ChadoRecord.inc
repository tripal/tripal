<?php

/**
 * Provide a class for basic querying of Chado.
 *
 * Specifically tihs class provides select, insert, update and delete.
 *
 * Eventually this class is meants to replace the existing
 * chado_select_record(), chado_insert_record(), chado_update_record() and
 * chado_delete_record() API functions to create a cleaner, more maintainable
 * and more easily tested interface to querying Chado.
 *
 * @todo Add documentation for save() and delete().
 *
 * Basic Usage:
 * - Select/Find
 *     The following example selects an organism with the scientific name
 *     "Tripalus databasica" from the organism table of Chado.
 * @code
 * // First we create an instance of the ChadoRecord class
 * // specifying the table we want to query.
 * $record = new \ChadoRecord('organism');
 *
 * // Next we indicate the values we know.
 * $record->setValues([
 * 'genus' => 'Tripalus',
 * 'species' => 'databasica',
 * ]);
 *
 * // And finally we simply ask the class to find the chado record
 * // we indicated when we set the values above.
 * $success = $record->find();
 * if ($success) {
 * // Retrieve the values if we were successful in finding the record.
 * $result = $record->getValues();
 * }
 * @endcode
 * - Insert:
 *     The following example inserts a sample record into the stock table.
 * @code
 * // First we create an instance of the ChadoRecord class
 * // specifying the table we want to query.
 * $record = new \ChadoRecord('stock');
 *
 * // Next we indicate the values we know.
 * $record->setValues([
 * 'name' => 'My Favourite Plant',
 * 'uniquename' => 'Plectranthus scutellarioides Trailing Plum Brocade',
 * 'organism_id' => [ 'genus' => 'Tripalus', 'species' => 'databasica' ],
 * 'type_id' => [ 'name' => 'sample', 'cv_id' => [ 'name' => 'Sample processing
 *   and separation techniques' ] ],
 * ]);
 *
 * // And finally, we ask the class to insert the chado record
 * // we described when we set the values above.
 * $result = $record->insert();
 * @endcode
 * - Update:
 *     The following example updates the "Tripalus databasica" record to
 *   specify the common name.
 * @code
 * // For brevity we're going to hardcode the original record
 * // including the id although you would Never do this in practice.
 * // Rather you would first find the record as shown in a previous example.
 * $original_record = [
 * 'organism_id' => 1,
 * 'genus' => 'Tripalus',
 * 'species' => 'databasica',
 * ];
 *
 * // First we create an instance of the ChadoRecord class
 * // specifying the table we want to query.
 * // NOTICE: this time we set the record_id when creating the instance.
 * $record = new \ChadoRecord('organism', $original_record['organism_id']);
 *
 * // Now we set the values we want to change.
 * $record->setValues([
 * 'common_name' => 'Tripal',
 * ]);
 *
 * // And then tell the class to update the record.
 * $record->update();
 * @endcode
 */
class ChadoRecord {

  /**
   * @var string
   *   Holds the name of the table that this record belogns to.
   */
  protected $table_name = '';

  /**
   * @var array
   *   Holds the Drupal schema definition for this table.
   */
  protected $schema = [];

  /**
   * @var array
   *   Holds the values for the columns of the record
   */
  protected $values = [];

  /**
   * @var array
   *   An array of required columns.
   */
  protected $required_cols = [];

  /**
   * @var boolean
   *   An array of required columns which have yet to be set.
   */
  protected $missing_required_col = [];

  /**
   * @var integer
   *   The numeric Id for this record.
   */
  protected $record_id = NULL;

  /**
   * @var string
   *   The column name for the primary key.
   */
  protected $pkey = '';

  /**
   * The list of column names in the table.
   *
   * @var array
   */
  protected $column_names = [];


  /**
   * The ChadoRecord constructor
   *
   * @param string $table_name
   *   The name of the table that the record belongs to.
   *
   * @param string $record_id
   *  An optional record ID if this record is already present in Chado.
   */
  public function __construct($table_name, $record_id = NULL) {

    if (!$table_name) {
      $message = t('ChadoRecord::_construct(). The $table_name argument is required for a ChadoRecord instance.');
      throw new Exception($message);
    }

    // Set the table name and schema.
    $this->table_name = $table_name;
    $this->schema = chado_get_schema($this->table_name);
    if (!$this->schema) {
      $message = t('ChadoRecord::_construct(). Could not find a matching table schema in Chado for the table: !table.',
        ['!table' => $this->table_name]);
      throw new Exception($message);
    }

    // Chado tables never have more than one column as a primary key so
    // we are good just getting the first element.
    $this->pkey = $this->schema['primary key'][0];

    // Save the column names.
    foreach ($this->schema['fields'] as $column_name => $col_details) {
      $this->column_names[] = $column_name;
    }

    // Get the required columns.
    foreach ($this->schema['fields'] as $column => $col_schema) {
      foreach ($col_schema as $param => $val) {
        if (preg_match('/not null/i', $param) and $col_schema[$param]) {
          $this->required_cols[] = $column;
          // Currently all required columns are missing.
          $this->missing_required_col[$column] = TRUE;
        }
      }
    }

    // If a record_id was provided then lookup the record and set the values.
    if ($record_id) {
      try {
        $sql = 'SELECT * FROM {' . $this->table_name . '} WHERE ' . $this->pkey . ' = :record_id';
        $result = chado_query($sql, [':record_id' => $record_id]);
        $values = $result->fetchAssoc();
        if (empty($values)) {
          $message = t('ChadoRecord::_construct(). Could not find a record in table, !table, with the given !pkey: !record_id.',
            [
              '!pkey' => $this->pkey,
              '!record_id' => $record_id,
              '!table' => $this->table_name,
            ]);
          throw new Exception($message);
        }
        $this->record_id = $record_id;
        $this->values = $values;
        $this->missing_required_col = [];
      } catch (Exception $e) {
        $message = t('ChadoRecord::_construct(). Could not find a record in table, !table, with the given !pkey: !record_id. ERROR: !error',
          [
            '!pkey' => $this->pkey,
            '!record_id' => $record_id,
            '!table' => $this->table_name,
            '!error' => $e->getMessage(),
          ]);
        throw new Exception($message);
      }
    }
  }

  /**
   * Retrieves the record ID.
   *
   * @return number
   */
  public function getID() {
    return $this->record_id;
  }

  /**
   * Retrieves the table name.
   *
   * @return string
   *   The name of the table that the record belongs to.
   */
  public function getTable() {
    return $this->table_name;
  }

  /**
   * Retrieves the table schema.
   *
   * @return array
   *   The Drupal schema array for the table.
   */
  public function getSchema() {
    return $this->schema;
  }

  /**
   * Performs either an update or insert into the table using the values.
   *
   * If the record already exists it will be updated. If the record does not
   * exist it will be inserted.  This function adds a bit more overhead by
   * checking for the existence of the record and performing the appropriate
   * action. You can save time by using the insert or update functions directly
   * if you only need to do one of those actions specifically.
   *
   * @throws Exception
   */
  public function save() {

    // Determine if we need to perform an update or an insert.
    $num_matches = $this->find();
    if ($num_matches == 1) {
      $this->update();
    }
    if ($num_matches == 0) {
      $this->insert();
    }
    if ($num_matches > 1) {
      $message = t('ChadoRecord::save(). Could not save the record into the table, !table. ' .
        'Multiple records already exist that match the values: !values. ' .
        'Please provide a set of values that can uniquely identify a record.',
        [
          '!table' => $this->table_name,
          '!values' => print_r($this->values, TRUE),
          '!error' => $e->getMessage(),
        ]);
      throw new Exception($message);
    }
  }

  /**
   * Inserts the values of this object as a new record.
   *
   * @todo Support options from chado_insert_record: return_record.
   * @todo check for violation of unique constraint.
   *
   * @throws Exception
   */
  public function insert() {

    // Make sure we have values for this record before inserting.
    if (empty($this->values)) {
      $message = t('ChadoRecord::insert(). Could not insert a record into the table, !table, without any values.',
        ['!table' => $this->table_name]);
      throw new Exception($message);
    }

    // Additionally, make sure we have all the required values!
    if (!empty($this->missing_required_col)) {
      $message = t('ChadoRecord::insert(). The columns named, "!columns", ' .
        'require a value for the table: "!table". You can set these values ' .
        'using ChadoRecord::setValues(). Current values: !values.',
        [
          '!columns' => implode('", "', array_keys($this->missing_required_col)),
          '!table' => $this->table_name,
          '!values' => print_r($this->values, TRUE),
        ]);
      throw new Exception($message);
    }

    // Build the SQL statement for insertion.
    $insert_cols = [];
    $insert_vals = [];
    $insert_args = [];
    foreach ($this->values as $column => $value) {
      $insert_cols[] = $column;
      $insert_vals[] = ':' . $column;
      $insert_args[':' . $column] = $value;
    }
    $sql = 'INSERT INTO {' . $this->table_name . '} (' .
      implode(", ", $insert_cols) . ') VALUES (' .
      implode(", ", $insert_vals) . ')';
    if ($this->pkey) {
      $sql .= ' RETURNING ' . $this->pkey;
    }
    try {
      $result = chado_query($sql, $insert_args);
      if ($this->pkey) {
        $record_id = $result->fetchField();
        $this->values[$this->pkey] = $record_id;
        $this->record_id = $record_id;
      }
    } catch (Exception $e) {
      $message = t('ChadoRecord::insert(). Could not insert a record into the ' .
        'table, !table, with the following values: !values. ERROR: !error',
        [
          '!table' => $this->table_name,
          '!values' => print_r($this->values, TRUE),
          '!error' => $e->getMessage(),
        ]);
      throw new Exception($message);
    }
  }

  /**
   * Updates the values of this object as a new record.
   *
   * @todo set defaults for columns not already set in values.
   * @todo Support options from chado_update_record: return_record.
   * @todo check for violation of unique constraint.
   * @todo if record_id not set then try finding it.
   *
   * @throws Exception
   */
  public function update() {

    // Make sure we have values for this record before updating.
    if (empty($this->values)) {
      $message = t('ChadoRecord::update(). Could not update a record into the ' .
        'table, !table, without any values.',
        ['!table' => $this->table_name]);
      throw new Exception($message);
    }

    // Additionally, make sure we have all the required values!
    if (!empty($this->missing_required_col)) {
      $message = t('ChadoRecord::update(). The columns named, "!columns", ' .
        'require a value for the table: "!table". You can set these values ' .
        'using ChadoRecord::setValues(). Current values: !values.',
        [
          '!columns' => implode('", "', $this->missing_required_col),
          '!table' => $this->table_name,
          '!values' => print_r($this->values, TRUE),
        ]);
      throw new Exception($message);
    }

    // We have to have a record ID for the record to update.
    if (!$this->record_id) {
      $message = t('ChadoRecord::update(). Could not update a record in the ' .
        'table, !table, without a record ID. Current values: !values.',
        [
          '!table' => $this->table_name,
          '!values' => print_r($this->values, TRUE),
        ]);
      throw new Exception($message);
    }

    // Build the SQL statement for updating.
    $update_args = [];
    $sql = 'UPDATE {' . $this->table_name . '}  SET ';
    foreach ($this->values as $column => $value) {
      // We're not updating the primary key so skip that if it's in the values.
      if ($column == $this->pkey) {
        continue;
      }
      $sql .= $column . ' = :' . $column . ', ';
      $update_args[':' . $column] = $value;
    }
    // Remove the trailing comma and space.
    $sql = substr($sql, 0, -2);
    $sql .= ' WHERE ' . $this->pkey . ' = :record_id';
    $update_args[':record_id'] = $this->record_id;

    // Now try the update.
    try {
      chado_query($sql, $update_args);
    } catch (Exception $e) {
      $message = t('ChadoRecord::update(). Could not update a record in the ' .
        'table, !table, with !record_id as the record ID and the following ' .
        'values: !values. ERROR: !error',
        [
          '!table' => $this->table_name,
          '!record_id' => $this->record_id,
          '!values' => print_r($this->values, TRUE),
          '!error' => $e->getMessage(),
        ]);
      throw new Exception($message);
    }
  }

  /**
   * Deletes the record that matches the given values.
   *
   * A record ID must be part of the current values.
   *
   * @throws Exception
   */
  public function delete() {

    // We have to have a record ID for the record to be deleted.
    if (!$this->record_id) {
      $message = t('ChadoRecord::delete(). Could not delete a record in the table, !table, without a record ID.',
        ['!table' => $this->table_name]);
      throw new Exception($message);
    }

    try {
      $sql = 'DELETE FROM {' . $this->table_name . '} WHERE ' . $this->pkey . ' = :record_id';
      chado_query($sql, [':record_id' => $this->record_id]);
    } catch (Exception $e) {
      $message = t('ChadoRecord::delete(). Could not delete a record in the table, !table, with !record_id as the record ID. ERROR: !error',
        [
          '!table' => $this->table_name,
          '!record_id' => $this->record_id,
          '!error' => $e->getMessage(),
        ]);
      throw new Exception($message);
    }
  }

  /**
   * A general-purpose setter function to set the column values for the record.
   *
   * This function should be used prior to insert or update of a record. For
   * an update, be sure to include the record ID in the list of values passed
   * to the function.
   *
   * @todo Support options from chado_insert_record: skip_validation.
   * @todo Validate the types match what is expected based on the schema.
   * @todo Set default values for columns not in this array?
   * @todo Support foreign key relationships: lookup the key.
   * @todo Support value = [a, b, c] for IN select statements?
   *
   * @param array $values
   *    An associative array where the keys are the table column names and
   *    the values are the record values for each column.
   *
   * @throws Exception
   */
  public function setValues($values) {

    // Intiailze the values array.
    $this->values = [];

    // Add the values provided into the values property.
    foreach ($values as $column => $value) {
      if (in_array($column, $this->column_names)) {
        $this->values[$column] = $value;
      }
      else {
        $message = t('ChadoRecord::setValues(). The column named, "!column", ' .
          'does not exist in table: "!table". Values: !values".',
          [
            '!column' => $column,
            '!table' => $this->table_name,
            '!values' => print_r($values, TRUE),
          ]);
        throw new Exception($message);
      }
    }

    // Check whether all required columns are set and indicate using the
    // $required_values_set flag for faster checking in insert/update.
    $this->missing_required_col = [];
    foreach ($this->required_cols as $rcol) {
      // It's okay if the primary key is missing, esepecially if the user
      // wants to use the find() or insert() functions.
      if ($rcol == $this->pkey) {
        continue;
      }

      if (in_array($rcol, array_keys($this->values)) and $this->values[$rcol] === '__NULL__') {
        $this->missing_required_col[$rcol] = TRUE;
      }
    }

    // Check to see if the user provided the primary key (record_id).
    if (in_array($this->pkey, array_keys($values))) {
      $this->record_id = $values[$this->pkey];
    }

    // Ensure that no values are arrays.
    foreach ($values as $column => $value) {
      if (is_array($value)) {
        $message = t('ChadoRecord::setValues(). The column named, "!column", ' .
          'must be a single value but is currently: "!values". NOTE: this function ' .
          'currently does not support expanding foreign key relationships or ' .
          'multiple values for a given column.',
          [
            '!column' => $column,
            '!table' => $this->table_name,
            '!values' => implode('", "', $value),
          ]);
        throw new Exception($message);
      }
    }
  }

  /**
   * Returns all values for the record.
   *
   * @todo We need to follow foreign key constraints.
   *
   * @return array
   */
  public function getValues() {
    return $this->values;
  }

  /**
   * Sets the value for a specific column.
   *
   * @todo Support options from chado_insert_record: skip_validation.
   * @todo Validate the types match what is expected based on the schema.
   * @todo Set default values for columns not in this array?
   * @todo Support foreign key relationships: lookup the key.
   * @todo Support value = [a, b, c] for IN select statements?
   *
   * @param string $column_name
   *   The name of the column to which the value should be set.
   * @param $value
   *   The value to set.
   */
  public function setValue($column_name, $value) {

    // Make sure the column is valid.
    if (!in_array($column_name, $this->column_names)) {
      $message = t('ChadoRecord::setValue(). The column named, "!column", does ' .
        'not exist in table: "!table".',
        [
          '!column' => $column_name,
          '!table' => $this->table_name,
        ]);
      throw new Exception($message);
    }

    // Make sure that the value is not NULL if this is a required field.
    if (!in_array($column_name, $this->required_cols) and $value == '__NULL__') {
      $message = t('ChadoRecord::setValue(). The column named, "!column", ' .
        'requires a value for the table: "!table".',
        [
          '!column' => $column_name,
          '!table' => $this->table_name,
        ]);
      throw new Exception($message);
    }

    // Remove from the missing list if it was there.
    if (isset($this->missing_required_col[$column_name])) {
      unset($this->missing_required_col[$column_name]);
    }

    // Ensure that no values are arrays.
    if (is_array($value)) {
      $message = t('ChadoRecord::setValue(). The column named, "!column", ' .
        'must be a single value but is currently: "!values". NOTE: this function ' .
        'currently does not support expanding foreign key relationships or ' .
        'multiple values for a given column.',
        [
          '!column' => $column,
          '!table' => $this->table_name,
          '!values' => implode('", "', $value),
        ]);
      throw new Exception($message);
    }

    $this->values[$column_name] = $value;
  }

  /**
   * Returns the value of a specific column.
   *
   * @param string $column_name
   *   The name of a column from the table from which to retrieve the value.
   */
  public function getValue($column_name) {

    // Make sure the column is valid.
    if (!in_array($column_name, $this->column_names)) {
      $message = t('ChadoRecord::getValue(). The column named, "!column", ' .
        'does not exist in table: "!table".',
        [
          '!column' => $column_name,
          '!table' => $this->table_name,
        ]);
      throw new Exception($message);
    }

    return $this->values[$column_name];
  }

  /**
   * Uses the current values given to this object to find a record.
   *
   * Use the setValues function first to set values for searching, then call
   * this function to find matching record.  The values provided to the
   * setValues function must uniquely identify a record.
   *
   * @todo Support options from chado_select_record: skip_validation,
   *   has_record, return_sql, case_insensitive_columns, regex_columns,
   *   order_by, is_duplicate, pager, limit, offset.
   * @todo Support following the foreign key
   * @todo Support complex filtering (e.g. fmin > 50)
   * @todo Support multiple records being returned?
   *
   * @return
   *   The number of matches found.  If 1 is returned then the query
   *   successfully found a match. If 0 then no matching records were found.
   *
   * @throws Exception
   */
  public function find() {

    // Make sure we have values for this record before searching.
    if (empty($this->values)) {
      $message = t('ChadoRecord::find(). Could not find a record from ' .
        'the table, !table, without any values.',
        ['!table' => $this->table_name]);
      throw new Exception($message);
    }

    // Build the SQL statement for searching.
    $select_args = [];
    $sql = 'SELECT * FROM {' . $this->table_name . '} WHERE 1=1 ';
    foreach ($this->values as $column => $value) {
      $sql .= ' AND ' . $column . ' = :' . $column;
      $select_args[':' . $column] = $value;
    }
    try {
      $results = chado_query($sql, $select_args);
    } catch (Exception $e) {
      $message = t('ChadoRecord::find(). Could not find a record in the ' .
        'table, !table, with the following values: !values. ERROR: !error',
        [
          '!table' => $this->table_name,
          '!values' => print_r($this->values, TRUE),
          '!error' => $e->getMessage(),
        ]);
      throw new Exception($message);
    }

    // If we only have a single match then we're good and we can update the
    // values for this object.
    $num_matches = $results->rowCount();
    if ($num_matches == 1) {
      $record = $results->fetchAssoc();
      $this->values = [];
      foreach ($record as $column => $value) {
        $this->values[$column] = $value;
      }
      $this->record_id = $record[$this->pkey];

      // We are no longer missing any required columns because we loaded
      // from the database record.
      $this->missing_required_col = [];
    }

    // Return the number of matches.
    return $num_matches;
  }
}
