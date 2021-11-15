<?php

namespace Drupal\tripal4\TripalStorage;

use Drupal\tripal4\TripalStorage\SchemaBase;

/**
 * Base class for a Tripal storage schema.
 */
class IntSchema extends SchemaBase {

  /**
   * Constructs a new integer tripal storage schema.
   *
   * @param string entityId
   *   The entity id associated with this schema.
   *
   * @param string fieldId
   *   The field id associated with this schema.
   *
   * @param string fieldKey
   *   The field key associated with this schema.
   */
  public function __construct($entityId,$fieldId,$fieldKey) {
    parent::__construct($entityId,$fieldId,$fieldKey,"int");
  }

}
