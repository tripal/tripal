<?php

namespace Drupal\tripal4\TripalStorage;

/**
 * Base class for a Tripal storage key based classes such as schemas or records.
 */
class KeyBase {

  /**
   * Constructs a new Tripal storage key object.
   *
   * @param string entityId
   *   The entity id associated with this storage key object.
   *
   * @param string fieldId
   *   The field id associated with this storage key object.
   *
   * @param string fieldKey
   *   The field key associated with this storage key object.
   */
  public function __construct($entityId,$fieldId,$fieldKey) {
    $this->entityId = $entityId;
    $this->fieldId = $fieldId;
    $this->fieldKey = $fieldKey;
  }

  /**
   * Returns the entity id associated with this storage key object.
   *
   * @return string
   *   The entity id.
   */
  public function getEntityId() {
    return $this->entityId;
  }

  /**
   * Returns the field id associated with this storage key object.
   *
   * @return string
   *   The field id.
   */
  public function getFieldId() {
    return $this->fieldId;
  }

  /**
   * Returns the field key associated with this storage key object.
   *
   * @return string
   *   The field key.
   */
  public function getFieldKey() {
    return $this->fieldKey;
  }

  /**
   * The entity id associated with this storage key object.
   *
   * @var string
   */
  private $entityId;

  /**
   * The field id associated with this storage key object.
   *
   * @var string
   */
  private $fieldId;

  /**
   * The field key associated with this storage key object.
   *
   * @var string
   */
  private $fieldKey;
}
