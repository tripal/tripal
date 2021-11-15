<?php

namespace Drupal\tripal4\TripalStorage;

use Drupal\tripal4\TripalStorage\KeyBase;

/**
 * Base class for a Tripal storage record.
 */
class Record extends KeyBase {

  /**
   * Constructs a new tripal storage record.
   *
   * @param string entityId
   *   The entity id associated with this record.
   *
   * @param string fieldId
   *   The field id associated with this record.
   *
   * @param string fieldKey
   *   The field key associated with this record.
   *
   * @param ? $value
   *   An optional initial value for this record.
   */
  public function __construct($entityId,$fieldId,$fieldKey,$value = Null) {
    parent::__construct($entityId,$fieldId,$fieldKey);
    $this->value = $value;
  }

  /**
   * Returns the value of this record.
   *
   * @return ?
   *   The value.
   */
  public function getValue() {
    return $this->value;
  }

  /**
   * The value of this record.
   *
   * @var ?
   */
  private $value;

}
