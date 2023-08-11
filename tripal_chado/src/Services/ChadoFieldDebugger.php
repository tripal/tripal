<?php

namespace Drupal\tripal_chado\Services;

use \Drupal\tripal_chado\Database\ChadoConnection;
use \Drupal\tripal\Services\TripalLogger;

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
  public function reportValues(array $values) {

    if ($this->has_fields2debug === FALSE) {
      return;
    }

    $message = 'The values submitted to ChadoStorage are:';

    $output = [];
    $i = 0;
    foreach ($values as $field_name => $level1) {
      if (array_key_exists($field_name, $this->fields2debug)) {
        $output[$field_name] = [];
        foreach ($level1 as $delta => $level2) {
          $output[$field_name][$delta] = [];
          foreach ($level2 as $property_key => $level3) {
            $val = $level3['value']->getValue();
            $output[$field_name][$delta][$property_key] = $val;
          }
        }
      }
    }

    $this->logger->notice($message, [],
      ['drupal_set_message' => TRUE]);
    dpm($output);
  }

}
