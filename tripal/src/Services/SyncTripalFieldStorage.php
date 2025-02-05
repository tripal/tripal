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
   * @return void
   */
  public function detectDifferences(string $entity_type = NULL, array $difference_types = []): void {
    // @todo Place your code here.
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
    // @todo Place your code here.
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
