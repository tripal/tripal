<?php

namespace Drupal\tripal\Services;

use Drupal\Core\Entity\EntityTypeManagerInterface;

/**
 * Resolve discrepancies between the Drupal field tables + Tripal Field schema.
 */
class SyncTripalFieldStorage {

  /**
   * The entity type manager.
   *
   * @var Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected EntityTypeManagerInterface $entityTypeManager;

  /**
   * Constructs a SyncTripalFieldStorage object.
   */
  public function __construct(EntityTypeManagerInterface $entityTypeManager) {
    $this->entityTypeManager = $entityTypeManager;
  }

  /**
   * Identify discrepancies between drupal field tables and tripal field schema.
   *
   * @param string $entity_type
   *   The id of the entity type whose associated fields we want to check.
   *   If this parameter is ommitted then all TripalEntityType instances
   *   will be checked.
   * @param array $difference_types
   *   A list of the types of differences to detect. If ommitted then all
   *   supported types of differences will be checked.
   *   The supported types include:
   *    - missing_property_column: where a column for a new TripalField property
   *      does not have a column in the corresponding Drupal field table.
   *
   * @return array
   */
  public function detectDifferences(string|null $entity_type = NULL, array $difference_types = []): array {
    $all_columns_added = [];

    if ($entity_type !== NULL) {
      $type = \Drupal::entityTypeManager()
        ->getStorage('tripal_entity_type')
        ->load($entity_type);
      if (is_object($type)) {
        $types = [$entity_type => $type];
      }
      else {
        throw new \Exception("We were unable to retrieve the TripalEntityType $entity_type when trying to detect differences in the field schema.");
      }
    }
    else {
      $types = \Drupal::entityTypeManager()
      ->getStorage('tripal_entity_type')
      ->loadMultiple();
    }

    foreach ($types as $bundle_name => $type_object) {
      $columns_added = $type_object->syncStorageSchema(TRUE);
      if (is_array($columns_added)) {
        $all_columns_added = $all_columns_added + $columns_added;
      }
    }

    return $all_columns_added;
  }

  /**
   * Resolve already detected differences.
   *
   * @param array $differences
   *   An array of the form returned by detectDifferences() where you wish all
   *   the included differences to be fixed.
   *
   * @return void
   */
  public function resolveDetectedDifferences(array $differences): void {

    // Get the drupal database schema object.
    $schema = \Drupal::service('database')->schema();

    foreach ($differences as $field_name => $field_differences) {
      foreach ($field_differences as $property_name => $property_difference) {
        $column_exists = $schema->fieldExists($property_difference['drupal_table'], $property_difference['column_name']);
        if ($column_exists === FALSE) {
          $schema->addField(
            $property_difference['drupal_table'],
            $property_difference['column_name'],
            $property_difference['column_spec']
          );
        }
      }
    }
  }

  /**
   * Identify + resolve differences between drupal tables + tripal field schema.
   *
   * @param string $entity_type
   *   The id of the entity type whose associated fields we want to fix.
   *   If this parameter is ommitted then all TripalEntityType instances
   *   will be fixed.
   * @param array $difference_types
   *   A list of the types of differences to detect. If ommitted then all
   *   supported types of differences will be fixed.
   *   The supported types include:
   *    - missing_property_column: where a column for a new TripalField property
   *      does not have a column in the corresponding Drupal field table.
   */
  public function resolveDifferences(string $entity_type = NULL, array $difference_types = []) {
    // @todo Place your code here.
  }

}
