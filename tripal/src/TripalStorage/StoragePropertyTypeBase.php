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
   * @param string term_id
   *   The controlled vocabulary term asssociated with this property. It must be
   *   in the form of "IdSpace:Accession" (e.g. "rdfs:label" or "OBI:0100026")
   *
   * @param string id
   *   The id of this storage property type base.
   *
   * @param array storage_settings
   *   An array of settings required for this property by the storage backend.
   */
  public function __construct($entityType, $fieldType, $key, $term_id, $id, $storage_settings = []) {
    parent::__construct($entityType, $fieldType, $key, $term_id);
    $this->id = $id;
    $this->cardinality = 1;
    $this->searchability = TRUE;
    $this->operations = array('=','<>','>','>=','<','<=','STARTS_WITH','CONTAINS',
      'ENDS_WITH','IN', 'NOT IN', 'BETWEEN', 'NOT BETWEEN');
    $this->sortable = TRUE;
    $this->readOnly_ = FALSE;
    $this->required = FALSE;
    $this->storage_settings = $storage_settings;
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
   * Sets the cardinality.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @param int $cardinality
   *   The cardinality. A value of 0 indicates unlimited values.
   */
  public function setCardinality(int $cardinality) {
    $this->cardinality = $cardinality;
  }

  /**
   * Gets the cardinality.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @return bool
   *   The cardinality.
   */
  public function getCardinality() {
    return $this->cardinality;
  }

  /**
   * Sets the searchability.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @param bool $searchability
   *   The searchability.
   */
  public function setSearchability($searchability) {
    $this->searchability = $searchability;
  }

  /**
   * Gets the searchability.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @return bool
   *   The searchability.
   */
  public function getSearchability() {
    return $this->searchability;
  }

  /**
   * Sets the supported operations.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * Valid operations are (eq,ne,contains,starts).
   *
   * @param bool $searchability
   *   The operations.
   */
  public function setOperations($operations) {
    $this->operations = $operations;
  }

  /**
   * Gets the supported operations.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @return bool
   *   The operations.
   */
  public function getOperations() {
    return $this->operations;
  }

  /**
   * Sets the sortable property.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @param bool $sortable
   *   The sortable property.
   */
  public function setSortable($sortable) {
    $this->sortable = $sortable;
  }

  /**
   * Gets the sortable property.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @return bool
   *   The sortable property.
   */
  public function getSortable() {
    return $this->sortable;
  }

  /**
   * Sets the read only property.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @param bool $readOnly
   *   The read only property.
   */
  public function setReadOnly($readOnly) {
    $this->readOnly_ = $readOnly;
  }

  /**
   * Gets the read only property.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @return bool
   *   The read only property.
   */
  public function getReadOnly() {
    return $this->readOnly_;
  }

  /**
   * Sets the required property.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @param bool $required
   *   The required property.
   */
  public function setRequired($required) {
    $this->required = $required;
  }

  /**
   * Gets the required property.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @return bool
   *   The required property.
   */
  public function getRequired() {
    return $this->required;
  }

  /**
   * Gets the storage settings for this property type.
   *
   * @return array
   */
  public function getStorageSettings() {
    return $this->storage_settings;
  }

  /**
   *
   * @param array $storage_settings
   */
  public function setStorageSettings($storage_settings) {
    $this->storage_settings = $storage_settings;
  }

  /**
   * The id of this storage property type base.
   *
   * @var string
   */
  private $id;

  /**
   * The cardinality of this storage property type base.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @var bool
   */
  private $cardinality;

  /**
   * The searchability of this storage property type base.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @var bool
   */
  private $searchability;

  /**
   * The supported operations of this storage property type base.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @var array
   */
  private $operations;

  /**
   * The sortable property of this storage property type base.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @var bool
   */
  private $sortable;

  /**
   * The read only property of this storage property type base.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @var bool
   */
  private $readOnly_;

  /**
   * The required of this storage property type base.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * @var bool
   */
  private $required;

  /**
   * An array of elements required for this property by the storage backend.
   *
   * @var array
   */
  private $storage_settings;

}
