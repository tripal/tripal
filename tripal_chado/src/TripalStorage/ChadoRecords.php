<?php

namespace Drupal\tripal_chado\TripalStorage;

use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\Services\ChadoFieldDebugger;
use Symfony\Component\Validator\ConstraintViolation;
use Symfony\Component\Validator\Constraints\Isbn;
use Drupal\Core\Ajax\BeforeCommand;
use Symfony\Component\DependencyInjection\Attribute\AsAlias;

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
  protected array $records = [];

  /**
   * Holds the violations during validatin.
   *
   * @var array
   */
  protected array $violations = [];


  /**
   * A service to provide debugging for fields to developers.
   *
   * @var \Drupal\tripal_chado\Services\ChadoFieldDebugger
   */
  protected ChadoFieldDebugger $field_debugger;


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
  protected array $values;

  /**
   * The database connection for querying Chado.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $connection;


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
   * A helper function used to check incoming $elements for various functions.
   *
   * @param array $elements
   *   The array of elements to check
   * @param string $key
   *   The array key to check
   * @param string $method
   *   The method being formed (e.g.. Initalzing, Adding, Setting, etc.)
   * @param string $what
   *   The type of element being added (e.g., 'field', 'condition', etc.)
   * @throws \Exception
   */
  protected function checkElement($elements, $key, $method, $what) {
    if (!array_key_exists($key, $elements)) {
      throw new \Exception(t('@method a ChadoRecord @what without a "@key" element: @elements',
          ['@method' => $method, '@what' => $what, '@key' => $key, '@elements' => print_r($elements, TRUE)]));
    }
  }

  /**
   * Initalies the records
   *
   * @param array $elements
   *   An array of items used to initalize the internal records array.
   *
   * @throws \Exception
   */
  protected function initTable($elements) {

    // Make sure all of the required elements are preesent
    $this->checkElement($elements, 'base_table', 'Initializing', 'table');
    $this->checkElement($elements, 'chado_table', 'Initializing', 'table');
    $this->checkElement($elements, 'table_alias', 'Initializing', 'table');
    $this->checkElement($elements, 'delta', 'Initializing', 'table');

    // Get the items needed to initalize a table.
    $base_table = $elements['base_table'];
    $chado_table = $elements['chado_table'];
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];

    // If this base table has not been yet added to the records array then
    // add it.  The first level holds all of the bsae tables for all records
    // needed by fields.  It also holds the record ID if known.
    if (!array_key_exists($base_table, $this->records)) {
      $this->records[$base_table] = [
        'tables' => [],
        'record_id' => 0,
      ];
    }

    // if the table has not been initialized then do so.  The tables
    // are indexed using their alias. The top-level keys are the true
    // table name and it's list of items.
    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      $this->records[$base_table]['tables'][$table_alias] = [
        'chado_table' => $chado_table,
        'items' => [],
      ];
    }

    // Each table can have multiple items all indexed using $delta. If we
    // haven't seen the delta yet then initalize it. The delta should include
    // the list of fields (or column values from the SQL query, any conditions
    // that should be included for a selct/update/delete, any joins that are
    // required to get the field values, instructions to delete the item if
    // it's empty and a mapping array for column aliases.
    if (!array_key_exists($delta, $this->records[$base_table]['tables'][$table_alias]['items'])) {
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta] = [
        // columns for this table.
        'columns' => [],
        // Conditinos for this table when performing a query.
        'conditions' => [],
        // Joins that should be made with this table.
        'joins' => [],
        // Helps indicate if a record should be removed if it's empty.
        // this only applies to recrods in ancillary tables.
        'delete_if_empty' => [],
        // Indicates the list of columns that store the base table record_id.
        'link_columns' => [],
        // Aliases for columns.
        'column_aliases' => [],
        // The values. It will combine all of the columns from the table, and
        // any columns from joined tables.  This contains a key/value pair
        // for each value.
        'values' => []
      ];
    }
  }

  /**
   * Adds a field to this ChadoRecords object.
   *
   * A field here corresponds to a column in a Chado table.
   *
   * @param array $elements
   *   The list of key/value pairs describing the element.
   *
   *   These keys are required:
   *   - base_table: the base table the field should be added to.
   *   - chado_table: the chado table the field should be added to. This
   *     can be the base table or an anciallary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the detla index of the field item being added.
   *   - chado_column: the name of the column that the field is for.
   *   - column_alias: an alias for the column.
   *   - value: a value for the column.  If the value is not known this should
   *     be NULL.
   *
   *   These keys are optional:
   *   - delete_if_empty: for updates, if the "value" is empty then delete the
   *     item.
   *   - empty_value: only used if "delete_if_empty" is used.  It indicates the
   *     value to use to determine if the field is empty.
   *
   * @param bool $is_link
   *   Indicates if this field stores a link (or foreign key) to the base
   *   table. If TRUE, and if the "value" key is NULL then a placeholder will
   *   be used to fill in the record.  The value will be set automatically
   *   once it's known. Defaults to FALSE.
   *
   * @throws \Exception
   *   If the any required fields are missing an error is thrown.
   */
  public function addField(array $elements, bool $is_link = FALSE) {

    // Make sure the table is initalized.
    $this->initTable($elements);

    // Make sure all of the required elements are preesent.
    $this->checkElement($elements, 'chado_column', 'Setting', 'field');
    $this->checkElement($elements, 'column_alias', 'Setting', 'field');
    $this->checkElement($elements, 'value', 'Setting', 'field');

    // Get the elements needed to add a field.
    $base_table = $elements['base_table'];
    $chado_table = $elements['chado_table'];
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];
    $chado_column = $elements['chado_column'];
    $column_alias = $elements['column_alias'];
    $value = $elements['value'];

    // Add the field.
    if (!in_array($column_alias, $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['columns'])) {
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['columns'][] = $column_alias;
    }
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] = $value;
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases'][$column_alias] = [
      'chado_table' => $chado_table,
      'table_alias' => $table_alias,
      'chado_column' => $chado_column
    ];

    // Add the optional delete_if_empty.
    if (array_key_exists('delete_if_empty', $elements) and $elements['delete_if_empty'] === TRUE) {
      $this->checkElement($elements, 'empty_value', 'Adding', 'field');
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['delete_if_empty'][] = [
        'chado_column' => $chado_column,
        'empty_value' => $elements['empty_value']
      ];
    }

    // If this field is for an ID and it has no value then set a marker
    // so we can replace it later.
    if ($is_link) {
      $this->checkElement($elements, 'base_table', 'Setting', 'field');
      $base_table = $elements['base_table'];
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['link_columns'][$column_alias] = $base_table;
      if ($value === NULL) {
        $value = ['REPLACE_RECORD_ID', $base_table];
      }
      else if ($base_table == $table_alias) {
        $this->setRecordID($base_table, $value);
      }
    }
  }


  /**
   * Adds a condition to this ChadoRecords object.
   *
   * A condition is used when querying to limit the set of records returned.
   *
   * @param array $elements
   *   The list of key/value pairs describing the element.
   *
   *   These keys are required:
   *   - base_table: the base table the field should be added to.
   *   - chado_table: the chado table the field should be added to. This
   *     can be the base table or an anciallary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the detla index of the field item being added.
   *   - column_alias: the alias used for the column (set via the setField()
   *     function.
   *   - value: a value for the column to use as the condition.
   *
   * @throws \Exception
   *   If the any required fields are missing an error is thrown.
   */
  public function addCondition(array $elements) {

    // Make sure the table is initalized.
    $this->initTable($elements);

    // Make sure all of the required elements are preesent
    $this->checkElement($elements, 'column_alias', 'Setting', 'condition');
    $this->checkElement($elements, 'value', 'Setting', 'condition');


    // Get the elements needed to add a condition.
    $base_table = $elements['base_table'];
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];
    $column_alias = $elements['column_alias'];
    $value = $elements['value'];
    $operation = array_key_exists('operation', $elements) ? $elements['operation'] : '=';

    // Add the condition.
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['conditions'][$column_alias] = ['value' => $value, 'operation' => $operation];
  }

  /**
   * Sets the value for a condition that has been added.
   *
   * A condition is used when querying to limit the set of records returned.
   * A condition sould not be added if the field for the same foe;d has not
   * been added first.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.
   * @param int $delta
   *   The numeric index of the item.
   * @param string $column_alias
   *   The alias for the column.
   * @param mixed $value
   *   The value to set for the condition
   *
   * @throws \Exception
   *   If the item has not yet been added for the base table, table alias and
   *   delta then an exception is thrown.
   */
  public function setConditionValue(string $base_table, string $table_alias, int $delta, $column_alias, $value) {

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::setConditionValue(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }
    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      throw new \Exception(t('ChadoRecords::setConditionValue(): table_alias, "@alias", does not exist in the records array: @record',
          ['@alias' => $table_alias, '@delta' => $delta, '@record' => print_r($this->records, TRUE)]));
    }
    if (!array_key_exists($delta, $this->records[$base_table]['tables'][$table_alias]['items'])) {
      throw new \Exception(t('ChadoRecords::setConditionValue(): delta, "@delta", for table_alias, "@alias", does not exist in the records array: @record',
          ['@alias' => $table_alias, '@delta' => $delta, '@record' => print_r($this->records, TRUE)]));
    }
    if (!array_key_exists($column_alias, $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['conditions'])) {
      throw new \Exception(t('ChadoRecords::setConditionValue(): column_alias, "@calias", for delta, "@delta", of table_alias, "@alias", does not exist in the records array: @record',
          ['@calias' => $column_alias, '@alias' => $table_alias, '@delta' => $delta, '@record' => print_r($this->records, TRUE)]));
    }
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['conditions'][$column_alias]['value'] = $value;
  }

  /**
   * Adds a join to this ChadoRecords object.
   *
   * @param array $elements
   *   The list of key/value pairs describing the element.
   *
   *   These keys are required:
   *   - base_table: the base table the field should be added to.
   *   - chado_table: the chado table the field should be added to. This
   *     can be the base table or an anciallary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the detla index of the field item being added.
   *   - join_path:  the path from the StoragePropertyType that indicates
   *     the sequences of tables joined together.
   *   - join_type: corresponds to 'inner', 'outer', etc. Currently, only
   *     'inner' is supported.
   *   - left_table: the left table in the join.
   *   - left_column: the left column in the join.
   *   - right_table: the right table in the join.
   *   - right_column: the right column in the join.
   *   - left_alias: the alias of the left column in the join.
   *   - right_alias: the alias of the right column in the join.
   *
   * @throws \Exception
   *   If the any required fields are missing an error is thrown.
   */
  public function addJoin(array $elements) {

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
    $base_table = $elements['base_table'];
    $table_alias = $elements['table_alias'];
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
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['joins'][$join_path]['on'] = [
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
   * Adds a fields to extract from a join in this ChadoRecords object.
   *
   * This function is used after an addJoin() function to indicate the
   * fileds (or table columns) that should be added to the fields retrieved
   * after a query.
   *
   * @param array $elements
   *   The list of key/value pairs describing the element.
   *
   *   These keys are required:
   *   - base_table: the base table the field should be added to.
   *   - chado_table: the chado table the field should be added to. This
   *     can be the base table or an anciallary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the detla index of the field item being added.
   *   - join_path:  the path from the StoragePropertyType that indicates
   *     the sequences of tables joined together.
   *   - chado_column: the column name in the table to add as a field.
   *   - column_alias: the alias of the column.
   *   - field_name: the name of the TripalFieldItemBase field. that
   *     requested the join.
   *   - key: The property key of the StoragePropertyType. that requested
   *     the join.
   *
   * @throws \Exception
   *   If the any required fields are missing an error is thrown.
   */
  public function addJoinColumn(array $elements) {

    // Make sure the table is initalized.
    $this->initTable($elements);

    // Make sure all of the required elements are preesent
    $this->checkElement($elements, 'join_path', 'Setting', 'join column');
    $this->checkElement($elements, 'chado_column', 'Setting', 'join column');
    $this->checkElement($elements, 'column_alias', 'Setting', 'join column');
    $this->checkElement($elements, 'field_name', 'Setting', 'join column');
    $this->checkElement($elements, 'property_key', 'Setting', 'join column');

    // Get the elements needed to add a join column.
    $base_table = $elements['base_table'];
    $chado_table = $elements['chado_table'];
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];
    $join_path = $elements['join_path'];
    $chado_column = $elements['chado_column'];
    $column_alias = $elements['column_alias'];
    $field_name = $elements['field_name'];
    $property_key = $elements['property_key'];

    // Add the join column.
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['joins'][$join_path]['columns'][] = [
      $chado_column,
      $column_alias,
      $field_name,
      $property_key
    ];

    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] = NULL;
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases'][$column_alias] = [
      'chado_table' => $chado_table,
      'table_alias' => $table_alias,
      'chado_column' => $chado_column
    ];
  }

  /**
   * Sets the record ID for all fields.
   *
   * Record IDs may not be known when ChadoRecords is setup. For example,
   * a field may be added that needs a link to a bse table, but it may not
   * yet be known, especially before an insert of the base record.  This
   * function should be run before a database operation like an insert, select,
   * update, or delete, this function can be used to populate IDs that
   * may have been set somewhere along the way for base tables.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   */
  public function setLinks(string $base_table) {

    $record_id = $this->getRecordID($base_table);

    // Iterate through the records and see if any fields link to this base ID
    // and if so, then update those.
    $tables = $this->getTables($base_table);
    foreach ($tables as $table_alias) {
      foreach ($this->records[$base_table]['tables'][$table_alias]['items'] as $delta => $record) {
        foreach (array_keys($record['values']) as $column_alias) {

          // if this column is an ID field and links to this base table then update the value.
          if (array_key_exists($column_alias, $record['link_columns'])) {
            $base_table =  $record['link_columns'][$column_alias];
            $record_id = $record_id;
            $this->setColumnValue($base_table, $table_alias, $delta, $column_alias, $record_id);

            // If a condition exists for this id set it as well.
            if (array_key_exists($column_alias, $record['conditions'])) {
              $this->setConditionValue($base_table, $table_alias, $delta, $column_alias, $record_id);
            }
          }
        }
      }
    }
  }

  /**
   * Sets the record ID for a given base table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param int $record_id
   *   The numeric record ID.
   *
   * @throws \Exception
   *   If the base table is unknown then an error is thrown.
   */
  protected function setRecordID(string $base_table, int $record_id) {

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::setRecordID(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }

    $this->records[$base_table]['record_id'] = $record_id;
  }

  /**
   * Gets the record ID for a given base table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   *
   * @return int
   *   A numeric record ID for the base table.  If the value is
   *   0 then the value has not been set.
   */
  public function getRecordID(string $base_table) : int {

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::getRecordID(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }

    return $this->records[$base_table]['record_id'];
  }

  /**
   * Indicates if the given base table has a record ID
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   *
   * @return bool
   *   TRUE if the record ID is set, otherwise FALSE
   */
  public function hasRecordID(string $base_table) : bool {
    $record_id = $this->getRecordID($base_table);
    if ($record_id > 0) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Returns the list of base tables.
   *
   * @return array
   */
  public function getBaseTables() {
    return array_keys($this->records);
  }


  /**
   * Gets the true Chado table name from an alias.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   *
   * @return string
   *   The Chado table name.
   */
  public function getTableFromAlias(string $base_table, string $table_alias) {

    $tables = $this->getTables($base_table);
    if (!in_array($table_alias, $tables)) {
      throw new \Exception(t('ChadoRecords::getTableFromAlias() Requesting a table for an alias that is not used: @alias. '
          . 'Current table aliases: @tables. Base table: @base_table',
          ['@base_table' => $base_table, '@alias' => $table_alias, '@records' => print_r($tables, TRUE)]));
    }
    return $this->records[$base_table]['tables'][$table_alias]['chado_table'];
  }


  /**
   * For the given base table, returns non base tables.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   *
   * @return array
   *   The list of tables linked to the base table but does
   *   not include the base table.
   */
  public function getAncillaryTables(string $base_table) {

    if (!array_key_exists($base_table, $this->records)) {
      return [];
    }

    $tables = $this->getTables($base_table);
    $non_base_tables = [];
    foreach ($tables as $table) {
      if ($table == $base_table) {
        continue;
      }
      $non_base_tables[] = $table;
    }

    return $non_base_tables;
  }

  /**
   * Returns the list of tables currently handled by this object.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   *
   * @return array
   *   The list of tables linked to the base table and including
   *   the base table.
   */
  public function getTables(string $base_table) {
    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::getTables(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }
    return array_keys($this->records[$base_table]['tables']);
  }

  /**
   * Gets an array of records (one per field item)
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   *
   * @return mixed
   *   The value of the field.
   */
  protected function getTableItems(string $base_table, string $table_alias) {
    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::getTableItems(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }
    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      throw new \Exception(t('ChadoRecords::getTableItems(): The table has not been added to the ChadoRecords object: @table_alias',
          ['@table_alias' => $table_alias]));
    }

    return $this->records[$base_table]['tables'][$table_alias]['items'];
  }


  /**
   * Retreives the Chado column for a given base table and table alis.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   * @param int $delta
   *   The numeric index of the item.
   * @param string $column_alias
   *   The alias for the column.
   *
   * @return string
   *   The name of the chado column
   */
  public function getFieldAliasColumn(string $base_table, string $table_alias, int $delta, string $column_alias) {

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::getFieldAliasColumn(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }

    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      return NULL;
    }
    if (!array_key_exists($delta, $this->records[$base_table]['tables'][$table_alias]['items'])) {
      return NULL;
    }
    if (!array_key_exists($column_alias, $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases'])) {
      return NULL;
    }
    return $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases'][$column_alias]['chado_column'];
  }

  /**
   * Retreives all of the column aliases for a given chado column.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   * @param int $delta
   *   The numeric index of the item.
   * @param string $column_alias
   *   The alias for the column.
   *
   * @return array
   *   An array containing all of the alias mappings for fields in the table whose
   *   column namthces the $chado_column provided.
   */
  public function getColumnFieldAliases(string $base_table, string $table_alias, int $delta, string $chado_column) {

    $aliases = [];

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::getColumnFieldAliases(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }

    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      return NULL;
    }
    if (!array_key_exists($delta, $this->records[$base_table]['tables'][$table_alias]['items'])) {
      return NULL;
    }

    $column_aliases = array_keys($this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases']);
    foreach ($column_aliases as $column_alias) {
      if ($chado_column === $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases'][$column_alias]['chado_column']) {
        $aliases[] = $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases'][$column_alias]['chado_column'];
      }
    }

    return $aliases;
  }

  /**
   * Sets a value for a field that has already been added.
   *
   * This is useful for after a query is run and the value needs to be set.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   * @param int $delta
   *   The numeric index of the item.
   * @param string $column_alias
   *   The alias for the column.
   * @param mixed $value
   *   The value t oset for the field.
   *
   * @throws \Exception
   *   If the base_table, table_alias or delta don't exist then an error is
   *   thrown.
   *
   * @return bool
   *   TRUE if the value was set, FALSE otherwise
   */
  protected function setColumnValue(string $base_table, string $table_alias, int $delta, string $column_alias, $value) : bool {

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::setColumnValue(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }
    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      throw new \Exception(t('ChadoRecords::setColumnValue(): table_alias, "@alias", does not exist in the records array: @record',
          ['@alias' => $table_alias, '@delta' => $delta, '@record' => print_r($this->records, TRUE)]));
    }
    if (!array_key_exists($delta, $this->records[$base_table]['tables'][$table_alias]['items'])) {
      throw new \Exception(t('ChadoRecords::setColumnValue(): delta, "@delta", for table_alias, "@alias", does not exist in the records array: @record',
          ['@alias' => $table_alias, '@delta' => $delta, '@record' => print_r($this->records, TRUE)]));
    }

    // Just skip columns that don't exist.  It shouldn't be an error.
    if (!array_key_exists($column_alias, $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'])) {
      return FALSE;
    }

    // Set the value.
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] = $value;
    return TRUE;
  }

  /**
   * Gets a value for a given field.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   * @param int $delta
   *   The numeric index of the item.
   * @param string $column_alias
   *   The alias for the column.
   *
   * @return mixed
   *   The value of the field.
   */
  public function getFieldValue(string $base_table, string $table_alias, int $delta, $column_alias) {

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::getFieldValue(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }
    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      return NULL;
    }
    if (!array_key_exists($delta, $this->records[$base_table]['tables'][$table_alias]['items'])) {
      return NULL;
    }
    if (!array_key_exists($column_alias, $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'])) {
      return NULL;
    }
    return $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias];
  }


  /**
   * Returns the records object as an array.
   *
   * @return array
   *   An array representation of this ChadoRecords object.
   */
  public function getRecordsArray() : array {
    return $this->records;
  }

  /**
   * Allows the caller to copy the records from another ChadoRecords object.
   *
   * @param ChadoRecords $records
   *
   *   The ChadoRecords object shose records should be copied.
   */
  public function copyRecords(ChadoRecords $records) {
    $this->records = $records->getRecordsArray();
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

    foreach ($this->records as $base_table => $details) {
      $record_id = $details['record_id'];

      // Make sure all IDs are up to date.
      $this->setIdFields($base_table);

      // We only need to validate the base table properties because
      // the linker table values get completely replaced on an update and
      // should not exist for an insert.
      foreach ($this->records[$base_table]['tables'][$base_table]['items'] as $delta => $record) {
        $this->validateRequired($base_table, $delta, $record_id, $record);
        $this->validateTypes($base_table, $delta, $record_id, $record);
        $this->validateSize($base_table, $delta, $record_id, $record);

        // Don't do the SQL checks if there are previous problems.
        if (count($this->violations) == 0) {
          $this->validateUnique($base_table, $delta, $record_id, $record);
          $this->validateFKs($base_table, $delta, $record_id, $record);
        }
      }
    }

    return $this->violations;
  }

  /**
   * Checks that foreign key fields exist in the record for the given table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param int $delta
   *   The numeric index of the item.
   * @param int $record_id
   *   The record ID for the base table.
   * @param array $record
   *   The field item to validate
   */
  protected function validateFKs($base_table, $delta, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);

    $bad_fks = [];
    if (!array_key_exists('foreign keys', $table_def)) {
      return;
    }
    $fkeys = $table_def['foreign keys'];
    foreach ($fkeys as $fk_table => $info) {
      foreach ($info['fields'] as $lcol => $rcol) {

        $lcol_aliases = $this->getColumnFieldAliases($base_table, $base_table, $delta, $lcol);

        // If the FK is not set in the record then skip it.
        if (!$lcol_aliases) {
          continue;
        }
        $lcol_alias = $lcol_aliases[0];

        // If an FK allows nulls and the value is null then skip this one.
        $col_val = $record['columns'][$lcol_alias];
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
      $message = 'The item cannot be saved because the following values have a missing '
       . 'linked record in the data store: ';

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
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param int $delta
   *   The numeric index of the item.
   * @param int $record_id
   *   The record ID for the base table.
   * @param array $record
   *   The field item to validate
   */
  protected function validateTypes($base_table, $delta, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);

    $bad_types = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['columns'])) {
        $col_val = $record['values'][$col];
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
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param int $delta
   *   The numeric index of the item.
   * @param int $record_id
   *   The record ID for the base table.
   * @param array $record
   *   The field item to validate
   */
  protected function validateSize($base_table, $delta, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);

    $bad_sizes = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['columns'])) {
        $col_val = $record['values'][$col];
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
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param int $delta
   *   The numeric index of the item.
   * @param int $record_id
   *   The record ID for the base table.
   * @param array $record
   *   The field item to validate
   */
  protected function validateUnique($base_table, $delta, $record_id,  $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);

    // Check if we are violating a unique constraint (if it's an insert)
    if (array_key_exists('unique keys',  $table_def)) {
      $pkey = $table_def['primary key'];

      // Iterate through the unique constraints and see if the record
      // violates it.
      $ukeys = $table_def['unique keys'];
      foreach ($ukeys as $ukey_name => $ukey_cols) {
        $ukey_cols = explode(',', $ukey_cols);
        $query = $this->connection->select('1:'. $base_table);
        $query->fields($base_table);
        foreach ($ukey_cols as $col) {
          $col = trim($col);
          $col_alias = $this->getColumnFieldAliases($base_table, $delta, $col)[0];
          $col_val = NULL;
          if ($col_alias) {
            $col_val = $record['values'][$col_alias];
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
              if (array_key_exists($col, $record['columns'])) {
                $col_val = $record['values'][$col];
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
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param int $delta
   *   The numeric index of the item.
   * @param int $record_id
   *   The record ID for the base table.
   * @param array $record
   *   The field item to validate
   */
  protected function validateRequired($base_table, $delta, $record_id, $record) {

    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($base_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    $missing = [];
    foreach ($table_def['fields'] as $col => $info) {
      $col_val = NULL;
      if (array_key_exists($col, $record['columns'])) {
        $col_val = $record['values'][$col];
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
   * A helper function for the insetTable() function.
   *
   * Checks to see if the record should not be inserted.
   *
   * @param array $record
   *   The record being considered for insertion.
   * @return bool
   *   Returns TRUE if the record should be skipped, FALSE otherwise.
   */
  protected function isSkipInsert(array $record) : bool {
    $skip_record = FALSE;

    // Don't insert any records if any of the columns have field that
    // are marked as "delete if empty".
    if (array_key_exists('delete_if_empty', $record)) {
      foreach ($record['delete_if_empty'] as $details) {
        if ($record['values'][$details['chado_column']] == $details['empty_value']) {
          $skip_record = TRUE;
        }
      }
    }
    return $skip_record;
  }


  /**
   * Inserts all records for a single Chado table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   *
   * @throws \Exception
   */
  public function insertRecords(string $base_table, string $table_alias) {

    // Make sure all IDs are up to date.
    $this->setLinks($base_table);

    // Get the Chado table for this given table alias.
    $chado_table = $this->getTableFromAlias($base_table, $table_alias);

    // Get informatino about this Chado table.
    $schema = $this->connection->schema();
    $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
    $pkey = $table_def['primary key'];

    // Iterate through each item of the table and perform an insert.
    $items = $this->getTableItems($base_table, $table_alias);
    foreach ($items as $delta => $record) {

      // If we need to skip this insert because it's empty then continue.
      if ($this->isSkipInsert($record)) {
        continue;
      }

      // Remove the pkey field as we need set it with an insert.
      $column_aliases = $this->getColumnFieldAliases($table_alias, $delta, $pkey);
      if (!$column_aliases){
        throw new \Exception(t('Failed to insert a record in the Chado "@table" because the primary key is missing as a field. Alias: @alias, Record: @record',
            ['@alias' => $table_alias, '@table' => $chado_table, '@record' => print_r($record, TRUE)]));

      }
      $pkey_alias = $column_aliases[0];

      // Build the Insert.
      $insert = $this->connection->insert('1:' . $chado_table);
      $values = [];
      foreach ($record['columns'] as $column_alias) {
        $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
        $values[$chado_column] = $record['values'][$column_alias];
      }
      $insert->fields($values);
      $this->field_debugger->reportQuery($insert, "Insert Query for $chado_table ($delta)");

      // Execute the insert.
      $record_id = $insert->execute();
      if (!$record_id) {
        throw new \Exception(t('Failed to insert a record in the Chado "@table" table. Alias: @alias, Record: @record',
            ['@alias' => $table_alias, '@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Update the field with the record id.
      $this->setColumnValue($table_alias, $delta, $pkey_alias, $record_id);
    }
  }

  /**
   * Queries for multiple records in Chado for a given table..
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   *
   * @throws \Exception
   */
  public function findRecords(string $base_table, string $table_alias) {

    $found_records = [];

    // Get informatino about this Chado table.
    $chado_table = $this->getTableFromAlias($table_alias);

    // Iterate through each item of the table and perform an insert.
    foreach ($this->records[$base_table]['tables'][$table_alias]['items'] as $delta => $record) {

      // Select the fields in the chado table.
      $select = $this->connection->select('1:' . $chado_table, $table_alias);
      $select->fields($table_alias, array_keys($record['columns']));

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
        if (is_array($value['value']) and in_array('REPLACE_RECORD_ID', array_values($value['value']))) {
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
        $new_record->setColumnValue($elements);

        // Save the new record object. to be returned later.
        $found_records[] = $new_record;
      }
    }
    return $found_records;
  }

  /**
   * Updates all records for a single Chado table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   *
   * @throws \Exception
   */
  public function updateRecords($base_table, $table_alias) {

    // Make sure all IDs are up to date.
    $this->setLinks($base_table);

    // Get the Chado table for this given table alias.
    $chado_table = $this->getTableFromAlias($base_table, $table_alias);

    // Iterate through each item of the table and perform an insert.
    $items = $this->getTableItems($base_table, $table_alias);
    foreach ($items as $delta => $record) {

      // Don't update if we don't have any conditions set.
      if (!$this->hasValidConditions($record)) {
        throw new \Exception(t('Cannot update record in the Chado "@table" table due to unset conditions. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Skip records that are empty.
      if ($this->isEmptyRecord($record)) {
        continue;
      }

      $update = $this->connection->update('1:'. $chado_table);
      $update->fields($record['columns']);
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
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   * @param bool $graceful
   *   Set to TRUE not not throw na exception of valid conditions are not
   *   set. If TRUE then it skips the record rather than performs the delete.
   *
   * @throws \Exception
   */
  public function deleteRecords(string $base_table, string $table_alias, bool $graceful = FALSE) {

    // Make sure all IDs are up to date.
    $this->setLinks($base_table);

    // Get the Chado table for this given table alias.
    $chado_table = $this->getTableFromAlias($base_table, $table_alias);

    // Iterate through each item of the table and perform an insert.
    $items = $this->getTableItems($base_table, $table_alias);
    foreach ($items as $delta => $record) {

      $schema = $this->connection->schema();
      $table_def = $schema->getTableDef($chado_table, ['format' => 'drupal']);
      $pkey = $table_def['primary key'];

      // Don't delete if we don't have any conditions set.
      if (!$this->hasValidConditions($record)) {
        if ($graceful) {
          continue;
        }
        throw new \Exception(t('Cannot delete record in the Chado "@table" table due to unset conditions. Record: @record',
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
        throw new \Exception(t('Failed to delete a record in the Chado "@table" table. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }
      if ($rows_affected > 1) {
        throw new \Exception(t('Incorrectly tried to delete multiple records in the Chado "@table" table. Record: @record',
            ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Unset the record Id for this deleted record.
      $this->setColumnValue($base_table, $table_alias, $delta, $pkey, 0);
    }
  }

  /**
   * Selects a single record from Chado.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   *
   * @throws \Exception
   */
  public function selectRecords($base_table, $table_alias) {

    // Make sure all IDs are up to date.
    $this->setLinks($base_table);

    // Get the Chado table for this given table alias.
    $chado_table = $this->getTableFromAlias($base_table, $table_alias);

    // Iterate through each item of the table and perform an insert.
    $items = $this->getTableItems($base_table, $table_alias);
    foreach ($items as $delta => $record) {

      if (!array_key_exists('conditions', $record)) {
        throw new \Exception(t('Cannot select record in the Chado "@table" table due to missing conditions. Record: @record',
            ['@table' => $table_alias, '@record' => print_r($record, TRUE)]));
      }

      // Make sure conditions are valid.
      if (!$this->hasValidConditions($record)) {
        throw new \Exception(t('Cannot select record in the Chado "@table" table due to unset conditions. Record: @record',
            ['@table' => $table_alias, '@record' => print_r($record, TRUE)]));
      }

      // Select the fields in the chado table.
      $select = $this->connection->select('1:' . $chado_table, $table_alias);
      foreach ($record['columns'] as $column_alias) {
        $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
        $select->addField($table_alias, $chado_column, $column_alias);
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
            $sel_col_as = $column[1];
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
      foreach ($results->fetchAssoc() as $column_alias => $value) {
        $this->setColumnValue($base_table, $table_alias, $delta, $column_alias, $value);
      }
    }
    dpm($this->records);
  }

  /**
   * Indicates if the record has any valid conditions.
   *
   * For the record to have valid conditions it must first have at least
   * one condition, and the value on which that condition relies is not empty.
   *
   * @param array $record
   *   The field item to validate
   *
   * @return bool
   *   Return TRUE if the conditions are valid. FALSE otherwise.
   */
  protected function hasValidConditions($record) : bool{

    $num_conditions = 0;
    foreach ($record['conditions'] as $details) {
      if (!empty($details['value'])) {
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
   *   The field item to validate
   *
   * @return bool
   *   Return TRUE if the record is empty. FALSE otherwise.
   */
  protected function isEmptyRecord($record) {
    if (array_key_exists('delete_if_empty', $record)) {
      foreach ($record['delete_if_empty'] as $del_record) {
        if ($record['values'][$del_record['chado_column']] == $del_record['empty_value']) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }
}