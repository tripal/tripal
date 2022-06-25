<?php

namespace Drupal\tripal\TripalStorage;

class TripalStorageUpdateException extends \Exception {

  /**
   * Constructs an TripalStorageTypeUpdateException with the given entity type
   * id and specific message.
   *
   * @param string $entityTypeId
   *   The entity type id.
   *
   * @param string $message
   *   The specific message describing the exception.
   */
  public function __construct($entityTypeId,$message) {
    parent::__construct(
      sprintf("Tripal storage update error in entity type '%s': %s",$entityTypeId,$message)
    );
  }

}
