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
   * @param string entityType
   *   The entity type associated with this storage property type base.
   *
   * @param string fieldType
   *   The field type associated with this storage property type base.
   *
   * @param string key
   *   The key associated with this storage property type base.
   *
   * @param string id
   *   The id of this storage property type base.
   */
  public function __construct($entityType,$fieldType,$key,$id) {
    parent::__construct($entityType,$fieldType,$key);
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
//Text
//Boolean
//Real
//DateTime
