<?php

namespace Drupal\tripal_chado\Services;

use \Drupal\tripal_chado\Database\ChadoConnection;
use \Drupal\tripal\Services\TripalLogger;
use Drupal\tripal_chado\TripalStorage\ChadoRecords;
use Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage;

/**
 * Provides debugging functionality for Chado data loading in fields extending
 * the ChadoFieldItemBase that use properties to automatically load the data.
 *
 * THIS SERVICE SHOULD ONLY BE CALLED BY CHADO STORAGE.
 *
 * If you are a developer of a field looking for debugging information,
 * you should set the $debug variable to TRUE in your field type class.
 *
 * This variable will be used by ChadoStorage to tell the ChadoFieldDebugger
 * service to display debugging information. All you need to do as a developer
 * is set this variable to TRUE in your field and debuggin information will be
 * displayed on the screen and in the drupal logs when you create, edit,
 * and load content that has you field attached.
 */
class ChadoFieldDebugger {

  /**
   * The chado connection used to query chado.
   */
  public ChadoConnection $chado_connection;

  /**
   * The logger class to use to providing our debugging messages to the developer.
   */
  public TripalLogger $logger;

  /**
   * An array of field names to enable debugging information for.
   *
   * Note: This will be set by chado storage based on the field annotation.
   */
  public array $fields2debug = [];

  /**
   * A simple flag to indicate if there are any fields to be debugged
   * for performances sake.
   */
  public bool $has_fields2debug = FALSE;
  /**
   * Object constructor for the Chado Field debugger
   *
   * @param Drupal\tripal_chado\Database\ChadoConnection
   *   The chado connection used to query chado.
   * @param Drupal\tripal\Services\TripalLogger
   *   The logger class to use to providing our debugging messages to the developer.
   */
  public function __construct(ChadoConnection $connection, TripalLogger $logger) {
    $this->chado_connection = $connection;
    $this->logger = $logger;
  }

  /**
   * A way for ChadoStorage to tell this service which fields should be debugged.
   */
  public function addFieldToDebugger(string $field_name) {
    $this->fields2debug[$field_name] = $field_name;
    $this->has_fields2debug = TRUE;
  }

  /**
   * Prints out the values array in a readable manner for debuggin purposes.
   * This is called by ChadoStorage::buildChadoRecords().
   */
  public function reportValues(array $values, string $message) {

    if ($this->has_fields2debug === FALSE) {
      return;
    }

    $debugging_fields = [];
    $all_fields = [];
    $i = 0;
    foreach ($values as $field_name => $level1) {

      $all_fields[$field_name] = [];
      foreach ($level1 as $delta => $level2) {
        $all_fields[$field_name][$delta] = [];
        foreach ($level2 as $property_key => $level3) {
          $val = $level3['value']->getValue();
          $all_fields[$field_name][$delta][$property_key] = $val;
        }
      }
    }

    dpm($all_fields, $message);
  }

  /**
   * Summarize the current state of chadostorage.
   *
   * @param ChadoStorage $chadostorage
   *   The current chadostorage object for interrogation.
   * @param string $message
   *   A short message describing where this method was called from.
   */
  public function summarizeChadoStorage(ChadoStorage $chadostorage, $message) {

    if ($this->has_fields2debug === FALSE) {
      return;
    }

    $state = [];

    // First summarize all the field properties.
    $properties = $chadostorage->getTypes();
    foreach ($properties as $field_name => $field_properties) {
      $field_definition = $chadostorage->getFieldDefinition($field_name);
      $state[$field_name] = [
        'field name' => $field_name,
        'field definition added' =>  is_object($field_definition),
        'field settings' => (is_object($field_definition)) ? $field_definition->getSettings() : NULL,
        'number of properties' => count($field_properties),
        'properties' => [],
      ];

      foreach ($field_properties as $property_key => $propertyType) {

        if (!array_key_exists('field type', $state[$field_name])) {
          $state[$field_name]['field type'] = $propertyType->getFieldType();
        }
        $state[$field_name]['properties'][$property_key] = [
          'property type' => get_class($propertyType),
          'term' => $propertyType->getTermIdSpace() . ':' . $propertyType->getTermAccession(),
          'storage settings' => $propertyType->getStorageSettings(),
        ];
      }
    }

    dpm($state, $message);
  }

  /**
   * This will summarize the results of ChadoStorage::buildChadoRecords().
   *
   * @param array $records
   *   This is an instance of the TripalStorage ChadoRecords class which contains
   *   all the information of records to be inserted/modifiedin chado.
   *   generated using the Drupal Query Builder in the ChadoStorage::*Values() methods.
   */
  public function summarizeBuiltRecords(ChadoRecords $records) {

    if ($this->has_fields2debug === FALSE) {
      return;
    }

    dpm($records->getRecordsArray(), 'The array describing the record queries to be generated');
    dpm($records->getBaseTables(), 'The known primary keys for our base records');
  }

  /**
   * This function will print out the query generated by the query builder.
   * It is expected that this function will be called right before
   * query->execute() is called in all the ChadoStorage::*ChadoRecord() methods.
   *
   * @param object $query
   *   This is the object built by the dynamic query builder. For example,
   *   if you are generating a select query then this is the object created
   *   by ChadoConnection::select() after all fields, conditions and joins
   *   have been added to it.
   * @param string $message
   *   This is a simple string to indicate who called this method.
   */
  public function reportQuery(object $query, $message) {

    if ($this->has_fields2debug === FALSE) {
      return;
    }

    $sql = (string) $query;

    /**
     * We would like to complete print out the query with subbed in parameters
     * but it's driving me crazy.
     *
     * Usually we would use $query->arguments() to get the arguments with
     * placeholders but there are a number of bugs here:
     *  - in Drupal 10 $insertQuery->arguments() provides a scope error.
     *  - $updateQuery->arguments() only provides the conditional arguments,
     *    not those being updated (facepalm; see Drupal Issue #2005626)
     *
    $quoted = [];
    foreach ((array) $query->arguments() as $index => $val) {
      $key = 'db_placeholder_' . $index;
      $quoted[$key] = is_null($val) ? 'NULL' : $this->chado_connection->quote($val);
    }
    $sql = strtr($sql, $quoted);
    */

    dpm($sql, $message . ' (See Records for parameters)');

  }

  /**
   * Print some sort of header to make reading all the output easier ;-p
   */
  public function printHeader(string $process_name) {

    if ($this->has_fields2debug === FALSE) {
      return;
    }

    $this->logger->notice(
      str_repeat('-', 9) . ' '
      . strtoupper($process_name) . ' '
      . str_repeat('-', 27)
      . date("H:i:s"),
      [], ['drupal_set_message' => TRUE, 'logger' => FALSE]
    );
  }

  /**
   * Print random debugging text.
   */
  public function printText(string $text) {

    if ($this->has_fields2debug === FALSE) {
      return;
    }

    $this->logger->notice(
      $text . ' (' . date("H:i:s") . ')',
      [], ['drupal_set_message' => TRUE, 'logger' => FALSE]
    );
  }
}
