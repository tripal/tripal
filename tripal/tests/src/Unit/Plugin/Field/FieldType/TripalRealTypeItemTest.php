<?php

namespace Drupal\Tests\tripal\Unit\Plugin\Field\FieldType;

use Drupal\Core\DependencyInjection\ContainerBuilder;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Plugin\Field\FieldType\TripalRealTypeItem;
use Drupal\tripal\TripalStorage\RealStoragePropertyType;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;
use Drupal\tripal\TripalVocabTerms\PluginManagers\TripalCollectionPluginManager;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\Group;
use Prophecy\Argument;

/**
 * Tests TripalRealTypeItem.
 *
 * @coversDefaultClass \Drupal\tripal\Plugin\Field\FieldType\TripalRealTypeItem
 *
 * @group tripal-field
 */
#[CoversClass(TripalRealTypeItem::class)]
#[Group('tripal-field')]
class TripalRealTypeItemTest extends UnitTestCase {

  /**
   * Test that defaultFieldSettings() returns an array and merges parent.
   */
  public function testDefaultFieldSettings(): void {
    $settings = TripalRealTypeItem::defaultFieldSettings();
    $this->assertIsArray($settings);
    // Test for the known keys.
    $this->assertArrayHasKey('termIdSpace', $settings);
    $this->assertArrayHasKey('termAccession', $settings);
    $this->assertArrayHasKey('debug', $settings);
  }

  /**
   * Test that defaultStorageSettings() sets storage_plugin_id.
   *
   * It should be set to drupal_sql_storage.
   */
  public function testDefaultStorageSettings(): void {
    $settings = TripalRealTypeItem::defaultStorageSettings();
    $this->assertIsArray($settings);
    $this->assertSame('drupal_sql_storage', $settings['storage_plugin_id']);
    // Keys inherited from TripalFieldItemBase.
    $this->assertArrayHasKey('termIdSpace', $settings);
    $this->assertArrayHasKey('termAccession', $settings);
    $this->assertArrayHasKey('storage_plugin_settings', $settings);
  }

  /**
   * Tests that tripalTypes() returns a RealStoragePropertyType.
   *
   * This should have been set with the term from settings.
   */
  public function testTripalTypesWithExplicitTerm(): void {
    $this->setUpIdspaceContainer();

    $field_def = $this->createMock(FieldDefinitionInterface::class);
    $field_def->method('getTargetEntityTypeId')->willReturn('tripal_entity');
    $field_def->method('getSettings')->willReturn([
      'termIdSpace' => 'OBO',
      'termAccession' => '00001',
    ]);

#    $idsmanager = \Drupal::service('tripal.collection_plugin_manager.idspace');
#    $x = $idsmanager->createCollection('OBO', 'tripal_default_id_space');
#var_dump($x);
#
    $types = TripalRealTypeItem::tripalTypes($field_def);

    $this->assertCount(1, $types);
    $this->assertInstanceOf(RealStoragePropertyType::class, $types[0]);
    $this->assertSame('OBO', $types[0]->getTermIdSpace());
    $this->assertSame('00001', $types[0]->getTermAccession());
    $this->assertSame('tripal_real_type', $types[0]->getFieldType());
    $this->assertSame('value', $types[0]->getKey());
  }

  /**
   * Tests that tripalTypes() falls back to NCIT:C25712 for empty termIdSpace.
   */
  public function testTripalTypesFallsBackToDefaultTerm(): void {
    $this->setUpIdspaceContainer();

    $field_def = $this->createMock(FieldDefinitionInterface::class);
    $field_def->method('getTargetEntityTypeId')->willReturn('tripal_entity');
    $field_def->method('getSettings')->willReturn([
      'termIdSpace' => '',
      'termAccession' => '',
    ]);

    $types = TripalRealTypeItem::tripalTypes($field_def);

    $this->assertCount(1, $types);
    $this->assertInstanceOf(RealStoragePropertyType::class, $types[0]);
    $this->assertSame('NCIT', $types[0]->getTermIdSpace());
    $this->assertSame('C25712', $types[0]->getTermAccession());
  }

  /**
   * Sets up a minimal Drupal container with a stub idspace service.
   *
   * StoragePropertyBase::__construct() resolves
   * tripal.collection_plugin_manager.idspace from the container to validate
   * the term string. This stubs out that chain so RealStoragePropertyType
   * can be instantiated in a unit test.
   */
  private function setUpIdspaceContainer(): void {
    $idspace = $this->prophesize(TripalIdSpaceInterface::class);
    $idspace->getTerm(Argument::any())->willReturn(new \stdClass());

    $idspace_manager = $this->prophesize(TripalCollectionPluginManager::class);
    $idspace_manager->loadCollection(Argument::any())->willReturn($idspace->reveal());
    $idspace_manager->loadCollection(Argument::any(), Argument::any())->willReturn($idspace->reveal());

    $container = new ContainerBuilder();
    $container->set('tripal.collection_plugin_manager.idspace', $idspace_manager->reveal());
    \Drupal::setContainer($container);
  }

}
