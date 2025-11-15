<?php

namespace Drupal\tripal_chado\ChadoBuddy;

use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoBuddy\Interfaces\ChadoBuddyInterface;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;

/**
 * Base class for chado_buddy plugins.
 */
abstract class ChadoBuddyPluginBase extends PluginBase implements ChadoBuddyInterface, ContainerFactoryPluginInterface {

  /**
   * Provides the TripalDBX connection to chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  public ChadoConnection $connection;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static
   * function will be called behind the scenes when a Plugin Manager uses
   * createInstance(). Specifically, this method is used to determine the
   * parameters to pass to the constructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   *   The current container.
   * @param array $configuration
   *   A configuration array.
   * @param string $plugin_id
   *   The plugin identifier.
   * @param mixed $plugin_definition
   *   The definition of the plugin.
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal_chado.database')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, ChadoConnection $connection) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->connection = $connection;
  }

  /**
   * {@inheritdoc}
   */
  public function label(): string {
    // Cast the label to a string since it is a TranslatableMarkup object.
    return (string) $this->pluginDefinition['label'];
  }

  /**
   * {@inheritdoc}
   */
  public function description(): string {
    // Cast the description to a string since it is a TranslatableMarkup object.
    return (string) $this->pluginDefinition['description'];
  }

  /**
   * Get a table definition from the chado schema.
   *
   * @param string $table_name
   *   The table name to query.
   *
   * @return array
   *   The table schema.
   */
  public function getChadoTableDef(string $table_name): array {
    $parameters = [
      'format' => 'drupal',
      'source' => [
        'file',
        'tripal',
        'database',
      ],
    ];
    $def = $this->connection->schema()->getTableDef($table_name, $parameters);
    return $def;
  }

  /**
   * Retrieve a list of table columns for one or more chado tables.
   *
   * Schema information is cached for better performance.
   *
   * @param array $chado_tables
   *   One or more chado table names.
   * @param string $filter
   *   A string that indicates which subset of columns to return.
   *   Valid values are:
   *   - required: return columns that [1]: have a NOT NULL constraint,
   *     and [2]: do not have a default value and are not serial, such as a
   *     primary key. In other words, a column with a NOT NULL constraint
   *     but with some form of a default value is considered to be not required.
   *   - unique: return only columns that are part of any unique constraint.
   *   - all: return all columns. This is the default if an unrecognized filter
   *     is provided.
   *
   * @return array
   *   An array of table+dot+column name, e.g. for 'db' table:
   *   ['db.db_id', 'db.name', 'db.description', 'db.urlprefix', 'db.url']
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   For invalid table name.
   */
  protected function getTableColumns(array $chado_tables, string $filter = 'all'): array {
    $columns = [];
    $cache_updated = FALSE;

    // Get cached columns if available.
    $cached_tables = $this->getTableCache();
    foreach ($chado_tables as $chado_table) {
      if (!array_key_exists($chado_table, $cached_tables)) {
        $cache_updated = TRUE;
        $this->addTableToCache($chado_table, $cached_tables);
      }

      // Lookup all or requested subset of columns, based on $filter setting.
      foreach (array_keys($cached_tables[$chado_table]['all']) as $column) {
        $is_required = $cached_tables[$chado_table]['required'][$column];
        $is_in_constraint = $cached_tables[$chado_table]['unique'][$column];
        $pass = TRUE;
        if (($filter == 'required') and !$is_required) {
          $pass = FALSE;
        }
        if (($filter == 'unique') and !$is_in_constraint) {
          $pass = FALSE;
        }
        if ($pass) {
          $columns[] = $chado_table . '.' . $column;
        }
      }
    }

    // If $cached_tables was updated, cache the new version, specifying
    // expiration in 1 hour.
    if ($cache_updated) {
      $this->setTableCache($cached_tables);
    }

    return $columns;
  }

  /**
   * Retrieves the chado table definition cache used by getTableColumns().
   *
   * @return array
   *   An array describing the fields of any cached tables.
   *   The array follows this format:
   *    - <table name>:
   *      - 'all':
   *        - <field name>: TRUE
   *      - 'required':
   *        - <field name>: TRUE|FALSE depending on if this field is not null
   *          and doesn't have a default.
   *      - 'unique':
   *        - <field name>: TRUE|FALSE depending on if this field is in a
   *          unique constraint.
   */
  protected function getTableCache() {
    $schema_name = $this->connection->getSchemaName();

    // Get cached columns.
    $cache_id = $schema_name . '_buddy_table_columns';
    $cached_tables = [];
    if ($cache = \Drupal::cache()->get($cache_id)) {
      $cached_tables = $cache->data;
    }

    return $cached_tables;
  }

  /**
   * Update the chado table definition cache used by getTableColumns().
   *
   * @param array $cached_tables
   *   Schema information will be inserted in this array for the table indicated
   *   above. The array follows this format:
   *    - <table name>:
   *      - 'all':
   *        - <field name>: TRUE
   *      - 'required':
   *        - <field name>: TRUE|FALSE depending on if this field is not null
   *          and doesn't have a default.
   *      - 'unique':
   *        - <field name>: TRUE|FALSE depending on if this field is in a
   *          unique constraint.
   *
   * @return void
   *   No return value.
   */
  private function setTableCache(array $cached_tables): void {
    $schema_name = $this->connection->getSchemaName();
    $cache_id = $schema_name . '_buddy_table_columns';

    \Drupal::cache()->set($cache_id, $cached_tables, \Drupal::time()->getRequestTime() + (3600));
  }

  /**
   * Add a chado table to the cache, used only by getTableColumns()
   *
   * @param string $chado_table
   *   Name of the table to add.
   * @param array $cached_tables
   *   Schema information will be inserted in this array for the table indicated
   *   above. The array follows this format:
   *    - <table name>:
   *      - 'all':
   *        - <field name>: TRUE
   *      - 'required':
   *        - <field name>: TRUE|FALSE depending on if this field is not null
   *          and doesn't have a default.
   *      - 'unique':
   *        - <field name>: TRUE|FALSE depending on if this field is in a
   *          unique constraint.
   *
   * @return void
   *   No return value.
   */
  protected function addTableToCache(string $chado_table, array &$cached_tables): void {
    $cached_tables[$chado_table] = [];
    $table_schema = $this->getChadoTableDef($chado_table);
    if (!($table_schema['fields'] ?? FALSE)) {
      // Two levels up.
      $calling_function = debug_backtrace()[2]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, invalid table \"$chado_table\" passed to getTableColumns()");
    }

    // Obtain a list of the columns that are present in any unique key.
    $in_unique_constraint = [];
    if (array_key_exists('unique keys', $table_schema)) {
      foreach ($table_schema['unique keys'] as $constraint_columns) {
        if (is_string($constraint_columns)) {
          $constraint_columns = explode(', ', $constraint_columns);
        }
        foreach ($constraint_columns as $column) {
          $in_unique_constraint[$column] = TRUE;
        }
      }
    }

    foreach ($table_schema['fields'] as $field_name => $field_schema) {
      $is_required = ($field_schema['not null']
                      and !array_key_exists('default', $field_schema)
                      and $field_schema['type'] != 'serial');
      $is_in_constraint = $in_unique_constraint[$field_name] ?? FALSE;
      $cached_tables[$chado_table]['all'][$field_name] = TRUE;
      $cached_tables[$chado_table]['required'][$field_name] = $is_required;
      $cached_tables[$chado_table]['unique'][$field_name] = $is_in_constraint;
    }
  }

  /**
   * Used by upsert functions to generate a subset of values.
   *
   * The subset uses only key columns, e.g. 'name' for cv table.
   * Key columns are those which are present in any of the unique
   * constraints that the table may have.
   *
   * @param array $values
   *   An associative array where the key is the table.column_name.
   * @param array $key_columns
   *   Only column keys in this list should be returned.
   *
   * @return array
   *   The subset of the passed $values array.
   */
  protected function makeUpsertConditions(array $values, array $key_columns): array {
    $conditions = [];
    foreach ($key_columns as $column) {
      if (array_key_exists($column, $values)) {
        $conditions[$column] = $values[$column];
      }
    }
    return $conditions;
  }

  /**
   * Replace the first period with a double underscore.
   *
   * This makes the string valid as a table column alias.
   *
   * @param string $name
   *   Table name+dot+table column.
   *
   * @return string
   *   The first period is replaced with double underscore.
   */
  protected function makeAlias(string $name): string {
    return preg_replace('/\./', '__', $name, 1);
  }

  /**
   * Replace the first double underscore with a period.
   *
   * This reverts the change made by the makeAlias() function.
   *
   * @param string $name
   *   Table name+__+table column.
   *
   * @return string
   *   The first __ is replaced with a period.
   */
  protected function unmakeAlias(string $name): string {
    return preg_replace('/__/', '.', $name, 1);
  }

  /**
   * Removes the table prefix from $values keys.
   *
   * This allows the $values keys to be used directly in an INSERT.
   * The prefix is anything up to and including the first period.
   *
   * @param array $values
   *   Associative array where keys are table name+dot+table column.
   *
   * @return array
   *   The keys have had the table name prefix removed, values are unchanged.
   */
  protected function removeTablePrefix(array $values): array {
    $new_values = [];
    foreach ($values as $key => $value) {
      $new_key = preg_replace('/^[^\.]*\./', '', $key);
      if (array_key_exists($new_key, $new_values)) {
        throw new ChadoBuddyException("Ambiguous columns passed to removeTablePrefix(), this function can only handle columns in a single table. Passed values: "
          . print_r($values, TRUE));
      }
      $new_values[$new_key] = $value;
    }
    return $new_values;
  }

  /**
   * Adds conditions to the database query.
   *
   * Implements case insensitive queries if requested.
   *
   * @param object $query
   *   Associative array where keys are table name+dot+table column.
   * @param array $conditions
   *   Associative array of conditions to add to the query.
   * @param object $options
   *   Associative array of options as passed to the calling buddy function.
   *   The option 'case_insensitive' can contain a single key string, or an
   *   array of multiple keys for which a case insensitive query is desired.
   *
   * @return void
   *   No return value.
   */
  protected function addConditions(object &$query, array $conditions, array $options): void {
    // Obtain a list of case insensitive columns, can be empty.
    $insensitive_columns = [];
    if (array_key_exists('case_insensitive', $options)) {
      if (is_array($options['case_insensitive'])) {
        $insensitive_columns = $options['case_insensitive'];
      }
      else {
        $insensitive_columns[] = $options['case_insensitive'];
      }
    }

    // Conditions are not aliased.
    $n = 0;
    foreach ($conditions as $key => $value) {
      if (in_array($key, $insensitive_columns)) {
        $query->where('LOWER(' . $key . ') = LOWER(:value' . $n . ')',
                      [':value' . $n => $value]);
        $n++;
      }
      else {
        $query->condition($key, $value, '=');
      }
    }
  }

  /**
   * Used to validate input arrays to various buddy functions.
   *
   * @param array $user_values
   *   An associative array to be validated. Keys are
   *   table+dot+column name, values are the database table values.
   * @param array $valid_values
   *   An array listing all valid keys for $user_values.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If $user_values array is empty.
   *   If a key in $user_values is not in $valid_values.
   */
  protected function validateInput(array $user_values, array $valid_values) {
    if (!$user_values) {
      $calling_function = debug_backtrace()[1]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, no values were specified.");
    }
    foreach ($user_values as $key => $value) {
      if (!in_array($key, $valid_values)) {
        $calling_function = debug_backtrace()[1]['function'];
        throw new ChadoBuddyException("ChadoBuddy $calling_function error, the key \"$key\" is not valid for this function. Valid keys are: "
          . implode(', ', $valid_values));
      }
    }
  }

  /**
   * Dereference a ChadoBuddyRecord into its component values.
   *
   * If a ChadoBuddyRecords is present in the $values array,
   * then it is converted to its component array values.
   *
   * @param array $values
   *   An associative array to be validated. Keys are
   *   table+dot+column name, values are the database table values.
   *   The special case value of 'buddy_record' => ChadoBuddyRecord
   *   will have its component values appended to the values.
   *   In the case of a duplicated key, if the value in the
   *   ChadoBuddyRecord is different from the one in the array,
   *   then an exception is thrown.
   *
   * @return array
   *   Merged associative array of values
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   - If the key 'buddy_record' does not have a ChadoBuddyRecord as
   *     its value.
   *   - If a value inside the ChadoBuddyRecord is different than one in
   *     the $values array.
   */
  protected function dereferenceBuddyRecord(array $values): array {
    if (array_key_exists('buddy_record', $values)) {
      if (!$values['buddy_record'] instanceof ChadoBuddyRecord) {
        $calling_function = debug_backtrace()[1]['function'];
        throw new ChadoBuddyException("ChadoBuddy $calling_function error, something other than a ChadoBuddyRecord was stored under the 'buddy_record' key");
      }
      $buddy_values = $values['buddy_record']->getValues();
      foreach ($buddy_values as $buddy_key => $buddy_value) {
        if (array_key_exists($buddy_key, $values) and ($values[$buddy_key] != $buddy_value)) {
          $calling_function = debug_backtrace()[1]['function'];
          throw new ChadoBuddyException("ChadoBuddy $calling_function error, a value with the key $buddy_key was declared twice with different values");
        }
        $values[$buddy_key] = $buddy_value;
      }
      unset($values['buddy_record']);
    }
    return $values;
  }

  /**
   * Used to return a subset of values applicable to a single chado table.
   *
   * For example, remove db table columns when inserting a new dbxref.
   *
   * @param array $user_values
   *   An associative array to be filtered. Keys are
   *   table+dot+column name, values are for that table+column.
   * @param array $valid_tables
   *   An array listing which tables should have keys returned.
   *
   * @return array
   *   The subset of passed $user_values with table prefixes
   *   present in the $valid_tables array.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If after subsetting there is nothing left.
   */
  protected function subsetInput(array $user_values, array $valid_tables) {
    $subset = [];
    foreach ($user_values as $key => $value) {
      $parts = explode('.', $key, 2);
      if (in_array($parts[0], $valid_tables)) {
        $subset[$key] = $value;
      }
    }
    if (!$subset) {
      $calling_function = debug_backtrace()[1]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, no valid values were specified for tables: "
        . implode(', ', $valid_tables));
    }
    return $subset;
  }

  /**
   * Used to validate results from a buddy function.
   *
   * Validates that there is exactly one record present.
   *
   * @param mixed $output_records
   *   An array of zero or more ChadoBuddyRecords.
   *   To be valid, it must contain exactly one ChadoBuddyRecord.
   * @param array $values
   *   Pass query values to print if an exception is thrown.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If not exactly one record is present.
   */
  protected function validateOutput($output_records, array $values) {
    // These are unlikely cases, but you never know.
    if (!is_array($output_records) or (count($output_records) < 1)) {
      $calling_function = debug_backtrace()[1]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, did not retrieve the expected record\n"
        . print_r($values, TRUE));
    }
    $n = count($output_records);
    if ($n > 1) {
      $calling_function = debug_backtrace()[1]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, more than one record ($n) was retrieved, only one was expected\n"
        . print_r($values, TRUE));
    }
    if (!array_key_exists(0, $output_records) or !($output_records[0] instanceof ChadoBuddyRecord)) {
      $calling_function = debug_backtrace()[1]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, the array passed to validateOutput does not contain a ChadoBuddyRecord");
    }
  }

}
