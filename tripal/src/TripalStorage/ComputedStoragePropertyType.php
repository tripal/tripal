<?php

namespace Drupal\tripal\TripalStorage;

/**
 * Defines the computed Tripal storage property type.
 *
 * Note: A computed type is a value that is calculated at runtime.
 */
class ComputedStoragePropertyType extends StoragePropertyTypeBase {

  /**
   * Constructs a new computed tripal storage property type.
   *
   * @param string $entityType
   *   The entity type associated with this property type.
   * @param string $fieldType
   *   The field type associated with this property type.
   * @param string $key
   *   The key associated with this property type.
   * @param string $term_id
   *   The controlled vocabulary term asssociated with this property. It must be
   *   in the form of "IdSpace:Accession" (e.g. "rdfs:label" or "OBI:0100026")
   * @param array $storage_settings
   *   The settings used to compute the value of this property. Specifically,
   *   the supported keys include:
   *   - callback: the static method that will be called to compute the value.
   *     This method should be in the form of "ClassName::methodName" and
   *     should return the computed value.
   * @param string $idspace_plugin_id
   *   The plugin_id associated with the term. This is optional but if provided
   *   allows a missing ID Space to be looked up in the backend storage.
   */
  public function __construct(string $entityType, string $fieldType, string $key, string $term_id, array $storage_settings = [], string $idspace_plugin_id = '') {
    parent::__construct($entityType, $fieldType, $key, $term_id, "computed", $storage_settings, $idspace_plugin_id);
  }

  /**
   * Returns the default empty value of the correct type for this property.
   *
   * @return string
   *   An empty string.
   */
  public function getDefaultValue() {
    return '';
  }

}
