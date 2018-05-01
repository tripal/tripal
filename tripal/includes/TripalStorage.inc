<?php

class TripalStorage {

  // An associative array containing information about this storage backend.
  // The valid keys are those specified in the hook_field_storage_info().
  static $information = [
    'label' => '',
    'description' => '',
    'settings' => [],
  ];

  /**
   * Provides default information about this storage type.
   *
   * This function corresponds to the hook_field_storage_info() function of
   * the Drupal Storage API.
   *
   * @return
   *   An array whose keys the same as for the
   *   hook_field_storage_info() function.
   */
  public function info() {
    $class = get_called_class();
    return [
      $class => $class::$information,
    ];
  }

  /**
   * Loads the fields that are supported by the storage backend.
   *
   * This function should behave just as the hook_field_storage_load() function.
   * See the documentation for that hook for example implementation.
   *
   * @param $entity_type
   *   The type of entity (typically a bundle of TripalEntity).
   * @param unknown $entities
   *   The array of entity objects to add fields to, keyed by entity ID.
   * @param unknown $age
   *   FIELD_LOAD_CURRENT to load the most recent revision for all fields, or
   *   FIELD_LOAD_REVISION to load the version indicated by each entity.
   * @param unknown $fields
   *   An array listing the fields to be loaded. The keys of the array are field
   *   IDs, and the values of the array are the entity IDs (or revision IDs,
   *   depending on the $age parameter) to add each field to.
   * @param unknown $options
   *   An associative array of additional options, with the following keys:
   *    - deleted: If TRUE, deleted fields should be loaded as well as
   *      non-deleted fields. If unset or FALSE, only non-deleted fields
   *      should be loaded.
   */
  public function load($entity_type, $entities, $age,
                       $fields, $options) {

  }

  /**
   * Searches for entities that match field conditions.
   *
   * This function is similar to the hook_field_storage_query(). Each
   * TripalStorage class is responsible for querying it's own storage
   * system using the conditions provided in the $query argument.
   *
   * @param $query
   *   A TripalFieldQuery object.
   *
   * @return
   *   See EntityFieldQuery::execute() for the return values.
   */
  public function query($query) {

  }
}