<?php

namespace Drupal\tripal_chado\ChadoBuddy;

use Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException;

/**
 * Chado Buddy Record
 *
 * Each chado record returned by a ChadoBuddy service will be in the form of an
 * instance of this class.
 */
class ChadoBuddyRecord {

  /**
   * The base chado table that this record was retrieved from.
   *
   * This is the table that would be the FROM in the query rather then
   * any tables included via joins.
   * @var string
   */
  protected string $base_table;

  /**
   * The name of the chado schema this record was retrieved from.
   * @var string
   */
  protected string $schema_name;

  /**
   * An associative array where the keys are chado table column
   * names and the values are chado table record values.
   * @var array
   */
  protected array $values;


  /**
   * Sets the associative array with values looked up from
   * a chado table record.
   *
   * @param array $values
   *   An associative array of key=>value pairs.
   */
  public function setValues(array $values) {
    $this->values = $values;
  }

  /**
   * Adds or updates one value in the values array.
   *
   * @param string $key
   *   A key for the $values associative array.
   * @param mixed $value
   *   The value to be stored.
   */
  public function setValue(string $key, $value) {
    $this->value[$key] = $value;
  }

  /**
   * Returns the associative array of values looked up from
   * a chado table record.
   *
   * @return array
   */
  public function getValues() {
    return $this->values;
  }

  /**
   * Retrieves one value from the values array,
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
   * @throws Drupal\tripal_chado\ChadoBuddy\Exceptions\ChadoBuddyException
   */
  public function getValue(string $key, array $options = []) {
    $strict = $options['strict'] ?? TRUE;
    if ($strict and !array_key_exists($key, $this->values)) {
      throw new ChadoBuddyException("ChadoBuddy error, the key '$key' is not present in the values array");
    }
    return $this->values[$key] ?? NULL;
  }

}
