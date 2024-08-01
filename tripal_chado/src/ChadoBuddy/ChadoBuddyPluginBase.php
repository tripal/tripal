<?php

namespace Drupal\tripal_chado\ChadoBuddy;

use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\tripal_chado\ChadoBuddy\Interfaces\ChadoBuddyInterface;
use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;
use Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord;

/**
 * Base class for chado_buddy plugins.
 */
abstract class ChadoBuddyPluginBase extends PluginBase implements ChadoBuddyInterface, ContainerFactoryPluginInterface {

  /**
   * Provides the TripalDBX connection to chado that this ChadoBuddy should act upon.
   * @var Drupal\tripal_chado\Database\ChadoConnection
   *
   */
  public ChadoConnection $connection;

 /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
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
   * Retrieve a list of table columns for one or more chado tables.
   * Schema information is cached for better performance.
   *
   * @param array $chado_tables
   *   One or more chado table namess.
   * @param bool $required_only
   *   If TRUE, only return columns that [1]: have a NOT NULL
   *   constraint, and [2]: do not have a default value and
   *   are not serial (i.e. a primary key is not required)
   *
   * @return array
   *   An array of table+dot+column name, e.g. for 'db' table:
   *   ['db.db_id', 'db.name', 'db.description', 'db.urlprefix', 'db.url']
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   **/
  protected function getTableColumns(array $chado_tables, bool $required_only = FALSE) {
    $columns = [];
    $cached_tables = [];
    $schema_name = $this->connection->getSchemaName();
    $cache_updated = FALSE;

    // Get cached columns if available
    $cache_id = $schema_name . '_buddy_table_columns';
    if ($cache = \Drupal::cache()->get($cache_id)) {
      $cached_tables = $cache->data;
    }
    foreach ($chado_tables as $chado_table) {
      if (!array_key_exists($chado_table, $chado_tables)) {
        $cache_updated = TRUE;
        $cached_tables[$chado_table] = [];
        $table_schema = $this->connection->schema()->getTableDef($chado_table, ['format' => 'drupal']);
        if (!array_key_exists('fields', $table_schema)) {
          $calling_function = debug_backtrace()[1]['function'];
          throw new ChadoBuddyException("ChadoBuddy $calling_function error, invalid table \"$chado_table\" passed to getTableColumns()");
        }
        foreach ($table_schema['fields'] as $field_name => $field_schema) {
          $required = FALSE;
          if ($field_schema['not null'] and !array_key_exists('default', $field_schema) and $field_schema['type'] != 'serial') {
            $required = TRUE;
          }
          $cached_tables[$chado_table][$field_name] = $required;
        }
      }

      // Lookup all or just required columns, depending on $required_only setting
      foreach ($cached_tables[$chado_table] as $column => $required) {
        if (!$required_only or $required) {
          $columns[] = $chado_table . '.' . $column;
        }
      }
    }

    // If $cached_tables was updated, cache the new version, specifying expiration in 1 hour.
    if ($cache_updated) {
      \Drupal::cache()->set($cache_id, $cached_tables, \Drupal::time()->getRequestTime() + (3600));
    }

    return $columns;
  }

  /**
   * Replace the first period with a double underscore
   * This makes the string valid as a table column alias.
   *
   * @param string $name
   *   table name+dot+table column
   *
   * @return string
   *   The first period is replaced with double underscore.
   **/
  protected function makeAlias(string $name): string {
    return preg_replace('/\./', '__', $name, 1);
  }

  /**
   * Replace the first double underscore with a period.
   * This reverts the change made by the makeAlias() function.
   *
   * @param string $name
   *   table name+__+table column
   *
   * @return string
   *   The first __ is replaced with a period.
   **/
  protected function unmakeAlias(string $name): string {
    return preg_replace('/__/', '.', $name, 1);
  }

  /**
   * Removes the table prefix from $values keys so that
   * they can be used directly in an INSERT.
   * The prefix is anything up to and including the first period.
   *
   * @param array $values
   *   Associative array where keys are table name+dot+table column.
   *
   * @return array
   *   The keys have had the table name prefix removed, values are unchanged.
   **/
  protected function removeTablePrefix(array $values): array {
    $new_values = [];
    foreach ($values as $key => $value) {
      $new_key = preg_replace('/^[^\.]*\./', '', $key);
      $new_values[$new_key] = $value;
    }
    return $new_values;
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
        throw new ChadoBuddyException("ChadoBuddy $calling_function error, the key \"$key\" is not valid for for this function.");
      }
    }
  }

  /**
   * Used to return a subset of values applicable to a
   * single chado table, e.g. remove db table columns when
   * inserting a new dbxref.
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
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, no valid values were specified.");
    }
    return $subset;
  }

  /**
   * Used to validate results from a buddy function,
   * to ensure there is exactly one record present.
   *
   * @param mixed $output_records
   *   This can be either FALSE, a ChadoBuddyRecord, or an array
   *   with multiple records. To be valid, it must be exactly
   *   one ChadoBuddyRecord.
   * @param array $values
   *   Pass query values to print if an exception is thrown.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If not exactly one record is present.
   */
  protected function validateOutput($output_records, array $values) {
    // These are unlikely cases, but you never know.
    if (!$output_records) {
      $calling_function = debug_backtrace()[1]['function'];
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, did not retrieve the expected record\n"
                                   . print_r($values, TRUE));
    }
    if (is_array($output_records)) {
      $calling_function = debug_backtrace()[1]['function'];
      $n = count($output_records);
      throw new ChadoBuddyException("ChadoBuddy $calling_function error, more than one record ($n) was retrieved, only one was expected\n"
                                   . print_r($values, TRUE));
    }
  }

}
