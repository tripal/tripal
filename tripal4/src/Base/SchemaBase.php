<?php

namespace Drupal\tripal4\TripalStorage;

use Drupal\tripal4\TripalStorage\KeyBase;

/**
 * Base class for a Tripal storage schema.
 */
class SchemaBase extends KeyBase {

  /**
   * Constructs a new tripal storage schema.
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
  public function __construct($entityId,$fieldId,$fieldKey,$id) {
    parent::__construct($entityId,$fieldId,$fieldKey);
    $this->id = $id;
  }

  /**
   * Returns the id of this schema.
   *
   * @return string
   *   The id.
   */
  public function getId() {
    return $this->id;
  }

  /**
   * The id of this schema.
   *
   * @var string
   */
  private $id;

}
