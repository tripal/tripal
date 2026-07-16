<?php

namespace Drupal\Tests\tripal\Kernel\Entity;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\tripal\Entity\TripalEntityType;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests TripalEntityType::sort().
 * 
 * This function is used to order Tripal Content Types in
 * both TripalEntityTypeListBuilder and TripalEntityUIController.
 *
 * @group TripalEntityType
 */
#[Group('tripal-content')]
class TripalEntityTypeSortTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'user', 'tripal'];

  /**
   * Creates an unsaved TripalEntityType with the given id/label/category.
   *
   * The entities are intentionally not saved since TripalEntityType::sort()
   * only relies on getCategory() and label(), which are both usable on
   * in-memory entities.
   */
  protected function createType(string $id, string $label, string $category): TripalEntityType {
    return TripalEntityType::create([
      'id' => $id,
      'label' => $label,
      'category' => $category,
    ]);
  }

  /**
   * Categories should be sorted alphabetically, with "General" always first.
   *
   * This is a regression test: the comparator previously had its arguments
   * swapped (`strnatcasecmp($b_value, $a_value)`), which sorted all
   * non-"General" categories in reverse-alphabetical order.
   */
  public function testCategoriesAreSortedAlphabeticallyWithGeneralFirst() {

    $types = [
      $this->createType('t_germplasm', 'Germplasm Type', 'Germplasm'),
      $this->createType('t_genomic', 'Genomic Type', 'Genomic'),
      $this->createType('t_genetic', 'Genetic Type', 'Genetic'),
      $this->createType('t_expression', 'Expression Type', 'Expression'),
      $this->createType('t_general', 'General Type', 'General'),
    ];

    usort($types, [TripalEntityType::class, 'sort']);

    $ordered_categories = array_map(fn($type) => $type->getCategory(), $types);

    $this->assertEquals(
      ['General', 'Expression', 'Genetic', 'Genomic', 'Germplasm'],
      $ordered_categories,
      'Categories should be sorted with "General" first and all other categories in ascending alphabetical order.'
    );
  }

  /**
   * Within the same category, types should be sorted alphabetically by label.
   */
  public function testTypesWithinCategoryAreSortedByLabel() {

    $types = [
      $this->createType('t_orange', 'Orange', 'Genetic'),
      $this->createType('t_monkey', 'Monkey', 'Genetic'),
      $this->createType('t_eagle', 'Eagle', 'Genetic'),
    ];

    usort($types, [TripalEntityType::class, 'sort']);

    $ordered_labels = array_map(fn($type) => $type->label(), $types);

    $this->assertEquals(
      ['Eagle', 'Monkey', 'Orange'],
      $ordered_labels,
      'Types within the same category should be sorted alphabetically by label.'
    );
  }

  /**
   * Multiple "General" types should be sorted by label among themselves.
   */
  public function testMultipleGeneralTypesAreSortedByLabel() {

    $types = [
      $this->createType('t_general_b', 'Bravo', 'General'),
      $this->createType('t_general_a', 'Alpha', 'General'),
      $this->createType('t_other_d', 'Delta', 'Other'),
    ];

    usort($types, [TripalEntityType::class, 'sort']);

    $ordered_labels = array_map(fn($type) => $type->label(), $types);

    $this->assertEquals(
      ['Alpha', 'Bravo', 'Delta'],
      $ordered_labels,
      '"General" types should sort before all other categories, and be sorted by label among themselves.'
    );
  }

}
