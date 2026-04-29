<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Entity\EntityTypeManagerInterface;

/**
 * This service quieries the database to lookup the values for a specific field.
 */
class TripalFieldValueLookup {

  /**
   * The Drupal entity type manager service.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected EntityTypeManagerInterface $entityTypeManager;

  /**
   * The entity type ID for which to lookup field values.
   *
   * @var string
   */
  protected string $entity_type_id;

  /**
   * Constructs a new TripalFieldValueLookup service.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entityTypeManager
   *   The entityTypeManager.
   */
  public function __construct(EntityTypeManagerInterface $entityTypeManager) {
    $this->entityTypeManager = $entityTypeManager;
    // Default to 'tripal_entity' but this can be set to any entity type.
    $this->entity_type_id = 'tripal_entity';
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
  public function getUniqueFieldValues(string $field_name, array $filters, array $options): array {
    $definition = $this->entityTypeManager->getDefinition($this->entity_type_id);

    $bundle_key = $definition->getKey('bundle');

    $query = $this->entityTypeManager
      ->getStorage($this->entity_type_id)
      ->getAggregateQuery();

    $query->accessCheck(FALSE);

    // Group by the field's value column.
    $query->groupBy("$field_name.value");

    $bundles = $filters['bundles'] ?? [];

    // Optional: restrict to bundles.
    if (!empty($bundles) && $bundle_key) {
      $query->condition($bundle_key, $bundles, 'IN');
    }

    // Optional: remove null/empty values.
    if (($filters['remove_null'] ?? TRUE)) {
      $query->condition("$field_name.value", NULL, 'IS NOT NULL');
    }
    if (($filters['remove_empty'] ?? TRUE)) {
      $query->condition("$field_name.value", '', '<>');
    }

    return $query->execute();
  }

}
