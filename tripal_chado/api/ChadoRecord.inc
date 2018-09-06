<?php

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
            ['!pkey' => $this->pkey, '!record_id' => $record_id, '!table' => $this->table_name]);
          throw new Exception($message);
        }
        $this->record_id = $record_id;
        $this->values = $values;
      }
      catch (Exception $e) {
        $message = t('ChadoRecord::_construct(). Could not find a record in table, !table, with the given !pkey: !record_id. ERROR: !error',
          ['!pkey' => $this->pkey, '!record_id' => $record_id, '!table' => $this->table_name, '!error' => $e->getMessage()]);
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
    return $this->table;
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
      $message = t('ChadoRecord::save(). Could not save the record into the table, !table. '. 
        'Multiple records already exist that match the values: !values. '. 
        'Please provide a set of values that can uniquely identify a record.',
        ['!table' => $this->table_name, '!values' => print_r($this->values, TRUE), '!error' => $e->getMessage()]);
      throw new Exception($message);
    }
  }
  /**
   * Inserts the values of this object as a new record.
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
    try {
      chado_query($sql, $insert_args);
      // TODO: we can speed up inserts if we can find a way to not have to
      // run the find(), but get the newly inserted record_id directly
      // from the insert command.
      $this->find();
    }
    catch (Exception $e) {
      $message = t('ChadoRecord::insert(). Could not insert a record into the table, !table, with the following values: !values. ERROR: !error',
        ['!table' => $this->table_name, '!values' => print_r($this->values, TRUE), '!error' => $e->getMessage()]);
      throw new Exception($message);
    }
  }
  
  /**
   * Updates the values of this object as a new record.
   * 
   * @throws Exception
   */
  public function update() {
    
    // Make sure we have values for this record before updating.
    if (empty($this->values)) {
      $message = t('ChadoRecord::update(). Could not update a record into the table, !table, without any values.',
        ['!table' => $this->table_name]);
      throw new Exception($message);
    }
    
    // We have to have a record ID for the record to update.
    if (!$this->record_id) {
      $message = t('ChadoRecord::update(). Could not update a record in the table, !table, without a record ID.',
        ['!table' => $this->table_name]);
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
    }
    catch (Exception $e) {
      $message = t('ChadoRecord::update(). Could not update a record in the table, !table, with !record_id as the record ID and the following values: !values. ERROR: !error',
        ['!table' => $this->table_name, 
         '!record_id' => $this->record_id, 
         '!values' => print_r($this->values, TRUE), 
         '!error' => $e->getMessage()]);
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
    }
    catch (Exception $e) {
      $message = t('ChadoRecord::delete(). Could not delete a record in the table, !table, with !record_id as the record ID. ERROR: !error',
        ['!table' => $this->table_name,
         '!record_id' => $this->record_id,
         '!error' => $e->getMessage()]);
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
        $message = t('ChadoRecord::setValues(). The column named, "!column", does not exist in table: "!table". Values: !values".', 
          ['!column' => $column, '!table' => $this->table_name, '!values' => print_r($values, TRUE)]);
        throw new Exception($message);
      }
    }
  
    // Make sure that the user did not miss any required columns or has
    // set a column to be NULL when it doesn't allow NULLs.
    foreach ($this->required_cols as $rcol) {
      // It's okay if the primary key is missing, esepecially if the user
      // wants to use the find() or insert() functions.
      if ($rcol == $this->pkey) {
        continue;
      }
      
      if (in_array($rcol, array_keys($this->values)) and $this->values[$rcol] === '__NULL__') {
        $message = t('ChadoRecord::setValues(). The column named, "!column", requires a value for the table: "!table".',
          ['!column' => $rcol, '!table' => $this->table_name]);
        throw new Exception($message);
      }
    }
    
    // Check to see if the user provided the primary key (record_id).
    if (in_array($this->pkey, array_keys($values))) {
      $this->record_id = $values[$this->pkey];
    }
  }
  
  /**
   * Returns all values for the record.
   * 
   * @return array
   */
  public function getValues() {
    return $this->values;
  }
  
  /**
   * Sets the value for a specific column.
   * 
   * @param string $column_name
   *   The name of the column to which the value should be set.
   * @param $value
   *   The value to set.
   */
  public function setValue($column_name, $value) {
    
    // Make sure the column is valid.
    if (!in_array($column_name, $this->column_names)) {
      $message = t('ChadoRecord::setValue(). The column named, "!column", does not exist in table: "!table".',
        ['!column' => $column_name, '!table' => $this->table_name]);
      throw new Exception($message);
    }
    
    // Make sure that the value is not NULL if this is a required field.
    if (!in_array($column_name, $this->required_cols) and $value == '__NULL__') {
      $message = t('ChadoRecord::setValue(). The column named, "!column", requires a value for the table: "!table".',
        ['!column' => $column_name, '!table' => $this->table_name]);
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
      $message = t('ChadoRecord::getValue(). The column named, "!column", does not exist in table: "!table".',
        ['!column' => $column_name, '!table' => $this->table_name]);
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
   * @return
   *   The number of matches found.  If 1 is returned then the query
   *   successfully found a match. If 0 then no matching records were found.
   *   
   * @throws Exception
   */
  public function find() {
    
    // Make sure we have values for this record before searching.
    if (empty($this->values)) {
      $message = t('ChadoRecord::find(). Could not find a record from the table, !table, without any values.',
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
    }
    catch (Exception $e) {
      $message = t('ChadoRecord::find(). Could not find a record in the table, !table, with the following values: !values. ERROR: !error',
        ['!table' => $this->table_name, '!values' => print_r($this->values, TRUE), '!error' => $e->getMessage()]);
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
    }
    
    // Return the number of matches.
    return $num_matches;
  }
}