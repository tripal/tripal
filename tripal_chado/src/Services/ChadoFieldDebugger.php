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
  }
}
