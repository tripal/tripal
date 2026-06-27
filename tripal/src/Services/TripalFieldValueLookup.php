<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Entity\EntityFieldManagerInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;

/**
 * This service queries the database to lookup the values for a specific field.
 */
class TripalFieldValueLookup {

  /**
   * The Drupal entity type manager service.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected EntityTypeManagerInterface $entityTypeManager;

  /**
   * The Drupal entity field manager service.
   *
   * @var \Drupal\Core\Entity\EntityFieldManagerInterface
   */
  protected EntityFieldManagerInterface $entityFieldManager;

  /**
   * The Drupal cache backend interface.
   *
   * @var \Drupal\Core\Cache\CacheBackendInterface
   */
  protected CacheBackendInterface $cacheBackend;

  /**
   * The entity type ID.
   */
  protected string $entity_type_id = 'tripal_entity';

  /**
   * Constructs a new TripalFieldValueLookup service.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entityTypeManager
   *   The Drupal entity type manager service.
   * @param \Drupal\Core\Entity\EntityFieldManagerInterface $entityFieldManager
   *   The Drupal entity field manager service.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   Cache backend instance to use.
   */
  public function __construct(
    EntityTypeManagerInterface $entityTypeManager,
    EntityFieldManagerInterface $entityFieldManager,
    CacheBackendInterface $cache_backend,
  ) {
    $this->entityTypeManager = $entityTypeManager;
    $this->entityFieldManager = $entityFieldManager;
    $this->cacheBackend = $cache_backend;
  }

  /**
   * Sets the entity type ID for which to lookup field values.
   *
   * @param string $entity_type_id
   *   The entity type ID to set.
   */
  public function setEntityTypeId(string $entity_type_id): void {
    $this->entity_type_id = $entity_type_id;
  }

  /**
   * Gets the entity type ID for which to lookup field values.
   *
   * @return string
   *   The current entity type ID.
   */
  public function getEntityTypeId(): string {
    return $this->entity_type_id;
  }

  /**
   * Retrieves the current set of values for this field.
   *
   * @param string $field_name
   *   The machine name of the field you are interested in.
   * @param string $property
   *   The field property for which to retrieve unique values (e.g. value,
   *   target_id, etc.).
   * @param array $filters
   *   Optional filters restricting the values returned.
   *   Supported keys include:
   *   - remove_null (bool; default TRUE) ensures that NULL is not included
   *     in the result set.
   *   - remove_empty (bool; default TRUE) ensures that empty string
   *     is also removed from the result set.
   *   - bundles (array; default []) ensures only values for that
   *     field within the bundles specified are included. By default
   *     values from all bundles are included.
   * @param array $options
   *   Additional options where supported keys include:
   *   - validate_field (bool; default TRUE) confirms that the field name
   *     passed in is valid, the field exists for the entity type.
   *   - refresh_cache (bool; default FALSE) allows you to indicate whether
   *     you want to use the cache (FALSE) or want to generate the values
   *     from a fresh query (TRUE).
   *
   * @return array
   *   Either returns an empty array if there are no values for this
   *   set or returns an array where the key and value both match the unique
   *   set of field values.
   */
  public function getUniqueFieldValues(
    string $field_name,
    string $property = 'value',
    array $filters = [],
    array $options = [],
  ): array {

    if (empty($field_name)) {
      return [];
    }

    $validate = $options['validate_field'] ?? TRUE;
    $refresh = $options['refresh_cache'] ?? FALSE;

    $bundles = $filters['bundles'] ?? [];

    // Generate cache IDs.
    $data_cid = $this->generateDataCacheId($field_name, $bundles);
    $validate_cid = $this->generateValidateCacheId($field_name);

    // Return cached data if allowed.
    if (!$refresh) {
      $cached = $this->retrieveFromCache($data_cid);
      if ($cached !== NULL) {
        return $cached;
      }
    }

    // Validate field name.
    if ($validate) {
      $is_valid = $this->retrieveFromCache($validate_cid);

      if ($is_valid === NULL) {
        $is_valid = $this->validateField($field_name);
        $this->setCache($validate_cid, $is_valid);
      }

      if (!$is_valid) {
        throw new \InvalidArgumentException("Invalid field name: {$field_name}");
      }
    }

    // Build query.
    $definition = $this->entityTypeManager->getDefinition($this->entity_type_id);
    $bundle_key = $definition->getKey('bundle');

    $query = $this->entityTypeManager
      ->getStorage($this->entity_type_id)
      ->getAggregateQuery();

    $query->accessCheck(FALSE);
    $query->groupBy("$field_name.$property");

    if (!empty($bundles) && $bundle_key) {
      $query->condition($bundle_key, $bundles, 'IN');
    }

    if ($filters['remove_null'] ?? TRUE) {
      $query->condition("$field_name.$property", NULL, 'IS NOT NULL');
    }

    if ($filters['remove_empty'] ?? TRUE) {
      $query->condition("$field_name.$property", '', '<>');
    }

    $results = $query->execute();

    // Cache final result.
    $this->setCache($data_cid, $results);

    return $results;
  }

  /**
   * Builds the data cache ID.
   *
   * @param string $field_name
   *   The machine name of the field for which to generate the cache ID.
   * @param array $bundles
   *   The bundles for which to generate the cache ID.
   *
   * @return string
   *   The generated cache ID.
   */
  protected function generateDataCacheId(string $field_name, array $bundles = []): string {
    $bundles_part = !empty($bundles) ? implode('_', $bundles) : 'all';

    return "data:{$this->entity_type_id}_{$bundles_part}_{$field_name}";
  }

  /**
   * Builds the validation cache ID.
   *
   * @param string $field_name
   *   The machine name of the field for which to generate the cache ID.
   *
   * @return string
   *   The generated cache ID.
   */
  protected function generateValidateCacheId(string $field_name): string {
    return "valid_field:{$this->entity_type_id}_{$field_name}";
  }

  /**
   * Retrieves data from cache.
   *
   * @param string $cache_id
   *   The cache ID to retrieve.
   *
   * @return mixed
   *   The cached data or NULL if not found.
   */
  protected function retrieveFromCache(string $cache_id): mixed {
    $item = $this->cacheBackend->get($cache_id);
    return $item ? $item->data : NULL;
  }

  /**
   * Stores data in cache.
   *
   * @param string $cache_id
   *   The cache ID under which to store the data.
   * @param mixed $data
   *   The data to store in cache.
   */
  protected function setCache(string $cache_id, mixed $data): void {
    $this->cacheBackend->set($cache_id, $data);
  }

  /**
   * Validates that a field exists for the entity type.
   *
   * @param string $field_name
   *   The machine name of the field to validate.
   *
   * @return bool
   *   TRUE if the field is valid, FALSE otherwise.
   */
  protected function validateField(string $field_name): bool {
    $definitions = $this->entityFieldManager
      ->getFieldStorageDefinitions($this->entity_type_id);

    return isset($definitions[$field_name]);
  }

}
