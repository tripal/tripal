<?php

namespace Drupal\tripal_chado\ChadoBuddy;

use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;

/**
 * This class defines the Chado Buddy Record.
 *
 * Each chado record returned by a ChadoBuddy service will be in the form of an
 * instance of this class.
 */
class ChadoBuddyRecord {

  /**
   * The base chado table that this record was retrieved from.
   *
   * In this context, the base table is the core table for this buddy.
   * For example, the base table would be cvterm for the CvtermBuddy.
   * A more complex example is for the PropertyBuddy, where the
   * base table is feature for a property buddy adding properties
   * to the featureprop table.
   *
   * @var string
   */
  protected string $base_table;

  /**
   * The name of the chado schema this record was retrieved from.
   *
   * @var string
   */
  protected string $schema_name;

  /**
   * An associative array of values from a chado table.
   *
   * The keys are chado table+dot+column name.
   * The values are chado table record values.
   * e.g. ['cvterm.name' => 'DNA'].
   *
   * @var array
   */
  protected array $values;

  /**
   * Sets the value of the base table.
   *
   * @param string $value
   *   The table name to be stored.
   *
   * @return void
   *   No return value.
   */
  public function setBaseTable(string $value): void {
    $this->base_table = $value;
  }

  /**
   * Returns the name of the base table.
   *
   * @return string
   *   The base table name.
   */
  public function getBaseTable(): string {
    return $this->base_table;
  }

  /**
   * Sets the value of the schema name.
   *
   * @param string $value
   *   The schema name to be stored.
   *
   * @return void
   *   No return value.
   */
  public function setSchemaName(string $value): void {
    $this->schema_name = $value;
  }

  /**
   * Returns the value of the schema name.
   *
   * @return string
   *   The schema name.
   */
  public function getSchemaName(): string {
    return $this->schema_name;
  }

  /**
   * Sets the associative array with values from a chado table record.
   *
   * @param array $values
   *   An associative array of key=>value pairs.
   *
   * @return void
   *   No return value.
   */
  public function setValues(array $values): void {
    $this->values = $values;
  }

  /**
   * Adds or updates one value in the values array.
   *
   * @param string $key
   *   A key for the $values associative array.
   * @param mixed $value
   *   The value to be stored.
   *
   * @return void
   *   No return value.
   */
  public function setValue(string $key, $value): void {
    $this->values[$key] = $value;
  }

  /**
   * Returns an associative array of values from a chado table record.
   *
   * @return array
   *   The array of key value pairs.
   */
  public function getValues(): array {
    return $this->values;
  }

  /**
   * Retrieves one value from the values array.
   *
   * @param string $key
   *   A key for the $values associative array.
   * @param array $options
   *   Associative array of options.
   *   The only supported option is 'strict'. If the key does not
   *   exist and strict is TRUE, throw an exception.
   *   If FALSE, return NULL. Defaults to TRUE.
   *
   * @return mixed
   *   The value corresponding to the key, or NULL if key is absent.
   *
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   *   If the specified key is not present and strict is set to TRUE (default).
   */
  public function getValue(string $key, array $options = []) {
    $strict = $options['strict'] ?? TRUE;
    if ($strict and !array_key_exists($key, $this->values)) {
      throw new ChadoBuddyException("ChadoBuddy error, the key '$key' is not present in the values array");
    }
    return $this->values[$key] ?? NULL;
  }

  /**
   * Compares two ChadoBuddyRecords.
   *
   * BOTH ChadoBuddyRecords must be from the same base table.
   *
   * @param ChadoBuddyRecord $a
   *   The first record to be compared.
   * @param ChadoBuddyRecord $b
   *   The second record to be compared.
   * @param string $value_key
   *   The key to use with getValue() to compare record $a and $b.
   *
   * @return int
   *   Uses the same return value approach as strcmp for the ChadoBuddyRecord
   *   value indicated by $value_key. Specifically,
   *   - negative integer if the value from $a is less than that from $b
   *   - positive integer if the value from $a is greater than that from $b
   *   - 0 if they are equal.
   */
  public static function compareTo(ChadoBuddyRecord $a, ChadoBuddyRecord $b, string $value_key) {
    $a_value = $a->getValue($value_key);
    $b_value = $b->getValue($value_key);
    return strcmp($a_value, $b_value);
  }

}
