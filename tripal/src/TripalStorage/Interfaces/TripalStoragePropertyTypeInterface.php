<?php

namespace Drupal\tripal\TripalStorage\Interfaces;

/**
 * Defines an interface for tripal storage property types.
 */
interface TripalStoragePropertyTypeInterface {

  /**
   * Returns the id of this storage property type base.
   *
   * @return string
   *   The id.
   */
  public function getId();

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
  public function setCardinality(int $cardinality);

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
  public function getCardinality();

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
  public function setSearchability($searchability);

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
  public function getSearchability();

  /**
   * Sets the supported operations.
   *
   * NOTE: Currently this is not being used but was part of the original design.
   *  We are leaving this here for now + intend to go back and discuss
   *  with Josh.
   *
   * Valid operations are (eq,ne,contains,starts).
   *
   * @param bool $operations
   *   The operations.
   */
  public function setOperations($operations);

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
  public function getOperations();

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
  public function setSortable($sortable);

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
  public function getSortable();

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
  public function setReadOnly($readOnly);

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
  public function getReadOnly();

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
  public function setRequired($required);

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
  public function getRequired();

  /**
   * Sets whether this property should be cached in the drupal tables or not.
   *
   * @param bool $status
   *   TRUE if it should be saved in Drupal and false if not.
   */
  public function setCacheStatus(bool $status);

  /**
   * Whether or not this property should be cached in the Drupal tables or not.
   *
   * @return bool
   *   TRUE if it should be saved in Drupal and false if not.
   */
  public function getCacheStatus();

  /**
   * Gets the storage settings for this property type.
   *
   * @return array
   *   An associative array of the storage settings for this property type.
   */
  public function getStorageSettings();

  /**
   * Gets the storage settings for this property type.
   *
   * @param array $storage_settings
   *   An associative array of the storage settings for this property type.
   */
  public function setStorageSettings($storage_settings);

}
