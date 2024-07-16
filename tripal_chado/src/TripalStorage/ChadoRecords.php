<?php

namespace Drupal\tripal_chado\TripalStorage;

use Drupal\tripal\Services\TripalLogger;
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
   * Holds the aliases for tables in joins. We keep track of aliases for joins
   * via their join paths, but converting a join path to an alias for each
   * table will be too long for SQL. Instead, we store the hashes here
   * for easy lookup when performing the join in SQL.
   *
   * @var array
   */
  protected array $join_aliases = [];


  /**
   * A service to provide debugging for fields to developers.
   *
   * @var \Drupal\tripal_chado\Services\ChadoFieldDebugger
   */
  protected ChadoFieldDebugger $field_debugger;


  /**
   * The database connection for querying Chado.
   *
   * @var \Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $connection;


  /**
   * The TripalLogger for logging messages.
   *
   * @var TripalLogger
   */
  protected TripalLogger $logger;



  /**
   * Constructor
   *
   * @param ChadoFieldDebugger $field_debugger
   */
  public function __construct(ChadoFieldDebugger $field_debugger, TripalLogger $logger, ChadoConnection $connection) {
    $this->field_debugger = $field_debugger;
    $this->logger = $logger;
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
      throw new \Exception(t('ChadoRecords::checkElement(). @method a ChadoRecord @what without a "@key" element: @elements',
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
   *
   * @return book:
   *   TRUE if the table and delta were initalized. FALSE otherwise. The
   *   base table can only be initialized once. This function will
   *   return FALSE if there is an attempt to initalize it with a delta
   *   higher than 1.
   */
  protected function initTable($elements) : bool {

    // Make sure all of the required elements are present
    $this->checkElement($elements, 'base_table', 'Initializing', 'table');
    $this->checkElement($elements, 'root_table', 'Initializing', 'table');
    $this->checkElement($elements, 'root_alias', 'Initializing', 'table');
    $this->checkElement($elements, 'chado_table', 'Initializing', 'table');
    $this->checkElement($elements, 'table_alias', 'Initializing', 'table');
    $this->checkElement($elements, 'delta', 'Initializing', 'table');

    // Get the items needed to initalize a table.
    $base_table = $elements['base_table'];
    $root_table = $elements['root_table'];
    $root_alias = $elements['root_alias'];
    $chado_table = $elements['chado_table'];
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];

    if ($base_table == $root_table) {
      if ($base_table != $root_alias) {
        throw new \Exception(t('ChadoRecords::initTable(). The base table cannot have an alias. '
          . 'Check all fields the contribute properties and make sure none of them use an alias '
          . 'for the root table in the "path" element. @elements',
          ['@elements' => print_r($elements, TRUE)]));
      }
    }

    // We do not want to initalize the base table more than once. When a
    // field has a cardinality > 1 then it can pass a delta value > 0. That
    // delta value is for the item not for the base table.  There can only
    // be one base table record.
    if ($base_table == $chado_table and $delta > 0)  {
      return FALSE;
    }

    // Use the table alias provided for joining.
    $this->join_aliases[$table_alias] = $table_alias;

    // If this base table has not been yet added to the records array then
    // add it.  The first level holds all of the bsae tables for all records
    // needed by fields.  It also holds the record ID if known.
    if (!array_key_exists($base_table, $this->records)) {
      $this->records[$base_table] = [
        'tables' => [],
        'record_id' => 0,
      ];
      // The base table doesn't have an alias so make sure we have an entry for it..
      $this->records[$base_table]['tables'][$base_table] = [
        'chado_table' => $base_table,
        'items' => [],
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
        // columns for this table. This is just a simple list of columns
        // for the base table that should be included in the record.
        'columns' => [],

        // An array mapping which Tripal fields want which Chado column values.
        // The key is the column alias and the value is an array, one entry
        // for each field/property that uses the value.
        'field_columns' => [],

        // Conditinos for this table when performing a query.
        'conditions' => [],

        // Joins that should be made with this table. The keys in this
        // array is the full join point from the root table. It will contain
        // two sub keys: 'on' (providing details about how to do the join) and
        // 'columns' with information about which columns from the join to
        // include in the final values set.
        'joins' => [],

        // Helps indicate if a record should be removed if it's empty.
        // this only applies to recrods in ancillary tables.
        'delete_if_empty' => [],

        // Indicates the list of columns that store the base table record_id.
        'link_columns' => [],

        // Aliases for columns. This is indexed by the column alias. The value
        // is a set of key value pairs indicating the chado_table, table_alias and
        // chado column names.
        'column_aliases' => [],

        // The values. It will combine all of the columns from the table, and
        // any columns from joined tables.  There is no guarnatee that fields
        // won't give the same name to the same fields in the same tables so
        // these values will be indexed by the field and key they belong to.
        'values' => [],

        // A boolean to indicate if any values have been set. We can't
        // rely on checking if all values are empty because it could be
        // possible that all values are meant to be empty. This value will
        // get set when a query is successful for the table and values have
        // been set.
        'has_values' => FALSE,
      ];
    }
    return TRUE;
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
   *     can be the base table or an ancillary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the delta index of the field item being added.
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
   * @param bool $read_only
   *   If the column requested has a read-only value then we want to make
   *   sure that the column is present in the query and we get results for it
   *   but any value that might be set coming in should be ignored.
   *
   * @throws \Exception
   *   If the any required fields are missing an error is thrown.
   */
  public function addColumn(array $elements, bool $is_link = FALSE, bool $read_only = FALSE) {

    // Initialize the table. If the function returns FALSE
    // then the caller is trying to re-initialize the base table so just quit.
    if(!$this->initTable($elements)){
      return;
    }

    // Make sure all of the required elements are present.
    $this->checkElement($elements, 'chado_column', 'Setting', 'field');
    $this->checkElement($elements, 'column_alias', 'Setting', 'field');
    $this->checkElement($elements, 'value', 'Setting', 'field');
    $this->checkElement($elements, 'field_name', 'Setting', 'field');
    $this->checkElement($elements, 'property_key', 'Setting', 'field');


    // Get the elements needed to add a field.
    $base_table = $elements['base_table'];
    $chado_table = $elements['chado_table'];
    $table_alias = $elements['table_alias'];
    $delta = $elements['delta'];
    $chado_column = $elements['chado_column'];
    $column_alias = $elements['column_alias'];
    $value = $elements['value'];
    $field_name = $elements['field_name'];
    $property_key = $elements['property_key'];

    // Add the field.
    if (!in_array($column_alias, $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['columns'])) {
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['columns'][] = $column_alias;
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] = NULL;
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['column_aliases'][$column_alias] = [
        'chado_table' => $chado_table,
        'table_alias' => $table_alias,
        'chado_column' => $chado_column
      ];
    }

    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['field_columns'][$column_alias][] = [
      'chado_column' => $chado_column,
      'column_alias' => $column_alias,
      'field_name' => $field_name,
      'property_key' => $property_key
    ];

    // If this is a read-only field then don't set the value. It will get set
    // after a load.
    if ($read_only) {
      return;
    }

    // If the value is set (i.e., not null) then don't reset it.
    if ($this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] !== NULL)  {
      return;
    }
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] = $value;


    // Add the optional delete_if_empty.
    if (array_key_exists('delete_if_empty', $elements) and $elements['delete_if_empty'] === TRUE) {
      $this->checkElement($elements, 'empty_value', 'Adding', 'field');
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['delete_if_empty'][] = [
        'chado_column' => $column_alias,
        'empty_value' => $elements['empty_value']
      ];
    }

    // If this field is for an ID then keep track of it. If this is the base
    // table then we can set the ID from any value provided.
    if ($is_link) {
      $this->checkElement($elements, 'base_table', 'Setting', 'field');
      $base_table = $elements['base_table'];
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['link_columns'][$column_alias] = $base_table;
      if ($value and $base_table == $table_alias) {
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
   *     can be the base table or an ancillary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the delta index of the field item being added.
   *   - column_alias: the alias used for the column (set via the setField()
   *     function.
   *   - value: a value for the column to use as the condition.
   *
   * @throws \Exception
   *   If the any required fields are missing an error is thrown.
   */
  public function addCondition(array $elements) {

    // Initialize the table. If the function returns FALSE
    // then the caller is trying to re-initialize the base table so just quit.
    if(!$this->initTable($elements)){
      return;
    }

    // Make sure all of the required elements are present
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
   *     can be the base table or an ancillary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the delta index of the field item being added.
   *   - join_path:  the path from the StoragePropertyType that indicates
   *     the sequences of tables joined together.
   *   - join_type: corresponds to 'inner', 'outer', etc. Currently, only
   *     'outer' is supported.
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
  public function addJoin(array &$elements) {

    // Initialize the table. If the function returns FALSE
    // then the caller is trying to re-initialize the base table so just quit.
    if (!$this->initTable($elements)){
      return;
    }

    // Make sure all of the required elements are present
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
    $chado_table = $elements['chado_table'];
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

    // Get the left and right aliases.
    [$left_alias, $right_alias] = $this->getJoinAliases($elements);

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

    // Add an empty columns array. It is possible that a property has a join
    // path that goes multiple tables deep and some of those interior joins
    // may not have any columns to be selected for the record.  In this case,
    // we need to have an empty columns array to prevent missing key errors.
    if (!array_key_exists('columns', $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['joins'][$join_path])) {
      $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['joins'][$join_path]['columns'] = [];
    }

    // Set the elements array
    $elements['left_alias'] = $left_alias;
    $elements['right_alias'] = $right_alias;
  }

  /**
   * Generates unique aliases for tables used in joins.
   *
   * This function will generate unique aliases if the callee did not
   * provide them.
   *
   * @param array $elements
   *   The array of elements passed to the addJoin() function
   * @return array
   *   An array of two strings: the left table alias and the right table alias.
   */
  protected function getJoinAliases(array $elements) : array {
    $base_table = $elements['base_table'];
    $right_table = $elements['right_table'];
    $left_table = $elements['left_table'];
    $join_path = $elements['join_path'];
    $left_alias = $elements['left_alias'];
    $right_alias = $elements['right_alias'];

    // So that we don't have conflicts when multiple joins use the same
    // tables we'll give the table an alias if the callee didn't give it one.
    // This alias needs to be short so that the SQL doesn't bork itself.
    // We know a property is referring to the same join if the join path
    // is the same, so we can assign a short random hash of letters to
    // each join path table/column to ensure the SQL won't break.
    // We only do this if the user didn't provide an alias.
    if ($right_alias == $right_table) {
      // The right path is everything up to the final column
      $right_path = preg_replace('/^(.+)\..+$/', '$1', $join_path);
      if (!array_key_exists($right_path, $this->join_aliases)) {
        $this->join_aliases[$right_path] = $this->generateJoinHash();
      }
      $right_alias = $this->join_aliases[$right_path];
    }
    if ($left_alias == $left_table and $left_table != $base_table) {
      // The left path is the table just before the right table.
      $left_path = preg_replace('/^(.+)\>.+$/', '$1', $join_path);
      $left_path = preg_replace('/^(.+)\..+$/', '$1', $left_path);
      $left_path = preg_replace('/^(.+)\;.+$/', '$1', $left_path);
      $left_path = preg_replace('/^(.+)\..+$/', '$1', $left_path);
      if (!array_key_exists($left_path, $this->join_aliases)) {
        $this->join_aliases[$left_path] = $this->generateJoinHash();
      }
      $left_alias = $this->join_aliases[$left_path];
    }

    return [$left_alias, $right_alias];
  }

  /**
   * Generates a random character string.
   *
   * @param int $length
   *   The length of the unique string.
   *
   * @return string
   */
  protected function generateJoinHash(int $length = 20) {

    $characters = 'abcdefghijklmnopqrstuvwxyz_';
    $characters_length = strlen($characters);
    $random_string = '';
    for ($i = 0; $i < $length; $i++) {
      $random_string .= $characters[random_int(0, $characters_length - 1)];
    }

    // If the string created already exists in the list (shouldn't happen)
    // then repeat until we get a unique one.
    while (array_key_exists($random_string, $this->join_aliases)) {
      $random_string = $this->generateJoinHash($length);
    }
    return $random_string;
  }

  /**
   * Adds a fields to extract from a join in this ChadoRecords object.
   *
   * This function is used after an addJoin() function to indicate the
   * fields (or table columns) that should be added to the fields retrieved
   * after a query.
   *
   * @param array $elements
   *   The list of key/value pairs describing the element.
   *
   *   These keys are required:
   *   - base_table: the base table the field should be added to.
   *   - chado_table: the chado table the field should be added to. This
   *     can be the base table or an ancillary table.
   *   - table_alias: the alias fo the table. A base table alias will always
   *     be the same as the as the base table name.
   *   - delta: the delta index of the field item being added.
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

    // Initialize the table. If the function returns FALSE
    // then the caller is trying to re-initialize the base table so just quit.
    if(!$this->initTable($elements)){
      return;
    }

    // Make sure all of the required elements are present
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
      'chado_column' => $chado_column,
      'column_alias' => $column_alias,
      'field_name' => $field_name,
      'property_key' => $property_key
    ];

    // We need to add a value to the 'values' arrah with an empty value as this will
    // get filled in after a load.
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] = NULL;
  }

  /**
   * Sets the record ID for all fields.
   *
   * Record IDs may not be known when ChadoRecords is setup. For example,
   * a field may be added that needs a link to a base table, but it may not
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
      $items = $this->getTableItems($base_table, $table_alias);
      foreach ($items as $delta => $record) {
        foreach (array_keys($record['values']) as $column_alias) {

          // if this column is an ID field and links to this base table then update the value.
          if (array_key_exists($column_alias, $record['link_columns'])) {
            $base_table = $record['link_columns'][$column_alias];
            $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['values'][$column_alias] = $record_id;


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
   * For the given base table, returns non base tables that have conditions set.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   *
   * @return array
   *   The list of tables linked to the base table but does
   *   not include the base table.
   */
  public function getAncillaryTablesWithCond(string $base_table) : array {
    $ret_val = [];
    $tables = $this->getAncillaryTables($base_table);
    foreach ($tables as $table_alias) {
      $items = $this->getTableItems($base_table, $table_alias);
      foreach (array_keys($items[0]['conditions']) as $column_alias) {
        $ret_val[$table_alias] = 1;
      }
    }
    return array_keys($ret_val);
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
  public function getNumTableItems(string $base_table, string $table_alias) {
    return count($this->records[$base_table]['tables'][$table_alias]['items']);
  }

  /**
   * Returns the list of fields that require values from the given table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   */
  public function getTableFields(string $base_table, string $table_alias) {

    $fields = [];
    $items = $this->getTableItems($base_table, $table_alias);
    foreach ($items as $delta => $item) {
      foreach ($item['field_columns'] as $chado_alias => $field_columns) {
        foreach ($field_columns as $info) {
          $field_name = $info['field_name'];
          $fields[$field_name] = 1;
        }
      }
    }
    return array_keys($fields);
  }

  /**
   * This function adds a new item to the table.
   *
   * This function gets called internally during a find operation when we
   * need to add recrods beyond the original element used for searching.
   * It simply copies the item for delta 0 and clears the values so they can
   * get set.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   */
  protected function addEmptyTableItem(string $base_table, string $table_alias) {
    $items = $this->getTableItems($base_table, $table_alias);
    $num_items = count($items);
    $this->records[$base_table]['tables'][$table_alias]['items'][$num_items] = $items[0];

    // Clear the values for this new item.
    foreach (array_keys($items[0]['values']) as $column_alias) {
      $this->records[$base_table]['tables'][$table_alias]['items'][$num_items]['values'][$column_alias] = NULL;
    }
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
        $aliases[] = $column_alias;
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
  protected function setColumnValue(string $base_table, string $table_alias,
      int $delta, string $column_alias, $value) : bool {

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
    $this->records[$base_table]['tables'][$table_alias]['items'][$delta]['has_values'] = TRUE;
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
  public function getColumnValue(string $base_table, string $table_alias, int $delta, string $column_alias) {

    if (!array_key_exists($base_table, $this->records)) {
      throw new \Exception(t('ChadoRecords::getFieldValue(): The base table has not been added to the ChadoRecords object: @base_table.',
          ['@base_table' => $base_table]));
    }
    if (!array_key_exists($table_alias, $this->records[$base_table]['tables'])) {
      return NULL;
    }

    $items = $this->getTableItems($base_table, $table_alias);
    if (!array_key_exists($delta, $items)) {
      return NULL;
    }
    if (!array_key_exists($column_alias, $items[$delta]['values'])) {
      return NULL;
    }

    // If the values were set then return it, otherwise return NULL;
    if ($items[$delta]['has_values'] === TRUE) {
      return $items[$delta]['values'][$column_alias];
    }

    return NULL;
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
      $this->setLinks($base_table);

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
      foreach ($info['columns'] as $lcol => $rcol) {

        $lcol_aliases = $this->getColumnFieldAliases($base_table, $base_table, $delta, $lcol);

        // If the FK is not set in the record then skip it.
        if (!$lcol_aliases) {
          continue;
        }
        $lcol_alias = $lcol_aliases[0];

        // If an FK allows nulls and the value is null then skip this one.
        $col_val = $record['values'][$lcol_alias];
        if ($table_def['fields'][$lcol]['not null'] == FALSE and !$col_val) {
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
        $query = $this->connection->select('1:'. $base_table, $base_table);
        $query->fields($base_table);
        foreach ($ukey_cols as $col) {
          $col = trim($col);
          $col_val = NULL;
          if (in_array($col, $record['columns'])) {
            $col_val = $record['values'][$col];
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
          // exists. Or, if the record_id isn't the same as the matched
          // record. This is an update that conflicts with an existing
          // record.
          if (($record_id == 0) or ($record_id != $match->$pkey)) {
            // Documentation for how to create a violation is here
            // https://github.com/symfony/validator/blob/6.1/ConstraintViolation.php
            $message = 'The item cannot be saved as another already exists with the following values: ';
            $params = [];
            foreach ($ukey_cols as $col) {
              $col = trim($col);
              $col_val = NULL;
              if (in_array($col, $record['columns'])) {
                // @todo need to use the column alias when getting the value.
                $col_val = $record['values'][$col];
              }
              $params["@$col"] = $col_val;
              $message .=  ucfirst($col) . ": '@$col'" . (count($ukey_cols) == count($params)?'. ':', ');
            }
            // Explanation of the unique violation.
            if (count($params) > 1) {
              $message .= 'The combination of these @param_count values';
              $params['@param_count'] = count($params);
            }
            else {
              $message .= 'This value';
            }
            $message .= ' must be unique for every item.';
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
      if (in_array($col, $record['columns'])) {
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
      $message = 'The item cannot be saved because the following fields for the Chado '
        . '"' . $base_table . '" table are missing. ';
      $params = [];
      foreach ($missing as $col) {
        $message .=  $col . ", ";
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

    // Get information about this Chado table.
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

      // Build the Insert.
      $insert = $this->connection->insert('1:' . $chado_table);

      // Generate the list of fields to add to the insert.
      $values = [];
      foreach ($record['columns'] as $column_alias) {
        $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
        $values[$chado_column] = $record['values'][$column_alias];
      }

      // Remove the primary key. It can't be set on an insert. Most likely it's
      // zero.
      unset($values[$pkey]);

      // Add the fields to the insert.
      $insert->fields($values);
      $this->field_debugger->reportQuery($insert, "Insert Query for $chado_table ($delta)");

      // Execute the insert.
      $record_id = $insert->execute();
      if (!$record_id) {
        throw new \Exception(t('Failed to insert a record in the Chado "@table" table. Alias: @alias, Record: @record',
            ['@alias' => $table_alias, '@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Update the field with the record id.
      $column_aliases = $this->getColumnFieldAliases($base_table, $table_alias, $delta, $pkey);
      if (!$column_aliases){
        throw new \Exception(t('Failed to insert a record in the Chado "@table" because the primary key is missing as a field. Alias: @alias, Record: @record',
            ['@alias' => $table_alias, '@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }
      $pkey_alias = array_shift($column_aliases);

      $this->setColumnValue($base_table, $table_alias, $delta, $pkey_alias, $record_id);
      if ($base_table === $table_alias) {
        $this->setRecordID($base_table, $record_id);
        $this->setLinks($base_table);
      }
    }
  }

  /**
   * Queries for multiple records in Chado for a given table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $base_table_alias
   *   The alias of the base table.
   *
   * @return array
   *   An array of \Drupal\tripal_chado\TripalStorage\ChadoRecords objects.
   *
   * @throws \Exception
   */
  public function findRecords(string $base_table, string $base_table_alias) {

    $found_records = [];

    // Get information about this Chado table.
    $chado_table = $this->getTableFromAlias($base_table, $base_table_alias);

    // Iterate through each and perform a select.
    $items = $this->getTableItems($base_table, $base_table_alias);
    foreach ($items as $delta => $record) {

      // Start the select
      $select = $this->connection->select('1:' . $chado_table, $base_table_alias);

      // Add the fields in the chado table.
      foreach ($record['columns'] as $column_alias) {
        $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
        $select->addField($base_table_alias, $chado_column, $column_alias);
      }

      // Add in any joins.
      if (array_key_exists('joins', $record)) {
        $join_paths = array_keys($record['joins']);
        sort($join_paths);
        foreach ($join_paths as $join_path) {
          $join_info = $record['joins'][$join_path];
          $right_table = $join_info['on']['right_table'];
          $right_alias = $join_info['on']['right_alias'];
          $right_column = $join_info['on']['right_column'];
          $left_alias = $join_info['on']['left_alias'];
          $left_column = $join_info['on']['left_column'];

          $select->leftJoin('1:' . $right_table, $right_alias, $left_alias . '.' .  $left_column . '=' .  $right_alias . '.' . $right_column);

          foreach ($join_info['columns'] as $column) {
            $join_column = $column['chado_column'];
            $join_column_alias = $column['column_alias'];
            $select->addField($right_alias, $join_column, $join_column_alias);
          }
        }
      }

      // Add the select condition
      foreach ($record['conditions'] as $column_alias => $value) {
        // If we don't have a primary key for the base table then skip the condition.
        if (array_key_exists($column_alias, $record['link_columns']) and !$this->getRecordID($base_table)) {
          continue;
        }
        $select->condition($base_table_alias . '.' . $column_alias, $value['value'], $value['operation']);
      }
      $this->field_debugger->reportQuery($select, "Select Query for $chado_table ($delta)");

      // Execute the query.
      $results = $select->execute();
      if (!$results) {
        throw new \Exception(t('Failed to select record in the Chado "@table" table. Record: @record',
          ['@table' => $chado_table, '@record' => print_r($record, TRUE)]));
      }

      // Iterate through the results and create a new record for each one.
      while ($values = $results->fetchAssoc()) {

        // We start by cloning the records array that was used to query.
        $new_record = new ChadoRecords($this->field_debugger, $this->logger, $this->connection);
        $new_record->copyRecords($this);

        // Update the values in the new record.
        foreach ($values as $column_alias => $value) {
          if ($value !== NULL) {
            $new_record->setColumnValue($base_table, $base_table_alias, $delta, $column_alias, $value);

            // If this is the base table be sure to set the record ID.
            if ($base_table === $base_table_alias and array_key_exists($column_alias, $record['link_columns'])) {
              $new_record->setRecordID($base_table, $value);
            }
          }
        }

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

      // Start the update.
      $update = $this->connection->update('1:'. $chado_table);

      // Add the fields to update.
      $fields = [];
      foreach ($record['columns'] as $column_alias) {
        $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
        $fields[$chado_column] = $record['values'][$column_alias];
      }
      $update->fields($fields);

      foreach ($record['conditions'] as $column_alias => $details) {
        $update->condition($column_alias, $details['value']);
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
   *   Set to TRUE to not throw an exception if valid conditions are not
   *   set. If TRUE then it skips the record rather than performs the delete.
   *
   * @throws \Exception
   */
  public function deleteRecords(string $base_table, string $table_alias, bool $graceful = FALSE) {

    // Make sure all IDs are up to date.
    $this->setLinks($base_table);

    // Get the Chado table for this given table alias.
    $chado_table = $this->getTableFromAlias($base_table, $table_alias);

    // Iterate through each item of the table and perform a delete.
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

      // Don't delete if the primary key is not set.
      $column_aliases = $this->getColumnFieldAliases($base_table, $table_alias, $delta, $pkey);
      if (!$column_aliases) {
        continue;
      }
      $pkey_alias = array_shift($column_aliases);
      if (empty($record['values'][$pkey_alias])) {
        continue;
      }


      $delete = $this->connection->delete('1:'. $chado_table);
      foreach ($record['conditions'] as $column_alias => $cond_value) {
        $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
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
   * Selects the items for a given table in a record object.
   *
   * This function is used for the findValues() and loadValues() functions so
   * it needs to be able to find multiple records from the base table and
   * multiple items from an ancillary table.
   *
   * @param string $base_table
   *   The name of the Chado table used as a base table.
   * @param string $table_alias
   *   The alias of the table.  For the base table, use the same table name as
   *   base tables don't have aliases.
   *
   * @throws \Exception
   *
   * @return int
   *   Returns the number of items for this table that were found.
   */
  public function selectItems(string $base_table, string $table_alias) : int {

    // Indicates the number of items that were found for this table.
    // We need to return the number found because even if no records are found
    // the `values` array of $this->records will still have the values that were
    // provided to it. Since we use that same array for updates/inserts it
    // makes sense for those values to be there.  So, we need something to
    // indicate if we actually did find values on a `loadValues()` or
    // `findValues()` call.
    $items_found = 0;

    // Make sure all IDs are up to date.
    $this->setLinks($base_table);

    // Get the Chado table for this given table alias.
    $chado_table = $this->getTableFromAlias($base_table, $table_alias);

    // Iterate through each item of the table and perform a select.
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

      // Start the select
      $select = $this->connection->select('1:' . $chado_table, $table_alias);

      // Add the fields in the chado table.
      foreach ($record['columns'] as $column_alias) {
        $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
        $select->addField($table_alias, $chado_column, $column_alias);
      }

      // Add in any joins.
      if (array_key_exists('joins', $record)) {
        $join_paths = array_keys($record['joins']);
        sort($join_paths);
        foreach ($join_paths as $join_path) {
          $join_info = $record['joins'][$join_path];
          $right_table = $join_info['on']['right_table'];
          $right_alias = $join_info['on']['right_alias'];
          $right_column = $join_info['on']['right_column'];
          $left_alias = $join_info['on']['left_alias'];
          $left_column = $join_info['on']['left_column'];

          $select->leftJoin('1:' . $right_table, $right_alias,
            $left_alias . '.' .  $left_column . '=' .  $right_alias . '.' . $right_column);

          foreach ($join_info['columns'] as $column) {
            $join_column = $column['chado_column'];
            $join_column_alias = $column['column_alias'];
            $select->addField($right_alias, $join_column, $join_column_alias);
          }
        }
      }

      // Add the select condition
      foreach ($record['conditions'] as $column_alias => $value) {
        if (!empty($value['value'])) {
          $chado_column = $record['column_aliases'][$column_alias]['chado_column'];
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

      // Update the values in the record.
      $current_items = count($this->getTableItems($base_table, $table_alias));
      $i = 0;
      while ($values = $results->fetchAssoc()) {

        // On a loadValues() then the record array will have all of the items
        // available. On a findValues() the records array is empty and we need
        // to expand it.  The following will allow us to expand the
        // items array if we don't have enough elements.
        if ($delta + $i > $current_items - 1) {
          $this->addEmptyTableItem($base_table, $table_alias);
        }

        foreach ($values as $column_alias => $value) {
          if ($value !== NULL) {
            $this->setColumnValue($base_table, $table_alias, $delta + $i, $column_alias, $value);
            // If this is the base table be sure to set the record ID.
            if ($base_table === $table_alias and array_key_exists($column_alias, $record['link_columns'])) {
              $this->setRecordID($base_table, $value);
            }
          }
        }
        $i++;
        $items_found++;
      }
    }
    return $items_found;
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
