<?php

namespace Drupal\tripal\TripalStorage;

use Drupal\tripal\TripalStorage\StoragePropertyBase;

/**
 * Base class for a Tripal storage property type.
 */
class StoragePropertyTypeBase extends StoragePropertyBase {

  /**
   * Constructs a new tripal storage property type base.
   *
   * @param string entityId
   *   The entity id associated with this storage property type base.
   *
   * @param string fieldId
   *   The field id associated with this storage property type base.
   *
   * @param string fieldKey
   *   The field key associated with this storage property type base.
   *
   * @param string id
   *   The id of this storage property type base.
   */
  public function __construct($entityId,$fieldId,$fieldKey,$id) {
    parent::__construct($entityId,$fieldId,$fieldKey);
    $this->id = $id;
  }

  /**
   * Returns the id of this storage property type base.
   *
   * @return string
   *   The id.
   */
  public function getId() {
    return $this->id;
  }

  /**
   * The id of this storage property type base.
   *
   * @var string
   */
  private $id;

}
