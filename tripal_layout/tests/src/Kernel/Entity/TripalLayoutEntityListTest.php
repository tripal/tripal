<?php

namespace Drupal\Tests\tripal_layout\Kernel\Entity;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal_layout\Traits\TripalLayoutTestTrait;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultView;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultForm;

/**
 * Tests the TripalLayoutDefaultView and TripalLayoutDefaultForm entities.
 *
 * @group TripalLayoutDisplay
 * @group TripalLayoutDisplayEntity
 */
class TripalLayoutEntityListTest extends TripalTestKernelBase {

  use TripalLayoutTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'tripal', 'tripal_layout'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    $this->installEntitySchema('user');
    $this->installEntitySchema('tripal_layout_default_form');
    $this->installEntitySchema('tripal_layout_default_view');
  }

  public function provideConfigEntities() {
    $scenarios = [];

    $scenarios['basic_view'] = [
      'listbuilder_class' => '\Drupal\tripal_layout\ListBuilders\TripalLayoutDefaultViewListBuilder',
      'config_entity_type' => 'tripal_layout_default_view',
      'yaml_file' => __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_view.test_view.yml',
    ];

    $scenarios['basic_form'] = [
      'listbuilder_class' => '\Drupal\tripal_layout\ListBuilders\TripalLayoutDefaultFormListBuilder',
      'config_entity_type' => 'tripal_layout_default_form',
      'yaml_file' => __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_form.test_form.yml',
    ];

    return $scenarios;
  }

  /**
   * Tests the listbuilder for the TripalLayoutDefaultView and
   * TripalLayoutDefaultForm entities.
   *
   * @dataProvider provideConfigEntities
   *
   * @return void
   */
  public function testListBuilder(string $listbuilder_class, string $config_entity_type, string $yaml_file) {

    // Create layout entity to be used in testing.
    $config_entity = $this->createLayoutEntityFromConfig(
      $config_entity_type,
      $yaml_file
    );
    $config_entity_id = $config_entity->id();

    // Get the entity_type and it's storage.
    /** @var \Drupal\Core\Entity\EntityTypeInterface $definition **/
    $definition = \Drupal::entityTypeManager()->getDefinition($config_entity_type);
    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_storage **/
    $config_storage = \Drupal::entityTypeManager()->getStorage($config_entity_type);

    // Get the list builder object.
    $listbuilder_object = new $listbuilder_class($definition, $config_storage);
    $this->assertInstanceOf($listbuilder_class, $listbuilder_object, "We were unable to initialize the listbuilder object for $config_entity_type.");

    // Now render the listbuilder page.
    $page_render = $listbuilder_object->render();

    // Check our test entity is in the table rows.
    $this->assertArrayHasKey('table', $page_render, "The listbuilder render() should have produced a table but it's not in the output.");
    $this->assertArrayHasKey('#rows', $page_render['table'], "The listbuilder render() table should have rows but they are not defined.");
    $this->assertCount(1, $page_render['table']['#rows'], "There was not the expected number of rows in the listbuilder table.");
    $this->assertArrayHasKey($config_entity_id, $page_render['table']['#rows'], "The listbuilder rows should be keyed by the config entity ids but our test entity is not there.");
  }
}
